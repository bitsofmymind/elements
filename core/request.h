/*
 * request.h
 *
 *  Created on: Feb 16, 2009
 *      Author: Antoine
 */

#ifndef REQUEST_H_
#define REQUEST_H_

#include "message.h"
//#include "../elements.h"
#include "../utils/types.h"
#include <stdint.h>

class Request: public Message
{
	public:

		//Request header fields
		//static const string< uint8_t > ACCEPT;
		//static const string< uint8_t > ACCEPT_CHARSET;
		//static const string< uint8_t > ACCEPT_ENCODING;
		//static const string< uint8_t > ACCEPT_LANGUAGE;
		//static const string< uint8_t > AUTHORIZATION;
		//static const string< uint8_t > EXPECT;
		//static const string< uint8_t > FROM;
		//static const string< uint8_t > HOST;
		//static const string< uint8_t > IF_MATCH;
		//static const string< uint8_t > IF_MODIFIED_SINCE;
		//static const string< uint8_t > IF_NONE_MATCH;
		//static const string< uint8_t > IF_UNMODIFIED_SINCE;
		//static const string< uint8_t > MAX_FORWARDS;
		//static const string< uint8_t > PROXY_AUTHORIZATION;
		//static const string< uint8_t > RANGE;
		//static const string< uint8_t > REFERER;
		//static const string< uint8_t > TE;
		//static const string< uint8_t > USER_AGENT;

		static const Message::TYPE type = REQUEST;


		Elements::string< uint8_t > method;
		Elements::string< uint8_t > url;
		uint8_t http_version;

		Request();
		~Request();

		#ifdef DEBUG
			virtual void print();
		#endif


		virtual char deserialize( Elements::string<MESSAGE_SIZE>& buffer, char* index );
		virtual char serialize( char* buffer );
		virtual MESSAGE_SIZE get_message_length(void);
};

#endif /* REQUEST_H_ */
