/* memfile.h - Implements a simple RAM file type
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

class MemFile: public File
{

	protected:
		bool is_const;

	public:
		char* data;
	public:
		MemFile(char* data, bool is_const);
		MemFile(char* data, size_t length, bool is_const);

		virtual ~MemFile();

		virtual size_t read(char* buffer, size_t length);
#if !READ_ONLY
		virtual size_t write(const char* buffer, size_t length);
#endif
};

#endif //MEMFILE_H_
