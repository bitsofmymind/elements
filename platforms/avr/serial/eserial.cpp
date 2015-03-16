/* eserial.cpp - Source file for the ESerial class.
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

#include "eserial.h"
#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "../avr_pal.h"
#include <string.h>

/** The current instance of the ESerial resource. Needed for interrupt
 * service routines.*/
static ESerial* instance;

ESerial::ESerial():
		Resource(),
		buffer_size(0),
		index(0),
		buffer(0),
		age(0)
{
	instance = this; // Set the running instance.
}

void ESerial::receive(uint8_t c)
{
	if(index >= buffer_size) // If we have overrun the buffer.
	{
		// Allocate a new larger buffer.
		char* new_buffer = (char*)ts_malloc(buffer_size + MESSAGE_BUFFER_INCREMENT_SIZE);

		if(new_buffer != NULL) // If allocation succeeded.
		{
			if(buffer) // If there is previously allocated buffer.
			{
				// Copy the old buffer over to the new buffer.
				memcpy(new_buffer, buffer, buffer_size);
				ts_free(buffer); // Free the old buffer.
			}
			// Increase the size of the buffer.
			buffer_size += MESSAGE_BUFFER_INCREMENT_SIZE;
			buffer = new_buffer; // Replace the old buffer.
		}
		else // Allocation of a new buffer failed.
		{
			if(buffer) // if there is a currently allocated buffer.
			{
				ts_free(buffer); // Free it.
				buffer = NULL;
			}

			index = buffer_size = 0; // Reset size and index.

			 /* Cannot receive more characters for this message. If extra
			  * characters keep on coming, they will parsed as an invalid
			  * message and discarded there.*/
			return;
		}
	}

	age = get_uptime(); // We just received a new character.

	buffer[index++] = c; // Write the character to the buffer.

	DEBUG_PRINT(c); // echo back the character that was received.

	schedule(ASAP); // Schedule the resource to be run ASAP to parse the message.
}


void ESerial::run(void)
{
	size_t len; // The length of the message.
	char* buf; // A buffer to hold the received part of the message.

	ATOMIC // Disable interrupts.
	{
		 // If the parsing delay has not expired.
		if(age + DELAY > get_uptime())
		{
			schedule(10); // Wait 10 ms.
			return; // More bytes could be received.
		}

		len = index; // Save the length of the message.
		buf = buffer; // Save the buffer.

		// Reset the USART character buffer.
		index = buffer_size = 0;
		buffer = NULL;
	}

	Request* request = new Request();

	if(!request) // If there is not enough memory for the request.
	{
		ts_free(buf); // Free the message buffer.

		return; // Not enough memory to transform the message into a request.
	}

	for(size_t i = 1; i < len; i++) // For each character in the message.
	{
		if( buf[i - 1] == ';' && buf[i] == ';') // ;; are changed to /r and /n.
		{
			buf[i - 1] = '\r';
			buf[i] = '\n';
		}
	}

	// Parse the message.
	Message::PARSER_RESULT res = request->parse(buf, len);

	ts_free(buf); // Done parsing that part of the message.

	switch(res)
	{
		case Message::PARSING_COMPLETE:
			DEBUG_PRINTLN("Parsing complete, sending");
			dispatch(request); // dispatch the message to the framework.
			break;
		case Message::PARSING_SUCESSFUL:
			DEBUG_PRINTLN("Parsing incomplete"); // Message is incomplete.

			delete request; // Partial messages are not processed.
			break;
		default:
			DEBUG_PRINT("Parsing Error: "); // There was a parsing error.
			Debug::println((uint8_t)res, 10);

			// Cannot process the message so delete all buffers.
			delete request;
	}

	ATOMIC // Disable interrupts.
	{
		/* If some characters were received while we were parsing the current
		 * message. */
		if(index)
		{
			schedule(ASAP); // Run the resource ASAP.
			return; // Done running the resource.
		}
	}

	schedule(NEVER); // Wait for more bytes to arrive.
}

Response::status_code ESerial::process(const Response* response)
{
	File* body = response->get_body(); // Will print the body of the response.
	print_transaction(response);

	char buffer[10]; // Will hold part of the body while we read it.

	if(body) // If the message has a body.
	{
		uint8_t read; // The number of bytes read.

		do
		{
			 // Attempt to read ten bytes from the body.
			read = body->read(buffer, 10);

			for(uint8_t i = 0; i < read; i++) // Output the bytes read.
			{
				DEBUG_PRINT(buffer[i]);
			}
			// No longer works?
			//DEBUG_NPRINT(buffer, read);

		} while(read == 10); // While there is data left in the body.

		DEBUG_PRINTLN(); // Skip a line.
	}

	delete response; // Done with the response.

	return Response::OK_200; // Response was successfully processed.
}

/// Interrupt service routine for the USART.
ISR(USART_RX_vect)
{
	instance->receive(UDR0); // Forward the received character to the framework.
}
