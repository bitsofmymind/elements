/* test_message_passing.cpp - Source file for Message passing unit tests.
 * Copyright (C) 2015 Antoine Mercier-Linteau
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
#include <stdlib.h>
#include "string.h"
#include <core/request.h>
#include <core/response.h>
#include <core/resource.h>
#include <core/authority.h>
#include <core/processing.h>

/** A class that allows sending and retrieving messages for testing the
 * framework. */
class TestResource: public Resource
{
	public:

		/** The last received response.*/
		const Response* last_response;

		/// Class constructor.
		TestResource():
			Resource(),
			last_response(NULL)
		{

		}

		/// Process a response message.
		/**
		 * YOU SHOULD NOT DELETE OR FREE THE RESPONSE ARGUMENT!
		 * This method is meant to be overridden in order to specialize this class.
		 * @param response the response to process.
		 * @return the status_code produced while processing the response.
		 */
		virtual Response::status_code process(const Response* response)
		{
			if(response->to_destination() != 0) // If the response is not at destination.
			{
				return Resource::process(response);
			}

			last_response = response;

			// Normally, we would return a DONE_207, but this gets the resource deleted.
			return OK_200;
		}

		/**
		 * Send a textual request.
		 * @param request the text of the message to send.
		 * @return if the request was valid.
		 * */
		bool send_request(const char* request)
		{
			Request* message = new Request();

			if(message->parse(request) != Message::PARSING_COMPLETE)
			{
				delete message;
				return false;
			}

			dispatch(message);

			return true;
		}
};

/** A class that can send and receive messages from another instance of the
 * framework. */
class TestInterface: public Authority
{
	private:
		/** The interface this instance is communicating with. */
		TestInterface* _interface;

	public:

		TestInterface():
			Authority(),
			_interface(NULL)
		{

		}

		/// Adds a Resource as a child.
		/**
		 * Override of parent implementation to prevent adding children to the
		 * interface so that every message directed through the interface is
		 * routed through it.
		 * @param name the name of the child.
		 * @param child the child to add.
		 * @return 0 if adding was successful, another value if an error occurred.
		 * */
		int8_t add_child(const char* name, Resource* child)
		{
			return 1; // Cannot add children to an interface.
		}

		/** @param interface the interface this instance will be communicating
		 * with.*/
		void set_interface(TestInterface* interface){ _interface = interface; }

		/** Receive a textual message through the other interface.
		 * @param data the message to receive.
		 * @return 0 if receiving was a success, 1 if it failed.*/
		uint8_t receive(char* data)
		{
			// Message is assumed to be response first.
			Message* message = new Response(OK_200, NULL);

			// If the message could not be parsed.
			if(message->parse(data) != Message::PARSING_COMPLETE)
			{
				// It is most likely a request.
				delete message;
				message = new Request();

				// If parsing failed again.
				if(message->parse(data) != Message::PARSING_COMPLETE)
				{
					free(data);
					delete(message);
					return 1; // Error.
				}
			}

			// If the message did no include the To-Url or From-Url headers.
			if(!message->get_to_url() || !message->get_from_url())
			{
				free(data);
				delete message;
				return 1;
			}

			message_queue.queue(message); // Queue the message for processing.

			free(data);

			schedule(ASAP); // A new message has arrived.

			return 0;
		}

	protected:

		/// Run the interface
		void run(void)
		{
			if(!_interface) // If the interface is in the disconnected state.
			{
				/** Let the parent function handle the messages. Since an
				 * Interface cannot have children resources, a 404 will be
				 * returned.*/
				Authority::run();

				return;
			}

			const Message* message; // The message currently being processed.

			// While there are messages in the queue.
			while(message_queue.get_item_count())
			{
				message = message_queue.peek();

				// If the message is at destination.
				if(!message->to_destination())
				{
					// It is directed to this resource.
					dispatch((Message*)message_queue.dequeue());

					continue;
				}

				const char* current = message->current();

				// If the url is directed to go through the interface.
				if(current && !strcmp(current, get_parent()->get_name(this)))
				{
					// This is an outgoing message.

					// Send the message.
					if(send((Message*)message_queue.dequeue()))
					{
						// Sending the message failed.

						// If the message was a request.
						if(message->get_type() == Message::REQUEST)
						{
							// Inform the sending resource that sending failed.
							Response* response = new Response(BAD_REQUEST_400, (Request*)message);

							dispatch(response);
						}
						else // The message was a response.
						{
							delete message; // Nothing we can do, delete the message.
						}

						continue;
					}
				}
				else // This is an incoming message.
				{
					dispatch((Message*)message);
					message_queue.dequeue();
				}
			}
		}

