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
#include <stdint.h>

class Representation
{
	protected:
		typedef struct tag
		{
			string< uint8_t > name;
			string< uint32_t > content;
			char* position;
			uint32_t length;
		};
		string< uint32_t > rendered_representation;
		const string< uint32_t > template_data;
		List< tag > tags;

	public:
		Representation( const string< uint32_t > );
		virtual ~Representation();

		virtual void set( Dictionary< string <uint32_t> >* );
		virtual string< uint32_t> render( void );
		virtual string< uint32_t > render( char* start, char* stop );
	protected:
		virtual void parse( void );
};



#endif /* REPRESENTATION_H_ */
