/* message.cpp - Source file for the Message class.
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
#include <utils/utils.h>
#include <pal/pal.h>
#include <string.h>
#include <utils/memfile.h>
#if !ITOA //If there is no ITOA function, use sprintf (located in cstdio)
	#include <cstdio>
#endif
#include "message.h"

Message::Message():
	_parsing_body(false),
	_current_line(NULL),
	_type(UNKNOWN),
	_dispatching_type(UNDETERMINED),
#if MESSAGE_AGE
	_age(get_uptime()), //Set the uptime at which the message was created.
#endif
	body(NULL),
	to_url_cursor(0),
	from_url_cursor(0),
	to_url(NULL),
	from_url(NULL),
	content_type(NULL)
{
}

Message::~Message()
{
	if(body) //If a File object for a body was created.
	{
		delete body;
	}

	free(_current_line);
}

void Message::print() const
{
	//Content-Length printing.
	DEBUG_PRINT("content-length");
	DEBUG_PRINT(": ");
	if(body) //If a message body is set.
	{
		DEBUG_TPRINTLN((uint32_t)body->get_size(), DEC); //Print its size.
	}
	else
	{
		DEBUG_PRINTLN('0'); //Else Content-Length is 0
	}

	if(to_url)
	{
		DEBUG_PRINT("to-url");
		DEBUG_PRINT(": ");
		to_url->print();
		DEBUG_PRINTLN();
	}

	if(from_url)
	{
		DEBUG_PRINT("from-url");
		DEBUG_PRINT(": ");
		from_url->print();
		DEBUG_PRINTLN();
	}
}

size_t Message::serialize(char* buffer, bool write) const
{
	//Note: The message header should have been serialized by a child class.

	char* start = buffer; //Keep a pointer to the start of the buffer.

	if(body && body->get_size()) // If there is a message body.
	{
		//CONTENT-LENGTH FIELD SERIALIZATION
		if( write ){ strcpy(buffer, "content-length"); }
		buffer += 14; //Equivalent to strlen("Content-Length");
		if( write )
		{
			*buffer = ':';
			*(buffer + 1) = ' ';
		}
		buffer += 2;

		size_t cl; //The size of the body.
		if(body){ cl = body->get_size(); } //If there is a body, get its length.
		else { cl = 0; } //Else Content-Length is 0.

		if( write )
		{
			//Convert the content-length integer to a string.
	#if ITOA
			itoa(cl, buffer, 10);
	#else
			sprintf(buffer, "%d", cl);
	#endif
		}

		/*This loop finds the number of chars in the content-length string.
		 * itoa and sprintf return the number of bytes written but since a
		 * buffer is generally not allocated when this method is called with
		 * write = false, we cannot use either.*/
		do
		{
			/* Finding the number of time cl divides by 10 gives us the length of
			the string. */
			buffer++;
			cl /= 10;
		}while( cl > 0 );

		if( write )
		{
			*buffer = '\r';
			*(buffer + 1) = '\n';
		}
		buffer += 2;
	}
	//CONTENT-TYPE FIELD SERIALIZATION
	if(content_type) //If the message has a Content-Type.
	{
		if(write) { strcpy(buffer, "content-type"); }
		buffer += 12; // Equivalent to strlen("Content-Type");
		if(write)
		{
			*buffer = ':';
			*( buffer + 1 ) = ' ';
		}
		buffer += 2;
		if(write) { strcpy(buffer, content_type); }
		buffer += strlen(content_type);
		if(write)
		{
			*buffer = '\r';
			*( buffer + 1 ) = '\n';
		}
		buffer += 2;
	}

	//FROM-URL FIELD SERIALIZATION
	if(from_url) // If the message came from another url.
	{
		if(write) { strcpy(buffer, "from-url"); }
		buffer += 8; // Equivalent to strlen("From-Url");
		if(write)
		{
			*buffer = ':';
			*( buffer + 1 ) = ' ';
		}
		buffer += 2;

		buffer += from_url->serialize(buffer, write);

		if(write)
		{
			*buffer = '\r';
			*( buffer + 1 ) = '\n';
		}
		buffer += 2;
	}

	//CRLF BETWEEN FIELDS AND BODY
	if( write )
	{
		*buffer = '\r';
		*(buffer + 1) = '\n';
	}
	buffer += 2;

	/*Since the start of the buffer was saved, the difference between the two
	 * gives us the length of the serialize message.*/
	return buffer - start;
}

