/* fat_file.cpp - Implements a file to interface with FAT file systems
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

#include "fat_file.h"
#include "../avr_pal.h"

FATFile::FATFile(char* name):
	name(name)
{
	File::size = 0;
	File::_cursor = 0;
	last_op_result = f_open(&file, name, FA_READ | FA_OPEN_EXISTING);
	size = file.fsize;
}
FATFile::~FATFile()
{
	f_close(&file);
	ts_free(name);
}

size_t FATFile::read(char* buffer, size_t length)
{
	uint16_t bytes_read;
	last_op_result = f_read(&file, buffer, length, &bytes_read);
	_cursor += bytes_read;
	return bytes_read;
}
#if !READ_ONLY
uint16_t FATFile::write(const char* buffer, size_t length)
{
	return 0;
}
#endif
