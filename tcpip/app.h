#ifndef __APP_H__
#define __APP_H__

#include <stdint.h>
#include <utils/types.h>
#include <elements.h>
#include <utils/file.h>

typedef struct elements_app_state {
	char* dataptr;
	MESSAGE_SIZE dataleft;
	Elements::string<MESSAGE_SIZE> buffer;
	bool body_sent;
	File<MESSAGE_SIZE> * body;
} uip_tcp_appstate_t;

void elements_appcall(void);
#ifndef UIP_APPCALL
#define UIP_APPCALL elements_appcall
#endif /* UIP_APPCALL */

#endif /* __APP_H__ */
