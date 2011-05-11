/*
 * authority.cpp
 *
 *  Created on: Feb 17, 2009
 *      Author: Antoine
 */




#include "processing.h"
//#include "../elements.h"
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
			if(heartbeat < get_uptime())
			{
				heart_beat();
				heartbeat = get_uptime() + 1000;
			}
		#endif
	}
}




