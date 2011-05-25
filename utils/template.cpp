/*
 * template.cpp
 *
 *  Created on: 2011-03-09
 *      Author: antoine
 */

#include "template.h"
#include <pal/pal.h>
#include <string.h>

Template::Template(File* file):
		file(file),
		state(TEXT),
		arg_index(0)
{
	File::_cursor = 0;
	File::size = file->size;
}


Template::~Template()
{

	while(args.items)
	{
		ts_free(args.remove(0));
	}

	delete file;
}

void Template::add_arg(char* arg)
{
	args.append(arg);
	if(arg != NULL ){ size += strlen(arg); }
	size--; //The ~ marker is removed
}

size_t Template::read(char* buffer, size_t length)
{
	size_t i;

	for(i = 0; i < length; i++, buffer++)
	{

		if(state == ARG)
		{
			if(args[arg_index] != NULL && *current != '\0')
			{
				*buffer = *current++;
				continue;
			}
			arg_index++;
			state = TEXT;
		}

		if(!file->read(buffer, 1))
		{
			break;
		}

		if(state == TEXT)
		{
			if(*buffer == '~')
			{
				if(arg_index < args.items)
				{
					current = args[arg_index];
					state = ARG;
					i--;
					buffer--;
				}
			}
			else if(*buffer == '\\')
			{
				state = SKIP;
			}
		}
		else if(state == SKIP)
		{
			state = TEXT;
		}

	}

	File::_cursor += i;
	return i;
}

void Template::cursor(size_t val)
{
	file->cursor(0);
	_cursor = 0;
	state = TEXT;
	arg_index = 0;
	char bit_bucket;
	for(; val > 0; val--)
	{
		read(&bit_bucket, 1);
	}
}
#if !READ_ONLY
size_t Template::write(const char* buffer, size_t length) { return 0;}
#endif

