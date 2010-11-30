/*
 * representation.h
 *
 *  Created on: Jun 30, 2009
 *      Author: Antoine
 */

#ifndef REPRESENTATION_H_
#define REPRESENTATION_H_

#include "../utils/types.h"
#include "../utils/utils.h"
#include "../elements.h"
#include <stdint.h>

class Representation
{
	protected:
		struct tag
		{
			string< uint8_t > name;
			string< MESSAGE_SIZE > content;
			char* position;
			MESSAGE_SIZE length;
		};
		string< MESSAGE_SIZE > rendered_representation;
		const string< MESSAGE_SIZE > template_data;
		List< tag > tags;

	public:
		Representation( const string< MESSAGE_SIZE > );
		virtual ~Representation();

		virtual void set( Dictionary< string <MESSAGE_SIZE> >* );
		virtual string<MESSAGE_SIZE> render( void );
		virtual string<MESSAGE_SIZE> render( char* start, char* stop );
	protected:
		virtual void parse( void );
};



#endif /* REPRESENTATION_H_ */
