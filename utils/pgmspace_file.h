/*
 * pgmspace_file.h
 *
 *  Created on: 2011-02-23
 *      Author: antoine
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

		virtual uint16_t read(char* buffer, uint16_t length, bool async);
		virtual uint16_t write(const char* buffer, uint16_t length,  bool async);
		virtual int8_t open(void);
		virtual void close(void);
};

#endif /* PGMSPACE_FILE_H_ */
