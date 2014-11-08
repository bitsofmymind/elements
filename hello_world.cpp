/*
 * blink.cpp
 *
 *  Created on: 2010-11-05
 *      Author: antoine
 */

#include "hello_world.h"
#include <pal/pal.h>
#include <stdlib.h>
#include <iostream>
#include <utils/template.h>
#include <utils/memfile.h>
#include <string.h>

HelloWorld::HelloWorld():
	Resource(),
	state(true)
{
	const char* m = "Default message";
	message = (char*)malloc(strlen(m) + 1);
	strcpy(message, m);

	schedule( 2000 );
}

HelloWorld::~HelloWorld()
{
	free(message);
}

void HelloWorld::run(void)
{
	if(state)
	{
		std::cout << message << std::endl;
	}
	schedule(2000);
}


File* HelloWorld::render( void )
{
	const char* r = \
			"<html>\n\
				<body>\n\
					<h2>Hello World resource</h2>\n\
					You can use the following form to change the message the resource is displaying in stdout and</br>\n\
					turn it on or off.</br>\n\
					<br/>\n\
					<form method=\"post\" accept-charset=\"us-ascii\">\n\
						Message: <input type=\"text\" id=\"msg\" name=\"msg\" value=\"~\"/><br/>\n\
						ON<input type=\"radio\" id =\"st_1\" name=\"st\" ~ value=\"1\"/><br/>\n\
						OFF<input type=\"radio\" id=\"st_0\" name=\"st\" ~ value=\"0\"/><br/>\n\
						<input type=\"submit\" value=\"Submit\" />\n\
					</form>\n\
				</body>\n\
			</html>";

	//const char* r = "<html><body> ~ blah ~~ ~ </body></html>";

	Template* t = new Template(new MemFile((char*)r, true));

	size_t len = strlen(message) + 1;
	char* arg = (char*)ts_malloc(len);
	memcpy(arg, message, len);
	t->add_arg(arg);

	if(!state)
	{
		t->add_arg(NULL);
	}
	len = strlen("checked=\"checked\"") + 1;
	arg = (char*)ts_malloc(len);
	memcpy(arg, "checked=\"checked\"", len);
	t->add_arg(arg);
	if(state)
	{
		t->add_arg(NULL);
	}

	return t;
}

Response::status_code HelloWorld::process( Request* request, Response* response )
{
	if(!request->to_destination())
	{
		if(request->is_method(Request::POST))
		{
			char buffer[50];
			uint8_t len = request->find_arg("msg", buffer, 50);

			if(len)
			{
				free(message);
				message = (char*)malloc(len + 1);
				memcpy(message, buffer, len + 1);
			}
			len = request->find_arg("st", buffer, 1);
			if(len)
			{
				if(buffer[0] == '1')
				{
					state = true;
				}
				else if(buffer[0] == '0')
				{
					state = false;
				}
			}
			goto get;
		}
		else if(request->is_method(Request::GET))
		{
			get:
			response->set_body(render(), MIME::TEXT_HTML);
			return OK_200;
		}
		return NOT_IMPLEMENTED_501;
	}
	return PASS_308;
}


