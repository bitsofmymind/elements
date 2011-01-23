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

class FATFile: public File<uint16_t>
{
	char* name;
	FIL file;

	public:
		FRESULT last_op_result;

	FATFile(char* name);
	~FATFile();

	virtual uint16_t read(string<uint16_t>* buffer, bool async);
	virtual uint16_t write(string<uint16_t>* buffer, bool async);
	virtual int8_t open(void);
	virtual void close(void);
};

#endif /* FAT_FILE_H_ */
