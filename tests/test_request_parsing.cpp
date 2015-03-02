/* test_request_parsing.cpp - Source file for the Request parsing unit tests.
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
#include <core/request.h>
#include <utils/utils.h>
#include <utils/memfile.h>

bool test_parsing(Message* message, const char* data, Message::PARSER_RESULT expected_result)
{
	bool error = false;

	// If parsing the message failed.
	if(message->parse(data, strlen(data)) != expected_result)
	{
		std::cout << "(error)" << std::endl;
		error = true;
	}
	else
	{
		std::cout << "(done)" << std::endl;
	}

	delete message;

	return error;
}

bool test_request_parsing(void)
{
	bool error = false;

	std::cout << "*** testing request parsing..." << std::endl;

	//######################################################

	std::cout << "   > normal message ... ";

	error |= test_parsing(
		new Request(),
		"GET /res2/echo2/?v=r&b=y#asd HTTP/1.1\r\nContent-Length: 6\r\n\r\n123456",
		Message::PARSING_COMPLETE
	);

	//######################################################

	std::cout << "   > simplest message ... ";

	error |= test_parsing(
		new Request(),
		"GET / HTTP/1.1\r\n\r\n",
		Message::PARSING_COMPLETE
	);

	//######################################################

	std::cout << "   > simple message #1 ... ";

	error |= test_parsing(
		new Request(),
		"GET /resource HTTP/1.1\r\n\r\n",
		Message::PARSING_COMPLETE
	);

	//######################################################

	std::cout << "   > simple message with ending slash ... ";

	error |= test_parsing(
		new Request(),
		"GET /resource/ HTTP/1.1\r\n\r\n",
		Message::PARSING_COMPLETE
	);

	//######################################################

	std::cout << "   > relative URL ... ";

	error |= test_parsing(
		new Request(),
		"GET ./res0/../res1/..#23 HTTP/1.1\r\n\r\n",
		Message::PARSING_COMPLETE
	);

	//######################################################

	std::cout << "   > message to self ... ";

	error |= test_parsing(
		new Request(),
		"GET . HTTP/1.1\r\n\r\n",
		Message::PARSING_COMPLETE
	);

	//######################################################

	std::cout << "   > partial message ... ";

	error |= test_parsing(
		new Request(),
		"POST /res2/echo2/#asd HTTP/1.1\r\nConten",
		Message::PARSING_SUCESSFUL
	);

	//######################################################

	std::cout << "   > normal message in parts ... ";

	Request* request = new Request();

	// Correctly formed message.
	const char* msg = "DELETE http://foo.com:7888/res2/echo2/?v=r&b=y#asd HTTP/1.1\r\nPragma: cache\r\nFrom-Url: /client/127.0.0.1/port/6523\r\nContent-Length: 6\r\n\r\n123456";

	// The length of message to parse, the 400 gets the remainder.
	size_t lengths[] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 10, 5, 400};

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

		Message::PARSER_RESULT result = request->parse(part, strlen(part));

		free(part); // Message will have been copied the content of part.

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

	delete request;

	//######################################################

	std::cout << "   > request with body arguments ... ";

	request = new Request();

	// If parsing the message failed.
	if(request->parse("POST /res2/echo2 HTTP/1.1\r\nContent-Length: 10\r\n\r\ni=500&st=1") != Message::PARSING_COMPLETE)
	{
		std::cout << "(error)" << std::endl;
		error |= true;
	}
	else
	{
		std::cout << "(done)" << std::endl;
	}

	//######################################################

	std::cout << "   > finding non existing body argument ... ";

	char arg[10];

	// If parsing the message failed.
	if(request->find_arg("blah", arg, 4))
	{
		std::cout << "(error)" << std::endl;
		error = true;
	}
	else
	{
		std::cout << "(done)" << std::endl;
	}

	//######################################################

	std::cout << "   > finding existing body argument ... ";

	request->find_arg("i", arg, 4);

	if(strcmp(arg, "500"))
	{
		std::cout << "(error)" << std::endl;
		error = true;
	}
	else
	{
		std::cout << "(done)" << std::endl;
	}

	//######################################################

	std::cout << "   > finding part of a body argument ... ";

	request->find_arg("i", arg, 2);

	arg[2] = '\0';

	if(strcmp(arg, "50"))
	{
		std::cout << "(error)" << std::endl;
		error = true;
	}
	else
	{
		std::cout << "(done)" << std::endl;
	}

	delete request;

	//######################################################

	std::cout << "   > malformed url arguments ... ";

	error |= test_parsing(
		new Request(),
		"GET .?v=4&#asd HTTP/1.1\r\nContent-Length: 6\r\n\r\n123456",
		Message::HEADER_MALFORMED
	);

	//######################################################

	std::cout << "   > malformed header ... ";

	error |= test_parsing(
		new Request(),
		"GET /res2/echo2/?v=r&b=y#asd HTTP/1.1\r\nContent \r\nLength: 6\r\n\r\n123456",
		Message::HEADER_MALFORMED
	);

	//######################################################

	std::cout << "   > duplicate header ... ";

	// The duplicated header should be ignored.
	error |= test_parsing(
		new Request(),
		"GET /res2/echo2/ HTTP/1.1\r\nFrom-Url: /url1\r\nFrom-Url: /url2\r\nContent-Length: 6\r\n\r\n123456",
		Message::PARSING_COMPLETE
	);

	//######################################################

	std::cout << "   > Content-Length too large ... ";

	error |= test_parsing(
		new Request(),
		"GET /res2/echo2/?v=r&b=y#asd HTTP/1.1\r\nContent-Length: 8\r\n\r\n123456",
		Message::PARSING_SUCESSFUL
	);

	//######################################################

	std::cout << "   > Content-Length too small ... ";

	error |= test_parsing(
		new Request(),
		"GET /res2/echo2/?v=r&b=y#asd HTTP/1.1\r\nContent-Length: 4\r\n\r\n123456",
		Message::BODY_OVERFLOW
	);

	//######################################################

	std::cout << "   > parsing a response ... ";

	error |= test_parsing(
		new Request(),
		"HTTP/1.1 200 OK\r\nContent \r\nLength: 6\r\n\r\n123456",
		Message::HEADER_MALFORMED
	);

	std::cout << "*** tested request parsing" << std::endl;

	return error;
}
