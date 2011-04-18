/*
 * template.h
 *
 *  Created on: 2011-03-09
 *      Author: antoine
 */

#ifndef TEMPLATE_H_
#define TEMPLATE_H_

#include "file.h"
#include "utils.h"

class Template: public File
{
	protected:
		File* file;
		List<char*> args;
		List<char*> lens;
		uint8_t arg_index;
		char* current;
		enum STATE {ARG, TEXT, SKIP} state;

	public:
		Template(File* file);
		~Template();

	public:
		virtual size_t read(char* buffer, size_t length);
		virtual size_t write(const char* buffer, size_t length);
		virtual void cursor(size_t val);
		void add_arg(char* arg, size_t arglen);

};

#endif /* TEMPLATE_H_ */
