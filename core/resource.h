/* resource.h - Implements an abstract resource
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

#ifndef RESOURCE_H_
#define RESOURCE_H_

#include "message.h"
#include "response.h"
#include "request.h"
#include "../elements.h"
#include "../utils/utils.h"
#include <stdint.h>

class Resource
{

	private:
		Dictionary< Resource* >* children;
		volatile uptime_t own_sleep_clock;
		Resource* parent;
		friend class Processing;
		volatile uptime_t children_sleep_clock;
		uint8_t child_to_visit;

	public:
		Resource(void);
		#if RESOURCE_DESTRUCTION
			virtual ~Resource(void);
		#endif


		int8_t add_child(const char* name, Resource* child);
		Resource* remove_child(const char* name);
		uint8_t get_number_of_children(void);
		virtual uint8_t send(Message* message);

	protected:
		virtual void visit(void);
		virtual Message* dispatch(Message* message);
		virtual void run( void );
		virtual uptime_t get_sleep_clock( void );
        const char* get_name(Resource* resource);
        virtual Response::status_code process( Request* request, File** return_body, const char** mime );
        virtual Response::status_code process( Response* response );
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
        void print_transaction(Message* message);

	private:
        Resource* get_next_child_to_visit();
};

#endif /* RESOURCE_H_ */
