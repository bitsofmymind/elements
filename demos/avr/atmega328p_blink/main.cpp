/* main.cpp - main file for the blink demo
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

#include "configuration.h"
#include <core/resource.h>
#include <core/processing.h>
#include <platforms/avr/led/blink.h>
#include <pal/pal.h>
#include <platforms/avr/avr_pal.h>
#include <platforms/avr/serial/eserial.h>
#include <avr/io.h>
/**
 * This program creates a simple system that blinks the onboard led on an
 * arduino development board and provides serial access to itself.
 * */
int main(void)
{
	init(); // Initialize the framework.

	Processing processing(NULL); // The processing resource.
	Blinker blinker1(100, PINB5, &DDRB, &PORTB);
	ESerial eserial; // A serial interface to the framework.

	// Add all the resources as child of the processing resource.
	processing.add_child("blinker", &blinker1);
	processing.add_child("serial", &eserial);

	processing.start(); // Start the oscilloscope.
}
