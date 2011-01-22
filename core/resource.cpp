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
  buffer_children_sleep_clock(MAX_UPTIME)
{
	own_sleep_clock = 0;
	child_to_visit = 0;
	visiting_children = false;
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

Resource* Resource::find_resource( URL* url )
{
	string<uint8_t>* name = url->resources[url->cursor];

	if(name->text[0] == '.')
	{
		if(name->text[1] == '.' && name->length == 2 )
		{
			return parent;
		}
		if(name->length == 1)
		{
			return this;
		}
	}

	if(!children)
	{
		return NULL;
	}

	return (Resource*)children->find( *name );
}

Message* Resource::dispatch( Message* message)
{
	/*string<uint8_t>* resource_name = message->to_url->resources[message->to_url->cursor];
	message->to_url->cursor++;
	if( resource_name == NULL )
	{
		if( message->object_type == Message::REQUEST)
		{
			return process( (Request*) message );
		}
		return process( (Response*)message);
	}
	if( resource_name->text[0] == '.' && resource_name->text[1] == '.' && resource_name->length == 2 )
	{
		if( !message->to_url->is_absolute_path)
		{
			message->from_url->resources.append(message->to_url->resources[message->to_url->cursor - 2 ]);
		}
		return message;
	}
	if( resource_name->text[0] == '.' && resource_name->length == 1 )
	{
		return dispatch(message); //RISK OF STACK OVERFLOW IF THERE IS TOO MUCH /./././././././././.
	}*/


	/*f( !message->to_url->is_absolute_path)
	{
		message->from_url->resources.append(&parent_resource);
	}
	Resource* child_resource = find_resource( resource_name );*/

	if(message->to_url->cursor >= message->to_url->resources.items)
	{
		if( message->object_type == Message::REQUEST)
		{
			return process( (Request*) message );
		}
		return process( (Response*)message);
	}

	Resource* next = find_resource( message->to_url);
	message->to_url->cursor++;

	if(next == this)
	{
		return dispatch(message);
	}
	if(next == NULL )
	{
		return error(NOT_FOUND_404, message);
	}
	else
	{
		message = next->dispatch(message);
	}
}

uint8_t Resource::send(Message* message)
{
    if(parent)
    {
        string<uint8_t>* name = parent->get_name(this);
        string<uint8_t>* new_name = (string<uint8_t>*)ts_malloc(sizeof(string<uint8_t>));

        *new_name = *name;

        if(!message->to_url->is_absolute_path)
        {
            message->to_url->resources.insert(new_name,0);
        }
        message->from_url->resources.insert(new_name,0);
        return parent->send(message);
    }
    return 1;
}


string<uint8_t>* Resource::get_name(Resource* resource)
{
    if(children)
    {
        return children->find(resource);
    }
    return NULL;
}

int8_t Resource::add_child(string<uint8_t> name, Resource* child )
{
	if( children == NULL )
	{
		children = new Dictionary<Resource>();
		if(!children)
		{
			return -2;
		}
		children_sleep_clock = 0;
	}

	int8_t return_code = children->add(name, child);
	/*The parent variable is set after we get confirmation the child was successfully added
	 * to the dictionary because doing otherwise could give upward access to a resource
	 * that is not a child of any resource. */
	if(!return_code)
	{
		child->parent = this;
	}
	return return_code;
}
uint8_t Resource::get_number_of_children()
{
	return !children ? 0 : children->items;
}
Resource* Resource::remove_child(string<uint8_t> name)
{
	Resource* orphan = !children ? NULL: (Resource*)children->remove(name);
	orphan->parent = NULL;
	return orphan;
}

/*string<uint8_t> get_method = MAKE_STRING("get");
string<uint8_t> head_method = MAKE_STRING("head");
string<uint8_t> trace_method = MAKE_STRING("trace");*/

Response* Resource::process( Request* request )
{
	#ifdef DEBUG
		print_transaction(request);
	#endif

	//if( request->method == get_method)
	if(!memcmp("get", request->method.text, 3))
	{
		return http_get( request );
	}
	//else if( request->method == head_method )
	else if(!memcmp("head", request->method.text, 4))
	{
		return http_head( request );
	}
	//else if( request->method == trace_method )
	else if(!memcmp("trace", request->method.text, 5))
	{
		return http_trace( request );
	}

	return error(NOT_IMPLEMENTED_501, request);
}

Message* Resource::process(Response* response)
{
	#ifdef DEBUG
		print_transaction(response);
	#endif
	return NULL;
}

#ifdef DEBUG
	void Resource::print_transaction(Message* message)
	{
		Debug::println("# Received: ");
		Debug::print("from: ");
		message->from_url->print();
		Debug::println();
		Debug::print("to: ");
		message->to_url->print();
		Debug::println();
		message->print();
		Debug::println();
	}

#endif

void Resource::run( void )
{
	schedule( &own_sleep_clock, MAX_UPTIME );
}


uptime_t Resource::get_sleep_clock( void )
{
	return children_sleep_clock > own_sleep_clock ? own_sleep_clock:children_sleep_clock;
}

Response* Resource::http_get(Request* request)
{
	Response* response =  http_head(request);
	response->body = render( request );
	return response;
}

Response* Resource::http_head(Request* request)
{
	Response* response =  new Response(OK_200, request );
	//response->body = render( request );
	string< uint8_t >* content_type = ( string< uint8_t >* )malloc( sizeof( string< uint8_t > ) );
	content_type->length = sizeof("text/html");
	content_type->text = (char*)"text/html";
	response->fields.add(Message::CONTENT_TYPE, content_type);
	return response;
}

Response* Resource::http_trace( Request* request )
{
	Response* response = new Response(OK_200, request );
	request->Message::serialize();
	response->body = request->message;
	string< uint8_t >* value = ( string< uint8_t >* )ts_malloc( sizeof( string< uint8_t > ) );
	char* content = ( char* )ts_malloc(13);
	content = (char*)"message/http";
	value->text = content;
	value->length = sizeof( "message/http");
	response->fields.add( Message::CONTENT_TYPE, value );
	return response;
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
		current->update_children_sleep_clock(time);
		current = current->parent;

	}
        processing_wake();

}

