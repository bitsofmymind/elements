/*
 * pal.cpp
 *
 *  Created on: 2010-11-05
 *      Author: antoine
 */

#include "avr_pal.h"
#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/sleep.h>
#include <stdlib.h>

void __cxa_pure_virtual(void){};

int __cxa_guard_acquire(__guard *g) {return !*(char *)(g);};
void __cxa_guard_release (__guard *g) {*(char *)g = 1;};
void __cxa_guard_abort (__guard *) {};

void* operator new(size_t size)
{
	return ts_malloc(size);
}

void operator delete(void * ptr)
{
	ts_free(ptr);
}


//This will give use a period of 8ms precisely
#define TIMER2_PRESCALER 256
#define TIMER2_OUTPUT_COMPARE 250

#define MS_TO_TIMER2_OVERFLOW 4//1000 / (F_CPU /(TIMER2_PRESCALER * TIMER2_OUTPUT_COMPARE))

static bool volatile wake_up = false;

void processing_wake()
{
	wake_up = true;
}
void processing_sleep(uptime_t time)
{
	uptime_t time_to_wake = get_uptime() + time;

	set_sleep_mode(SLEEP_MODE_IDLE);

	while(time_to_wake < get_uptime() || wake_up)
	{
		sei();
		sleep_enable();
		sleep_cpu();
		sleep_disable();
	}

	wake_up = false;
}

void init(void)
{


	TCCR2A |= _BV(WGM21); //Timer clear on output match of OCR2A
	TCCR2B |= _BV(CS22) + _BV(CS21); //Prescaler at 256
	TIMSK2 |= _BV(OCIE2A); //Interrupt on overflow
	//Nothing to set for TIFR2
	//Nothing to set for ASSR

	GTCCR |= _BV(PSRASY); //Clears the prescaler register
	TCNT2 = 0; //Clears the timer
	OCR2A = TIMER2_OUTPUT_COMPARE;
	OCR2B = 1;

	/*TCCR2 |= _BV(WGM21); //Timer clear on output match of OCR2A
	TCCR2 |= _BV(CS22) + _BV(CS21); //Prescaler at 256
	TIMSK |= _BV(OCIE2); //Interrupt on overflow
	//Nothing to set for TIFR2
	//Nothing to set for ASSR

	//GTCCR |= _BV(PSRASY); //Clears the prescaler register
	TCNT2 = 0; //Clears the timer
	OCR2 = TIMER2_OUTPUT_COMPARE;*/
#if OUTPUT_ERRORS || OUTPUT_WARNING || VERBOSITY || OUTPUT_DEBUG
	uint16_t baud_setting;
	bool use_u2x;

	// U2X mode is needed for baud rates higher than (CPU Hz / 16)
	if (BAUD_RATE > F_CPU / 16) {
	use_u2x = true;
	} else {
	// figure out if U2X mode would allow for a better connection

	// calculate the percent difference between the baud-rate specified and
	// the real baud rate for both U2X and non-U2X mode (0-255 error percent)
	uint8_t nonu2x_baud_error = abs((int)(255-((F_CPU/(16*(((F_CPU/8/BAUD_RATE-1)/2)+1))*255)/BAUD_RATE)));
	uint8_t u2x_baud_error = abs((int)(255-((F_CPU/(8*(((F_CPU/4/BAUD_RATE-1)/2)+1))*255)/BAUD_RATE)));

	// prefer non-U2X mode because it handles clock skew better
	use_u2x = (nonu2x_baud_error > u2x_baud_error);
	}

	if (use_u2x) {
	UCSR0A = _BV(U2X0);
	baud_setting = (F_CPU / 4 / BAUD_RATE - 1) / 2;
	} else {
	UCSR0A = 0;
	baud_setting = (F_CPU / 8 / BAUD_RATE - 1) / 2;
	}

	// assign the baud_setting, a.k.a. ubbr (USART Baud Rate Register)
	UBRR0H = baud_setting >> 8;
	UBRR0L = baud_setting;

	UCSR0B |= _BV(RXCIE0) + _BV(TXEN0) + _BV(RXEN0);
#endif

	VERBOSE_PRINTLN_P("Ready...");

#if HEARTBEAT
	DDRD |= _BV(DDD7);
#endif

	sei();
}

#if HEARTBEAT
void heart_beat(void)
{
	PIND = _BV(PIN7);
}
#endif

ISR(TIMER2_COMPA_vect)//, ISR_NOBLOCK)
{
	//Interrupts deactivated by hardware
	increase_uptime(MS_TO_TIMER2_OVERFLOW);
	//Interrupts reactivated by compiler
}

/*ISR(TIMER2_COMPB_vect)
{

	if(_SFR_MEM16(0x5D) < stack_pointer)
	{
		stack_pointer = _SFR_MEM16(0x5D);
	}
}*/

#if OUTPUT_ERRORS || OUTPUT_WARNING || VERBOSITY || OUTPUT_DEBUG
//Should be able to deactivate from a avr implementation specific config file

void Debug::print_char(char c)
{
	  loop_until_bit_is_set(UCSR0A, UDRE0);
	  UDR0 = c;
}

void Debug::println()
{
	print("\r");
	print("\n");
}

void Debug_P::print(const prog_char* str)
{
	char c;
	do
	{
		c = pgm_read_byte(str++);
		Debug::print((char)c);
	}while(c);
}
void Debug_P::print(const prog_char* str, uint16_t length)
{
	char c;
	while(length--)
	{
		c = pgm_read_byte(str++);
		Debug::print(c);
	}
}
void Debug_P::println(const prog_char* str)
{
	Debug_P::print(str);
	Debug::println();
}

void Debug_P::println(const prog_char* str, uint16_t length)
{
	Debug_P::print(str, length);
	Debug::println();
}
#endif
