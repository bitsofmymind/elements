/* pal.h - Header file the platform abstraction layer.
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

#ifndef PAL_H_
#define PAL_H_

#include <stdint.h>
#include <configuration.h>
#include <stdlib.h>

/**
 * @file
 * This file defines the prototypes for certain core functions used
 * by the framework to interact with the platform it is being executed on.
 * PAL stands for Platform Abstraction Layer so in effect, what is defined
 * here abstracts the platform.
 * */

/**
 * A thread safe version of malloc().
 * This function can only be located in the header file, otherwise the
 * program is not correctly compiled.
 * @param __size the size of the block to allocate.
 * @return a pointer to the memory block of 0 if there is not enough memory
 * to complete the request.
 * */
static void* thread_safe_malloc(size_t __size)
{
	ATOMIC
	{
		return malloc(__size);
	}
}

/**
 * A thread safe version of free().
 * This function can only be located in the header file, otherwise the
 * program is not correctly compiled.
 * @param __ptr the pointer to free.
 * */
static void thread_safe_free(void* __ptr)
{
	ATOMIC
	{
		free(__ptr);
	}
}

/**
 * A thread safe version of calloc().
 * This function can only be located in the header file, otherwise the
 * program is not correctly compiled.
 * @param __nele the value to initialize the block to.
 * @param __size the size of the block.
 * @return a pointer to the allocated memory block or 0 if there is not enough
 * memory to complete the request.
 * */
static void* thread_safe_calloc(size_t __nele, size_t __size)
{
	ATOMIC
	{
		return calloc(__nele, __size);
	}
}

/**
 * A thread safe version of realloc().
 * This function can only be located in the header file, otherwise the
 * program is not correctly compiled.
 * @param __ptr a pointer to the block to reallocated.
 * @param __size the new size of the block.
 * @return a pointer to the reallocated memory block or 0 if there is not enough
 * memory to complete the request.
 * */
static void* thread_safe_realloc(void* __ptr, size_t __size)
{
	ATOMIC
	{
		return realloc(__ptr, __size);
	}
}

/** A redefine of free() that points to a thread safe version of the function.*/
#define free(ptr) thread_safe_free(ptr)

/** A redefine of malloc() that points to a thread safe version of the function.*/
#define malloc(size) thread_safe_malloc(size)

/** A redefine of calloc() that points to a thread safe version of the function.*/
#define calloc(nele, size) thread_safe_calloc(nele, size)

/** A redefine of realloc() that points to a thread safe version of the function.*/
#define realloc(ptr, size) thread_safe_realloc(ptr, size)

/**
 * Initializes the platform to make it ready for executing the
 * framework.
 * */
extern void init(void);

/**
 * Terminate the execution of the framework.
 * */
extern void terminate(void);

/*
 * Defines an uptime_t type for working with the framework's uptime.
 * */
#if UPTIME_BITS == 64
	#define MAX_UPTIME __UINT64_C(18446744073709551615) // Largest number that can fit in a 64 bit space.
	typedef uint64_t uptime_t;
#elif UPTIME_BITS == 32
	#define MAX_UPTIME 4294967295U // Largest number that can fit in a 32 bit space.
	typedef uint32_t uptime_t;
#elif UPTIME_BITS == 16
	#define MAX_UPTIME 65535 // Largest number that can fit in a 16 bit space.
	typedef uint16_t uptime_t;
#elif UPTIME_BITS == 8
	#define MAX_UPTIME 255 // Largest number that can fit in a 8 bit space.
	typedef uint8_t uptime_t;
#else
	#error "Illegal UPTIME_BITS number. This value should be one of 8, 16, 32 or 64."
#endif

/**
 * Different macros and constants to reduce some units of time to their
 * milliseconds equivalent used by the framework.
 */
#define ASAP 0
#define SECONDS(s) (s) * 1000
#define MINUTES(m) (m) * SECONDS(60)
#define HOURS(h) (h) * MINUTES(60)
#define DAYS(d) (d) * HOURS(24)
#define WEEKS(w) (w) * DAYS(7)
#define NEVER MAX_UPTIME

#if TIME_KEEPING

	/**
	 * @return the unix time in milliseconds.
	 * */
	extern uint64_t get_time( void );

	/**
	 * @param time sets the time in unix format but in milliseconds.
	 * */
	extern void set_time(uint64_t time);

#endif

/**
 * A macro to set the expiration time on a variable.
 * */
#define expire(var, time) var = get_uptime() + time

/**
 * A macro to check if a timer has expired.
 * */
#define is_expired(var) var <= get_uptime()

/**
 * Gets the uptime of the system.
 * @return the amount of milliseconds the system has been running.
 * */
uptime_t get_uptime(void);

