/* elements.h - Defines configuration options for the franework
 * Copyright (C) 2011 Antoine Mercier-Linteau
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef ELEMENTS_H_
#define ELEMENTS_H_

#include <stddef.h>
#include <stdint.h>

#define RESOURCE_DESTRUCTION 1
#define RESPONSE_DESERIALIZATION 0//Set to 1 if framework is to receive responses from another domain
#define REQUEST_SERIALIZATION 1//Set to 1 if framework is to send request to another domain
#define TIME_KEEPING 1
#define URL_SERIALIZATION 1
#define MESSAGE_AGE 1

#define HEARTBEAT 1
#define PROCESSING_AS_AUTHORITY 1

typedef uint64_t uptime_t;
#define MAX_UPTIME 18446744073709551615ULL

#define ATOMIC

#define ITOA 0

#define BODY_ARGS_PARSING 1

/*Debug output controls*/
#define OUTPUT_DEBUG 1

/*Verbosity controls*/
#define OUTPUT_ERRORS 1
#define OUTPUT_WARNINGS 1
#define VERBOSITY 1

/*URL parsing controls*/
#define URL_PROTOCOL 0
#define URL_AUTHORITY 0
#define URL_PORT 0
//Resource parsing is essential
#define URL_ARGUMENTS 0
#define URL_FRAGMENT 0

/*File class controls*/
#define REVERSIBLE 1
#define READ_ONLY 1

/*Response headers*/
#define LOCATION 1

#define AUTHORITY_REDIRECT 1
#if AUTHORITY_REDIRECT
#undef LOCATION
#define LOCATION 1
#endif

#endif /* ELEMENTS_H_ */
