/* blink.cpp - Source file for the Blinker and BusyBlinker classes.
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

#include "blink.h"
#include <avr/io.h>
#include <pal/pal.h>
#include "../utils/pgmspace_file.h"
#include <stdlib.h>
#include <utils/template.h>
#include <string.h>

Blinker::Blinker(uint32_t interval, uint8_t pin, volatile uint8_t* ddr, volatile uint8_t* port):
	Resource(),
	_interval(interval),
	_pin(_BV(pin)),
	_port(port),
	state(true)
{
	*ddr |= _BV(pin); // Set the data direction register to output.

	// Schedule the resource to run at the end of the interval.
	schedule(_interval);
}

void Blinker::run(void)
{
	if(*_port & _pin) // Can't remember what that does.
	{
		*_port &= ~_pin; // Can't remember what that does.
	}
	else if(state) // If the LED is on.
	{
		*_port |= _pin; // Switch the state of the LED.
	}

	// Schedule the resource to run at the end of the interval.
	schedule(_interval);
}

/** The content of the web page.*/
#define CONTENT \
"<html>\
	<body>\
		<h2>Blinker resource</h2>\
		You can use the following form to adjust the blinking interval in milliseconds of the LED and turn it on or off.</br>\
		<br/>\
		<form method=\"post\">\
			<b>Interval</b>: <input type=\"text\" name=\"i\" value=\"~\"/><br/>\
			ON<input type=\"radio\" name=\"st\" ~ value=\"1\"/><br/>\
			OFF<input type=\"radio\" name=\"st\" ~ value=\"0\"/><br/>\
			<input type=\"submit\" value=\"Submit\" />\
		</form>\
	</body>\
</html>"

/** The size in bytes of the web page. */
#define CONTENT_SIZE sizeof(CONTENT)

/** Set the content of the web page as a program memory string. */
static const char content_P[] PROGMEM = CONTENT;

File* Blinker::http_get(void)
{
	// Create a program space file for the web page's content.
	File* f = new PGMSpaceFile(content_P, CONTENT_SIZE);

	if(!f) // If a file could not be created.
	{
		return NULL;
	}

	// Create a template to fill the program space file with data.
	Template* t = new Template(f);

	if(!t) // If memory for the template could not be allocated.
	{
		delete f; // Delete the file.
		return NULL;
	}

	// Add the interval as an number argument.
	t->add_narg(_interval);

	// Allocate a buffer containing the stated of the LED.
	char* c = (char*)ts_malloc(strlen("checked=\"checked\"") + 1);
	memcpy(c, "checked=\"checked\"", strlen("checked=\"checked\"") + 1);

	if(!state) // If blinking is off.
	{
		t->skip_argument(); // Skip the next argument.
	}
	t->add_arg(c); // Add the state of the led as an argument.

	if(state) // If blinking is on.
	{
		t->skip_argument(); // Skip the next argument.
	}

	return t;
}

Response::status_code Blinker::process(const Request* request, Response* response)
{
	if(request->is_method("post")) // If this is a post request.
	{
		char buffer[8]; // Allocate a buffer to contain the form data.

		// Find the interval argument.
		uint8_t len = request->find_arg("i", buffer, 7);

		if(len) // If the interval value was found.
		{
			buffer[len] = '\0'; // End it with a null character.
			_interval = atoi(buffer); // Set the new interval value.
		}

		// Find the state argument.
		len = request->find_arg("st", buffer, 1);

		if(len) // If the state value was found.
		{
			if(buffer[0] == '1') // If the state is ON.
			{
				state = true;
			}
			else if(buffer[0] == '0') // If the state is OFF.
			{
				state = false;
			}
		}

		goto get; // Return the form.
	}
	else if(request->is_method("get")) // If this is a get request.
	{
		get:
		File* f = http_get(); // Create a file to contain the body of the response.
		if(!f) // If the body could not be allocated.
		{
			return Response::INTERNAL_SERVER_ERROR_500; // Error 500.
		}

		// Set the body of the response.
		response->set_body(f, "text/html");

		return Response::OK_200; // Done.
	}

	return Resource::process(request, response);
}


BusyBlinker::BusyBlinker(uint32_t interval, uint8_t pin):
	Resource(),
	_interval(interval),
	_pin(_BV(pin)),
	counter(interval)
{
	DDRB |= _BV(pin);  // Set the data direction register to output.
}

void BusyBlinker::run(void)
{
	schedule(1); // Wait 1 ms.

	if(get_uptime() > counter) // If the LED needs to be toggled.
	{
		 // Set the counter to the next time the LED needs to be toggled.
		counter = get_uptime() + _interval;

		PINB = _pin; // Switch the LED's state.
	}
}
