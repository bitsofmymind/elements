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
	uip_ipaddr(ipaddr, 192,167,0,2);
	uip_sethostaddr(ipaddr);
	uip_ipaddr(ipaddr, 192,167,0,1);
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

	}

	if(uip_connected())
	{
		Debug::println("Connection made");
		/*s->dataleft = txt.length;
		s->dataptr = txt.text;*/
		s->buffer.length = 0;
		s->body_sent = false;
	}

	if(uip_newdata())
	{
		if(s->buffer.length)
		{

			//char* ptr = (char*)ts_realloc(s->request, s->sz);
			/*Realloc appears to be causing a failure in memory allocation as sucessive reallocs and frees
			 * will not yield to symmetrical adresses. There is bug with realloc
			 * (http://www.mail-archive.com/avr-libc-dev@nongnu.org/msg03679.html)*/
		}
		else
		{
			for(uint16_t i = 1; i < uip_datalen(); i++)
			{
				if(	((char*)uip_appdata)[i - 1] == '\r' &&
					((char*)uip_appdata)[i] == '\n')
				{
					Debug::println("Request received");
					i += 3; //To add the \r\n and one because we will use i as a length;
					s->buffer.text = (char*)ts_malloc(i);
					if(!s->buffer.text)
					{
						break;
					}
					memcpy(s->buffer.text, uip_appdata, i - 2);
					s->buffer.length = i;
					s->buffer.text[i - 2] = '\r';
					s->buffer.text[i - 1] = '\n';

					return;
				}
			}
			uip_abort();
			return;

		}


		for(uint16_t i = 0; i < uip_datalen(); i++)
		{
			Debug::print(((char*)uip_appdata)[i]);

			if(i > 2 &&
				((char*)uip_appdata)[i - 3] == '\r' &&
				((char*)uip_appdata)[i - 2] == '\n' &&
				((char*)uip_appdata)[i - 1] == '\r' &&
				((char*)uip_appdata)[i] == '\n')
			{


				Debug::print("request: ");
				Debug::print(s->buffer.length, DEC);
				Debug::print(" ");
				Debug::println(s->buffer.text, s->buffer.length);

				Request* request = new Request();

				if (!request)
				{

				}
				else if(request->deserialize(s->buffer, s->buffer.text))
				{
					Debug::println("deserialization failed");
				}
				else if(send(request))
				{

				}
				else
				{

					break;
				}
				delete request;
				uip_abort();
				break;


			}
		}
	}

	if(uip_acked())
	{
		Debug::println("ack");

		if(!s->body_sent)
		{
			if(s->dataleft < uip_mss())
			{
				ts_free(s->dataptr);
				s->body_sent = true;
				goto next;
			}
			s->dataptr += uip_conn->len;
			s->dataleft -= uip_conn->len;
			uip_send(s->dataptr, s->dataleft);
		}
		else
		{
			next:

			uint16_t len = s->body->read((char*)uip_appdata, uip_mss(), true);
			Debug::print(len, DEC);
			Debug::print(" ");
			Debug::print(s->body->cursor, DEC);
			Debug::print(" ");
			Debug::println(s->body->size, DEC);
			if(!len)
			{
				uip_close();
				delete s->body;
				return;
			}
			uip_send(uip_appdata, len);
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
			if(to_send[i]->original_request->message.text == s->buffer.text)
			{
				Debug::println("replying");
				Response* response = to_send.remove(i);
				s->dataleft = response->get_message_length();
				s->dataptr = (char*)ts_malloc(s->dataleft);
				if(!s->dataptr)
				{
					uip_abort();
				}
				else
				{
					response->serialize(s->dataptr);
					s->body = response->body_file;
					Debug::print(s->body->cursor, DEC);
					Debug::print(" ");
					Debug::println(s->body->size, DEC);
					response->body_file = NULL;
					uip_send(s->dataptr, s->dataleft);
				}

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

