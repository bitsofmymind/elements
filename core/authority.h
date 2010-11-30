/*
 * authority.h
 *
 *  Created on: Feb 17, 2009
 *      Author: Antoine
 */

#ifndef AUTHORITY_H_
#define AUTHORITY_H_

#include <stdlib.h>
#include "message.h"
#include "response.h"
#include "request.h"
#include "resource.h"
//#include "../elements.h"
#include "../utils/utils.h"
#include "../utils/types.h"

#define MAX_NUMBER_OF_CHILD_AUTHORITIES 10

class Authority: public Resource
{

	public:
		Queue<Message>* message_queue;

	public:
		Authority(void);
		#ifndef NO_RESOURCE_DESTRUCTION
			virtual ~Authority(void);
		#endif

	protected:
		virtual void visit(void);
		virtual Message* dispatch(Message* message);
        virtual uint8_t send(Message* message);

        virtual uint64_t get_sleep_clock( void );

		virtual void process_queue(void);
};


#endif /* AUTHORITY_H_ */
