#ifndef __APP_H__
#define __APP_H__

#include <stdint.h>

typedef struct elements_app_state {
	const char* dataptr;
	uint16_t dataleft;
} uip_tcp_appstate_t;

void elements_appcall(void);
#ifndef UIP_APPCALL
#define UIP_APPCALL elements_appcall
#endif /* UIP_APPCALL */

#endif /* __APP_H__ */
