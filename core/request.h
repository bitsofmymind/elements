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
#include <stdint.h>

class Request: public Message
{
	protected:
		static const Message::TYPE type = REQUEST;
		const char* method;
	public:

		//Request header fields
		//static const char* ACCEPT;
		//static const char* ACCEPT_CHARSET;
		//static const char* ACCEPT_ENCODING;
		//static const char* ACCEPT_LANGUAGE;
		//static const char* AUTHORIZATION;
		//static const char* EXPECT;
		//static const char* FROM;
		//static const char* HOST;
		//static const char* IF_MATCH;
		//static const char* IF_MODIFIED_SINCE;
		//static const char* IF_NONE_MATCH;
		//static const char* IF_UNMODIFIED_SINCE;
		//static const char* MAX_FORWARDS;
		//static const char* PROXY_AUTHORIZATION;
		//static const char* RANGE;
		//static const char* REFERER;
		//static const char* TE;
		//static const char* USER_AGENT;


		static const char* GET;
		static const char* POST;
		static const char* DELETE;

	public:
		Request();
		~Request();

		virtual void print();
#if REQUEST_SERIALIZATION
		virtual size_t serialize( char* buffer, bool write );
#endif
#if BODY_ARGS_PARSING
		uint8_t find_arg(const char* key, char* value, uint8_t max_size);
#endif
		bool is_method(const char* m);

	protected:
		virtual Message::PARSER_RESULT parse_header(const char* line, size_t size);
};

#endif /* REQUEST_H_ */
