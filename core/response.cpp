//============================================================================
// Name        : response.cpp
// Author      : Antoine Mercier-Linteau
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <stdlib.h>
#include "message.h"
#include "response.h"
//#include "../elements.h"
#include <stdint.h>
#if defined(DEBUG) ||  defined(VERBOSE)
	#include <iostream>
#endif

using namespace Elements;

#ifdef DEBUG
	void Response::print(void)
	{
		using namespace std;

		cout << "%%%%%%%%%%%%%%%%%%%%%% Response %%%%%%%%%%%%%%%%%%%%%%" << endl;
		cout << "---------- Status Line ----------" << endl;
		cout << "HTTP version: " << (int)http_version << endl;
		cout << "Status code: ";
		cout << response_code.text[0] << response_code.text[1] << response_code.text[2] << endl;
		cout << "Reason phrase: ";
		for(unsigned int i = 0 ; i < reason_phrase.length; i++) { cout << reason_phrase.text[i]; }
		cout << endl;

		cout << '\n' << endl;
		Message::print();
	}
#endif

Response::Response(
		const string<uint8_t>* _response_code,
		const string<uint8_t>* _reason_phrase = NULL,
		Request* _original_request = NULL ):
			Message(),
			response_code(*_response_code),
			original_request(_original_request)
{
	if( _reason_phrase == NULL )
	{
		this->reason_phrase.text = NULL;
		this->reason_phrase.length = 0;
	}
	else
	{
		this->reason_phrase = *_reason_phrase;
	}
	if(_original_request != NULL)
	{
		to_url = _original_request->from_url;
		from_url = _original_request->to_url;
	}
	else
	{
		to_url = new URL();
		from_url = new URL();
	}
}

Response::Response(){}

Response::~Response()
{
	if(original_request)
	{
		delete original_request;
	}
	else
	{
		delete to_url;
		delete from_url;
	}
	free(body.text);
}

Message::TYPE Response::get_type( void )
{
	return Message::RESPONSE;
}

char Response::deserialize(string<MESSAGE_SIZE>& buffer, char* index)
{
	//Status-Line = HTTP-Version SP Status-Code SP Reason-Phrase CRLF

	index += 5; //Jumps HTTP/

	http_version = (*index++ - 48) * 10;
	index++; //Skips the '.'
	http_version += *index++ - 48;

	index++; //Jumps the SP. Check in the specification if there can be more than one SP.

	response_code.text = index;
	response_code.length = 3;

	index += 4; //jumps the response code and the following space.
	//Check in the specification if there can be more than one SP.

	reason_phrase.text = index;
	while(*index != '\r' && *(index + 1) !='\n' ) {index++; } //Jumps the reason_phrase
	reason_phrase.length = index - reason_phrase.text;

	index += 2; //skips \r\n (CRLF)

	return Message::deserialize(buffer, index);
}

MESSAGE_SIZE Response::get_message_length(void)
{
	//The 3 and 5 are for 'HTTP/' and the version
	return 5 /*For 'HTTP/'*/ \
		+ 3 /*For the HTTP version*/ \
		+ 1 /*For a space*/ \
		+ response_code.length \
		+ 1 /*For a space*/ \
		+ reason_phrase.length \
		+ 2 /*For CLRF between header and fields*/ \
		+ Message::get_message_length();
}

char Response::serialize( char* buffer)
{

	*buffer++ = 'H'; *buffer++ = 'T'; *buffer++ = 'T'; *buffer++ = 'P'; *buffer++ = '/';
	*buffer++ = '1'; *buffer++ = '.'; *buffer++ = '1';
	*buffer++ = ' ';

	buffer += response_code.copy(buffer);
	*buffer++ = ' ';
	buffer += reason_phrase.copy(buffer);
	*buffer++ = '\r';
	*buffer++ = '\n';

	return Message::serialize(buffer);
}

//STATUS CODE DEFINITIONS
//Informational: 1xx
//const string<uint8_t> Response::CONTINUE_REASON_PHRASE = { "Continue", 100 };
//const string<uint8_t> Response::CONTINUE_CODE = { "100", 3 };
//const string<uint8_t> Response::SWITCHING_PROTOCOLS_REASON_PHRASE = { "Switching Protocols", 101 };
//const string<uint8_t> Response::SWITCHING_PROTOCOLS_CODE = { "101", 3 };

