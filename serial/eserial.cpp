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
#include <pal/pal.h>

static ESerial* instance;

ESerial::ESerial():
		Resource(),
		buffer_size(0),
		body_started(false),
		received(0),
		newcomm(true)
{
	instance = this;

	buffer.length = 0;
	buffer.text = 0;

}

void ESerial::receive(uint8_t c)
{
	if(buffer.length >= buffer_size)
	{
		char* new_buffer = (char*)ts_malloc(buffer_size + MESSAGE_BUFFER_INCREMENT_SIZE);
		if(new_buffer != NULL)
		{
			if(buffer.text)
			{
				memcpy(new_buffer, buffer.text, buffer_size);
				ts_free(buffer.text);
			}
			buffer_size += MESSAGE_BUFFER_INCREMENT_SIZE;
			buffer.text = new_buffer;
		}
		else
		{
			if(buffer.text)
			{
				ts_free(buffer.text);
			}
			buffer.text = NULL;
			buffer.length = received = buffer_size = 0;
			return;
		}
	}

	received++;
	buffer.text[buffer.length++] = c;

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
		len = buffer.length;
		received = 0;
	}

	while(rec_alt)
	{
		rec = true;

		if(len > 2
				&& buffer.text[len - rec_alt - 1 ] == ';'
				&& buffer.text[len - rec_alt] == ';'
				&& buffer.text[len - rec_alt - 2] != '\r' )
		{
			buffer.text[len - rec_alt-1 ] = '\r';
			buffer.text[len - rec_alt] = '\n';
		}

		if(len > 4
				&& buffer.text[len - rec_alt - 3] == '\r'
				&& buffer.text[len - rec_alt - 2] == '\n'
				&& buffer.text[len - rec_alt - 1] == '\r'
				&& buffer.text[len - rec_alt] == '\n')
		{
			Elements::string<uint16_t> message = {buffer.text, len};

			if(rec_alt)
			{
				ATOMIC
				{
					buffer.text = (char*)ts_malloc(rec_alt + received + MESSAGE_BUFFER_INCREMENT_SIZE);
					if(buffer.text)
					{
						buffer.length = received + rec_alt;
						buffer_size = received + MESSAGE_BUFFER_INCREMENT_SIZE + rec_alt;
						memcpy(buffer.text, message.text + message.length, received + rec_alt);
						//print("remaining: "); println(buffer.text, buffer.length);
					}
					else{ received = buffer.length = buffer_size = 0; }
				}

			}
			else
			{
				ATOMIC
				{
					received = buffer.length = buffer_size = 0;
				}
			}

			Request* request = new Request();
			if(!request)
			{
				//Debug::println("not enough space");
			}

			request->message.length = message.length;
			request->message.text = message.text;

			//Debug::println("msg");

			if(request->deserialize(message, message.text))
			{

				Debug::print("Parsing failed");

				delete request;
			}
			else
			{

				Debug::println("Request sent");

				send(request);
			}

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
			if(buffer.text && buffer.length){ ts_free(buffer.text); }
			buffer.text = 0;
			received = buffer.length = buffer_size = 0;
		}
		schedule(NEVER);
		newcomm = true;
	}

}

Response::status_code ESerial::process(Response* response, Message** return_message)
{

	Debug::print(response->response_code_int, DEC);
	Debug::print("  "); //2 spaces because we do not display the reason phrase
	Debug::print("HTTP/");
	Debug::println(response->http_version, DEC);


	char buffer[10];
	string<MESSAGE_SIZE> body;
	body.length = 10;
	body.text = buffer;
	if(response->body_file)
	{
		response->body_file->open();

		MESSAGE_SIZE read;
		do
		{
			read = response->body_file->read(body.text, body.length, false);
			Debug::print(body.text, read);
		} while(read == 10);

		Debug::println();
	}
	response->body_file->close();
	delete response;

	return OK_200;
}

ISR(USART_RX_vect)
{
	instance->receive(UDR0);
}



