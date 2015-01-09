/* pgmspace_file.h - Implements a file to interface with data in program space
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

#ifndef PGMSPACE_FILE_H_
#define PGMSPACE_FILE_H_

#include <utils/file.h>
#include <avr/pgmspace.h>
#include <configuration.h>

/// A file for strings stored in program memory.
/** Strings are by default
 * stored in RAM from an initialization list automatically compiled in by the
 * compiler and ran at boot. Storing strings in program memory saves on RAM and
 * code space as well.*/
class PGMSpaceFile: public File
{
	private:
		/// The text wrapped by this class.
		PGM_P text;

	public:

		/** Class constructor.
		 * @param text the string to wrap.
		 * @param size the size of the string.
		 * */
		PGMSpaceFile(PGM_P text, size_t size);

		/// Reads bytes into a buffer.
		/** @param buffer the buffer to transfer the read bytes into.
		 * @param length the number of bytes to read.
		 * @return the number of bytes that have been read. */
		virtual size_t read(char* buffer, size_t length);

#if !READ_ONLY
		/** Writes bytes to the file. For PGMSpaceFile, this function always
		 * return 0 because program memory is read only.
		 * @param buffer the bytes to write from.
		 * @param length the number of bytes to write.
		 * @return the number of bytes that have been written. */
		virtual uint16_t write(const char* buffer, uint16_t length);
#endif
};

#endif /* PGMSPACE_FILE_H_ */
