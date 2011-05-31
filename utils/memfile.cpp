/* memfile.cpp - Implements a simple RAM file type
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

MemFile::MemFile(char* data, bool is_const):
	data(data),
	is_const(is_const)
{
	File::size = strlen(data);
	File::_cursor = 0;

}
MemFile::MemFile(char* data, size_t length, bool is_const ):
	data(data),
	is_const(is_const)
{
	File::size = length;
	File::_cursor = 0;
}

MemFile::~MemFile(void)
{
	if(!is_const){ ts_free(data); }
}

size_t MemFile::read(char* buffer, size_t length)
{
	size_t i = 0;
	for(; i < length && File::_cursor < File::size; File::_cursor++, i++)
	{
		buffer[i] = data[File::_cursor];
	}
	return i;
}
#if !READ_ONLY
 size_t MemFile::write(const char* buffer, size_t length)
{
	if(is_const){ return 0; }

	size_t i = 0;
	for(; i < length && File::_cursor < File::size; File::_cursor++, i++)
	{
		data[File::_cursor] = buffer[i];
	}
	return i;
}
#endif

