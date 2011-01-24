
#ifndef FILE_H_
#define FILE_H_

#include <stdint.h>
#include "types.h"

using namespace Elements;

template<class T> class Device
{
	public:
		
/*when passing a string to a read operation on a device, the memory should
preallocated to the amount of bytes the caller is expecting. The read method will
return with the buffer filled up with no more than the specified amount of bytes and
with the length property set to the amount of bytes
*/
		virtual T read(string<T>* buffer, bool async) = 0;
		virtual T write(string<T>* buffer, bool async) = 0;
};

template<class T> class File: public Device<T>
{

	public:
		T size;
		T cursor;

		virtual T read(string<T>* buffer, bool async) = 0;
		virtual T write(string<T>* buffer, bool async) = 0;
		virtual int8_t open(void) = 0;
		virtual void close(void) = 0;
};

template<class T> class ConstFile: public File<T>
{

	protected:
		const char* data;
	public:
		ConstFile(const char* data);
		ConstFile(const char* data, T length);

		virtual T read(string<T>* buffer, bool async);
		virtual T write(string<T>* buffer, bool async);
		virtual int8_t open(void);
		virtual void close(void);
};

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

#endif //FILE_H_
