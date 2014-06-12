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
 * Header file for the Request class.
 */

#ifndef REQUEST_H_
#define REQUEST_H_

//INCLUDES
#include <stdint.h>
#include "message.h"

class Request: public Message
{
	protected:
		static const Message::TYPE type = REQUEST; //TODO this is probably useless.
		///The HTTP method of this request.
		const char* method;

	public:
		//REQUEST HEADER FIELDS
		//static const char ACCEPT[];
		//static const char ACCEPT_CHARSET[];
		//static const char ACCEPT_ENCODING[];
		//static const char ACCEPT_LANGUAGE[];
		//static const char AUTHORIZATION[];
		//static const char EXPECT[];
		//static const char FROM[];
		//static const char HOST[];
		//static const char IF_MATCH[];
		//static const char IF_MODIFIED_SINCE[];
		//static const char IF_NONE_MATCH[];
		//static const char IF_UNMODIFIED_SINCE[];
		//static const char MAX_FORWARDS[];
		//static const char PROXY_AUTHORIZATION[];
		//static const char RANGE[];
		//static const char REFERER[];
		//static const char TE[];
		//static const char USER_AGENT[];


		///HTTP METHODS
		///HTTP GET request method.
		static const char GET[];
		///HTTP POST request method.
		static const char POST[];
		///HTTP DELETE request method.
		static const char DELETE[];

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
