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

#include "../utils/types.h"
#include "../utils/utils.h"
#include <stdint.h>
#include "../elements.h"

using namespace Elements;


class URL
{
	public:
		Elements::string<uint8_t> url;
		Elements::string<uint8_t> protocol;
		List< Elements::string<uint8_t> >* authorities;
		Elements::string< uint8_t> port;
		List< Elements::string<uint8_t> > resources;
		Dictionary< Elements::string< uint8_t > >* arguments;
		Elements::string<uint8_t> fragment;
		bool valid;
		bool is_absolute_url;
		bool is_absolute_path;
		uint8_t cursor;

	public:
		URL();
		~URL();

		void deserialize(char* url_string);
		int8_t serialize( char* destination );
		int8_t serialize_authority( char* destination );
		int8_t serialize_resource( char* destination );
		uint8_t get_length(void);
		#ifdef DEBUG
			void print(void);
		#endif


};


#endif /* URI_H_ */
