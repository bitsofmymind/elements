/* authority.h - Header file for the authority class.
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
#ifndef AUTHORITY_H_
#define AUTHORITY_H_

//INCLUDES

#include <stdlib.h>
#include <utils/utils.h>
#include "message.h"
#include "response.h"
#include "request.h"
#include "resource.h"

///@todo delete that if it is useless.
#define MAX_NUMBER_OF_CHILD_AUTHORITIES 10

/// An authority is a Resource class which provides message buffering.
/**
 * An Authority act as a message flow regulator by its capacity to buffer
 * messages and releasing them according to the available processing capacity.
 * It does so by queuing every message it receives and releasing them a few at
 * a time. If much processing is available, the Authority will be visited very
 * often thereby allowing it to release message at an increased rate. If the
 * system is under a high load, an Authority will be visited less often and
 * in so doing releasing messages at a slower rate.
 */
class Authority: public Resource
{
	private:

		/** The queue the authority uses to buffer its messages.
		 * Defined inline so the cost of memory allocation is saved.
		 * */
		Queue<const Message*> _message_queue;

	public:

#if AUTHORITY_REDIRECT
		/**
		 * The redirect url when requests for this authority should get
		 * redirected. The content of this variable gets put in the Location
		 * header field.
		 * */
		const char* redirect_url;
#endif

		/// Class constructor.
		Authority(void);

#if RESOURCE_DESTRUCTION
		/// Class destructor.
		virtual ~Authority(void);
#endif

	protected:

		/**
		 * Authority overrides this method to intercept messages and queue them instead
		 * of processing them like a Resource would do.
		 * */
		virtual Response::status_code process(const Request* request, Response* response);

		/**
		 * Authority overrides this method to intercept messages and queue them instead
		 * of processing them a Resource would do.
		 * */
		virtual Response::status_code process(const Response* response);

		/**
		 * Authority overrides this method so what when it gets ran by processing,
		 * the messages that it has in queue get dispatched.
		 * */
		virtual void run(void);
};

#endif /* AUTHORITY_H_ */
