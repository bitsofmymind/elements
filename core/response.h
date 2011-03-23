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
		#define OK_200 200

		#define PASS_308 308

		#define BAD_REQUEST_400 400
		#define NOT_FOUND_404 404

		#define INTERNAL_SERVER_ERROR_500 500
		#define NOT_IMPLEMENTED_501 501

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

		static const Message::TYPE type = RESPONSE;

	public:



		uint16_t response_code_int;
		mime content_type;
		Request* original_request;

	public:
		Response( const uint16_t response_code,	Request* request );
		~Response();


		virtual void print();


		virtual size_t get_header_length(void);
		#ifndef NO_RESPONSE_DESERIALIZATION
			virtual Message::PARSER_RESULT parse_header(const char* line, size_t size);
		#endif
		virtual void serialize(char* buffer);

};

//STATUS CODE DEFINITIONS
//Informational: 1xx
//static const string<uint8_t> CONTINUE_REASON_PHRASE;
//static const string<uint8_t> CONTINUE_CODE;
//static const string<uint8_t> SWITCHING_PROTOCOLS_REASON_PHRASE;
//static const string<uint8_t> SWITCHING_PROTOCOLS_CODE;

//Successful: 2xx
//static const string<uint8_t> OK_REASON_PHRASE;
//static const string<uint8_t> OK_CODE;
//static const string<uint8_t> CREATED_REASON_PHRASE;
//static const string<uint8_t> CREATED_CODE;
//static const string<uint8_t> ACCEPTED_REASON_PHRASE;
//static const string<uint8_t> ACCEPTED_CODE;
//static const string<uint8_t> NON_AUTHORITATIVE_INFORMATION_REASON_PHRASE;
//static const string<uint8_t> NON_AUTHORITATIVE_INFORMATION_CODE;
//static const string<uint8_t> NO_CONTENT_REASON_PHRASE;
//static const string<uint8_t> NO_CONTENT_CODE;
//static const string<uint8_t> RESET_CONTENT_REASON_PHRASE;
//static const string<uint8_t> RESET_CONTENT_CODE;
//static const string<uint8_t> PARTIAL_CONTENT_REASON_PHRASE;
//static const string<uint8_t> PARTIAL_CONTENT_CODE;

//Redirection: 3xx
//static const string<uint8_t> MULTIPLE_CHOICES_REASON_PHRASE;
//static const string<uint8_t> MULTIPLE_CHOICES_CODE;
//static const string<uint8_t> MOVED_PERMANENTLY_REASON_PHRASE;
//static const string<uint8_t> MOVED_PERMANENTLY_CODE;
//static const string<uint8_t> FOUND_REASON_PHRASE;
//static const string<uint8_t> FOUND_CODE;
//static const string<uint8_t> SEE_OTHER_REASON_PHRASE;
//static const string<uint8_t> SEE_OTHER_CODE;
//static const string<uint8_t> NOT_MODIFIED_REASON_PHRASE;
//static const string<uint8_t> NOT_MODIFIED_CODE;
//static const string<uint8_t> USE_PROXY_REASON_PHRASE;
//static const string<uint8_t> USE_PROXY_CODE;
//306 is no longer unused
//static const string<uint8_t> TEMPORARY_REDIRECT_REASON_PHRASE;
//static const string<uint8_t> TEMPORARY_REDIRECT_CODE;

//Client Error: 4xx
//static const Elements::string<uint8_t> BAD_REQUEST_REASON_PHRASE;
//static const Elements::string<uint8_t> BAD_REQUEST_CODE;
//static const string<uint8_t> UNAUTHORIZED_REASON_PHRASE;
//static const string<uint8_t> UNAUTHORIZED_CODE;
//static const string<uint8_t> PAYMENT_REQUIRED_REASON_PHRASE;
//static const string<uint8_t> PAYMENT_REQUIRED_CODE;
//static const string<uint8_t> FORBIDDEN_REASON_PHRASE;
//static const string<uint8_t> FORBIDDEN_CODE;
//static const Elements::string<uint8_t> NOT_FOUND_REASON_PHRASE;
//static const Elements::string<uint8_t> NOT_FOUND_CODE;
//static const string<uint8_t> METHOD_NOT_ALLOWED_REASON_PHRASE;
//static const string<uint8_t> METHOD_NOT_ALLOWED_CODE;
//static const string<uint8_t> NOT_ACCEPTABLE_REASON_PHRASE;
//static const string<uint8_t> NOT_ACCEPTABLE_CODE;
//static const string<uint8_t> PROXY_AUTHENTICATION_REQUIRED_REASON_PHRASE;
//static const string<uint8_t> PROXY_AUTHENTICATION_REQUIRED_CODE;
//static const string<uint8_t> REQUEST_TIMEOUT_REASON_PHRASE;
//static const string<uint8_t> REQUEST_TIMEOUT_CODE;
//static const string<uint8_t> CONFLICT_REASON_PHRASE;
//static const string<uint8_t> CONFLICT_CODE;
//static const string<uint8_t> GONE_REASON_PHRASE;
//static const string<uint8_t> GONE_CODE;
//static const string<uint8_t> LENGTH_REQUIRED_REASON_PHRASE;
//static const string<uint8_t> LENGTH_REQUIRED_CODE;
//static const string<uint8_t> PRECONDITION_FAILED_REASON_PHRASE;
//static const string<uint8_t> PRECONDITION_FAILED_CODE;
//static const string<uint8_t> REQUEST_ENTITY_TOO_LARGE_REASON_PHRASE;
//static const string<uint8_t> REQUEST_ENTITY_TOO_LARGE_CODE;
//static const string<uint8_t> REQUEST_URI_TOO_LONG_REASON_PHRASE;
//static const string<uint8_t> REQUEST_URI_TOO_LONG_CODE;
//static const string<uint8_t> UNSUPPORTED_MEDIA_TYPE_REASON_PHRASE;
//static const string<uint8_t> UNSUPPORTED_MEDIA_TYPE_CODE;
//static const string<uint8_t> REQUESTED_RANGE_NOT_SATISFIABLE_REASON_PHRASE;
//static const string<uint8_t> REQUESTED_RANGE_NOT_SATISFIABLE_CODE;
//static const string<uint8_t> EXPECTATION_FAILED_REASON_PHRASE;
//static const string<uint8_t> EXPECTATION_FAILED_CODE;

//Server error: 5xx
//static const string<uint8_t> INTERNAL_SERVER_ERROR_REASON_PHRASE;
//static const string<uint8_t> INTERNAL_SERVER_ERROR_CODE;
//static const Elements::string<uint8_t> NOT_IMPLEMENTED_REASON_PHRASE;
//static const Elements::string<uint8_t> NOT_IMPLEMENTED_CODE;
//static const string<uint8_t> BAD_GATEWAY_REASON_PHRASE;
//static const string<uint8_t> BAD_GATEWAY_CODE;
//static const string<uint8_t> SERVICE_UNAVAILABE_REASON_PHRASE;
//static const string<uint8_t> SERVICE_UNAVAILABE_CODE;
//static const string<uint8_t> GATEWAY_TIMEOUT_REASON_PHRASE;
//static const string<uint8_t> GATEWAY_TIMEOUT_CODE;
//static const string<uint8_t> HTTP_VERSION_NOT_SUPPORTED_REASON_PHRASE;
//static const string<uint8_t> HTTP_VERSION_NOT_SUPPORTED_CODE;

#endif /* RESPONSE_H_ */
