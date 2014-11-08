/* main.h - main file for a elements hello world demo.
 * Copyright (C) 2014 Antoine Mercier-Linteau
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <iostream>
#include <posix_socket_interface.h>
#include <pthread.h>
#include <core/resource.h>
#include <core/authority.h>
#include <core/processing.h>
#include <pal/pal.h>
#include <stdint.h>
#include <stdio.h>
#include <signal.h>
#include "hello_world.h"

/** The root resource of the framework.*/
Authority* root;

/** A function that cleanly terminates the framework.
 * @param i the signal number. */
void exit_function( int i )
{
	if(i != SIGINT && i != SIGHUP && i != SIGTERM)
	{
		return;
	}

	std::cout << "Exiting framework...";
	delete root;
	std::cout << "[DONE]" << std::endl;
	exit(0); // Done.
}

int main(int argc, char* argv[])
{
	if(argc < 2) // If the number of arguments is below 2.
	{
		std::cerr << "Missing arguments." << std::endl;
		exit(1); // Error.
	}

	std::cout << "Starting Elements Framework." << std::endl;

	init(); // Initialize the platform.

	std::cout << "Creating Resources...";

	root = new Authority(); // Create the root resource.

	//signal(SIGINT, exit_function); // Bind the exit function to the interrupt signal.

	struct sigaction sigint_handler;

	sigint_handler.sa_handler = exit_function;
	sigemptyset(&sigint_handler.sa_mask);
	sigint_handler.sa_flags = 0;

	/* NOTE: SIGINT is added just for good mesure. Since we are in a multithreaded
	 * environment, it will get caught by pthread and not rebroadcasted.*/
	sigaction(SIGINT, &sigint_handler, NULL);
	sigaction(SIGHUP, &sigint_handler, NULL);
	sigaction(SIGTERM, &sigint_handler, NULL);

	// Create a bunch of vanilla resources.
	Authority* auth0 = new Authority();
	Processing* proc0 = new Processing(NULL); // Processing will stop at root.
	Resource* res0 = new Resource(), * res1 = new Resource(), * res2 = new Resource();

	// Creates the resource that will handle network connections.
	PosixSocketInterface* posix_socket_interface = new PosixSocketInterface(atoi(argv[1]));
	HelloWorld* hw = new HelloWorld(); // Creates a hello world resource.

	std::cout << "[DONE]" << std::endl;

	std::cout << "Creating Resource Tree...";

	// Adds all the created resources to the tree.
	root->add_child("proc0", proc0);
	root->add_child("posix_socket_interface", posix_socket_interface);
	root->add_child("auth0", auth0);
	root->add_child("res0", res0);
	root->add_child("hw", hw);

	auth0->add_child("res1", res1);
	auth0->add_child("res2", res2);

	std::cout << "[DONE]" << std::endl;

	std::cout << "Starting processing...[DONE]" << std::endl;
	std::cout << "Waiting for request ( press c to quit )..." << std::endl;

	proc0->start(); // Start the framework.

	exit(0); // Done.
}
