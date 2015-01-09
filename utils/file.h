
/* file.h - Implements an abstract file type
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

#ifndef FILE_H_
#define FILE_H_

#include <configuration.h>

/// File is a base class for data streams.
/**
 * This class declares basic methods for manipulating data streams.
 */
class File
{

	protected:

		/** A cursor that points to the location in the stream that
		 * is currently being read. */
		size_t _cursor;

	public:

		/** The total size of the data stream. */
		size_t size;

	public:

		/** Class destructor. */
		virtual ~File();

#if VERBOSITY
		/// Prints out the content of the file to the debug output.
		void print();
#endif

		/// Reads bytes into a buffer.
		/** @param buffer the buffer to transfer the read bytes into.
		 * @param length the number of bytes to read.
		 * @return the number of bytes that have been read. */
		virtual size_t read(char* buffer, size_t length) = 0;

		/// Extracts the whole file into a buffer.
		/** @param buffer the buffer to transfer the data to.
		 * @return the number of bytes that have been transfered. */
		size_t extract(char* buffer);

#if !READ_ONLY
		/// Writes bytes to the file.
		/** @param buffer the bytes to write from.
		 * @param length the number of bytes to write.
		 * @return the number of bytes that have been written. */
		virtual size_t write(const char* buffer, size_t length) = 0;
#endif

		/// Get the position of the cursor in the file.
		/** @return the position of the cursor. */
		inline size_t cursor(void) { return _cursor; }

		/// Moves the cursor in the file.
		/** @param val the new position of the cursor.  */
		virtual void cursor(size_t val);
};

#endif //FILE_H_
