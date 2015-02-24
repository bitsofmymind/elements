/* url.h - Header file for the URL class.
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

#ifndef URL_H_
#define URL_H_

//INCLUDES
#include <stdint.h>
#include "../utils/utils.h"
#include <configuration.h>

///TODO: Useless, delete.
#define NUMBER_OF_AUTHORITES 10
#define NUMBER_OF_RESOURCES 10
#define NUMBER_OF_ARGUMENTS 10

///URL implements an URL for use with the Elements framework.
/** This class parses and manages an URL's many parts for ease of
 * manipulation by objects who need to work with urls.
 * */
class URL
{
	public:

		///The parsing result for a URL.
		enum PARSING_RESULT { VALID = 0, INVALID, OUT_OF_MEMORY };

	protected:

#if URL_PROTOCOL
		///The protocol part of the URL.
		char* protocol;
#endif

#if URL_AUTHORITY
		///The authority part of the URL.
		char* authority;
#endif

#if URL_PORT && URL_AUTHORITY
		///The port part of the URL.
		char* port;
#endif

#if URL_FRAGMENT
		/**The fragment part of the URL. Normally, browsers don't provide this
		 * part to the server.*/
		char* fragment;
#endif

		///All the resource names contained in the URL.
		List<const char*> resources;

#if URL_ARGUMENTS
		///The indexed arguments of the URL.
		Dictionary<const char*>* arguments;
#endif

	public:

		///Class constructor.
		URL();

		///Class destructor.
		~URL();

#if URL_PROTOCOL
		/** @return the protocol part of the URL. */
		inline const char* get_protocol(void) const { return protocol; }
#endif

#if URL_AUTHORITY
		/** @return the authority part of the URL. */
		inline const char* get_authority(void) const { return authority; }
#endif

#if URL_PORT && URL_AUTHORITY
		/** @return the port part of the URL. */
		inline const char* get_port(void) const { return port; }
#endif

#if URL_FRAGMENT
		/** @return the fragment part of the URL. */
		inline const char* get_fragment(void) const { return fragment; }
#endif

		/** @return the resource list of the URL. */
		inline List<const char*> * get_resources(void) { return &resources; }

#if URL_ARGUMENTS
		/** @return the argument list of the URL. */
		inline Dictionary<const char*>* get_arguments(void) const { return arguments; }
#endif

		/** Append a resource to the url by copying the name into the internal
		 * resources list.
		 * @param name the resource to add.
		 * @param length the length of the name.
		 * @return boolean if there was enough memory to complete the operation.*/
		bool append_resource(const char* name, int8_t length);

		/** Append a resource to the url by copying the name into the internal
		 * resources list.
		 * @param name the resource to add.
		 * @param position the position at which the resource should be inserted.
		 * @param length the length of the name.
		 * @return boolean if there was enough memory to complete the operation.*/
		bool insert_resource(const char* name, uint8_t position, int8_t length);

		///Parses a URL string.
		/**
		 * This method will decompose an URL into its many parts:
		 * protocol://authority:port/resource/resource/?argument=argument#fragment
		 * If the framework is configured to ignore a part, it will not be parsed.
		 * @param str the url string.
		 * @return the result of the parsing.
		 * */
		URL::PARSING_RESULT parse(const char* str);

#if URL_SERIALIZATION
		/// Serializes and url into a string.
		/**
		 * Serializes an url into a string buffer. When write is set to false,
		 * the url is not actually written to the buffer but its size is
		 * returned.
		 * @param buffer the buffer to write the url to.
		 * @param write if the url's characters should be written to the buffer.
		 * @return the number of bytes written.
		 * */
		size_t serialize(char* buffer, bool write) const;
#endif

		/// Prints the url to the output.
		void print(void) const;

		/**
		 * @return if the url is absolute or relative.
		 * */
		bool is_absolute(void) const;
};

#endif /* URL_H_ */
