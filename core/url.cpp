/* url.cpp - Source file for the URL class.
 * Copyright (C) 2015 Antoine Mercier-Linteau
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

//INCLUDES
#include <stdlib.h>
#include <pal/pal.h>
#include <stdint.h>
#include <string.h>
#include "url.h"

URL::URL( )
{
#if URL_PROTOCOL
	protocol = NULL;
#endif
#if URL_AUTHORITY
	authority = NULL;
#endif
#if URL_PORT && URL_AUTHORITY
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
#if URL_PROTOCOL //If the framework is configured to parse protocols.
	free(protocol);
#endif

#if URL_AUTHORITY //If the framework is configured to parse authorities.
	free(authority);
#endif

#if URL_PORT //If the framework is configured to parse the port.
	free(port);
#endif

	resources.free_all(); // Remove and free all resources.

#if URL_ARGUMENTS
	if(arguments) //If the arguments dictionary has been allocated.
	{
		arguments->free_all(true);
		delete arguments; //Delete it.
	}
#endif

#if URL_FRAGMENT //If the framework is configured to parse the fragment.
	free(fragment);
#endif

}

bool URL::append_resource(const char* name, int8_t length = -1)
{
	return insert_resource(name, resources.get_item_count(), length);
}

bool URL::insert_resource(const char* name, uint8_t position, int8_t length = -1)
{
	// If the length of the string is not specified, get it.
	length = length != -1 ? length: strlen(name);

	// Allocate memory for the name (add 1 for the terminating null character).
	char* buffer = (char*)malloc(length + 1);

	if(!buffer) // If there is not enough memory.
	{
		return false;
	}

	memcpy(buffer, name, length); // Copy the name into the buffer.

	buffer[length] = '\0'; // Terminate the string;

	// If there is not enough memory to append the name to the list.
	if(resources.insert(buffer, position) != 0)
	{
		free(buffer);
		return false;
	}

	return true; // Done.
}

URL::PARSING_RESULT URL::parse(const char* str)
{
	// Holds the start of the part of the URL that is currently being parsed.
	const char* start = str;

#if URL_PROTOCOL //If the framework is configured to parse protocols.

	//PROTOCOL PART
	while(true)
	{
		/* If we have reached the authority part. We check for ':' and '/' so
		 * as not to confuse the protocol with the port*/
		if(*str == ':' && *(str + 1) == '/' && *(str + 2) == '/')
		{
			// Allocate space to hold the protocol.
			protocol = (char*)malloc(str - start + 1);

			if(!protocol) // If memory allocation failed.
			{
				return OUT_OF_MEMORY;
			}

			// Copy the protocol string.
			memcpy(protocol, start, str - start);

			protocol[str - start] = '\0'; // Terminate the protocol.

			str += 3; //jumps the '//'

			break; //Done parsing the protocol.
		}
		//If there is no protocol.
		else if(*str == '.' || *str == '/' || *str == '#' || *str == ' ' || *str == '?')
		{
			str = start; //Move the str pointer back to the beginning.
			break; //Done parsing the protocol.
		}
		else if(*str == '\0') // If we have reached a null character.
		{
			return INVALID; // The URL is invalid.
		}

		str++;
	}

#endif

	// TODO could use *str instead...
	char next_part = *str; //Holds the start character of the next part.

#if URL_AUTHORITY //If the framework is configured to parse authorities.
	//AUTHORITY PART
	//If the next part is an alphanumeric character.
	if((next_part >= '0' && next_part <= '9') || (next_part >= 'a' && next_part <= 'z'))
	{
		start = str; //Move the start pointer.

		// Advance to the next separator.
		while(*str != '/' && *str != ':' && *str != '#' && *str != '?' && *str != ' ')
		{
			if(*str++ == '\0') // If we have reached a null character.
			{
				return INVALID; // The URL is invalid.
			}
		}

		// Allocate space to hold the authority.
		authority = (char*)malloc(str - start + 1);

		if(!authority) // If memory allocation failed.
		{
			return OUT_OF_MEMORY;
		}

		// Copy the protocol string.
		memcpy(authority, start, str - start);

		authority[str - start] = '\0'; // Terminate the string.

		next_part = *str; //Save the beginning character of the next part.

#if URL_PORT //If the framework is configured to parse the port.
		//PORT PART

		if(next_part == ':') //If the next part starts with a : then there is a port.
		{
			start = str; //Save the next part.

			//Go to the beginning of the next part.
			while(*str != '/' && *str != ':' && *str != '#' && *str != '?' && *str != ' ')
			{
				if(*str++ == '\0') // If we have reached a null character.
				{
					return INVALID; // The URL is invalid.
				}
			}

			// Allocate space to hold the port.
			port = (char*)malloc(str - start + 1);

			if(!port) // If memory allocation failed.
			{
				return OUT_OF_MEMORY;
			}

			// Copy the port string.
			memcpy(port, start, str - start);

			port[str - start] = '\0'; // Terminate the string.

			next_part = *str; //Save the beginning character of the next part.
		}

#endif
	}
