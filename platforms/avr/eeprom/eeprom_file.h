/* eeprom_file.h - Header file for the EEPROMFile class.
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

/// A file for data stored on an EEPROM.
class EEPROMFile: public File
{
	protected:

		/// The EEPROM on which the file resides.
		EEPROM_24LCXX* _eeprom;

		/// The address where the file starts.
		uint16_t _addr;

	public:

		/** Class constructor.
		 * @param eeprom the eeprom on which the file resides.
		 * @param addr the address of the beginning of the file.
		 * @param length the length of the file in bytes.
		 * */
		EEPROMFile(EEPROM_24LCXX* eeprom, uint16_t addr, uint16_t length);

		/// Reads bytes into a buffer.
		/** @param buffer the buffer to transfer the read bytes into.
		 * @param length the number of bytes to read.
		 * @return the number of bytes that have been read. */
		virtual size_t read(char* buffer, size_t length);

#if !READ_ONLY
		/** Writes bytes to the file. For EEPROMFile, this function always
		 * return 0 because eeprom memory is read only.
		 * @param buffer the bytes to write from.
		 * @param length the number of bytes to write.
		 * @return the number of bytes that have been written. */
		virtual size_t write(const char* buffer, size_t length);
#endif

};

#endif /* EEPROM_FILE_H_ */
