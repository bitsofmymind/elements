/*
 * pal.cpp
 *
 *  Created on: Sep 27, 2009
 *      Author: Antoine
 */

#include <pthread.h>

#include <stdint.h>
#include <utils/types.h>

pthread_cond_t alarm = PTHREAD_COND_INITIALIZER;

void processing_wake()
{
	pthread_cond_broadcast(&alarm);
}
void processing_sleep(Elements::e_time_t time)
{
	pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
	timespec time_to_sleep = { time.seconds, 1000*time.milliseconds};
	pthread_cond_timedwait(&alarm, &mutex, &time_to_sleep);
	pthread_mutex_destroy(&mutex);
}
