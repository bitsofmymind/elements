/* tcpip.cpp - Implements a resource to interface with a TCP/IP network
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

#include "tcpip.h"

#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>

#include "global-conf.h"
#include "uip_arp.h"
#include "network.h"
#include "drivers/enc28j60/enc28j60.h"

#include <core/request.h>
#include "../avr_pal.h"
#include <utils/memfile.h>

#include <string.h>
#define BUF ((struct uip_eth_hdr *)&uip_buf[0])

TCPIPStack::TCPIPStack():
	Resource(),
	periodic_timer(0),
	arp_timer(0),
#ifdef DHCP
	dhcp_timer(0),
#endif
	counter(0)
{
	stack = this; // There can only be one TCPIP stack (for now).

	network_init(); // Initialize the network.

	uip_init(); // Initialize UIP.

	// Set the MAC address for the interface.
	struct uip_eth_addr mac =
	{
		UIP_ETHADDR0,
		UIP_ETHADDR1,
		UIP_ETHADDR2,
		UIP_ETHADDR3,
		UIP_ETHADDR4,
		UIP_ETHADDR5
	};
	uip_setethaddr(mac);

	uip_listen(HTONS(80)); // Listen to port 80.

	VERBOSE_PRINTLN_P("Network initialized");

	uip_ipaddr_t ipaddr; // Will store the resource's IP address.

#ifndef DHCP

	uip_ipaddr(ipaddr, 10,0,0,2);
	uip_sethostaddr(ipaddr);
	VERBOSE_PRINT_P("Host address: ");
	printip(ipaddr);
	VERBOSE_PRINTLN();
	uip_ipaddr(ipaddr, 10,0,0,1);
	uip_setdraddr(ipaddr);
	VERBOSE_PRINT_P("Default router: ");
	printip(ipaddr);
	VERBOSE_PRINTLN();
	uip_ipaddr(ipaddr, 255,255,255,0);
	uip_setnetmask(ipaddr);
	VERBOSE_PRINT_P("Netmask: ");
	printip(ipaddr);
	VERBOSE_PRINTLN();

#else

	uip_ipaddr_t addr;

	this->mac_addr = mac;

	state = STATE_INITIAL;
	uip_ipaddr(addr, 255,255,255,255); // Reset the ip address.

	// Create a connection to the DHCP server.
	dhcp_conn = uip_udp_new(&addr, HTONS(DHCPC_SERVER_PORT));
	uip_ipaddr(addr, 0,0,0,0); // Set ip address to 0.0.0.0.
	uip_sethostaddr(addr);

	if(dhcp_conn != NULL) // If the connection to the DHCP server was created.
	{
		// Listen to the DHCP client port.
		uip_udp_bind(dhcp_conn, HTONS(DHCPC_CLIENT_PORT));
		VERBOSE_PRINTLN_P("DHCP Initialized");
	}

#endif

	schedule(ASAP); // Resource should be run ASAP.
}

void TCPIPStack::run(void)
{
	uip_len = network_read(); // How many bytes does UIP have for us?

	/* This routine, if I remember correctly, is for updating the ARP table.
	 * I'm not sure about the specifics though so I'll skip commenting it until
	 * my memory refreshes. */

	if(uip_len > 0) // If there are some bytes that require our attention.
	{
		if(BUF->type == htons(UIP_ETHTYPE_IP)) // If there is an IP in the buffer.
		{
			uip_arp_ipin(); // Save this IP to the ARP table.
			uip_input();
			if(uip_len > 0)
			{
				uip_arp_out();
				network_send();
			}
		}
		else if(BUF->type == htons(UIP_ETHTYPE_ARP))
		{
			VERBOSE_PRINTLN_P("ARP Received");
			uip_arp_arpin();
			if(uip_len > 0)
			{
				network_send();
			}
		}

	}

	//expire(periodic_timer, 15);
	for(uint8_t i = 0; i < UIP_CONNS; i++)
	{
		uip_periodic(i);
		if(uip_len > 0)
		{
			uip_arp_out();
			network_send();
		}
	}

	if(is_expired(dhcp_timer)) // If the DHCP lease has expired.
	{
		for(uint8_t i = 0; i < UIP_UDP_CONNS; i++)
		{
			uip_udp_periodic(i);
			if(uip_len > 0)
			{
				uip_arp_out();
				network_send();
			}
		}
	}

	if(is_expired(arp_timer))
	{
		arp_timer = expire(arp_timer, SECONDS(10));
		uip_arp_timer();
	}

	schedule(10);
}

