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
	File(0),
	name(name)
{
	// Open the file in read only mode and only if it exists.
	last_op_result = f_open(&file, name, FA_READ | FA_OPEN_EXISTING);

	set_size(file.fsize); // Set the file size.
}

FATFile::~FATFile()
{
	f_close(&file); // Close the open file.
	ts_free(name); // The path string to the file is no longer needed.
}

size_t FATFile::read(char* buffer, size_t length)
{
	uint16_t bytes_read; // A unint16_t as defined by integer.h

	/// TODO set the file read/write pointer along with the _cursor.

	// Read the file into the buffer.
	last_op_result = f_read(&file, buffer, length, &bytes_read);

	increment_cursor(bytes_read); // Increment the cursor.

	return bytes_read;

}
#if !READ_ONLY
uint16_t FATFile::write(const char* buffer, size_t length)
{
	return 0; // Writing to files not implemented yet.
}
#endif