/**
 * Increases the amount of time the system has been up and running by
 * a certain amount.
 * @param time the amount of time to increase the uptime to.
 * */
void increase_uptime(uptime_t time);

/**
 * Resumes processing from a sleep state.
 * */
extern void processing_wake();

/**
 * Halts processing for a certain amount of time.
 * @param time the amount of time to halt processing.
 * */
extern void processing_sleep(uptime_t time);

#if HEARTBEAT
/**
 * Does a heart beat. A heart beat is any mean to let a user know that the
 * framework is not stalled. It can be a flashing led, a sound, etc.
 * */
extern void heart_beat(void);
#endif

#if OUTPUT_ERRORS // If errors should be displayed.

/**
 * Prints an error character.
 * */
#define ERROR_PRINT(a) DEBUG_PRINT(a)

/**
 * Prints a number of error characters.
 * */
#define ERROR_NPRINT(a, l) DEBUG_NPRINT(a, l)

/**
 * Prints a numerical type as an error.
 * */
#define ERROR_TPRINT(a, t) DEBUG_TPRINT(a, t);

/**
 * Prints an error character followed by a new line.
 * */
#define ERROR_PRINTLN(a) DEBUG_PRINTLN(a);

/**
 * Prints a number of error characters followed by a new line..
 * */
#define ERROR_NPRINTLN(a, l) DEBUG_NPRINTLN(a, l);

/**
 * Prints a numerical type as an error followed by a new line.
 * */
#define ERROR_TPRINTLN(a, t) DEBUG_TPRINTLN(a, t);

#else // Errors are not displayed, blank the macros.

#define ERROR_PRINT(a)
#define ERROR_NPRINT(a, l)
#define ERROR_TPRINT(a, t)

#define ERROR_PRINTLN(a)
#define ERROR_NPRINTLN(a, l)
#define ERROR_TPRINTLN(a, t)

#endif

#if OUTPUT_WARNINGS // If warnings should be output.

/**
 * Prints a warning character.
 * */
#define WARNING_PRINT(a) DEBUG_PRINT(a)

/**
 * Prints a number of warning characters.
 * */
#define WARNING_NPRINT(a, l) DEBUG_NPRINT(a, l)

/**
 * Prints a numerical type as a warning.
 * */
#define WARNING_TPRINT(a, t) DEBUG_TPRINT(a, t);

/**
 * Prints a warning character followed by a new line.
 * */
#define WARNING_PRINTLN(a) DEBUG_PRINTLN(a);

/**
 * Prints a number of warning characters followed by a new line.
 * */
#define WARNING_NPRINTLN(a, l) DEBUG_NPRINTLN(a, l);

/**
 * Prints a numerical type as a warning followed by a new line.
 * */
#define WARNING_TPRINTLN(a, t) DEBUG_TPRINTLN(a, t);

#else // Warnings are not required, blank the macros.

#define WARNING_PRINT(a)
#define WARNING_NPRINT(a, l)
#define WARNING_TPRINT(a, t)

#define WARNING_PRINTLN(a)
#define WARNING_NPRINTLN(a, l)
#define WARNING_TPRINTLN(a, t)

#endif

#if VERBOSITY // If verbosity is required.

/**
 * Prints a verbose character.
 * */
#define VERBOSE_PRINT(a) DEBUG_PRINT(a)

/**
 * Prints a number of verbose characters.
 * */
#define VERBOSE_NPRINT(a, l) DEBUG_NPRINT(a, l)

/**
 * Prints a numerical type as verbose.
 * */
#define VERBOSE_TPRINT(a, t) DEBUG_TPRINT(a, t)

/**
 * Prints a verbose character followed by a new line.
 * */
#define VERBOSE_PRINTLN(a) DEBUG_PRINTLN(a);

/**
 * Prints a number of verbose characters followed by a new line..
 * */
#define VERBOSE_NPRINTLN(a, l) DEBUG_NPRINTLN(a, l);

/**
 * Prints a numerical type as verbose followed by a new line.
 * */
#define VERBOSE_TPRINTLN(a, t) DEBUG_TPRINTLN(a, t)

#else // Verbosity is not required, blank the macros.

#define VERBOSE_PRINT(a)
#define VERBOSE_NPRINT(a, l)
#define VERBOSE_TPRINT(a, t)

#define VERBOSE_PRINTLN(a)
#define VERBOSE_NPRINTLN(a, l)
#define VERBOSE_TPRINTLN(a, t)

#endif

// If any outputting to debug is required.
#if OUTPUT_ERRORS || OUTPUT_WARNINGS || VERBOSITY || OUTPUT_DEBUG

/** Decimal base. */
#define DEC (uint8_t)10
/** Hexadecimal base. */
#define HEX (uint8_t)16
/** Octal base. */
#define OCT (uint8_t)8
/** Binary base. */
#define BIN (uint8_t)2
/** Byte base. */
#define BYTE (uint8_t)0

