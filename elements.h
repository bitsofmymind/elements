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

#define RESOURCE_DESTRUCTION 0
#define RESPONSE_DESERIALIZATION 0//Set to 1 if framework is to receive responses from another domain
#define REQUEST_SERIALIZATION 0//Set to 1 if framework is to send request to another domain
#define TIME_KEEPING 1


typedef uint64_t uptime_t;
#define MAX_UPTIME 18446744073709551615ULL

#define ATOMIC

#define ITOA 0

#define BODY_ARGS_PARSING 1

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
