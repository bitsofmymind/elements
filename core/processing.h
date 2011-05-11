/*
 * authority.h
 *
 *  Created on: Feb 17, 2009
 *      Author: Antoine
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

		virtual void start(void);
		void step(void);

};


#endif /* PROCESSING_H_ */
