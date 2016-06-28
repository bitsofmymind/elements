/* test_response.cpp - Header file for the Response parsing unit tests.
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

#include <iostream>
#include <stdlib.h>
#include "string.h"
#include <core/response.h>
#include <test_request.h>
#include <utils/utils.h>
#include <utils/memfile.h>

bool test_response(void)
{
	bool error = false;

	std::cout << "*** testing response parsing..." << std::endl;

	//######################################################

	std::cout << "   > normal response ... ";

	error |= test_parsing(
		new Response(Response::NONE_0, NULL),
		"HTTP/1.1 200 OK\r\nContent-Length: 6\r\nSome-Field: 12\r\nOther-Field: example\r\n\r\n123456",
		Message::PARSING_COMPLETE
	);

	//######################################################

	std::cout << "   > short response header ... ";

	error |= test_parsing(
		new Response(Response::NONE_0, NULL),
		"HTTP/1.1 200 \r\n\r\n",
		Message::PARSING_COMPLETE
	);

	//######################################################

	std::cout << "   > partial response ... ";

	error |= test_parsing(
		new Response(Response::NONE_0, NULL),
		"HTTP/1.1 500 SERVER ERROR\r\nConten",
		Message::PARSING_SUCESSFUL
	);

	//######################################################

	std::cout << "   > partial response header ... ";

	error |= test_parsing(
		new Response(Response::NONE_0, NULL),
		"HTTP/1.1 50",
		Message::PARSING_SUCESSFUL
	);

	//######################################################

	std::cout << "   > normal message in parts ... ";

	Response* response = new Response(Response::NONE_0, NULL);

	// Correctly formed message.
	const char* msg = "HTTP/1.1 400 BAD REQUEST \r\nPragma: cache\r\nFrom-Url: /client/127.0.0.1/port/6523\r\nContent-Length: 6\r\n\r\n123456";

	// The length of message to parse, the 400 gets the remainder.
	size_t lengths[] = {1, 1, 1, 1, 1, 1, 6, 1, 1, 1, 1, 2, 10, 5, 400};

	const char* message_index = msg;

	for(uint32_t i = 0; i < 15; i++)
	{
		// Make sure we do not go past the end of the string.
		size_t length = (size_t)message_index + lengths[i] > (size_t)msg + strlen(msg) ? strlen(msg) + (size_t)msg - (size_t)message_index : lengths[i];

		// Get the current part from the message.
		char* part = strncpy((char*)malloc(length + 1), message_index, length);
		part[length] = '\0'; // strncpy does not terminate string;

		// Move the pointer in the message to the next part.
		message_index += length;

		Message::PARSER_RESULT result = response->parse(part, strlen(part));

		free(part); // Message will have copied the content of part.

		if(result == Message::PARSING_COMPLETE) // Done parsing.
		{
			std::cout << "(done)" << std::endl;
			break;
		}
		else if(result == Message::PARSING_SUCESSFUL) // Not done parsing yet.
		{
			continue; // Keep going.
		}
		else // Error
		{
			std::cout << "(error)" << std::endl;
			error = true;
			break;
		}
	}

	delete response;

	//######################################################

	std::cout << "   > response code missing ... ";

	error |= test_parsing(
		new Response(Response::NONE_0, NULL),
		"HTTP/1.1 SERVER ERROR\r\n\r\n",
		Message::HEADER_MALFORMED
	);

	//######################################################

	std::cout << "   > Incomplete line ... ";

	error |= test_parsing(
		new Response(Response::NONE_0, NULL),
		"HTTP/1.1 500 SERVER ERROR\r\nTo-Url\r\n\r\n",
		Message::LINE_MALFORMED
	);

	//######################################################

	std::cout << "   > response code int32_t overflow ... ";

	error |= test_parsing(
		new Response(Response::NONE_0, NULL),
		"HTTP/1.1 5000000000000 SERVER ERROR\r\n\r\n",
		Message::HEADER_MALFORMED
	);

	//######################################################

	std::cout << "   > HTTP version missing ... ";

	error |= test_parsing(
		new Response(Response::NONE_0, NULL),
		"500 SERVER ERROR\r\n\r\n",
		Message::HEADER_MALFORMED
	);

	//######################################################

	std::cout << "   > Content-Length too large ... ";

	error |= test_parsing(
		new Response(Response::NONE_0, NULL),
		"HTTP/1.0 200 OK\r\nContent-Length: 8\r\n\r\n123456",
		Message::PARSING_SUCESSFUL
	);

	//######################################################

	std::cout << "   > Content-Length too small ... ";

	error |= test_parsing(
		new Response(Response::NONE_0, NULL),
		"HTTP/1.0 200 OK\r\nContent-Length: 4\r\n\r\n123456",
		Message::BODY_OVERFLOW
	);

	//######################################################

	std::cout << "   > incomplete response ... ";

	error |= test_parsing(
		new Response(Response::NONE_0, NULL),
		"HTTP/1.0 200 OK\r\nContent-Length: 8\r\n",
		Message::PARSING_SUCESSFUL
	);

	//######################################################

	std::cout << "   > empty response ... ";

	error |= test_parsing(
		new Response(Response::NONE_0, NULL),
		"",
		Message::SIZE_IS_0
	);

	//######################################################

	std::cout << "   > reusing same response object ... ";

	response = new Response(Response::NONE_0, NULL);
	const char* message = "HTTP/1.1 200 OK\r\nContent-Length: 6\r\n\r\n123456";

	if(response->parse(message, strlen(message)) != Message::PARSING_COMPLETE)
	{
		std::cout << "(error)" << std::endl;
		error = true;
	}
	else
	{
		/* The extra data is automatically added to the body of the message.
		 * Once we have gone beyond the allocated size of the body, a BODY_OVERFLOW
		 * ERROR should be returned. */
		if(response->parse(message, strlen(message)) != Message::BODY_OVERFLOW)
		{
			std::cout << "(error)" << std::endl;
			error = true;
		}
		else
		{
			std::cout << "(done)" << std::endl;
		}
	}

	delete response;

	//######################################################

	std::cout << "   > null character in response ... ";

	error |= test_parsing(
		new Response(Response::NONE_0, NULL),
		"HTTP/1.1 200 OK\r\0\nContent-Length: 6\r\n\r\n123456",
		Message::PARSING_SUCESSFUL
	);

	//######################################################

	std::cout << "   > buffer size mismatch ... ";

	error |= test_parsing(
		new Response(Response::NONE_0, NULL),
		"HTTP/1.1 200 OK\r\nContent-Length: 6\r\n\r\n123456",
		Message::PARSING_COMPLETE,
		200
	);

	//######################################################

	std::cout << "   > non null terminated buffer ... ";

	error |= test_parsing(
		new Response(Response::NONE_0, NULL),
		"HTTP/1.1 200 OK\r\nContent-Length: 6\r\n\r\n123456",
		Message::PARSING_SUCESSFUL,
		20
	);

	//######################################################

	std::cout << "   > incorrectly terminated lines (\\r) ... ";

	error |= test_parsing(
		new Response(Response::NONE_0, NULL),
		"HTTP/1.1 200 OK\rContent-Length: 6\r123456",
		Message::LINE_MALFORMED
	);

	//######################################################

	std::cout << "   > incorrectly terminated lines (\\n) ... ";

	error |= test_parsing(
		new Response(Response::NONE_0, NULL),
		"HTTP/1.1 200 OK\nContent-Length: 6\r\n123456",
		Message::PARSING_SUCESSFUL // Interpreted as an incomplete buffer.
	);

	//######################################################

	std::cout << "   > parsing a request ... ";

	error |= test_parsing(
		new Response(Response::NONE_0, NULL),
		"POST /res2/echo2/#asd HTTP/1.1\r\nContent-Length: 6\r\n\r\n123456",
		Message::HEADER_MALFORMED
	);

	//######################################################

	std::cout << "   > parsing a malicious request ... ";

	error |= test_parsing(
		new Response(Response::NONE_0, NULL),
		"HTTP/1.0 /res2/echo2/#asd HTTP/1.1\r\nContent-Length: 6\r\n\r\n123456",
		Message::HEADER_MALFORMED
	);

	std::cout << "*** tested response parsing" << std::endl;

	std::cout << "*** testing response serializing..." << std::endl;

	std::cout << "   > serializing valid response ... ";

	response = new Response();
	message = "HTTP/1.0 200\r\ncontent-length: 6\r\nsome-field: 12\r\nother-field: example\r\n\r\n123456";

	if(!response->parse(message, strlen(message)))
	{
		char* buffer = (char*)alloca(strlen(message) + 1);
		response->serialize(buffer, true);
		buffer[strlen(message)] = '\0';

		if(strcmp(buffer, message))
		{
			error = true;
			std::cout << "(error)" << std::endl;
		}
		else
		{
			std::cout << "(done)" << std::endl;
		}
	}
	else
	{
		std::cout << "(error)" << std::endl;
	}

	delete response;

	std::cout << "*** tested request serializing" << std::endl;

	return error;
}
