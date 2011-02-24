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
	url.text = NULL;
	url.length = 0;
	http_version = 0;
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
		Debug::print(url.text, url.length);
		Debug::print(" HTTP/");
		Debug::println(http_version, DEC);
		Message::print();
	}
#endif

char Request::deserialize(string<MESSAGE_SIZE>& buffer, char* index)
{
	char* start = index;

	while(true)
	{
		if( *index == ' ' )
		{
			method.text = start;
			method.length = index - start;
			break;
		}
		else if (index > (buffer.text + buffer.length))
		{
			return 1;
		}
		if(*index >= 'A' && *index <= 'Z')
		{
			*index += 32;
		}
		index++;

	}

	url.text = ++index;

	to_url->deserialize( index );
	url.length = to_url->url.length;
	from_url->is_absolute_path = to_url->is_absolute_path;

	index += to_url->url.length + 1; //Jumps to HTTP/1.x part

	if(*index == 'H')
	{
		index += 5; //Jumps the 'HTTP/'
		http_version = (*index++ - 48) * 10;
		index++; //Skips the '.'
		http_version += *index++ - 48;
	}
	else
	{
		return 1;
	}

	if(*index++ != '\r' || *index++ != '\n')
	{
		return 1;
	}

	return Message::deserialize( buffer, index );
}

MESSAGE_SIZE Request::get_message_length(void)
{
	//The 3 and 5 are for 'HTTP/' and the version
	return method.length /*For 'HTTP/'*/ \
		+ 1 /*For a space*/ \
		+ to_url->get_length() \
		+ 1 /*For a space*/ \
		+ 5 /*For 'HTTP/'*/
		+ 3 /*for the HTTP version*/ \
		+ 2 /*For CLRF between header and fields*/ \
		+ Message::get_message_length();
}
#ifndef NO_REQUEST_SERIALIZATION
	char Request::serialize(char* buffer)
	{

		buffer += method.copy(buffer);
		*buffer++ = ' ';

		buffer += to_url->serialize(buffer);
		*buffer++ = ' ';

		*buffer++ = 'H'; *buffer++ = 'T'; *buffer++ = 'T'; *buffer++ = 'P'; *buffer++ = '/';
		*buffer++ = (http_version / 10) +  48;
		*buffer++ = '.';
		*buffer++ = ( http_version - (http_version / 10 ) *10 ) + 48;

		*buffer++ = '\r';
		*buffer++ = '\n';

		return Message::serialize(buffer);
	}
#endif

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
