/* posix_pal.cpp - Defines the platform abstraction layer for a POSIX platform.
 * Copyright (C) 2014 Antoine Mercier-Linteau
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

// INCLUDES.

#include <pthread.h>
#include <stdint.h>
#include <iostream>
#include <pal/pal.h>
#include <signal.h>
#include <sys/time.h>

/**
 * A condition to make the main thread sleep.
 * */
pthread_cond_t cond =  PTHREAD_COND_INITIALIZER;

/**
 * The mutex on which processing will wait.
 */
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

/**
 * A thread that updates the uptime.
 * */
pthread_t uptime_thread;

using namespace std;

/*void timeout_handler( int signal_number )
{
	struct itimerval timer;

	// Sets a time interval of 1 millisecond.
	timer.it_interval.tv_usec = 1000;
	timer.it_interval.tv_sec = 0;
	timer.it_value.tv_usec = 1000;
	timer.it_value.tv_sec = 0;

	// Sets a handler for the alarm signal.
	signal(SIGALRM, timeout_handler);

	// Set the timer that will trigger after the set interval.
	setitimer(ITIMER_REAL, &timer, 0);

	if(signal_number == SIGALRM) // If the signal was sent by the timer expiring.
	{
		increase_uptime(1); // Increase the uptime by one millisecond.
	}
}*/

void* update_uptime(void* args)
{
	// Only required by pthread_cond_timedwait.
	pthread_cond_t uptime_condition =  PTHREAD_COND_INITIALIZER;
	pthread_mutex_t uptime_mutex = PTHREAD_MUTEX_INITIALIZER;

	timespec interval = {0, 1000000}; // One millisecond.

	// pthread_cond_timedwait works using absolute time.
	timespec timer;

	// Block this thread from receiving signals.
	sigset_t set;
	sigfillset(&set);
	pthread_sigmask(SIG_SETMASK, &set, NULL);

	while(true)
	{
		clock_gettime(CLOCK_REALTIME, &timer);
		timer.tv_nsec += 1000000; // Add one millisecond.

		if(timer.tv_nsec >= 1000000000) // If the nanoseconds have overflowed.
		{
			timer.tv_nsec -= 1000000000;
			timer.tv_sec += 1;
		}

		pthread_cond_init(&uptime_condition, NULL);
		pthread_mutex_init(&uptime_mutex, NULL);

		// Wait until time expires.
		pthread_mutex_lock(&uptime_mutex);
		pthread_cond_timedwait(&uptime_condition, &uptime_mutex, &timer);
		pthread_mutex_unlock(&uptime_mutex);

		timespec current_time;
		clock_gettime(CLOCK_REALTIME, &current_time);

		// Increase the uptime by the time that has elapsed.
		//increase_uptime(SECONDS(current_time.tv_sec - timer.tv_sec) + ((double)current_time.tv_nsec / 1000000 - (double)timer.tv_nsec / 1000000));
		increase_uptime(1); // The above causes the thread to stop waiting for some reason.
	}

	return NULL;
}

void init(void)
{
	pthread_cond_init(&cond, NULL);
	pthread_mutex_init(&mutex, NULL);

	/* Sets up a signal to keep track of uptime but give it a bogus signal
	 * number so that uptime is not increased.*/
	//timeout_handler(SIGUSR1);

	/* Stars a thread that updates the uptime at specific intervals.*/
	pthread_create(&uptime_thread, NULL, update_uptime, NULL);
}

void processing_wake()
{
	pthread_mutex_lock(&mutex);
	pthread_cond_broadcast(&cond); // Wake up the main thread.
	pthread_mutex_unlock(&mutex);
}

void processing_sleep(uptime_t sleep_time)
{
	// The amount of time on which processing will wait.
	timespec time_to_sleep = {(sleep_time - (sleep_time % 1000)) / 1000, (sleep_time % 1000) * 1000};
	// pthread_cond_timedwait works using absolute time.
	timespec system_clock;
	clock_gettime(CLOCK_REALTIME, &system_clock);
	time_to_sleep.tv_sec += system_clock.tv_sec;
	time_to_sleep.tv_nsec += system_clock.tv_nsec;

	if(time_to_sleep.tv_nsec >= 1000000000) // If the nanoseconds have overflowed.
	{
		time_to_sleep.tv_nsec -= 1000000000;
		time_to_sleep.tv_sec += 1;
	}

	pthread_cond_init(&cond, NULL);
	pthread_mutex_init(&mutex, NULL);

	// Wait until the condition is triggered or the time expires.
	pthread_mutex_lock(&mutex);
	pthread_cond_timedwait(&cond, &mutex, &time_to_sleep);
	pthread_mutex_unlock(&mutex);
}

#if HEARTBEAT
void heart_beat(void)
{
	/* No need to have a POSIX compatible system do heartbeats since the status
	 * of the process can easily be checked using operating system tools.*/
}
#endif

#ifdef OUTPUT_DEBUG

void Debug::println()
{
	std::cout << std::endl;
}

void Debug::print_char(char c)
{
	std::cout << c;
}

#endif

