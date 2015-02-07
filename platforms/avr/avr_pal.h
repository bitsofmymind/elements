/* avr_pal.h - Platform abstraction layer for the AVR ATMEGA architecture.
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

#ifndef AVR_PAL_H_
#define AVR_PAL_H_

#include <pal/pal.h>
#include <stdlib.h>
#include <avr/pgmspace.h>

/**
 * Define the *new* operator for use on the ATMEGA platform.
 * @param size the size of the object to allocated.
 * @return a pointer to the allocated memory. NULL if allocation failed.
 * */
void * operator new(size_t size);

/**
 * Define the *delete* operator for use on the ATMEGA platform.
 * @param ptr the object to delete (free).
 * */
void operator delete(void * ptr);

/// A function that is called on a virtual method call.
extern "C" void __cxa_pure_virtual();

/// Define the guard attribute.
__extension__ typedef int __guard __attribute__((mode (__DI__)));

// Static initialization functions.
//see http://monoinfinito.wordpress.com/2013/12/03/static-initialization-in-c/

/**
 * Acquire a guard for a static variable.
 * @param the guard to the variable.
 * @return if the acquisition was a success.
 * */
extern "C" int __cxa_guard_acquire(__guard *);

/**
 * Release the guard to a variable.
 * @param the guard to the variable.
 * */
extern "C" void __cxa_guard_release (__guard *);

/**
 * Abort the acquisition of a guard to a variable.
 * @param the guard to the variable.
 * */
extern "C" void __cxa_guard_abort (__guard *);

/// The baud rate for the UART.
#define BAUD_RATE 9600

/// Initialize the platform.
void init(void);

#if OUTPUT_ERRORS // If the output of errors to the debug console is enabled.
	#define ERROR_PRINT_P(a) DEBUG_PRINT_P(a)
	#define ERROR_NPRINT_P(a, l) DEBUG_NPRINT_P(a, l)
	#define ERROR_PRINTLN_P(a) DEBUG_PRINTLN_P(a)
	#define ERROR_NPRINTLN_P(a, l) DEBUG_NPRINTLN_P(a, l)
	#else
	#define ERROR_PRINT_P(a) // Intentionally defined to nothing.
	#define ERROR_NPRINT_P(a, l) // Intentionally defined to nothing.
	#define ERROR_PRINTLN_P(a) // Intentionally defined to nothing.
	#define ERROR_NPRINTLN_P(a, l) // Intentionally defined to nothing.
#endif

#if OUTPUT_WARNINGS // If the output of warnings to the debug console is enabled.
	#define WARNING_PRINT_P(a) DEBUG_PRINT_P(a)
	#define WARNING_NPRINT_P(a, l) DEBUG_NPRINT_P(a, l)
	#define WARNING_PRINTLN_P(a) DEBUG_PRINTLN_P(a)
	#define WARNING_NPRINTLN_P(a, l) DEBUG_NPRINTLN_P(a, l)
	#else
	#define WARNING_PRINT_P(a) // Intentionally defined to nothing.
	#define WARNING_NPRINT_P(a, l) // Intentionally defined to nothing.
	#define WARNING_PRINTLN_P(a) // Intentionally defined to nothing.
	#define WARNING_NPRINTLN_P(a, l) // Intentionally defined to nothing.
#endif

#if VERBOSITY // If the output of message to the debug console is enabled.
	#define VERBOSE_PRINT_P(a) DEBUG_PRINT_P(a)
	#define VERBOSE_NPRINT_P(a, l) DEBUG_NPRINT_P(a, l)
	#define VERBOSE_PRINTLN_P(a) DEBUG_PRINTLN_P(a)
	#define VERBOSE_NPRINTLN_P(a, l) DEBUG_NPRINTLN_P(a, l)
#else
	#define VERBOSE_PRINT_P(a) // Intentionally defined to nothing.
	#define VERBOSE_NPRINT_P(a, l) // Intentionally defined to nothing.
	#define VERBOSE_PRINTLN_P(a) // Intentionally defined to nothing.
	#define VERBOSE_NPRINTLN_P(a, l) // Intentionally defined to nothing.
#endif

// If any output is defined.
#if OUTPUT_ERRORS || OUTPUT_WARNINGS || VERBOSITY || OUTPUT_DEBUG

	// Define output macros that use the program memory to store strings.
	#define DEBUG_PRINT_P(a)  Debug_P::print(PSTR(a));
	#define DEBUG_NPRINT_P(a, l)  Debug_P::print(PSTR(a), l);
	#define DEBUG_PRINTLN_P(a) Debug_P::println(PSTR(a));
	#define DEBUG_NPRINTLN_P(a, l) Debug_P::println(PSTR(a), l);

	namespace Debug_P
	{
		/**
		 * Print a string to the console.
		 * @param the string to print.
		 * */
		void print(const char*);

		/**
		 * Print a string of a certain length to the console.
		 * @param the string to print.
		 * @param length the length of string to print.
		 * */
		void print(const char* str, uint16_t length);

		/**
		 * Print a string followed by a new line to the console.
		 * @param the string to print.
		 * */
		void println(const char*);

		/**
		 * Print a string of a certain length followed by a new line to the console.
		 * @param the string to print.
		 * @param length the length of string to print.
		 * */
		void println(const char* str, uint16_t length);
	}
#else
	#define DEBUG_PRINT_P(a) // Intentionally defined to nothing.
	#define DEBUG_NPRINT_P(a, l) // Intentionally defined to nothing.
	#define DEBUG_PRINTLN_P(a) // Intentionally defined to nothing.
	#define DEBUG_NPRINTLN_P(a, l) // Intentionally defined to nothing.
#endif

#endif /* AVR_PAL_H_ */
