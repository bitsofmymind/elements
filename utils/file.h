
#ifndef FILE_H_
#define FILE_H_

#include <stdint.h>
#include "types.h"

using namespace Elements;

/*template<class T> class Device
{
	public:
		
/*when passing a string to a read operation on a device, the memory should
preallocated to the amount of bytes the caller is expecting. The read method will
return with the buffer filled up with no more than the specified amount of bytes and
with the length property set to the amount of bytes
*/
		/*virtual T read(string<T>* buffer, bool async) = 0;
		virtual T write(string<T>* buffer, bool async) = 0;*/
//};*/

template<class T> class File//: public Device<T>
{

	public:
		T size;
		T cursor;

		#ifdef DEBUG
			void print();
		#endif
		virtual T read(char* buffer, T length, bool async) = 0;
		T extract(char* buffer);
		virtual T write(const char* buffer, T length, bool async) = 0;
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

		virtual T read(char* buffer, T length, bool async);
		virtual T write(const char* buffer, T length, bool async);
		virtual int8_t open(void);
		virtual void close(void);
};

template<class T> class MemFile: public File<T>
{

	protected:
		char* data;
	public:
		MemFile(char* data);
		MemFile(char* data, T length);

		~MemFile();

		virtual T read(char* buffer, T length, bool async);
		virtual T write(const char* buffer, T length, bool async);
		virtual int8_t open(void);
		virtual void close(void);
};


#endif //FILE_H_
