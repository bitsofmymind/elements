/*
 * filein.cpp
 *
 *  Created on: 2010-11-07
 *      Author: antoine
 */

#include "filein.h"
#include <cstdio>
#include <stdlib.h>
#include <iostream>

Filein::Filein(void): Resource()
{
	file = fopen("/home/antoine/projects/elements/elements_test/input", "r");
	if(file)
	{
		buffer.length = 0;
		fseek (file , 0 , SEEK_END);
		content_length = ftell(file);
		buffer.text = (char*)malloc(content_length);
		rewind(file);
	}
}

/*Message* Filein::process(Response* response)
{
	uint32_t response_size = response->get_message_length();
	char* resp = (char*)malloc( response_size + 1);
	resp[response_size] = '\0';
	response->serialize(resp);
	std::cout << resp << std::endl;

	return NULL;
}*/

void Filein::run(void)
{
	if(!file)
	{
		schedule(1000000000);
		return;
	}

	while( fread( buffer.text + buffer.length, 1, 1, file) )
	{

		if(buffer.length > 1
				&& buffer.text[buffer.length - 1 ] == ';'
				&& buffer.text[buffer.length] == 10  )
		{
			buffer.text[buffer.length - 1 ] = '\r';
			buffer.text[buffer.length] = '\n';
		}

		if(body_started)
		{

		}
		else if(buffer.length > 4)
		{
			if(buffer.text[buffer.length - 3] == '\r'
								&& buffer.text[buffer.length - 2] == '\n'
								&& buffer.text[buffer.length - 1] == '\r'
								&& buffer.text[buffer.length] == '\n')
			{
				request = new Request();
				Elements::string<uint32_t>* msg = (Elements::string<uint32_t>*)malloc(sizeof(Elements::string<uint32_t>));
				msg->text = buffer.text;
				msg->length = buffer.length;
				request->deserialize(*msg, msg->text);

				Elements::string<uint8_t>* val = request->fields.find(Elements::string<uint8_t>::make("content-length"));
				if(!val)
				{
					buffer.text = (char*)malloc(100);
					buffer.length = 0;
					send(request);
				}
				else
				{
					body_started = true;
					//content_length = atoi
				}

			}
		}
		buffer.length++;
	}

	schedule(100);
}
