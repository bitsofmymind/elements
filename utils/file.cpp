/* file.cpp - Implements an abstract file type
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

#include "file.h"
#include <pal/pal.h>

File::~File(){ }

#if VERBOSITY

void File::print(void)
{
	size_t prev_cursor = _cursor;
	char c;
	while(read(&c, 1))
	{
		DEBUG_TPRINT(c, BYTE);
	}
	_cursor = prev_cursor;
}
#endif


size_t File::extract(char* buffer)
{
	_cursor = 0;
	return read(buffer, size);
}

void File::cursor(size_t val)
{
	_cursor = val;
}
