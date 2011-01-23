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

ESerial::ESerial(uint16_t baud):
		Resource(),
		buffer_size(0),
		body_started(false),
		received(0),
		newcomm(true)
{
	instance = this;

	buffer.length = 0;
	buffer.text = 0;

	uint16_t baud_setting;
	bool use_u2x;

	// U2X mode is needed for baud rates higher than (CPU Hz / 16)
	if (baud > F_CPU / 16) {
	use_u2x = true;
	} else {
	// figure out if U2X mode would allow for a better connection

	// calculate the percent difference between the baud-rate specified and
	// the real baud rate for both U2X and non-U2X mode (0-255 error percent)
	uint8_t nonu2x_baud_error = abs((int)(255-((F_CPU/(16*(((F_CPU/8/baud-1)/2)+1))*255)/baud)));
	uint8_t u2x_baud_error = abs((int)(255-((F_CPU/(8*(((F_CPU/4/baud-1)/2)+1))*255)/baud)));

	// prefer non-U2X mode because it handles clock skew better
	use_u2x = (nonu2x_baud_error > u2x_baud_error);
	}

	if (use_u2x) {
	UCSR0A = _BV(U2X0);
	baud_setting = (F_CPU / 4 / baud - 1) / 2;
	} else {
	UCSR0A = 0;
	baud_setting = (F_CPU / 8 / baud - 1) / 2;
	}

	// assign the baud_setting, a.k.a. ubbr (USART Baud Rate Register)
	UBRR0H = baud_setting >> 8;
	UBRR0L = baud_setting;

	UCSR0B |= _BV(RXCIE0) + _BV(TXEN0) + _BV(RXEN0);

	Debug::println("Waiting...");

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


void ESerial::write(uint8_t c)
{
  loop_until_bit_is_set(UCSR0A, UDRE0);
  UDR0 = c;
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
				Debug::println("not enough space");
			}

			request->message.length = message.length;
			request->message.text = message.text;

			Debug::println("msg");
			int8_t res = request->deserialize(message, message.text);

			if(res)
			{
				Debug::print("parsing failed ");
				delete request;
				Debug::println(res, DEC);
			}
			else
			{
				Debug::println("sent");
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
		Debug::println("timed out");
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

Message* ESerial::process(Response* response)
{
	Resource::process(response);
	char buffer[10];
	string<MESSAGE_SIZE> body;
	body.length = 10;
	body.text = buffer;
	response->body_file->open();
	Debug::println("rec");
	MESSAGE_SIZE read;
	do
	{
		read = response->body_file->read(&body, false);
		Debug::print(body.text, read);
	} while(read == 10);

	Debug::println();
	delete response;

	return NULL;
}

ISR(USART_RX_vect)
{
	instance->receive(UDR0);
}

void Debug::print(char c)
{
	instance->write(c);
}
void Debug::println()
{
	print("\r");
	print("\n");
}


