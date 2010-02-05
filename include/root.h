/*
 * root.h
 *
 *  Created on: Jul 2, 2009
 *      Author: Antoine
 */

#ifndef ROOT_H_
#define ROOT_H_

#include <sys/types.h>
#include "../elements/utils/utils.h"
#include "../elements/core/authority.h"
#include "../posix_socket_interface/posix_socket_interface.h"
#include <stdint.h>
#include <signal.h>
#include <pthread.h>

class Root: public Authority
{
	private:
		struct sigaction action;
		pthread_t timer_thread_id;

	public:
		PosixSocketInterface* interface;

		Root();
		virtual ~Root();

                virtual void process_queue(void);

};

#endif /* ROOT_H_ */
