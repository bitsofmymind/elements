/*
 * serial.cpp
 *
 *  Created on: 2010-11-07
 *      Author: antoine
 */

#include "eserial.h"
#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr_pal.h>
#include <string.h>

static ESerial* instance;

ESerial::ESerial():
		Resource(),
		buffer_size(0),
		index(0),
		buffer(0),
		age(0)
{
	instance = this;


}

void ESerial::receive(uint8_t c)
{
	if(index >= buffer_size)
	{
		char* new_buffer = (char*)ts_malloc(buffer_size + MESSAGE_BUFFER_INCREMENT_SIZE);
		if(new_buffer != NULL)
		{
			if(buffer)
			{
				memcpy(new_buffer, buffer, buffer_size);
				ts_free(buffer);
			}
			buffer_size += MESSAGE_BUFFER_INCREMENT_SIZE;
			buffer = new_buffer;
		}
		else
		{
			if(buffer)
			{
				ts_free(buffer);
			}
			buffer = NULL;
			index = buffer_size = 0;
			return;
		}
	}

	age = get_uptime();

	buffer[index++] = c;

	schedule(ASAP);
}


void ESerial::run(void)
{
	size_t len;
	char* buf;

	ATOMIC
	{
		if(age + MAX_AGE > get_uptime())
		{
			schedule(10);
			return;
		}
		len = index;
		buf = buffer;
		index = buffer_size = 0;
		buffer = NULL;
	}

	Request* request = new Request();
	if(!request)
	{
		//Debug::println("not enough space");
	}

	for(size_t i = 1; i < len; i++)
	{
		if( buf[i-1] == ';' && buf[i] == ';')
		{
			buf[i-1] = '\r';
			buf[i] = '\n';
		}
	}

	Message::PARSER_RESULT res = request->parse(buf, len);
	switch(res)
	{
		case Message::PARSING_COMPLETE:
			VERBOSE_PRINTLN_P("Parsing complete, sending");
			send(request);
			break;
		case Message::PARSING_SUCESSFUL:
			VERBOSE_PRINTLN_P("Parsing incomplete");
		default:
			VERBOSE_PRINTLN_P("Parsing Error");
			delete request;
	}

	ts_free(buf);

	ATOMIC
	{
		if(index)
		{
			schedule(ASAP);
			return;
		}
	}
	schedule(NEVER);
}

Response::status_code ESerial::process(Response* response, Message** return_message)
{
	File* body = response->get_body();
	print_transaction(response);

	char buffer[10];
	if(body)
	{
		uint8_t read;
		do
		{
			read = body->read(buffer, 10);
			DEBUG_TPRINT(buffer, read);
		} while(read == 10);

		DEBUG_PRINTLN();
	}

	delete response;

	return OK_200;
}

ISR(USART_RX_vect)
{
	instance->receive(UDR0);
}



