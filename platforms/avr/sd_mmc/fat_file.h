/* fat_file.h - Header file for the FATFile class.
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

#ifndef FAT_FILE_H_
#define FAT_FILE_H_

#include <utils/file.h>
#include "ff.h"

/// A File for interfacing with data stored on a FAT file system.
class FATFile: public File
{
	private:

		/// The path of the file.
		char* name;

		/// An internal object for interfacing with the file.
		FIL file;

	public:

		/// The result of the last operation.
		FRESULT last_op_result;

		/// Class constructor.
		/**
		 * @param name the path to the file.
		 * */
		FATFile(char* name);

		/// Class destructor
		~FATFile();

		/// Reads bytes into a buffer.
		/**
		 * @param buffer the buffer to transfer the read bytes into.
		 * @param length the number of bytes to read.
		 * @return the number of bytes that have been read.
		 * */
		virtual size_t read(char* buffer, size_t length);

#if !READ_ONLY
		/// WRite bytes to the file.
		/**
		 * @param buffer the bytes to write from.
		 * @param length the number of bytes to write.
		 * @return the number of bytes that have been written.
		 * */
	virtual size_t write(const char* buffer, size_t length);
#endif

};

#endif /* FAT_FILE_H_ */