Message::PARSER_RESULT Message::parse(const char* data, size_t size)
{
	if(size < 1) // If size is 0.
	{
		return SIZE_IS_0;
	}

	if(_parsing_body) //If we are done parsing the header.
	{
		return store_body(data, size); //Forward the buffer to store_body().
	}

	size_t current_line_length;

	if(!_current_line)
	{
		current_line_length = 0;
	}
	else
	{
		current_line_length = strlen(_current_line);
	}

	/* Allocate a new line buffer. If this is the first time data is received,
	 * (_current_line = NULL) realloc will function as a malloc.*/
	char* new_line_buffer = (char*)realloc(_current_line, current_line_length + size + 1);

	if(!new_line_buffer) // If there is no memory left.
	{
		return OUT_OF_MEMORY;
	}

	_current_line = new_line_buffer;

	// Copy the data to the new line buffer.
	memcpy(_current_line + current_line_length, data, size);

	_current_line[size + current_line_length] = '\0'; // Terminate the line.

	while(true) // Parse all the lines we have in the buffer.
	{
		char* line_end = strchr(_current_line, '\r'); // Find the end of the line.

		/* If no end of line could be found, or the end (CRLF of the line is part
		 * of another packet. */
		if(!line_end || *(line_end + 1) == '\0')
		{
			return PARSING_SUCESSFUL; // More data is required.
		}

		*line_end++ = '\0'; // Terminate the line.

		if(*(line_end++) != '\n') // If the line is not properly terminated.
		{
			return LINE_MALFORMED;
		}

		// Parse the line.
		PARSER_RESULT result = parse_header(_current_line);

		// The length of the remaining data in the buffer.
		size_t remainder_length = strlen(line_end) + 1;

		switch(result)
		{
			case PARSING_SUCESSFUL:

				// Move the remaining data back.
				for(size_t i = 0; i < remainder_length; i++)
				{
					_current_line[i] = line_end[i];
				}

				// Shrink the buffer that hold the data being parsed.
				_current_line = (char*)realloc(_current_line, remainder_length);

				if(*(_current_line) == '\r' && *(_current_line + 1) == '\n') // If the line is empty.
				{
					_parsing_body = true; // The body is now being parsed.
					return store_body(_current_line + 2, strlen(_current_line + 2));
				}

				break;
			default: // Something went wrong.
				return result;
		}
	}

	/* This code is in theory not reachable, but included otherwise to
	 * satisfy the compiler. */

	return HEADER_MALFORMED;
}

Message::PARSER_RESULT Message::parse(const char* buffer)
{
	return parse(buffer, strlen(buffer));
}

void Message::set_body(File* f, const char* mime)
{
	body = f;
	content_type = mime;
}

File* Message::unset_body(void)
{
	File* f = body;
	body = NULL;
	content_type = NULL;
	return f;
}

uint8_t Message::to_destination(void) const
{
	/*If dispatching is not relative, there is no real way to tell where
	 * the message currently is relative to its destination.*/
	if(get_dispatching_type() != RELATIVE)
	{
		///TODO make this a configurable value.
		return 255; //Return the maximum depth a message may be at.
	}

	// If there are not resources in the url.
	if(to_url->get_resources()->get_item_count() == 0)
	{
		// The url is assumed to be ".".
		return 0;
	}

	/*Else return the depth. It is computed using the cursor. 1 is subtracted
	 * from the total because a message always has at least one resource
	 * in its url.*/
	return to_url->get_resources()->get_item_count() - to_url_cursor - 1;
}

void Message::next(void)
{
	//Check if there is a next resource.
	if(to_url_cursor < to_url->get_resources()->get_item_count() - 1)
	{
		to_url_cursor++; //Increment the cursor.
	}
}

