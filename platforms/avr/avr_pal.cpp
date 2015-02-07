/* avr_pal.cpp - Platform abstraction layer for the AVR architecture.
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

// Timer configuration.

/// The prescaler value for the system timer.
#define TIMER2_PRESCALER 256 //This will give use a period of 8ms precisely

/// The output compare value value for the system timer.
#define TIMER2_OUTPUT_COMPARE 250

/// The number of ms it takes for timer 2 to overflow.
#define MS_TO_TIMER2_OVERFLOW 4 //1000 / (F_CPU /(TIMER2_PRESCALER * TIMER2_OUTPUT_COMPARE))

/// This variable is used to wake up from sleep when processing_wake() is called.
static bool volatile wake_up = false;

void processing_wake()
{
	wake_up = true;
}

void processing_sleep(uptime_t time)
{
	// Compute the time at which the system needs to wake up.
	uptime_t time_to_wake = get_uptime() + time;

	set_sleep_mode(SLEEP_MODE_IDLE); // Set the sleep mode.

	/* If we are not done sleeping, go back to sleep. It's possible that the
	 * was awaken by the increase_uptime event or by another interrupt routine
	 * that does not need to call the framework. */
	/// TODO This condition is weird, it should be the opposite.
	while(time_to_wake > get_uptime() && !wake_up)
	{
		// Enable interrupts just in case something forgot to turn them back on.
		sei();
		sleep_enable(); // Enable sleep.
		sleep_cpu(); // ZZZZZZZZZZZzzzzzzzzz...........
		sleep_disable(); // Just woke up, disable sleep.
	}

	wake_up = false;
}

void init(void)
{

	// Configure the timer 2.

	TCCR2A |= _BV(WGM21); // Timer clear on output match of OCR2A.
	TCCR2B |= _BV(CS22) + _BV(CS21); // Prescaler at 256.
	TIMSK2 |= _BV(OCIE2A); // Interrupt on overflow.
	// Nothing to set for TIFR2.
	// Nothing to set for ASSR.

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

	// U2X mode is needed for baud rates higher than (CPU Hz / 16).
	if(BAUD_RATE > F_CPU / 16)
	{
		use_u2x = true;
	}
	else
	{
		// TODO Figure out if U2X mode would allow for a better connection.

		/* Calculate the percent difference between the baud-rate specified and
		  *the real baud rate for both U2X and non-U2X mode (0-255 error percent) */
		uint8_t nonu2x_baud_error = abs((int)(255-((F_CPU/(16*(((F_CPU/8/BAUD_RATE-1)/2)+1))*255)/BAUD_RATE)));
		uint8_t u2x_baud_error = abs((int)(255-((F_CPU/(8*(((F_CPU/4/BAUD_RATE-1)/2)+1))*255)/BAUD_RATE)));

		// Prefer non-U2X mode because it handles clock skew better.
		use_u2x = (nonu2x_baud_error > u2x_baud_error);
	}

	if(use_u2x) // If U2X should be used.
	{
		UCSR0A = _BV(U2X0);
		baud_setting = (F_CPU / 4 / BAUD_RATE - 1) / 2;
	}
	else // Do not use U2X.
	{
		UCSR0A = 0;
		baud_setting = (F_CPU / 8 / BAUD_RATE - 1) / 2;
	}

	// Assign the baud_setting.
	UBRR0H = baud_setting >> 8;
	UBRR0L = baud_setting;

	UCSR0B |= _BV(RXCIE0) + _BV(TXEN0) + _BV(RXEN0);

#endif

	VERBOSE_PRINTLN_P("Ready...");

#if HEARTBEAT // If the framework should output an heart beat.
	DDRD |= _BV(DDD7); // Configure the data direction register for the pin.
#endif

	sei(); // Enable interrupts.
}

void terminate(void)
{
	// Nothing to for termination.
}

#if HEARTBEAT  // If the framework should output an heart beat.
void heart_beat(void)
{
	PIND = _BV(PIN7); // Toggle the heart beat pin.
}
#endif

ISR(TIMER2_COMPA_vect)//, ISR_NOBLOCK)
{
	// Interrupts deactivated by hardware.
	increase_uptime(MS_TO_TIMER2_OVERFLOW);
	// Interrupts reactivated by compiler.
}

/*ISR(TIMER2_COMPB_vect)
{

	if(_SFR_MEM16(0x5D) < stack_pointer)
	{
		stack_pointer = _SFR_MEM16(0x5D);
	}
}*/

#if OUTPUT_ERRORS || OUTPUT_WARNING || VERBOSITY || OUTPUT_DEBUG
//Should be able to deactivate from a avr implementation specific config file.

void Debug::print_char(char c)
{
	// Wait until the previous character has been sent.
	loop_until_bit_is_set(UCSR0A, UDRE0);
	UDR0 = c; // Load the UART data register with the character to send.
	// Sending starts automatically.
}

void Debug::println()
{
	/// TODO Call print_char instead.
	print("\r");
	print("\n");
}

void Debug_P::print(const char* str)
{
	char c; // The current character.

	do
	{
		c = pgm_read_byte(str++); // Read the current char from the program memory.
		Debug::print((char)c); // Print the character.
	} while(c); // While the character to send is not a null character.
}
void Debug_P::print(const char* str, uint16_t length)
{
	char c; // The current character.

	while(length--) // While we have not reached the length.
	{
		c = pgm_read_byte(str++); // Read the current char from the program memory.
		Debug::print(c); // Print the character.
	}
}
void Debug_P::println(const char* str)
{
	Debug_P::print(str);
	Debug::println();
}

void Debug_P::println(const char* str, uint16_t length)
{
	Debug_P::print(str, length);
	Debug::println();
}
#endif
