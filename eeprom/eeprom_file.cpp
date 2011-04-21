/*
 * eeprom_file.cpp
 *
 *  Created on: 2011-04-20
 *      Author: antoine
 */
#include "eeprom_file.h"
#include <string.h>

EEPROMFile::EEPROMFile(EEPROM_24LCXX* eeprom, uint16_t addr):
	File(),
	_eeprom(eeprom),
	_addr(addr)
{

}

size_t EEPROMFile::read(char* buffer, size_t length)
{
	size_t total = 0;
	while(true)
	{
		if(length < 64)
		{
			total += _eeprom->read(_addr, length);
			memcpy(_eeprom->page_buffer, buffer, length);
			break;
		}
		else
		{
			uint8_t read = _eeprom->read(_addr, 64);
			total += read;
			memcpy(_eeprom->page_buffer, buffer, length);
			if(read < 64)
			{
				break;
			}
			buffer += 64;
			length -= 64;
		}
	}

	return total;
}
size_t EEPROMFile::write(const char* buffer, size_t length)
{
	return 0;
}
