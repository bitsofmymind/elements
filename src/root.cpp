/*
 * root.cpp
 *
 *  Created on: Jul 2, 2009
 *      Author: Antoine
 */

#include <root.h>
#include <stdint.h>
#include <stdlib.h>
#include <cstdio>
#include <cstring>
#include <utils/types.h>
#include <pal/pal.h>
#include <sys/time.h>
#include <unistd.h>
#include <pthread.h>
#include <iostream>


using namespace std;

void* timeout_handler( void* args )
{
	//struct itimerval timer;

	/*timer.it_interval.tv_usec = 1000;
	timer.it_interval.tv_sec = 0;
	timer.it_value.tv_usec = 1000;
	timer.it_value.tv_sec = 0;*/

	//signal(SIGALRM, timeout_handler);

	uint32_t last_timeout = time(NULL);
	while(true)
	{
		increase_uptime(time(NULL) - last_timeout);
		set_time( time(NULL) );
		last_timeout = time(NULL);
		usleep(50000);
	}

	//setitimer(ITIMER_REAL, &timer, 0);
}

Root::Root():
	Authority()
{
	/*action.__sigaction_u.__sa_handler = timeout_handler;
	action.sa_flags = 0;//SA_NODEFER;
	action.sa_mask = 0;

	//sigaction(SIGALRM, &action, NULL);

	struct itimerval timer;
	timer.it_interval.tv_usec = 50000;
	timer.it_interval.tv_sec = 0;
	timer.it_value.tv_usec = 50000;
	timer.it_value.tv_sec = 0;
	setitimer(ITIMER_REAL, &timer, 0);*/

	if( pthread_create( &timer_thread_id, NULL, timeout_handler, NULL ) )
	{
		cerr << "Error creating thread." << endl;
	}
}

Root::~Root()
{

}

void Root::process_queue(void)
{
    Message* message;
    while(message_queue->items && message_queue->items < message_queue->capacity)
    {
            message = message_queue->dequeue();
            message = Resource::dispatch(message);
            if(message){ message_queue->queue( message ); }
            
            /*CONCURENCY PROBLEM: there will most likely be a significant delay between the time that its dertemined there is
            enough space on the messages_out queue and the actual queuing of the message. For exampe, collect() might be called in between
            and load the queue with messages, in which case there might no longer be enough space for the message returned by dispatch(). Also,
            the check cannot happen after the message has been produced, because is the message cannot be queued, it will be lost. A possible
            workaround could be to reserve a spot on the a multithreaded queue and free it if is not needed. */

    }
}
