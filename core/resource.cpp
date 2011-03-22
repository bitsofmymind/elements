/*
 * resource.cpp
 *
 *  Created on: Feb 17, 2009
 *      Author: Antoine
 */

#include "message.h"
#include "response.h"
#include "request.h"
#include "resource.h"
#include "../elements.h"
#include "url.h"
#include "../utils/utils.h"
#include "../utils/types.h"
#include "../pal/pal.h"
#include <stdint.h>
#include <stdlib.h>

using namespace Elements;

string<uint8_t> parent_resource = MAKE_STRING("..");

Resource::Resource(void):
  children_sleep_clock(MAX_UPTIME),
  own_sleep_clock(MAX_UPTIME)
{
	child_to_visit = 0;
	children = 0;
	parent = 0;
	//register_element();
}

#ifndef NO_RESOURCE_DESTRUCTION
	Resource::~Resource(void)
	{
		//unregister_element();
		delete children;
	}
#endif

void Resource::visit(void)
{
	if(own_sleep_clock <= get_uptime() )
	{
		own_sleep_clock = MAX_UPTIME;
		run();
	}
}

Message* Resource::dispatch( Message* message )
{

	const char* name;

	while(message->to_url->cursor < message->to_url->resources.items)
	{
		name = message->to_url->resources[message->to_url->cursor];

		if(name[0] == '.')
		{
			if(name[1] == '.' && name[2] == '\0' )
			{
				message->to_url->cursor++;
				if(parent)
				{
					return parent->dispatch(message);
				}
				return error(404, message);
			}
			if(name[1] == '\0')
			{
				message->to_url->cursor++;
				continue;
			}
		}
		break;

	}


	Response::status_code sc;
	Message* return_message = NULL;

	if(message->object_type == Message::REQUEST)
	{

		sc = process((Request*)message, &return_message);
	}
	else
	{
		sc = process((Response*)message, &return_message);
	}


	switch(sc)
	{
		case OK_200:
			break;
		case PASS_308:
			if(children)
			{
				if(message->to_url->cursor < message->to_url->resources.items)
				{
					Resource* next = children->find( message->to_url->resources[message->to_url->cursor++] );
					if(next)
					{
						return next->dispatch(message);
					}
				}
			}
			sc = NOT_FOUND_404;
			return_message = NULL;
			//No break here
		default:
			if(!return_message)
			{
				//PROBLEM! = Message is not necessarily a Request
				return_message = error(sc, (Request*)message);
			}
	}


	return return_message;
}

uint8_t Resource::send(Message* message)
{
    if(parent)
    {
        const char* name = parent->get_name(this);
        char* new_name = (char*)ts_malloc(strlen(name));

        strcpy(new_name, name);

        if(!message->to_url->is_absolute_path)
        {
            message->to_url->resources.insert((const char *)new_name,0);
        }
        message->from_url->resources.insert((const char *)new_name,0);
        return parent->send(message);
    }
    return 1;
}


const char* Resource::get_name(Resource* resource)
{
    if(children)
    {
        return children->find_val(resource);
    }
    return NULL;
}

int8_t Resource::add_child(const char* name, Resource* child )
{
	if( children == NULL )
	{
		children = new Dictionary<Resource>();
		if(!children)
		{
			return -2;
		}
		//children_sleep_clock = 0;
	}

	int8_t return_code = children->add(name, child);
	/*The parent variable is set after we get confirmation the child was successfully added
	 * to the dictionary because doing otherwise could give upward access to a resource
	 * that is not a child of any resource. */
	if(!return_code)
	{
		child->parent = this;
		if(child->get_sleep_clock() < children_sleep_clock)
		{
			schedule(&children_sleep_clock, child->get_sleep_clock());
		}
	}
	return return_code;
}
uint8_t Resource::get_number_of_children()
{
	return !children ? 0 : children->items;
}

Resource* Resource::remove_child(const char* name)
{
	Resource* orphan = !children ? NULL: (Resource*)children->remove(name);
	orphan->parent = NULL;
	return orphan;
}

void Resource::print_transaction(Message* message)
{
	/*If VERBOSITYis undefined, this method should be optimizes away by the compiler.*/

	VERBOSE_PRINT("from: ");
	message->from_url->print();
	VERBOSE_PRINTLN();
	VERBOSE_PRINT("to: ");
	message->to_url->print();
	VERBOSE_PRINTLN();
	message->print();
	VERBOSE_PRINTLN();
}

