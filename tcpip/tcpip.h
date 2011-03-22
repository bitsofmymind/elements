/*
 * tcpip.h
 *
 *  Created on: 2011-02-21
 *      Author: antoine
 */

#ifndef TCPIP_H_
#define TCPIP_H_

#include <core/resource.h>
#include "uip.h"

class TCPIPStack: public Resource
{
	private:
		uptime_t periodic_timer;
		uptime_t arp_timer;

		uint8_t counter;

		List<Response> to_send;

	public:
		TCPIPStack();

		virtual void run(void);
		void appcall(void);

		virtual Response::status_code process(Response* response, Message** return_message);
	protected:
		void printip(uip_ipaddr_t addr);

};

static TCPIPStack* stack;

#endif /* TCPIP_H_ */
