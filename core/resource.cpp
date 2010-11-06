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
//#include "../elements.h"
#include "url.h"
#include "../utils/utils.h"
#include "../utils/types.h"
#include "../pal/pal.h"
#include <stdint.h>
#include <stdlib.h>

#ifdef DEBUG
	#include <iostream>
#endif

using namespace Elements;

string<uint8_t> parent_resource = MAKE_STRING("..");

Resource::Resource(void)
{
	own_sleep_clock = 0;
	children_sleep_clock = UINT64_MAX;
	buffer_children_sleep_clock = UINT64_MAX;
	child_to_visit = 0;
	visiting_children = false;
	//register_element();
}

Resource::~Resource(void)
{
	//unregister_element();
	delete children;
}

void Resource::visit(void)
{
	if(own_sleep_clock <= get_uptime() )
	{
		own_sleep_clock = UINT64_MAX;
		run();
	}
}

Resource* Resource::find_resource( string<uint8_t>* name )
{
	if(!children)
	{
		return NULL;
	}
	return children->find( *name );
}

Message* Resource::dispatch( Message* message)
{
	string<uint8_t>* resource_name = message->to_url->resources[message->to_url_resource_index];
	message->to_url_resource_index++;
	if( resource_name == NULL )
	{
		if( message->get_type() == Message::REQUEST)
		{
			return process( (Request*) message );
		}
		return process( (Response*)message);
	}
	if( resource_name->text[0] == '.' && resource_name->text[1] == '.' && resource_name->length == 2 )
	{
		if( !message->to_url->is_absolute_path)
		{
			message->from_url->resources.append(message->to_url->resources[message->to_url_resource_index - 2 ]);
		}
		return message;
	}
	if( resource_name->text[0] == '.' && resource_name->length == 1 )
	{
		return dispatch(message); //RISK OF STACK OVERFLOW IF THERE IS TOO MUCH /./././././././././.
	}

	if( !message->to_url->is_absolute_path)
	{
		message->from_url->resources.append(&parent_resource);
	}
	Resource* child_resource = find_resource( resource_name );

	if( child_resource == NULL )
	{
		if( message->get_type() == Message::REQUEST )
		{
			Response* response = new Response( &Response::NOT_FOUND_CODE, &Response::NOT_FOUND_REASON_PHRASE, (Request*)message );
			string<uint32_t> content = MAKE_STRING("<html><body>Not found</body></html>");
			response->body = content;
			string<uint8_t>* content_type = (string<uint8_t>*)malloc(sizeof(string<uint8_t>));
			content_type->length = sizeof("text/html");
			content_type->text = (char*)"text/html";
			response->fields.add(Message::CONTENT_TYPE, content_type );
			return response;
		}
		delete message ;
		return NULL;
	}

	message = child_resource->dispatch(message);
	update_children_sleep_clock(child_resource->get_sleep_clock());

	if(message==NULL) {return NULL;}
	if( message->to_url->is_absolute_path )
	{
		return message;
	}
	return dispatch(message);
}

uint8_t Resource::send(Message* message)
{
    if(parent)
    {
        string<uint8_t>* name = parent->get_name(this);

        if(!message->to_url->is_absolute_path)
        {
            message->to_url->resources.insert(name,0);
        }
        message->from_url->resources.insert(name,0);
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
	Resource* orphan = !children ? NULL: children->remove(name);
	orphan->parent = NULL;
	return orphan;
}

string<uint8_t> get_method = MAKE_STRING( "get");
string<uint8_t> head_method = MAKE_STRING( "head");
string<uint8_t> trace_method = MAKE_STRING( "trace");

Response* Resource::process( Request* request )
{
	#ifdef DEBUG
		print_transaction(request);
	#endif
	if( request->method == get_method )
	{
		return http_get( request );
	}
	else if( request->method == head_method )
	{
		return http_head( request );
	}
	else if( request->method == trace_method )
	{
		return http_trace( request );
	}

	return new Response(&Response::NOT_IMPLEMENTED_CODE, &Response::NOT_IMPLEMENTED_REASON_PHRASE, request);
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
		using namespace std;

		if(message->get_type() == Message::RESPONSE)
		{
			cout << "################### RESPONSE Received ###################" << endl;
		}
		else
		{
			cout << "################### REQUEST Received ###################" << endl;
		}
		cout << "From: ";
		message->from_url->print();
		cout << endl;
		cout << "To: ";
		message->to_url->print();
		cout << endl;
		message->print();
		cout << endl;
	}

#endif

void Resource::run( void )
{
	schedule( &own_sleep_clock, UINT64_MAX );
}


uint64_t Resource::get_sleep_clock( void )
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
	Response* response =  new Response(&Response::OK_CODE, &Response::OK_REASON_PHRASE, request );
	response->body = render( request );
	string< uint8_t >* content_type = ( string< uint8_t >* )malloc( sizeof( string< uint8_t > ) );
	content_type->length = sizeof("text/html");
	content_type->text = (char*)"text/html";
	response->fields.add(Message::CONTENT_TYPE, content_type);
	return response;
}

Response* Resource::http_trace( Request* request )
{
	Response* response = new Response( &Response::OK_CODE, &Response::OK_REASON_PHRASE, request );
	request->Message::serialize();
	response->body = request->message;
	string< uint8_t >* value = ( string< uint8_t >* )malloc( sizeof( string< uint8_t > ) );
	char* content = ( char* )malloc(13);
	content = (char*)"message/http";
	value->text = content;
	value->length = sizeof( "message/http");
	response->fields.add( Message::CONTENT_TYPE, value );
	return response;
}

void Resource::schedule( uint64_t* timer, uint64_t time )
{

    if(time != UINT64_MAX)
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

void Resource::schedule(uint64_t time)
{
    schedule(&own_sleep_clock, time);
}

string<uint32_t> Resource::render( Request* request )
{
	string<uint32_t> buffer = MAKE_STRING("<html><body>There are currently no representation associated with this resource.</body></html>");
	return buffer;
}

void Resource::update_children_sleep_clock(uint64_t time)
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
			Resource* child = (*children)[child_to_visit]->value;

			if(child->get_sleep_clock() <= get_uptime())
			{
				return child;
			}
			if(child->get_sleep_clock() < buffer_children_sleep_clock)
			{
				buffer_children_sleep_clock = child->get_sleep_clock();
			}
			if(++child_to_visit == children->items)
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

				buffer_children_sleep_clock = UINT64_MAX;

				break;
			}
		}
	}

	return NULL;
}

