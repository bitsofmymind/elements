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
#include "psock.h"

//#include "clock-arch.h"

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

	string<uint8_t> txt = MAKE_STRING("HTTP/1.0 200 OK\r\nContent-Type: text/plain\r\n\r\ncounter is 0");

	txt.text[txt.length - 1] = counter++ + 42;

	if(uip_connected())
	{
		Debug::println("Connection made");
		s->dataleft = txt.length;
		s->dataptr = txt.text;
	}

	if(uip_newdata())
	{
		Debug::println("Receiving request");
		for(uint16_t i = 3; i < uip_datalen(); i++)
		{
			if(((char*)uip_appdata)[i - 3] == '\r' &&
				((char*)uip_appdata)[i - 2] == '\n' &&
				((char*)uip_appdata)[i - 1] == '\r' &&
				((char*)uip_appdata)[i] == '\n')
			{
				Debug::println("request complete");
				uip_send(s->dataptr, s->dataleft);
			}
		}
	}

	if(uip_acked())
	{
		Debug::println("ack");

		if(s->dataleft < uip_mss())
		{
		  uip_close();
		  return;
		}
		s->dataptr += uip_conn->len;
		s->dataleft -= uip_conn->len;
		uip_send(s->dataptr, s->dataleft);
	}


	Debug::println("ret");
}

void elements_appcall(void)
{
	stack->appcall();
}


