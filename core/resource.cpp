/* resource.cpp - Source file for the Resource class.
 * Copyright (C) 2015 Antoine Mercier-Linteau
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

//INCLUDES
#include <utils/utils.h>
#include <pal/pal.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <configuration.h>
#include "message.h"
#include "response.h"
#include "request.h"
#include "resource.h"
#include "url.h"

Resource::Resource(void):
	/* Children is initialized as needed (lazy) in order to save on memory. */
	_children(NULL),
    _own_sleep_clock(MAX_UPTIME),
    _children_sleep_clock(MAX_UPTIME),
	_parent(NULL),
	_child_to_visit(0)
{
}

#if RESOURCE_DESTRUCTION
Resource::~Resource(void)
{
	if(_children) // If a children dictionary was created.
	{
		_children->delete_all(false);

		delete _children;
	}
}
#endif

void Resource::dispatch(Message* message)
{
	Response::status_code sc; //The status code of the processing.
	Response* response = NULL;

	//If the message dispatching type has not yet be determined.
	/* TODO should probably be the responsibility of the message to determine its type of dispatching.
	 * Maybe it equates to to_url->is_absolute().*/
	if(message->get_dispatching_type() == Message::UNDETERMINED)
	{
		 //If the message it going to an absolute URL.
		if(message->get_to_url()->is_absolute())
		{
			message->set_dispatching_type(Message::ABSOLUTE);
		}
		else //Else it is going to a relative URL.
		{
			message->set_dispatching_type(Message::RELATIVE);
		}
	}

	//If the message is a request.
	if(message->get_type() == Message::REQUEST)
	{
		/* In order to save on program memory, a response is always allocated
		 * and given to the process method.
		 * TODO: find a better way to manage this because this is inducing a lot
		 * 	of useless allocations.*/
		response = new Response(Response::OK_200, NULL);
		if(!response) //If allocating a response failed.
		{
			//That message is lost since it cannot have a response.
			delete message; //Free it.
			return;
		}

		/*Process the request and get back the status code. Depending on the
		 * status code, there may also be a response returned.*/
		sc = process((Request*)message, response);
	}
	else //The message is a response.
	{
		//Process the response and get back the status code.
		sc = process((Response*)message);
	}

	/*The resource the message is going to, if it stays NULL, this means
	 * the resource was not found.*/
	Resource* next = NULL;

	/* Switch depending on the status code returned by processing(), cases
	 * concern framework response codes and not HTTP codes. HTTP codes are
	 * processed in the default case. */
	switch(sc)
	{
		case Response::DONE_207: //Processing consumed the message.
			/* The message is freed by dispatch() to save on program memory,
			 * however this introduces the added risk of the processing
			 * methods having already freed the message.*/
			delete message; //Free the message to avoid memory leakage.
			//No break.
		case Response::RESPONSE_DELAYED_102: //The resource will answer this message later.
			/*TODO I should probably replace RESPONSE_DELAYED_102 with KEEP_102 to indicate the
			* framework a resource is keeping the message within its control.*/
			delete response; //We do not need the response, delete it.
			break;
		/*The resource is not processing this message, probably because it is
		not the destination.*/
		case Response::PASS_308:
			/*If the message is going to an absolute URL, it is just sent
			 to the root because it has to transit trough there before
			 finding its destination.*/
			if(message->get_dispatching_type() == Message::ABSOLUTE)
			{
				if(!_parent) //If this resource has no parent.
				{
					//We are at root! dispatch message the other way!
					next = this; //Message will go twice trough root!
					//Dispatching is now relative (to root).
					message->set_dispatching_type(Message::RELATIVE);

					// If the source url is not already absolute.
					if(!message->get_from_url()->is_absolute())
					{
						/*Add a blank resource to from_url to indicate that
						 the message went trough root (/).*/
						if(!message->get_from_url()->insert_resource("", 0, 0))
						{
							// Not enough memory to complete the request.
							sc = Response::INTERNAL_SERVER_ERROR_500;
							goto error;
						}
					}
				}
				else //This resource has a parent.
				{
					/*Since dispatching is absolute, the message has to go to
					root.*/
					next = _parent;
					//If the message is a request.
					if(message->get_type() == Message::REQUEST)
					{
						/*Save this resource's name to the from_url so the
						 * message can be properly routed back to its origin.*/
						//todo error checking.
						if(!message->get_from_url()->insert_resource(_parent->get_name(this), 0, -1))
						{
							// Not enough memory to complete the request.
							sc = Response::INTERNAL_SERVER_ERROR_500;
							goto error;
						}
					}
					//Useless if message is a response.
				}
			}
			else if(message->to_destination()) //If message is not at destination.
			{
				message->next(); //Increment to the next resource in the url.

				//Get that resource's name.
				const char* name = message->current();

				/*If the next resource's name starts with a ".", it could be
				 * an in-place dispatch (".") or a parent dispatch (".."). */
				if(name[0] == '.')
				{
					//If name is ".."
					if(name[1] == '.' && name[2] == '\0' )
					{
						// If the message was relative.
						if(!message->get_to_url()->is_absolute())
						{
							if(!message->get_from_url()->insert_resource(_parent->get_name(this), 0, -1))
							{
								// Not enough memory to complete the request.
								sc = Response::INTERNAL_SERVER_ERROR_500;
								goto error;
							}
						}

						next = _parent; //It is a parent dispatch.
					}
					//Else if name is "." it is an in-place dispatch.
					else if(name[1] == '\0') { next = this; }
				}
				/*If a next resource has not yet been found and this resource
				 * has children.*/
				if(!next && _children)
				{
					// If the message was relative.
					if(!message->get_to_url()->is_absolute())
					{
						if(!message->get_from_url()->insert_resource("..", 0, 2))
						{
							// Not enough memory to complete the request.
							sc = Response::INTERNAL_SERVER_ERROR_500;
							goto error;
						}
					}

					//Look within the children if the next resource is there.
					next = _children->find( name );
				}
			}

			if(next) //If a next resource was found.
			{
				delete response; //We do not need the response.
				 //Dispatch that message to the next resource.
				next->dispatch(message);
				return; //Done.
			}
			sc = Response::NOT_FOUND_404; //Else no resource was found, return a 404.
			//No break here
		default:
			error:
			/*If message was a request and a response code was not found
			in the above cases, this means it is an HTTP response code.*/
			if(message->get_type() == Message::REQUEST)
			{
				response->set_status_code(sc);
				response->set_request((Request*)message);

				// If the dispatching for the request was relative.
				if(!response->get_from_url()->is_absolute())
				{
					// Indicate we passed through this resource.
					///todo useful?
					if(!response->get_to_url()->insert_resource(".", 0, 1))
					{
						// Not even enough memory to return an error.
						delete response;
						return;
					}
				}

				dispatch(response); //Dispatch it.
				return; //Done.
			}
			//TODO what if the response code for a resource was not within the accepted range?
			break;
	}
}

