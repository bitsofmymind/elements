/*
 * eeprom_file.h
 *
 *  Created on: 2011-04-20
 *      Author: antoine
 */

#ifndef EEPROM_FILE_H_
#define EEPROM_FILE_H_

#include <utils/file.h>
#include "eeprom_24LCxx.h"

class EEPROMFile: public File
{
	protected:
		EEPROM_24LCXX* _eeprom;
		uint16_t _addr;
	public:
		EEPROMFile(EEPROM_24LCXX* eeprom, uint16_t addr, uint16_t length);

		virtual size_t read(char* buffer, size_t length);
		virtual size_t write(const char* buffer, size_t length);

		//implemente cursor
};

#endif /* EEPROM_FILE_H_ */
