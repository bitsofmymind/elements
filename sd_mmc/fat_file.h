/*
 * fat_file.h
 *
 *  Created on: 2010-12-19
 *      Author: antoine
 */

#ifndef FAT_FILE_H_
#define FAT_FILE_H_

#include <utils/file.h>
#include "ff.h"

class FATFile: public File
{
	char* name;
	FIL file;

	public:
		FRESULT last_op_result;

	FATFile(char* name);
	~FATFile();

	virtual size_t read(char* buffer, size_t length);
#if !READ_ONLY
	virtual size_t write(const char* buffer, size_t length);
#endif
};

#endif /* FAT_FILE_H_ */
