/*
 * serial.cpp
 *
 *  Created on: 2010-11-07
 *      Author: antoine
 */

#include "eserial.h"
#include "HardwareSerial.h"
#include <stdlib.h>

ESerial::ESerial(void): Resource()
{
	Serial.begin(9600);
	buffer.text = (char*)malloc(100);
	buffer.length = 0;
	wait_for_response = false;
}

void ESerial::run(void)
{

	while( Serial.available() && !wait_for_response )
	{
		buffer.text[buffer.length] = Serial.read();

		if(buffer.length > 1
				&& buffer.text[buffer.length - 1 ] == ';'
				&& buffer.text[buffer.length] == ';'  )
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
				buffer.text[buffer.length+1] = 0;
				Serial.println("Message Received");

				request = new Request();
				if(request == 0)
				{
					Serial.println("Request Failed");
				}
				Elements::string<uint16_t>* msg = (Elements::string<uint16_t>*)malloc(sizeof(Elements::string<uint32_t>));
				if(msg == 0)
				{
					Serial.println("Malloc Failed");
				}
				msg->text = buffer.text;
				msg->length = buffer.length;
				if(request->deserialize(*msg, msg->text) == -1)
				{
					Serial.println("Deserialization Failed");
				}

				Elements::string<uint8_t>* val = request->fields.find(Elements::string<uint8_t>::make("content-length"));
				Serial.println("Sending Message");
				if(!val)
				{

					wait_for_response = true;
					Serial.flush();
					buffer.length = 0;
					send(request);
					Serial.println("Sent Message");
					break;
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


	schedule(0);
}

Message* ESerial::process(Response* response)
{
	Serial.println("Returned response");
	delete response;
	buffer.text = (char*)malloc(100);
	wait_for_response = false;
	return NULL;
}