void TCPIPStack::printip(uip_ipaddr_t addr)
{
	/*If VERBOSITY is undefined, this method should be optimized away by the compiler.*/

	VERBOSE_TPRINT(((uint8_t*)addr)[0], DEC);
	VERBOSE_PRINT('.');
	VERBOSE_TPRINT(((uint8_t*)addr)[1], DEC);
	VERBOSE_PRINT('.');
	VERBOSE_TPRINT(((uint8_t*)addr)[2], DEC);
	VERBOSE_PRINT('.');
	VERBOSE_TPRINT(((uint8_t*)addr)[3], DEC);
}
void TCPIPStack::printip(uip_ipaddr_t addr, uint16_t port)
{
	/*If VERBOSITY is undefined, this method should be optimized away by the compiler.*/

	printip(addr);
	VERBOSE_PRINT(':');
	VERBOSE_TPRINT(HTONS(port), DEC);
}

void TCPIPStack::cleanup(struct elements_app_state* s)
{
	VERBOSE_PRINT_P("Cleanup on ");
	printip(uip_conn->ripaddr, uip_conn->rport);
	VERBOSE_PRINTLN();

	if(s->body) // If a body was allocated.
	{
		delete s->body; // Delete it.
	}
	if(s->header) // If a header was allocated.
	{
		delete s->header; // Delete it.
	}
	else if(s->request) // If a request was allocated.
	{
		///TODO what if we delete a request that has been sent out to framework.
		/* May be requests are no longer kept here once they have been sent out
		 * to their destination, I can't remember.*/
		delete s->request; // Delete it.
	}

	// Blank all attributes.
	s->header = NULL;
	s->request = NULL;
	s->body = NULL;
	s->last_sent = 0;
}

