/* file.cpp - Source file for the abstract File class.
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

/*File::File(size_t size) :
	_size(size),
	_cursor(0)
{
}*/

File::~File(){ }

#if VERBOSITY
void File::print(void)
{
	// Save the current position of the cursor to restore it later.
	size_t prev_cursor = _cursor;

	char c; // Holds the character that has been read.
	while(read(&c, 1)) // While there is data in the file.
	{
		DEBUG_TPRINT((int8_t)c, BYTE); // Prints the data.
	}

	_cursor = prev_cursor; // Restore the previous position of the cursor.
}
#endif

size_t File::extract(char* buffer)
{
	_cursor = 0; // Moves the cursor to the beginning of the file.
	return read(buffer, _size); // Read the whole file.
}

size_t File::read(char* buffer, size_t* length)
{
	*length = read(buffer, *length);
	return *length;
}

void File::set_cursor(size_t val)
{
	/// TODO prevent the cursor from getting set past the size of the file.
	_cursor = val;
}
