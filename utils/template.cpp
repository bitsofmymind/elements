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
#include <configuration.h>
#if ITOA
#include <stdlib.h>
#else
#include <cstdio>
#endif

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

void Template::add_narg(uint8_t arg)
{
	char* buf = (char*)ts_malloc(4);
	if(buf)
	{
#if ITOA
		itoa(arg, buf, 10);
#else
		sprintf(buf, "%d", arg);
#endif
	}
	add_arg(buf);
}
void Template::add_narg(uint16_t arg)
{
	char* buf = (char*)ts_malloc(6);
	if(buf)
	{
#if ITOA
		itoa(arg, buf, 10);
#else
		sprintf(buf, "%d", arg);
#endif
	}
	add_arg(buf);
}

void Template::add_narg(uint32_t arg)
{
	char* buf = (char*)ts_malloc(11);
	if(buf)
	{
#if ITOA
		itoa(arg, buf, 10);
#else
		sprintf(buf, "%d", arg);
#endif
	}
	add_arg(buf);
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

