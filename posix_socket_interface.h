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
        Queue<Message> interface_out;

        public:
		struct connection
		{
			int file_descriptor;
		};

		Dictionary<int> connections;
		struct sockaddr_in server_address;
		int port_number;
		int file_descriptor;
		pthread_t dispatching_thread;


		PosixSocketInterface( int port_number );
		~PosixSocketInterface();

		static void* dispatch( void* args );

        virtual void run(void);
		virtual Message* process(Response*);
		virtual Response* process(Request*);

                void receive(Message*);

		void send(Request*);
		void reply(Response*);

};

#endif /* INTERFACE_H_ */
