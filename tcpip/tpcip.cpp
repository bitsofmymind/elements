/*
 * tpcip.cpp
 *
 *  Created on: 2011-02-21
 *      Author: antoine
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
#include <avr_pal.h>

#include <string.h>
#define BUF ((struct uip_eth_hdr *)&uip_buf[0])

TCPIPStack::TCPIPStack():
	Resource(),
	periodic_timer(0),
	arp_timer(0),
	counter(0)
{
	stack = this;

	network_init();
	VERBOSE_PRINTLN_P("Network initialized");

	uip_init();

	struct uip_eth_addr mac = {UIP_ETHADDR0, UIP_ETHADDR1, UIP_ETHADDR2, UIP_ETHADDR3, UIP_ETHADDR4, UIP_ETHADDR5};

	uip_setethaddr(mac);

	uip_listen(HTONS(80));

#ifdef __DHCPC_H__
	dhcpc_init(&mac, 6);
#else
	uip_ipaddr_t ipaddr;

	uip_ipaddr(ipaddr, 10,0,0,2);
	uip_sethostaddr(ipaddr);
	VERBOSE_PRINT_P("Host address: ");
	printip(ipaddr);
	VERBOSE_PRINTLN();
	uip_ipaddr(ipaddr, 10,0,0,1);
	VERBOSE_PRINT_P("Default router: ");
	printip(ipaddr);
	VERBOSE_PRINTLN();
	uip_setdraddr(ipaddr);
	uip_ipaddr(ipaddr, 255,255,255,0);
	VERBOSE_PRINT_P("Netmask: ");
	printip(ipaddr);
	VERBOSE_PRINTLN();
	uip_setnetmask(ipaddr);
#endif /*__DHCPC_H__*/

	schedule(ASAP);
}

