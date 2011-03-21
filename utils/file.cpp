/*
 * file.cpp
 *
 *  Created on: 2011-01-24
 *      Author: antoine
 */

#include "file.h"
#include "../elements.h"

#ifdef DEBUG
template< class T >
void File<T>::print(void)
{
	T prev_cursor = cursor;
	char c;
	while(read(&c, 1, false))
	{
		Debug::print(c);
	}
	Debug::println();
	cursor = prev_cursor;
}
#endif

template< class T >
T File<T>::extract(char* buffer)
{
	cursor = 0;
	return read(buffer, size, false);
}

template< class T>
ConstFile<T>::ConstFile(const char* data):
	data(data)
{
	File<T>::size = strlen(data);
	File<T>::cursor = 0;
}

template< class T>
ConstFile<T>::ConstFile(const char* data, T length):
	data(data)
{
	File<T>::size = length;
	File<T>::cursor = 0;
}

template< class T>
T ConstFile<T>::read(char* buffer, T length, bool async)
{
	T i = 0;
	for(; i < length && File<T>::cursor < File<T>::size; File<T>::cursor++, i++)
	{
		buffer[i] = data[File<T>::cursor];
	}
	return i;
}
template< class T> T ConstFile<T>::write(const char* buffer, T length, bool async) { return 0; }
template< class T> int8_t ConstFile<T>::open(void){ return 0; }
template< class T> void ConstFile<T>::close(void){}

template< class T>
MemFile<T>::MemFile(char* data):
	data(data)
{
	File<T>::size = strlen(data);
	File<T>::cursor = 0;
}

template< class T>
MemFile<T>::MemFile(char* data, T length):
	data(data)
{
	File<T>::size = length;
	File<T>::cursor = 0;
}

template< class T>
MemFile<T>::~MemFile(void)
{
	ts_free(data);
}

template< class T>
T MemFile<T>::read(char* buffer, T length, bool async)
{
	T i = 0;
	for(; i < length && File<T>::cursor < File<T>::size; File<T>::cursor++, i++)
	{
		buffer[i] = data[File<T>::cursor];
	}
	return i;
}
template< class T> T MemFile<T>::write(const char* buffer, T length, bool async)
{
	T i = 0;
	for(; i < length && File<T>::cursor < File<T>::size; File<T>::cursor++, i++)
	{
		data[File<T>::cursor] = buffer[i];
	}
	return i;
}
template< class T> int8_t MemFile<T>::open(void){ return 0; }
template< class T> void MemFile<T>::close(void){}

template class File<MESSAGE_SIZE>;
template class ConstFile<MESSAGE_SIZE>;
template class MemFile<MESSAGE_SIZE>;
