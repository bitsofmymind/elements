/* posix_socket_interface.cpp - A resource that interfaces with a POSIX socket.
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

// INCLUDES

#include "posix_socket_interface.h"
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <pal/pal.h>
#include <core/request.h>
#include <core/response.h>
#include <signal.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>

using namespace std;

void* PosixSocketInterface::receive( void* args )
{
	int socket_file_descriptor = ((PosixSocketInterface*)args)->file_descriptor;

	// The PosixSocketInterface object was passed in the arguments.
	PosixSocketInterface* interface = (PosixSocketInterface*)args;
	int number_of_characters_exchanged;
	char buffer[1024]; // A buffer to hold the received data.
	socklen_t client_address_length;
	Connection c;

	// Block this thread from receiving signals.
	sigset_t set;
	sigfillset(&set);
	pthread_sigmask(SIG_SETMASK, &set, NULL);

	/* Since this method is meant to run inside a thread, it is not supposed to
	 * return. */
	while( true )
	{
		client_address_length = sizeof(sockaddr_in);
		// Initialize a connection.
		c.request = NULL;

		c.response = NULL;
		// Accept a connection. This is a blocking call.
		c.file_descriptor = accept(socket_file_descriptor, (sockaddr*)&c.addr, &client_address_length);

		if(c.file_descriptor < 0) // If there was an error on accept.
		{
			cerr << "Error on accept." << endl;
			break; // End the thread.
		}

		cout << "Accepted connection from client at address " <<
		inet_ntoa(c.addr.sin_addr) <<
		" from port " << c.addr.sin_port << endl;

		bool done_read = false; // If we are done reading from the buffer.
		do // Loop until all request data has been read and parsed..
		{
			/* Read some number of characters from the buffer. Its entirety could
			 * be read, but for the sake of testing the framework, it is retrieved
			 * and parsed in small chunks.*/
			number_of_characters_exchanged = read(c.file_descriptor, buffer, 246);

			if(number_of_characters_exchanged < 1) // If reading returned 0.
			{
				cerr << "Client closed the connection prematurely." << endl;
				close(c.file_descriptor); // Close the connection.
				break;
			}

			Connection* current = NULL;

			// For each opened connection.
			for(int i = 0; i < interface->connections.items; i++)
			{
				// If this connection is receiving new data.
				if(c.file_descriptor == interface->connections[i]->file_descriptor)
				{
					current = interface->connections[i]; // Set it as a the current connection.
				}
			}

			if(current == NULL) // If this is a new connection.
			{
				// Allocate space for the new connection.
				current = (Connection*)malloc(sizeof(Connection));
				*current = c; // Copy the data from the accepted connection.
				current->request = new Request(); // Create a request object.

				// This call needs to be protected by a mutex.
				pthread_mutex_lock(&interface->_receive_mutex);
					// Add the connection to the list.
					interface->connections.append(current);
				pthread_mutex_unlock(&interface->_receive_mutex);
			}

			// Parse the part of the buffer we have received.
			switch(current->request->parse(buffer, number_of_characters_exchanged))
			{
				case Message::PARSING_SUCESSFUL: // Parsing is going well.
					break;
				case Message::PARSING_COMPLETE: // Parsing is done.
					done_read = true;
					interface->dispatch(current->request); // Dispatch the request.
					cout << "New message has arrived!" << endl;
					break;
				default: // Something went wrong.
					cout << "Parsing failed!" << endl;
					//Clean up the connection.
					delete current->request;
					// This call needs to be protected by a mutex.
					pthread_mutex_lock(&interface->_receive_mutex);
						interface->connections.remove_item(current);
					pthread_mutex_unlock(&interface->_receive_mutex);
					close(c.file_descriptor);
					free(current);
					done_read = true;  // Done reading the request data.
			}
		}
		while(!done_read);
	}

	pthread_exit(NULL);
}

