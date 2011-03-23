/*
 * uri.h
 *
 *  Created on: Mar 11, 2009
 *      Author: Antoine
 */

#ifndef URI_H_
#define URI_H_

//url.h

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
		const char* protocol;
		const char* authority;
		const char* port;
		List< const char > resources;
		Dictionary< const char >* arguments;
		const char* fragment;
		bool is_absolute_path;
		uint8_t cursor;

	public:
		URL();
		~URL();

		URL::PARSING_RESULT parse(char* str);
		size_t serialize( char* destination );
		size_t get_length(void);

		void print(void);



};


#endif /* URI_H_ */
