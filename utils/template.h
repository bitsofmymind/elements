/*
 * template.h
 *
 *  Created on: 2011-03-09
 *      Author: antoine
 */

#ifndef TEMPLATE_H_
#define TEMPLATE_H_

#include "file.h"

template<class T>class Template: public File<T>
{
	protected:
		File<T>* file;
		char* args;
		char* argend;
		char* argindex;
		T previous_args_length;
		T previous_read_length;
		enum STATE {ARG, TEXT, DONE, SKIP} state;

	public:
		Template(File<T>* file, char* args, MESSAGE_SIZE arglen, uint8_t argc);
		~Template();

	public:
		virtual T read(char* buffer, T length, bool async);
		virtual T write(const char* buffer, T length, bool async);
		virtual int8_t open(void);
		virtual void close(void);

};

#endif /* TEMPLATE_H_ */
