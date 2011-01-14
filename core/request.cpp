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
#ifdef DEBUG
	#include <iostream>
#endif

using namespace Elements;

Request::Request():
		Message()
{
	to_url = new URL();
	from_url = new URL();
	object_type = REQUEST;
}

Request::~Request()
{
	delete to_url;
	delete from_url;
}

#ifdef DEBUG
	void Request::print(void)
	{
		using namespace std;

		cout << "%%%%%%%%%%%%%%%%%%%%%% Request %%%%%%%%%%%%%%%%%%%%%%" << endl;
		cout << "---------- Request Line ----------" << endl;
		cout << "Method: ";
		for(uint8_t i = 0 ; i < method.length; i++) { cout << method.text[i]; }
		cout << endl;
		cout << "Request URI: ";
		for( uint8_t i = 0 ; i < url.length; i++ )
		{
			cout << this->url.text[i];
		}
		cout << endl;

		cout << "HTTP version: " << (int)(http_version) << endl;
		cout << '\n' << endl;
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
		else if (index > buffer.text + buffer.length)
		{
			return 1;
		}
		index++;
	}

	url.text = ++index;

	to_url->deserialize( index );
	url.length = to_url->url.length;
	from_url->is_absolute_path = to_url->is_absolute_path;

	index += to_url->url.length + 1 + 5; //Jumps the URL, the space and 'HTTP/'
	http_version = (*index++ - 48) * 10;
	index++; //Skips the '.'
	http_version += *index++ - 48;

	index += 2; //skips \r\n (CRLF)

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
