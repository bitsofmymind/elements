/* pgmspace_file.cpp - Implements a file to interface with data in program space
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

#include "pgmspace_file.h"

PGMSpaceFile::PGMSpaceFile(PGM_P text, size_t length):
	File(length),
	_text(text)
{
}

size_t PGMSpaceFile::read(char* buffer, size_t length)
{
	size_t i = 0; // Number of bytes read.

	// For each by we want to read until the end of the file.
	for(; i < length && get_cursor() < get_size(); increment_cursor(1), i++)
	{
		// Read a byte from the file and write it to the buffer.
		buffer[i] = pgm_read_byte(_text + get_cursor());
	}

	return i;
}
#if !READ_ONLY
uint16_t PGMSpaceFile::write(const char* buffer, size_t length)
{
	return 0; // Cannot write to program space.
}
#endif
