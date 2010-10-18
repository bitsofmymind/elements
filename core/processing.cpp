/*
 * processing.cpp
 *
 *  Created on: Sep 21, 2009
 *      Author: Antoine
 */

#include "processing.h"
#include "../pal/pal.h"
#include "../utils/types.h"

Processing::Processing(Resource* bound): Resource(),
	bound(bound)

{}
Processing::Processing(void): Resource(),
		bound(NULL)
{

}


Resource* Processing::step(Resource* current)
{
	current->visit();

	Resource* to_run = current->get_next_child_to_visit();
	if(to_run)
	{
		return to_run;
	}

	return current->parent;
}

void Processing::start(void)
{
	Resource* current = this;
	Resource* next;

	while(true)
	{
		next = step(current);
		if(next == bound)
		{
			Elements::e_time_t sleep_amount = current->get_sleep_clock() - get_uptime();
			if(sleep_amount > e_time_t::MIN)
			{
				processing_sleep( sleep_amount );
			}
		}
		else
		{
			current = next;
		}
	}
}

