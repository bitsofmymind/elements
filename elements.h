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

#define DEBUG
//#define VERBOSE 1
#define MESSAGE_SIZE uint32_t
//#define NO_RESOURCE_DESTRUCTION
//#define NO_RESPONSE_DESERIALIZATION //#undefine if framework is to receive responses from another domain
//#define NO_REQUEST_SERIALIZATION //#undefine if framework is to send request to another domain
#define TIME_KEEPING
#define BODY_ARGS_PARSING
typedef uint64_t uptime_t;
#define MAX_UPTIME 18446744073709551615ULL

#define ATOMIC

/*Verbosity controls*/
#define OUTPUT_ERRORS 1
#define OUTPUT_WARNINGS 1
#define VERBOSITY 1

/*Default methods controls*/
#define HTTP_TRACE 0
#define HTTP_HEAD 0
#define HTTP_GET 1

/*URL parsing controls*/
#define URL_PROTOCOL 0
#define URL_AUTHORITY 0
#define URL_PORT 0
//Resource parsing is essential
#define URL_ARGUMENTS 0
#define URL_FRAGMENT 0

#endif /* ELEMENTS_H_ */
