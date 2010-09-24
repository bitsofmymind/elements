/*
 * buffer.h
 *
 *  Created on: Jun 30, 2009
 *      Author: Antoine
 */

#ifndef BUFFER_H_
#define BUFFER_H_

#include "types.h"
#include <stdint.h>

using namespace Elements;

struct Buffer
{
	uint64_t age;
	uint32_t size;
	uint8_t cursor;

	Buffer( uint32_t size );
	virtual ~Buffer();

	virtual string< uint32_t > get( uint32_t start, uint32_t stop ) = 0;
	virtual uint8_t set(string<uint32_t> data, uint32_t start ) = 0;
	virtual uint8_t modify_size( int32_t start_offset, int32_t stop_offset ) = 0;
};

struct LocalBuffer: public Buffer
{
	char* buffer;
	bool extracted;

	LocalBuffer( uint32_t size );
	virtual ~LocalBuffer();

	string< uint32_t > extract( void );
	virtual string< uint32_t > get( uint32_t start, uint32_t stop );
	virtual uint8_t set(string<uint32_t> data, uint32_t start );
	virtual uint8_t modify_size( int32_t start_offset, int32_t stop_offset );
};

#endif /* BUFFER_H_ */
