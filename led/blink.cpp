/* blink.cpp - Implements a resource for blinking leds
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
#include <utils/pgmspace_file.h>
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
	*ddr |= _BV(pin);
	schedule( _interval );
}

void Blinker::run(void)
{
	if(*_port & _pin)
	{
		*_port &= ~_pin;
	}
	else if(state)
	{
		*_port |= _pin;
	}
	schedule( _interval );
}

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
#define CONTENT_SIZE sizeof(CONTENT)

static char content_P[] PROGMEM = CONTENT;

File* Blinker::http_get( void )
{
	File* f = new PGMSpaceFile(content_P, CONTENT_SIZE);
	if(!f)
	{
		return NULL;
	}
	Template* t = new Template(f);
	if(!t)
	{
		delete f;
		return NULL;
	}

	t->add_narg(_interval);

	char* c = (char*)ts_malloc(strlen("checked=\"checked\"") + 1);
	memcpy(c, "checked=\"checked\"", strlen("checked=\"checked\""));

	if(!state)
	{
		t->add_arg(NULL);
	}
	t->add_arg(c);
	if(state)
	{
		t->add_arg(NULL);
	}

	return t;

}

Response::status_code Blinker::process( Request* request, File** return_body, const char** mime )
{
	if(request->is_method(Request::POST))
	{
		char buffer[8];
		uint8_t len = request->find_arg("i", buffer, 7);
		if(len)
		{
			buffer[len] = '\0';
			_interval = atoi(buffer);
		}

		len = request->find_arg("st", buffer, 1);

		if(len)
		{
			if(buffer[0] == '1')
			{
				state = true;
			}
			else if(buffer[0] == '0')
			{
				state = false;
			}
		}
		goto get;
	}
	else if(request->is_method(Request::GET))
	{
		get:
		File* f = http_get();
		if(!f)
		{
			return INTERNAL_SERVER_ERROR_500;
		}
		*return_body = f;
		*mime = MIME::TEXT_HTML;
		return OK_200;
	}

	return NOT_IMPLEMENTED_501;
}


BusyBlinker::BusyBlinker(uint32_t interval, uint8_t pin):
	Resource(),
	_interval(interval),
	_pin(_BV(pin)),
	counter(interval)
{
	DDRB |= _BV(pin);
}

void BusyBlinker::run(void)
{
	schedule( 1 );

	if(get_uptime() > counter)
	{
		counter = get_uptime() + _interval;

		PINB = _pin;
	}
}