#endif

	//RESOURCE PART
	/*If the next part is not a beginning character for resource, then there
	 * are no resources.*/
	if(next_part != '?' && next_part != '#' && next_part != ' ')
	{
		start = str; //Save the start of the resource part.

		while(true) //Loop to decompose the resources part.
		{
			if(*str == '/') //If we have found the end of a resource.
			{
				if(!append_resource(start, str - start)) // If memory allocation failed.
				{
					return OUT_OF_MEMORY;
				}

				start = ++str; //Move the start pointer after that resource.

				continue; //Keep looking for other resources.
			}
			//If we have reached the end of the resource part.
			else if(*str == '?' || *str == '#' || *str == ' ' || *str == '\0')
			{
				next_part = *str; //Save the beginning character.
				//If the previous resource did not finish with a "/".
				if( *(str - 1) != '/' )
				{
					if(!append_resource(start, str - start)) // If memory allocation failed.
					{
						return OUT_OF_MEMORY;
					}
				}

				if(*str == '\0') // If we have reached a null character.
				{
					return VALID; // The URL ends here.
				}

				/*If the resource did finish with a "/", it has been accounted
				 * for previously. */

				break; //Done parsing resources.
			}

			str++;
		}
	}

	/* Note, there is no conditional compilation here, even if the framework
	 * does not support arguments or fragments, their presence does not render
	 * the url invalid, they are simply ignored.*/

#if URL_ARGUMENTS //If the framework is configured to parse arguments.

	//ARGUMENT PART

	//If the next part starts with a ? then there are arguments.
	if(next_part == '?')
	{
		start = ++str; //Save the start of the arguments.
		bool is_key = true; //True if we are parsing a key.
		const char* key; //Pointer to the key.

		//Initialize the arguments dictionary.
		arguments = new Dictionary< const char* >();

		if(!arguments) // If no memory could be allocated.
		{
			return OUT_OF_MEMORY;
		}

		while(true)
		{
			if(is_key) //If we are parsing for a key.
			{
				if( *str == '=' ) //If we have reached the key=value delimiter.
				{
					is_key = false; //We will be parsing for a value.
					key = start; //Save the key.
					start = ++str; //Move the start pointer.
					continue; //Keep going.
				}
				// If the end of the url or the fragment has been reached.
				else if(*str == '\0' || *str == '#')
				{
					return INVALID; // A key is incomplete.
				}
			}
			else //We are parsing a value.
			{
				//If we have reached the end of an argument or the url.
				if(*str == '&' || *str == '#' || *str == ' ' || *str == '\0')
				{
					is_key = true; //We will be parsing for a key.

					// Allocate space to hold the key.
					char* k = (char*)malloc(start - key);

					// Allocate space to hold the value.
					char* v = (char*)malloc(str - start + 1);

					if(!k || !v) // If memory allocation failed.
					{
						if(v) // If a value was allocated.
						{
							free(v);
						}
						else if(k) // If a key was allocated.
						{
							free(k);
						}

						return OUT_OF_MEMORY;
					}

					// Copy the key string.
					memcpy(k, key, start - key - 1);

					// Copy the value string.
					memcpy(v, start, str - start);

					k[start - key - 1] = '\0'; // Terminate the string.
					v[str - start] = '\0'; // Terminate the string.

					arguments->add(k, v); //Save the key=value pair.

					//If we have reached the end of the arguments part.
					if(*str == '#' || *str == ' ' || *str == '\0')
					{
						next_part = *str; //Save the beginning character.
						break; //Done parsing arguments.
					}

					start = ++str; //Move the start pointer.

					continue; //Keep going.
				}
			}
			str++;
		}
	}
	// If the next part is not a fragment or the end.
	else if((next_part != ' ' && next_part != '#') || next_part == '\0')
	{
		return VALID; // What comes next is not part of the URL.
	}

#endif

#if URL_FRAGMENT //If the framework is configured to parse the fragment.

	//FRAGMENT PART
	if(next_part == '#') //If there is a fragment part.
	{
		start = ++str; //Save the start of the fragment.

		while(true)
		{
			// If a space or the end of the string has been reached.
			if(*str == ' ' || *str == '\0')
			{
				break; // The end of the fragment has been found.
			}
			str++;
		}

		// Allocate space to hold the fragment.
		fragment = (char*)malloc(str - start + 1);

		if(!fragment) // If memory allocation failed.
		{
			return OUT_OF_MEMORY;
		}

		// Copy the fragment string.
		memcpy(fragment, start, str - start);

		fragment[str - start] = '\0'; // Terminate the string.
	}

