/*
 * processing.h
 *
 *  Created on: Sep 21, 2009
 *      Author: Antoine
 */

#ifndef PROCESSING_H_
#define PROCESSING_H_

#include "resource.h"

class Processing: public Resource
{
	protected:
		Resource* bound;

	public:
		Processing(Resource* bound);
		Processing();


		virtual void start(void);

	protected:
		static Resource* step(Resource* current);

};


#endif /* PROCESSING_H_ */
