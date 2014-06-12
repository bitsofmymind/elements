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
 * Header file for the Authority class.
 */

#ifndef AUTHORITY_H_
#define AUTHORITY_H_

//INCLUDES

#include <stdlib.h>
#include <utils/utils.h>
#include "message.h"
#include "response.h"
#include "request.h"
#include "resource.h"

///@todo delete that if it is useless.
#define MAX_NUMBER_OF_CHILD_AUTHORITIES 10

class Authority: public Resource
{
	private:
		/** The queue the authority uses to buffer its messages.
		 * Defined inline so the cost of memory allocation is saved.*/
		Queue<Message*> message_queue;
	public:
#if AUTHORITY_REDIRECT
		/** The redirect url when requests for this authority should get
		 * redirected. The content of this variable gets put in the Location
		 * header field. */
		const char* redirect_url;
#endif

		Authority(void);
#if RESOURCE_DESTRUCTION
		virtual ~Authority(void);
#endif

	protected:
		virtual Response::status_code process( Request* request, Response* response );
		virtual Response::status_code process( Response* response );
		virtual void run(void);
};

#endif /* AUTHORITY_H_ */
