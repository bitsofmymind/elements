/*
 * request.h
 *
 *  Created on: Feb 16, 2009
 *      Author: Antoine
 */

#ifndef REQUEST_H_
#define REQUEST_H_

#include "message.h"
//#include <elements.h>
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


		Request();
		~Request();


		virtual void print();



		#ifndef NO_REQUEST_SERIALIZATION
			virtual void serialize( char* buffer );
		#endif

		virtual MESSAGE_SIZE get_header_length(void);
		virtual Message::PARSER_RESULT parse_header(const char* line, MESSAGE_SIZE size);

		bool methodcmp(const char* m, uint8_t len);

		#ifdef BODY_ARGS_PARSING
			uint8_t find_arg(const char* key, char* value, uint8_t max_size);
		#endif
};

#endif /* REQUEST_H_ */
