/* response.cpp - Implements an HTTP response
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

#include <stdlib.h>
#include "message.h"
#include "response.h"
#include <pal/pal.h>
#include <stdint.h>
#include <string.h>
#if !ITOA
#include <cstdio>
#endif

void Response::print(void)
{
	/*If VERBOSITY, OUTPUT_WARNINGS or OUTPUT_ERRORS is undefined,
	 * this method should be optimizes away by the compiler.*/

	DEBUG_PRINT("% Response: ");
	DEBUG_PRINT(" HTTP/1.0");
	DEBUG_TPRINT(' ', BYTE);
	uint16_t rc = ( response_code_int >> 5 ) * 100 + ( response_code_int & 0b00011111 );
	DEBUG_TPRINTLN(rc, DEC);
	if(content_type)
	{
		DEBUG_PRINT("Content-Type: ");
		DEBUG_PRINTLN(content_type);
	}
	Message::print();
}


Response::Response(
		const status_code _response_code,
		Request* _original_request = NULL ):
			Message(),
			response_code_int(_response_code),
			original_request(_original_request)
{
	object_type = RESPONSE;

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
}

#if RESPONSE_DESERIALIZATION
	Message::PARSER_RESULT Response::parse_header(const char* line, size_t size)
	{
		if(line[size - 2] != '\r' && line[size - 1] != '\n')
		{
			return LINE_INCOMPLETE;
		}

		if(!header)
		{
			header = (char*)ts_malloc(size - 2); /*We substract two because the
			\r\n is implicit*/
			if(!header)
			{
				return Message::OUT_OF_MEMORY;
			}
			header_length = size - 2;
			memcpy(header, line , size - 2);

			/*We do not really care about the HTTP version here, in fact, we could avoir saving it entirely*/

			char* index = header;

			while(true)
			{
				if( *index == ' ' )
				{
					response_code_int = (*(++index) - 48) << 5;
					response_code_int += atoi(index);
					break;
				}
				else if (index > (header + header_length))
				{
					return HEADER_MALFORMED;
				}
				index++;
			}
			Message::PARSER_RESULT res = PARSING_SUCESSFUL;

			/*We do not really care about the reason phrase here, in fact, we could avoir saving it entirely*/

			return PARSING_SUCESSFUL;
		}

		//Here we would parse for headers we want to keep

		return Message::parse_header(line, size);
	}
#endif

size_t Response::serialize( char* buffer, bool write)
{

	char* start = buffer;

	if( write )
	{
		*buffer++ = 'H'; *buffer++ = 'T'; *buffer++ = 'T'; *buffer++ = 'P'; *buffer++ = '/';
		*buffer++ = '1'; *buffer++ = '.'; *buffer++ = '0';
		*buffer++ = ' ';
		uint16_t rc = ( response_code_int >> 5 ) * 100 + ( response_code_int & 0b00011111 );
#if ITOA
		itoa(rc, buffer, 10);

#else
		sprintf(buffer, "%d", rc);
#endif
	}
	else { buffer += 9; }

	buffer += 3; //Moved the pointer after the last part of the response code

	if( write )
	{
		*buffer = '\r';
		*( buffer + 1 ) = '\n';
	}
	buffer += 2;

	if(content_type)
	{
		if( write ) { strcpy(buffer, CONTENT_TYPE); }
		buffer += 12; //strlen(CONTENT_TYPE); //Moves the pointer after "Content-Type"
		if( write )
		{
			*buffer = ':';
			*( buffer + 1 ) = ' ';
		}
		buffer += 2;
		if( write ) { strcpy(buffer, content_type); }
		buffer += strlen(content_type); //Moves the pointer after the content type
		if( write )
		{
			*buffer = '\r';
			*( buffer + 1 ) = '\n';
		}
		buffer += 2;
	}

	buffer += Message::serialize(buffer, write);

	return buffer - start;
}

//STATUS CODE DEFINITIONS
//Informational: 1xx
//const string<uint8_t> Response::CONTINUE_REASON_PHRASE = { "Continue", 100 };
//const string<uint8_t> Response::CONTINUE_CODE = { "100", 3 };
//const string<uint8_t> Response::SWITCHING_PROTOCOLS_REASON_PHRASE = { "Switching Protocols", 101 };
//const string<uint8_t> Response::SWITCHING_PROTOCOLS_CODE = { "101", 3 };

//Successful: 2xx
//const string<uint8_t> Response::OK_REASON_PHRASE = MAKE_STRING("OK");
//const string<uint8_t> Response::OK_CODE = MAKE_STRING("200");
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
//const string<uint8_t> Response::BAD_REQUEST_REASON_PHRASE = MAKE_STRING("Bad Request");
//const string<uint8_t> Response::BAD_REQUEST_CODE = MAKE_STRING("400");
//const string<uint8_t> Response::UNAUTHORIZED_REASON_PHRASE = { "Unauthorized", 401 };
//const string<uint8_t> Response::UNAUTHORIZED_CODE = { "401", 3 };
//const string<uint8_t> Response::PAYMENT_REQUIRED_REASON_PHRASE = { "Payment Required", 402 };
//const string<uint8_t> Response::PAYMENT_REQUIRED_CODE = { "402", 3 };
//const string<uint8_t> Response::FORBIDDEN_REASON_PHRASE = { "Forbidden", 403 };
//const string<uint8_t> Response::FORBIDDEN_CODE = { "403", 3 };
//const string<uint8_t> Response::NOT_FOUND_REASON_PHRASE = MAKE_STRING("Not Found");
//const string<uint8_t> Response::NOT_FOUND_CODE = MAKE_STRING("404");
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
//const string<uint8_t> Response::NOT_IMPLEMENTED_REASON_PHRASE = MAKE_STRING("Not Implemented");
//const string<uint8_t> Response::NOT_IMPLEMENTED_CODE = MAKE_STRING("501");
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
