
/* file.h - Header file for the abstract File class.
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
	private:

		/** A cursor that points to the location in the stream that
		 * is currently being read. */
		size_t _cursor;

		/** The total size of the data stream. */
		size_t _size;

	public:

		/** Class constructor.
		 * @param size the initial size of the file. */
		inline File(size_t size) { _cursor = 0; _size = size; }

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

		/// Reads bytes into a buffer.
		/**
		 * Note: this overload has been created to satisfy SWIG's cstring.i.
		 * @param buffer the buffer to transfer the read bytes into.
		 * @param length the number of bytes to read. On return, this
		 * value will contain the actual number of bytes read.
		 * @return the number of bytes that have been read. */
		size_t read(char* buffer, size_t* length);

		/// Extracts the whole file into a buffer.
		/** @param buffer the buffer to transfer the data to.
		 * @return the number of bytes that have been transfered. */
		size_t extract(char* buffer);

		/// Writes bytes to the file.
		/** @param buffer the bytes to write from.
		 * @param length the number of bytes to write.
		 * @return the number of bytes that have been written. */
		virtual size_t write(const char* buffer, size_t length) = 0;

		/// Get the position of the cursor in the file.
		/** @return the position of the cursor. */
		inline size_t get_cursor(void) const { return _cursor; }

		/// Sets the cursor position in the file.
		/** @param val the new position of the cursor.  */
		virtual void set_cursor(size_t val);

		/// Increment the cursor in the file.
		/** @param val the amount by which the cursor is incremented. */
		inline void increment_cursor(size_t offset) { _cursor += offset; }

		/// Increment the cursor in the file.
		/** @param val the amount by which the cursor is decremented. */
		inline void decrement_cursor(size_t offset) { _cursor -= offset; }

		/** @return the size of the file.*/
		inline size_t get_size(void) const { return _size; }

	protected:

		/** @param size the size of the file. */
		inline void set_size(size_t size) { _size = size; }
};

#endif //FILE_H_
