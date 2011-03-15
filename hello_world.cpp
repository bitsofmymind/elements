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


File<MESSAGE_SIZE>* HelloWorld::render( Request* request )
{
	const char* r = \
			"<html>\
				<body>\
					<h2>Hello World resource</h2>\
					You can use the following form to change the message the resource is displaying in stdout and</br>\
					turn it on or off.</br>\
					<br/>\
					<form method=\"post\" accept-charset=\"us-ascii\">\
						Message: <input type=\"text\" id=\"msg\" name=\"msg\" value=\"~\"/><br/>\
						ON<input type=\"radio\" id =\"st_1\" name=\"st\" ~ value=\"1\"/><br/>\
						OFF<input type=\"radio\" id=\"st_0\" name=\"st\" ~ value=\"0\"/><br/>\
						<input type=\"submit\" value=\"Submit\" />\
					</form>\
				</body>\
			</html>";

	//const char* r = "<html><body> ~ blah ~~ ~ </body></html>";

	ConstFile<MESSAGE_SIZE>* f = new ConstFile<MESSAGE_SIZE>(r);

	MESSAGE_SIZE data_len = strlen(message) + 1 + strlen("checked=\"checked\"") + 1 + 1;
	char* data = (char*)ts_malloc(data_len);
	char* ptr = data + strlen(message) + 1;
	memcpy(data, message, strlen(message) + 1 );
	if(state)
	{
		memcpy(ptr, "checked=\"checked\"", strlen("checked=\"checked\"") + 1);
		ptr += strlen("checked=\"checked\"") + 1;
		*ptr = '\0';
	}
	else
	{
		*ptr++ = '\0';;
		memcpy(ptr, "checked=\"checked\"", strlen("checked=\"checked\"") + 1);
	}

	return new Template<MESSAGE_SIZE>(f, data, data_len, 3);
}

Response::status_code HelloWorld::process( Request* request, Message** return_message )
{
	Response::status_code sc = Resource::process(request, return_message);
	if(sc == NOT_IMPLEMENTED_501)
	{
		if(request->methodcmp("post", 4))
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

			*return_message = http_get( request );
		}
	}

	return sc;
}


