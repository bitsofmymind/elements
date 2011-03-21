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
#include "../utils/types.h"
#include <pal/pal.h>
#ifndef ITOA
#include <cstdio>
#endif

using namespace Elements;

#ifdef DEBUG
	void Message::print()
	{
		/*for( uint8_t i = 0; i<fields.items ; i++)
		{
			Debug::print("   ");
			Debug::print(fields[i]->key.text, fields[i]->key.length);
			Debug::print(": ");
			Debug::println(fields[i]->value->text, fields[i]->value->length);
		}*/

		Debug::print("   body: ");
		if(body_file)
		{
			Debug::println(body_file->size, DEC);
		}
		else
		{
			Debug::println(0);
		}
	}
#endif

Message::Message():
		content_length(0)
{
	body_file = NULL;
	header.length = 0;
	header.text = NULL;
	current_line.length = 0;
}
Message::~Message()
{
	/*while(fields.items)
	{
		ts_free(fields.remove(fields[0]->key));
	}*/
	if(header.length)
	{
		ts_free(header.text);
	}
	if(body_file)
	{
		delete body_file;
	}
	if(current_line.length)
	{
		ts_free(current_line.text);
	}
}


MESSAGE_SIZE Message::get_header_length(void)
{
	MESSAGE_SIZE size = 2; //For \r\n at end of the headers

	if(content_length)
	{
		/*itoa(content_length) will not produce more chars than the type that can
		contain it plus a null termination.*/
		#if MESSAGE_SIZE == uint16_t
			char content_string[5 + 1];
		#elif MESSAGE_SIZE == uint32_t
			char content_string[10 + 1];
		#elif MESSAGE_SIZE == uint64_t
			char content_string[20 + 1];
		#else
			#error "MESSAGE_SIZE is of unknown type"
		#endif

		#ifdef ITOA
			itoa(content_length, content_string, 10);
		#else
			sprintf(content_string, "%d", content_length);
		#endif
		size += CONTENT_LENGTH.length + 2/*For ": "*/+ strlen(content_string) + 2/*For \r\n*/;

	}
	//Add fields here.
	return size;
}

void Message::serialize( char* buffer )
{
	if(content_length)
	{
		CONTENT_LENGTH.copy(buffer);
		buffer += CONTENT_LENGTH.length; //Moves the pointer after "Content-Length"
		*buffer++ = ':';
		*buffer++ =' ';
		#ifdef ITOA
			itoa(content_length, buffer, 10);
		#else
			sprintf(buffer, "%d", content_length);
		#endif
		buffer += strlen(buffer); /*Moves the pointer after the length. The terminating
		character gets discarded because strlen() does not count it.*/

		*buffer++ = '\r';
		*buffer++ = '\n';
	}

	//Serialize other fields here

	*buffer++ = '\r';
	*buffer = '\n';
}

