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
	File<uint16_t>::size = 0;
	File<uint16_t>::cursor = 0;
	open();
}
FATFile::~FATFile()
{
	close();
	ts_free(name);
}

uint16_t FATFile::read(char* buffer, uint16_t length, bool async)
{
	uint16_t bytes_read;
	last_op_result = f_read(&file, buffer, length, &bytes_read);
	cursor += bytes_read;
	return bytes_read;
}
uint16_t FATFile::write(const char* buffer, uint16_t length,  bool async)
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
