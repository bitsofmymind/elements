// SVN FILE: $Id: $
/**
 * @lastChangedBy           $lastChangedBy: Mercier $
 * @revision                $Revision: 397 $
 * @copyright    			GNU General Public License
 * 		This program is free software: you can redistribute it and/or modify
 * 		it under the terms of the GNU General Public License as published by
 * 		the Free Software Foundation, either version 3 of the License, or
 * 		(at your option) any later version.
 * 		This program is distributed in the hope that it will be useful,
 * 		but WITHOUT ANY WARRANTY; without even the implied warranty of
 *		MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * 		GNU General Public License for more details.
 * 		You should have received a copy of the GNU General Public License
 * 		along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * Source file for the Authority class.
 */

//INCLUDES
#include <utils/utils.h>
#include <pal/pal.h>
#include <stdint.h>
#include "message.h"
#include "response.h"
#include "request.h"
#include "authority.h"
#include "resource.h"

///@todo delete that if it is useless.
#define OUT_QUEUE_LENGTH 10
///@todo delete that if it is useless.
#define IN_QUEUE_LENGTH 10

/// An authority is a Resource class which provides message buffering.
/**
 * @class Authority
 * An Authority act as a message flow regulator by its capacity to buffer
 * messages and releasing them according to the available processing capacity.
 * It does so by queuing every message it receives and releasing them a few at
 * a time. If much processing is available, the Authority will be visited very
 * often thereby allowing it to release message at an increased rate. If the
 * system is under a high load, an Authority will be visited less often and
 * in so doing releasing messages at a slower rate.
 */

/// The constructor for the Authority class.
Authority::Authority(void):
	Resource()
#if AUTHORITY_REDIRECT
	,redirect_url(NULL) //Set redirect_url to null to disable redirection.
#endif
{}

#if RESOURCE_DESTRUCTION
	/// The destructor for the Authority class.
	Authority::~Authority(void)
	{}
#endif

/**
 * Authority overrides this method to intercept messages and queue them instead
 * of processing them like a Resource would do.
 */
Response::status_code Authority::process( Request* request, Response* response )
{
	/* Since a call to dispatch will cause this method to be called, we
	 * it is possible to identify if the request being dispatched had been
	 * queued before or not. */
	if(request != message_queue.peek())
	{
		//This request was not part of the queue.
		///@todo should check if the queue is not full.
		message_queue.queue(request); //Queue it.
		schedule(ASAP); //Schedule the resource to be run ASAP.
		return RESPONSE_DELAYED_102; //Inform the framework that we are keeping the message.
	}
	//Else process it normally.

	if(!request->to_destination()) //If the request has arrived at destination.
	{
#if AUTHORITY_REDIRECT
		if(redirect_url) //If redirection is active.
		{
			response->location = redirect_url; //Write the location header.
			return TEMPORARY_REDIRECT_307; //Redirect the client's browser.
		}
#endif
		return NOT_IMPLEMENTED_501; //We cannot process this request.
	}

	return PASS_308; //The message it not at destination so pass it.
}

/**
 * Authority overrides this method to intercept messages and queue them instead
 * of processing them a Resource would do.
 */
Response::status_code Authority::process( Response* response )
{
	/* Since a call to dispatch will cause this method to be called, we
	 * it is possible to identify if the response being dispatched had been
	 * queued before or not. */
	if(response != message_queue.peek())
	{
		//This response was not part of the queue.
		///@todo should check if the queue is not full.
		message_queue.queue(response); //Queue it.
		schedule(ASAP); //Schedule the resource to be run ASAP.
		return RESPONSE_DELAYED_102; //Inform the framework that we are keeping the message.
	}
	//Else process it normally.

	//If the response has arrived at destination.
	if(!response->to_destination())
	{
		return NOT_IMPLEMENTED_501; //We cannot process this request.
	}
	return PASS_308; //The message it not at destination so pass it.
}

/**
 * Authority overrides this method so what when it gets ran by processing,
 * the messages that it has in queue get dispatched.
 */
void Authority::run(void)
{
	///@todo checking if queue is full here is useless.
	// While there are messages in the queue.
	while(message_queue.items && message_queue.items < CAPACITY)
	{
		/*Dispatch the first message in the queue. Use peek because the
		 * processing functions should be able to tell where the message it
		 * from. */
		dispatch(message_queue.peek());
		//Message has been dispatched so it can be dequeued.
		message_queue.dequeue();
	}
}
