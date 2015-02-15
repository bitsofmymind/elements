/* request.cpp - Source file for the Request class.
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
#include <stdint.h>
#include <pal/pal.h>
#include <ctype.h>
#include <string.h>
#include "message.h"
#include "request.h"
#include "url.h"

Request::Request():
	Message(),
	method(NULL)
{
	set_type(REQUEST);

	//Allocates two blank URL objects for the source and destination urls.
	to_url = new URL();
	from_url = new URL();
}

Request::~Request()
{
	delete to_url;
	delete from_url;
}

void Request::print(void)
{
	/*If VERBOSITY, OUTPUT_WARNINGS or OUTPUT_ERRORS is undefined,
	 * this method should be optimized away by the compiler.*/
	DEBUG_PRINT(" % Request: ");

	//Prints the HTTP request header.
	DEBUG_PRINT(method);
	DEBUG_PRINT(' ');
	to_url->print(); //Prints the url.
	DEBUG_PRINTLN(" HTTP/1.0");

	Message::print(); //Hands of printing to parent method.
}

#if REQUEST_SERIALIZATION
size_t Request::serialize(char* buffer, bool write) const
{
	char* start = buffer; //The start of the buffer.

	//HTTP REQUEST HEADER SERIALIZATION

	//Serialize the method.
	if( write ){ strcpy(buffer, method); }
	buffer += strlen(method);

	///TODO merge with method write.
	if( write ){ *buffer = ' '; }
	buffer++;

	buffer += to_url->serialize(buffer, write); //Serialize the destination URL.

	//Serialize the HTTP version.
	if(write)
	{
		/*Note: In order so save on RAM, the following string is hardcoded in
		 * program memory.*/

		*buffer++ = ' ';
		*buffer++ = 'H'; *buffer++ = 'T'; *buffer++ = 'T'; *buffer++ = 'P'; *buffer++ = '/';
		*buffer++ = '1';
		*buffer++ = '.';
		*buffer++ = '0';
		*buffer++ = '\r';
		*buffer++ = '\n';
	}
	else
	{
		buffer += 11; //The length of " HTTP/1.x\r\n".
	}

	//Calls the parent method to do generic message serialization
	buffer += Message::serialize(buffer, write);

	return buffer - start; //Return the size of the serialized message.
}
#endif

Message::PARSER_RESULT Request::parse_header(const char* line, size_t size)
{
	//If the received line does not end with CRLF.
	if(line[size - 2] != '\r' && line[size - 1] != '\n')
	{
		return LINE_INCOMPLETE; //Return the appropriate error.
	}

	if(!header) //If we are parsing the first line of the header.
	{
		/* Allocates space for the request header line. 1 is subtracted to that
		because the CRLF is implicit but a terminating null character is needed.*/
		header = (char*)ts_malloc(size - 1);
		if(!header) // If allocating memory failed.
		{
			return Message::OUT_OF_MEMORY; //Return the appropriate error.
		}

		header_length = size - 2; //Set the header length.

		header[header_length] = '\0'; // Terminate the string.

		//Copy the content of the buffer to the header buffer.
		memcpy(header, line , size - 2);

		char* index = header; //Index for the header buffer.

		// HTTP REQUEST METHOD PARSING

		while(true)
		{
			if(*index == ' ') // If a space is found.
			{
				//This means we have reached the end of the method part.
				method = header; //Save the location of the method.
				/*Replace the space with a null character so the method can
				 * be used like a normal string.*/
				*index = '\0';
				break; //Done parsing the method.
			}
			//If the index has incremented past the header.
			else if(index > (header + header_length))
			{
				//The HTTP method was not found so the header is malformed.
				return HEADER_MALFORMED;
			}
			//If a letter in the header is upper case.
			if(*index >= 'A' && *index <= 'Z')
			{
				/*TODO this should be done when parts of the header are saved.
				 * Should save a bit of program memory.*/
				*index += 32; //change it to lower case.
			}
			index++; //Increment the index to the next character.
		}

		//HTTP REQUEST URL PARSING

		if(to_url->parse(++index) == URL::INVALID) // Parse the url.
		{
			return HEADER_MALFORMED;
		}

		/* We do not really care about the HTTP version for now,
		 * everything that comes next is assumed to be valid.*/

		return PARSING_SUCESSFUL;
	}
	//Else the header's first line has already been parsed.


	//Here we would parse for request specific headers we want to keep.

	//Hand generic parsing to the parent method.
	return Message::parse_header(line, size);
}

#if BODY_ARGS_PARSING
uint8_t Request::find_arg(const char* key, char* value, uint8_t max_size) const
{
	/* Note: Arguments are given in the following form:
	 * key1=value1&key2value2&key3=value3&...*/

	if(!body) //If there is no body for this request.
	{
		return 0; //There cannot be body arguments so return 0.
	}

	//Argument parser states.
	enum STATE {KEY, VALUE, SEPARATOR} state = KEY;

	uint8_t read; //Number of bytes read in the body file.
	char buffer; //The char currently being read.
	uint8_t index = 0; //Index of the character being read.

	body->set_cursor(0); //Reset the body's file cursor.

	do //While there is still data in the body file.
	{
		read = body->read(&buffer, 1); //Read one byte.

		switch(state)
		{
			case KEY: //Parser is finding the key.
				if(buffer != key[index]) //If the keys do not match.
				{
					//This is not the argument we are looking for.
					state = SEPARATOR; //Find the next separator.
				}
				/**TODO: This should be else if, otherwise this could trigger
				a false positive.*/
				//If we have checked the entire key and have reached the value.
				if( key[index++] == '\0' && buffer == '=')
				{
					//The argument has been found!
					index = 0; //Reset the index.
					state = VALUE; //Next output the argument's value.
				}
				/**TODO: if buffer is = and we have not reached the end of the
				 * key, go look for the next key*/
				break;
			case SEPARATOR: //Parser is finding an argument separator.
				if(buffer == '&')
				{
					state = KEY;
					index = 0; //Reset the index
				}
				break;
			case VALUE: //Parser is outputting the value.
				//If we have reached another argument of the end of the buffer.
				if(buffer == '&' || read == 0)
				{
					value[index] = '\0'; // Terminate the string with a null character.
					return index; //Done, return the number of bytes read.
				}
				value[index++] = buffer;
				//If we have reached the maximum size allowed.
				if(index == max_size)
				{
					return index; //Quit.
				}
				break;
		}

	}
	while(read); //While there is still characters in the buffer.

	return 0; //The argument was not found so return 0.
}
#endif

bool Request::is_method(const char* m) const
{
	return !strcmp(method, m);
}
