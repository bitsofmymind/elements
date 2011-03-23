#ifndef __APP_H__
#define __APP_H__

#include <stdint.h>
#include <elements.h>
#include <utils/file.h>
#include <core/request.h>

typedef struct elements_app_state
{
	Request* request;
	File* header;
	File* body;
} uip_tcp_appstate_t;

void elements_appcall(void);
#ifndef UIP_APPCALL
#define UIP_APPCALL elements_appcall
#endif /* UIP_APPCALL */

#endif /* __APP_H__ */