Response::status_code Resource::process( Request* request, Message** return_message )
{

	if(request->to_url->cursor >=  request->to_url->resources.items)
	{
		print_transaction(request);
#if HTTP_GET
		if(request->methodcmp("get", 3))
		{
			 *return_message = http_get( request );
		}
#endif
#if HTTP_HEAD
		else if(request->methodcmp("head", 4))
		{
			*return_message = http_head( request );
		}
#endif
#if HTTP_TRACE
		else if(request->methodcmp("trace", 5))
		{
			*return_message = http_trace( request );
		}
#endif
		else
		{
			return NOT_IMPLEMENTED_501;
		}

		return ((Response*)(*return_message))->response_code_int;
	}

	return PASS_308;

}
#if HTTP_GET
Response* Resource::http_get(Request* request)
{
	Response* response =  new Response(OK_200, request );
	if(!response)
	{
		return NULL;
	}
	response->body_file = render( request );
	response->content_type = &Message::TEXT_HTML;
	if(response->body_file)
	{
		response->content_length = response->body_file->size;
	}
	else
	{
		response->content_length = 0;
		ERROR_PRINTLN("Fail to render body");
	}
	return response;
}
#endif
#if HTTP_HEAD
Response* Resource::http_head(Request* request)
{
	Response* response =  new Response(OK_200, request );
	if(!response)
	{
		return NULL;
	}
	//response->body = render( request );
	response->content_type = &Message::TEXT_HTML;
	return response;
}
#endif
#if HTTP_TRACE
Response* Resource::http_trace( Request* request )
{
	Response* response = new Response(OK_200, request );
	//request->Message::serialize();
	//response->body = request->message;
	response->content_type = &Message::MESSAGE_HTTP;
	return response;
}
#endif

Response::status_code Resource::process(Response* response, Message** return_message)
{
	if(response->to_url->cursor >=  response->to_url->resources.items)
	{
		print_transaction(response);
		delete response;
		return OK_200;
	}
	return PASS_308;
}


void Resource::run( void )
{
	schedule( &own_sleep_clock, MAX_UPTIME );
}


uptime_t Resource::get_sleep_clock( void )
{
	return children_sleep_clock > own_sleep_clock ? own_sleep_clock:children_sleep_clock;
}

void Resource::schedule( volatile uptime_t* timer, uptime_t time )
{

    if(time != MAX_UPTIME)
    {
    	time += get_uptime();
    }

    if(timer)
    {
    	*timer = time;
    }

	Resource* current = parent;
	while(true)
	{
		if (!current )
		{
			break;
		}
		if(current->children_sleep_clock > time)
		{
			current->children_sleep_clock = time;
		}
		else
		{
			break;
		}
		current = current->parent;

	}
        processing_wake();

}

void Resource::schedule(uptime_t time)
{
	schedule(&own_sleep_clock, time);
}

File<MESSAGE_SIZE>* Resource::render( Request* request )
{
	return new ConstFile<MESSAGE_SIZE>("o");//<html><body>There are currently no representation associated with this resource.</body></html>");
}

Resource* Resource::get_next_child_to_visit(void)
{
	if(children && children_sleep_clock <= get_uptime() )
	{

		while(true)
		{
			if(child_to_visit == children->items)
			{
				child_to_visit = 0;

				children_sleep_clock = MAX_UPTIME;

				for(uint8_t i = 0; i < children->items; i++)
				{
					uptime_t sleep_clock = (*children)[i]->value->get_sleep_clock();
					if(sleep_clock < children_sleep_clock)
					{
						children_sleep_clock = sleep_clock;
					}
				}

				break;
			}
			Resource* child = (*children)[child_to_visit]->value;
			child_to_visit++;
			if(child->get_sleep_clock() <= get_uptime())
			{
				return child;
			}
		}
	}

	return NULL;
}

Response* Resource::error(uint16_t error, Message* message)
{

	if( message->object_type == Message::REQUEST )
	{

		Response* response = new Response(error, (Request*)message);
		if(!response)
		{
			//Memory could not be allocated for the response
			delete message;
			return NULL;
		}
		switch(error)
		{
			case NOT_FOUND_404:
				//response->body = string<MESSAGE_SIZE>::make("?");//"<html><body>Not found</body></html>");
				response->body_file = new ConstFile<MESSAGE_SIZE>("?");
				break;
			default:
				break;
		}
		return response;
	}
	return NULL;
}
