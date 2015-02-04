/* blink.h - Implements a resource for blinking leds
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

#ifndef BLINK_H_
#define BLINK_H_

#include <core/resource.h>

/// Blinks a led at a given interval.
/** This resource blinks a led at a given interval. The interval can be modified
 * using an HTML form obtained by querying the resource.*/
class Blinker: public Resource
{
	/** The interval at which the LED should blink. */
	uint32_t _interval;

	/** The pin where the LED is on the microcontroller.*/
	uint8_t _pin;

	/** If the LED is on or off. */
	bool state;

	/** The port the LED is hooked to on the microcontroller. */
	volatile uint8_t* _port;

	public:

		/// Class constructor
		/**
		 * @param interval the interval in ms at which the LED should blink.
		 * @param pin the microcontroler pin to which the LED is hooked up.
		 * @param ddr the microcontroler ddr to which the LED is hooked up.
		 * @param port the microcontroler port to which the LED is hooked up.
		 * */
		Blinker(uint32_t interval, uint8_t pin, volatile uint8_t* ddr, volatile uint8_t* port);

		/// Runs the resource.
		virtual void run(void);

		/// Returns the HTML page for this resource.
		/**
		 * @return the HTML for this resource.
		 * */
		File* http_get(void);

        /// Process a request message.
        /**
         * Override of parent implementation to present the client with the LED
         * blink control form.
         * @param request the request to process.
         * @param response the response to fill if a response should be returned (which
         * depends on the status code).
         * @return the status_code produced while processing the request.
         */
		virtual Response::status_code process(const Request* request, Response* response);
};

/// Blinks a led at a given interval.
/** This resource blinks a led at a given interval. As opposed to the Blinker
 * class, which is meant to run only when the LED needs to switch states, this
 * resource runs every millisecond to check if the LED should be toggled. The
 * purpose of this resource is to create a load on the framework. */
class BusyBlinker: public Resource
{
	/** The interval at which the LED should blink. */
	uint32_t _interval;

	/** The next time the LED needs to be blinked. */
	uint32_t counter;

	/** The pin where the LED is on the microcontroller.*/
	uint8_t _pin;

	public:

		/// Class constructor
		/**
		 * @param interval the interval in ms at which the LED should blink.
		 * @param pin the microcontroler pin to which the LED is hooked up.
		 * */
		BusyBlinker(uint32_t interval, uint8_t pin);

		/// Runs the resource.
		void run(void);
};

#endif /* BLINK_H_ */
