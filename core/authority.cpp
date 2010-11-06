/*
 * authority.cpp
 *
 *  Created on: Feb 17, 2009
 *      Author: Antoine
 */


#include "message.h"
#include "response.h"
#include "request.h"
#include "authority.h"
//#include "../elements.h"
#include "../utils/utils.h"
#include "../utils/types.h"
#include "resource.h"
#include "../pal/pal.h"
#include <stdint.h>

#define OUT_QUEUE_LENGTH 10
#define IN_QUEUE_LENGTH 10

using namespace Elements;

Authority::Authority(void):Resource()
{
	message_queue = new Queue<Message>();
}

Authority::~Authority(void)
{
	/*for( uint8_t i = 0; i < message_queue->items ; i++ )
	{
		delete message_queue[i];
	}*/

}

void Authority::visit(void)
{
        process_queue();
        Resource::visit();
}

Message* Authority::dispatch(Message* message)
{
	message_queue->queue(message);
	schedule(NULL, 0);
	return NULL;
}

/*This is the processing task. If a resource is multithreaded, this method will
constitutes the entry point of a thread.*/
void Authority::process_queue(void)
{
	Message* message;
	while(message_queue->items && message_queue->items < message_queue->capacity)
	{
		message = message_queue->dequeue();
                if(message->to_url_resource_index > 0 || !message->to_url->is_absolute_path)
                {
                    message = Resource::dispatch(message);
                    if(message){ message_queue->queue( message ); }
                }
                else
                {
                    if(Resource::send(message))
                    {
                        message = Resource::dispatch(message);
                        if(message){ message_queue->queue( message ); }
                    }
                }
		/*CONCURENCY PROBLEM: there will most likely be a significant delay between the time that its dertemined there is
		enough space on the messages_out queue and the actual queuing of the message. For exampe, collect() might be called in between
		and load the queue with messages, in which case there might no longer be enough space for the message returned by dispatch(). Also,
	 	the check cannot happen after the message has been produced, because is the message cannot be queued, it will be lost. A possible
		workaround could be to reserve a spot on the a multithreaded queue and free it if is not needed. */

		
	}
}

uint8_t Authority::send(Message* message)
{
    uint8_t result = message_queue->queue(message);
    if(!result)
    {
        schedule(NULL, 0);
    }
    return result;
}

uint64_t Authority::get_sleep_clock(void)
{
    if(message_queue->items)
    {
        return 0;
    }
    return Resource::get_sleep_clock();

}


