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
//#include "../elements.h"
#include "../utils/utils.h"
#include "../utils/types.h"

using namespace Elements;

class Response: public Message
{
	public:

		typedef struct status_code
		{
			const string< unsigned int> reason_phrase;
			const unsigned int value_int;
			const string< uint8_t > value_string;
		};

		//STATUS CODE DEFINITIONS
		//Informational: 1xx
		//static const string<uint8_t> CONTINUE_REASON_PHRASE;
		//static const string<uint8_t> CONTINUE_CODE;
		//static const string<uint8_t> SWITCHING_PROTOCOLS_REASON_PHRASE;
		//static const string<uint8_t> SWITCHING_PROTOCOLS_CODE;

		//Successful: 2xx
		static const string<uint8_t> OK_REASON_PHRASE;
		static const string<uint8_t> OK_CODE;
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

		//Redirection: 4xx
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
		static const Elements::string<uint8_t> BAD_REQUEST_REASON_PHRASE;
		static const Elements::string<uint8_t> BAD_REQUEST_CODE;
		//static const string<uint8_t> UNAUTHORIZED_REASON_PHRASE;
		//static const string<uint8_t> UNAUTHORIZED_CODE;
		//static const string<uint8_t> PAYMENT_REQUIRED_REASON_PHRASE;
		//static const string<uint8_t> PAYMENT_REQUIRED_CODE;
		//static const string<uint8_t> FORBIDDEN_REASON_PHRASE;
		//static const string<uint8_t> FORBIDDEN_CODE;
		static const Elements::string<uint8_t> NOT_FOUND_REASON_PHRASE;
		static const Elements::string<uint8_t> NOT_FOUND_CODE;
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
		static const Elements::string<uint8_t> NOT_IMPLEMENTED_REASON_PHRASE;
		static const Elements::string<uint8_t> NOT_IMPLEMENTED_CODE;
		//static const string<uint8_t> BAD_GATEWAY_REASON_PHRASE;
		//static const string<uint8_t> BAD_GATEWAY_CODE;
		//static const string<uint8_t> SERVICE_UNAVAILABE_REASON_PHRASE;
		//static const string<uint8_t> SERVICE_UNAVAILABE_CODE;
		//static const string<uint8_t> GATEWAY_TIMEOUT_REASON_PHRASE;
		//static const string<uint8_t> GATEWAY_TIMEOUT_CODE;
		//static const string<uint8_t> HTTP_VERSION_NOT_SUPPORTED_REASON_PHRASE;
		//static const string<uint8_t> HTTP_VERSION_NOT_SUPPORTED_CODE;



		//Response header fields
		//static const string< uint8_t > ACCEPT_RANGES;
		//static const string< uint8_t > AGE;
		//static const string< uint8_t > ALLOW;
		//static const string< uint8_t > ETAG;
		//static const string< uint8_t > LOCATION;
		//static const string< uint8_t > PROXY_AUTHENTICATE;
		//static const string< uint8_t > RETRY_AFTER;
		//static const string< uint8_t > SERVER;
		//static const string< uint8_t > VARY;
		//static const string< uint8_t > WWW_AUTHENTICATE;

		static const Message::TYPE type = RESPONSE;

	public:


		uint8_t http_version;
		Elements::string<uint8_t> response_code;
		uint16_t response_code_int;
		Elements::string<uint8_t> reason_phrase;

		Request* original_request;

	public:
		Response(
				const Elements::string<uint8_t>* response_code,
				const Elements::string<uint8_t>* reason_phase,
				Request* request );
		Response();
		~Response();

		#ifdef DEBUG
			virtual void print();
		#endif

		virtual TYPE get_type(void);
		virtual uint32_t get_message_length(void);
		virtual char deserialize( Elements::string<uint32_t>& buffer, char* index );
		virtual char serialize(char* buffer);

};

#endif /* RESPONSE_H_ */
