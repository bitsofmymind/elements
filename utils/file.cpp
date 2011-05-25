/*
 * file.cpp
 *
 *  Created on: 2011-01-24
 *      Author: antoine
 */

#include "file.h"
#include <string.h>
#include <pal/pal.h>

File::~File(){ }

#if VERBOSITY

void File::print(void)
{
	size_t prev_cursor = _cursor;
	char c;
	while(read(&c, 1))
	{
		DEBUG_TPRINT(c, BYTE);
	}
	_cursor = prev_cursor;
}
#endif


size_t File::extract(char* buffer)
{
	_cursor = 0;
	return read(buffer, size);
}

size_t File::cursor(void)
{
	return _cursor;
}

void File::cursor(size_t val)
{
	_cursor = val;
}


MemFile::MemFile(char* data, bool is_const):
	data(data),
	is_const(is_const)
{
	File::size = strlen(data);
	File::_cursor = 0;

}
MemFile::MemFile(char* data, size_t length, bool is_const ):
	data(data),
	is_const(is_const)
{
	File::size = length;
	File::_cursor = 0;
}

MemFile::~MemFile(void)
{
	if(!is_const){ ts_free(data); }
}

size_t MemFile::read(char* buffer, size_t length)
{
	size_t i = 0;
	for(; i < length && File::_cursor < File::size; File::_cursor++, i++)
	{
		buffer[i] = data[File::_cursor];
	}
	return i;
}
#if !READ_ONLY
 size_t MemFile::write(const char* buffer, size_t length)
{
	if(is_const){ return 0; }

	size_t i = 0;
	for(; i < length && File::_cursor < File::size; File::_cursor++, i++)
	{
		data[File::_cursor] = buffer[i];
	}
	return i;
}
#endif

