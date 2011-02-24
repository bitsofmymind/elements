/*
 * file.cpp
 *
 *  Created on: 2011-01-24
 *      Author: antoine
 */

#include "file.h"
#include "../elements.h"

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

template class File<MESSAGE_SIZE>;
template class ConstFile<MESSAGE_SIZE>;
