/*
 * blink.cpp
 *
 *  Created on: 2010-11-05
 *      Author: antoine
 */

#include "blink.h"
#include <avr/io.h>
#include <pal/pal.h>

Blinker::Blinker(uint32_t interval, uint8_t pin, volatile uint8_t* ddr, volatile uint8_t* port):
	Resource(),
	_interval(interval),
	_pin(_BV(pin)),
	_port(port)
{
	*ddr |= _BV(pin);
}

void Blinker::run(void)
{
	schedule( _interval );
	if(*_port & _pin)
	{
		*_port &= ~_pin;
	}
	else
	{
		*_port |= _pin;
	}
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