/**
 * Prints a vdebug character.
 * */
#define DEBUG_PRINT(a) Debug::print(a);

/**
 * Prints a number of debug characters.
 * */
#define DEBUG_NPRINT(a, l) Debug::nprint(a, l);

/**
 * Prints a numerical type to debug.
 * */
#define DEBUG_TPRINT(a, t) Debug::print(a, t);

/**
 * Prints a debug character followed by a new line.
 * */
#define DEBUG_PRINTLN(a) Debug::println(a);

/**
 * Prints a number of debug characters followed by a new line..
 * */
#define DEBUG_NPRINTLN(a, l) Debug::nprintln(a, l);

/**
 * Prints a numerical type to debug followed by a new line.
 * */
#define DEBUG_TPRINTLN(a, t) Debug::println(a, t);

/**
 * This namespace defines printing functions used to debug the framework.
 * */
namespace Debug
{
	/**
	 * Prints a character to a platform specific debug output.
	 * More complex printing functions make use of this more basic one to send
	 * their formatted characters to the output.
	 * @param character the character to print.
	 * */
	extern void print_char(char character);

	/**
	 * Prints a new line on the platform specific debug output.
	 * */
	extern void println(void);

	/**
	 * Prints a string terminated by a null character.
	 * @param string the string to print.
	 * */
	void print(const char* string);

	/**
	 * Prints a singular character.
	 * @param character the character to print.
	 * */
	void print(char character);

	/**
	 * Prints a string over a defined length.
	 * @param str the string to print.
	 * @param length the number of characters to print.
	 * */
	void nprint(const char* str, uint16_t length);

	/**
	 * Prints an int8_t to the output.
	 * @param value the value to print.
	 * @param base the base to print the value in.
	 * */
	void print(int8_t value, uint8_t base);

	/**
	 * Prints an uint8_t to the output.
	 * @param value the value to print.
	 * @param base the base to print the value in.
	 * */
	void print(uint8_t value, uint8_t base);

	/**
	 * Prints an int16_t to the output.
	 * @param value the value to print.
	 * @param base the base to print the value in.
	 * */
	void print(int16_t value, uint8_t base);

	/**
	 * Prints an uint16_t to the output.
	 * @param value the value to print.
	 * @param base the base to print the value in.
	 * */
	void print(uint16_t value, uint8_t base);

	/**
	 * Prints an int32_t to the output.
	 * @param value the value to print.
	 * @param base the base to print the value in.
	 * */
	void print(int32_t value, uint8_t base);

	/**
	 * Prints an uint32_t to the output.
	 * @param value the value to print.
	 * @param base the base to print the value in.
	 * */
	void print(uint32_t value, uint8_t base);

	/**
	 * Prints a string terminated by a null character followed by a new line.
	 * @param string the string to print.
	 * */
	void println(const char* string);

	/**
	 * Prints a singular character followed by a new line.
	 * @param character the character to print.
	 * */
	void println(char character);

	/**
	 * Prints a string over a defined length followed by a new line.
	 * @param str the string to print.
	 * @param length the number of characters to print.
	 * */
	void nprintln(const char* str, uint16_t length);

	/**
	 * Prints an int8_t to the output followed by a new line.
	 * @param value the value to print.
	 * @param base the base to print the value in.
	 * */
	void println(int8_t value, uint8_t base);

	/**
	 * Prints an uint8_t to the output followed by a new line.
	 * @param value the value to print.
	 * @param base the base to print the value in.
	 * */
	void println(uint8_t value, uint8_t base);

	/**
	 * Prints an int16_t to the output followed by a new line.
	 * @param value the value to print.
	 * @param base the base to print the value in.
	 * */
	void println(int16_t value, uint8_t base);

	/**
	 * Prints an uint16_t to the output followed by a new line.
	 * @param value the value to print.
	 * @param base the base to print the value in.
	 * */
	void println(uint16_t value, uint8_t base);

	/**
	 * Prints an int32_t to the output followed by a new line.
	 * @param value the value to print.
	 * @param base the base to print the value in.
	 * */
	void println(int32_t value, uint8_t base);

	/**
	 * Prints an uint32_t to the output followed by a new line.
	 * @param value the value to print.
	 * @param base the base to print the value in.
	 * */
	void println(uint32_t value, uint8_t base);
}

#else // No outputting is required, blank the macros.

#define DEBUG_PRINT(a)
#define DEBUG_NPRINT(a, l)
#define DEBUG_TPRINT(a, t)


#define DEBUG_PRINTLN(a)
#define DEBUG_NPRINTLN(a, l)
#define DEBUG_TPRINTLN(a, t)

#endif

#endif /* PAL_H_ */
