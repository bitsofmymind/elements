/* authority.cpp - Implements a basic authority resource
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
#include "authority.h"
#include <utils/utils.h>
#include "resource.h"
#include <pal/pal.h>
#include <stdint.h>

#define OUT_QUEUE_LENGTH 10
#define IN_QUEUE_LENGTH 10

Authority::Authority(void):
	Resource()
#if AUTHORITY_REDIRECT
	,redirect_url(NULL)
#endif
{
}

#if RESOURCE_DESTRUCTION
	Authority::~Authority(void)
	{}
#endif

Response::status_code Authority::process( Request* request, Response* response )
{
	if(request != message_queue.peek())
	{
		message_queue.queue(request);
		schedule(ASAP);
		return RESPONSE_DELAYED_102;
	}

	if(!request->to_destination())
	{
#if AUTHORITY_REDIRECT
		if(redirect_url)
		{
			response->location = redirect_url;
			return TEMPORARY_REDIRECT_307;
		}
#endif
		return NOT_IMPLEMENTED_501;
	}
	return PASS_308;
}

Response::status_code Authority::process( Response* response )
{
	if(response != message_queue.peek())
	{
		message_queue.queue(response);
		schedule(ASAP);
		return RESPONSE_DELAYED_102;
	}
	if(!response->to_destination())
	{
		return NOT_IMPLEMENTED_501;
	}
	return PASS_308;
}

void Authority::run(void)
{
	while(message_queue.items && message_queue.items < CAPACITY)
	{
		dispatch(message_queue.peek()); /*We do not care about what comes out of dispatch since it will have
		been queued during dispatch's call to Authority::process.*/
		message_queue.dequeue(); //message has been dispatched so it can be dequeued.
		/*CONCURENCY PROBLEM: there will most likely be a significant delay between the time that its dertemined there is
		enough space on the messages_out queue and the actual queuing of the message. For exampe, collect() might be called in between
		and load the queue with messages, in which case there might no longer be enough space for the message returned by dispatch(). Also,
	 	the check cannot happen after the message has been produced, because is the message cannot be queued, it will be lost. A possible
		workaround could be to reserve a spot on the a multithreaded queue and free it if is not needed. */


	}
}
