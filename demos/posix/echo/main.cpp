/* main.cpp - Main file for the echo demo.
 * Copyright (C) 2011 Antoine Mercier-Linteau
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
#include <pal/pal.h>
#include <core/resource.h>
#include <core/authority.h>
#include <core/processing.h>
#include <core/request.h>
#include <utils/utils.h>
#include <utils/memfile.h>
#include <iostream>
#include <stdlib.h>
#include "string.h"

//Platform function implementations
void processing_wake(){}
void processing_sleep(uint64_t time){}
void Debug::print_char(char c){ std::cout << c; }
void Debug::println(){ std::cout << std::endl; }
void heart_beat(){}

/// A counter for the number of steps processing has done.
uint32_t steps = 0;

/// A simple Resource class that echoes messages it receives and prints what it sent.
class Echo: public Resource
{
	public:

		/**
		 * Sends a request.
		 * @param request the request to send.
		 * */
		void send(Request* request){ dispatch(request); }

        /// Process a response message.
        /**
         * YOU SHOULD NOT DELETE OR FREE THE RESPONSE ARGUMENT!
         * Parent method override to print the received response and its associated
         * request.
         * @param request the request to process.
         * @param response the response to fill if a response should be returned (which
         * depends on the status code).
         * @return the status_code produced while processing the request.
         * @todo make the request object const so it will not be deleted.
         */
		virtual Response::status_code process(Response* response)
		{
			// If the response has not reached its destination.
			if(response->to_destination())
			{
				// Let the parent method handle it.
				return Resource::process(response);
			}

			std::cout << "----------------------" << std::endl;
			std::cout << "Echo completed in " << steps << " steps." << std::endl;

			// Print the request.
			std::cout << "Request:" << std::endl;

			// Get the length of the serialized request.
			size_t len = response->original_request->serialize(NULL, false);
			/* Allocate a buffer to hold the serialized request. The buffer length
			 * is increased by one to make room for a null terminating character. */
			char* buffer = (char*)malloc( len + 1);
			//Serialize the request in the buffer.
			response->original_request->serialize(buffer, true);
			buffer[len] = '\0'; // Terminate the serialized request.
			std::cout << buffer << std::endl; // Display the buffer.
			free(buffer); // Done with the buffer.

			// Print the response.
			std::cout << "Response:" << std::endl;

			// Get the length of the serialized response.
			len = response->serialize(NULL, false);
			/* Allocate a buffer to hold the serialized response. The buffer length
			 * is increased by one to make room for a null terminating character. */
			buffer = (char*)malloc( len + 1);
			 // Serialize the response into the buffer.
			response->serialize(buffer, true);
			buffer[len] = '\0';  // Terminate the serialized response.
			std::cout << buffer << std::endl; // Display the buffer.
			free(buffer);// Done with the buffer.

			if(response->get_body()) // If the response contains a body.
			{
				char body_buffer[21]; // Allocate a buffer for displaying the body.
				uint8_t read; // Number of characters read.

				response->get_body()->cursor(0); // Reset the cursor.

				do // Read the content of the buffer.
				{
					// Attempt to read 20 bytes from the buffer.
					read = response->get_body()->read(body_buffer, 20);
					body_buffer[read]  ='\0'; // Terminate what was read.
					std::cout << body_buffer; // Display the characters read/
				}
				while(read > 0); // If there is data left in the body.
			}

			std::cout << std::endl << std::endl << "----------------------" << std::endl;

			return DONE_207; // Done processing the response.
		}

        /// Process a response message.
        /**
         * Parent method override to echo the body of the received request.
         * @param response the response to process.
         * @return the status_code produced while processing the response.
         */
		virtual Response::status_code process(Request* request, Response* response)
		{
			// If the request has not reached its destination.
			if(request->to_destination())
			{
				// Let the parent method handle it.
				return Resource::process(request, response);
			}

			// Allocate a buffer to copy the request's body.
			char* buffer = (char*)malloc(request->get_body()->size + 1);
			request->get_body()->extract(buffer); // Extract the body in the buffer.
			buffer[request->get_body()->size] = '\0'; // Terminate the buffer.
			// Create a new MemFile with the buffer and set it as the response's body.
			response->set_body(new MemFile(buffer, request->get_body()->size, false), MIME::TEXT_HTML);

			std::cout << "Echoing message." << std::endl;

			return OK_200; // Done.
		}
};

/// A Resource that displays a message at a given interval.
class Timer: public Resource
{
	private:

		/// The time interval at which the resource should be displaying its message.
		uint32_t _interval;

		/// An identifier for the message the resource will output.
		int _id;

	public:
		/**
		 * The class constructor.
		 * @param interval the interval at which the resource should display a
		 * message.
		 * @param id an identifier for the message.
		 * */
		Timer(uint32_t interval, int id):
			Resource(),
			_interval(interval),
			_id(id)
		{
			schedule(_interval); // Schedule the resource to be run.
		}

		/// Runs the resource.
		virtual void run()
		{
			// Display a message.
			std::cout << "Timer " << _id << " tick" << std::endl;

			// Schedule the resource to be run again.
			schedule(_interval);
		}
};

/// The program's entry point.
int main()
{
	// Instantiate all the resources that will be used.
	Echo* echo1 = new Echo();
	Echo* echo2 = new Echo();
	Resource* res2 = new Resource();
	Authority* root = new Authority();
	Processing* proc = new Processing(NULL);
	Timer* timer1 = new Timer(500, 1); // Have timer 1 tick every 500 cycles.
	Timer* timer2 = new Timer(1000, 2); // Have timer 1 tick every 1000 cycles.

	// Link resources into a tree:
	/*
	 * root => proc
	 *     |=> echo1 => timer1
	 *     		    |=> timer2
	 *     |=> res2 => echo2
	 * */

	root->add_child("proc", proc);
	root->add_child("echo1", echo1);
	root->add_child("res2", res2);
	res2->add_child("echo2", echo2);
	echo1->add_child("timer1", timer1);
	echo1->add_child("timer2", timer2);

	Request* req = new Request(); // Create a new request.

	/* The content of the message. Allocating a buffer is not necessary because
	 * the Request object will copy the message internally. */
	const char* msg = "GET /res2/echo2/?v=r&b=y#asd HTTP/1.1\r\nContent-Length: 6\r\n\r\n123456";

	// If parsing the message failed.
	if(req->parse(msg, strlen(msg)) != Message::PARSING_COMPLETE)
	{
		std::cout << "Message parsing error" << std::endl;
		delete req;
		return 1;
	}

	echo1->send(req); // Send the message from echo1.

	/* Run the framework. Normally, Processing::start() is called and does
	 * not return. In this case however, we do not want the program to last
	 * indefinitely so we run the loop ourselves. The uptime would also normally
	 * be hooked up to the system's timer and update itself automatically but
	 * in this case, we also want full control on it so it is updated manually.*/
	for(int ticks = 0; ticks < 100; ticks++)
	{
		proc->step(); // Steps through processing.
		steps++; // Count the number of steps.
		increase_uptime(100); // Increase the uptime by 100 cycles.
	}

	// Clean up all resources for good measure and to test for memory leaks.
	/* This is not necessary since the program is ending, but it's a good habit
	 * and lets up debug destructors and find memory leaks. */
	delete root; // Child resources will be deleted as well.

	return 0;
}
