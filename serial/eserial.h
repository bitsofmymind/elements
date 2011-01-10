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
#include <pal/pal.h>
#include <core/request.h>

class ESerial: public Resource
{

	#define MESSAGE_BUFFER_INCREMENT_SIZE 20
	Elements::string<uint8_t> buffer;
	uint16_t buffer_size;
	uint16_t content_length;
	volatile uint16_t received;
	bool body_started;

	uptime_t timeout;

	public:
		ESerial(uint16_t baud);

		void receive(uint8_t c);
		void write(uint8_t c);

	protected:
		void run(void);
		Message* process(Response* response);

};

#define DEC 10
#define HEX 16
#define OCT 8
#define BIN 2
#define BYTE 0

void print(const char*);
void print(const char* str, uint16_t length);
void print(char, uint8_t = BYTE);
void print(uint8_t, uint8_t = BYTE);
void print(int16_t, uint8_t = DEC);
void print(uint16_t, uint8_t = DEC);
void print(int32_t, uint8_t= DEC);
void print(uint32_t, uint8_t = DEC);
//void print(double, int = 2);

void println(const char*);
void println(const char* str, uint16_t length);
void println(char, uint8_t = BYTE);
void println(uint8_t, uint8_t = BYTE);
void println(int16_t, uint8_t = DEC);
void println(uint16_t, uint8_t = DEC);
void println(int32_t, uint8_t= DEC);
void println(uint32_t, uint8_t = DEC);
//void println(double, int = 2);
void println(void);

#endif /* ESERIAL_H_ */
