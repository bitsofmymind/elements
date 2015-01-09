/* pal.cpp - Implements an abstract platform abstraction layer
 * Copyright (C) 2011 Antoine Mercier-Linteau
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
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
	/* On 32 or 64 bit systems, retrieving the uptime is done in a singular
	 * operation. However, one 16 bit and 8 bit architectures, it takes more
	 * so in order to prevent the uptime from changing during the call
	 * due to an interrupt updating it, its retrival is made atomic. */
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

	/* To prevent corruption of the heap due to concurrent access between two
	 * threads or by an interrupt routine, this call is made atomic.*/
	ATOMIC
	{
		block = malloc(size);
	}

	if(!block)
	{
		//This should be optimized away by the compiler when ERROR_OUTPUT is defined to 0.
		ERROR_PRINTLN("Malloc failed!");
	}

	return block;
}

void ts_free(void* block)
{
	if(block != NULL) // Makes sure we are not freeing a NULL pointer.
	{
		/* To prevent corruption of the heap due to concurrent access between two
		 * threads or by an interrupt routine, this call is made atomic.*/
		ATOMIC
		{
			free(block);
		}
	}
}

void* ts_realloc(void* ptr, size_t size)
{
	void* block;

	/* To prevent corruption of the heap due to concurrent access between two
	 * threads or by an interrupt routine, this call is made atomic.*/
	ATOMIC
	{
		block = realloc(ptr, size);
	}

	return block;
}

#if OUTPUT_ERRORS || OUTPUT_WARNING || VERBOSITY || OUTPUT_DEBUG

/** Prints a number using an arbitrary base.
 * @param n the number to print.
 * @param base the base ro print the number in.*/
void printNumber(uint32_t n, uint8_t base)
{
	unsigned char buf[8 * sizeof(int32_t)]; // Assumes 8-bit chars.
	uint32_t i = 0;

	if (n == 0) // If the number is 0.
	{
	  Debug::print_char('0'); // 0 is valid in every base.
	  return;
	}

	while (n > 0) // Divides the number into its digits.
	{
		buf[i++] = n % base;
		n /= base;
	}

	for (; i > 0; i--) // For each digit in the number.
	{
		// Prints a digit using numerical digits below 10 and letters above.
		Debug::print_char((char) (buf[i - 1] < 10 ?
		  '0' + buf[i - 1] :
		  'A' + buf[i - 1] - 10));
	}
}

void Debug::print(const char* str)
{
	while(*str) // While we have not reached the end of the string.
	{
		// Prints the current character.
		Debug::print_char((char)(*str++));
	}
}

void Debug::print(char c)
{
	Debug::print((int8_t)c, BYTE);
}

void Debug::nprint(const char* str, uint16_t length)
{
	while(length--) // Where there is still some characters to print.
	{
		// Prints the current character.
		Debug::print((int8_t)(*str++), BYTE);
	}
}

void Debug::print(int8_t n, uint8_t base)
{
	Debug::print((int32_t) n, base);
}

void Debug::print(uint8_t n, uint8_t base)
{
	Debug::print((uint32_t) n, base);
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
	if (base == 0) // If the value should be printed as a character.
	{
	    Debug::print_char(n);
	}
	else if (base == 10) // If the base is 10.
	{
		if (n < 0) // If the number is negative.
		{
			Debug::print_char('-'); // Add a minus sign.
			n = -n; // Make the number positive.
		}

		printNumber(n, 10); // Print the number.
	}
	else
	{
		printNumber(n, base); // Print the number using the required base.
	}
}

void Debug::print(uint32_t n, uint8_t base)
{
  if (base == 0) { Debug::print_char(n); }
  else { printNumber(n, base); }
}

void Debug::println(char c)
{
	Debug::print(c);
	Debug::println();
}

void Debug::println(const char* str)
{
	Debug::print(str);
	Debug::println();
}

void Debug::nprintln(const char* str, uint16_t length)
{
	Debug::nprint(str, length);
	Debug::println();
}

void Debug::println(int8_t n, uint8_t base)
{
	Debug::print(n, base);
	Debug::println();
}

void Debug::println(uint8_t n, uint8_t base)
{
  print(n, base);
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
