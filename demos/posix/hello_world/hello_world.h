/* hello_world.h - a basic hello_world resource.
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


#ifndef HELLO_WORLD_H_
#define HELLO_WORLD_H_

#include <core/resource.h>

/**
 * A basic demonstration resource that displays a message and a simple form
 * to modify the interval at which it prints a message to the output.
 * */
class HelloWorld: public Resource
{
	protected:

		/**
		 * The message that is being printed on the output.
		 * */
		char* message;

		/**
		 * Turns the printing of the message on or off.
		 * */
		bool state;

	public:

		/**
		 * Class constructor.
		 * */
		HelloWorld();

		/**
		 * Class destructor.
		 * */
		~HelloWorld();

		/**
		 * Executes the resource, effectively printing the message.
		 * */
		virtual void run(void);

		/**
		 * Process a request to this resource.
		 * @param request the request.
		 * @param response response
		 * @return a status code informing on what was done with the request.
		 * */
		virtual Response::status_code process( Request* request, Response* response );
};

#endif /* HELLO_WORLD_H_ */
