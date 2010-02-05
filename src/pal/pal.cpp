/*
 * pal.c
 *
 *  Created on: May 23, 2009
 *      Author: Antoine
 */

#include <pal/pal.h>
#include <stdint.h>

/*formatedTime get_formated_time( void )
{

}*/

using namespace Elements;
static e_time_t system_uptime;
static e_time_t system_time;

e_time_t get_time( void )
{
	//time_in_milliseconds += uptime_in_milliseconds;
	return system_time;
}

void set_time(Elements::e_time_t time)
{
	system_time = time;
}

e_time_t get_uptime( void )
{
	return system_uptime;
}

void increase_uptime(uint16_t amount)
{
	system_uptime.increase_ms(amount);
}

/*This is the Interrupt handling function for time keeping, it should be vectored to every millisecond.*/

static uint32_t number_of_elements = 0;

inline uint32_t get_number_of_elements( void ){ return number_of_elements; }

inline void register_element( void )
{
	number_of_elements++;
}

inline void unregister_element( void )
{
	number_of_elements--;
}

/*
It would be a great idea to build a bunch of functions for elements to register with interrupts, EEPROM adresses and system registers. The problem, there is no memory security
( at least on the AVR) to begin with and keeping track of who owns what memory appears to be too resource intensive for now.
bool register_interrupt_handler( void ( *handler_pointer )( void ), void* vector_address )
{

}

bool unregister_interrupt_handler( void ( *handler_pointer )( void ), void* vector_address )
{
	for( unsigned int i = VECTORS_BOTTOM; i < VECTORS_TOP ; i += sizeof( void * )
	{
		if( *i == vector_address )
		{
			*i = 0;
			//Deactivate interrupt
		}
	}
	return false;
}
*/
