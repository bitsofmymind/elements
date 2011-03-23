
#ifndef FILE_H_
#define FILE_H_

#include <stdint.h>
#include <pal/pal.h>
#include "../elements.h"

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

class File
{

	public:
		size_t size;
		size_t cursor;

		virtual ~File();

		#ifdef DEBUG
			void print();
		#endif
		virtual size_t read(char* buffer, size_t length) = 0;
		size_t extract(char* buffer);
		virtual size_t write(const char* buffer, size_t length) = 0;
		virtual int8_t open(void) = 0;
		virtual void close(void) = 0;
};

class ConstFile: public File
{

	protected:
		const char* data;
	public:
		ConstFile(const char* data);
		ConstFile(const char* data, size_t length);

		virtual size_t read(char* buffer, size_t length);
		virtual size_t write(const char* buffer, size_t length);
		virtual int8_t open(void);
		virtual void close(void);
};

class MemFile: public File
{

	protected:
		char* data;
	public:
		MemFile(char* data);
		MemFile(char* data, size_t length);

		virtual ~MemFile();

		virtual size_t read(char* buffer, size_t length);
		virtual size_t write(const char* buffer, size_t length);
		virtual int8_t open(void);
		virtual void close(void);
};


#endif //FILE_H_
