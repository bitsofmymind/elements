/*
 * tcpip.cpp
 *
 *  Created on: 2010-12-23
 *      Author: antoine
 */

#include "tcpip.h"
#include "uip/uip_arp.h"


TCPIP::TCPIP():
	Resource()
{
	instance = this;

	//init device driver here

	uip_init();
	uip_listen(HTONS(80));

}

void TCPIP::run(void)
{
	schedule(1);
}
