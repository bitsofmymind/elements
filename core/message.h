/* message.h - Implements an abstract HTTP message
 * Copyright (C) 2011 Antoine Mercier-Linteau
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef MESSAGE_H_
#define MESSAGE_H_

#include <configuration.h>
#include "url.h"
#include <utils/utils.h>
#include <utils/file.h>
#include "mime.h"


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
		//static const char CACHE_CONTROL[];
		//static const char CONNECTION[];
		//static const char DATE[];
		//static const char PRAGMA[];
		//static const char TRAILER[];
		//static const char TRANSFER_ENCODING[];
		//static const char UPGRADE[];
		//static const char VIA[];
		//static const char WARNING[];

		//Entity header fields
		//static const char CONTENT_ENCODING[];
		//static const char CONTENT_LANGUAGE[];
		static const char CONTENT_LENGTH[];
		//static const char CONTENT_LOCATION[];
		//static const char CONTENT_MD5[];
		//static const char CONTENT_RANGE[];
		static const char CONTENT_TYPE[];
		//static const char EXPIRES[];
		//static const char LAST_MODIFIED[];

		URL* to_url;
		URL* from_url;

		TYPE object_type;

		const char* content_type;

#if MESSAGE_AGE
		uptime_t age;

protected:
		char*  header;
		size_t header_length;

		bool parsing_body;

		char* current_line;
		size_t current_line_length;

		File* body;
#endif

	public:
		Message();
		virtual ~Message();

		virtual void print();
		virtual size_t serialize( char* buffer, bool write);
		Message::PARSER_RESULT parse(const char* buffer);
		Message::PARSER_RESULT parse(const char* buffer, size_t size);

		void set_body(File* f, const char* mime);
		File* get_body(void) const;
		File* unset_body(void);

	protected:
		virtual Message::PARSER_RESULT parse_header(const char* line, size_t size);
		Message::PARSER_RESULT store_body(const char* buffer, size_t size);

};

#endif /* MESSAGE_H_ */
