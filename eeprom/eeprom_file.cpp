/*
 * eeprom_file.cpp
 *
 *  Created on: 2011-04-20
 *      Author: antoine
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
size_t EEPROMFile::write(const char* buffer, size_t length)
{
	return 0;
}
