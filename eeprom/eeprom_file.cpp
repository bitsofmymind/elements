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
}

size_t EEPROMFile::read(char* buffer, size_t length)
{
	size_t i;
	size_t bytes_read = length % 64;
	for( i = 0; i < length && _cursor < size; bytes_read = 64 )
	{
		_eeprom->read(_addr, bytes_read);
		memcpy(buffer, _eeprom->page_buffer, bytes_read);
		i += bytes_read;
		_cursor += bytes_read;
		_addr += bytes_read;
		buffer += bytes_read;

	}

	return i;
}
size_t EEPROMFile::write(const char* buffer, size_t length)
{
	return 0;
}
