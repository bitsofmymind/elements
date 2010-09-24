/*
 * thread.h
 *
 *  Created on: Jul 5, 2009
 *      Author: Antoine
 */

#ifndef THREAD_H_
#define THREAD_H_

#include <core/processing.h>
#include <utils/utils.h>
#include <pthread.h>
#include "root.h"
#include <time.h>

class PosixMainThread: public Processing
{
	protected:
		virtual void processing_sleep(uint32_t milliseconds);
};

class PosixThread: public Processing
{
	private:
		volatile bool disposing;
		uint32_t sleep_start;
		pthread_t id;

	protected:

		enum THREAD_STATE{ BUSY, SLEEPING } state;

	public:

		PosixThread( );
		virtual ~PosixThread();

		virtual void start(void);
		static void* run_thread( void* arg );
		virtual void processing_sleep(uint32_t milliseconds);
};
static List<pthread_t> ids;

class PosixThreadManager: public Resource
{

	protected:
		Dictionary<PosixThread> threads;
		Root* root;
	public:
		PosixThreadManager( Root* root, uint32_t initial_number_of_threads );

	protected:
		virtual string<uint32_t> render(Request* request);
};

#endif /* THREAD_H_ */