#endif

	if(!resources.get_item_count() // If there are no resources.
#if URL_AUTHORITY
		&& !authority // If there was no authority.
#endif
#if URL_ARGUMENTS
		&& (arguments && !arguments->get_item_count()) // If there are no arguments.
#endif
#if URL_FRAGMENT
		&& !fragment // If there is no fragment.
#endif
	)
	{
		// Nothing useful could be parsed.

		return INVALID; // Invalid URL.
	}

	return VALID; // URL is valid.
}

#if URL_SERIALIZATION
size_t URL::serialize(char* buffer, bool write) const
{
	char* start = buffer; // The start of the buffer.

#if URL_PROTOCOL
	if( protocol ) // If there is a protocol part to the url.
	{
		 // Write the protocol to the buffer.
		if(write) { strcpy(buffer, protocol); }

		 // Increment the buffer pointer past the written protocol.
		buffer += strlen(protocol);

		if(write) // Adds the "://".
		{
			*buffer = ':'; *( buffer + 1 ) = '/'; *(buffer + 2) = '/';
		}

		buffer += 3; // Increments the buffer past the "://"
	}
#endif

#if URL_AUTHORITY
	if( authority )
	{
		if( write ){ strcpy(buffer, authority); }
		buffer += strlen(authority);
#endif
#if URL_PORT && URL_AUTHORITY
		if(port) // If the url has a port.
		{
			if( write ){ *buffer = ':'; } // Adds the colon.
			buffer++; // Increments the buffer past the colon.
			if( write ){ strcpy(buffer, port); } // Write the port to the buffer.
			buffer += strlen(port); // Increment the buffer past the port.
		}
	}
#endif

	if(is_absolute()) // If the url is absolute.
	{
		if( write ){ *buffer = '/'; } // Adds the first slash.
		buffer++; // Increments the buffer past the slash.
	}

	// For each resource in the hierarchy.
	for(uint8_t i = is_absolute() ? 1: 0; i < resources.get_item_count(); i++)
	{
		// Adds the resource to the url.
		if( write ){ strcpy(buffer, resources[i]); }
		buffer += strlen(resources[i]); // Increments the pointer past the resource.
		 // Add a slash to signal the end of the resource.
		if( write ){ *buffer = '/'; }
		buffer++; // Increments the buffer pas the slash.
	}

#if URL_ARGUMENTS
	if (arguments) // If the url has arguments.
	{
		const key_value_pair<const char*>* kv; // Holds the current key value pair.

		if( write ){ *buffer = '?'; } // Adds the arguments marker.
		buffer++; // Increments the buffer pas the arguments marker.

		// For each argument.
		for(uint8_t i = 0; i< arguments->get_item_count(); i++)
		{
			kv = (*arguments)[i]; // Gets the current argument.

			// Writes the name of the argument to the buffer.
			if( write ){ strcpy(buffer, kv->key); }
			buffer += strlen(kv->key); // Increments the buffer past the name.

			if( write ) {*buffer = '='; } // Adds the equal sign.
			buffer++; // Increments the buffer pas the equal sign.

			// Writes the value of the argument to the buffer.
			if( write ){ strcpy(buffer, kv->value); }
			buffer += strlen(kv->value); // Increments the buffer past the argument.

			if( write ){ *buffer = '&'; } // Writes the marker for a new argument.
			buffer++; // Increments the buffer past the marker.
		}

		if(arguments->get_item_count()) // If there were arguments.
		{
			buffer--; // An extra "&" was added by the previous loop.
		}
	}
#endif

#if URL_FRAGMENT
	if(fragment) // If the url has a fragment.
	{
		if( write ){ *buffer = '#'; } // Adds a fragment marker.
		buffer++; // Increments the buffer past the fragment marker.
		if( write ){ strcpy(buffer, fragment); } // Adds the fragment.
		buffer += strlen(fragment); // Increments the buffer pas the fragment.
	}
#endif

	 // The difference between the two pointers is the number of bytes written.
	return buffer - start;
}
#endif

void URL::print(void) const
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
	for(uint8_t i = 0; i < resources.get_item_count(); i++)
	{
		DEBUG_PRINT(resources[i]);
		DEBUG_PRINT('/');
	}
#if URL_ARGUMENTS
	if(arguments)
	{
		DEBUG_PRINT('?');
		for(uint8_t i = 0; i < arguments->get_item_count(); i++)
		{
			DEBUG_PRINT((*arguments)[i]->key)
			DEBUG_PRINT('=');
			DEBUG_PRINT((*arguments)[i]->value)
			if(i != arguments->get_item_count() - 1)
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

bool URL::is_absolute(void) const
{
	 // If the first resource is a slash.
	if((resources.get_item_count() && *(resources[0]) == '\0')
#if URL_AUTHORITY
		|| authority
#endif
	)
	{
		return true;
	}
	return false;
}
