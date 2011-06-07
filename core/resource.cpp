/* resource.cpp - Implements an abstract resource
 * Copyright (C) 2011 Antoine Mercier-Linteau
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "message.h"
#include "response.h"
#include "request.h"
#include "resource.h"
#include "../elements.h"
#include "url.h"
#include "../utils/utils.h"
#include "../pal/pal.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>


Resource::Resource(void):
  children_sleep_clock(MAX_UPTIME),
  own_sleep_clock(MAX_UPTIME)
{
	child_to_visit = 0;
	children = 0;
	parent = 0;
	//register_element();
}

#if RESOURCE_DESTRUCTION
	Resource::~Resource(void)
	{
		//unregister_element();
		delete children;
	}
#endif

void Resource::visit(void)
{
	if(is_expired(own_sleep_clock))
	{
		own_sleep_clock = NEVER;
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
	File* return_body = NULL;
	const char * mime = NULL;

	if(message->object_type == Message::REQUEST)
	{
		sc = process((Request*)message, &return_body, &mime);
	}
	else
	{
		sc = process((Response*)message);
	}


	switch(sc)
	{
		case RESPONSE_DELAYED_102: break;
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
			//No break here
		default:
			if(message->object_type == Message::REQUEST)
			{
				Response* response = new Response(sc, (Request*)message);
				if(!response)
				{
					delete return_body;
					break;
				}
				if(return_body)
				{
					response->set_body(return_body, mime);
				}
				return response;
			}

			/*I should propably replace RESPONSE_DELAYED_102 with KEEP_102 to indicate the
			 * framework a resource is keeping the message wthing its control.*/
	}
	return NULL;
}

uint8_t Resource::send(Message* message)
{
    if(parent)
    {
        /*PROBLEM: In a case where a child's name string was allocated on the heap and it is removed
         * while a message is in flight, that name, since it was not copied, could be modified
         * and become something different.*/
    	if(message->object_type == Message::REQUEST)
    	{
			const char* name = parent->get_name(this);

			if(!message->to_url->is_absolute_path)
			{
				message->to_url->resources.insert(name,0);
			}
			message->from_url->resources.insert(name,0);
    	}
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
		children = new Dictionary<Resource*>();
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
	/*If VERBOSITY is undefined, this method should be optimizes away by the compiler.*/

#if VERBOSITY
	VERBOSE_PRINT("from: ");
	message->from_url->print();
	VERBOSE_PRINTLN();
	VERBOSE_PRINT("to: ");
	message->to_url->print();
	VERBOSE_PRINTLN();
	message->print();
	VERBOSE_PRINTLN();
#endif
}

Response::status_code Resource::process( Request* request, File** return_body, const char** mime )
{
	if(request->to_url->cursor >=  request->to_url->resources.items)
	{
		print_transaction(request);
		return NOT_IMPLEMENTED_501;
	}
	return PASS_308;
}

Response::status_code Resource::process(Response* response)
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

Resource* Resource::get_next_child_to_visit(void)
{
	if(children && is_expired(children_sleep_clock) )
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
