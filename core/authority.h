/* authority.h - Implements a basic authority resource
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

#ifndef AUTHORITY_H_
#define AUTHORITY_H_

#include <stdlib.h>
#include "message.h"
#include "response.h"
#include "request.h"
#include "resource.h"
#include "../utils/utils.h"

#define MAX_NUMBER_OF_CHILD_AUTHORITIES 10

class Authority: public Resource
{

	private:
		Queue<Message*> message_queue;

	public:
		Authority(void);
#if RESOURCE_DESTRUCTION
		virtual ~Authority(void);
#endif
        virtual uint8_t send(Message* message);

	protected:
		virtual void visit(void);
		virtual Message* dispatch(Message* message);
        virtual uptime_t get_sleep_clock( void );
		void process_queue(void);
};


#endif /* AUTHORITY_H_ */
