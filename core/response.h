/*
 * response.h
 *
 *  Created on: Feb 16, 2009
 *      Author: Antoine
 */

#ifndef RESPONSE_H_
#define RESPONSE_H_

#include "message.h"
#include "request.h"
#include "../utils/utils.h"


class Response: public Message
{
	public:
		//STATUS CODES DEFINITIONS
		typedef uint8_t status_code;

		//Informational: 1xx
		#define CONTINUE_100 ((1 << 5) + 0)
		#define SWITCHING_PROTOCOLS_101 ((1 << 5) + 1)

		//Successful: 2xx
		#define OK_200 ((2 << 5) + 0)
		#define CREATED_201 ((2 << 5) + 1)
		#define ACCEPTED_202 ((2 << 5) + 2)
		#define NON_AUTHORITATIVE_INFORMATION_203 ((2 << 5) + 3)
		#define NO_CONTENT_204 ((2 << 5) + 4)
		#define RESET_CONTENT_205 ((2 << 5) + 5)
		#define PARTIAL_CONTENT_206 ((2 << 5) + 6)

		//Redirection: 3xx
		#define MULTIPLE_CHOICES_300 ((3 << 5) + 0)
		#define MOVED_PERMANENTLY_301 ((3 << 5) + 1)
		#define FOUND_302 ((3 << 5) + 2)
		#define SEE_OTHER_303 ((3 << 5) + 3)
		#define NOT_MODIFIED_304 ((3 << 5) + 4)
		#define USE_PROXY_305 ((3 << 5) + 5)
		//306 is no longer unused
		#define TEMPORARY_REDIRECT_307 ((3 << 5) + 7)
		#define PASS_308 ((3 << 5) + 8) //Elements framework specific

		//Client Error: 4xx
		#define BAD_REQUEST_400 ((4 << 5) + 0)
		#define UNAUTHORIZED_401 ((4 << 5) + 1)
		#define PAYMENT_REQUIRED_402 ((4 << 5) + 2)
		#define FORBIDDEN_403 ((4 << 5) + 3)
		#define NOT_FOUND_404 ((4 << 5) + 4)
		#define METHOD_NOT_ALLOWED_405 ((4 << 5) + 5)
		#define NOT_ACCEPTABLE_406 ((4 << 5) + 6)
		#define PROXY_AUTHENTICATION_REQUIRED_407 ((4 << 5) + 7)
		#define REQUEST_TIMEOUT_408 ((4 << 5) + 8)
		#define CONFLICT_409 ((4 << 5) + 9)
		#define GONE_410 ((4 << 5) + 10)
		#define LENGTH_REQUIRED_411 ((4 << 5) + 11)
		#define PRECONDITION_FAILED_412 ((4 << 5) + 12)
		#define REQUEST_ENTITY_TOO_LARGE_413 ((4 << 5) + 13)
		#define REQUEST_URI_TOO_LONG_414 ((4 << 5) + 14)
		#define UNSUPPORTED_MEDIA_TYPE_415 ((4 << 5) + 15)
		#define REQUESTED_RANGE_NOT_SATISFIABLE_416 ((4 << 5) + 16)
		#define EXPECTATION_FAILED_417 ((4 << 5) + 17)

		//Server error: 5xx
		#define INTERNAL_SERVER_ERROR_500 ((5 << 5) + 0)
		#define NOT_IMPLEMENTED_501 ((5 << 5) + 1)
		#define BAD_GATEWAY_502 ((5 << 5) + 2)
		#define SERVICE_UNAVAILABE_503 ((5 << 5) + 3)
		#define GATEWAY_TIMEOUT_504 ((5 << 5) + 4)
		#define HTTP_VERSION_NOT_SUPPORTED_503 ((5 << 5) + 5)

		//Response header fields
		//static const char* ACCEPT_RANGES;
		//static const char* AGE;
		//static const char* ALLOW;
		//static const char* ETAG;
		//static const char* LOCATION;
		//static const char* PROXY_AUTHENTICATE;
		//static const char* RETRY_AFTER;
		//static const char* SERVER;
		//static const char* VARY;
		//static const char* WWW_AUTHENTICATE;

		status_code response_code_int;
		Request* original_request;

	public:
		Response( const status_code response_code,	Request* request );
		~Response();

		virtual void print();
		virtual size_t serialize(char* buffer, bool write);

	protected:
#if RESPONSE_DESERIALIZATION
		virtual Message::PARSER_RESULT parse_header(const char* line, size_t size);
#endif
};
#endif /* RESPONSE_H_ */
