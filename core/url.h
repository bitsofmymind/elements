/* url.h - Implements an Uniform Resource Locatorresponse
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

#ifndef URI_H_
#define URI_H_

#define NUMBER_OF_AUTHORITES 10
#define NUMBER_OF_RESOURCES 10
#define NUMBER_OF_ARGUMENTS 10

#include "../utils/utils.h"
#include <stdint.h>
#include "../elements.h"


class URL
{
	public:
		enum PARSING_RESULT { VALID, INVALID };

		const char* url_str;
		size_t url_length;
#if URL_PROTOCOL
		const char* protocol;
#endif
#if URL_AUTHORITY
		const char* authority;
#endif
#if URL_PORT
		const char* port;
#endif
		List< const char* > resources;
#if URL_ARGUMENTS
		Dictionary< const char* >* arguments;
#endif
#if URL_FRAGMENT
		const char* fragment;
#endif
		bool is_absolute_path;
		uint8_t cursor;

	public:
		URL();
		~URL();

		URL::PARSING_RESULT parse(char* str);
#if URL_SERIALIZATION
		size_t serialize( char* buffer , bool write);
#endif
		void print(void);
};


#endif /* URI_H_ */
