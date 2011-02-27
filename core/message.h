/*
 * message.h
 *
 *  Created on: Feb 16, 2009
 *      Author: Antoine
 */

#ifndef MESSAGE_H_
#define MESSAGE_H_

#include <elements.h>
#include "url.h"
#include <utils/utils.h>
#include <utils/types.h>
#include <utils/file.h>

using namespace Elements;

#ifndef MESSAGE_SIZE
	#define MESSAGE_SIZE uint32_t
#endif

class Message
{
	public:
		enum TYPE { REQUEST, RESPONSE, UNKNOWN };
		enum PARSER_RESULT { PARSING_COMPLETE,
			PARSING_SUCESSFUL,
			LINE_INCOMPLETE,
			LINE_MALFORMED,
			LINE_OVERFLOW,
			HEADER_MALFORMED,
			OUT_OF_MEMORY };

		//General header fields
		//static const string< uint8_t > CACHE_CONTROL;
		//static const string< uint8_t > CONNECTION;
		//static const string< uint8_t > DATE;
		//static const string< uint8_t > PRAGMA;
		//static const string< uint8_t > TRAILER;
		//static const string< uint8_t > TRANSFER_ENCODING;
		//static const string< uint8_t > UPGRADE;
		//static const string< uint8_t > VIA;
		//static const string< uint8_t > WARNING;

		//Entity header fields
		//static const string< uint8_t > CONTENT_ENCODING;
		//static const string< uint8_t > CONTENT_LANGUAGE;
		static const string< uint8_t > CONTENT_LENGTH;
		//static const string< uint8_t > CONTENT_LOCATION;
		//static const string< uint8_t > CONTENT_MD5;
		//static const string< uint8_t > CONTENT_RANGE;
		static const Elements::string< uint8_t > CONTENT_TYPE;
		//static const string< uint8_t > EXPIRES;
		//static const string< uint8_t > LAST_MODIFIED;

		typedef const string<uint8_t> mime;

		static mime TEXT_HTML;
		static mime MESSAGE_HTTP;

		//Elements header fields
		static const Elements::string< uint8_t > FROM_URL;

		URL* to_url;
		URL* from_url;

		TYPE object_type;

		File<MESSAGE_SIZE>* body_file;
		MESSAGE_SIZE content_length;
		string<uint8_t> header;
		string<MESSAGE_SIZE> current_line;

	public:
		Message();
		~Message();

		#ifdef DEBUG
			virtual void print();
		#endif

 		virtual MESSAGE_SIZE get_header_length(void);

		virtual void serialize( char* buffer );

		Message::PARSER_RESULT parse(const char* buffer);
		Message::PARSER_RESULT parse(const char* buffer, MESSAGE_SIZE size);
		virtual Message::PARSER_RESULT parse_header(const char* line, MESSAGE_SIZE size);
		//int8_t parse_body(const char* buffer);

	protected:

};

#endif /* MESSAGE_H_ */
