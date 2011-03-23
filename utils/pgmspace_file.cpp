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
	File::cursor = 0;

}

uint16_t PGMSpaceFile::read(char* buffer, uint16_t length, bool async)
{
	uint16_t i = 0;
	for(; i < length && cursor < size; cursor++, i++)
	{
		buffer[i] = pgm_read_byte(text + cursor);
	}
	return i;
}
uint16_t PGMSpaceFile::write(const char* buffer, uint16_t length,  bool async)
{
	return 0;
}
int8_t PGMSpaceFile::open(void){ return 0; }
void PGMSpaceFile::close(void){}