void TCPIPStack::appcall(void)
{
	// Get the application state for the current connection.
	struct elements_app_state *s = &(uip_conn->appstate);

	if(uip_aborted()) // If a connection was aborted.
	{
		// Print its details.
		VERBOSE_PRINT_P("Connection from ");
		printip(uip_conn->ripaddr, uip_conn->rport);
		VERBOSE_PRINTLN_P(" aborted");
	}
	else if(uip_timedout()) // If a connection timed out.
	{
		// Print its details.
		VERBOSE_PRINT_P("Connection from ");
		printip(uip_conn->ripaddr, uip_conn->rport);
		VERBOSE_PRINTLN_P(" timed out");
	}
	else if(uip_closed()) // If a connection was closed.
	{
		// Print its details.
		VERBOSE_PRINT_P("Connection from ");
		printip(uip_conn->ripaddr, uip_conn->rport);
		VERBOSE_PRINTLN_P(" closed");
	}

	// If a connection was terminated.
	if(uip_closed() || uip_timedout() || uip_aborted())
	{
		cleanup(s); // Clean it up.
		return; // Nothing else to do.
	}
	else if(uip_connected()) // If someone has connected to the framework.
	{
		printip(uip_conn->ripaddr, uip_conn->rport);
		VERBOSE_PRINTLN_P(" connected");

		// Reset all attributes of the application state.
		s->request = NULL;
		s->body = NULL;
		s->header = NULL;
		s->last_sent = 0;
	}
	else if(uip_newdata()) // If new data has been received by the connection.
	{
		VERBOSE_PRINT_P("New data from ");
		printip(uip_conn->ripaddr, uip_conn->rport);
		VERBOSE_PRINTLN();

		if(!s->request) // If this is the first data we receive from this connection.
		{
			cleanup(s); // Reset the application state.
			s->request = new Request(); // Create a new request.
			if(!s->request) // If the request could not be allocated.
			{
				uip_abort(); // Abort the connection.
				// Cleanup is done when the abort routine calls this method.
				return; // Cannot do anything more.
			}
		}

		// Parse the request data.
		Message::PARSER_RESULT res = s->request->parse((const char*)uip_appdata, uip_len);
		switch(res)
		{
			case Message::PARSING_COMPLETE:
				VERBOSE_PRINTLN_P("Parsing complete, sending");
				dispatch(s->request); // Send the message out to the framework.
				break;
			case Message::PARSING_SUCESSFUL:
				// The part of the message that we got checks out, wait for the rest.
				break;
			default: // A parsing error occurred.
				VERBOSE_PRINTLN_P("Parsing Error");
				uip_abort(); // Abort the connection.
				// Cleanup is done when the abort routine calls this method.
				return; // The message is malformed, abort.
		}
	}
	// If the message that we sent was acked or needs to be retransmitted.
	else if(uip_acked() || uip_rexmit())
	{
		if(uip_acked()) // Message was acked.
		{
			VERBOSE_PRINT_P("ACK from ");
		}
		else // Message needs to be retransmitted.
		{
			VERBOSE_PRINT_P("Retransmit from ");
			// If the part to resend falls inside the header.
			if(s->body->cursor() < s->last_sent)
			{
				// Get the amount of header data that was sent.
				size_t header_sent = s->last_sent - s->body->cursor();
				// Rewind the header cursor.
				s->header->cursor(s->header->cursor() - header_sent);
				s->body->cursor(0); // Reset the body cursor.
			}
			else // The part to resend is in the body.
			{
				// Rewind the cursor.
				s->body->cursor( s->body->cursor() - s->last_sent );
			}
		}

		printip(uip_conn->ripaddr, uip_conn->rport);
		VERBOSE_PRINTLN();

		// If we are done sending the header.
		if(s->header->cursor() == s->header->size
			&& s->body // If the message has a body.
			&& s->body->cursor() == s->body->size // If we are done sending the body.
		)
		{
			cleanup(s);
			return; // Done sending that message.
		}

		size_t sent = 0; /* Number of bytes send on this transaction.
		Note: we cannot send more than MSS bytes of data. */

		// If the header has not yet been sent.
		if(s->header->cursor() != s->header->size)
		{
			// Transfer up to MSS bytes from the header.
			sent += s->header->read((char*)uip_appdata, uip_mss());
		}

		if(s->body && // If the message has a body.
			// If we are done sending the header.
			s->header->cursor() == s->header->size &&
			sent < uip_mss() // If we can still send some bytes.
		)
		{
			// Transfer up to MSS bytes from the body.
			sent += s->body->read((char*)uip_appdata + sent, uip_mss() - sent);
		}

		uip_send(uip_appdata, sent); // Send the data.
		// Save the amount of data sent in case we need to retransmit.
		s->last_sent = sent;

	}
	else if(uip_poll()) // If UIP is polling for our attention.
	{
		VERBOSE_PRINT_P("Polling for ");
		printip(uip_conn->ripaddr, uip_conn->rport);
		VERBOSE_PRINTLN();

		// For each response waiting to be sent out.
		for(uint8_t i = 0; i < to_send.items; i++ )
		{
			// If this is the response for the connection that polled us.
			if(to_send[i]->original_request == s->request)
			{
				// This means the response is ready.
				// Remove the response from the list.
				Response* response = to_send.remove(i);

				// Get the total size of the response'header.
				size_t size = response->serialize(NULL, false);

				// Allocate a buffer to contain the serialized response.
				char* buffer = (char*)ts_malloc(size);
				if(!buffer) // If allocation failed.
				{
					s->request = NULL; // So the request is not double freed.
					delete response;
					uip_abort(); // Abort the connection.
					// Cleanup is done when the abort routine calls this method.
					return; // Nothing we can do, that transaction is lost.
				}

				// Serialize the response header into its buffer.
				response->serialize(buffer, true);

				// Wrap the header in a memory file.
				s->header = new MemFile(buffer, size, false);
				if(!s->header) // If allocation failed.
				{
					ts_free(buffer);
					// Cleanup is done when the abort routine calls this method.
					s->request = NULL;
					delete response;
					uip_abort(); // Abort the connection.
					// Cleanup is done when the abort routine calls this method.
					return; // Nothing we can do, that transaction is lost.
				}

				if(response->get_body()) // If the response has a body.
				{
					 s->body = response->unset_body(); // Retrieve it.
				}

				size_t sent = 0; /* Number of bytes send on this transaction.
				Note: we cannot send more than MSS bytes of data. */

				// Transfer up to MSS bytes from the header.
				sent += s->header->read((char*)uip_appdata, uip_mss());

				if(s->body && // If there is a body.
					// If the header has been sent.
					s->header->cursor() == s->header->size &&
					sent < uip_mss() // If there is room for more transmission.
				)
				{
					// Transfer up to MSS bytes from the body.
					sent += s->body->read((char*)uip_appdata + sent, uip_mss() - sent);
				}

				uip_send(uip_appdata, sent); // Send the data.

				// Save the amount of data sent in case we need to retransmit.
				s->last_sent = sent;
				VERBOSE_PRINTLN_P("Sending reply packet");

				s->request = NULL; // So the request is not double freed.
				/* From this point on, the Response object is no longer needed
				 * because we extracted all of its contents in buffers. */
				delete response;
				break;
			}
		}
	}
}

