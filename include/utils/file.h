
#include <stdint.h>

#include "types.h"
#include "../core/resource.h"

using namespace Elements;

template<class T> class Device
{
	public:
		
/*when passing a string to a read operation on a device, the memory should
preallocated to the amount of bytes the caller is expecting. The read method will
return with the buffer filled up with no more than the specified amount of bytes and
with the length property set to the amount of bytes
*/
		virtual int8_t read(string<T>* buffer, bool async) = 0;
		virtual int8_t write(string<T>* buffer, bool async) = 0;
};

template<class T> class File: public Device<T>
{
	public:

		Resource* owner;

		File(Resource* owner):
			owner(owner)
		{
			
		}

		virtual int8_t read(string<T>* buffer, bool async) = 0;
		virtual int8_t write(string<T>* buffer, bool async) = 0;
		virtual int8_t open(Resource* owner);
		virtual int8_t close(void);
};

template<class T> class Buffer: public Device<T>
{
    public:

        bool free_if_consumed;
	T size;
	
	uint8_t* data; 
	
	virtual int8_t write(uint8_t byte);
	virtual int8_t read(uint8_t* dest);
};

template<class T> class IndexedBuffer: public Buffer<T>
{
	uint8_t* cursor;
};

template<class T> class CircularBuffer: public IndexedBuffer<T>
{
	uint8_t start;
};