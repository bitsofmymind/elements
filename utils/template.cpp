/*
 * template.cpp
 *
 *  Created on: 2011-03-09
 *      Author: antoine
 */

#include "template.h"
#include <pal/pal.h>

Template::Template(File* file):
		file(file),
		state(TEXT)
{
	File::cursor = 0;
	File::size = file->size;
}


Template::~Template()
{

	while(args.items)
	{
		ts_free(args.remove(0));
		lens.remove(0);
	}

	delete file;
}

void Template::add_arg(char* arg, size_t len)
{
	args.append(arg);
	lens.append(arg + len );
	size += len;
	size--; //The ~ marker is removed
}

size_t Template::read(char* buffer, size_t length)
{
	size_t i;

	for(i = 0; i < length; i++, buffer++)
	{

		if(state == ARG)
		{
			if(current < lens[0])
			{
				*buffer = *current++;
				continue;
			}

			ts_free(args.remove(0));
			lens.remove(0);
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
				if(args.items != 0)
				{
					current = args[0];
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

	File::cursor += i;
	return i;
}

size_t Template::write(const char* buffer, size_t length)
{

}

int8_t Template::open(void)
{
	return 0;
}

void Template::close(void)
{

}