		/** Send a message through the interface.
		 * @param message the message to send.
		 * @return 0 if send was a success, 1 if it failed. */
		uint8_t send(Message* message)
		{
			if(!message->get_to_url()->is_absolute()) // If the message is not absolute.
			{
				return 1; // Failed.
			}

			uint8_t to_interface = message->get_to_url()->get_resources()->get_item_count() - message->to_destination();

			// Remove all the resources that precede the interface (except root).
			for(uint8_t i = 1 ; i < to_interface; i++)
			{
				message->get_to_url()->get_resources()->remove(1);
			}

			// Remove all the / that have added up.
			uint8_t item_count = message->get_to_url()->get_resources()->get_item_count();
			for(uint8_t i = 1 ; i < item_count; i++)
			{
				if(*((*message->get_to_url()->get_resources())[0]) == '\0')
				{
					message->get_to_url()->get_resources()->remove(0);
				}
			}

			message->get_to_url()->get_resources()->insert("", 0);


			// Allocate a buffer to hold the message.
			char* data = (char*)malloc(message->serialize(NULL, false));

			// Serialize the message as a string.
			message->serialize(data, true);

			_interface->receive(data); // Send the message to the other interface.

			delete message; // This message is no longer our responsibility.

			return 0; // Message was successfully sent.
		}
};

/**
 * Runs the framework.
 * @param steps the number of processing steps to execute.
 * @param processing1 a processing resource.
 * @param processing2 a processing resource.
 * */
void run_framework(uint32_t steps, Processing* processing1, Processing* processing2)
{
	for(uint32_t i = 0; i < steps ; i++)
	{
		processing1->step();

		if(processing2)
		{
			processing2->step();
		}

		increase_uptime(1);
	}
}

