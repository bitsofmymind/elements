/*
 * authority.cpp
 *
 *  Created on: Feb 17, 2009
 *      Author: Antoine
 */




#include "processing.h"
//#include "../elements.h"
#include "../utils/utils.h"
#include "../utils/types.h"
#include "resource.h"
#include "../pal/pal.h"
#include <stdint.h>

using namespace Elements;

Processing::Processing(Resource* bound):Resource(),
		bound(bound),
		current(this)
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
	while(true)
	{
		step();
		if(current == bound)
		{
			Elements::e_time_t sleep_amount = current->get_sleep_clock() - get_uptime();
			if(sleep_amount > e_time_t::MIN)
			{
				processing_sleep( sleep_amount );
			}
		}
	}
}




