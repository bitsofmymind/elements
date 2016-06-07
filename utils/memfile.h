/* memfile.h - Header file for the MemFile class.
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

#ifndef MEMFILE_H_
#define MEMFILE_H_

#include "file.h"

/// A file whose data is stored in RAM.
class MemFile: public File
{

	protected:
		/** If the file is read-only.*/
		bool is_const;
		///todo rename to read-only.

	public:

		/** The buffer that holds the file's data.*/
		char* data;
		///todo make this attribute protected.

	public:

		/// Class constructor.
		/** Use this constructor when the buffer is terminated by a null
		 * character.
		 * @param data the data that will be contained by the file.
		 * @param is_const if the data should be read only.*/
		MemFile(char* data, bool is_const);

		/// Class constructor.
		/**
		 * The contents of const_data will be copied to a buffer.
		 * @param data the data that will be contained by the file.
		 */
		MemFile(const char* const_data);

		/// Class constructor.
		/**
		 * The contents of const_data will be copied to a buffer.
		 * @param data the data that will be contained by the file.
		 * @param length the length of the data.
		 */
		MemFile(const char* const_data, size_t length);

		/// Class constructor.
		/** @param data the data that will be contained by the file.
		 * @param length the length of the data.
		 * @param is_const if the data should be read only.*/
		MemFile(char* data, size_t length, bool is_const);

		/// Class destructor.
		virtual ~MemFile();

		virtual size_t read(char* buffer, size_t length);

		virtual size_t write(const char* buffer, size_t length);
};

#endif //MEMFILE_H_
