//============================================================================
// Name        : message.cpp
// Author      : Antoine Mercier-Linteau
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <stdlib.h>
#include "message.h"
#include "../elements.h"
#include "../utils/utils.h"
#include <pal/pal.h>
#include <string.h>
#if !ITOA
#include <cstdio>
#endif


void Message::print()
{
	/*If VERBOSITY, OUTPUT_WARNINGS or OUTPUT_ERRORS is undefined,
	 * this method should be optimizes away by the compiler.*/

	if(content_length)
	{
		DEBUG_PRINT("Content-Length: ");
		DEBUG_PRINTLN_DEC(content_length);
	}
}


Message::Message():
		content_length(0),
		parsing_body(false)
{
	body_file = NULL;
	header_length = 0;
	header = NULL;
	current_line_length = 0;
	current_line = NULL;
}
Message::~Message()
{
	/*while(fields.items)
	{
		ts_free(fields.remove(fields[0]->key));
	}*/
	if(header)
	{
		ts_free(header);
	}
	if(body_file)
	{
		delete body_file;
	}
	if(current_line_length)
	{
		ts_free(current_line);
	}
}


size_t Message::serialize( char* buffer, bool write )
{
	char* start = buffer;

	if(content_length)
	{
		if( write ){ strcpy( buffer, CONTENT_LENGTH ); }
		buffer += 14; //strlen(CONTENT_LENGTH);
		if( write )
		{
			*buffer = ':';
			*(buffer + 1) = ' ';
		}
		buffer += 2;

		if( write )
		{
#if ITOA
			itoa(content_length, buffer, 10);
#else
			sprintf(buffer, "%d", content_length);
#endif
		}

		size_t cl = content_length;
		do
		{
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

	//Serialize other fields here

	if( write )
	{
		*buffer = '\r';
		*(buffer + 1) = '\n';
	}
	buffer+=2;

	return buffer - start;
}

Message::PARSER_RESULT Message::parse(const char* data, size_t size)
{
	size_t line_end = 0;
	size_t line_start = 0;

	if(parsing_body)
	{
		return store_body(data, size);
	}

	while(true)
	{
		/*To account for cases where a buffer ends at \r, we only consider the \n to
		 * detect a new line. If the line is misformed and \r was omitted, it will be detected by
		 * the line parser.*/
		if(data[line_end] == '\n')
		{
			PARSER_RESULT res;
			if(current_line_length)
			{
				//Part of a line was previously saved
				char* line = (char*)ts_malloc(line_end + 1 + current_line_length);
				//Stack allocation coud be used here.
				if(!line)
				{
					return OUT_OF_MEMORY;
				}
				memcpy(line, current_line, current_line_length);
				memcpy(line + current_line_length, data, line_end + 1);
				ts_free(current_line);
				res = parse_header(line, current_line_length + line_end + 1 );
				ts_free(line);
				current_line_length = 0;
			}
			else
			{
				res = parse_header(data + line_start, line_end - line_start + 1);
			}

			switch(res)
			{
				case PARSING_COMPLETE:
					//The remainder of the buffer is part of the the body so we store it.
					parsing_body = true;
					return store_body(data + line_end + 1, size - (line_end + 1));
				case PARSING_SUCESSFUL:
					line_start = ++line_end;
					break;
				default:
					return res;
			}
		}
		if(line_end == size)
		{
			if(line_start == line_end)
			{
				//The current line ended on the buffer's bounds, no need to save anything

			}
			else
			{
				/*The current line ends in the next buffer, we need to save the current line for the next call
				 * to this method.*/
				if(current_line_length) //The end of the line was not located within this buffer
				{
					char* line = (char*)ts_malloc(line_end + current_line_length);
					if(!line)
					{
						return OUT_OF_MEMORY;
					}
					memcpy( line, current_line, current_line_length);
					memcpy( line + current_line_length, data, size);
					ts_free(current_line);
					current_line = line;
					current_line_length += size;
				}
				else
				{
					current_line_length = line_end - line_start;
					current_line = (char*)ts_malloc(current_line_length);
					if(!current_line)
					{
						return OUT_OF_MEMORY;
					}
					memcpy(current_line, data + line_start, current_line_length);
				}
			}
			return PARSING_SUCESSFUL;
		}
		line_end++;
	}
}

Message::PARSER_RESULT Message::parse( const char* buffer )
{

	uint8_t line_size = 1;
	Message::PARSER_RESULT res;

	while(*++buffer != '\0')
	{

		if(*(buffer - 1) == '\r' && *buffer == '\n')
		{
			res = parse_header(buffer - line_size, line_size + 1);
			if(res == PARSING_COMPLETE)
			{
				break;
			}
			if(res == PARSING_SUCESSFUL)
			{
				line_size = 0;
				continue;
			}
			else
			{
				return res;
			}
		}
		line_size++;
	}

	return PARSING_COMPLETE;
}

Message::PARSER_RESULT Message::parse_header(const char* line, size_t size)
{
	//Overrides of this method should have checked if the line was correcly formed
	if(size == 2)
	{
		return PARSING_COMPLETE;
	}

	/*The content length line gets special treatment, for the rest of the fields, we should proceed normally
	 * and store them in a buffer.*/
	if(!strncmp(CONTENT_LENGTH, line, 14)) //Could use memcmp
	{
		content_length = atoi(line + 14 +/*strlen(CONTENT_LENGTH)*/ + 2);
	}

	//Store fields in buffers

	return PARSING_SUCESSFUL;
}

Message::PARSER_RESULT Message::store_body(const char* buffer, size_t size)
{

	if(!content_length)
	{
		return PARSING_COMPLETE;
	}
	if(!current_line_length)
	{
		current_line = (char*)ts_malloc(content_length);
		if(!current_line)
		{
			return OUT_OF_MEMORY;
		}
	}

	if(size > content_length - current_line_length)
	{
		size = content_length - current_line_length;
	}

	memcpy(current_line + current_line_length, buffer, size);
	current_line_length += size;

	if(current_line_length >= content_length)
	{
		body_file = new MemFile(current_line, current_line_length);
		current_line_length = 0;
		if(!body_file)
		{
			ts_free(current_line);
			return OUT_OF_MEMORY;
		}

		return PARSING_COMPLETE;
	}

	return PARSING_SUCESSFUL;
}


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
const char *Message::CONTENT_LENGTH = "Content-Length";
//const char* Message::CONTENT_LOCATION = {"content-location", 13 };
//const char* Message::CONTENT_MD5 = {"content-md5", 14 };
//const char* Message::CONTENT_RANGE = {"content-range", 15 };
const char *Message::CONTENT_TYPE = "Content-Type";
//const char* Message::EXPIRES = {"expires", 17 };
//const char* Message::LAST_MODIFIED = "Last-Modified";
//const char* Message::FROM_URL = {"from-url", 48 };
