/* template.h - Implements a simple templating file type.
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

/// Defines a simple templating file type.
/** Template encapsulates a File to allow the setting of template markers
 * at run time. A marker is a special character that will be replaced by
 * a value when the template is read back. Markers have to be added in the
 * order that they are present in the template.*/
class Template: public File
{
	protected:
		/** The encapsulated file. */
		File* file;

		/** The arguments that will substitute the markers. */
		List<char*> args;
		///todo change to a linked list.

		/** The index of argument that is currently being substituted.*/
		uint8_t arg_index;

		/** The content of the argument currently being substituted. */
		char* current;

		/** The internal state of the templated renderer.*/
		enum STATE {ARG, TEXT, SKIP} state;

	public:

		/// Class constructor.
		/** @param file the file this template encapsulates.*/
		Template(File* file);

		/// Class destructor.
		virtual ~Template();

		virtual size_t read(char* buffer, size_t length);

#if !READ_ONLY
		virtual size_t write(const char* buffer, size_t length);
#endif

		virtual void set_cursor(size_t val);

		/// Adds a string to the list of arguments.
		/** @param arg the argument.
		 * */
		void add_arg(char* arg);

		/**
		 * Skips the next argument.
		 * */
		void skip_argument();

		/// Adds a 1 byte integer to the list of arguments.
		/** @param arg the argument.
		 * */
		void add_narg(uint8_t arg);

		/// Adds a 2 byte integer to the list of arguments.
		/** @param arg the argument.
		 * */
		void add_narg(uint16_t arg);

		/// Adds a 4 byte integer to the list of arguments.
		/** @param arg the argument.
		 * */
		void add_narg(uint32_t arg);
};

#endif /* TEMPLATE_H_ */
