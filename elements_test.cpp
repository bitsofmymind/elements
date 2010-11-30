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
#include "filein.h"

void processing_wake(){}
void processing_sleep(uint64_t time){}

class Echo: public Resource
{
	public:
		bool echoed;

	Echo(): Resource(), echoed(false)
	{}

	virtual Message* process(Response* response)
	{
		Resource::process(response);
		echoed = true;
		delete response;
		return NULL;
	}

};

class Timer: public Resource
{
	uint32_t _interval;
	int _id;

	public:
	Timer(uint32_t interval, int id): Resource(), _interval(interval), _id(id) { }

	virtual void run()
	{
		//Elements::e_time_t time =;
		schedule( _interval );
		std::cout << "Timer " << _id << " tick" << std::endl;
	}
};

int main()
{
	Echo* echo = new Echo();
	Resource* res2 = new Resource();
	//Resource* res3 = new Resource();
	Authority* root = new Authority();
	//Authority* auth1 = new Authority();
	Processing* proc = new Processing(NULL);
	Timer* timer1 = new Timer(500, 1);
	Timer* timer2 = new Timer(1000, 2);
	//Filein* filein = new Filein();

	root->add_child(Elements::string<uint8_t>::make("proc"), proc);
	root->add_child(Elements::string<uint8_t>::make("echo"), echo);
	root->add_child(Elements::string<uint8_t>::make("res2"), res2);
	//root->add_child(Elements::string<uint8_t>::make("auth1"), auth1);
	//root->add_child(Elements::string<uint8_t>::make("filein"), filein);
	echo->add_child(Elements::string<uint8_t>::make("timer1"), timer1);
	echo->add_child(Elements::string<uint8_t>::make("timer2"), timer2);

	//res2->add_child(Elements::string<uint8_t>::make("res3"), res3);

	const char* cmsg = "GET /res2 HTTP/1.1\r\nHost: www.example.com\r\n\r\n";

	for(int i  = 0; i< 3; i++)
	{
		char * msg = (char*)malloc(sizeof("GET /res2 HTTP/1.1\r\nHost: www.example.com\r\n\r\n"));
		strcpy(msg,cmsg);
		string<uint32_t> str = Elements::string<uint32_t>::make(msg);
		Request* req = new Request();

		req->deserialize(str, str.text);

		echo->send(req);

		int steps = 0;

		while(!echo->echoed)
		{
			proc->step();
			steps++;
		}
		echo->echoed = false;
		std::cout << "Echo completed in " << steps << " steps." << std::endl;

	}

	for(int ticks = 0; ticks < 100; ticks++)
	{
		proc->step();
		increase_uptime(100);
	}

	delete echo;
	delete root;
	delete res2;
	delete proc;
	delete timer1;
	delete timer2;

	return 0;
}
