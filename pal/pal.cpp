/*
 * pal.c
 *
 *  Created on: May 23, 2009
 *      Author: Antoine
 */

#include "pal.h"
#include <stdint.h>
#include <stdlib.h>

/*formatedTime get_formated_time( void )
{

}*/


#if TIME_KEEPING

	uint64_t system_time = 0;

	uint64_t get_time( void )
	{
		//time_in_milliseconds += uptime_in_milliseconds;
		return system_time;
	}

	void set_time(uint64_t time)
	{
		system_time = time;
	}

#endif

uptime_t system_uptime = 0;

uptime_t get_uptime( void )
{
	uptime_t val;
	ATOMIC
	{
		val = system_uptime;
	}

	return val;
}

void increase_uptime(uptime_t time)
{
	system_uptime += time;
}

/*This is the Interrupt handling function for time keeping, it should be vectored to every millisecond.*/



void* ts_malloc(size_t size)
{
	void* block;
	ATOMIC
	{
		block = malloc(size);
	}

	if(!block)
	{
		//This should be optimized away by the compiler when ERROR_OUTPUT is defined to 0
		ERROR_PRINTLN("Malloc failed!");
	}

	return block;
}

void ts_free(void* block)
{
	if(block != NULL)
	{
		ATOMIC
		{
			free(block);
		}
	}
}

void* ts_realloc(void* ptr, size_t size)
{
	void* block;
	ATOMIC
	{
		block = realloc(ptr, size);
	}
	return block;
}

/*
It would be a great idea to build a bunch of functions for elements to register with interrupts, EEPROM adresses and system registers. The problem, there is no memory security
( at least on the AVR) to begin with and keeping track of who owns what memory appears to be too resource intensive for now.
bool register_interrupt_handler( void ( *handler_pointer )( void ), void* vector_address )
{

}

bool unregister_interrupt_handler( void ( *handler_pointer )( void ), void* vector_address )
{
	for( unsigned int i = VECTORS_BOTTOM; i < VECTORS_TOP ; i += sizeof( void * )
	{
		if( *i == vector_address )
		{
			*i = 0;
			//Deactivate interrupt
		}
	}
	return false;
}
*/
#if OUTPUT_ERRORS || OUTPUT_WARNING || VERBOSITY || OUTPUT_DEBUG
void printNumber(uint32_t n, uint8_t base)
{
  unsigned char buf[8 * sizeof(int32_t)]; // Assumes 8-bit chars.
  uint32_t i = 0;

  if (n == 0) {
	  Debug::print_char('0');
    return;
  }

  while (n > 0) {
    buf[i++] = n % base;
    n /= base;
  }

  for (; i > 0; i--)
	  Debug::print_char((char) (buf[i - 1] < 10 ?
      '0' + buf[i - 1] :
      'A' + buf[i - 1] - 10));
}

void Debug::print(const char* str)
{
	while(*str)
	{
		Debug::print_char((char)(*str++));
	}
}

void Debug::print(const char* str, uint16_t length)
{
	while(length--)
	{
		Debug::print(*str++);
	}
}

void Debug::print(char c, uint8_t base)
{
	Debug::print((int32_t) c, base);
}

void Debug::print(uint8_t b, uint8_t base)
{
	Debug::print((uint32_t) b, base);
}

void Debug::print(int16_t n, uint8_t base)
{
	Debug::print((int32_t) n, base);
}

void Debug::print(uint16_t n, uint8_t base)
{
	Debug::print((uint32_t) n, base);
}

void Debug::print(int32_t n, uint8_t base)
{
  if (base == 0) {
	  Debug::print_char(n);
  } else if (base == 10) {
    if (n < 0) {
    	Debug::print_char('-');
      n = -n;
    }
    printNumber(n, 10);
  } else {
    printNumber(n, base);
  }
}

void Debug::print(uint32_t n, uint8_t base)
{
  if (base == 0) { Debug::print_char(n); }
  else { printNumber(n, base); }
}

/*void Print::print(double n, int digits)
{
  printFloat(n, digits);
}*/

void Debug::println(const char* str)
{
	Debug::print(str);
	Debug::println();
}

void Debug::println(const char* str, uint16_t length)
{
	Debug::print(str, length);
	Debug::println();
}

void Debug::println(char c, uint8_t base)
{
	Debug::print(c, base);
	Debug::println();
}

void Debug::println(unsigned char b, uint8_t base)
{
  print(b, base);
  println();
}

void Debug::println(int16_t n, uint8_t base)
{
	Debug::print(n, base);
	Debug::println();
}

void Debug::println(uint16_t n, uint8_t base)
{
	Debug::print(n, base);
	Debug::println();
}

void Debug::println(int32_t n, uint8_t base)
{
	Debug::print(n, base);
	Debug::println();
}

void Debug::println(uint32_t n, uint8_t base)
{
	Debug::print(n, base);
	Debug::println();
}
#endif
