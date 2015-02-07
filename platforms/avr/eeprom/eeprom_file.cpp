/* eeprom_file.cpp - Source file for the EEPROMFile class.
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

#include "eeprom_file.h"
#include <string.h>

EEPROMFile::EEPROMFile(EEPROM_24LCXX* eeprom, uint16_t addr, uint16_t length):
	File(),
	_eeprom(eeprom),
	_addr(addr)
{
	File::size = length;
	File::_cursor = 0;
}

size_t EEPROMFile::read(char* buffer, size_t length)
{
	size_t i; // The total number of bytes read from the EEPROM.
	uint8_t bytes_read; // The number of bytes read from a block.

	 // If the length requested is more than what remains in the file.
	if(length > size - _cursor)
	{
		length = size - _cursor; // Set the length to what remains in the file.
	}

	/* Data on the EEPROM is read in blocks of size up to PAGE_SIZE. If the number
	 * of bytes requested is not a multiple of PAGE_SIZE, the size of the
	 * first block read is made smaller so that the remainder will become a
	 * multiple.*/

	bytes_read = length % PAGE_SIZE; // Check if length falls on a block boundary.
	if(bytes_read == 0)
	{
		bytes_read = PAGE_SIZE; // If it does read the maximum amount of bytes.
	}

	/* Read data block by block until the end of the file. The first amount of
	 * bytes to be read can be smaller than a block but the next amount will
	 * always be the size of a block.*/
	for(i = 0; i < length && _cursor + i < size; bytes_read = PAGE_SIZE)
	{
		// Read a block from the EEPROM into the page_buffer.
		_eeprom->read(_addr + _cursor + i, bytes_read);

		// Copy the content of the page buffer to the buffer.
		memcpy(buffer, _eeprom->page_buffer, bytes_read);

		buffer += bytes_read; // Move the address of the buffer.

		i += bytes_read; // A block has been read.
	}

	_cursor += i; // Advance the cursor.

	return i; // Return the number of bytes read.
}

#if !READ_ONLY
size_t EEPROMFile::write(const char* buffer, size_t length)
{
	return 0; // EEPROMs are read only.
}
#endif
