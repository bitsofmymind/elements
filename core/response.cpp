/* response.cpp - Source file for the Response class.
 * Copyright (C) 2015 Antoine Mercier-Linteau
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

//INCLUDES
#include <stdlib.h>
#include <pal/pal.h>
#include <stdint.h>
#include <string.h>
//If the itoa function is not available, use the equivalent from cstdio.
#if !ITOA
#include <cstdio>
#endif
#include "message.h"
#include "response.h"

Response::Response(const status_code code, Request* request):
	Message(),
	_response_code_int(code)
{
	set_type(RESPONSE); //Set the object type.

	set_request(request);

#if LOCATION //If the Location header field is used.
	location = NULL;
#endif
}

Response::~Response()
{
	if(_original_request)
	{
		delete _original_request;
	}
	else
	{
		delete from_url;
		delete to_url;
	}
}

void Response::print(void) const
{
	/*If VERBOSITY, OUTPUT_WARNINGS or OUTPUT_ERRORS is undefined,
	 * this method should be optimized away by the compiler.*/

	DEBUG_PRINT("% Response: ");
	DEBUG_PRINT(" HTTP/1.0");
	DEBUG_PRINT(' ');
	DEBUG_TPRINTLN(_response_code_int, DEC);
	if(content_type) //If there is a content type header field.
	{
		DEBUG_PRINT("content-type: ");
		DEBUG_PRINTLN(content_type);
	}
	Message::print();
}

//Since response deserialization is not always needed, it can be deactivated.
#if RESPONSE_DESERIALIZATION
Message::PARSER_RESULT Response::parse_header(char* line)
{
	char* field_value;

	if(!_response_code_int) //If this is the first time we received a header line.
	{
		char* space = strchr(line, ' '); // Find the first space.

		// If the first two letters of the response header are not HT (for HTTP/1.x).
		if(line[0] != 'H' || line[1] != 'T' || !space)
		{
			return HEADER_MALFORMED; // Wrong message type.
		}

		// The HTTP version is not saved yet.

		// Save the response code.
		_response_code_int = atoi(space + 1);

		// If there was no response code or the response code overflowed.
		if(_response_code_int == NONE_0 || _response_code_int == MAXIMUM_65535)
		{
			return HEADER_MALFORMED;
		}

		// The reason phrase is not important.

		return PARSING_SUCESSFUL;
	}

	//Here we parse for headers we want to keep.

	/* If the To-Url is present, this means the message comes from another
	 * resource.*/
	else if(!to_url && (field_value = extract_field_value("to-url", line)))
	{
		to_url = new URL();

		if(!to_url) // If no memory could be allocated.
		{
			return OUT_OF_MEMORY;
		}

		if(to_url->parse(field_value) != URL::VALID)
		{
			return LINE_MALFORMED;
		}
	}

	 // Calls the parent to parse other headers.
	return Message::parse_header(line);
}
#endif

size_t Response::serialize(char* buffer, bool write) const
{
	char* start = buffer; //Save the start of the buffer.

	if( write ) //If we should write the data.
	{
		//Write "HTTP/1.0".
		*buffer++ = 'H'; *buffer++ = 'T'; *buffer++ = 'T'; *buffer++ = 'P'; *buffer++ = '/';
		*buffer++ = '1'; *buffer++ = '.'; *buffer++ = '0';
		*buffer++ = ' ';

#if ITOA
		itoa(_response_code_int, buffer, _response_code_int); //Write the response code.

#else
		sprintf(buffer, "%d", _response_code_int); //Write the response code.
#endif
	}
	else { buffer += 9; }

	buffer += 3; //Move the pointer after the last part of the response code

	if( write ) //If we should write the data.
	{
		*buffer = '\r'; //Write a CRLF.
		*( buffer + 1 ) = '\n';
	}
	buffer += 2;
#if LOCATION
	if(location) //If location header is present.
	{
		if( write ) { strcpy(buffer, "location"); } //Write the header name.
		buffer += 8; //strlen("Location"); //Moves the pointer after "Location".
		if( write ) //If we should write the data.
		{
			*buffer = ':';
			*( buffer + 1 ) = ' ';
		}
		buffer += 2;
		if( write ) { strcpy(buffer, location); } //Write the location content.
		buffer += strlen(location); //Moves the pointer after the content type
		if( write ) //If we should write the data.
		{
			*buffer = '\r'; //Write a CRLF.
			*( buffer + 1 ) = '\n';
		}
		buffer += 2;
	}
#endif

	//Serialize other headers here.

	//To-URL FIELD SERIALIZATION
	if(to_url) // If the message came from another url.
	{
		if(write) { strcpy(buffer, "to-url"); }
		buffer += 6; // Equivalent to strlen("To-Url");
		if(write)
		{
			*buffer = ':';
			*( buffer + 1 ) = ' ';
		}
		buffer += 2;

		buffer += to_url->serialize(buffer, write);

		if(write)
		{
			*buffer = '\r';
			*( buffer + 1 ) = '\n';
		}
		buffer += 2;
	}

	 //Calls the parent to write other headers.
	buffer += Message::serialize(buffer, write);

	return buffer - start; //Return the size of the buffer.
}

void Response::set_request(Request* request)
{
	_original_request = request;

	if(request)
	{
		/*Invert the "to" and "from" urls to get the origin and destination
		of this response. This means it will follow that same route of its
		triggering request.*/
		to_url = request->get_from_url();
		from_url = request->get_to_url();
	}
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
