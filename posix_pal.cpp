/*
 * pal.cpp
 *
 *  Created on: Sep 27, 2009
 *      Author: Antoine
 */

#include <pthread.h>

#include <stdint.h>
#include <iostream>
#include <pal/pal.h>
#include <signal.h>

pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
struct sigaction action;
pthread_t timer_thread_id;

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

void init(void)
{
  if( pthread_create( &timer_thread_id, NULL, timeout_handler, NULL ) )
  {
          cerr << "Error creating thread." << endl;
  }
}

void processing_wake()
{
	pthread_cond_broadcast(&cond);
}
void processing_sleep(uptime_t time)
{
	pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
	timespec time_to_sleep = { time - (time % 1000), time % 1000};
	pthread_cond_timedwait(&cond, &mutex, &time_to_sleep);
	pthread_mutex_destroy(&mutex);
}

#ifdef DEBUG
void Debug::print(char c)
{
	 std::cout << c;
}

void Debug::println()
{
	std::cout << std::endl;
}
#endif