Message::PARSER_RESULT Message::parse(const char* data, MESSAGE_SIZE size)
{
	MESSAGE_SIZE line_end = 0;
	MESSAGE_SIZE line_start = 0;

	if(body_file)
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
			if(current_line.length)
			{
				//Part of a line was previously saved
				char* line = (char*)ts_malloc(line_end + 1 + current_line.length);
				//Stack allocation coud be used here.
				if(!line)
				{
					return OUT_OF_MEMORY;
				}
				memcpy(line, current_line.text, current_line.length);
				memcpy(line + current_line.length, data, line_end + 1);
				ts_free(current_line.text);
				res = parse_header(line, current_line.length + line_end + 1 );
				ts_free(line);
				current_line.length = 0;
			}
			else
			{
				res = parse_header(data + line_start, line_end - line_start + 1);
			}

			switch(res)
			{
				case PARSING_COMPLETE:
					//The remainder of the buffer is part of the the body so we store it.
					return store_body(data + line_end + 1, size - (line_end + 1));
					/*if(line_end + 1 < size)
					{
						return store_body(data + line_end + 1, size - line_end + 1);
					}
					else if(content_length)
					{
						return PARSING_SUCESSFUL;
					}
					return PARSING_COMPLETE;
					break;*/
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
				if(current_line.length) //The end of the line was not located within this buffer
				{
					char* line = (char*)ts_malloc(line_end + current_line.length);
					if(!line)
					{
						return OUT_OF_MEMORY;
					}
					memcpy( line, current_line.text, current_line.length);
					memcpy( line + current_line.length, data, size);
					ts_free(current_line.text);
					current_line.text = line;
					current_line.length += size;
				}
				else
				{
					current_line.length = line_end - line_start;
					current_line.text = (char*)ts_malloc(current_line.length);
					if(!current_line.text)
					{
						return OUT_OF_MEMORY;
					}
					memcpy(current_line.text, data + line_start, current_line.length);
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

	/*if(content_length)
	{
		//body present
		body_file = new ConstFile<MESSAGE_SIZE>(buffer);
	}*/

	return PARSING_COMPLETE;
}

Message::PARSER_RESULT Message::parse_header(const char* line, MESSAGE_SIZE size)
{
	//Overrides of this method should have checked if the line was correcly formed
	if(size == 2)
	{
		return PARSING_COMPLETE;
	}

	/*The content length line gets special treatment, for the rest of the fields, we should proceed normally
	 * and store them in a buffer.*/
	if(!strncmp(CONTENT_LENGTH.text, line, CONTENT_LENGTH.length)) //Could use memcmp
	{
		content_length = atoi(line + CONTENT_LENGTH.length + 2);
	}

	//Store fields in buffers

	return PARSING_SUCESSFUL;
}

Message::PARSER_RESULT Message::store_body(const char* buffer, MESSAGE_SIZE size)
{

	if(!content_length)
	{
		return PARSING_COMPLETE;
	}
	if(!current_line.length)
	{
		current_line.text = (char*)ts_malloc(content_length);
		if(!current_line.text)
		{
			return OUT_OF_MEMORY;
		}
	}

	if(size > content_length - current_line.length)
	{
		size = content_length - current_line.length;
	}

	memcpy(current_line.text + current_line.length, buffer, size);
	current_line.length += size;

	if(current_line.length >= content_length)
	{
		body_file = new MemFile<MESSAGE_SIZE>(current_line.text, current_line.length);
		current_line.length = 0;
		if(!body_file)
		{
			ts_free(current_line.text);
			return OUT_OF_MEMORY;
		}

		return PARSING_COMPLETE;
	}

	return PARSING_SUCESSFUL;
}


//const string< uint8_t > Message::CACHE_CONTROL = {"cache-control", 1};
//const string< uint8_t > Message::CONNECTION = {"connection", 2 };
//const string< uint8_t > Message::DATE = {"date", 3 };
//const string< uint8_t > Message::PRAGMA = {"pragma", 4 };
//const string< uint8_t > Message::TRAILER = {"trailer", 5 };
//const string< uint8_t > Message::TRANSFER_ENCODING = {"transfer-encoding", 6 };
//const string< uint8_t > Message::UPGRADE = {"upgrade", 7 };
//const string< uint8_t > Message::VIA = {"via", 8 };
//const string< uint8_t > Message::WARNING = {"warning", 9 };

//const string< uint8_t > Message::CONTENT_ENCODING = {"content-encoding", 10 };
//const string< uint8_t > Message::CONTENT_LANGUAGE = {"content-language", 11 };
const string< uint8_t > Message::CONTENT_LENGTH = MAKE_STRING("Content-Length");
//const string< uint8_t > Message::CONTENT_LOCATION = {"content-location", 13 };
//const string< uint8_t > Message::CONTENT_MD5 = {"content-md5", 14 };
//const string< uint8_t > Message::CONTENT_RANGE = {"content-range", 15 };
const string< uint8_t > Message::CONTENT_TYPE = MAKE_STRING("Content-Type");
//const string< uint8_t > Message::EXPIRES = {"expires", 17 };
//const string< uint8_t > Message::LAST_MODIFIED = {"last-modified", 18 };
//const string< uint8_t > Message::FROM_URL = {"from-url", 48 };

Message::mime Message::TEXT_HTML = MAKE_STRING("text/html");
Message::mime Message::MESSAGE_HTTP = MAKE_STRING("message/http");
