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

void processing_wake(){}
void processing_sleep(uint64_t time){}

#include <stdlib.h>
#include "string.h"

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

		return NULL;
	}

};

class Timer: public Resource
{
	public:


	virtual void run()
	{
		//Elements::e_time_t time =;
		schedule( 500 );
		std::cout << "Timer tick" << std::endl;
	}
};

int main()
{
	Echo* echo = new Echo();
	Resource* res2 = new Resource();
	Resource* res3 = new Resource();
	Authority* root = new Authority();
	Authority* auth1 = new Authority();
	Processing* proc = new Processing(NULL);
	Timer* timer = new Timer();

	root->add_child(Elements::string<uint8_t>::make("proc"), proc);
	root->add_child(Elements::string<uint8_t>::make("echo"), echo);
	root->add_child(Elements::string<uint8_t>::make("res2"), res2);
	root->add_child(Elements::string<uint8_t>::make("auth1"), auth1);

	auth1->add_child(Elements::string<uint8_t>::make("timer"), timer);

	res2->add_child(Elements::string<uint8_t>::make("res3"), res3);

	const char* cmsg = "GET /res2/res3 HTTP/1.1\r\n\
Host: www.example.com\r\n\r\n";
	char * msg = (char*)malloc(sizeof("GET /res2/res3 HTTP/1.1\r\n\
Host: www.example.com\r\n\r\n"));
	strcpy(msg,cmsg);
	string<uint32_t> str = Elements::string<uint32_t>::make(msg);
	Request* req = new Request();

	req->deserialize(str, str.text);

	echo->send(req);

	int steps = 0;

	/*while(!echo->echoed)
	{
		proc->step();
		steps++;
	}*/

	std::cout << "Echo completed in " << steps << " steps." << std::endl;


	for(int ticks = 0; ticks < 100; ticks++)
	{
		proc->step();
		increase_uptime(100);
	}

	return 0;
}
