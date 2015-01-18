// SVN FILE: $Id: $
/**
 * @lastChangedBy           $lastChangedBy: Mercier $
 * @revision                $Revision: 397 $
 * @copyright    			GNU General Public License
 * 		This program is free software: you can redistribute it and/or modify
 * 		it under the terms of the GNU General Public License as published by
 * 		the Free Software Foundation, either version 3 of the License, or
 * 		(at your option) any later version.
 * 		This program is distributed in the hope that it will be useful,
 * 		but WITHOUT ANY WARRANTY; without even the implied warranty of
 *		MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * 		GNU General Public License for more details.
 * 		You should have received a copy of the GNU General Public License
 * 		along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * Source file for the Message class.
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
		parsing_body(false),
		content_type(NULL),
		to_url_cursor(0),
		from_url_cursor(0),
		dispatching(UNDETERMINED)
{
	body = NULL;
	header_length = 0;
	header = NULL;
	current_line_length = 0;
	current_line = NULL;
#if MESSAGE_AGE
	age = get_uptime(); //Set the uptime at which the message was created.
#endif
}

Message::~Message()
{
	/*while(fields.items)
	{
		ts_free(fields.remove(fields[0]->key));
	}*/
	//TODO free should not free null pointers so condition can be removed.
	if(header) //If memory for a header was allocated.
	{
		ts_free(header);
	}
	if(body) //If a File object for a body was created.
	{
		delete body;
	}
	if(current_line_length) //If memory for parsing was allocated.
	{
		ts_free(current_line);
	}
}

void Message::print()
{
	//Content-Length printing.
	DEBUG_PRINT(Message::CONTENT_LENGTH);
	DEBUG_PRINT(": ");
	if(body) //If a message body is set.
	{
		DEBUG_TPRINTLN((uint32_t)body->size, DEC); //Print its size.
	}
	else
	{
		DEBUG_PRINTLN('0'); //Else Content-Length is 0
	}
}

