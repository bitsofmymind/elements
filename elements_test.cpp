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
void processing_sleep(Elements::e_time_t time){}

int main()
{
	//Resource* res1 = new Resource();
	//Resource* res2 = new Resource();
	//Resource* res3 = new Resource();
	//Authority* root = new Authority();
	Processing* proc = new Processing();
	/*root->add_child(Elements::string<uint8_t>::make("proc"), proc);
	root->add_child(Elements::string<uint8_t>::make("res1"), res1);
	root->add_child(Elements::string<uint8_t>::make("res2"), res2);
	res2->add_child(Elements::string<uint8_t>::make("res3"), res3);*/

	const char* msg = "GET /res1/res3 HTTP/1.1\r\n\
Host: www.example.com\r\n\r\n";
	string<uint32_t> str = Elements::string<uint32_t>::make(msg);
	Request* req = new Request();
	req->deserialize(str, str.text);

	//res2->send(req);

	proc->start();

	return 0;
}
