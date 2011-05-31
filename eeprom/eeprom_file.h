/* eeprom_file.h - Implements a file for interfacing with a 24_LCxx eeprom
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
#if !READ_ONLY
		virtual size_t write(const char* buffer, size_t length);
#endif

		//implemente cursor
};

#endif /* EEPROM_FILE_H_ */