//Successful: 2xx
const string<uint8_t> Response::OK_REASON_PHRASE = MAKE_STRING("OK");
const string<uint8_t> Response::OK_CODE = MAKE_STRING("200");
//const string<uint8_t> Response::CREATED_REASON_PHRASE = { "Created", 201 };
//const string<uint8_t> Response::CREATED_CODE = { "201", 3 };
//const string<uint8_t> Response::ACCEPTED_REASON_PHRASE = { "Accepted", 202 };
//const string<uint8_t> Response::ACCEPTED_CODE = { "202", 3 };
//const string<uint8_t> Response::NON_AUTHORITATIVE_INFORMATION_REASON_PHRASE = { "Non-Authoritative Information", 203 };
//const string<uint8_t> Response::NON_AUTHORITATIVE_INFORMATION_CODE = { "203", 3 };
//const string<uint8_t> Response::NO_CONTENT_REASON_PHRASE = { "No Content", 204 };
//const string<uint8_t> Response::NO_CONTENT_CODE = { "204", 3 };
//const string<uint8_t> Response::RESET_CONTENT_REASON_PHRASE = { "Reset Content", 205 };
//const string<uint8_t> Response::RESET_CONTENT_CODE = { "205", 3 };
//const string<uint8_t> Response::PARTIAL_CONTENT_REASON_PHRASE = { "Partial Content", 206 };
//const string<uint8_t> Response::PARTIAL_CONTENT_CODE = { "206", 3 };

//Redirection: 4xx
//const string<uint8_t> Response::MULTIPLE_CHOICES_REASON_PHRASE = { "Multiple Choices", 300 };
//const string<uint8_t> Response::MULTIPLE_CHOICES_CODE = { "300", 3 };
//const string<uint8_t> Response::MOVED_PERMANENTLY_REASON_PHRASE = { "Moved Permanently", 301 };
//const string<uint8_t> Response::MOVED_PERMANENTLY_CODE = { "301", 3 };
//const string<uint8_t> Response::FOUND_REASON_PHRASE = { "Moved Permanently", 302 };
//const string<uint8_t> Response::FOUND_CODE = { "302", 3 };
//const string<uint8_t> Response::SEE_OTHER_REASON_PHRASE = { "See Other", 303 };
//const string<uint8_t> Response::SEE_OTHER_CODE = { "303", 3 };
//const string<uint8_t> Response::NOT_MODIFIED_REASON_PHRASE = { "Not Modified", 304 };
//const string<uint8_t> Response::NOT_MODIFIED_CODE = { "304", 3 };
//const string<uint8_t> Response::USE_PROXY_REASON_PHRASE = { "Use Proxy", 305 };
//const string<uint8_t> Response::USE_PROXY_CODE = { "305", 3 };
//306 is no longer unused
//const string<uint8_t> Response::TEMPORARY_REDIRECT_REASON_PHRASE = { "Temporary Redirect", 307 };
//const string<uint8_t> Response::TEMPORARY_REDIRECT_CODE = { "307", 3 };

