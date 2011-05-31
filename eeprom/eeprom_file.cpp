/* eeprom_file.cpp - Implements a file for interfacing with a 24_LCxx eeprom
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
	size_t i;
	uint8_t bytes_read;

	if(length > size - _cursor)
	{
		length = size - _cursor;
	}

	bytes_read = length % PAGE_SIZE;
	if(bytes_read == 0)
	{
		bytes_read = PAGE_SIZE;
	}
	for( i = 0; i < length && _cursor + i < size; bytes_read = PAGE_SIZE)
	{
		_eeprom->read(_addr + _cursor + i, bytes_read);
		memcpy(buffer, _eeprom->page_buffer, bytes_read);
		buffer += bytes_read;
		i += bytes_read;
	}

	_cursor += i;

	return i;
}
#if !READ_ONLY
size_t EEPROMFile::write(const char* buffer, size_t length)
{
	return 0;
}
#endif