bool test_message_passing(void)
{
	bool error = false;

	std::cout << "*** testing message passing..." << std::endl;

	// Build the resource tree.
	Authority* root1 = new Authority();
	Processing* processing1 = new Processing(NULL);
	Resource* resource1_0 = new Resource();
	Resource* resource1_1 = new Resource();
	TestResource* test1 = new TestResource();
	TestInterface* interface1 = new TestInterface();

	resource1_0->add_child("resource1_1", resource1_1);
	root1->add_child("resource1_0", resource1_0);
	root1->add_child("test1", test1);
	root1->add_child("processing1", processing1);
	root1->add_child("interface1", interface1);

	//######################################################

	std::cout << "   > request to relative url ... ";

	if(test1->send_request("PUT ./../resource1_0/../resource1_0/././resource1_1 HTTP/1.1\r\n\r\n"))
	{
		run_framework(100, processing1, NULL);

		if(test1->last_response && test1->last_response->get_status_code() == NOT_IMPLEMENTED_501)
		{
			std::cout << "(done)" << std::endl;
		}
		else
		{
			error = true;
			std::cout << "(error)" << std::endl;
		}

		delete test1->last_response;
		test1->last_response = NULL;
	}
	else
	{
		error = true;
		std::cout << "(message not valid)" << std::endl;
	}

	//######################################################

	std::cout << "   > request to non existing resource ... ";

	if(test1->send_request("DELETE /resource1_0/resource1_1/resource1_2 HTTP/1.1\r\n\r\n"))
	{
		run_framework(100, processing1, NULL);

		if(test1->last_response && test1->last_response->get_status_code() == NOT_FOUND_404)
		{
			std::cout << "(done)" << std::endl;
		}
		else
		{
			error = true;
			std::cout << "(error)" << std::endl;
		}

		delete test1->last_response;
		test1->last_response = NULL;
	}
	else
	{
		error = true;
		std::cout << "(message not valid)" << std::endl;
	}

	//######################################################

	std::cout << "   > request to wrong resource name ... ";

	if(test1->send_request("DELETE /resource1_0/resource1_4 HTTP/1.1\r\n\r\n"))
	{
		run_framework(100, processing1, NULL);

		if(test1->last_response && test1->last_response->get_status_code() == NOT_FOUND_404)
		{
			std::cout << "(done)" << std::endl;
		}
		else
		{
			error = true;
			std::cout << "(error)" << std::endl;
		}

		delete test1->last_response;
		test1->last_response = NULL;
	}
	else
	{
		error = true;
		std::cout << "(message not valid)" << std::endl;
	}

	//######################################################

	std::cout << "   > request to existing resource ... ";

	if(test1->send_request("DELETE /resource1_0 HTTP/1.1\r\n\r\n"))
	{
		run_framework(100, processing1, NULL);

		if(test1->last_response && test1->last_response->get_status_code() == NOT_IMPLEMENTED_501)
		{
			std::cout << "(done)" << std::endl;
		}
		else
		{
			error = true;
			std::cout << "(error)" << std::endl;
		}

		delete test1->last_response;
		test1->last_response = NULL;
	}
	else
	{
		error = true;
		std::cout << "(message not valid)" << std::endl;
	}

	//######################################################

	std::cout << "   > request to self ... ";

	if(test1->send_request("POST . HTTP/1.1\r\n\r\n"))
	{
		if(test1->last_response && test1->last_response->get_status_code() == NOT_IMPLEMENTED_501)
		{
			std::cout << "(done)" << std::endl;
		}
		else
		{
			error = true;
			std::cout << "(error)" << std::endl;
		}

		delete test1->last_response;
		test1->last_response = NULL;
	}
	else
	{
		error = true;
		std::cout << "(message not valid)" << std::endl;
	}

	// Build another resource tree to communicate with.
	Authority* root2 = new Authority();
	Processing* processing2 = new Processing(NULL);
	Resource* resource2_0 = new Resource();
	Resource* resource2_1 = new Resource();
	TestResource* test2 = new TestResource();
	TestInterface* interface2 = new TestInterface();

	resource2_0->add_child("resource2_1", resource2_1);
	root2->add_child("resource2_0", resource2_0);
	root2->add_child("test2", test2);
	root2->add_child("processing2", processing2);
	root2->add_child("interface2", interface2);

	//######################################################

	std::cout << "   > request through a disconnected interface ... ";

	if(test1->send_request("POST /interface1/resource2_0 HTTP/1.1\r\n\r\n"))
	{
		run_framework(100, processing1, NULL);

		if(test1->last_response && test1->last_response->get_status_code() == NOT_FOUND_404)
		{
			std::cout << "(done)" << std::endl;
		}
		else
		{
			error = true;
			std::cout << "(error)" << std::endl;
		}

		delete test1->last_response;
		test1->last_response = NULL;
	}
	else
	{
		error = true;
		std::cout << "(message not valid)" << std::endl;
	}

	//######################################################

	std::cout << "   > request through a connected interface #1 ... ";

	// Connect the two interfaces.
	interface1->set_interface(interface2);
	interface2->set_interface(interface1);

	if(test1->send_request("GET /interface1/resource2_0 HTTP/1.1\r\n\r\n"))
	{
		run_framework(100, processing1, processing2);

		if(test1->last_response && test1->last_response->get_status_code() == NOT_IMPLEMENTED_501)
		{
			std::cout << "(done)" << std::endl;
		}
		else
		{
			error = true;
			std::cout << "(error)" << std::endl;
		}

		delete test1->last_response;
		test1->last_response = NULL;
	}
	else
	{
		error = true;
		std::cout << "(message not valid)" << std::endl;
	}

	//######################################################

	std::cout << "   > request through a connected interface #2 ... ";

	if(test1->send_request("GET /interface1/resource2_0/resource2_1 HTTP/1.1\r\n\r\n"))
	{
		run_framework(100, processing1, processing2);

		if(test1->last_response && test1->last_response->get_status_code() == NOT_IMPLEMENTED_501)
		{
			std::cout << "(done)" << std::endl;
		}
		else
		{
			error = true;
			std::cout << "(error)" << std::endl;
		}

		delete test1->last_response;
		test1->last_response = NULL;
	}
	else
	{
		error = true;
		std::cout << "(message not valid)" << std::endl;
	}

	//######################################################

	std::cout << "   > simultaneous requests through a connected interface ... ";

	test1->send_request("GET /interface1/resource2_0/resource2_1 HTTP/1.1\r\n\r\n");
	test2->send_request("GET /interface2/resource1_0/resource1_1 HTTP/1.1\r\n\r\n");

	run_framework(100, processing1, processing2);

	if(test1->last_response &&
		test1->last_response->get_status_code() == NOT_IMPLEMENTED_501 &&
		test2->last_response &&
		test2->last_response->get_status_code() == NOT_IMPLEMENTED_501
	)
	{
		std::cout << "(done)" << std::endl;
	}
	else
	{
		error = true;
		std::cout << "(error)" << std::endl;
	}

	delete test1->last_response;
	test1->last_response = NULL;
	delete test2->last_response;
	test2->last_response = NULL;


	delete root1;
	delete root2;

	std::cout << "*** tested message passing" << std::endl;

	return error;
}
