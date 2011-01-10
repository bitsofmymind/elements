/*
 * tcpip.h
 *
 *  Created on: 2010-12-23
 *      Author: antoine
 */

#ifndef TCPIP_H_
#define TCPIP_H_

#include <core/resource.h>
#include "uip/uip.h"

class TCPIP: public Resource
{


	public:
		TCPIP();

		void run(void);
};

TCPIP* instance;


void elements_appcall(void);
#define UIP_APPCALL elements_appcall

struct elements_state{

};

typedef struct elements_state uip_tcp_appstate_t;

#endif /* TCPIP_H_ */
