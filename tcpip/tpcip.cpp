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

	uip_init();

	struct uip_eth_addr mac = {UIP_ETHADDR0, UIP_ETHADDR1, UIP_ETHADDR2, UIP_ETHADDR3, UIP_ETHADDR4, UIP_ETHADDR5};

	uip_setethaddr(mac);

	uip_listen(HTONS(80));

#ifdef __DHCPC_H__
	dhcpc_init(&mac, 6);
#else
	uip_ipaddr(ipaddr, 10,0,0,2);
	uip_sethostaddr(ipaddr);
	uip_ipaddr(ipaddr, 10,0,0,1);
	uip_setdraddr(ipaddr);
	uip_ipaddr(ipaddr, 255,255,255,0);
	uip_setnetmask(ipaddr);
#endif /*__DHCPC_H__*/

	Debug::println("Network initialized");
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
			Debug::println("arp");
			uip_arp_arpin();
			if(uip_len > 0)
			{
				network_send();
			}
		}

	}
	else if(periodic_timer <= get_uptime() )
	{
		periodic_timer = get_uptime() + 500;
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
			arp_timer = get_uptime() + 10000;
			uip_arp_timer();
		}
	}

	schedule(ASAP);
}

void TCPIPStack::appcall(void)
{
	struct elements_app_state *s = &(uip_conn->appstate);

	//string<uint8_t> txt = MAKE_STRING("HTTP/1.0 200 OK\r\nContent-Type: text/plain\r\n\r\ncounter is 0");

	//txt.text[txt.length - 1] = counter++ + 42;

	if(uip_aborted())
	{
		Debug::println("Aborted");
	}
	if(uip_timedout())
	{
		Debug::println("Timedout");
	}
	if(uip_closed())
	{
		Debug::println("Closed");
	}

	if(uip_closed() || uip_timedout() || uip_aborted())
	{
		if(s->body)
		{
			delete s->body;
			s->body = NULL;
		}
		if(s->header)
		{
			delete s->header;
			s->header = NULL;
		}
		else if(s->request)
		{
			delete s->request;
			s->request = NULL;
		}
	}

	if(uip_connected())
	{
		Debug::println("Connection made");
		s->request = new Request();
		Debug::print("request addr ");
		Debug::println((size_t)s->request, DEC);
		s->receiving_body = false;
		s->body = NULL;
		s->header = NULL;
		if(!s->request)
		{
			uip_abort();
			return;
		}
	}

	if(uip_newdata())
	{
		Message::PARSER_RESULT res = s->request->parse((const char*)uip_appdata, uip_len);
		switch(res)
		{
			case Message::PARSING_COMPLETE:
				if(s->request->content_length)
				{
					s->receiving_body = true; //now useless
					Debug::print("has body");
					s->request->body_file->print();
					//char c[4];
					//s->request->body_file->read(c, 4, true);
					//Debug::println((size_t)s->request->body_file, DEC);
				}
				else
				{
					Debug::println("no body");
				}
				send(s->request);
				break;
			case Message::PARSING_SUCESSFUL:
				break;
			default:
				Debug::println("Parsing Error");
				uip_abort();
				return;
		}
	}

	if(uip_acked())
	{
		MESSAGE_SIZE sent = 0;

		if(s->header->cursor != s->header->size)
		{
			sent += s->header->read((char*)uip_appdata, uip_mss(), true);
		}
		if(s->body &&
				s->header->cursor == s->header->size &&
				sent < uip_mss())
		{
			sent += s->body->read((char*)uip_appdata + sent, uip_mss() - sent ,true);
		}
		uip_send(uip_appdata, sent);
		if(s->header->cursor == s->header->size
				&& s->body
				&& s->body->cursor == s->body->size)
		{
			uip_close();
		}
	}
	if(uip_rexmit())
	{
		Debug::println("rexmit");
	}
	if(uip_poll())
	{
		Debug::println("poll");
		for(uint8_t i = 0; i < to_send.items; i++ )
		{
			if(to_send[i]->original_request == s->request)
			{
				Debug::println("replying");
				Response* response = to_send.remove(i);
				MESSAGE_SIZE size = response->get_header_length();
				char* buffer = (char*)ts_malloc(size);
				if(!buffer)
				{
					uip_abort();
				}
				response->serialize(buffer);
				s->header = new MemFile<MESSAGE_SIZE>(buffer, size);
				if(!s->header)
				{
					ts_free(buffer);
					uip_abort();
				}
				s->header->print();
				if(response->body_file)
				{
					s->body = response->body_file;
					response->body_file = NULL;
				}
				MESSAGE_SIZE sent = 0;
				sent += s->header->read((char*)uip_appdata, uip_mss(), true);
				if(s->body &&
						s->header->cursor == s->header->size &&
						sent < uip_mss())
				{
					sent += s->body->read((char*)uip_appdata + sent, uip_mss() - sent, true);
				}
				uip_send(uip_appdata, sent);
				Debug::println("reply");

				/*This is a bit messy, people should not have to know whether to delete a request object or not*/
				s->request = NULL;
				delete response;
				break;
			}
		}
	}

	Debug::println("ret");
}

void elements_appcall(void)
{
	stack->appcall();
}

Response::status_code TCPIPStack::process(Response* response, Message** return_message)
{

	if(response->to_url->cursor >=  response->to_url->resources.items)
	{
		Debug::print("msg came");
		to_send.append(response);
		Debug::println(" back!");
		return OK_200;
	}

	return PASS_308;
}

