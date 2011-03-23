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
#include <utils/file.h>


#ifndef MESSAGE_SIZE
	#define MESSAGE_SIZE uint32_t
#endif

class Message
{
	public:
		enum TYPE { REQUEST, RESPONSE, UNKNOWN };
		enum PARSER_RESULT { NOT_PARSED,
			PARSING_COMPLETE,
			PARSING_SUCESSFUL,
			LINE_INCOMPLETE,
			LINE_MALFORMED,
			LINE_OVERFLOW,
			HEADER_MALFORMED,
			OUT_OF_MEMORY };

		//General header fields
		//static const char * CACHE_CONTROL;
		//static const char * CONNECTION;
		//static const char * DATE;
		//static const char * PRAGMA;
		//static const char * TRAILER;
		//static const char * TRANSFER_ENCODING;
		//static const char * UPGRADE;
		//static const char * VIA;
		//static const char * WARNING;

		//Entity header fields
		//static const char * CONTENT_ENCODING;
		//static const char * CONTENT_LANGUAGE;
		static const char* CONTENT_LENGTH;
		//static const char * CONTENT_LOCATION;
		//static const char * CONTENT_MD5;
		//static const char * CONTENT_RANGE;
		static const char * CONTENT_TYPE;
		//static const char * EXPIRES;
		//static const char * LAST_MODIFIED;

		typedef const char* mime;

		static mime TEXT_HTML;
		static mime MESSAGE_HTTP;

		//Elements header fields
		static const char* FROM_URL;

		URL* to_url;
		URL* from_url;

		TYPE object_type;

		File* body_file;
		MESSAGE_SIZE content_length;

		char*  header;
		size_t header_length;

		char* current_line;
		size_t current_line_length;

		bool parsing_body;

	public:
		Message();
		virtual ~Message();


		virtual void print();

 		virtual MESSAGE_SIZE get_header_length(void);

		virtual void serialize( char* buffer );

		Message::PARSER_RESULT parse(const char* buffer);
		Message::PARSER_RESULT parse(const char* buffer, MESSAGE_SIZE size);
		virtual Message::PARSER_RESULT parse_header(const char* line, MESSAGE_SIZE size);
		Message::PARSER_RESULT store_body(const char* buffer, MESSAGE_SIZE size);


	protected:

};

#endif /* MESSAGE_H_ */
