/*
 * uri.cpp
 *
 *  Created on: Mar 11, 2009
 *      Author: Antoine
 */

#include <stdlib.h>
#include "../utils/types.h"
#include <pal/pal.h>
#include "url.h"
#include <stdint.h>

using namespace Elements;

URL::URL( )
{
	this->url.text = NULL;
	this->url.length = 0;
	port.text = NULL;
	port.length = 0;
	protocol.text = NULL;
	protocol.length = 0;
	fragment.text = NULL;
	fragment.length = 0;
	valid = false;
	authorities = NULL;
	arguments = 0;
	cursor = 0;
}
URL::~URL()
{
	if(authorities)
	{
		while(authorities->items)
		{
			ts_free(authorities->remove(0));
		}
		delete authorities;
	}
	//delete args if necessary
}

void URL::parse(char* str)
{
	url.text = str;

	uint8_t index = 0;
	string<uint8_t>* temp;

	//PROTOCOL PART
	while(true)
	{
		if( url.text[index] == ':' )
		{
			protocol.length = index;
			protocol.text = url.text;
			index += 3; //jumps the '//'
			break;
		}
		else if( url.text[index] == '.' || url.text[index] == '/' || url.text[index] == '#')
		{

			protocol.text = NULL;
			protocol.length = 0;
			index = 0; //The url did not contain the protocol part.
			break;
		}
		index++;
	}


	//AUTHORITY PART
	if( url.text[index] != '.' && url.text[index] != '/' && url.text[index] != '#' )
	{
		//authorities = new List< string<uint8_t> >();
		uint8_t start = index;
		authorities = new List< string<uint8_t> >();

		while(true)
		{
			if(url.text[index] == '.')
			{
				temp = (string<uint8_t>*)ts_malloc(sizeof(string<uint8_t>));
				temp->text = url.text + start;
				temp->length = index - start;
				authorities->append( temp );
				start = index + 1;
			}
			else if( url.text[index] == '/' || url.text[index] == ':' || url.text[index] == '#' || url.text[index] == '?' )
			{
				temp = (string<uint8_t>*)ts_malloc(sizeof(string<uint8_t>));
				temp->text = url.text + start;
				temp->length = index - start;
				authorities->append( temp );
				break;
			}
			index++;
		}
		is_absolute_url = true;
	}
	else
	{
		is_absolute_url = false;
	}


	//PORT PART

	if(url.text[index] == ':')
	{
		index++; // jumps the ':'
		uint8_t start = index;
		port.text = url.text + index;
		while(true)
		{
			if(url.text[index] < '0' || url.text[index] > '9') { break; }
			index++;
		}
		port.length = index - start;
	}
	else
	{
		port.text = NULL;
		port.length = 0;
	}

	char end_of_resources_char = '\0';

	//RESOURCE PART
	if( url.text[index] != '?' || url.text[index] != '#' || url.text[index] != ' ' )
	{
		if( url.text[index] == '/' )
		{
			is_absolute_path = true;
			index++;
		}
		else
		{
			is_absolute_path = false;
		}

		uint8_t start = index;

		while( true )
		{
			if( url.text[index] == '/' )
			{
				url.text[index] = '\0';
				resources.append( url.text + start );
				start = index + 1;
			}
			else if( url.text[index] == '?' || url.text[index] == '#' || url.text[index] == ' ' )
			{
				end_of_resources_char = url.text[index];
				if( url.text[index-1] == '/' )
				{
					if( resources.items == 0 ){ break; }

					url.text[index-1] = '\0';
					resources.append(url.text + index);
				}
				else
				{
					/*There is necessarily a resource present because it was verified
					at the beginning of this part.*/
					url.text[index] = '\0';
					resources.append( url.text + start );
				}
				break;
			}
			index++;
		}

	}
	else
	{
		is_absolute_path = true;
	}



	//ARGUMENT PART
	if( end_of_resources_char == '?')
	{
		index++; //jumps the '?'
		uint8_t start = index;
		bool is_key = true;
		const char* key;
		arguments = new Dictionary< const char >();
		for(;;)
		{
			if( is_key )
			{
				if( url.text[index] == '=' )
				{
					url.text[index] = '\0';
					is_key = false;
					key = url.text + start;
					start = index + 1;
				}
			}
			else
			{
				if( url.text[index] == '&' )
				{
					url.text[index] = '\0';
					is_key = true;
					temp = (string<uint8_t>*)ts_malloc(sizeof(string<uint8_t>));
					arguments->add( key, url.text+start );
					start = index + 1;
				}
				else if( url.text[index] == '#' || url.text[index] == ' ' )
				{

					end_of_resources_char = url.text[index];
					url.text[index] = '\0';
					arguments->add( key, url.text + start );
					break;
				}

			}
			index++;
		}
	}

	//FRAGMENT PART
	if(end_of_resources_char == '#')
	{
		fragment.text = url.text + index + 1;
		uint8_t start = index + 1;
		while( url.text[index] != ' ' ){ index++; }
		fragment.length = index - start;
	}

	url.length = index;
	valid = true;
}

