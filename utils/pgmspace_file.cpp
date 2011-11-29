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

PGMSpaceFile::PGMSpaceFile(PGM_P text, size_t size):
	text(text)
{
	File::size = size;
	File::_cursor = 0;

}

size_t PGMSpaceFile::read(char* buffer, size_t length)
{
	uint16_t i = 0;
	for(; i < length && _cursor < size; _cursor++, i++)
	{
		buffer[i] = pgm_read_byte(text + _cursor);
	}
	return i;
}
#if !READ_ONLY
uint16_t PGMSpaceFile::write(const char* buffer, size_t length)
{
	return 0;
}
#endif
