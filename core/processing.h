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
 * Header file for the Processing class.
 */

#ifndef PROCESSING_H_
#define PROCESSING_H_

//INCLUDES
#include <utils/utils.h>
#include <stdlib.h>
#include "message.h"
#include "response.h"
#include "request.h"
#include "resource.h"
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
		/** Processing will stop going up the resource tree when it
		 * reaches that Resource pointer. Set to NULL if processing should
		 * go up to the root resource.*/
		Resource* bound;
		/// The current resource being processed.
		Resource* current;
#if HEARTBEAT
		/// The last time a heartbeat was done.
		uptime_t heartbeat;
#endif

	public:
		Processing(Resource* bound);

		void start(void);
		void step(void);
};


#endif /* PROCESSING_H_ */
