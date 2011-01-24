/*
 * file.cpp
 *
 *  Created on: 2011-01-24
 *      Author: antoine
 */

#include "file.h"

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
T ConstFile<T>::read(string<T>* buffer, bool async)
{
	T i = 0;
	for(; i < buffer->length && File<T>::cursor < File<T>::size; File<T>::cursor++, i++)
	{
		buffer->text[i] = data[File<T>::cursor];
	}
	return i;
}
template< class T> T ConstFile<T>::write(string<T>* buffer, bool async) { return 0; }
template< class T> int8_t ConstFile<T>::open(void){ return 0; }
template< class T> void ConstFile<T>::close(void){}

template class ConstFile<uint16_t>;
