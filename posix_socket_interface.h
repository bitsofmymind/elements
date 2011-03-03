/*
 * interface.h
 *
 *  Created on: Jul 2, 2009
 *      Author: Antoine
 */

#ifndef INTERFACE_H_
#define INTERFACE_H_

#include <utils/utils.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <core/authority.h>
#include <core/message.h>
#include <pthread.h>
#include <sys/types.h>

class PosixSocketInterface: public Authority
{
    protected:
		struct Connection
		{
			int file_descriptor;
			Request* request;
			Response* response;
			struct sockaddr_in addr;
		};
		Queue<Message>* filler; /*interface_out, for some reason, is at the same address as Authority::messageQueue. This makes no sense
		and could be a linker bug*/

    public:


		List<Connection> connections;
		struct sockaddr_in server_address;
		int port_number;
		int file_descriptor;
		pthread_t dispatching_thread;


		PosixSocketInterface( int port_number );
		~PosixSocketInterface();

		static void* dispatch( void* args );

        virtual void run(void);
        virtual Response::status_code process( Response* response, Message** return_message );
		void reply(Connection*);

};

#endif /* INTERFACE_H_ */
