/*
 * uri.cpp
 *
 *  Created on: Mar 11, 2009
 *      Author: Antoine
 */

#include <stdlib.h>
#include <pal/pal.h>
#include "url.h"
#include <stdint.h>
#include <string.h>


URL::URL( ):
	cursor(0)
{
	url_str = NULL;
	url_length = 0;
#if URL_PROTOCOL
	protocol = NULL;
#endif
#if URL_AUTHORITY
	authority = NULL;
#endif
#if URL_PORT
	port = NULL;
#endif
#if URL_ARGUMENTS
	arguments = NULL;
#endif
#if URL_FRAGMENT
	fragment = NULL;
#endif

}
URL::~URL()
{
#if URL_ARGUMENTS
	if(arguments)
	{
		delete arguments;
	}
#endif
}

URL::PARSING_RESULT URL::parse(char* str)
{
	url_str = str;

	char* start = str;
#if URL_PROTOCOL
	//PROTOCOL PART
	while(true)
	{
		if( *str == ':' && *(str + 1) == '/' )
		{
			/*We check for ':' and '/' so as not to confuse the protocol with the port*/
			*str = '\0';
			protocol = start;
			str += 3; //jumps the '//'
			break;
		}
		else if( *str == '.' || *str == '/' || *str == '#' || *str == ' ')
		{
			//The url did not contain the protocol part.
			str = start;
			break;
		}
		str++;
	}
#endif

	char next_part = *str;
#if URL_AUTHORITY
	//AUTHORITY PART
	if( *str != '/' && *str != '?' && *str != '#' )
	{
		start = str;

		do
		{
			str++;
		} while(*str != '/' || *str != ':' || *str != '#' || *str != '?' || *str != ' ');
		authority = start;
		next_part = *str;
		*str++ = '\0';
	}
#endif

#if URL_PORT
	//PORT PART
	if(next_part == ':')
	{
		port = start = str;
		while(*str != '/' || *str != ':' || *str != '#' || *str != '?' || *str != ' ');
		next_part = *str;
		*str++ = '\0';
	}
#endif

	//RESOURCE PART
	if( next_part != '?' && next_part != '#' && next_part != ' ' )
	{
		if( next_part == '/' )
		{
			is_absolute_path = true;
			str++;
		}
		else
		{
			is_absolute_path = false;
		}

		start = str;

		while( true )
		{
			if( *str == '/' )
			{
				*str = '\0';
				resources.append( start );
				start = ++str;
				continue;
			}
			else if( *str == '?' || *str == '#' || *str == ' ' )
			{
				next_part = *str;
				if( *(str - 1) == '/' )
				{
					if( resources.items == 0 ){ break; }

					*(str - 1) = '\0';
					resources.append(str - 1);
				}
				else
				{
					/*There is necessarily a resource present because it was verified
					at the beginning of this part.*/
					*str++ = '\0';
					resources.append( start );
				}
				break;
			}
			str++;
		}

	}
	else
	{
		is_absolute_path = false;
	}
#if URL_ARGUMENT
	//ARGUMENT PART
	if( next_part == '?')
	{
		start = str;
		bool is_key = true;
		const char* key;
		arguments = new Dictionary< const char* >();
		for(;;)
		{
			if( is_key )
			{
				if( *str == '=' )
				{
					*str = '\0';
					is_key = false;
					key = start;
					start = ++str;
					continue;
				}
			}
			else
			{
				if( *str == '&' )
				{
					*str = '\0';
					is_key = true;
					arguments->add( key, start );
					start = ++str;
					continue;
				}
				else if( *str == '#' || *str == ' ' )
				{

					arguments->add( key, start );
					next_part = *str;
					*str++ = '\0';
					break;
				}

			}
			str++;
		}
	}
#endif

#if URL_FRAGMENT
	//FRAGMENT PART
	if(next_part == '#')
	{
		fragment = str;
		while( *str++ != ' ' );
		*(str - 1) = '\0';
	}
#endif
	url_length = str - url_str - 1;

	return VALID;
}

size_t URL::serialize(char* destination)
{
	char* start = destination;
#if URL_PROTOCOL
	if( protocol )
	{
		strcpy(destination, protocol);
		destination += strlen(protocol);
		*destination++ = ':'; *destination++ = '/'; *destination++ = '/';
	}
#endif
#if URL_AUTHORITY
	if( authority )
	{
		strcpy(destination, authority);
		destination += strlen(authority);
	}
#endif
#if URL_PORT
	if(port)
	{
		*destination++ = ':';
		strcpy(destination, port);
		destination += strlen(port);
	}
#endif
	if(is_absolute_path){ *destination++ = '/'; }

	for(uint8_t i = 0; i< resources.items; i++)
	{
		strcpy(destination, resources[i]);
		destination += strlen(destination);
		*destination++ = '/';
	}
#if URL_ARGUMENTS
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
#endif
#if URL_FRAGMENT
	if(fragment)
	{
		*destination++ = '#';
		strcpy(destination, fragment);
		destination += strlen(fragment);
	}
#endif
	return destination - start;
}

void URL::print(void)
{
	/*If VERBOSITY, OUTPUT_WARNINGS or OUTPUT_ERRORS is undefined,
	 * this method should be optimized away by the compiler.*/
#if URL_PROTOCOL
	if(protocol)
	{
		DEBUG_PRINT(protocol);
		DEBUG_PRINT_BYTE(':');
	}
#endif
#if URL_AUTHORITY
	if(authority)
	{
		DEBUG_PRINT(authority);
	}
#endif
#if URL_PORT
	if(port)
	{
		DEBUG_PRINT_BYTE(':');
		DEBUG_PRINT(port);
	}
#endif
	if(is_absolute_path)
	{
		DEBUG_PRINT_BYTE('/');
	}
	for(uint8_t i = 0; i < resources.items; i++)
	{
		DEBUG_PRINT(resources[i]);
		DEBUG_PRINT_BYTE('/');
	}
#if URL_ARGUMENTS
	if(arguments)
	{
		DEBUG_PRINT_BYTE('?');
		for(uint8_t i = 0; i < arguments->items; i++)
		{
			DEBUG_PRINT((*arguments)[i]->key)
			DEBUG_PRINT_BYTE('=');
			DEBUG_PRINT((*arguments)[i]->value)
			if(i != arguments->items - 1)
			{
				DEBUG_PRINT_BYTE('&');
			}
		}
	}
#endif
#if URL_FRAGMENT
	if(fragment)
	{
		DEBUG_PRINT_BYTE('#');
		DEBUG_PRINT(fragment);
	}
#endif
}

size_t URL::get_length(void)
{
	size_t length = 0;
#if URL_PROTOCOL
	if( protocol )
	{
		length += strlen(protocol);
		length += 3; //For '://'
	}
#endif
#if URL_AUTHORITY
	if(authority)
	{
		length += strlen(authority);
	}
#endif
#if URL_PORT
	if(port)
	{
		length += strlen(port) + 1; //For ':'
	}
#endif
	if(is_absolute_path)
	{
		length++;
	}
	for(uint8_t i = 0; i<resources.items; i++ )
	{
		length++; //For the '/'
		length += strlen(resources[i]);
	}
#if URL_ARGUMENTS
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
#endif
#if URL_FRAGMENT
	if(fragment)
	{
		length++; //For '#'
		length += strlen(fragment);
	}
#endif
	return length;
}

