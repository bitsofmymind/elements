/* hello_world.cpp - a basic hello_world resource.
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

// INCLUDES

#include "hello_world.h"
#include <pal/pal.h>
#include <stdlib.h>
#include <iostream>
#include <utils/template.h>
#include <utils/memfile.h>
#include <string.h>

HelloWorld::HelloWorld():
	Resource(),
	state(true)
{
	// Allocates block containing the default message.
	const char* m = "Hello world!";
	message = (char*)malloc(strlen(m) + 1);
	strcpy(message, m);

	schedule( 2000 );
}

HelloWorld::~HelloWorld()
{
	free(message);
}

void HelloWorld::run(void)
{
	if(state) // If the message should be displayed.
	{
		std::cout << message << std::endl;
	}

	schedule(SECONDS(2)); // Display message again in two seconds.
}

Response::status_code HelloWorld::process( Request* request, Response* response )
{
	// Let the parent class process the message.
	Response::status_code sc = Resource::process(request, response);

	 // If the parent class could not handle the message.
	if(sc == NOT_IMPLEMENTED_501)
	{
		if(request->is_method("post")) // If this is a post request.
		{
			char buffer[50]; // Holds the message.

			// Read the message argument from the request.
			uint8_t len = request->find_arg("m", buffer, 50);

			if(len) // If an argument containing the new message was found.
			{
				// Replace the current message with the new message.
				free(message);
				message = (char*)malloc(len + 1);
				strcpy(message, buffer);
			}

			// Read the state argument from the message.
			len = request->find_arg("st", buffer, 1);

			if(len) // If there is a state argument in the request.
			{
				// Turns the displaying of the message on or off.
				if(buffer[0] == '1')
				{
					state = true;
				}
				else if(buffer[0] == '0')
				{
					state = false;
				}
			}
		}
		else if(!request->is_method("get")) // If the request is not a get.
		{
			return NOT_IMPLEMENTED_501;
		}

		const char* html = \
		"<html>\
			<body>\
				<h2>Hello World resource</h2>\
				You can use the following form to change the message the resource is displaying in stdout and</br>\
				turn it on or off.</br>\
				<br/>\
				<form method=\"post\" accept-charset=\"us-ascii\">\
					Message: <input type=\"text\" id=\"m\" name=\"m\" value=\"~\"/><br/>\
					ON<input type=\"radio\" id =\"st_1\" name=\"st\" ~ value=\"1\"/><br/>\
					OFF<input type=\"radio\" id=\"st_0\" name=\"st\" ~ value=\"0\"/><br/>\
					<input type=\"submit\" value=\"Submit\" />\
				</form>\
			</body>\
		</html>";

		 // Creates a file to wrap the constant string above.
		File* f = new MemFile((char*)html, true);

		if(!f) // If the file could not be created.
		{
			return INTERNAL_SERVER_ERROR_500;
		}

		Template* t = new Template(f); // Create a template to hold the file.

		if(!t) // If the template could not be created.
		{
			delete f; // Delete the allocated file.
			return INTERNAL_SERVER_ERROR_500;
		}

		// Create a checked string for the input tag.
		char* c = (char*)malloc(strlen("checked=\"checked\"") + 1);
		strcpy(c, "checked=\"checked\"");

		// Adds an argument for the message.
		char* msg = (char*)malloc(strlen(message) + 1);
		strcpy(msg, message);
		t->add_arg(msg);

		// Add the check argument to the correct input depending on the state.
		if(!state)
		{
			t->skip_argument();
		}
		t->add_arg(c);
		if(state)
		{
			t->skip_argument();
		}

		// Sets the body of the response.
		response->set_body(t, MIME::TEXT_HTML);

		return OK_200; // Done.
	}

	return sc; // If the parent resource processed the request.
}
