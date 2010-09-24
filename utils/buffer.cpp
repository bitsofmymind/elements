/*
 * buffer.cpp
 *
 *  Created on: Jun 30, 2009
 *      Author: Antoine
 */

#include <utils/buffer.h>
#include <stdlib.h>
#include <string.h>
#include <utils/types.h>

Buffer::Buffer( uint32_t size ):
	size( size )
{
	age = 0;
	cursor = 0;
}

Buffer::~Buffer(void)
{

}

LocalBuffer::LocalBuffer( uint32_t size ):
	Buffer( size ),
	extracted( false )
{
	buffer = ( char* )malloc( size );
}

LocalBuffer::~LocalBuffer()
{
	if( extracted )
	{
		free( buffer );
	}
}

string< uint32_t > LocalBuffer::get( uint32_t start, uint32_t stop )
{

	string< uint32_t > data = { buffer + start, stop - start };

	if( stop > size || start > size || stop <= start )
	{
		data.text = NULL;
		data.length = 0;
	}

	return data;

}
string< uint32_t > LocalBuffer::extract( void )
{
	string< uint32_t > data = { buffer, size };
	extracted = true;
	return data;
}

uint8_t LocalBuffer::set(string<uint32_t> data, uint32_t start )
{
	if( start + data.length > size )
	{
		return 1;
	}

	else if( data.text == buffer + start )
	{
		//The modified region was already part of the locally allocated memory so no copying necessary.
		return 0;
	}

	memcpy( buffer + start, data.text, data.length );

	return 0;
}

uint8_t LocalBuffer::modify_size( int32_t start_offset, int32_t stop_offset )
{
	void* new_buffer;

	if( start_offset > stop_offset )
	{
		return 2;
	}
	else if( start_offset == 0 )
	{
		new_buffer = realloc(buffer, size + stop_offset );
		if( new_buffer == NULL )
		{
			return 1;
		}
	}
	else if( start_offset != 0 || stop_offset != 0 )
	{
		new_buffer = malloc( size + stop_offset - start_offset );
		if( new_buffer == NULL )
		{
			return 1;
		}
		memcpy( buffer + start_offset, new_buffer, size );
		free( buffer );
	}
	buffer = (char*)new_buffer;
	size = size + stop_offset - start_offset;
	return 0;
}
