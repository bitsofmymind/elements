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
	size_t prev_cursor = cursor;
	char c;
	while(read(&c, 1))
	{
		DEBUG_PRINT_BYTE(c);
	}
	cursor = prev_cursor;
}
#endif


size_t File::extract(char* buffer)
{
	cursor = 0;
	return read(buffer, size);
}

ConstFile::ConstFile(const char* data):
	data(data)
{
	File::size = strlen(data);
	File::cursor = 0;
}

ConstFile::ConstFile(const char* data, size_t length):
	data(data)
{
	File::size = length;
	File::cursor = 0;
}


size_t ConstFile::read(char* buffer, size_t length)
{
	size_t i = 0;
	for(; i < length && File::cursor < File::size; File::cursor++, i++)
	{
		buffer[i] = data[File::cursor];
	}
	return i;
}
 size_t ConstFile::write(const char* buffer, size_t length) { return 0; }
 int8_t ConstFile::open(void){ return 0; }
 void ConstFile::close(void){}


MemFile::MemFile(char* data):
	data(data)
{
	File::size = strlen(data);
	File::cursor = 0;
}


MemFile::MemFile(char* data, size_t length):
	data(data)
{
	File::size = length;
	File::cursor = 0;
}

MemFile::~MemFile(void)
{
	ts_free(data);
}


size_t MemFile::read(char* buffer, size_t length)
{
	size_t i = 0;
	for(; i < length && File::cursor < File::size; File::cursor++, i++)
	{
		buffer[i] = data[File::cursor];
	}
	return i;
}
 size_t MemFile::write(const char* buffer, size_t length)
{
	size_t i = 0;
	for(; i < length && File::cursor < File::size; File::cursor++, i++)
	{
		data[File::cursor] = buffer[i];
	}
	return i;
}
 int8_t MemFile::open(void){ return 0; }
 void MemFile::close(void){}

