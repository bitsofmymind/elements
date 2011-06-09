/* processing.cpp - Implements a basic processing resource
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

#ifndef PROCESSING_H_
#define PROCESSING_H_

#include <stdlib.h>
#include "message.h"
#include "response.h"
#include "request.h"
#include "resource.h"
#include "../utils/utils.h"
#if PROCESSING_AS_AUTHORITY
#include "authority.h"
#endif
#if PROCESSING_AS_AUTHORITY
class Processing: public Authority
#else
class Processing: public Resource
#endif
{
	private:
		Resource* bound;
		Resource* current;
#if HEARTBEAT
		uptime_t heartbeat;
#endif

	public:
		Processing(Resource* bound);

		void start(void);
		void step(void);

};


#endif /* PROCESSING_H_ */
