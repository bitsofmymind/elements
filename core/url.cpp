// SVN FILE: $Id: $
/**
 * @lastChangedBy           $lastChangedBy: Mercier $
 * @revision                $Revision: 397 $
 * @copyright    			GNU General Public License
 * 		This program is free software: you can redistribute it and/or modify
 * 		it under the terms of the GNU General Public License as published by
 * 		the Free Software Foundation, either version 3 of the License, or
 * 		(at your option) any later version.
 * 		This program is distributed in the hope that it will be useful,
 * 		but WITHOUT ANY WARRANTY; without even the implied warranty of
 *		MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * 		GNU General Public License for more details.
 * 		You should have received a copy of the GNU General Public License
 * 		along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * Source file for the URL class.
 */

//INCLUDES
#include <stdlib.h>
#include <pal/pal.h>
#include <stdint.h>
#include <string.h>
#include "url.h"

///URL implements an URL for use with the Elements framework.
/** This class parses and manages an URL's many parts for ease of
 * manipulation by objects who need to work with urls.
 * @class Response
 * */

///Class constructor.
///@todo move all the values below to the initialization list.
URL::URL( )
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

///Class destructor.
URL::~URL()
{
#if URL_ARGUMENTS
	if(arguments) //If the arguments dictionnary has been allocated.
	{
		delete arguments; //Delete it.
	}
#endif
}

///Parses a URL string.
/** This method will decompose an URL into its many parts:
 * protcol://authority:port/resource/resource/?argument=argument#fragment
 * If the framework is configured to ignore a part, it will not be parsed.
 * The parser does copy string but keeps them in place and instead isolates
 * them by replacing their separators with null characters.
 * @param str the url string.
 * @return the result of the parsing.
 * @todo check if we have reached the end of the URL.
 * */
