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
	open();
}
FATFile::~FATFile()
{
	//close();
	ts_free(name);
}

uint16_t FATFile::read(string<uint16_t>* buffer, bool async)
{
	uint16_t bytes_read;
	last_op_result = f_read(&file, buffer->text, buffer->length, &bytes_read);
	return bytes_read;
}
uint16_t FATFile::write(string<uint16_t>* buffer, bool async)
{

}

int8_t FATFile::open(void)
{
	last_op_result = f_open(&file, name, FA_READ | FA_OPEN_EXISTING);
}

void FATFile::close(void)
{
	f_close(&file);
}
