/* template.h - Implements a simple template file type
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

#ifndef TEMPLATE_H_
#define TEMPLATE_H_

#include "file.h"
#include "utils.h"

class Template: public File
{
	protected:
		File* file;
		List<char*> args;
		uint8_t arg_index;
		char* current;
		enum STATE {ARG, TEXT, SKIP} state;

	public:
		Template(File* file);
		virtual ~Template();

	public:
		virtual size_t read(char* buffer, size_t length);
#if !READ_ONLY
		virtual size_t write(const char* buffer, size_t length);
#endif
		virtual void cursor(size_t val);
		void add_arg(char* arg);
		void add_narg(uint8_t arg);
		void add_narg(uint16_t arg);
		void add_narg(uint32_t arg);

};

#endif /* TEMPLATE_H_ */
