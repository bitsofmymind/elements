/*
 * serial.h
 *
 *  Created on: 2010-11-07
 *      Author: antoine
 */

#ifndef ESERIAL_H_
#define ESERIAL_H_

#include <core/resource.h>
#include <utils/types.h>
#include <core/request.h>


class ESerial: public Resource
{
	Elements::string<uint8_t> buffer;
	uint16_t content_length;
	bool body_started;
	bool wait_for_response;
	Request* request;

	public:
		ESerial(void);

	protected:
		void run(void);
		Message* process(Response* response);
};

#endif /* ESERIAL_H_ */
