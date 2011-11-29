/* blink.h - Implements a resource for blinking leds
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
#ifndef BLINK_H_
#define BLINK_H_

#include <core/resource.h>

class Blinker: public Resource
{
	uint32_t _interval;
	uint8_t _pin;
	bool state;
	volatile uint8_t* _port;

	public:
		Blinker(uint32_t interval, uint8_t pin, volatile uint8_t* ddr, volatile uint8_t* port);
		virtual void run(void);
		File* http_get( void );
		virtual Response::status_code process( Request* request, Response* response );

};

class BusyBlinker: public Resource
{
	uint32_t _interval;
	uint32_t counter;
	uint8_t _pin;

	public:
		BusyBlinker(uint32_t interval, uint8_t pin);
	void run(void);
};

/*class SweepPWM: public Resource
{
	float _interval;
	float position;
	uint8_t period;
	uint8_t counter;
	uint8_t _pin;
	bool off;

	public:
		SweepPWM(float interval, uint8_t pin);
	void run(void);
};*/

#endif /* BLINK_H_ */
