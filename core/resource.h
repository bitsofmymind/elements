/* resource.h - Header file for the Resource class.
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

#ifndef RESOURCE_H_
#define RESOURCE_H_

//INCLUDES
#include <configuration.h>
#include <utils/utils.h>
#include <stdint.h>
#include "message.h"
#include "response.h"
#include "request.h"

/// Resource implements a Elements resource for use with HTTP messages.
/** Resource is the cornerstone of the Elements framework. Resources
 * are responsible for shaping the resource tree, processing messages and doing
 * periodic processing, Implementations of this framework will necessarily
 * mainly be done by adapting this class'behavior through inheritance.
 */
class Resource
{
	private:

		///The children of this resource. NULL if there are no children.
		Dictionary< Resource* >* _children;

		/**
		 * This resource's sleep clock. Used to tell the next uptime the run()
		 * method will be called. This attribute is declared volatile because it
		 * may be modified asynchronously.
		 * */
		volatile uptime_t _own_sleep_clock;

		/** This resource's children sleep clock. This clock tells the next
		 * uptime this a child will need to be run hence, it is always set
		 * to the lowest sleep clock of all the children. This attribute is
		 * declared volative because it may be modified asynchronously.
		 * */
		volatile uptime_t _children_sleep_clock;

		///This resource's parent. NULL if the resource has no parent.
		Resource* _parent;

		/**
		 * Since Processing needs access to attributes that are normally private
		 * , it is declared a friend.
		 * */
		friend class Processing;

		/**
		 * The index of the child to visit the next time processing visits
		 * this resource.
		 * */
		uint8_t _child_to_visit;

	public:

		/// Class constructor.
		Resource(void);

#if RESOURCE_DESTRUCTION
		/// Class destructor.
		virtual ~Resource(void);
#endif

		/// Adds a Resource as a child.
		/**
		 * @param name the name of the child.
		 * @param child the child to add.
		 * @return 0 if adding was successful, another value if an error occurred.
		 * */
		int8_t add_child(const char* name, Resource* child);

		/// Removes a child resource.
		/**
		 * @param name the name of the child.
		 * @return NULL if removing failed or the children if it was successful.
		 * */
		Resource* remove_child(const char* name);

		/**
		 * @return the number of children of this resource.
		 * */
		uint8_t get_number_of_children(void);

		/// Gets the name of child resource.
		/** @param resource the child resource we want the name of.
		 * @return the name of the child resource. NULL if the resource was not found.
		 */
        const char* get_name(const Resource* resource) const;

        /** @return the parent of this resource. */
        inline const Resource* get_parent(void) const { return _parent; };

	protected:

		/// Dispatches a message up or down the resource tree.
		/**
		 * dispatch() is one of the core methods of the elements framework. When a
		 * message transit in the tree, it does so through this method. dispatch()
		 * will look at a message's origin and destination, hand it to the Resource's
		 * processing methods for processing or interception or return the appropriate
		 * error response if a message is lost.
		 *
		 * dispatch() works recursively amongst resources but is stopped when it
		 * reaches an Authority because the latter buffers messages.
		 * @param message the message to dispatch.
		 * @see process()
		 */
		void dispatch(Message* message);

		/// Lets the resource do some processing.
		/**
		 * The run method is used for implementing periodic processing for a resource.
		 * It is automatically called once the own_sleep_clock timer has expired.
		 * Depending on the implementation on the framework, the interval at which this
		 * method is called might be more than what was request. It will, however, never
		 * be less.
		 * This method is meant to be overridden in order to specialize this class.
		 * @see own_sleep_clock.
		 * */
		virtual void run(void);

		/// Get the sleep timer of this resource.
		/**
		 * When asked by the next uptime a resource is to be ran, it can either be
		 * its own internal sleep clock or one of its children, whichever is less.
		 * This value will determine when the Resource is next visited by processing.
		 * @see Processing::start()
		 * @return children_sleep_clock or own_sleep_clock, whichever is less.
		 * */
		uptime_t get_sleep_clock(void);

        /// Process a request message.
        /**
         * YOU SHOULD NOT DELETE OR FREE THE REQUEST ARGUMENT!
         * This method is meant to be overridden in order to specialize this class.
         * The reason it is given a response is to save on code space by doing away
         * by moving the same response initialization routine out of this method.
         * @param request the request to process.
         * @param response the response to fill if a response should be returned (which
         * depends on the status code).
         * @return the status_code produced while processing the request.
         */
        virtual Response::status_code process(const Request* request, Response* response);

        /// Process a response message.
        /**
         * YOU SHOULD NOT DELETE OR FREE THE RESPONSE ARGUMENT!
         * This method is meant to be overridden in order to specialize this class.
         * @param response the response to process.
         * @return the status_code produced while processing the response.
         */
        virtual Response::status_code process(const Response* response);

#if HTTP_GET
        Response* http_get(Request* request);
#endif
#if HTTP_HEAD
		Response* http_head(Request* request);
#endif
#if HTTP_TRACE
		Response* http_trace(Request* request);
#endif

		/// Schedules the resource to run.
		/**
		 * @param time the interval for which the resource should sleep.
		 * */
        void schedule(uptime_t time);

        ///Prints the content of a message.
        /**
         *  @param message the message to print.
         * */
        void print_transaction(const Message* message);

	private:

        /// Get the next child to visit.
        /**
         * This method is called by Processing::step() to get the next child that it
         * should visit.
         * @return the next child to visit; NULL if there is none.
         * @see Processing::step()
         * */
        Resource* _get_next_child_to_visit();

		/// Schedule a timer to a certain time
		/**
		 * And propagate that timer up the resource tree.
		 * This method may be called by an interrupt.
		 * @todo something is fucked up with this method...
		 * @param timer the timer to update.
		 * @param time the time to add to the timer.
		 * */
		void _schedule(volatile uptime_t* timer, uptime_t time);
};

#endif /* RESOURCE_H_ */
