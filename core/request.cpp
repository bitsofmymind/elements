//============================================================================
// Name        : request.cpp
// Author      : Antoine Mercier-Linteau
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include "message.h"
#include "request.h"
//#include "../elements.h"
#include "../utils/types.h"
#include "url.h"
#include <stdint.h>
#include <pal/pal.h>
#include <ctype.h>

using namespace Elements;

Request::Request():
		Message()
{
	to_url = new URL();
	from_url = new URL();
	object_type = REQUEST;

	method.text = NULL;
	method.length = 0;

}

Request::~Request()
{
	delete to_url;
	delete from_url;
}

#ifdef DEBUG
	void Request::print(void)
	{
		Debug::print(" % Request: ");
		Debug::print(method.text, method.length);
		Debug::print(" ");
		Debug::print(to_url->url.text, to_url->url.length);
		Debug::println(" HTTP/1.0");
		Message::print();
	}
#endif

MESSAGE_SIZE Request::get_header_length(void)
{
	return method.length /*For 'HTTP/'*/ \
		+ 1 /*For a space*/ \
		+ to_url->get_length() \
		+ 1 /*For a space*/ \
		+ 5 /*For 'HTTP/'*/
		+ 3 /*for the HTTP version*/ \
		+ 2 /*For CLRF between header and fields*/ \
		+ Message::get_header_length();

}
#ifndef NO_REQUEST_SERIALIZATION
	void Request::serialize(char* buffer)
	{

		buffer += method.copy(buffer);
		*buffer++ = ' ';

		buffer += to_url->serialize(buffer);
		*buffer++ = ' ';

		*buffer++ = 'H'; *buffer++ = 'T'; *buffer++ = 'T'; *buffer++ = 'P'; *buffer++ = '/';
		*buffer++ = '1';
		*buffer++ = '.';
		*buffer++ = '0';

		*buffer++ = '\r';
		*buffer++ = '\n';

		Message::serialize(buffer);
	}
#endif

Message::PARSER_RESULT Request::parse_header(const char* line, MESSAGE_SIZE size)
{

	if(line[size - 2] != '\r' && line[size - 1] != '\n')
	{
		return LINE_INCOMPLETE;
	}

	if(!header.text)
	{
		header.text = (char*)ts_malloc(size - 2); /*We substract two because the
		\r\n is implicit*/
		if(!header.text)
		{
			return Message::OUT_OF_MEMORY;
		}
		header.length = size - 2;
		memcpy(header.text, line , size - 2);

		char* index = header.text;


		while(true)
		{
			if( *index == ' ' )
			{
				method.text = header.text;
				method.length = index - header.text;
				break;
			}
			else if (index > (header.text + header.length))
			{
				return HEADER_MALFORMED;
			}
			if(*index >= 'A' && *index <= 'Z')
			{
				*index += 32;
			}
			index++;
		}
		Message::PARSER_RESULT res = PARSING_SUCESSFUL;

		to_url->deserialize( ++index );
		//Should checl is url parsing was sucessful
		from_url->is_absolute_path = to_url->is_absolute_path;

		/*We do not really care about the HTTP version here, in fact, we could avoir saving it entirely*/
		if(res != PARSING_SUCESSFUL)
		{
			return HEADER_MALFORMED;
		}
		return PARSING_SUCESSFUL;
	}

	//Here we would parse for headers we want to keep

	return Message::parse_header(line, size);
}

bool Request::methodcmp(const char * m, uint8_t len)
{
	return method.length == len && !memcmp(m, method.text, 3);
}
//const string< uint8_t > Request::ACCEPT = {"accept", 19};
//const string< uint8_t > Request::ACCEPT_CHARSET = {"accept-string< uint8_t >set", 20};
//const string< uint8_t > Request::ACCEPT_ENCODING = {"accept-encoding", 21};
//const string< uint8_t > Request::ACCEPT_LANGUAGE = {"accept-language", 22};
//const string< uint8_t > Request::AUTHORIZATION = {"authorization", 23};
//const string< uint8_t > Request::EXPECT = {"except", 24};
//const string< uint8_t > Request::FROM = {"from", 25};
//const string< uint8_t > Request::HOST = {"host", 26};
//const string< uint8_t > Request::IF_MATCH = {"if-match", 27};
//const string< uint8_t > Request::IF_MODIFIED_SINCE = {"if-modified-since", 28};
//const string< uint8_t > Request::IF_NONE_MATCH = {"if-none-match", 29};
//const string< uint8_t > Request::IF_UNMODIFIED_SINCE = {"if-unmodified-since", 30};
//const string< uint8_t > Request::MAX_FORWARDS = {"max-forwards", 31};
//const string< uint8_t > Request::PROXY_AUTHORIZATION = {"proxy-authorization", 32};
//const string< uint8_t > Request::RANGE = {"range", 33};
//const string< uint8_t > Request::REFERER = {"referer", 34};
//const string< uint8_t > Request::TE = {"te", 35};
//const string< uint8_t > Request::USER_AGENT = {"user-agent", 36};
