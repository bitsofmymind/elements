/*
 * tcpip.h
 *
 *  Created on: 2011-02-21
 *      Author: antoine
 */

#ifndef TCPIP_H_
#define TCPIP_H_

#include <core/resource.h>
#include "timer.h"
#include "uip.h"

class TCPIPStack: public Resource
{
	private:
		uip_ipaddr_t ipaddr;
		//struct timer periodic_timer, arp_timer;
		uptime_t periodic_timer;
		uptime_t arp_timer;

	public:
		TCPIPStack();

		virtual void run(void);
};

#endif /* TCPIP_H_ */
