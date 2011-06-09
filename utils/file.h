
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

class File
{

	protected:
		size_t _cursor;
	public:
		size_t size;

		virtual ~File();

		#if VERBOSITY
			void print();
		#endif
		virtual size_t read(char* buffer, size_t length) = 0;
		size_t extract(char* buffer);
#if !READ_ONLY
		virtual size_t write(const char* buffer, size_t length) = 0;
#endif
		inline size_t cursor(void) { return _cursor; }
		virtual void cursor(size_t val);
};

#endif //FILE_H_
