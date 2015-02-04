/* posix_socket_interface.h - A resource that interfaces with a POSIX socket.
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

#ifndef INTERFACE_H_
#define INTERFACE_H_

// INCLUDES

#include <utils/utils.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <core/authority.h>
#include <core/message.h>
#include <pthread.h>
#include <sys/types.h>

/**
 * This class a Resource that provides an interface between a POSIX socket an
 * the framework, enabling messages to be sent to and from that socket.
 * */
class PosixSocketInterface: public Authority
{
	private:
		pthread_mutex_t _receive_mutex;

	protected:

		/**
		 * This structure holds the information from a connection to the socket.
		 * */
		struct Connection
		{
			/**
			 * The file descriptor of the connection.
			 * */
			int file_descriptor;

			/**
			 * The request received from the connection.
			 * */
			Request* request;

			/**
			 * The response to the request.
			 * */
			const Response* response;

			/**
			 * The address of the client.
			 * */
			struct sockaddr_in addr;
		};

		// Queue<Message>* filler; /*interface_out, for some reason, is at the same address as Authority::messageQueue. This makes no sense
		// and could be a linker bug*/

		/**
		 * A list to keep active connections.
		 * */
		List<Connection*> connections;

		/**
		 * The address of the socket interface.
		 * */
		struct sockaddr_in server_address;

		/**
		 * The port number of the interface.
		 * */
		int port_number;

		/**
		 * A file descriptor to the socket.
		 * */
		int file_descriptor;

		/**
		 * The dispatching thread that receives requests.
		 * */
		pthread_t dispatching_thread;

    public :

		/**
		 * Since receive is called by the OS, it has to be static.
		 * */
		static void* receive( void* args );

		/**
		 * Class constructor.
		 * @param port_number the port to which the socket should bind.
		 * */
		PosixSocketInterface( int port_number );

		/**
		 * Class destructor.
		 * */
		virtual ~PosixSocketInterface();

		/**
		 * Runs the resource. Effectively handling connections and sending
		 * replies.
		 * */
        virtual void run(void);

        /**
         * Processes a received response.
         * @param response the response.
         * @return the result of the processing.
         * */
        virtual Response::status_code process( const Response* response );

    protected:

        /** Sends a reply to a request received through a connection.
         * @param connection the connection from which the request originated.
         * */
        void reply(Connection* connection);
};

#endif /* INTERFACE_H_ */
