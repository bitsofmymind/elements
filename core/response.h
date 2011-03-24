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
		typedef uint16_t status_code;

		//Informational: 1xx
		#define CONTINUE_100 100
		#define SWITCHING_PROTOCOLS_101 101

		//Successful: 2xx
		#define OK_200 200
		#define CREATED_201 201
		#define ACCEPTED_202 202
		#define NON_AUTHORITATIVE_INFORMATION_203 203
		#define NO_CONTENT_204 204
		#define RESET_CONTENT_205 205
		#define PARTIAL_CONTENT_206 206

		//Redirection: 3xx
		#define MULTIPLE_CHOICES_300 300
		#define MOVED_PERMANENTLY_301 301
		#define FOUND_302 302
		#define SEE_OTHER_303 303
		#define NOT_MODIFIED_304 304
		#define USE_PROXY_305 305
		//306 is no longer unused
		#define TEMPORARY_REDIRECT_307 307
		#define PASS_308 308 //Elements framework specific

		//Client Error: 4xx
		#define BAD_REQUEST_400 400
		#define UNAUTHORIZED_401 401
		#define PAYMENT_REQUIRED_402 402
		#define FORBIDDEN_403 403
		#define NOT_FOUND_404 404
		#define METHOD_NOT_ALLOWED_405 405
		#define NOT_ACCEPTABLE_406 406
		#define PROXY_AUTHENTICATION_REQUIRED_407 407
		#define REQUEST_TIMEOUT_408 408
		#define CONFLICT_409 409
		#define GONE_410 410
		#define LENGTH_REQUIRED_411 411
		#define PRECONDITION_FAILED_412 412
		#define REQUEST_ENTITY_TOO_LARGE_413 413
		#define REQUEST_URI_TOO_LONG_414 414
		#define UNSUPPORTED_MEDIA_TYPE_415 415
		#define REQUESTED_RANGE_NOT_SATISFIABLE_416 416
		#define EXPECTATION_FAILED_417 417

		//Server error: 5xx
		#define INTERNAL_SERVER_ERROR_500 500
		#define NOT_IMPLEMENTED_501 501
		#define BAD_GATEWAY_502 502
		#define SERVICE_UNAVAILABE_503 503
		#define GATEWAY_TIMEOUT_504 504
		#define HTTP_VERSION_NOT_SUPPORTED_503 503

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

		uint16_t response_code_int;
		mime content_type;
		Request* original_request;

	private:
		static const Message::TYPE type = RESPONSE;

	public:
		Response( const uint16_t response_code,	Request* request );
		~Response();

		virtual void print();
		virtual size_t serialize(char* buffer, bool write);

	protected:
#if RESPONSE_DESERIALIZATION
		virtual Message::PARSER_RESULT parse_header(const char* line, size_t size);
#endif
};
#endif /* RESPONSE_H_ */