URL::PARSING_RESULT URL::parse(char* str)
{
	url_str = str; //Save the string.

	//Holds the start of the part of the URL that is currently being parsed.
	char* start = str;
#if URL_PROTOCOL //If the framework is configured to parse protocols.
	//PROTOCOL PART
	while(true)
	{
		/*If we have reached the authority part. We check for ':' and '/' so
		as not to confuse the protocol  with the port*/
		if( *str == ':' && *(str + 1) == '/' )
		{
			*str = '\0'; //Replace the separator by a NULL character.
			protocol = start; //Set the start of the protocol part.
			str += 3; //jumps the '//'
			break; //Done parsing the protocol.
		}
		//If there is no protocol.
		else if( *str == '.' || *str == '/' || *str == '#' || *str == ' ')
		{
			str = start; //Move the str pointer back to the beginning.
			break; //Done parsing the protocol.
		}
		str++;
	}
#endif

	char next_part = *str; //Holds the start character of the next part.
#if URL_AUTHORITY //If the framework is configured to parse authorities.
	//AUTHORITY PART
	//If the next part begins by a separator, then there is no authority.
	if( next_part != '/' && next_part != '?' && next_part != '#' )
	{
		start = str; //Move the start pointer.

		do //Find the next separator.
		{
			str++;
		} while(*str != '/' || *str != ':' || *str != '#' || *str != '?' || *str != ' ');
		authority = start; //Save the start.
		next_part = *str; //Save the beginning character of the next part.
		*str++ = '\0'; //Replace the separator by a NULL character.
	}
#endif

#if URL_PORT //If the framework is configured to parse the port.
	//PORT PART
	if(next_part == ':') //If the next part starts with a : then there is a port.
	{
		//TODO Bug! the : is included with the port.
		port = start = str; //Save the next part.
		//TODO bug! this will loop infinetly ! Should add ++ to the end.
		//Go to the beginning of the next part.
		while(*str != '/' || *str != ':' || *str != '#' || *str != '?' || *str != ' ');
		next_part = *str; //Save the beginning character of the next part.
		*str++ = '\0'; //Replace the separator by a NULL character.
	}
#endif

	//RESOURCE PART
	/*If the next part is not a beginning character for resource, then there
	 * are no resources.*/
	if( next_part != '?' && next_part != '#' && next_part != ' ' )
	{
		start = str; //Save the start of the resource part.

		while(true) //Loop to decompose the resources part.
		{
			if(*str == '/') //If we have found the end of a resource.
			{
				*str = '\0'; //Replace the separator by a NULL character.
				resources.append( start ); //Save that resource.
				start = ++str; //Move the start pointer after that resource.
				continue; //Keep looking for other resources.
			}
			//If we have reached the end of the resource part.
			else if(*str == '?' || *str == '#' || *str == ' ')
			{
				next_part = *str; //Save the beginning character.
				//If the previous resource did not finish with a "/".
				if( *(str - 1) != '\0' )
				{
					resources.append(start); //It has not been saved yet.
				}
				/*If the resource did finish with a "/", it has been accounted
				 * for previously. */
				*str++ = '\0'; //Replace the separator by a NULL character.
				break; //Done parsing resources.
			}
			str++;
		}
	}

#if URL_ARGUMENT //If the framework is configured to parse authorities.
	//ARGUMENT PART
	//If the next part starts with a ? then there are arguments.
	if(next_part == '?')
	{
		start = str; //Save the start of the arguments.
		bool is_key = true; //True if we are parsing a key.
		const char* key; //Pointer to the key.
		//Initialize the arguments dictionary.
		arguments = new Dictionary< const char* >();
		while(true)
		{
			if(is_key) //If we are parsing for a key.
			{
				if( *str == '=' ) //If we have reached the key=value delimiter.
				{
					*str = '\0'; //Replace the separator by a NULL character.
					is_key = false; //We will be parsing for a value.
					key = start; //Save the key.
					start = ++str; //Move the start pointer.
					continue; //Keep going.
				}
			}
			else //We are parsing a value.
			{
				if(*str == '&') //If we have reached the end of an argument.
				{
					*str = '\0'; //Replace the separator by a NULL character.
					is_key = true; //We will be parsing for a key.
					arguments->add( key, start ); //Save the key=value pair.
					start = ++str; //Move the start pointer.
					continue; //Keep going.
				}
				//If we have reached the end of the arguments part.
				else if(*str == '#' || *str == ' ')
				{
					arguments->add( key, start ); .//Save the last argument.
					next_part = *str; //Save the beginning character.
					*str++ = '\0'; //Replace the separator by a NULL character.
					break; //Done parsing arguments.
				}

			}
			str++;
		}
	}
#endif

#if URL_FRAGMENT //If the framework is configured to parse the fragment.
	//FRAGMENT PART
	if(next_part == '#') //If there is a fragment part.
	{
		//TODO bug! the # will be saved with the fragment.
		fragment = str; //Save the start of the fragment.
		while(*str++ != ' ');
		*(str - 1) = '\0'; //Replace the separator by a NULL character.
	}
#endif
	///TODO the url_length is most likely useless.
	url_length = str - url_str - 1; //Save the length of the url.

	return VALID; //URL is valid.
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
		DEBUG_PRINT(':');
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
		DEBUG_PRINT(':');
		DEBUG_PRINT(port);
	}
#endif
	for(uint8_t i = 0; i < resources.items; i++)
	{
		DEBUG_PRINT(resources[i]);
		DEBUG_PRINT('/');
	}
#if URL_ARGUMENTS
	if(arguments)
	{
		DEBUG_PRINT('?');
		for(uint8_t i = 0; i < arguments->items; i++)
		{
			DEBUG_PRINT((*arguments)[i]->key)
			DEBUG_PRINT('=');
			DEBUG_PRINT((*arguments)[i]->value)
			if(i != arguments->items - 1)
			{
				DEBUG_PRINT('&');
			}
		}
	}
#endif
#if URL_FRAGMENT
	if(fragment)
	{
		DEBUG_PRINT('#');
		DEBUG_PRINT(fragment);
	}
#endif
}

bool URL::is_absolute(void)
{
	if(resources.items && *(resources[0]) == '\0')
	{
		return true;
	}
	return false;
}