void Resource::schedule(uptime_t time)
{
    schedule(&own_sleep_clock, time);
}

string<MESSAGE_SIZE> Resource::render( Request* request )
{
	//string<MESSAGE_SIZE> buffer = MAKE_STRING("<html><body>There are currently no representation associated with this resource.</body></html>");
	//const char* cmsg = "<html><body>There are currently no representation associated with this resource.</body></html>";
	//char* msg = (char*)malloc(sizeof("<html><body>There are currently no representation associated with this resource.</body></html>"));
	return string<MESSAGE_SIZE>::make("//");
}

void Resource::update_children_sleep_clock(uptime_t time)
{


	if(time <  buffer_children_sleep_clock)
	{
		buffer_children_sleep_clock = time;
	}
	if(!visiting_children)
	{
		if(time <  children_sleep_clock)
		{
			children_sleep_clock = time;
		}
	}
}

Resource* Resource::get_next_child_to_visit(void)
{
	if(children && children_sleep_clock <= get_uptime() )
	{

		visiting_children = true;

		while(true)
		{
			if(child_to_visit == children->items)
			{
				child_to_visit = 0;
				visiting_children = false;
				/*In this case, the actual act of finding the next children
				 * to visit can be considered a Resource task in its own
				 * right. Hence, when actually going through the children
				 * to find the next one to process, the current Resource will keep
				 * its 'children_sleep_clock' so other thread can come and
				 * find themselves a child. If this method was entered, it means
				 * the 'child_sleep_clock' attribute was below the uptime; no wake or
				 * sleep_clock update by Resources already visited will lead to a
				 * lower value.*/

				children_sleep_clock = buffer_children_sleep_clock;

				/*Now that we are done running the children that required attention,
				 * we update 'children_sleep_clock' with the value in the buffer.
				 */

				buffer_children_sleep_clock = MAX_UPTIME;

				break;
			}
			Resource* child = (*children)[child_to_visit]->value;
			child_to_visit++;
			if(child->get_sleep_clock() <= get_uptime())
			{
				return child;
			}
			//What follows is probably useless...
			if(child->get_sleep_clock() < buffer_children_sleep_clock)
			{
				buffer_children_sleep_clock = child->get_sleep_clock();
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
				response->body = string<MESSAGE_SIZE>::make("<html><body>Not found</body></html>");
				break;
			default:
				break;
		}
		return response;
	}
	return NULL;
}
