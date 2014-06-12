// SVN FILE: $Id: $
/**
 * @lastChangedBy           $lastChangedBy: Mercier $
 * @revision                $Revision: 397 $
 * @copyright    			GNU General Public License
 * 		This program is free software: you can redistribute it and/or modify
 * 		it under the terms of the GNU General Public License as published by
 * 		the Free Software Foundation, either version 3 of the License, or
 * 		(at your option) any later version.
 * 		This program is distributed in the hope that it will be useful,
 * 		but WITHOUT ANY WARRANTY; without even the implied warranty of
 *		MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * 		GNU General Public License for more details.
 * 		You should have received a copy of the GNU General Public License
 * 		along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * Header file for the Resource class.
 */

#ifndef RESOURCE_H_
#define RESOURCE_H_

//INCLUDES
#include <configuration.h>
#include <utils/utils.h>
#include <stdint.h>
#include "message.h"
#include "response.h"
#include "request.h"

class Resource
{
	private:
		///The children of this resource. NULL if there are no children.
		Dictionary< Resource* >* children;
		/** This resource's sleep clock. Used to tell the next uptime the run()
		 * method will be called. This attribute is declared volatile because it
		 * may be modified asynchronously.*/
		volatile uptime_t own_sleep_clock;
		/** This resource's children sleep clock. This clock tells the next
		 * uptime this a child will need to be run hence, it is always set
		 * to the lowest sleep clock of all the children. This attribute is
		 * declared volative because it may be modified asynchronously.*/
		volatile uptime_t children_sleep_clock;
		///This resource's parent. NULL if the resource has not parent.
		Resource* parent;
		/**Since Processing needs access to attributes that are normally private
		 * , it is declared a friend.*/
		friend class Processing;
		/** The index of the child to visit the next time processing visits
		 * this resource. */
		uint8_t child_to_visit;

	public:
		Resource(void);
#if RESOURCE_DESTRUCTION
		virtual ~Resource(void);
#endif

		int8_t add_child(const char* name, Resource* child);
		Resource* remove_child(const char* name);
		uint8_t get_number_of_children(void);

	protected:
		void dispatch(Message* message);
		virtual void run( void );
		uptime_t get_sleep_clock( void );
        const char* get_name(Resource* resource);
        virtual Response::status_code process( Request* request, Response* response );
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
