//============================================================================
// Name        : message.cpp
// Author      : Antoine Mercier-Linteau
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <stdlib.h>
#include "message.h"
#include "../elements.h"
#include "../utils/utils.h"
#include "../utils/types.h"
#include <pal/pal.h>


using namespace Elements;

#ifdef DEBUG
	void Message::print()
	{
		for( uint8_t i = 0; i<fields.items ; i++)
		{
			Debug::print("   ");
			Debug::print(fields[i]->key.text, fields[i]->key.length);
			Debug::print(": ");
			Debug::println(fields[i]->value->text, fields[i]->value->length);
		}

		Debug::print("   body: ");
		if(body_file)
		{
			Debug::println(body_file->size, DEC);
		}
		else
		{
			Debug::println(0);
		}
	}
#endif

Message::Message()
{
	message.length = 0;
	message.text = NULL;
	body_file = NULL;
}
Message::~Message()
{
	while(fields.items)
	{
		ts_free(fields.remove(fields[0]->key));
	}
	if(message.length)
	{
		ts_free(message.text);
	}
	if(body_file)
	{
		delete body_file;
	}
}

char Message::deserialize(void)
{
	return deserialize( message, message.text );
}

char Message::deserialize( string< MESSAGE_SIZE >& buffer, char* index )
{
	char* start = index;
	string<uint8_t> name;
	string<uint8_t>* value;
	bool is_name = true;

	for(;;)
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
			if(!is_name)
			{
				value = (string<uint8_t>*)ts_malloc(sizeof(string<uint8_t>));
				value->text = start;
				value->length = index - start;
				fields.add( name, value );
			}
			else
			{
				index -= 2;
			}

			index += 2;
			if( *index == '\r' && *(index + 1) == '\n' )
			{
				break;
			}
			start = index;
			is_name = true;
		}
		else if(index >= (buffer.text + buffer.length))
		{
			return -1;
		}
		else
		{
				index++;
		}
	}

	index += 2; //Skips the '\r\n'
	message = buffer;
	MESSAGE_SIZE body_size = buffer.length - (index - buffer.text);
	if( body_size > 0)
	{
		body_file = new ConstFile<MESSAGE_SIZE>(index, body_size);
	}


	return 0;
}

MESSAGE_SIZE Message::get_message_length(void)
{
	key_value_pair<string<uint8_t>*>* kv;
	MESSAGE_SIZE size = 0;

	for( uint8_t i = 0; i < fields.items; i++)
	{
		 kv = fields[i];
		 size += kv->key.length \
			 + 2 /*For ': '*/ \
			 + kv->value->length \
			 + 2; //For CLRF
	}

	size += 2 /*ForCLRF between the fields and the body*/ \
		+ body_file->size;

	return size;
}

char Message::serialize(void)
{
	if(message.text != NULL )
	{
		ts_free(message.text);
	}

	message.length = get_message_length();
	message.text = (char*)ts_malloc( message.length );

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
	//buffer += body.copy(buffer);
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

