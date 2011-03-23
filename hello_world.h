/*
 * blink.h
 *
 *  Created on: 2010-11-05
 *      Author: antoine
 */

#ifndef HELLO_WORLD_H_
#define HELLO_WORLD_H_

#include <core/resource.h>

class HelloWorld: public Resource
{
	char* message;
	bool state;

	public:
		HelloWorld();
		~HelloWorld();
		virtual void run(void);
		virtual File* render( Request* request );
		virtual Response::status_code process( Request* request, Message** return_message );

};

#endif /* HELLO_WORLD_H_ */
