/* test_request_parsing.cpp - Header file for the Request parsing unit tests.
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

bool test_request_parsing(void)
{
	bool error = false;

	std::cout << "*** testing request parsing..." << std::endl;

	//######################################################

	std::cout << "   > normal message ... ";

	Request* request = new Request();

	const char* msg = "GET /res2/echo2/?v=r&b=y#asd HTTP/1.1\r\nContent-Length: 6\r\n\r\n123456";

	// If parsing the message failed.
	if(request->parse(msg, strlen(msg)) != Message::PARSING_COMPLETE)
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

	std::cout << "   > partial message ... ";

	request = new Request();

	 msg = "POST /res2/echo2/#asd HTTP/1.1\r\nConten";

	// If parsing the message indicated success.
	if(request->parse(msg, strlen(msg)) == Message::PARSING_SUCESSFUL)
	{
		std::cout << "(done)" << std::endl;
	}
	else
	{
		std::cout << "(error)" << std::endl;
		error = true;
	}

	// Deleting a partially parsed message could expose memory leaks.
	delete request;

	//######################################################

	std::cout << "   > normal message in parts ... ";

	request = new Request();

	// Correctly formed message.
	msg = "DELETE http://foo.com:7888/res2/echo2/?v=r&b=y#asd HTTP/1.1\r\nPragma: cache\r\nContent-Length: 6\r\n\r\n123456";

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

	std::cout << "   > malformed url arguments ... ";

	request = new Request();

	msg = "GET .?v=4&#asd HTTP/1.1\r\nContent-Length: 6\r\n\r\n123456";

	// If parsing the message worked.
	if(request->parse(msg, strlen(msg)) == Message::PARSING_COMPLETE)
	{
		std::cout << "(error)" << std::endl;
		error = true;
	}
	else // It was supposed to fail.
	{
		std::cout << "(done)" << std::endl;
	}

	delete request;

	//######################################################

	std::cout << "   > malformed header ... ";

	request = new Request();

	msg = "GET /res2/echo2/?v=r&b=y#asd HTTP/1.1\r\nContent \r\nLength: 6\r\n\r\n123456";

	// If parsing the message worked.
	if(request->parse(msg, strlen(msg)) != Message::HEADER_MALFORMED)
	{
		std::cout << "(error)" << std::endl;
		error = true;
	}
	else // It was supposed to fail.
	{
		std::cout << "(done)" << std::endl;
	}

	delete request;

	//######################################################

	std::cout << "   > Content-Length too large ... ";

	request = new Request();

	msg = "GET /res2/echo2/?v=r&b=y#asd HTTP/1.1\r\nContent-Length: 8\r\n\r\n123456";

	// If parsing the message worked.
	if(request->parse(msg, strlen(msg)) == Message::PARSING_COMPLETE)
	{
		std::cout << "(error)" << std::endl;
		error = true;
	}
	else // It was supposed to give use a PARSING_SUCCESSFUL.
	{
		std::cout << "(done)" << std::endl;
	}

	delete request;

	//######################################################

	std::cout << "   > Content-Length too small ... ";

	request = new Request();

	msg = "GET /res2/echo2/?v=r&b=y#asd HTTP/1.1\r\nContent-Length: 4\r\n\r\n123456";

	if(request->parse(msg, strlen(msg)) == Message::BODY_OVERFLOW)
	{
		std::cout << "(done)" << std::endl;
	}
	else // Wrong error code.
	{
		std::cout << "(error)" << std::endl;
		error = true;
	}

	delete request;

	//######################################################

	std::cout << "*** tested request parsing" << std::endl;

	return error;
}
