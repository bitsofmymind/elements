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
#include "serial/HardwareSerial.h"
#include <stdlib.h>

void __cxa_pure_virtual(void){};

int __cxa_guard_acquire(__guard *g) {return !*(char *)(g);};
void __cxa_guard_release (__guard *g) {*(char *)g = 1;};
void __cxa_guard_abort (__guard *) {};

void * operator new(size_t size)
{
    void* temp = malloc(size);
    int sz = size;
    Serial.print("new ");
    Serial.print(sz, HEX);
    Serial.print(" ");
    Serial.println((uint16_t)temp);

	return temp;
}

void operator delete(void * ptr)
{
	Serial.print("del ");
	free(ptr);
	Serial.println((uint16_t)ptr);
}


//This will give use a period of 8ms precisely
#define TIMER2_PRESCALER 256
#define TIMER2_OUTPUT_COMPARE 250

#define MS_TO_TIMER2_OVERFLOW 4//1000 / (F_CPU /(TIMER2_PRESCALER * TIMER2_OUTPUT_COMPARE))

static bool volatile wake_up = false;
volatile uint16_t stack_pointer = RAMEND;

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
	Serial.begin(9600);

	TCCR2A |= _BV(WGM21); //Timer clear on output match of OCR2A
	TCCR2B |= _BV(CS22) + _BV(CS21); //Prescaler at 256
	TIMSK2 |= _BV(OCIE2A) + _BV(OCIE2B); //Interrupt on overflow
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

	sei();
}

ISR(TIMER2_COMPA_vect)//, ISR_NOBLOCK)
{
	//Interrupts deactivated by hardware
	increase_uptime(MS_TO_TIMER2_OVERFLOW);
	//Interrupts reactivated by compiler
}

ISR(TIMER2_COMPB_vect)
{

	if(_SFR_MEM16(0x5D) < stack_pointer)
	{
		stack_pointer = _SFR_MEM16(0x5D);
	}
}