void TCPIPStack::run(void)
{
	uip_len = network_read();

	if(uip_len > 0)
	{
		if(BUF->type == htons(UIP_ETHTYPE_IP))
		{
			uip_arp_ipin();
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
	else if(periodic_timer <= get_uptime() )
	{
		periodic_timer = get_uptime() + 200;
		for(uint8_t i = 0; i < UIP_CONNS; i++)
		{
			uip_periodic(i);
			if(uip_len > 0)
			{
				uip_arp_out();
				network_send();
			}
		}

		if( arp_timer <= get_uptime() )
		{
			arp_timer = get_uptime() + 1000;
			uip_arp_timer();
		}
	}

	schedule(ASAP);
}

void TCPIPStack::printip(uip_ipaddr_t addr)
{
	/*If VERBOSITY is undefined, this method should be optimizes away by the compiler.*/

	VERBOSE_PRINT_DEC(((uint8_t*)addr)[0]);
	VERBOSE_PRINT_BYTE('.');
	VERBOSE_PRINT_DEC(((uint8_t*)addr)[1]);
	VERBOSE_PRINT_BYTE('.');
	VERBOSE_PRINT_DEC(((uint8_t*)addr)[2]);
	VERBOSE_PRINT_BYTE('.');
	VERBOSE_PRINT_DEC(((uint8_t*)addr)[3]);
}
void TCPIPStack::printip(uip_ipaddr_t addr, uint16_t port)
{
	/*If VERBOSITY is undefined, this method should be optimizes away by the compiler.*/

	VERBOSE_PRINT_DEC(((uint8_t*)addr)[0]);
	VERBOSE_PRINT_BYTE('.');
	VERBOSE_PRINT_DEC(((uint8_t*)addr)[1]);
	VERBOSE_PRINT_BYTE('.');
	VERBOSE_PRINT_DEC(((uint8_t*)addr)[2]);
	VERBOSE_PRINT_BYTE('.');
	VERBOSE_PRINT_DEC(((uint8_t*)addr)[3]);
	VERBOSE_PRINT_BYTE(':');
	VERBOSE_PRINT_DEC(port);
}

void TCPIPStack::cleanup(struct elements_app_state* s)
{
	VERBOSE_PRINT_P("Cleanup on ");
	printip(uip_conn->ripaddr, uip_conn->rport);
	VERBOSE_PRINTLN();

	if(s->body)
	{
		delete s->body;
	}
	if(s->header)
	{
		delete s->header;
	}
	else if(s->request)
	{
		delete s->request;
	}

	s->header = NULL;
	s->request = NULL;
	s->body = NULL;
}

void TCPIPStack::appcall(void)
{
	struct elements_app_state *s = &(uip_conn->appstate);

	//string<uint8_t> txt = MAKE_STRING("HTTP/1.0 200 OK\r\nContent-Type: text/plain\r\n\r\ncounter is 0");

	//txt.text[txt.length - 1] = counter++ + 42;

	if(uip_aborted())
	{
		VERBOSE_PRINT_P("Connection from ");
		printip(uip_conn->ripaddr, uip_conn->rport);
		VERBOSE_PRINTLN_P(" aborted");
	}
	else if(uip_timedout())
	{
		VERBOSE_PRINT_P("Connection from ");
		printip(uip_conn->ripaddr, uip_conn->rport);
		VERBOSE_PRINTLN_P(" timed out");
	}
	else if(uip_closed())
	{
		VERBOSE_PRINT_P("Connection from ");
		printip(uip_conn->ripaddr, uip_conn->rport);
		VERBOSE_PRINTLN_P(" closed");
	}

	if(uip_closed() || uip_timedout() || uip_aborted())
	{
		cleanup(s);
		return;
	}
	else if(uip_connected())
	{
		printip(uip_conn->ripaddr, uip_conn->rport);
		VERBOSE_PRINTLN_P(" connected");

		s->request = NULL;
		s->body = NULL;
		s->header = NULL;
	}
	else if(uip_newdata())
	{
		VERBOSE_PRINT_P("New data from ");
		printip(uip_conn->ripaddr, uip_conn->rport);
		VERBOSE_PRINTLN();

		if(!s->request)
		{
			cleanup(s);
			s->request = new Request();
			if(!s->request)
			{
				uip_abort();
				return;
			}
		}

		Message::PARSER_RESULT res = s->request->parse((const char*)uip_appdata, uip_len);
		switch(res)
		{
			case Message::PARSING_COMPLETE:
				VERBOSE_PRINTLN_P("Parsing_complete, sending");
				send(s->request);
				break;
			case Message::PARSING_SUCESSFUL:
				break;
			default:
				VERBOSE_PRINTLN_P("Parsing Error");
				uip_abort();
				return;
		}
	}
	else if(uip_acked())
	{
		VERBOSE_PRINT_P("ACK from ");
		printip(uip_conn->ripaddr, uip_conn->rport);
		VERBOSE_PRINTLN();

		if(s->header->cursor == s->header->size
						&& s->body
						&& s->body->cursor == s->body->size)
		{
			cleanup(s);
			return;
		}

		size_t sent = 0;

		if(s->header->cursor != s->header->size)
		{
			sent += s->header->read((char*)uip_appdata, uip_mss());
		}
		if(s->body &&
				s->header->cursor == s->header->size &&
				sent < uip_mss())
		{
			sent += s->body->read((char*)uip_appdata + sent, uip_mss() - sent);
		}
		uip_send(uip_appdata, sent);
		/*if(s->header->cursor == s->header->size
				&& s->body
				&& s->body->cursor == s->body->size)
		{
			//uip_close();
		}*/

	}
	else if(uip_rexmit())
	{
		VERBOSE_PRINT_P("Retransmit from ");
		printip(uip_conn->ripaddr, uip_conn->rport);
		VERBOSE_PRINTLN();
		//No rexmit for now
		uip_abort();
	}
	else if(uip_poll())
	{
		VERBOSE_PRINT_P("Polling for ");
		printip(uip_conn->ripaddr, uip_conn->rport);
		VERBOSE_PRINTLN();

		for(uint8_t i = 0; i < to_send.items; i++ )
		{
			if(to_send[i]->original_request == s->request)
			{
				Response* response = to_send.remove(i);
				size_t size = response->get_header_length();
				char* buffer = (char*)ts_malloc(size);
				if(!buffer)
				{
					uip_abort();
				}
				response->serialize(buffer);
				s->header = new MemFile(buffer, size);
				if(!s->header)
				{
					ts_free(buffer);
					uip_abort();
				}

				if(response->body_file)
				{
					s->body = response->body_file;
					response->body_file = NULL;
				}
				size_t sent = 0;
				sent += s->header->read((char*)uip_appdata, uip_mss());
				if(s->body &&
						s->header->cursor == s->header->size &&
						sent < uip_mss())
				{
					sent += s->body->read((char*)uip_appdata + sent, uip_mss() - sent);
				}
				uip_send(uip_appdata, sent);
				VERBOSE_PRINTLN_P("Sending reply packet");

				/*This is a bit messy, people should not have to know whether to delete a request object or not*/
				s->request = NULL;
				delete response;
				break;
			}
		}
	}
}

void elements_appcall(void)
{
	stack->appcall();
}

Response::status_code TCPIPStack::process(Response* response, Message** return_message)
{

	if(response->to_url->cursor >=  response->to_url->resources.items)
	{
		to_send.append(response);
#if VERBOSITY
		VERBOSE_PRINTLN_P("Received response: ");
		print_transaction(response);
#endif
		return OK_200;
	}

	return PASS_308;
}

