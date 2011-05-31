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

class ESerial: public Resource
{

	#define MESSAGE_BUFFER_INCREMENT_SIZE 20
	#define MAX_AGE 100

	char* buffer;
	volatile uint16_t index;
	volatile uint16_t buffer_size;
	volatile uptime_t age;
	bool body_started;
	bool newcomm;

	public:
		ESerial();

		void receive(uint8_t c);
		void write(uint8_t c);

	protected:
		void run(void);
		virtual Response::status_code process(Response* response, Message** return_message);

};

#endif /* ESERIAL_H_ */
