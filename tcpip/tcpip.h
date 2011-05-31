/* tcpip.h - Implements a resource to interface with a TCP/IP network
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

		List<Response*> to_send;

	public:
		TCPIPStack();

		virtual void run(void);
		void appcall(void);

		virtual Response::status_code process(Response* response);
	protected:
		void printip(uip_ipaddr_t addr);
		void printip(uip_ipaddr_t addr, uint16_t port);
		void cleanup(struct elements_app_state* s);
};

static TCPIPStack* stack;

#endif /* TCPIP_H_ */
