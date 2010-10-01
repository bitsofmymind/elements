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

    protected:
		Dictionary< Resource >* children;
		Elements::e_time_t own_sleep_clock;
                Resource* parent;

	private:
		friend class Processing;

		Elements::e_time_t children_sleep_clock;
		Elements::e_time_t buffer_children_sleep_clock;
		volatile bool visiting_children;
		uint8_t child_to_visit;

	public:
		#ifdef DEBUG
			char* id;
		#endif

	public:
		Resource(void);
		virtual ~Resource(void);


		int8_t add_child(Elements::string<uint8_t> name, Resource* child);
		Resource* remove_child(Elements::string<uint8_t> name);
		uint8_t get_number_of_children(void);

	public: //:protected
		virtual void visit(void);
		virtual Message* dispatch(Message* message);
		virtual void run( void );
        virtual uint8_t send(Message* message);

		virtual Elements::e_time_t get_sleep_clock( void );
        string<uint8_t>* get_name(Resource* resource);

		Resource* find_resource( Elements::string<uint8_t>* name );

		virtual Response* process( Request* request );
		virtual Message* process( Response* response );

		virtual Response* http_get(Request* request);
		Response* http_head(Request* request);
		Response* http_trace(Request* request);

		void schedule(Elements::e_time_t* timer, Elements::e_time_t time );
        void schedule(Elements::e_time_t time);

		virtual Elements::string<uint32_t> render( Request* request );

		virtual Resource* get_next_child_to_visit();
		bool update_children_sleep_clock(e_time_t time);

};

#endif /* RESOURCE_H_ */