size_t Message::serialize(char* buffer, bool write)
{
	//Note: The message header should have been serialized by a child class.

	char* start = buffer; //Keep a pointer to the start of the buffer.

	//CONTENT-LENGTH FIELD SERIALIZING
	if( write ){ strcpy( buffer, CONTENT_LENGTH ); }
	buffer += 14; //Equivalent to strlen(CONTENT_LENGTH);
	if( write )
	{
		*buffer = ':';
		*(buffer + 1) = ' ';
	}
	buffer += 2;

	size_t cl; //The size of the body.
	if(body){ cl = body->size; } //If there is a body, get its length.
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

	//CONTENT-TYPE FIELD SERIALIZING
	if(content_type) //If the message has a Content-type.
	{
		if( write ) { strcpy(buffer, CONTENT_TYPE); }
		buffer += 12; //Equivalent to strlen(CONTENT_TYPE);
		if( write )
		{
			*buffer = ':';
			*( buffer + 1 ) = ' ';
		}
		buffer += 2;
		if( write ) { strcpy(buffer, content_type); }
		buffer += strlen(content_type);
		if( write )
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
	size_t line_start = 0; //Where the current line starts in the buffer.
	///TODO rename line_end to ptr.
	size_t line_end = 0; //What we are currently parsing.

	if(parsing_body) //If we are done parsing the header.
	{
		return store_body(data, size); //Forward the buffer to store_body().
	}

	while(true)
	{
		/*To account for cases where a buffer ends at \r, we only look for
		 * the \n to detect a new line. If the line is misformed and \r was
		 * omitted, it will be detected by the line parser.*/

		if(data[line_end] == '\n') //If we are at the end of the current line.
		{
			PARSER_RESULT res; //The result of the parsing.
			//If part of a line was received in a previous call to this method.
			if(current_line_length)
			{
				//Part of a line was previously saved, allocated bigger buffer.
				char* line = (char*)ts_malloc(line_end + 1 + current_line_length);
				/**TODO use stack allocation here because we only need line
				 * within this scope.*/
				if(!line) //If allocating the new line buffer failed.
				{
					return OUT_OF_MEMORY; //Return the appropriate error.
				}
				//Copy the content of the old line buffer to the new.
				memcpy(line, current_line, current_line_length);
				//Copy the content of the received buffer to the line buffer.
				memcpy(line + current_line_length, data, line_end + 1);
				ts_free(current_line); //Free the old line buffer.
				//A complete line is in the buffer so parse it.
				res = parse_header(line, current_line_length + line_end + 1 );
				ts_free(line); //We no longer need line so free it.
				current_line_length = 0; //Reset the line_buffer length.
			}
			else //A complete is contained within the current buffer.
			{
				//Parse it.
				res = parse_header(data + line_start, line_end - line_start + 1);
			}

			switch(res) //Switch according to the result of the parsing.
			{
				case PARSING_COMPLETE:
					/*The remainder of the buffer is part of the the body so
					 * we store it next.
					 */
					parsing_body = true;
					return store_body(data + line_end + 1, size - (line_end + 1));
				case PARSING_SUCESSFUL: //A line has been parsed successfully.
					line_start = ++line_end; //Start a new line.
					break;
				default:
					return res; //Something unexpected has occurred.
			}
		}
		if(line_end == size) //If we are at the end of the buffer.
		{
			//If the current line ended with the buffer.
			if(line_start == line_end)
			{
				//No need to save anything

			}
			else
			{
				/*The current line ends in a subsequent buffer, we need to save
				 * the current line for the next call to this method.*/

				//If the line buffer already contains data.
				if(current_line_length)
				{
					/*Allocate a bigger buffer to contain both this buffer and
					the line buffer. */
					char* line = (char*)ts_malloc(line_end + current_line_length);
					if(!line) //If allocating the new line buffer failed.
					{
						return OUT_OF_MEMORY; //Return the appropriate error.
					}
					//Copy the content of the old line buffer to the new.
					memcpy( line, current_line, current_line_length);
					//Copy the content of the received buffer to the line buffer.
					memcpy( line + current_line_length, data, size);
					ts_free(current_line); //Frees the old line buffer.
					current_line = line; //Save the new line buffer.
					current_line_length += size; //Increase its size.
				}
				else //No data is within the line buffer.
				{
					//Save the current line's length.
					current_line_length = line_end - line_start;
					//Allocate a buffer to save it.
					current_line = (char*)ts_malloc(current_line_length);
					//If allocating the new line buffer failed.
					if(!current_line)
					{
						return OUT_OF_MEMORY; //Return the appropriate error.
					}
					//Copy the content of the received buffer to the line buffer.
					memcpy(current_line, data + line_start, current_line_length);
				}
			}
			return PARSING_SUCESSFUL; //Parsing is doing OK.
		}
		line_end++; //Parse the next character.
	}
}

Message::PARSER_RESULT Message::parse(const char* buffer)
{
	/*The size of the line being parsed. Initialized to 1 because buffer
	 * is incremented at the beginning of the loop.*/
	uint8_t line_size = 1;
	Message::PARSER_RESULT res; //The result of the parsing.

	/*While we have not reached a null character in the buffer. buffer is
	incremented at the beginning of the loop so we are never checking memory
	that may be outside the buffer.*/
	while(*++buffer != '\0')
	{
		//If an HTTP end of line (CRLF) is detected.
		if(*(buffer - 1) == '\r' && *buffer == '\n')
		{
			/*Parse that line. One is added to line_size because we want it to
			be a length and not an index.*/
			res = parse_header(buffer - line_size, line_size + 1);
			if(res == PARSING_COMPLETE) //If parsing is done.
			{
				break; //Exit the loop.
			}
			if(res == PARSING_SUCESSFUL) //If parsing the line was sucessful.
			{
				line_size = 0; //Reset line_size (should it not be set to 1?)
				continue; //Continue with the rest of the buffer.
			}
			else //Else something unexpected has occurred.
			{
				return res; //Return the parsing result.
			}
		}
		line_size++; //Increase the line_size.
	}

	return PARSING_COMPLETE; //Done parsing.
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

uint8_t Message::to_destination(void)
{
	/*If dispatching is not relative, there is no real way to tell where
	 * the message currently is relative to its destination.*/
	if(dispatching != RELATIVE)
	{
		///TODO make this a configurable value.
		return 255; //Return the maximum depth a message may be at.
	}

	/*Else return the depth. It is computed using the cursor. 1 is substracted
	 * from the total because a message always has at least one resource
	 * in its url.*/
	return to_url->resources.items - to_url_cursor - 1;
}

void Message::next(void)
{
	//Check if there is a next resource.
	if(to_url_cursor < to_url->resources.items - 1)
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

Message::PARSER_RESULT Message::parse_header(const char* line, size_t size)
{
	/* Note: Overrides of this method should have checked if the line was
	 * correctly formed. */

	/*Since the body of an HTTP message is separated from the header using an
	empty line, we can detect it by only checking if size is 2 (CR and LF)*/
	if(size == 2)
	{
		return PARSING_COMPLETE; //Parsing is done.
	}

	/*The content length line gets special treatment because it gives the
	 * size of the body, for the rest of the fields, we should proceed normally
	 * and store them in their structure.*/
	//If the header line is "content-length".
	if(!strncmp(CONTENT_LENGTH, line, 14)) /// TODO Could use memcmp.
	{
		/*Get the content length by converting from its textual representation.
		 * 14 is strlen(CONTENT_LENGTH) and 2 is ": ". */
		size_t content_length = atoi(line + 14 + 2);
		/*Create a body object the size of content_length. Set its memory
		 * pointer to NULL because we will be giving a memory area later
		 * when the body is actually received.*/
		body = new MemFile(NULL, content_length, false);
		if(!body) //If creating the body failed.
		{
			ts_free(current_line); //Free the line buffer.
			return OUT_OF_MEMORY; //Return the appropriate error.
		}
	}

	//Store fields in buffers

	return PARSING_SUCESSFUL; //Parsing that line was sucessful.
}

Message::PARSER_RESULT Message::store_body(const char* buffer, size_t size)
{
	if(!body) //If no body object is set.
	{
		return PARSING_COMPLETE; //Parsing is done.
	}

	//The size of the body, it was set in parse_header().
	size_t cl = body->size;

	//Note: this method reuses the line buffer to store body chunks.

	/*TODO this method should use the line buffer to store body chunks,
	 * is should do it used the file already allocated for the body.*/

	if(!current_line_length) //If no buffer was allocated for the body already.
	{
		current_line = (char*)ts_malloc(cl); //Allocate one.
		if(!current_line) //If allocation failed.
		{
			return OUT_OF_MEMORY; //Return the appropriate error.
		}
	}

	/*If for some reason, size got larger than the size of the remaining body
	data.*/
	if(size > cl - current_line_length)
	{
		size = cl - current_line_length; //Set it back to avoid overflow.
	}

	//Copy the content of the buffer to the line buffer.
	memcpy(current_line + current_line_length, buffer, size);
	current_line_length += size; //Increase the size of the line_buffer.

	//If we have received all the body.
	if(current_line_length >= cl)
	{
		//Set the line buffer to be the memfile's data.
		((MemFile*)body)->data = current_line;

		/*Reset the line buffer length to 0, what used to be the line buffer is
		now the body and under the responsibility of a File object.*/
		current_line_length = 0;

		return PARSING_COMPLETE; //Parsing is done.
	}

	 //We are still expecting parts of the body but everything is good so far.
	return PARSING_SUCESSFUL;
}

//MESSAGE FIELD DEFINITIONS

//const char* Message::CACHE_CONTROL = {"cache-control", 1};
//const char* Message::CONNECTION = {"connection", 2 };
//const char* Message::DATE = {"date", 3 };
//const char* Message::PRAGMA = {"pragma", 4 };
//const char* Message::TRAILER = {"trailer", 5 };
//const char* Message::TRANSFER_ENCODING = {"transfer-encoding", 6 };
//const char* Message::UPGRADE = {"upgrade", 7 };
//const char* Message::VIA = {"via", 8 };
//const char* Message::WARNING = {"warning", 9 };

//const char* Message::CONTENT_ENCODING = {"content-encoding", 10 };
//const char* Message::CONTENT_LANGUAGE = {"content-language", 11 };
/** The Content-Length header field definition.*/
const char Message::CONTENT_LENGTH[] = "Content-Length";
//const char* Message::CONTENT_LOCATION = {"content-location", 13 };
//const char* Message::CONTENT_MD5 = {"content-md5", 14 };
//const char* Message::CONTENT_RANGE = {"content-range", 15 };
/** The Content-Type header field definition.*/
const char Message::CONTENT_TYPE[] = "Content-Type";
//const char* Message::EXPIRES = {"expires", 17 };
//const char* Message::LAST_MODIFIED = "Last-Modified";
//const char* Message::FROM_URL = {"from-url", 48 };
