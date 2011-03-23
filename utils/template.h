/*
 * template.h
 *
 *  Created on: 2011-03-09
 *      Author: antoine
 */

#ifndef TEMPLATE_H_
#define TEMPLATE_H_

#include "file.h"

class Template: public File
{
	protected:
		File* file;
		char* args;
		char* argend;
		char* argindex;
		size_t previous_args_length;
		size_t previous_read_length;
		enum STATE {ARG, TEXT, DONE, SKIP} state;

	public:
		Template(File* file, char* args, size_t arglen, uint8_t argc);
		~Template();

	public:
		virtual size_t read(char* buffer, size_t length, bool async);
		virtual size_t write(const char* buffer, size_t length, bool async);
		virtual int8_t open(void);
		virtual void close(void);

};

#endif /* TEMPLATE_H_ */
