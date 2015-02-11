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
}

void Response::print(void)
{
	/*If VERBOSITY, OUTPUT_WARNINGS or OUTPUT_ERRORS is undefined,
	 * this method should be optimized away by the compiler.*/

	DEBUG_PRINT("% Response: ");
	DEBUG_PRINT(" HTTP/1.0");
	DEBUG_PRINT(' ');
	uint16_t rc = ( _response_code_int >> 5 ) * 100 + ( _response_code_int & 0b00011111 );
	DEBUG_TPRINTLN(rc, DEC);
	if(content_type) //If there is a content type header field.
	{
		DEBUG_PRINT("Content-Type: ");
		DEBUG_PRINTLN(content_type);
	}
	Message::print();
}

//Since response deserialization is not always needed, it can be deactivated.
#if RESPONSE_DESERIALIZATION
Message::PARSER_RESULT Response::parse_header(const char* line, size_t size)
{
	//If the end of the line is not a CRLF.
	if(line[size - 2] != '\r' && line[size - 1] != '\n')
	{
		return LINE_INCOMPLETE;
	}

	if(!header) //If this is the first time we received a header line.
	{
		/*Allocate  a buffer for the line. Two is subtracted because the
		 * CRLF is implicit but a null char needs to be added.*/
		header = (char*)ts_malloc(size - 2 + 1);
		if(!header) //If memory allocation failed.
		{
			return Message::OUT_OF_MEMORY;
		}
		header_length = size - 2; //Compute the header length.
		memcpy(header, line , size - 2); //Copy the line to the buffer.

		header[size - 2] = '\0'; // Terminate the header.

		char* index = header; //Index for the header line.

		// If the first two letters of the response header are not HT (for HTTP/1.x).
		if(index[0] != 'H' || index[1] != 'T')
		{
			return HEADER_MALFORMED; // Wrong message type.
		}

		/*We do not really care about the HTTP version here, in fact, we
		 * can avoid saving it entirely.*/

		while(true) //Attempt to find the response code.
		{
			if( *index == ' ' ) //If a space is detected.
			{
				/* The response code appears at the end of the first space.
				 * The response code is compressed to fit in a byte.
				 * Save the first part of the response code.*/
				_response_code_int = (*(++index) - 48) << 5;

				//Save the second part of the response code.
				_response_code_int += atoi(++index);
				break; //Done with finding the response code.
			}
			//If we have reached the end of the header buffer.
			else if (index > (header + header_length))
			{
				return HEADER_MALFORMED; //No response code was present.
			}
			index++;
		}
		Message::PARSER_RESULT res = PARSING_SUCESSFUL;

		/*We do not really care about the reason phrase here, in fact, we could avoid
		 *  saving it entirely.*/

		return PARSING_SUCESSFUL;
	}

	//Here we would parse for headers we want to keep

	/* If the To-Url is present, this means the message comes from another
	 * resource.*/
	else if(!strncmp(TO_URL, line, 6))
	{
		///todo what if a previous "To-Url" has been parsed?

		line += 8; // Move past the "To-Url: ".

		const char* end = strchr(line, '\r');
		if(!end) // If the header end character was not found.
		{
			return HEADER_MALFORMED;
		}

		char* url = (char*)malloc(end - line + 1);

		if(!url) // If no memory could be allocated.
		{
			return OUT_OF_MEMORY;
		}

		url[end-line] = '\0'; // Add termination to the URL string.

		strncpy(url, line, end - line);

		to_url = new URL();

		if(!to_url) // If no memory could be allocated.
		{
			ts_free(url);
			return OUT_OF_MEMORY;
		}

		if(to_url->parse(url) != URL::VALID)
		{
			// Do not delete the allocated url string, it is now owned by the URL.
			return LINE_MALFORMED;
		}
	}

	 //Calls the parent to parse other headers.
	return Message::parse_header(line, size);
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
		//Convert the response code to a a uint16_t.
		uint16_t rc = ( _response_code_int >> 5 ) * 100 + ( _response_code_int & 0b00011111 );
#if ITOA
		itoa(rc, buffer, 10); //Write the response code.

#else
		sprintf(buffer, "%d", rc); //Write the response code.
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
		if( write ) { strcpy(buffer, LOCATION_STR); } //Write the header name.
		buffer += 8; //strlen(LOCATION); //Moves the pointer after "Location".
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
		if(write) { strcpy(buffer, TO_URL); }
		buffer += 6; // Equivalent to strlen(TO_URL);
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


//Response header fields
//static const char ACCEPT_RANGES[];
//static const char AGE[];
//static const char ALLOW[];
//static const char ETAG[];
#if LOCATION
	const char Response::LOCATION_STR[] = "Location";
#endif
//static const char PROXY_AUTHENTICATE[];
//static const char RETRY_AFTER[];
//static const char SERVER[];
//static const char VARY[];
//static const char WWW_AUTHENTICATE[];
