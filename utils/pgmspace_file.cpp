/*
 * pgmspace_file.cpp
 *
 *  Created on: 2011-02-23
 *      Author: antoine
 */

#include "pgmspace_file.h"

PGMSpaceFile::PGMSpaceFile(PGM_P text, uint16_t size):
	text(text)
{
	File::size = size;
	File::_cursor = 0;

}

uint16_t PGMSpaceFile::read(char* buffer, uint16_t length)
{
	uint16_t i = 0;
	for(; i < length && _cursor < size; _cursor++, i++)
	{
		buffer[i] = pgm_read_byte(text + _cursor);
	}
	return i;
}
uint16_t PGMSpaceFile::write(const char* buffer, uint16_t length)
{
	return 0;
}
