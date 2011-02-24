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
			Interval: <input type=\"text\" name=\"interval\" value=\"500\"/><br/>\
			ON<input type=\"radio\" name=\"state\" value=\"on\"/><br/>\
			OFF<input type=\"radio\" name=\"state\" value=\"off\"/><br/>\
			<input type=\"submit\" value=\"Submit\" />\
		</form>\
	</body>\
</html>"
#define CONTENT_SIZE sizeof(CONTENT)

char content_P[] PROGMEM = CONTENT;

File<MESSAGE_SIZE>* Blinker::render( Request* request )
{
	return new PGMSpaceFile(content_P, CONTENT_SIZE);
}

Response::status_code Blinker::process( Request* request, Message** return_message )
{
	Response::status_code sc = Resource::process(request, return_message);
	if(sc == NOT_IMPLEMENTED_501)
	{
		if(request->methodcmp("post", 4))
		{
			string<uint8_t>* value;

			{
				string<uint8_t> key = MAKE_STRING("state");
				value = request->to_url->arguments->find(key);
			}
			if(value)
			{
				if(value->text[0] == 'o' && value->text[1] == 'n')
				{
					state = true;
				}
				else if(value->text[0] == 'o' && value->text[1] == 'f' && value->text[2] == 'f')
				{
					state = false;
				}
			}
			{
				string<uint8_t> key = MAKE_STRING("interval");
				value = request->to_url->arguments->find(key);
			}
			if(value)
			{
				_interval = atoi(value->text);
			}
			*return_message = http_get( request );
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
