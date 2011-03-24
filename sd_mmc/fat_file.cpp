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
	File::cursor = 0;
	open();
}
FATFile::~FATFile()
{
	close();
	ts_free(name);
}

uint16_t FATFile::read(char* buffer, size_t length)
{
	uint16_t bytes_read;
	last_op_result = f_read(&file, buffer, length, &bytes_read);
	cursor += bytes_read;
	return bytes_read;
}
uint16_t FATFile::write(const char* buffer, size_t length)
{
	return 0;
}

int8_t FATFile::open(void)
{
	last_op_result = f_open(&file, name, FA_READ | FA_OPEN_EXISTING);
	size = file.fsize;
	return last_op_result;
}

void FATFile::close(void)
{
	f_close(&file);
}
