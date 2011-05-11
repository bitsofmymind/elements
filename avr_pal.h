/*
 * pal.h
 *
 *  Created on: 2010-11-07
 *      Author: antoine
 */

#ifndef AVR_PAL_H_
#define AVR_PAL_H_


#include <pal/pal.h>
#include <stdlib.h>
#include <avr/pgmspace.h>

void * operator new(size_t size);
void operator delete(void * ptr);

__extension__ typedef int __guard __attribute__((mode (__DI__)));

extern "C" void __cxa_pure_virtual();

extern "C" int __cxa_guard_acquire(__guard *);
extern "C" void __cxa_guard_release (__guard *);
extern "C" void __cxa_guard_abort (__guard *);

#define BAUD_RATE 9600

void init(void);

#if OUTPUT_ERRORS
#define ERROR_PRINT_P(a) DEBUG_PRINT_P(a)
#define ERROR_NPRINT_P(a, l) DEBUG_NPRINT_P(a, l)
#define ERROR_PRINTLN_P(a) DEBUG_PRINTLN_P(a)
#define ERROR_NPRINTLN_P(a, l) DEBUG_NPRINTLN_P(a, l)
#else
#define ERROR_PRINT_P(a)
#define ERROR_NPRINT_P(a, l)
#define ERROR_PRINTLN_P(a)
#define ERROR_NPRINTLN_P(a, l)
#endif

#if OUTPUT_WARNINGS
#define WARNING_PRINT_P(a) DEBUG_PRINT_P(a)
#define WARNING_NPRINT_P(a, l) DEBUG_NPRINT_P(a, l)
#define WARNING_PRINTLN_P(a) DEBUG_PRINTLN_P(a)
#define WARNING_NPRINTLN_P(a, l) DEBUG_NPRINTLN_P(a, l)
#else
#define WARNING_PRINT_P(a)
#define WARNING_NPRINT_P(a, l)
#define WARNING_PRINTLN_P(a)
#define WARNING_NPRINTLN_P(a, l)
#endif

#if VERBOSITY
#define VERBOSE_PRINT_P(a) DEBUG_PRINT_P(a)
#define VERBOSE_NPRINT_P(a, l) DEBUG_NPRINT_P(a, l)
#define VERBOSE_PRINTLN_P(a) DEBUG_PRINTLN_P(a)
#define VERBOSE_NPRINTLN_P(a, l) DEBUG_NPRINTLN_P(a, l)
#else
#define VERBOSE_PRINT_P(a)
#define VERBOSE_NPRINT_P(a, l)
#define VERBOSE_PRINTLN_P(a)
#define VERBOSE_NPRINTLN_P(a, l)
#endif

#if OUTPUT_ERRORS || OUTPUT_WARNINGS || VERBOSITY || OUTPUT_DEBUG

#define DEBUG_PRINT_P(a)  Debug_P::print(PSTR(a));
#define DEBUG_NPRINT_P(a, l)  Debug_P::print(PSTR(a), l);
#define DEBUG_PRINTLN_P(a) Debug_P::println(PSTR(a));
#define DEBUG_NPRINTLN_P(a, l) Debug_P::println(PSTR(a), l);

namespace Debug_P
{
	void print(const prog_char*);
	void print(const prog_char* str, uint16_t length);

	void println(const prog_char*);
	void println(const prog_char* str, uint16_t length);

}
#else

#define DEBUG_PRINT_P(a)
#define DEBUG_PRINTLN_P(a)

#endif

#endif /* AVR_PAL_H_ */
