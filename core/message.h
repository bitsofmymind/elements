/*
 * message.h
 *
 *  Created on: Feb 16, 2009
 *      Author: Antoine
 */

#ifndef MESSAGE_H_
#define MESSAGE_H_

//#include "../elements.h"
#include "url.h"
#include "../utils/utils.h"
#include "../utils/types.h"

using namespace Elements;

class Message
{
	public:
		enum TYPE { REQUEST, RESPONSE, UNKNOWN };

		//General header fields
		//static const string< uint8_t > CACHE_CONTROL;
		//static const string< uint8_t > CONNECTION;
		//static const string< uint8_t > DATE;
		//static const string< uint8_t > PRAGMA;
		//static const string< uint8_t > TRAILER;
		//static const string< uint8_t > TRANSFER_ENCODING;
		//static const string< uint8_t > UPGRADE;
		//static const string< uint8_t > VIA;
		//static const string< uint8_t > WARNING;

		//Entity header fields
		//static const string< uint8_t > CONTENT_ENCODING;
		//static const string< uint8_t > CONTENT_LANGUAGE;
		//static const string< uint8_t > CONTENT_LENGTH;
		//static const string< uint8_t > CONTENT_LOCATION;
		//static const string< uint8_t > CONTENT_MD5;
		//static const string< uint8_t > CONTENT_RANGE;
		static const Elements::string< uint8_t > CONTENT_TYPE;
		//static const string< uint8_t > EXPIRES;
		//static const string< uint8_t > LAST_MODIFIED;

		//Elements header fields
		static const Elements::string< uint8_t > FROM_URL;

		URL* to_url;
		URL* from_url;

		uint8_t to_url_resource_index;


		Elements::string< uint32_t > message;
		Elements::string< uint32_t > body;
		Dictionary< Elements::string<uint8_t> > fields;

	public:
		Message();
		virtual ~Message();


		#ifdef DEBUG
			virtual void print();
		#endif
 		virtual TYPE get_type( void ) = 0;
 		virtual uint32_t get_message_length(void);
		virtual char deserialize(Elements::string< uint32_t>& buffer, char* index );
		char deserialize();
		char serialize(void);
		virtual char serialize( char* buffer );
};

#endif /* MESSAGE_H_ */
