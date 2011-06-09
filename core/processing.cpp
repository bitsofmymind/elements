/* processing.h - Implements a basic processing resource
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

#include "processing.h"
#include "../utils/utils.h"
#include "resource.h"
#include "../pal/pal.h"
#include <stdint.h>
#if PROCESSING_AS_AUTHORITY
Processing::Processing(Resource* bound): Authority(),
#else
Processing::Processing(Resource* bound): Resource(),
#endif
		bound(bound),
		current(this)
#if HEARTBEAT
		,heartbeat(1000)
#endif
{

}


void Processing::step(void)
{
	current->visit();

	Resource* to_run = current->get_next_child_to_visit();
	if(to_run)
	{
		current = to_run;
	}
	else if(current->parent != bound)
	{
		current = current->parent;
	}

}

void Processing::start(void)
{
	for(;;)
	{
		step();
		if(current == bound)
		{
			uptime_t sleep_amount = current->get_sleep_clock() - get_uptime();
			if(sleep_amount > 0)
			{
				processing_sleep( sleep_amount );
			}
		}
		#if HEARTBEAT
			if(is_expired(heartbeat))
			{
				heart_beat();
				expire(heartbeat, 1000);
			}
		#endif
	}
}




