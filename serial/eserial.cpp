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
		body_started(false),
		received(0),
		newcomm(true)
{
	instance = this;

	index = 0;
	buffer = 0;

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
			index = received = buffer_size = 0;
			return;
		}
	}

	received++;
	buffer[index++] = c;

	schedule(ASAP);
}


void ESerial::run(void)
{
	uint16_t rec_alt;
	uint16_t len;

	bool rec = false;
	ATOMIC
	{
		rec_alt = received;
		len = index;
		received = 0;
	}

	while(rec_alt)
	{
		rec = true;

		if(len > 2
				&& buffer[len - rec_alt - 1 ] == ';'
				&& buffer[len - rec_alt] == ';'
				&& buffer[len - rec_alt - 2] != '\r' )
		{
			buffer[len - rec_alt-1 ] = '\r';
			buffer[len - rec_alt] = '\n';
		}

		if(len > 4
				&& buffer[len - rec_alt - 3] == '\r'
				&& buffer[len - rec_alt - 2] == '\n'
				&& buffer[len - rec_alt - 1] == '\r'
				&& buffer[len - rec_alt] == '\n')
		{
			char* message_buffer =  buffer;
			size_t message_len = index;

			if(rec_alt)
			{
				ATOMIC
				{
					buffer = (char*)ts_malloc(rec_alt + received + MESSAGE_BUFFER_INCREMENT_SIZE);
					if(buffer)
					{
						index = received + rec_alt;
						buffer_size = received + MESSAGE_BUFFER_INCREMENT_SIZE + rec_alt;
						memcpy(buffer, message_buffer + message_len, received + rec_alt);
						//print("remaining: "); println(buffer.text, buffer.length);
					}
					else{ received = index = buffer_size = 0; }
				}

			}
			else
			{
				ATOMIC
				{
					received = index = buffer_size = 0;
				}
			}

			Request* request = new Request();
			if(!request)
			{
				//Debug::println("not enough space");
			}
			Message::PARSER_RESULT res = request->parse(message_buffer, message_len);
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
			ts_free(message_buffer);
		}
		rec_alt--;
	}

	if(rec)
	{
		timeout = get_uptime() + 100;
		schedule(100);
	}
	else if(timeout <= get_uptime())
	{
		//Debug::println("timed out");
		ATOMIC
		{
			if(buffer && index){ ts_free(buffer); }
			buffer = 0;
			received = index = buffer_size = 0;
		}
		schedule(NEVER);
		newcomm = true;
	}

}

Response::status_code ESerial::process(Response* response, Message** return_message)
{
	File* body = response->body_file;
	print_transaction(response);

	char buffer[10];
	if(body)
	{
		uint8_t read;
		do
		{
			read = body->read(buffer, 10);
			Debug::print(buffer, read);
		} while(read == 10);

		Debug::println();
	}

	delete response;

	return OK_200;
}

ISR(USART_RX_vect)
{
	instance->receive(UDR0);
}



