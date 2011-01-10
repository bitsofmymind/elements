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

static ESerial* instance;

ESerial::ESerial(uint16_t baud):
		Resource(),
		buffer_size(0),
		body_started(false),
		received(0)
{
	instance = this;

	buffer.length = 0;

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

	println("Waiting...");

}

void ESerial::receive(uint8_t c)
{
	if(buffer.length >= buffer_size)
	{
		char* new_buffer = (char*)malloc(buffer_size + MESSAGE_BUFFER_INCREMENT_SIZE);
		if(new_buffer != NULL)
		{
			memcpy(new_buffer, buffer.text, buffer_size);
			buffer_size += MESSAGE_BUFFER_INCREMENT_SIZE;
			free(buffer.text);
			buffer.text = new_buffer;
		}
		else
		{
			buffer.length = 0;
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

	bool rec = false;

	while(received)
	{
		rec = true;
		cli();
		received--;

		if(buffer.length > 1
				&& buffer.text[buffer.length-1 ] == ';'
				&& buffer.text[buffer.length] == ';'  )
		{
			buffer.text[buffer.length-1 ] = '\r';
			buffer.text[buffer.length] = '\n';
		}

		if(buffer.length > 4
				&& buffer.text[buffer.length - 3] == '\r'
				&& buffer.text[buffer.length - 2] == '\n'
				&& buffer.text[buffer.length - 1] == '\r'
				&& buffer.text[buffer.length] == '\n')
		{
			Elements::string<uint16_t> message = {buffer.text, buffer.length};

			if(received != 0)
			{
				buffer.text = (char*)malloc(received + MESSAGE_BUFFER_INCREMENT_SIZE);
				if(buffer.text)
				{
					buffer.length = received;
					buffer_size = received + MESSAGE_BUFFER_INCREMENT_SIZE;
					memcpy(buffer.text, message.text + message.length, received);
				}
				else{ received = buffer.length = buffer_size = 0; }
			}
			else
			{
				received = buffer.length = buffer_size = 0;
			}

			sei();

			Request* request = new Request();
			request->message.length = message.length;
			request->message.text = message.text;
			request->deserialize(message, message.text);
			send(request);
		}

		sei();
	}

	if(rec)
	{
		timeout = get_uptime() + 100;
		schedule(100);
	}
	else if(timeout <= get_uptime())
	{
		println("timed out");
		cli();
		if(buffer.length){ free(buffer.text); }
		received = buffer.length = buffer_size = 0;
		sei();
		schedule(NEVER);
	}

}

Message* ESerial::process(Response* response)
{

	delete response;
	buffer.text = (char*)malloc(100);

	return NULL;
}

ISR(USART_RX_vect)
{
	instance->receive(UDR0);
}

void printNumber(uint32_t n, uint8_t base)
{
  unsigned char buf[8 * sizeof(int32_t)]; // Assumes 8-bit chars.
  uint32_t i = 0;

  if (n == 0) {
	  instance->write('0');
    return;
  }

  while (n > 0) {
    buf[i++] = n % base;
    n /= base;
  }

  for (; i > 0; i--)
	  instance->write((char) (buf[i - 1] < 10 ?
      '0' + buf[i - 1] :
      'A' + buf[i - 1] - 10));
}

void print(const char* str)
{
	while(*str)
	{
		instance->write(*str++);
	}
}

void print(const char* str, uint16_t length)
{
	while(length--)
	{
		instance->write(*str++);
	}
}

void print(char c, uint8_t base)
{
  print((int32_t) c, base);
}

void print(uint8_t b, uint8_t base)
{
  print((uint32_t) b, base);
}

void print(int16_t n, uint8_t base)
{
  print((int32_t) n, base);
}

void print(uint16_t n, uint8_t base)
{
  print((uint32_t) n, base);
}

void print(int32_t n, uint8_t base)
{
  if (base == 0) {
    instance->write(n);
  } else if (base == 10) {
    if (n < 0) {
      print('-');
      n = -n;
    }
    printNumber(n, 10);
  } else {
    printNumber(n, base);
  }
}

void print(uint32_t n, uint8_t base)
{
  if (base == 0) { instance->write(n); }
  else { printNumber(n, base); }
}

/*void Print::print(double n, int digits)
{
  printFloat(n, digits);
}*/

void println(void)
{
  print('\r');
  print('\n');
}

void println(const char* c)
{
  print(c);
  println();
}

void println(const char* str, uint16_t length)
{
	print(str, length);
	println();
}

void println(char c, uint8_t base)
{
  print(c, base);
  println();
}

void println(unsigned char b, uint8_t base)
{
  print(b, base);
  println();
}

void Pprintln(int n, uint8_t base)
{
  print(n, base);
  println();
}

void println(unsigned int n, uint8_t base)
{
  print(n, base);
  println();
}

void println(long n, uint8_t base)
{
  print(n, base);
  println();
}

void println(unsigned long n, uint8_t base)
{
  print(n, base);
  println();
}

/*void Print::println(double n, int digits)
{
  print(n, digits);
  println();
}*/


/*void Print::printFloat(double number, uint8_t digits)
{
  // Handle negative numbers
  if (number < 0.0)
  {
     print('-');
     number = -number;
  }

  // Round correctly so that print(1.999, 2) prints as "2.00"
  double rounding = 0.5;
  for (uint8_t i=0; i<digits; ++i)
    rounding /= 10.0;

  number += rounding;

  // Extract the integer part of the number and print it
  unsigned long int_part = (unsigned long)number;
  double remainder = number - (double)int_part;
  print(int_part);

  // Print the decimal point, but only if there are digits beyond
  if (digits > 0)
    print(".");

  // Extract digits from the remainder one at a time
  while (digits-- > 0)
  {
    remainder *= 10.0;
    int toPrint = int(remainder);
    print(toPrint);
    remainder -= toPrint;
  }
}*/