int8_t URL::serialize(char* destination)
{
	int8_t code = serialize_authority(destination);
	if( code > 0) { return code; }
	return serialize_resource(destination);
}

void URL::print(void)
{
	/*If VERBOSITY, OUTPUT_WARNINGS or OUTPUT_ERRORS is undefined,
	 * this method should be optimizes away by the compiler.*/

	if(protocol.length)
	{
		DEBUG_NPRINT(protocol.text, protocol.length);
		DEBUG_PRINT_BYTE(':');
	}
	if(authorities)
	{
		for(uint8_t i = 0; i < authorities->items; i++)
		{
			DEBUG_NPRINT((*authorities)[i]->text, (*authorities)[i]->length);
			DEBUG_PRINT_BYTE('.');
		}
	}
	if(port.length)
	{
		DEBUG_PRINT_BYTE(':');
		DEBUG_NPRINT(port.text, port.length);
	}
	if(is_absolute_path)
	{

	}
	for(uint8_t i = 0; i < resources.items; i++)
	{
		DEBUG_PRINT(resources[i]);
		DEBUG_PRINT_BYTE('/');
	}

	//TODO: print the rest of the url

}

int8_t URL::serialize_authority( char* destination )
{
	if( protocol.length )
	{
		destination += protocol.copy(destination);
		*destination++ = ':'; *destination++ = '/'; *destination++ = '/';
	}
	if( authorities )
	{
		for( uint8_t i = 0; i < authorities->items ; i++ )
		{
			destination += (*authorities)[i]->copy(destination);
		}
	}
	if(port.length )
	{
		*destination++ = ':';
		destination += port.copy(destination);
	}

	return 0;
}
int8_t URL::serialize_resource( char* destination )
{
	if(is_absolute_path){*destination++ = '/';}

	for(uint8_t i = 0; i< resources.items; i++)
	{
		strcpy(destination, resources[i]);
		destination += strlen(destination);
		*destination++ = '/';
	}
	if (arguments )
	{
		key_value_pair<const char*>* kv;
		*destination++ = '?';
		for(uint8_t i = 0; i< arguments->items; i++)
		{
			kv = (*arguments)[i];
			strcpy(destination, kv->key);
			destination += strlen(destination);
			*destination++ = '=';
			strcpy(destination, kv->value);
			destination += strlen(destination);
			*destination++ = '&';
		}
		if(arguments->items)
		{
			destination--; //We do not need the last '&'
		}
	}
	if(fragment.length)
	{
		*destination++ = '#';
		destination += fragment.copy(destination);
	}
	return 0;
}

uint8_t URL::get_length(void)
{
	uint8_t length = protocol.length;

	if( protocol.length )
	{
		length += 3; //For '://'
	}
	if(authorities)
	{
		for(uint8_t i = 0; i < authorities->items; i++)
		{
			length += (*authorities)[i]->length;
		}
		if(authorities->items > 0)
		{
			length += authorities->items - 1; //For the '.'
		}
	}
	if(is_absolute_path)
	{
		length++;
	}
	for(uint8_t i = 0; i<resources.items; i++ )
	{
		length++; //For the '/'
		length += strlen(resources[i]);
	}
	if(arguments)
	{
		key_value_pair<const char*>* kv;
		length++; //For the '?'
		for(uint8_t i = 0; i< arguments->items; i++)
		{
			kv = (*arguments)[i];
			length += strlen(kv->key);
			length += strlen(kv->value);
		}
		if( arguments->items)
		{
			length += arguments->items - 1; //For the '&'
			length += arguments->items - 1; //For the '='
		}
	}

	if(fragment.length)
	{
		length++; //For '#'
		length += fragment.length;
	}

	return length;
}

