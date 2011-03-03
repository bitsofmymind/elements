//============================================================================
// Name        : Elements_test_bench.cpp
// Author      : Antoine Mercier-Linteau
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
//using namespace std;
#include <posix_socket_interface.h>
#include <pthread.h>
#include <core/resource.h>
#include <core/authority.h>
#include <core/processing.h>
#include <utils/types.h>
#include <pal/pal.h>
#include <stdint.h>
#include <stdio.h>
#include <signal.h>



void exit_function( int i )
{
	std::cout << "Exiting Elements test executable." << std::endl;
	exit(0);
}

int main(int argc, char* argv[])
{
	signal(SIGINT, exit_function);
	init();

	std::cout << "Starting Elements test executable." << std::endl;
	std::cout << "Creating Resources...";

	Authority* root = new Authority();
	Authority* auth0 = new Authority();
	Processing* proc0 = new Processing(NULL);
	Resource* res0 = new Resource(), * res1 = new Resource(), * res2 = new Resource();
	PosixSocketInterface* posix_socket_interface = new PosixSocketInterface( atoi(argv[1]));

	std::cout << "[DONE]" << std::endl;

	std::cout << "Creating Resource Tree...";
	root->add_child(Elements::string<uint8_t>::make("proc0"), proc0);
	//root->add_child(Elements::string<uint8_t>::make("thread_manager"), thread_manager);
	root->add_child(Elements::string<uint8_t>::make("posix_socket_interface"), posix_socket_interface);
	root->add_child(Elements::string<uint8_t>::make("auth0"), auth0);
	root->add_child(Elements::string<uint8_t>::make("res0"), res0);

	auth0->add_child(Elements::string<uint8_t>::make("res1"), res1);
	auth0->add_child(Elements::string<uint8_t>::make("res2"), res2);

	std::cout << "[DONE]" << std::endl;

	std::cout << "Starting processing...[DONE]" << std::endl;
	std::cout << "Waiting for request ( press c to quit )..." << std::endl;

	proc0->start();

	return 0;
}
