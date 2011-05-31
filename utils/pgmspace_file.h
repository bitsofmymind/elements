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

class PGMSpaceFile: public File
{
	private:
		PGM_P text;

	public:

		PGMSpaceFile(PGM_P text, uint16_t size);

		virtual uint16_t read(char* buffer, uint16_t length);
		virtual uint16_t write(const char* buffer, uint16_t length);
};

#endif /* PGMSPACE_FILE_H_ */
