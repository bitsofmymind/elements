// SVN FILE: $Id: $
/**
 * @lastChangedBy           $lastChangedBy: Mercier $
 * @revision                $Revision: 397 $
 * @copyright    			GNU General Public License
 * 		This program is free software: you can redistribute it and/or modify
 * 		it under the terms of the GNU General Public License as published by
 * 		the Free Software Foundation, either version 3 of the License, or
 * 		(at your option) any later version.
 * 		This program is distributed in the hope that it will be useful,
 * 		but WITHOUT ANY WARRANTY; without even the implied warranty of
 *		MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * 		GNU General Public License for more details.
 * 		You should have received a copy of the GNU General Public License
 * 		along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * Header file for the Message class.
 */

#ifndef MESSAGE_H_
#define MESSAGE_H_

//INCLUDES

#include <configuration.h>
#include <utils/utils.h>
#include <utils/file.h>
#include "mime.h"
#include "url.h"

///@todo delete that if it is useless.
#ifndef MESSAGE_SIZE
	#define MESSAGE_SIZE uint32_t
#endif

class Message
{
	public:
		/** The possible results of parsing a message.*/
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

		/** The url this message is going to.*/
		URL* to_url;
		/** The url this message is coming from. This url is built as the
		 * message is travelling in the resource tree. It is used to know send
		 * a response to this message by setting it as to to_url when the
		 * message has reached destination.*/
		URL* from_url;

		/** The type this message is of. Implements a crude form of
		 * reflection.*/
		enum TYPE { REQUEST, RESPONSE, UNKNOWN } object_type;

		/** The Content-type of the message's body. Defined as an attribute
		 * field because is it very often needed.*/
		const char* content_type;
		/**The type of dispatching this message uses. Absolute messages
		 * always transit trough the root resource while relative messages
		 * are routed directly to their destination.*/
		enum dispatching { ABSOLUTE, RELATIVE, UNDETERMINED } dispatching;
#if MESSAGE_AGE
		/**The uptime at which the message was created.*/
		uptime_t age;
#endif

	protected:
		/** Points to the first line of the message header (the line specific
		 * to requests or responses).*/
		char*  header;
		/** The length of the message header first line (the line specific
		 * to requests or responses).*/
		size_t header_length;
		/** Pointer to the body of the message.*/
		File* body;
		/** The cursor for the destination url. The cursor is used to point
		 * to the current resource we are at within the destination url.*/
		uint8_t to_url_cursor;
		/** The cursor for the origin url. The cursor is used to point
		 * to the current resource we are at within the origin url.*/
		uint8_t from_url_cursor;

		///TODO the following should probably be private.

		//Parsing controls, used to resume parsing when new data arrives.
		/**If we are parsing the body of the message.*/
		bool parsing_body;
		/** Pointer to the current line we are parsing.*/
		char* current_line; ///TODO rename current_line to line_buffer.
		/** The length of the line we are currently parsing.*/
		size_t current_line_length; ///TODO rename current_line_buffer to line_buffer_length


	public:
		Message();
		virtual ~Message();

		virtual void print();
		virtual size_t serialize( char* buffer, bool write);
		Message::PARSER_RESULT parse(const char* buffer);
		Message::PARSER_RESULT parse(const char* buffer, size_t size);
		void set_body(File* f, const char* mime);
		inline File* get_body(void) const { return body; }
		File* unset_body(void);
		uint8_t to_destination(void);
		void next(void);
		void previous(void);
		///Returns the resource name pointed by the destination url cursor.
		/** The resource name currently pointed to by the destination url
		 *  cursor is the resource name the framework is routing the message
		 *  to next.*/
		inline const char* current(void) { return to_url->resources[to_url_cursor];	}

	protected:
		virtual Message::PARSER_RESULT parse_header(const char* line, size_t size);
		Message::PARSER_RESULT store_body(const char* buffer, size_t size);
};

#endif /* MESSAGE_H_ */
