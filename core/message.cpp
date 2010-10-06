//============================================================================
// Name        : message.cpp
// Author      : Antoine Mercier-Linteau
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <stdlib.h>
#include "message.h"
//#include "../elements.h"
#include "../utils/utils.h"
#include "../utils/types.h"
#include "url.h"
#ifdef DEBUG
	#include <iostream>
#endif

using namespace Elements;

#ifdef DEBUG
	void Message::print()
	{
		using namespace std;

		cout << "---------- Message header fields ----------" << endl;

		for( uint8_t i = 0; i<fields.items ; i++)
		{
			cout << "Field: ";
			for( uint8_t j = 0; j < fields[i]->key.length; j++)
			{
				cout << fields[i]->key.text[j];
			}
			cout << endl;
			cout << "Value: ";
			for( uint8_t j = 0; j < fields[i]->value->length; j++)
			{
				cout << fields[i]->value->text[j];
			}

			cout << '\n' << endl;
		}

		cout << "---------- Message body ----------" << endl;

		for (uint32_t i = 0; i < body.length; i++)
		{
			cout << this->body.text[i];
		}
		cout << '\n' << endl;
	}
#endif

Message::Message()
{
	to_url_resource_index = 0;
	to_url = new URL();
	from_url = new URL();
}
Message::~Message()
{
	free( message.text );
}

char Message::deserialize(void)
{
	return deserialize( message, message.text );
}

char Message::deserialize( string< uint32_t >& buffer, char* index )
{
	char* start = index;
	string<uint8_t> name;
	string<uint8_t>* value;
	bool is_name = true;

	while( true )
	{
		if( *index >= 'A' && *index <= 'Z' )
		{
			*index += 32;
		}
		else if( is_name && *index == ':' )
		{
			name.text = start;
			name.length = index - start;
			while(*++index == ' ');
			start = index;
			is_name = false;
		}
		else if(*index == '\r' && *(index + 1) == '\n' )
		{
			value = (string<uint8_t>*)malloc(sizeof(string<uint8_t>));
			value->text = start;
			value->length = index - start;
			index += 2;
			fields.add( name, value );
			if( *index == '\r' && *(index + 1) == '\n' )
			{
				index += 2;
				break;
			}
			start = index;
			is_name = true;
		}
		else if(index == (buffer.text + buffer.length))
		{
			return -1;
		}
		else
		{
				index++;
		}
	}

	message = buffer;
	body.length = buffer.length - (index - buffer.text);
	body.text = index;

	return 0;
}

uint32_t Message::get_message_length(void)
{
	key_value_pair<string<uint8_t>*>* kv;
	uint32_t size = 0;

	for( uint8_t i = 0; i < fields.items; i++)
	{
		 kv = fields[i];
		 size += kv->key.length \
			 + 2 /*For ': '*/ \
			 + kv->value->length \
			 + 2; //For CLRF
	}

	size += 2 /*ForCLRF between the fields and the body*/ \
		+ body.length;

	return size;
}

char Message::serialize(void)
{
	if(message.text != NULL )
	{
		free(message.text);
	}

	message.length = get_message_length();
	message.text = (char*)malloc( message.length );

	return serialize( message.text );
}

char Message::serialize( char* buffer )
{
	key_value_pair<Elements::string<uint8_t>*>* kv;

	for( uint8_t i = 0; i < fields.items; i++)
		{
			 kv = fields[i];
			 buffer += kv->key.copy(buffer);
			 *buffer++ = ':'; *buffer++ = ' ';
			 buffer += kv->value->copy(buffer);
			 *buffer++ = '\r';
			 *buffer++ = '\n';
		}
		*buffer++ = '\r';
		*buffer++ = '\n';
		buffer += body.copy(buffer);
		*buffer = '\0'; //So we can print the message to cout
		message.length++;
		return 0;
}


//const string< uint8_t > Message::CACHE_CONTROL = {"cache-control", 1};
//const string< uint8_t > Message::CONNECTION = {"connection", 2 };
//const string< uint8_t > Message::DATE = {"date", 3 };
//const string< uint8_t > Message::PRAGMA = {"pragma", 4 };
//const string< uint8_t > Message::TRAILER = {"trailer", 5 };
//const string< uint8_t > Message::TRANSFER_ENCODING = {"transfer-encoding", 6 };
//const string< uint8_t > Message::UPGRADE = {"upgrade", 7 };
//const string< uint8_t > Message::VIA = {"via", 8 };
//const string< uint8_t > Message::WARNING = {"warning", 9 };

//const string< uint8_t > Message::CONTENT_ENCODING = {"content-encoding", 10 };
//const string< uint8_t > Message::CONTENT_LANGUAGE = {"content-language", 11 };
//const string< uint8_t > Message::CONTENT_LENGTH = {"content-length", 12 };
//const string< uint8_t > Message::CONTENT_LOCATION = {"content-location", 13 };
//const string< uint8_t > Message::CONTENT_MD5 = {"content-md5", 14 };
//const string< uint8_t > Message::CONTENT_RANGE = {"content-range", 15 };
const string< uint8_t > Message::CONTENT_TYPE = MAKE_STRING("content-type");
//const string< uint8_t > Message::EXPIRES = {"expires", 17 };
//const string< uint8_t > Message::LAST_MODIFIED = {"last-modified", 18 };
//const string< uint8_t > Message::FROM_URL = {"from-url", 48 };

