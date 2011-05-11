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
#if URL_SERIALIZATION
size_t URL::serialize(char* buffer, bool write)
{
	char* start = buffer;
#if URL_PROTOCOL
	if( protocol )
	{
		if(write) { strcpy(buffer, protocol); }
		buffer += strlen(protocol);
		if(write)
		{
			*buffer = ':'; *( buffer + 1 ) = '/'; *(buffer + 2) = '/';
		}
		buffer += 3;
	}
#endif
#if URL_AUTHORITY
	if( authority )
	{
		if( write ){ strcpy(buffer, authority); }
		buffer += strlen(authority);
	}
#endif
#if URL_PORT
	if(port)
	{
		if( write ){ *buffer = ':' };
		buffer++;
		if( write ){ strcpy(buffer, port); }
		buffer += strlen(port);
	}
#endif
	if(is_absolute_path)
	{
		if(write){ *buffer = '/'; }
		buffer++;
	}

	for(uint8_t i = 0; i< resources.items; i++)
	{
		if( write ){ strcpy(buffer, resources[i]); }
		buffer += strlen(resources[i]);
		if( write ){ *buffer = '/'; }
		buffer++;
	}
#if URL_ARGUMENTS
	if (arguments )
	{
		key_value_pair<const char*>* kv;
		if( write ){ *buffer = '?'; }
		buffer++;
		for(uint8_t i = 0; i< arguments->items; i++)
		{
			kv = (*arguments)[i];
			if( write ){ strcpy(buffer, kv->key); }
			buffer += strlen(buffer);
			if( write ) {*buffer = '='; }
			buffer += 1;
			if( write ){ strcpy(buffer, kv->value); }
			buffer += strlen(buffer);
			if( write ){ *buffer = '&'; }
			buffer += 1;
		}
		if(arguments->items)
		{
			buffer--; //We do not need the last '&'
		}
	}
#endif
#if URL_FRAGMENT
	if(fragment)
	{
		if( write ){ *buffer = '#'; }
		buffer++;
		if( write ){ strcpy(buffer, fragment); }
		buffer += strlen(fragment);
	}
#endif
	return buffer - start;
}
#endif

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
		DEBUG_TPRINT('/', BYTE);
	}
	for(uint8_t i = 0; i < resources.items; i++)
	{
		DEBUG_PRINT(resources[i]);
		DEBUG_TPRINT('/', BYTE);
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