PosixSocketInterface::PosixSocketInterface( int port_number ):
	Authority(),
	port_number(port_number),
	dispatching_thread(0)
{
	pthread_mutex_init(&_receive_mutex, NULL);

	cout << "Connecting socket to port " << port_number << endl;

	file_descriptor = socket( AF_INET, SOCK_STREAM, 0); // Creates a socket.
	if( file_descriptor < 0) // If the socket creation failed.
	{
		cerr << "The socket could not be opened." << endl;
		return; // Cannot proceed.
	}

	// Blanks the server_address structure to start from a clean slate.
	bzero( (char*)&server_address, sizeof(server_address) );

	// Sets the wanted parameters for the address.
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(port_number);
	server_address.sin_addr.s_addr = INADDR_ANY;

	// Bind the socket to the wanted address.
	if( bind(file_descriptor, (struct sockaddr*) &server_address, sizeof(server_address)) < 0)
	{
		cerr << "Error during binding." << endl;
		return; // Cannot proceed.
	}

	listen( file_descriptor, 5); // Listen to the wanted address.

	cout << "Listening for connection on port " << port_number << endl;
	cout << "Starting dispatching thread..." << endl;

	// Start the thread that will dispatch received requests.
	if(pthread_create(&dispatching_thread, NULL, receive, this))
	{
		cerr << "Error creating thread." << endl;
		return; // Cannot proceed.
	}
}

PosixSocketInterface::~PosixSocketInterface()
{
	// This call needs to be protected by a mutex.
	pthread_mutex_lock(&_receive_mutex);
	pthread_cancel(dispatching_thread); // Stop the receiving thread.
	pthread_join(dispatching_thread, NULL); // Wait for it to terminate.
	pthread_mutex_unlock(&_receive_mutex);

	// For each current connection.
	for(int i = 0; i < connections.items; i++)
	{
		// Close the connection.
		close(connections[i]->file_descriptor);
		/* PROBLEM!
		 * Since the connections also hold pointers to the request and response
		 * and those could currently being processed by the framework, it is
		 * difficult to know if they can be deleted here. Messages should not
		 * be kept in the connections.
		 * */
	}

	close(file_descriptor); // Close the socket.
}

void PosixSocketInterface::run()
{
	// This call needs to be protected by a mutex.
	pthread_mutex_lock(&_receive_mutex);
		// For each connection currently open.
		for(int i = 0; i < connections.items; i++)
		{
			 // If there is a response pending for this connection.
			if(connections[i]->response != NULL)
			{
				 // Remove the connection from the list.
				Connection* c = connections.remove(i);
				reply(c); // Reply to the client.
				delete c->response; // Also deletes the request.
				close(c->file_descriptor); // Close the connection with the client.
				free(c); // Frees the connection.
			}
		}
	pthread_mutex_unlock(&_receive_mutex);

    Authority::run(); // Call parent method.
}

Response::status_code PosixSocketInterface::process(Response* response)
{
	// This call needs to be protected by a mutex.
	pthread_mutex_lock(&_receive_mutex);
		// For each connection currently open.
		for(int i = 0; i < connections.items; i++)
		{
			// If the received response is for this connection.
			if(connections[i]->request == response->original_request)
			{
				connections[i]->response = response; // Set the response on that connection.
				 // Run the resource as soon as possible to send the reply.
				schedule(ASAP);

				pthread_mutex_unlock(&_receive_mutex);

				return OK_200; // Response has been processed.
			}
		}
	pthread_mutex_unlock(&_receive_mutex);

	return Authority::process(response); // Calls the parent.
}

void PosixSocketInterface::reply(Connection* c)
{
	char* response; // Will hold the response string.

	// Compute the length of the response.
	size_t length = c->response->serialize(NULL, false);

	response = (char*)malloc(length + 1); // Allocates a block for the response.

	c->response->serialize(response, true); // Serialize the response in the buffer.

	 // Sends the response's header to the client.
	if(!write(c->file_descriptor, response, length))
	{
		cerr << "Writing to the socket caused and error." << endl;
		free(response); // Frees the buffer for the response.
		return; // Cannot proceed.
	}

	free(response); // Frees the buffer for the response.

	if(c->response->get_body()) // If the response has a body.
	{
		MESSAGE_SIZE len; // Length of the body chunk.
		const int sz = 100; // The size of the buffer.
		char buffer[sz]; //

		/* Body will be sent in chunk. It could very well be send in one block,
		 * but just for the sake of testing, it is sent in many parts. */
		while(true)
		{
			// Read part of the body in the buffer.
			len = c->response->get_body()->read(&buffer[0], sz);

			// Send the part of the body that is currently in the buffer.
			if(len != 0 && !write(c->file_descriptor, buffer, len))
			{
				cerr << "Writing to the socket caused and error." << endl;
				return; // Cannot proceed.
			}

			if(len < sz)
			{
				break; // Done sending the body.
			}
		}
	}

	cout << "Reply sent" << endl;
}


