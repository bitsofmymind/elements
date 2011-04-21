/*
 * serial.h
 *
 *  Created on: 2010-11-07
 *      Author: antoine
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