//Client Error: 4xx
const string<uint8_t> Response::BAD_REQUEST_REASON_PHRASE = MAKE_STRING("Bad Request");
const string<uint8_t> Response::BAD_REQUEST_CODE = MAKE_STRING("400");
//const string<uint8_t> Response::UNAUTHORIZED_REASON_PHRASE = { "Unauthorized", 401 };
//const string<uint8_t> Response::UNAUTHORIZED_CODE = { "401", 3 };
//const string<uint8_t> Response::PAYMENT_REQUIRED_REASON_PHRASE = { "Payment Required", 402 };
//const string<uint8_t> Response::PAYMENT_REQUIRED_CODE = { "402", 3 };
//const string<uint8_t> Response::FORBIDDEN_REASON_PHRASE = { "Forbidden", 403 };
//const string<uint8_t> Response::FORBIDDEN_CODE = { "403", 3 };
const string<uint8_t> Response::NOT_FOUND_REASON_PHRASE = MAKE_STRING("Not Found");
const string<uint8_t> Response::NOT_FOUND_CODE = MAKE_STRING("404");
//const string<uint8_t> Response::METHOD_NOT_ALLOWED_REASON_PHRASE = { "Mehtod Not Allowed", 405 };
//const string<uint8_t> Response::METHOD_NOT_ALLOWED_CODE = { "405", 3 };
//const string<uint8_t> Response::NOT_ACCEPTABLE_REASON_PHRASE = { "Not Allowed", 406 };
//const string<uint8_t> Response::NOT_ACCEPTABLE_CODE = { "406", 3 };
//const string<uint8_t> Response::PROXY_AUTHENTICATION_REQUIRED_REASON_PHRASE = { "Proxy Authentication Required", 407 };
//const string<uint8_t> Response::PROXY_AUTHENTICATION_REQUIRED_CODE = { "407", 3 };
//const string<uint8_t> Response::REQUEST_TIMEOUT_REASON_PHRASE = { "Request Timeout", 408 };
//const string<uint8_t> Response::REQUEST_TIMEOUT_CODE = { "408", 3 };
//const string<uint8_t> Response::CONFLICT_REASON_PHRASE = { "Conflict", 409 };
//const string<uint8_t> Response::CONFLICT_CODE = { "409", 3 };
//const string<uint8_t> Response::GONE_REASON_PHRASE = { "Gone", 410 };
//const string<uint8_t> Response::GONE_CODE = { "410", 3 };
//const string<uint8_t> Response::LENGTH_REQUIRED_REASON_PHRASE = { "Length Required", 411 };
//const string<uint8_t> Response::LENGTH_REQUIRED_CODE = { "411", 3 };
//const string<uint8_t> Response::PRECONDITION_FAILED_REASON_PHRASE = { "Precondition Failed", 412 };
//const string<uint8_t> Response::PRECONDITION_FAILED_CODE = { "412", 3 };
//const string<uint8_t> Response::REQUEST_ENTITY_TOO_LARGE_REASON_PHRASE = { "Request Entity Too Large", 413 };
//const string<uint8_t> Response::REQUEST_ENTITY_TOO_LARGE_CODE = { "413", 3 };
//const string<uint8_t> Response::REQUEST_URI_TOO_LONG_REASON_PHRASE = { "Request-URI Too Long", 414 };
//const string<uint8_t> Response::REQUEST_URI_TOO_LONG_CODE = { "414", 3 };
//const string<uint8_t> Response::UNSUPPORTED_MEDIA_TYPE_REASON_PHRASE = { "Unsupported Media Type", 415 };
//const string<uint8_t> Response::UNSUPPORTED_MEDIA_TYPE_CODE = { "415", 3 };
//const string<uint8_t> Response::REQUESTED_RANGE_NOT_SATISFIABLE_REASON_PHRASE = { "Requested Range Not Satisfiable", 416 };
//const string<uint8_t> Response::REQUESTED_RANGE_NOT_SATISFIABLE_CODE = { "416", 3 };
//const string<uint8_t> Response::EXPECTATION_FAILED_REASON_PHRASE = { "Expectation Failed", 417 };
//const string<uint8_t> Response::EXPECTATION_FAILED_CODE = { "417", 3 };

//Server error: 5xx
//const string<uint8_t> Response::INTERNAL_SERVER_ERROR_REASON_PHRASE = { "Internal Server Error", 500 };
//const string<uint8_t> Response::INTERNAL_SERVER_ERROR_CODE = { "500", 3 };
const string<uint8_t> Response::NOT_IMPLEMENTED_REASON_PHRASE = MAKE_STRING("Not Implemented");
const string<uint8_t> Response::NOT_IMPLEMENTED_CODE = MAKE_STRING("501");
//const string<uint8_t> Response::BAD_GATEWAY_REASON_PHRASE = { "Bad Gateway", 502 };
//const string<uint8_t> Response::BAD_GATEWAY_CODE = { "502", 3 };
//const string<uint8_t> Response::SERVICE_UNAVAILABE_REASON_PHRASE = { "Service Unavailable", 503 };
//const string<uint8_t> Response::SERVICE_UNAVAILABE_CODE = { "503", 3 };
//const string<uint8_t> Response::GATEWAY_TIMEOUT_REASON_PHRASE = { "Gateway Timeout", 504 };
//const string<uint8_t> Response::GATEWAY_TIMEOUT_CODE = { "504", 3 };
//const string<uint8_t> Response::HTTP_VERSION_NOT_SUPPORTED_REASON_PHRASE = { "HTTP Version Not Supported", 505 };
//const string<uint8_t> Response::HTTP_VERSION_NOT_SUPPORTED_CODE = { "505", 3 };


//Response header fields
//const string<uint8_t> Response::ACCEPT_RANGES = {"accept-ranges", 38 };
//const string<uint8_t> Response::AGE = {"age", 39 };
//const string<uint8_t> Response::ALLOW = {"allow", 40 };
//const string<uint8_t> Response::ETAG = {"etag", 41 };
//const string<uint8_t> Response::LOCATION = {"location", 42 };
//const string<uint8_t> Response::PROXY_AUTHENTICATE = {"proxy-authenticate", 43 };
//const string<uint8_t> Response::RETRY_AFTER = {"retry-after", 44 };
//const string<uint8_t> Response::SERVER = {"server", 45 };
//const string<uint8_t> Response::VARY = {"vary", 46 };
//const string<uint8_t> Response::WWW_AUTHENTICATE = {"www-authenticate", 47 };
