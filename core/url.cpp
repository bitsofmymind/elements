/*
 * uri.cpp
 *
 *  Created on: Mar 11, 2009
 *      Author: Antoine
 */

#include <stdlib.h>
#include "../elements.h"
#include "../utils/types.h"
#include "url.h"
#include <stdint.h>

#ifdef DEBUG
	#include <iostream>
#endif

using namespace Elements;

URL::URL( )
{
	this->url.text = NULL;
	this->url.length = 0;
	valid = false;
	authorities = 0;
	arguments = 0;
	cursor = 0;
}
URL::~URL()
{
	if(authorities)
	{
		while(authorities->items)
		{
			free(authorities->remove(0));
		}
		delete authorities;
	}
	while(resources.items)
	{
		free(resources.remove(0));
	}
}

void URL::deserialize(char* url_string)
{
	url.text = url_string;

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
				temp = (string<uint8_t>*)malloc(sizeof(string<uint8_t>));
				temp->text = url.text + start;
				temp->length = index - start;
				authorities->append( temp );
				start = index + 1;
			}
			else if( url.text[index] == '/' || url.text[index] == ':' || url.text[index] == '#' || url.text[index] == '?' )
			{
				temp = (string<uint8_t>*)malloc(sizeof(string<uint8_t>));
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
				temp = (string<uint8_t>*)malloc(sizeof(string<uint8_t>));
				temp->text = url.text + start;
				temp->length = index - start;
				resources.append( temp );
				start = index + 1;
			}
			else if( url.text[index] == '?' || url.text[index] == '#' || url.text[index] == ' ' )
			{
				if( url.text[index-1] == '/' )
				{
					if( resources.items == 0 ){ break; }
					temp = (string<uint8_t>*)malloc(sizeof(string<uint8_t>));
					temp->text = url.text + index;
					temp->length = 1;
					resources.append( temp );
				}
				else
				{
					/*There is necessarily a resource present because it was verified
					at the beginning of this part.*/
					temp = (string<uint8_t>*)malloc(sizeof(string<uint8_t>));
					temp->text = url.text + start;
					temp->length = index - start;
					resources.append( temp );
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
	if( url.text[index] == '?')
	{
		index++; //jumps the '?'
		uint8_t start = index;
		bool is_key = true;
		string<uint8_t> key;
		arguments = new Dictionary< string< uint8_t > >();
		for(;;)
		{
			if( is_key )
			{
				if( url.text[index] == '=' )
				{
					is_key = false;
					key.text = url.text + start;
					key.length = index - start;
					start = index + 1;
				}
			}
			else
			{
				if( url.text[index] == '&' )
				{
					is_key = true;
					temp = (string<uint8_t>*)malloc(sizeof(string<uint8_t>));
					temp->text = url.text + start;
					temp->length = index - start;
					arguments->add( key, temp );
					start = index + 1;
				}
				else if( url.text[index] == '#' || url.text[index] == ' ' )
				{
					temp = (string<uint8_t>*)malloc(sizeof(string<uint8_t>));
					temp->text = url.text + start;
					temp->length = index - start;
					arguments->add( key, temp );
					break;
				}

			}
			index++;
		}
	}

	//FRAGMENT PART
	if(url.text[index] == '#')
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

#ifdef DEBUG

void URL::print(void)
{
	char* urlstr = (char*)malloc(get_length()+1);
	//serialize(urlstr);
	urlstr[get_length()] = 0;
	std::cout << urlstr;
	free(urlstr);
}

#endif

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
		destination += resources[i]->copy(destination);
		*destination++ = '/';
	}
	if (arguments )
	{
		key_value_pair<string<uint8_t>*>* kv;
		*destination++ = '?';
		for(uint8_t i = 0; i< arguments->items; i++)
		{
			kv = (*arguments)[i];
			destination += kv->key.copy(destination);
			*destination++ = '=';
			destination += kv->value->copy(destination);
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
		length += resources[i]->length;
	}
	if(arguments)
	{
		key_value_pair<string<uint8_t>*>* kv;
		length++; //For the '?'
		for(uint8_t i = 0; i< arguments->items; i++)
		{
			kv = (*arguments)[i];
			length += kv->key.length;
			length += kv->value->length;
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

