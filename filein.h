/*
 * filein.h
 *
 *  Created on: 2010-11-07
 *      Author: antoine
 */

#ifndef FILEIN_H_
#define FILEIN_H_

#include <core/resource.h>
#include <core/request.h>
#include <utils/types.h>
#include <stdio.h>

class Filein: public Resource
{
	Elements::string<uint32_t> buffer;
	FILE* file;

	bool body_started;
	Request* request;
	uint32_t content_length;

	public:
		Filein(void);

	protected:
		void run(void);
};


#endif /* FILEIN_H_ */
