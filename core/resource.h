/*
 * resource.h
 *
 *  Created on: Feb 17, 2009
 *      Author: Antoine
 */

#ifndef RESOURCE_H_
#define RESOURCE_H_

#include "message.h"
#include "response.h"
#include "request.h"
//#include "representation.h"
#include "../elements.h"
#include "../utils/utils.h"
#include "../utils/types.h"
#include <stdint.h>

class Resource
{

    public:
		Dictionary< Resource >* children;
		volatile uptime_t own_sleep_clock;
		Resource* parent;

	private:
		friend class Processing;

		volatile uptime_t children_sleep_clock;
		uint8_t child_to_visit;

	public:
		Resource(void);
		#ifndef NO_RESOURCE_DESTRUCTION
			virtual ~Resource(void);
		#endif


		int8_t add_child(Elements::string<uint8_t> name, Resource* child);
		Resource* remove_child(Elements::string<uint8_t> name);
		uint8_t get_number_of_children(void);

	public: //:protected
		virtual void visit(void);
		virtual Message* dispatch(Message* message);
		virtual void run( void );
        virtual uint8_t send(Message* message);

		virtual uptime_t get_sleep_clock( void );
        string<uint8_t>* get_name(Resource* resource);

        virtual Response::status_code process( Request* request, Message** return_message );
        virtual Response::status_code process( Response* response, Message** return_message );

#if HTTP_GET
        Response* http_get(Request* request);
#endif
#if HTTP_HEAD
		Response* http_head(Request* request);
#endif
#if HTTP_TRACE
		Response* http_trace(Request* request);
#endif

		void schedule(volatile uptime_t* timer, uptime_t time );
        void schedule(uptime_t time);

		virtual File<MESSAGE_SIZE>* render( Request* request );

		Resource* get_next_child_to_visit();

	protected:
        Response* error(uint16_t error, Message* message);
        void print_transaction(Message* message);
};

#endif /* RESOURCE_H_ */
