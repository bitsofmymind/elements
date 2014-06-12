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
 * Header file for the URL class.
 */

#ifndef URI_H_
#define URI_H_

//INCLUDES
#include <stdint.h>
#include "../utils/utils.h"
#include "../configuration.h"

///TODO: Useless, delete.
#define NUMBER_OF_AUTHORITES 10
#define NUMBER_OF_RESOURCES 10
#define NUMBER_OF_ARGUMENTS 10

class URL
{
	public:
		///The parsing result for a URL.
		enum PARSING_RESULT { VALID, INVALID };

		///The string containing the URL.
		const char* url_str;

		///The length of the string containing the URL.
		size_t url_length;

		///All the resource names contained in the URL.
		List< const char* > resources;
#if URL_PROTOCOL
		///The protocol part of the URL.
		const char* protocol;
#endif
#if URL_AUTHORITY
		///The authority part of the URL.
		const char* authority;
#endif
#if URL_PORT
		///The port part of the URL.
		const char* port;
#endif
#if URL_ARGUMENTS
		///The index arguments of the URL.
		Dictionary< const char* >* arguments;
#endif
#if URL_FRAGMENT
		/**The fragment part of the URL. Normally, browser*/
		const char* fragment;
#endif

	public:
		URL();
		~URL();

		URL::PARSING_RESULT parse(char* str);
#if URL_SERIALIZATION
		size_t serialize( char* buffer , bool write);
#endif
		void print(void);
		bool is_absolute(void);
};


#endif /* URI_H_ */
