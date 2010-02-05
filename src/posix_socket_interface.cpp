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
#include <stdint.h>
#include "../elements/utils/types.h"
#include "../elements/core/request.h"
#include "../elements/core/response.h"

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

	int new_socket_file_descriptor;
	int socket_file_descriptor = ((PosixSocketInterface*)args)->file_descriptor;
	PosixSocketInterface* interface = (PosixSocketInterface*)args;
	int number_of_characters_exchanged;
	char* buffer;
	struct sockaddr_in client_address;
	int client_address_length = sizeof( client_address );
	Elements::string<uint8_t> connection_key;

	while( true )
	{

		new_socket_file_descriptor= accept(
				socket_file_descriptor,
				(struct sockaddr*)&client_address,
				(socklen_t*)&client_address_length);

		if( new_socket_file_descriptor < 0)
		{
			cerr << "Error on accept." << endl;
			exit(1);
		}

		cout << "Accepted connection from client at address " << \
		(&client_address.sin_addr.s_addr)[3] << '.' << \
		(&client_address.sin_addr.s_addr)[2] << '.' << \
		(&client_address.sin_addr.s_addr)[1] << '.' << \
		(int)((&client_address.sin_addr.s_addr)[0]) \
		<< " on port " << client_address.sin_port << endl;
		//((char*)&client_address.sin_addr.s_addr)[3] << "." << \
		//((char*)&client_address.sin_addr.s_addr)[2] << "." << \
		//((char*)&client_address.sin_addr.s_addr)[1] << "." << \
		//((char*)&client_address.sin_addr.s_addr)[0] \

		buffer = (char*)malloc(2048);
		bzero( buffer, 2048);
		number_of_characters_exchanged = read( new_socket_file_descriptor, buffer, 2048);

		if( number_of_characters_exchanged < 0 )
		{
			cerr << "Reading from the socket caused and error." << endl;
			pthread_exit(NULL);
		}

		cout << buffer << endl;
		connection_key = make_connection_key( client_address );
		interface->connections.add( connection_key , &new_socket_file_descriptor );
		Message* message;
		if(buffer[4] == '/' && buffer[3] == 'P')
		{
			message = new Response();
		}
		else
		{
			message = new Request();
		}
		message->message.length = number_of_characters_exchanged;
		message->fields.add( connection_address_type, &connection_key );
		message->message.text = buffer;
                interface->receive(message);
                cout << "New message has arrived!" << endl;
	}
	pthread_exit(NULL);
}

PosixSocketInterface::PosixSocketInterface( int port_number ):
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
    Message* message;
    
    while( interface_out.items )
    {
        message = interface_out.dequeue();
        if(message->deserialize())
        {
           /*The message was not correctly formed, we should reply with an
            HTTP error.*/
            delete message;
        }
        else
        {
            if(message_queue.queue(message))
            {
                interface_out.queue(message);
                schedule(NULL, ASAP);
            }
        }

    }
}

Message* PosixSocketInterface::process(Response* response)
{
	/*Solely responses for requests that originated from this interface can be sent back. For now
	the HTTP standards mandates a response can only be in reply to a request, but in the future, this
	may change so this method will have to account for that by checking if there is a host fileld
	in the lone response.*/

	if( response->original_request != NULL )
	{
		if( response->original_request->fields.find(connection_address_type) )
		{
			reply(response);
			return NULL;
		}
	}
	return Authority::process(response);
}

Response* PosixSocketInterface::process(Request* request)
{
	if(request->to_url->is_absolute_url)
	{
		send(request);
		return NULL;
	}

	return Authority::process(request);
}

void PosixSocketInterface::receive(Message* message)
{
    interface_out.queue(message);
    schedule(ASAP);
}

void PosixSocketInterface::send(Request* request)
{

}

void PosixSocketInterface::reply(Response* response)
{
	int file_descriptor;
	int number_of_characters_exchanged;

	file_descriptor = *connections.find(*response->original_request->fields.find(connection_address_type));

	response->Message::serialize();
	cout << "Sending:" << endl;
	cout << response->message.text << endl;
	cout << "total-length:" << response->message.length << endl;
	number_of_characters_exchanged = write(file_descriptor,
			response->message.text, response->message.length);

	if( number_of_characters_exchanged < 0)
	{
		cerr << "Writing to the socket caused and error." << endl;
		exit(1);
	}

	delete response;
	close(file_descriptor);
}


