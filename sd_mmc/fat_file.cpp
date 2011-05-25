/*
 * fat_file.cpp
 *
 *  Created on: 2010-12-19
 *      Author: antoine
 */

#include "fat_file.h"

FATFile::FATFile(char* name):
	name(name)
{
	File::size = 0;
	File::_cursor = 0;
	last_op_result = f_open(&file, name, FA_READ | FA_OPEN_EXISTING);
	size = file.fsize;
}
FATFile::~FATFile()
{
	f_close(&file);
	ts_free(name);
}

uint16_t FATFile::read(char* buffer, size_t length)
{
	uint16_t bytes_read;
	last_op_result = f_read(&file, buffer, length, &bytes_read);
	_cursor += bytes_read;
	return bytes_read;
}
#if !READ_ONLY
uint16_t FATFile::write(const char* buffer, size_t length)
{
	return 0;
}
#endif