#ifdef DHCP

static const u8_t xid[4] = {0xad, 0xde, 0x12, 0x23};
static const u8_t magic_cookie[4] = {99, 130, 83, 99};

/*---------------------------------------------------------------------------*/
static u8_t *
add_msg_type(u8_t *optptr, u8_t type)
{
  *optptr++ = DHCP_OPTION_MSG_TYPE;
  *optptr++ = 1;
  *optptr++ = type;
  return optptr;
}
/*---------------------------------------------------------------------------*/
u8_t * TCPIPStack::add_server_id(u8_t *optptr)
{
  *optptr++ = DHCP_OPTION_SERVER_ID;
  *optptr++ = 4;
  memcpy(optptr, serverid, 4);
  return optptr + 4;
}
/*---------------------------------------------------------------------------*/
u8_t * TCPIPStack::add_req_ipaddr(u8_t *optptr)
{
  *optptr++ = DHCP_OPTION_REQ_IPADDR;
  *optptr++ = 4;
  memcpy(optptr, ipaddr, 4);
  return optptr + 4;
}
/*---------------------------------------------------------------------------*/
static u8_t *
add_req_options(u8_t *optptr)
{
  *optptr++ = DHCP_OPTION_REQ_LIST;
  *optptr++ = 3;
  *optptr++ = DHCP_OPTION_SUBNET_MASK;
  *optptr++ = DHCP_OPTION_ROUTER;
  *optptr++ = DHCP_OPTION_DNS_SERVER;
  return optptr;
}
/*---------------------------------------------------------------------------*/
static u8_t *
add_end(u8_t *optptr)
{
  *optptr++ = DHCP_OPTION_END;
  return optptr;
}
/*---------------------------------------------------------------------------*/
void TCPIPStack::create_msg(register struct dhcp_msg *m)
{
  m->op = DHCP_REQUEST;
  m->htype = DHCP_HTYPE_ETHERNET;
  m->hlen = 6;
  m->hops = 0;
  memcpy(m->xid, xid, sizeof(m->xid));
  m->secs = 0;
  m->flags = HTONS(BOOTP_BROADCAST); /*  Broadcast bit. */
  /*  uip_ipaddr_copy(m->ciaddr, uip_hostaddr);*/
  memcpy(m->ciaddr, uip_hostaddr, sizeof(m->ciaddr));
  memset(m->yiaddr, 0, sizeof(m->yiaddr));
  memset(m->siaddr, 0, sizeof(m->siaddr));
  memset(m->giaddr, 0, sizeof(m->giaddr));
  memcpy(m->chaddr, &mac_addr, 6);
  memset(&m->chaddr[6], 0, sizeof(m->chaddr) - 6);
#ifndef UIP_CONF_DHCP_LIGHT
  memset(m->sname, 0, sizeof(m->sname));
  memset(m->file, 0, sizeof(m->file));
#endif

  memcpy(m->options, magic_cookie, sizeof(magic_cookie));
}
/*---------------------------------------------------------------------------*/
void TCPIPStack::send_discover(void)
{
	u8_t *end;
	struct dhcp_msg *m = (struct dhcp_msg *)uip_appdata;

	create_msg(m);

	end = add_msg_type(&m->options[4], DHCPDISCOVER);
	end = add_req_options(end);
	end = add_end(end);

	uip_send(uip_appdata, end - (u8_t *)uip_appdata);
}
/*---------------------------------------------------------------------------*/
void TCPIPStack::send_request(void)
{
	u8_t *end;
	struct dhcp_msg *m = (struct dhcp_msg *)uip_appdata;

	create_msg(m);

	end = add_msg_type(&m->options[4], DHCPREQUEST);
	end = add_server_id(end);
	end = add_req_ipaddr(end);
	end = add_end(end);

	uip_send(uip_appdata, end - (u8_t *)uip_appdata);
}
/*---------------------------------------------------------------------------*/
uint8_t TCPIPStack::parse_options(u8_t *optptr, int len)
{
  u8_t *end = optptr + len;
  u8_t type = 0;

  while(optptr < end) {
    switch(*optptr) {
    case DHCP_OPTION_SUBNET_MASK:
      memcpy(netmask, optptr + 2, 4);
      break;
    case DHCP_OPTION_ROUTER:
      memcpy(default_router, optptr + 2, 4);
      break;
    case DHCP_OPTION_DNS_SERVER:
      memcpy(dnsaddr, optptr + 2, 4);
      break;
    case DHCP_OPTION_MSG_TYPE:
      type = *(optptr + 2);
      break;
    case DHCP_OPTION_SERVER_ID:
      memcpy(serverid, optptr + 2, 4);
      break;
    case DHCP_OPTION_LEASE_TIME:
      memcpy(lease_time, optptr + 2, 4);
      break;
    case DHCP_OPTION_END:
      return type;
    }

    optptr += optptr[1] + 2;
  }
  return type;
}
/*---------------------------------------------------------------------------*/
uint8_t TCPIPStack::parse_msg(void)
{
	struct dhcp_msg *m = (struct dhcp_msg *)uip_appdata;

	/*if(state == STATE_OFFER_RECEIVED)
	{
		for(uint16_t i= 0 ; i< 350; i++)
		{
			VERBOSE_TPRINT(((uint8_t*)uip_appdata)[i], HEX);
			VERBOSE_PRINT(' ');
		}
		while(true);
	}*/
	if(m->op == DHCP_REPLY &&
		memcmp(m->xid, xid, sizeof(xid)) == 0 &&
		memcmp(m->chaddr, &mac_addr, 6) == 0)
	{
		memcpy(ipaddr, m->yiaddr, 4);
		return parse_options(&m->options[4], uip_datalen());
	}
	return 0;
}
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/