const char* Resource::get_name(const Resource* resource) const
{
    if(_children) //If this resource has children.
    {
        //Get the key for the value resource and return it.
    	return _children->find_val((Resource*)resource);
    }
    return NULL; //Since there are no children, the name could not be found.
}

int8_t Resource::add_child(const char* name, Resource* child )
{
	if(_children == NULL) //If there are no children.
	{
		//This means that children was not initialized.
		_children = new Dictionary<Resource*>();
		if(!_children) //If there is no memory left.
		{
			return -2; //Return an error.
		}
		//children_sleep_clock = 0;
	}

	int8_t return_code = _children->add(name, child); //Add the child.

	if(!return_code) //If the children was successfully added to the dictionary.
	{
		child->_parent = this; //Set that child's parent.
		/*Check if the children needs to be run. It could be that during
		 * initialization, internal tasks have been scheduled.*/
		if(child->get_sleep_clock() < _children_sleep_clock)
		{
			//If so, schedule it.
			_schedule(&_children_sleep_clock, child->get_sleep_clock());
		}
	}

	return return_code; //Return the result of the operation.
}

uint8_t Resource::get_number_of_children()
{
	return !_children ? 0 : _children->get_item_count();
}

///Removes a child resource.
/** @param name the name of the child.
 * @return NULL if removing failed or the children if it was successful. */
