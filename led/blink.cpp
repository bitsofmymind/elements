/*
 * blink.cpp
 *
 *  Created on: 2010-11-05
 *      Author: antoine
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

File* Blinker::render( Request* request )
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

	char* val = (char*)ts_malloc(10);
	if(!val)
	{
		delete t;
		return NULL;
	}
	itoa(_interval, val , 10);
	t->add_arg( val, strlen(val) );

	char* c = (char*)ts_malloc(strlen("checked=\"checked\"") + 1);
	memcpy(c, "checked=\"checked\"", strlen("checked=\"checked\""));

	if(!state)
	{
		t->add_arg(NULL, 0);
	}
	t->add_arg( c, strlen("checked=\"checked\"") );
	if(state)
	{
		t->add_arg(NULL, 0);
	}

	return t;

}

Response::status_code Blinker::process( Request* request, Message** return_message )
{
	Response::status_code sc = Resource::process(request, return_message);

	if(sc == NOT_IMPLEMENTED_501)
	{
		if(!strcmp(request->method, "post"))
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

			*return_message = http_get( request );
			(*return_message)->content_type = "text/html";
		}
	}

	return sc;
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

/*SweepPWM::SweepPWM(float interval, uint8_t pin):
	Resource(),
	_interval(interval),
	_pin(_BV(pin))
{
	DDRB |= _BV(pin);
	position = 0;
	counter = 0;
	period = 1;
	off = false;
}

void SweepPWM::run(void)
{
	if(off)
	{
		PORTB &= ~_pin;
		schedule(20 - period);
		off = false;
	}
	else
	{
		PORTB |= _pin;
		schedule(period);
		off = true;
	}
	position += period;
	period = (uint8_t)((position / _interval) * 20.0 ) + 1;

	if(period >= 20)
	{
		period = 1;
		position = 0;
	}
}*/
