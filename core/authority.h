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
