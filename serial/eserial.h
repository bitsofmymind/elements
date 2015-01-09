/* eserial.h - Implements a resource to interface with an USART
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

#ifndef ESERIAL_H_
#define ESERIAL_H_

#include <core/resource.h>
#include <pal/pal.h>
#include <core/request.h>

/// ESerial allows HTTP messages to be sent and received through a USART (serial)
class ESerial: public Resource
{
	/** The number of bytes by which the size of the buffer will be increased
	 * one it fills up. */
	#define MESSAGE_BUFFER_INCREMENT_SIZE 20

	/** The number of milliseconds to wait after the last character has been
	 * received before parsing the message. */
	#define DELAY 100

	/** The buffer that hols the message while they are being received. */
	char* buffer;

	/** The index at which message received is at in the buffer. */
	volatile uint16_t index;

	/** The size of the buffer. */
	volatile uint16_t buffer_size;

	/** The time at which we started receiving the current message. */
	volatile uptime_t age;

	/** If the body of the message has started being transmitted. */
	bool body_started;

	public:

		/** Class constructor. */
		ESerial();

		/** Receive a character from the USART.
		 * @param c the received character. */
		void receive(uint8_t c);

		/** Send a character through the USART.
		 * @param c the character to send. */
		void write(uint8_t c);

	protected:

		/// Lets the resource do some processing.
		/** Override of parent implementation to forward messages received
		 *  through the USART to the framework. */
		void run(void);

        /// Process a response message.
        /**
         * Override of parent implementation to send out messages from the
         * framework.
         * @param response the response to fill if a response should be returned (which
         * depends on the status code).
         * @return the status_code produced while processing the request.
         */
		virtual Response::status_code process(Response* response);
};

#endif /* ESERIAL_H_ */
