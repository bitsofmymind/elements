/*
 * template.cpp
 *
 *  Created on: 2011-03-09
 *      Author: antoine
 */

#include "template.h"

template< class T>
Template<T>::Template(File<T>* file, char* args, MESSAGE_SIZE arglen, uint8_t argc):
		args(args),
		argend(args + arglen),
		file(file),
		argindex(args),
		state(TEXT),
		previous_args_length(0),
		previous_read_length(0)
{
	File<T>::cursor = 0;
	File<T>::size = file->size + arglen - 2 * argc; /*argc is substracted two times to remove the ~
	and the null characters that separates the arguments.*/
}

template< class T>
Template<T>::~Template()
{
	ts_free(args);
	delete file;
}
template< class T>
T Template<T>::read(char* buffer, T length, bool async)
{
	if( state != ARG )
	{
		previous_read_length = file->read(buffer, length, true);
		if(state == DONE)
		{
			return previous_read_length;
		}
	}

	T i;

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
				file->cursor = File<T>::cursor + i + 1 - previous_args_length;
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
					previous_read_length = file->read(buffer + i, length - i, true);
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
	File<T>::cursor += i;
	return i;
}
template< class T>
T Template<T>::write(const char* buffer, T length, bool async)
{

}
template< class T>
int8_t Template<T>::open(void)
{
	return 0;
}
template< class T>
void Template<T>::close(void)
{

}

template class Template<MESSAGE_SIZE>;