Resource* Resource::remove_child(const char* name)
{
	if(_children) //If there are children.
	{
		///TODO check it there is a child with that name.
		Resource* orphan = _children->remove(name);
		orphan->_parent = NULL;
		return orphan;
	}

	return NULL;
}

void Resource::print_transaction(const Message* message)
{
	/*If VERBOSITY is undefined, this method should be optimized away
	 *  by the compiler.*/

#if VERBOSITY
	VERBOSE_PRINT("from: ");
	message->get_from_url()->print();
	VERBOSE_PRINTLN();
	VERBOSE_PRINT("to: ");
	message->get_to_url()->print();
	VERBOSE_PRINTLN();
	message->print();
	VERBOSE_PRINTLN();
#endif

}

Response::status_code Resource::process(const Request* request, Response* response)
{
	if(!request->to_destination()) //If the message is at destination.
	{
		//print_transaction(request); //Print it to the console.
		//Whatever was request is not implemented.
		return Response::NOT_IMPLEMENTED_501;
	}
	return Response::PASS_308; //Pass the message.
}

Response::status_code Resource::process(const Response* response)
{
	if(!response->to_destination()) //If the message is at destination.
	{
		//print_transaction(response); //Print it to the console.
		//Nothing is done with the message, it will be deleted by dispatch.
		return Response::DONE_207; //Done with processing.
	}
	return Response::PASS_308; //Pass the message.
}

void Resource::run(void)
{
	 //Nothing to do, schedule the timer so it never expires again.
	_schedule(&_own_sleep_clock, NEVER);
}

uptime_t Resource::get_sleep_clock(void)
{
	//Return children_sleep_clock or own_sleep_clock, whichever is less.
	return _children_sleep_clock > _own_sleep_clock ? _own_sleep_clock: _children_sleep_clock;
}

void Resource::schedule(uptime_t time)
{
	_schedule(&_own_sleep_clock, time);
}

void Resource::_schedule(volatile uptime_t* timer, uptime_t time)
{
    ///TODO check if timer will overflow.
	if(time != MAX_UPTIME) //If time is less than MAX_UPTIME.
    {
		//Add the uptime to get the uptime at which the timer will expire.
		time += get_uptime();
    }

    if(timer) //If a timer was specified.
    {
    	*timer = time; //Update it.
    }

	/* Loops up the resource tree to update the children_sleep_clock.
	 * This is not made recursive in order to save on stack space and
	 * processing time. */
    Resource* current = _parent; //Start with the parent.
	while(true)
	{
		if (!current) //If current is null, we have reached the top.
		{
			break; //Exit the loop.
		}
		//Update the children_sleep_clock timer.
		if(current->_children_sleep_clock > time)
		{
			current->_children_sleep_clock = time;
		}
		else
		{
			break;
		}
		current = current->_parent; //select the parent.
	}

    processing_wake(); //Wake processing if we were called by an interrupt.

}

Resource* Resource::_get_next_child_to_visit(void)
{
	//If this resource has children and if some need to be visited.
	if(_children && is_expired(_children_sleep_clock) )
	{
		while(true)
		{
			/* An internal index of the last Resource visited is kept so all
			 * get a chance for processing. */
			//If we have reached the end of the children list.
			if(_child_to_visit == _children->get_item_count())
			{
				_child_to_visit = 0; //Reset the internal counter.
				_children_sleep_clock = MAX_UPTIME; //Reset the sleep clock.

				/* While we were processing children, it is possible that some
				 * other children updated their sleep clock (through an
				 * interrupt or another instance of Processing). Loop trough
				 * all of them to update our children sleep clock. */
				for(uint8_t i = 0; i < _children->get_item_count(); i++)
				{
					uptime_t sleep_clock = (*_children)[i]->value->get_sleep_clock();
					//If that child's sleep_clock is lower than ours.
					if(sleep_clock < _children_sleep_clock)
					{
						_children_sleep_clock = sleep_clock; //Replace it.
					}
				}

				break; //Done.
			}

			//Get the next child to visit.
			Resource* child = (*_children)[_child_to_visit]->value;
			_child_to_visit++; //Increment the internal index.
			//If that child needs to be ran.
			if(child->get_sleep_clock() <= get_uptime())
			{
				return child; //Return it.
			}
		}
	}

	return NULL; //No child to visit.
}
