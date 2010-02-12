//============================================================================
// Name        : Elements_test_bench.cpp
// Author      : Antoine Mercier-Linteau
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
//using namespace std;
#include <core/resource.h>
#include <core/authority.h>
#include <utils/types.h>
#include <posix_socket_interface.h>
#include <root.h>
#include <thread.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>

Root* root;
PosixThreadManager* thread_manager;
PosixSocketInterface* posix_socket_interface;
Authority* auth0, * auth1, * auth2;
Processing* proc0;
Resource* res0, * res1, * res2, * res3, * res4, * res5;

void exit_function( int i )
{
	std::cout << "Exiting Elements test executable." << std::endl;
	exit(0);
}

int main(int argc, char* argv[])
{
	signal(SIGINT, exit_function);
	std::cout << "Starting Elements test executable." << std::endl;
	std::cout << "Creating Resources...";
	root = new Root();
	root->id = "root";
	//thread_manager = new PosixThreadManager(root, 0);
	posix_socket_interface = new PosixSocketInterface( atoi(argv[1]) );
	posix_socket_interface->id = "posix_socket_interface";

	auth0 = new Authority();
	auth0->id = "auth0";
	auth1 = new Authority();
	auth1->id = "auth1";
	auth2 = new Authority();
	auth2->id = "auth2";

	proc0 = new PosixMainThread();
	proc0->id = "proc0";

	res0 = new Resource();
	res0->id = "res0";
	res1 = new Resource();
	res1->id = "res1";
	res2 = new Resource();
	res2->id = "res2";
	res3 = new Resource();
	res3->id = "res3";
	res4 = new Resource();
	res4->id = "res4";
	res5 = new Resource();
	res5->id = "res5";
	std::cout << "[DONE]" << std::endl;

	std::cout << "Creating Resource Tree...";
	root->add_child(Elements::string<uint8_t>::make("proc0"), proc0);
	//root->add_child(Elements::string<uint8_t>::make("thread_manager"), thread_manager);
	root->add_child(Elements::string<uint8_t>::make("posix_socket_interface"), posix_socket_interface);
	root->add_child(Elements::string<uint8_t>::make("auth0"), auth0);
	root->add_child(Elements::string<uint8_t>::make("res0"), res0);

	auth0->add_child(Elements::string<uint8_t>::make("auth1"), auth1);
	auth0->add_child(Elements::string<uint8_t>::make("res1"), res1);
	auth1->add_child(Elements::string<uint8_t>::make("res2"), res2);
	res1->add_child(Elements::string<uint8_t>::make("auth2"), auth2);
	auth2->add_child(Elements::string<uint8_t>::make("res3"), res3);
	auth2->add_child(Elements::string<uint8_t>::make("res4"), res4);
	res4->add_child(Elements::string<uint8_t>::make("res5"), res5);
	std::cout << "[DONE]" << std::endl;

	std::cout << "Starting processing...[DONE]" << std::endl;
	std::cout << "Waiting for request ( press c to quit )..." << std::endl;

	proc0->start();

	return 0;
}
