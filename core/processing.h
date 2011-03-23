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
//#include "../elements.h"
#include "../utils/utils.h"

class Processing: public Resource
{
	Resource* bound;
	Resource* current;

	public:
		Processing(Resource* bound);

		virtual void start(void);
		void step(void);

	protected:


};


#endif /* PROCESSING_H_ */
