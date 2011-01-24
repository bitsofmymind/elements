/*
 * elements.h
 *
 *  Created on: 2010-09-23
 *      Author: antoine
 */

#ifndef ELEMENTS_H_
#define ELEMENTS_H_

#include <stddef.h>
#include <stdint.h>

//#define VERBOSE 1
#define MESSAGE_SIZE uint32_t
//#define NO_RESOURCE_DESTRUCTION
//#define NO_RESPONSE_DESERIALIZATION //#undefine if framework is to receive responses from another domain
//#define NO_REQUEST_SERIALIZATION //#undefine if framework is to send request to another domain

typedef uint64_t uptime_t;
#define MAX_UPTIME __UINT64_C(18446744073709551615)

#define ATOMIC

#endif /* ELEMENTS_H_ */
