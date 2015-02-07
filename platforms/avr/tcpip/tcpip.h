/* tcpip.h - Header file for the TCPIPStack class.
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

#define DHCP

#ifdef DHCP
#define UDP_CONF_BROADCAST 1 //Necessary if we want to reacquire IP adresses.
#define UIP_CONF_UDP 1 //DHCP works over UDP.
#if UIP_CONF_UDP_CONNS < 1
#define UIP_CONF_UDP_CONNS 1
#endif
#define UIP_CONF_UDP_CHECKSUMS 1


#define STATE_INITIAL         0
#define STATE_SENDING         1
#define STATE_OFFER_RECEIVED  2
#define STATE_CONFIG_RECEIVED 3

static struct dhcpc_state s;

struct dhcp_msg {
  u8_t op, htype, hlen, hops;
  u8_t xid[4];
  u16_t secs, flags;
  u8_t ciaddr[4];
  u8_t yiaddr[4];
  u8_t siaddr[4];
  u8_t giaddr[4];
  u8_t chaddr[16];
#ifndef UIP_CONF_DHCP_LIGHT
  u8_t sname[64];
  u8_t file[128];
#endif
  u8_t options[312];
};

#define BOOTP_BROADCAST 0x8000

#define DHCP_REQUEST        1
#define DHCP_REPLY          2
#define DHCP_HTYPE_ETHERNET 1
#define DHCP_HLEN_ETHERNET  6
#define DHCP_MSG_LEN      236

#define DHCPC_SERVER_PORT  67
#define DHCPC_CLIENT_PORT  68

#define DHCPDISCOVER  1
#define DHCPOFFER     2
#define DHCPREQUEST   3
#define DHCPDECLINE   4
#define DHCPACK       5
#define DHCPNAK       6
#define DHCPRELEASE   7

#define DHCP_OPTION_SUBNET_MASK   1
#define DHCP_OPTION_ROUTER        3
#define DHCP_OPTION_DNS_SERVER    6
#define DHCP_OPTION_REQ_IPADDR   50
#define DHCP_OPTION_LEASE_TIME   51
#define DHCP_OPTION_MSG_TYPE     53
#define DHCP_OPTION_SERVER_ID    54
#define DHCP_OPTION_REQ_LIST     55
#define DHCP_OPTION_END         255
#endif

/// A Resource representing a TCPIP stack connected to an Ethernet interface.
/** The lightweight TCPIP library uip is being use to take care of the stack
 * itself. What this resource does is bridging it to the elements framework.
 * It also features a crude DHCP client function.*/
class TCPIPStack: public Resource
{
	private:

		/// @todo No longer remember what that does, probably useless.
		uptime_t periodic_timer;

		/// The delay between updates to the ARP table.
		uptime_t arp_timer;

		/// @todo No longer remember what that does, probably useless.
		uint8_t counter;

		/// The responses to send.
		List<Response*> to_send;

#ifdef DHCP
		/// The state of the DHCP server.
		char state;

		/// The timer keeping track of the DHCP lease's expiration.
		uptime_t dhcp_timer;

		/// A connection structure to the DHCP server.
		struct uip_udp_conn *dhcp_conn;

		/// Don't know what that does.
		u8_t serverid[4];

		/// DHCP lease time in seconds.
		u16_t lease_time[2];

		/// The received IP address.
		u16_t ipaddr[2];

		/// The received netmask.
		u16_t netmask[2];

		/// The received DNS address.
		u16_t dnsaddr[2];

		/// The received default router.
		u16_t default_router[2];

		/// Our MAC address.
		uip_eth_addr mac_addr;
#endif

	public:

		/// Class contructor.
		TCPIPStack();

		/// Runs the resource.
		virtual void run(void);

		/// The appcall for this resource.
		/** When UIP receives a packet, it calls this method. */
		void appcall(void);

#ifdef DHCP
		/// The appcall for the DHCP function.
		/** When UIP receives a DHCP packet, it calls this method. */
		void dhcpc_appcall(void);
#endif

        /// Process a response message.
        /**
         * YOU SHOULD NOT DELETE OR FREE THE RESPONSE ARGUMENT!
         * This method is meant to be overridden in order to specialize this class.
         * @param response the response to process.
         * @return the status_code produced while processing the response.
         */
		virtual Response::status_code process(Response* response);

	protected:

		/// Prints out an ip address.
		/** @param addr the address*/
		void printip(uip_ipaddr_t addr);

		/// Prints out an ip address and a port.
		/** @param addr the address
		 * @param port the port*/
		void printip(uip_ipaddr_t addr, uint16_t port);

		/// Cleans up an elements_app_state structure.
		/** Structure of this type a given to uip to make
		 * TCPIP transaction asynchronous.
		 * @param addr the address.*/
		void cleanup(struct elements_app_state* s);

#ifdef DHCP
		uint8_t parse_msg(void);
		uint8_t parse_options(u8_t *optptr, int len);
		void create_msg(register struct dhcp_msg *m);
		void send_request(void);
		void send_discover(void);
		u8_t * add_server_id(u8_t *optptr);
		u8_t * add_req_ipaddr(u8_t *optptr);
#endif
};

/// A pointer to the stack for asynchronous calls to it.
static TCPIPStack* stack;

#endif /* TCPIP_H_ */