void TCPIPStack::dhcpc_appcall(void)
{
	VERBOSE_TPRINTLN(uip_flags, BIN);
	switch(state)
	{
		case STATE_INITIAL:
			send_discover();
			VERBOSE_PRINTLN_P("Discovering DHCP server...");
			state = STATE_SENDING;
			dhcp_timer = expire(dhcp_timer, SECONDS(2));
			return;
		case STATE_SENDING:
			if(uip_newdata() && parse_msg() == DHCPOFFER)
			{
				state = STATE_OFFER_RECEIVED;
				send_request();
				VERBOSE_PRINTLN_P("Sending request to DHCP server...");
				dhcp_timer = expire(dhcp_timer, SECONDS(2));
				return;
			}
			break;
		case STATE_OFFER_RECEIVED:
			if(uip_newdata() && parse_msg() == DHCPACK)
			{
				state = STATE_CONFIG_RECEIVED;

				uip_sethostaddr(ipaddr);
				VERBOSE_PRINT_P("Host address: ");
				printip(ipaddr);
				VERBOSE_PRINTLN();
				uip_setdraddr(default_router);
				VERBOSE_PRINT_P("Default router: ");
				printip(default_router);
				VERBOSE_PRINTLN();
				uip_setnetmask(netmask);
				VERBOSE_PRINT_P("Netmask: ");
				printip(netmask);
				VERBOSE_PRINTLN();
				uint32_t lease = ntohs(lease_time[1]) + ((uint32_t)ntohs(lease_time[0]) >> 16);
				dhcp_timer = expire(dhcp_timer, SECONDS(lease - 10));
				VERBOSE_PRINT_P("Lease time: ");
				VERBOSE_TPRINT(lease, DEC);
				VERBOSE_PRINTLN_P(" seconds");

				VERBOSE_PRINTLN_P("DHCP Offer received and configured.");
				return;
			}
			break;
		default:
			break;
	}

	if(is_expired(dhcp_timer))
	{
		if(state == STATE_CONFIG_RECEIVED)
		{
			VERBOSE_PRINTLN_P("DHCP lease expired!");
		}
		else
		{
			ERROR_PRINTLN_P("DHCP Timeout!");
			dhcp_timer = expire(dhcp_timer, SECONDS(10));
		}
		state = STATE_INITIAL;
	}
}

void dhcpc_appcall()
{
	stack->dhcpc_appcall(); // Call the appcall method on the stack.
}

#endif

void elements_appcall(void)
{
	stack->appcall(); // Call the appcall method on the stack.
}

Response::status_code TCPIPStack::process(Response* response)
{

	if(!response->to_destination()) // If the response is at destination.
	{
		ATOMIC // Prevent UIP calls from executing while we queue the message.
		{
			// Queue the response for sending to its originator.
			to_send.append(response);
		}
		print_transaction(response);

		return OK_200; // Keep the response.
	}

	return PASS_308; // The response is not at destination.
}

