/*
 * interface.cpp
 *
 *  Created on: Jul 2, 2009
 *      Author: Antoine
 */

#include "posix_socket_interface.h"
#include <iostream>
using namespace std;
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <pal/pal.h>
#include <core/request.h>
#include <core/response.h>

Elements::string<uint8_t> make_connection_key(sockaddr_in &address)
{
	char* key_text = (char*)malloc( sizeof( address.sin_port ) + sizeof( address.sin_addr) );

	key_text[0] = (&address.sin_addr.s_addr)[0];
	key_text[1] = (&address.sin_addr.s_addr)[1];
	key_text[2] = (&address.sin_addr.s_addr)[2];
	key_text[3] = (&address.sin_addr.s_addr)[3];
	key_text[4] = (&address.sin_port)[0];
	key_text[5] = (&address.sin_port)[1];
	Elements::string< uint8_t > key = { key_text, sizeof( address.sin_port ) + sizeof( address.sin_addr) };

	return key;
}

Elements::string<uint8_t> connection_address_type = MAKE_STRING("ipv4_addr");

void* PosixSocketInterface::dispatch( void* args )
{

	int socket_file_descriptor = ((PosixSocketInterface*)args)->file_descriptor;
	PosixSocketInterface* interface = (PosixSocketInterface*)args;
	int number_of_characters_exchanged;
	char buffer[1024];
	socklen_t client_address_length;
	Connection c;

	while( true )
	{
		client_address_length = sizeof(sockaddr_in);
		c.request = NULL;
		c.response = NULL;
		c.file_descriptor = accept(
				socket_file_descriptor,
				(sockaddr*)&c.addr,
				&client_address_length);

		if( c.file_descriptor < 0)
		{
			cerr << "Error on accept." << endl;
			exit(1);
		}

		/*cout << "Accepted connection from client at address " << \
		(uint8_t)((&c->addr.sin_addr.s_addr)[3]) << '.' << \
		(uint8_t)((&c->addr.sin_addr.s_addr)[2]) << '.' << \
		(uint8_t)((&c->addr.sin_addr.s_addr)[1]) << '.' << \
		(uint8_t)((&c->addr.sin_addr.s_addr)[0]) \
		<< " from port " << client_address.sin_port << endl;*/

		number_of_characters_exchanged = read( c.file_descriptor, buffer, 1024);

		if( number_of_characters_exchanged < 0 )
		{
			cerr << "Reading from the socket caused and error." << endl;
			pthread_exit(NULL);
		}

		Connection* current = NULL;

		for(int i = 0; i < interface->connections.items; i++)
		{
			if(c.file_descriptor == interface->connections[i]->file_descriptor)
			{
				current = interface->connections[i];
			}
		}
		if(current == NULL)
		{
			current = (Connection*)malloc(sizeof(Connection));
			*current = c;
			current->request = new Request();
			interface->connections.append(current);
		}

		switch(current->request->parse(buffer, number_of_characters_exchanged))
		{
			case Message::PARSING_SUCESSFUL:
				break;
			case Message::PARSING_COMPLETE:
				interface->send(current->request);
				cout << "New message has arrived!" << endl;
				break;
			default:
				cout << "Parsing failed!" << endl;
				delete current->request;
				interface->connections.remove_item(current);
				free(current);
				continue;
		}

	}
	pthread_exit(NULL);
}

PosixSocketInterface::PosixSocketInterface( int port_number ):
	Authority(),
	port_number(port_number),
	dispatching_thread(0)
{
	cout << "Connecting socket to port " << port_number << endl;
	file_descriptor = socket( AF_INET, SOCK_STREAM, 0);

	if( file_descriptor < 0)
	{
		cerr << "The socket could not be openend." << endl;
		return;
	}

	bzero( (char*)&server_address, sizeof(server_address) );
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(port_number);
	server_address.sin_addr.s_addr = INADDR_ANY;

	if( bind(file_descriptor, (struct sockaddr*) &server_address, sizeof(server_address)) < 0)
	{
		cerr << "Error during binding." << endl;
		return;
	}

	listen( file_descriptor, 5);

	cout << "Listening for connection on port " << port_number << endl;
	cout << "Starting dispatching thread..." << endl;

	if( pthread_create( &dispatching_thread, NULL, dispatch, this) )
	{
		cerr << "Error creating thread." << endl;
		return;
	}

}
PosixSocketInterface::~PosixSocketInterface()
{
	close( file_descriptor );
}

void PosixSocketInterface::run()
{
    Connection* c;

    schedule(NEVER);
    for(int i = 0; i < connections.items; i++)
    {
    	if(connections[i]->response != NULL)
    	{
    		Connection* c = connections.remove(i);
    		reply(c);
    		free(c);
    	}

    }

}

Response::status_code PosixSocketInterface::process( Response* response, Message** return_message )
{

	for(int i = 0; i < connections.items; i++)
	{
		if(connections[i]->request == response->original_request)
		{
			connections[i]->response = response;
			schedule(ASAP);
			return OK_200;
		}
	}

	delete response;
	return OK_200;

}


void PosixSocketInterface::reply(Connection* c)
{


	MESSAGE_SIZE length = c->response->get_header_length();
	char buffer[1500];//header_length];
	c->response->serialize(buffer);
	length += c->response->body_file->read(buffer+length - 1, 1500 - length, true);
	Debug::println(buffer, length - 1);
	//char* buffer = "HTTP/1.0 200\r\nContent-Type: text/html\r\nContent-Length: 94\r\n\r\n<html><body>There are currently no representation associated with this resource.</body></html>";


	if(!write(c->file_descriptor, buffer, length - 1 ))
	{
		cerr << "Writing to the socket caused and error." << endl;
		exit(1);
	}
	/*if(c->response->body_file)
	{
		char body_buffer[1500];
		MESSAGE_SIZE body_buffer_length;

		while(true)
		{
			body_buffer_length = c->response->body_file->read(body_buffer, 1500, true);
			if(!write(c->file_descriptor, body_buffer, body_buffer_length))
			{
				cerr << "Writing to the socket caused and error." << endl;
				exit(1);
			}
			if(body_buffer_length < 1500)
			{
				break;
			}
		}
	}*/

	cout << "Reply sent" << endl;

	delete c->response;
	close(c->file_descriptor);
}


