/* template.cpp - Implements a simple template file type
 * Copyright (C) 2011 Antoine Mercier-Linteau
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


#include "template.h"
#include <pal/pal.h>
#include <string.h>
#if ITOA
#include <stdlib.h>
#else
#include <cstdio>
#endif

Template::Template(File* file):
		file(file),
		arg_index(0),
		current(NULL),
		state(TEXT)
{
	File::_cursor = 0; // Set the cursor to the beginning of the template.
	///todo move to initialization list.
	File::size = file->size; // Sets the size of the file.
}


Template::~Template()
{
	while(args.items) // For each argument in the list.
	{
		ts_free(args.remove(0)); // Free the argument.
	}

	delete file; // The file is no longer needed.
}

void Template::add_arg(char* arg)
{
	//todo do error checking.
	args.append(arg); // Adds the argument to the end of the list.

	// Adds the length of the argument to the length of the file.
	size += strlen(arg) - 1; //The ~ marker is removed
}

void Template::skip_argument(void)
{
	// Allocate space for an empty string.
	char* empty_argument = (char*)ts_malloc(1);

	if(empty_argument) // If allocation was a success.
	{
		*empty_argument = '\0'; // Set the empty string.
		add_arg(empty_argument);
	}
}

void Template::add_narg(uint8_t arg)
{
	///todo simplify this method using by making the size of the argument configurable.

	// Allocate a buffer to contain the string representation of the argument.
	char* string = (char*)ts_malloc(4);

	if(string) // If allocation was successful.
	{
#if ITOA
		itoa(arg, string, 10); // Convert the number to a string.
#else
		sprintf(string, "%d", arg); // Convert the number to a string.
#endif
	}
	// Else the string will be null.

	add_arg(string); // Add the argument as a string.
}
void Template::add_narg(uint16_t arg)
{
	///todo simplify this method using by making the size of the argument configurable.

	// Allocate a buffer to contain the string representation of the argument.
	char* string = (char*)ts_malloc(4);

	if(string) // If allocation was successful.
	{
#if ITOA
		itoa(arg, string, 10); // Convert the number to a string.
#else
		sprintf(string, "%d", arg); // Convert the number to a string.
#endif
	}
	// Else the string will be null.

	add_arg(string); // Add the argument as a string.
}

void Template::add_narg(uint32_t arg)
{
	///todo simplify this method using by making the size of the argument configurable.

	// Allocate a buffer to contain the string representation of the argument.
	char* string = (char*)ts_malloc(4);

	if(string) // If allocation was successful.
	{
#if ITOA
		itoa(arg, string, 10); // Convert the number to a string.
#else
		sprintf(string, "%d", arg); // Convert the number to a string.
#endif
	}
	// Else the string will be null.

	add_arg(string); // Add the argument as a string.
}

size_t Template::read(char* buffer, size_t length)
{
	/* This algorithm reads an arbitrary length of bytes from the buffer but
	 * will switch to displaying an argument when a marker is encountered.*/

	size_t i;

	// For each byte wanted out of the buffer.
	for(i = 0; i < length; i++, buffer++)
	{
		if(state == ARG) // If we are outputting an argument.
		{
			// If an argument is being read and its end has not been reached.
			if(args[arg_index] != NULL && *current != '\0')
			{
				 // Adds the content of the argument to the buffer.
				*buffer = *current++;
				continue; // Keep reading the argument.
			}

			arg_index++; // Go to the next argument.
			state = TEXT; // Done with reading an argument.
		}

		if(!file->read(buffer, 1)) // If there is still data in the buffer.
		{
			break; // Done reading.
		}

		if(state == TEXT) // If we are parsing the text of the template.
		{
			if(*buffer == '~') // If a marker has been found.
			{
				 // If there are still arguments left to replace.
				if(arg_index < args.items)
				{
					current = args[arg_index]; // Set the current argument.
					state = ARG; // Now reading an argument.
					i--; // Go back to erase the marker.
					buffer--; // Go back to erase the marker.
				}
			}
			else if(*buffer == '\\') // If an escape character has been found.
			{
				state = SKIP;
			}
		}
		else if(state == SKIP) // An escape character was found.
		{
			state = TEXT; // Go back to reading text.
		}

	}

	File::_cursor += i; // Increment the cursor with the number of bytes read.

	return i; // Return the number of bytes read.
}

void Template::cursor(size_t val)
{
	file->cursor(0); // Reset the cursor on the encapsulated file.
	_cursor = 0; // Reset the internal cursor.
	state = TEXT; // Reset the renderer's state.
	arg_index = 0; // Reset the argument index.

	char bit_bucket; // Just to hold read bytes.

	/* This loops will increment counters and arguments using the read function
	 * because arguments need to be accounted for. The reason it is done with a
	 * loop is because allocation of the number of bytes needed to be read should
	 * cannot be guaranteed. */
	 ///todo use a bigger bit bucket for faster setting of the cursor.*/
	for(; val > 0; val--)
	{
		read(&bit_bucket, 1);
	}
}

#if !READ_ONLY
size_t Template::write(const char* buffer, size_t length)
{
	return 0; // A template cannot be written to.
}
#endif

