/*
 * file.cpp
 *
 *  Created on: 2011-01-24
 *      Author: antoine
 */

#include "file.h"
#include <string.h>
#include <pal/pal.h>


File::~File(void)
{}

#if VERBOSITY

void File::print(void)
{
	size_t prev_cursor = _cursor;
	char c;
	while(read(&c, 1))
	{
		DEBUG_PRINT_BYTE(c);
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

ConstFile::ConstFile(const char* data):
	data(data)
{
	File::size = strlen(data);
	File::_cursor = 0;
}

ConstFile::ConstFile(const char* data, size_t length):
	data(data)
{
	File::size = length;
	File::_cursor = 0;
}


size_t ConstFile::read(char* buffer, size_t length)
{
	size_t i = 0;
	for(; i < length && File::_cursor < File::size; File::_cursor++, i++)
	{
		buffer[i] = data[File::_cursor];
	}
	return i;
}
 size_t ConstFile::write(const char* buffer, size_t length) { return 0; }


MemFile::MemFile(char* data):
	data(data)
{
	File::size = strlen(data);
	File::_cursor = 0;
}


MemFile::MemFile(char* data, size_t length):
	data(data)
{
	File::size = length;
	File::_cursor = 0;
}

MemFile::~MemFile(void)
{
	ts_free(data);
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
 size_t MemFile::write(const char* buffer, size_t length)
{
	size_t i = 0;
	for(; i < length && File::_cursor < File::size; File::_cursor++, i++)
	{
		data[File::_cursor] = buffer[i];
	}
	return i;
}

