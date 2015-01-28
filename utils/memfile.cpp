/* memfile.cpp - Implements a file whose data is stored in RAM.
 * Copyright (C) 2011 Antoine Mercier-Linteau
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "memfile.h"
#include <string.h>
#include <pal/pal.h>

MemFile::MemFile(char* data, bool is_const = false ):
	is_const(is_const),
	data(data)
{
	File::size = strlen(data) + 1; // Find the end of the buffer.
	File::_cursor = 0; ///todo move to the initialization list.
}

MemFile::MemFile(const char* const_data):
	is_const(false)
{
	File::size = strlen(const_data) + 1; // Find the end of the buffer.
	File::_cursor = 0; ///todo move to the initialization list.

	data = (char*)ts_malloc(size);
	if(!data) // If space for the data could not be allocated.
	{
		size = 0;
	}
	else
	{
		strcpy(data, const_data);
	}
}

// Note: is_const is not pre assigned to prevent a mixup with the other constructor.
MemFile::MemFile(char* data, size_t length, bool is_const):
	is_const(is_const),
	data(data)
{
	File::size = length; ///todo move to the initialization list.
	File::_cursor = 0; ///todo move to the initialization list.
}

MemFile::~MemFile(void)
{
	if(!is_const) // If the file is not read-only.
	{
		// This means its data was allocated somehow, free it.
		ts_free(data);
	}
}

size_t MemFile::read(char* buffer, size_t length)
{
	size_t i = 0; // Index for the destination buffer.

	/* For all the data in the buffer or until the number of bytes wanted
	 * have been fetched. */
	for(; i < length && File::_cursor < File::size; File::_cursor++, i++)
	{
		buffer[i] = data[File::_cursor]; // Transfer the byte.
	}

	return i; // The index is the number of bytes read.
}

#if !READ_ONLY
size_t MemFile::write(const char* buffer, size_t length)
{
	if(is_const) // If the buffer is read-only.
	{
		return 0; // It is not possible to write to it.
	}

	size_t i = 0; // Index for the destination buffer.

	/* For all the data in the buffer or until the number of bytes wanted
	 * have been written. */
	for(; i < length && File::_cursor < File::size; File::_cursor++, i++)
	{
		data[File::_cursor] = buffer[i]; // Write a byte to the buffer.
	}

	return i; // Index is the number of bytes written.
}
#endif

