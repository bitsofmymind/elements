/* memfile.cpp - Source file for the MemFile class.
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
	File(0),
	is_const(is_const),
	data(data)
{
	set_size(strlen(data) + 1); // Find the end of the buffer.
}

MemFile::MemFile(const char* const_data):
	File(0),
	is_const(false)
{
	set_size(strlen(const_data) + 1); // Find the end of the buffer.

	data = (char*)ts_malloc(get_size());
	if(!data) // If space for the data could not be allocated.
	{
		set_size(0);
	}
	else
	{
		strcpy(data, const_data);
	}
}

// Note: is_const is not pre assigned to prevent a mixup with the other constructor.
MemFile::MemFile(char* data, size_t length, bool is_const):
	File(length),
	is_const(is_const),
	data(data)
{
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
	// Check which is longest, the remaining data of the length requested.
	length = length < get_size() - get_cursor() ? length : get_size() - get_cursor();

	memcpy(buffer, data, length);

	increment_cursor(length);

	return length;
}

#if !READ_ONLY
size_t MemFile::write(const char* buffer, size_t length)
{
	if(is_const) // If the buffer is read-only.
	{
		return 0; // It is not possible to write to it.
	}

	// Check which is longest, the remaining data of the length requested.
	length = length < get_size() - get_cursor() ? length : get_size() - get_cursor();

	memcpy(data, buffer, length);

	increment_cursor(length);

	return length;
}
#endif

