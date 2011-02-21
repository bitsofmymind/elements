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

void * operator new(size_t size);
void operator delete(void * ptr);

__extension__ typedef int __guard __attribute__((mode (__DI__)));

extern "C" void __cxa_pure_virtual();

extern "C" int __cxa_guard_acquire(__guard *);
extern "C" void __cxa_guard_release (__guard *);
extern "C" void __cxa_guard_abort (__guard *);

#define BAUD_RATE 9600

void init(void);

#endif /* AVR_PAL_H_ */
