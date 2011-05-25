/*
 * blink.h
 *
 *  Created on: 2010-11-05
 *      Author: antoine
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
		virtual Response::status_code process( Request* request, File** return_body, const char** mime );

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
