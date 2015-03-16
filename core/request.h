/* request.h - Header file for the Request class.
 * Copyright (C) 2015 Antoine Mercier-Linteau
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

#ifndef REQUEST_H_
#define REQUEST_H_

//INCLUDES
#include <stdint.h>
#include "message.h"

///Request implements an HTTP request.
class Request: public Message
{
	protected:

		///The HTTP method of this request.
		char* method;

	public:

		///HTTP METHODS
		///HTTP GET request method.
		static const char GET[];
		///HTTP POST request method.
		static const char POST[];
		///HTTP DELETE request method.
		static const char DELETE[];

		/// Class constructor.
		Request();

		/// Class destructor.
		virtual ~Request();

		/// Prints the content of a request to the output.
		virtual void print() const;

#if REQUEST_SERIALIZATION || ENABLE_HTTP_TRACE
		/// Serialize the request to a buffer.
		/** Serialize the request to a buffer and/or returns the length in bytes of the
		 * serialized request. Simply returning the length is useful for allocating a
		 * buffer to which the message is then serialized to.
		 * @param buffer the buffer to serialize the request to.
		 * @param write if the data should be written to the buffer. If set to false,
		 * 		only the length of the serialized request will be returned.
		 * @return if write is true, the number of bytes written to the buffer, if
		 * 		write is false, the length of the serialized request.
		 * */
		virtual size_t serialize( char* buffer, bool write ) const;
#endif

#if BODY_ARGS_PARSING
		//TODO check if MIME type is correct in comment.
		/// Finds an argument within a x-www-htmlform and returns it.
		/** This method is a helper for parsing the arguments provided within an
		 * x-www-htmlform encoded body in POST requests.
		 * @param key the name of the argument.
		 * @param value a string where the value will be stored.
		 * @param max_size the maximum allowed size of the value. Since the space for
		 * 	value can be preallocated (if we are expecting arguments of a certain size),
		 * 	its size must be limited to avoid potential overflows.
		 * 	@return the length of the argument's value or 0 if it was not found.
		 * */
		uint8_t find_arg(const char* key, char* value, uint8_t max_size) const;
#endif

		/// Compare a string with the request's method.
		/**
		 * @param m the method string.
		 * @return boolean true if the request method and m match.
		 */
		bool is_method(const char* m) const;

	protected:

		///Parses a line from the message header.
		/**
		 * Override of parent implementation to parse request specific header
		 * lines.
		 * Parses the first line of an HTTP request header and if that line has
		 * already been parsed, hand off parsing to the parent method.
		 * @param line a pointer to a complete line.
		 * @return the result of the parsing.
		 */
		virtual Message::PARSER_RESULT parse_header(char* line);
};

#endif /* REQUEST_H_ */
