/* processing.cpp - Source file for the Processing class.
 * Copyright (C) 2015 Antoine Mercier-Linteau
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

//INCLUDES
#include <pal/pal.h>
#include "processing.h"

#if PROCESSING_AS_AUTHORITY
Processing::Processing(Resource* bound): Authority(),
#else
Processing::Processing(Resource* bound): Resource(),
#endif
		_bound(bound),
		_current(this) //A Processing object starts by running itself.
#if HEARTBEAT
		,_heartbeat(1000)
#endif
{}

void Processing::step(void)
{
	//If the current resource is scheduled to be run.
	if(is_expired(_current->_own_sleep_clock))
	{
		_current->_own_sleep_clock = NEVER; //Reset its sleep clock.
		_current->run(); //Run it.
	}
	//Get the next Resource to be run.
	Resource* to_run = _current->_get_next_child_to_visit();

	if(to_run) //If a Resource was returned.
	{
		_current = to_run; //Set to be run next.
	}
	/*Else no resource was returned. Go to the parent resource if it is not
	 * the bound.*/
	else if(_current->_parent != _bound)
	{
		_current = _current->_parent; //Set it to be run next.
	}
	//Else we are one resource below the bound. Stay there.
}

void Processing::start(void)
{
	for(;;)
	{
		step(); //Step through a resource.

		if(_current->_parent == _bound) //If the bound has been reached.
		{
			/*The sleep clock for the current resource gives us the next
			 * time processing will be needed again.
			 * Since the sleep clock is an uptime, is is substracted with
			 * the uptime to get the actual amount of time.*/
			uptime_t sleep_amount = _current->get_sleep_clock() - get_uptime();

			/*TODO: Since uptime_t are unsigned int, sleep amount will
			alway be positive. See #181. */
			if(sleep_amount > 0) //If that amount is more than 0.
			{
				//Make that processing element sleep.
				processing_sleep(sleep_amount);
			}
		}

#if HEARTBEAT //If heartbeat function is activated.
		if(is_expired(_heartbeat)) //If the heart beat timer has expired.
		{
			heart_beat(); //Do a heart beat.
			//Set the heartbeat timer to expire in 1 second.
			expire(_heartbeat, 1000);
		}
#endif
	}
}
