// SVN FILE: $Id: $
/**
 * @lastChangedBy           $lastChangedBy: Mercier $
 * @revision                $Revision: 397 $
 * @copyright    			GNU General Public License
 * 		This program is free software: you can redistribute it and/or modify
 * 		it under the terms of the GNU General Public License as published by
 * 		the Free Software Foundation, either version 3 of the License, or
 * 		(at your option) any later version.
 * 		This program is distributed in the hope that it will be useful,
 * 		but WITHOUT ANY WARRANTY; without even the implied warranty of
 *		MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * 		GNU General Public License for more details.
 * 		You should have received a copy of the GNU General Public License
 * 		along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * Class file for the Processing class.
 */

//INCLUDES
#include <pal/pal.h>
#include "processing.h"

///Implements a Processing resource.
/**
 * Processing objects are responsible for running an Elements tree. They are
 * hooked to an arbitrary node in the resource tree and from that point
 * traverse all other nodes (up to a set bound) and run then one at the time.
 * A tree can incorporate a virtually infinite number of Processing objects
 * but if there is more than one present, a mechanism for concurrency control
 * must be active.
 *
 * Processing can also be set to act as Authorities.
 * @class Processing
 * @see configuration.h/PROCESSING_AS_AUTHORITY
 * */

#if PROCESSING_AS_AUTHORITY
///Processing class constructor.
Processing::Processing(Resource* bound): Authority(),
#else
Processing::Processing(Resource* bound): Resource(),
#endif
		bound(bound),
		current(this) //A Processing object starts by running itself.
#if HEARTBEAT
		,heartbeat(1000)
#endif
{}

///Steps trough a processing run.
/** A processing run consists of running the current Resource and
 * then getting the Resource that should be run next.
 * @todo this method sould be protected.
 * */
void Processing::step(void)
{
	//If the current resource is scheduled to be run.
	if(is_expired(current->own_sleep_clock))
	{
		current->own_sleep_clock = NEVER; //Reset its sleep clock.
		current->run(); //Run it.
	}
	//Get the next Resource to be run.
	Resource* to_run = current->get_next_child_to_visit();

	if(to_run) //If a Resource was returned.
	{
		current = to_run; //Set to be run next.
	}
	/*Else no resource was returned. Go to the parent resource if it is not
	 * the bound.*/
	else if(current->parent != bound)
	{
		current = current->parent; //Set it to be run next.
	}
	//Else we are one resource below the bound. Stay there.
}

///Starts a processing object's activity.
/** This method does not return and can be considered the main program loop.*/
void Processing::start(void)
{
	for(;;)
	{
		step(); //Step through a resource.
		if(current == bound) //If the bound has been reached.
		{
			/*The sleep clock for the current resource gives us the next
			 * time processing will be needed again.
			 * Since the sleep clock is an uptime, is is substracted with
			 * the uptime to get the actual amount of time.*/
			uptime_t sleep_amount = current->get_sleep_clock() - get_uptime();
			/*TODO: Since uptime_t are unsigned int, sleep amount will
			alway be positive. See #181. */
			if(sleep_amount > 0) //If that amount is more than 0.
			{
				//Make that processing element sleep.
				processing_sleep( sleep_amount );
			}
		}

#if HEARTBEAT //If heartbeat function is activated.
		if(is_expired(heartbeat)) //If the heart beat timer has expired.
		{
			heart_beat(); //Do a heart beat.
			//Set the heartbeat timer to expire in 1 second.
			expire(heartbeat, 1000);
		}
#endif
	}
}
