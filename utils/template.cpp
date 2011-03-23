/*
 * template.cpp
 *
 *  Created on: 2011-03-09
 *      Author: antoine
 */

#include "template.h"
#include <pal/pal.h>

Template::Template(File* file, char* args, size_t arglen, uint8_t argc):
		args(args),
		argend(args + arglen),
		file(file),
		argindex(args),
		state(TEXT),
		previous_args_length(0),
		previous_read_length(0)
{
	File::cursor = 0;
	File::size = file->size + arglen - 2 * argc; /*argc is substracted two times to remove the ~
	and the null characters that separates the arguments.*/
}


Template::~Template()
{
	ts_free(args);
	delete file;
}

size_t Template::read(char* buffer, size_t length)
{
	if( state != ARG )
	{
		previous_read_length = file->read(buffer, length);
		if(state == DONE)
		{
			return previous_read_length;
		}
	}

	size_t i;

	for(i = 0; i < length && previous_read_length != 0; i++)
	{
		if(state == SKIP)
		{
			state = TEXT;
			i++;
		}

		if(state == TEXT)
		{
			if(buffer[i] == '\\')
			{
				state = SKIP;
			}
			else if(buffer[i] == '~')
			{
				state = ARG;
				file->cursor = File::cursor + i + 1 - previous_args_length;
				//previous_arg_length = 0;
				i--;
				continue;
			}
			previous_read_length--;
		}
		else //state == ARG
		{

			if(*argindex == '\0')
			{

				previous_args_length--;
				state = TEXT;
				argindex++;
				if(i < length)
				{
					previous_read_length = file->read(buffer + i, length - i);
					i--;
					if(argindex >= argend )
					{

						state = DONE;
						i += previous_read_length + 1;
						break;
					}
				}
				continue;
			}
			buffer[i] = *argindex++;
			previous_args_length++;
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