void Message::previous(void)
{
	if(to_url_cursor > 0) //Check if there is a previous resource.
	{
		to_url_cursor--; //Decrement the cursor.
	}
}

Message::PARSER_RESULT Message::parse_header(char* line)
{
	char* field[2];

	if(!extract_field(line, field))
	{
		return LINE_MALFORMED;
	}

	/*The content length line gets special treatment because it gives the
	 * size of the body, for the rest of the fields, we should proceed normally
	 * and store them in their structure.*/
	//If the header line is "content-length".
	if(!body && !strcmp(field[0], "content-length"))
	{
		// Get the content length by converting from its textual representation.
		size_t content_length = atoi(field[1]);

		// Create a buffer to hold the body.
		char* buffer = (char*)malloc(content_length);

		if(!buffer) // If there is no memory left.
		{
			return OUT_OF_MEMORY;
		}

		/*Create a body object the size of content_length. Set its memory
		 * pointer to NULL because we will be giving a memory area later
		 * when the body is actually received.*/
		body = new MemFile(buffer, content_length, false);

		if(!body) // If creating the body failed.
		{
			free(buffer);
			return OUT_OF_MEMORY; // Return the appropriate error.
		}
	}
	/* If the from-url is present, this means the message comes from another
	 * resource.*/
	else if(!strcmp(field[0], "from-url"))
	{
		if(!from_url)
		{
			from_url = new URL();

			if(!from_url) // If no memory could be allocated.
			{
				return OUT_OF_MEMORY;
			}
		}

		if(from_url->parse(field[1]) != URL::VALID)
		{
			// Do not delete the allocated url string, it is now owned by the URL.
			return LINE_MALFORMED;
		}
	}
	/* If the To-Url is present, this means the message comes from another
	 * resource.*/
	else if(!strcmp(field[0], "to-url"))
	{
		if(!to_url)
		{
			to_url = new URL();

			if(!to_url) // If no memory could be allocated.
			{
				return OUT_OF_MEMORY;
			}
		}

		if(to_url->parse(field[1]) != URL::VALID)
		{
			// Do not delete the allocated url string, it is now owned by the URL.
			return LINE_MALFORMED;
		}
	}

	//Store fields in buffers

	return PARSING_SUCESSFUL; // Parsing that line was successful.
}

bool Message::extract_field(char* line, char* field[2]) const
{
	size_t field_body_start = false;

	for(size_t i = 0; i < SIZE_MAX; i++)
	{
		char c = line[i];

		if(field_body_start)
		{
			if(c == '\0')
			{
				field[1] = line + field_body_start;
				return true;
			}

			continue;
		}

		if(c == ':') // End of field-name found.
		{
			field[0] = line;
			line[i] = '\0';

			while(line[i++ + 1] == ' '); // Skip all the white spaces.

			field_body_start = i; // field-name has been parsed.
			continue;
		}

		if(c < 32) // If the character is a control code.
		{
			return false; // Field is invalid.
		}

		// If a letter in the field_name is upper case.
		if(c >= 'A' && c <= 'Z')
		{
			line[i] += 32; // Change it to lower case.
		}
	}

	return false; // The field's format is incorrect.
}

Message::PARSER_RESULT Message::store_body(const char* buffer, size_t size)
{
	if(!body) //If no body object is set.
	{
		// Free the data buffer, it is no longer needed.
		free(_current_line);
		_current_line = NULL;

		return PARSING_COMPLETE; //Parsing is done.
	}

	//The size of the body, it was set in parse_header().
	size_t cl = body->get_size();

	/*If for some reason, size got larger than the size of the remaining body
	data.*/
	if(size + body->get_cursor() > cl)
	{
		return BODY_OVERFLOW; // Error;
	}

	body->write(buffer, size);

	// If we have received all the body.
	if(body->get_cursor() >= body->get_size())
	{
		// Free the data buffer, it is no longer needed.
		free(_current_line);
		_current_line = NULL;

		return PARSING_COMPLETE; // Parsing is done.
	}

	// We are still expecting parts of the body but everything is good so far.
	return PARSING_SUCESSFUL;
}
