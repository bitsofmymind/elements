//============================================================================
// Name        : elements_test.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <pal/pal.h>
#include <core/resource.h>
#include <core/authority.h>
#include <core/processing.h>
#include <core/request.h>
#include <utils/utils.h>
#include <iostream>
#include <stdlib.h>
#include "string.h"
//#include "filein.h"
#include "hello_world.h"

void processing_wake(){}
void processing_sleep(uint64_t time){}
void Debug::print(char c){ std::cout << c; }
void Debug::println(){ std::cout << std::endl; }

uint32_t steps = 0;

class Echo: public Resource
{

	virtual Response::status_code process(Response* response, Message** reply_message)
	{
		std::cout << "Echo completed in " << steps << " steps." << std::endl;
		std::cout << "Response printout:" << std::endl;
		MESSAGE_SIZE len = response->get_header_length();
		char* str = (char*)malloc(len + 1);
		str[len] = '\0';
		response->serialize(str);
		std::cout << str << std::endl;
		free(str);
		char body_buffer[21];
		uint8_t read;
		if(response->body_file)
		{
			do
			{
				read = response->body_file->read(body_buffer, 20, false);
				body_buffer[read]  ='\0';
				std::cout << body_buffer;
			}while(read > 0);
		}
		std::cout << std::endl;

		Resource::process(response, reply_message);
		return OK_200;
	}

};

class Timer: public Resource
{
	uint32_t _interval;
	int _id;

	public:
	Timer(uint32_t interval, int id): Resource(), _interval(interval), _id(id)
	{
		schedule( _interval );
	}

	virtual void run()
	{
		//Elements::e_time_t time =;
		std::cout << "Timer " << _id << " tick" << std::endl;
		schedule( _interval );
	}
};

int main()
{
	Echo* echo = new Echo();
	Resource* res2 = new Resource();
	Authority* root = new Authority();
	Processing* proc = new Processing(NULL);
	Timer* timer1 = new Timer(500, 1);
	Timer* timer2 = new Timer(1000, 2);
	HelloWorld* hw = new HelloWorld();

	root->add_child("proc", proc);
	root->add_child("echo", echo);
	root->add_child("res2", res2);
	root->add_child("hw", hw);
	//root->add_child(Elements::string<uint8_t>::make("filein"), filein);
	echo->add_child("timer1", timer1);
	echo->add_child("timer2", timer2);

	Request* req = new Request();

	/* PARSING CASE 1:
	 * Message is fed as a null terminated string
	*/
	/*const char* msg = "GET / HTTP/1.1\r\nContent-Length: 4\r\n\r\n1234";
	if(((Message*)req)->parse(msg) != Message::PARSING_COMPLETE)
	{
		std::cout << "Message parsing error" << std::endl;
		delete req;
		return 1;
	}*/


	/* PARSING CASE 2:
	 * Message is fed as \r\n terminated parts
	 */
	/*string<MESSAGE_SIZE> part1 = MAKE_STRING("GET / HTTP/1.1\r\n");
	string<MESSAGE_SIZE> part2 = MAKE_STRING("Content-Length: 4\r\nField: data\r\n");
	string<MESSAGE_SIZE> part3 = MAKE_STRING("\r\n");
	string<MESSAGE_SIZE> part4 = MAKE_STRING("1234");

	if(req->parse(part1.text, part1.length) != Message::PARSING_SUCESSFUL)
	{
		std::cout << "Message parsing error" << std::endl;
		delete req;
		return 1;
	}
	if(req->parse(part2.text, part2.length) != Message::PARSING_SUCESSFUL)
	{
		std::cout << "Message parsing error" << std::endl;
		delete req;
		return 1;
	}
	if(req->parse(part3.text, part3.length) != Message::PARSING_COMPLETE)
	{
		std::cout << "Message parsing error" << std::endl;
		delete req;
		return 1;
	}
	if(req->parse(part4.text, part4.length) != Message::PARSING_COMPLETE)
	{
		std::cout << "Message parsing error" << std::endl;
		delete req;
		return 1;
	}*/

	/* PARSING CASE 3:
	 * Message is fed as unequal parts with no line termination falling between bounds
	 */
	/*string<MESSAGE_SIZE> part1 = MAKE_STRING("GET / HTTP/1");
	string<MESSAGE_SIZE> part2 = MAKE_STRING(".1\r\nContent-Length: 4\r\n");
	string<MESSAGE_SIZE> part3 = MAKE_STRING("Field1: data");
	string<MESSAGE_SIZE> part4 = MAKE_STRING("datadatadata");
	string<MESSAGE_SIZE> part5 = MAKE_STRING("\r\n\r\n");
	string<MESSAGE_SIZE> part6 = MAKE_STRING("1234");

	if(req->parse(part1.text, part1.length) != Message::PARSING_SUCESSFUL)
	{
		std::cout << "Message parsing error" << std::endl;
		delete req;
		return 1;
	}
	if(req->parse(part2.text, part2.length) != Message::PARSING_SUCESSFUL)
	{
		std::cout << "Message parsing error" << std::endl;
		delete req;
		return 1;
	}
	if(req->parse(part3.text, part3.length) != Message::PARSING_SUCESSFUL)
	{
		std::cout << "Message parsing error" << std::endl;
		delete req;
		return 1;
	}
	if(req->parse(part4.text, part4.length) != Message::PARSING_SUCESSFUL)
	{
		std::cout << "Message parsing error" << std::endl;
		delete req;
		return 1;
	}
	if(req->parse(part5.text, part5.length) != Message::PARSING_SUCESSFUL)
	{
		std::cout << "Message parsing error" << std::endl;
		delete req;
		return 1;
	}
	if(req->parse(part6.text, part6.length) != Message::PARSING_COMPLETE)
	{
		std::cout << "Message parsing error" << std::endl;
		delete req;
		return 1;
	}*/

	/* PARSING CASE 4:
	 * Buffer ends between \r\n
	 */
	/*string<MESSAGE_SIZE> part1 = MAKE_STRING("GET /echo/timer1 HTTP/1.1\r");
	string<MESSAGE_SIZE> part2 = MAKE_STRING("\n");
	string<MESSAGE_SIZE> part3 = MAKE_STRING("\r\n");

	if(req->parse(part1.text, part1.length) != Message::PARSING_SUCESSFUL)
	{
		std::cout << "Message parsing error" << std::endl;
		delete req;
		return 1;
	}
	if(req->parse(part2.text, part2.length) != Message::PARSING_SUCESSFUL)
	{
		std::cout << "Message parsing error" << std::endl;
		delete req;
		return 1;
	}
	if(req->parse(part3.text, part3.length) != Message::PARSING_COMPLETE)
	{
		std::cout << "Message parsing error" << std::endl;
		delete req;
		return 1;
	}*/

	const char* msg = "GET /hw?v=r&b=y#asd HTTP/1.1\r\nContent-Length: 4\r\n\r\n1234";
	if(req->parse(msg, strlen(msg)) != Message::PARSING_COMPLETE)
	{
		std::cout << "Message parsing error" << std::endl;
		delete req;
		return 1;
	}

	echo->send(req);

	for(int ticks = 0; ticks < 100; ticks++)
	{
		proc->step();
		steps++;
		increase_uptime(100);
	}

	delete echo;
	delete root;
	delete res2;
	delete proc;
	delete timer1;
	delete timer2;
	delete hw;

	return 0;
}
