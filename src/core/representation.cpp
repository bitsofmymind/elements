/*
 * representation.cpp
 *
 *  Created on: Jun 30, 2009
 *      Author: Antoine
 */

#include <core/representation.h>
#include <stdlib.h>

Representation::Representation( const string< uint32_t > template_data  ):
	template_data(template_data)
{
	parse();
}

Representation::~Representation()
{
	while( tags.items != 0 )
	{
		//free( tags[0]->content.text );
		free( tags.remove( 0 ) );
	}
}

void Representation::parse( void )
{
	char* start;
	bool at_tag = false;
	for( char* i = template_data.text ; i< template_data.text + template_data.length; i++ )
	{
		if( *i == '{' && *(i + 1) == '{' )
		{
			start = i;
			at_tag = true;

		}
		else if( *i == '}' && *(i + 1) == '}' && at_tag )
		{
			at_tag = false;
			tag* t = (tag*)( malloc ( sizeof( tag ) ) );

			t->name.text = start;
			t->name.length = i - start - 2;
			t->position = start;
			t->length = i - start;
			tags.append( t );
		}
	}
}

void Representation::set( Dictionary< string <uint32_t> >* rendering_arguments )
{
	string< uint32_t >* content;

	for( uint8_t i = 0; i < tags.items; i++)
	{
		content = rendering_arguments->find( tags[i]->name );
		if( content != NULL )
		{
			tags[i]->content = *content;
		}
	}
}

string< uint32_t> Representation::render( void )
{
	if( rendered_representation.text != NULL )
	{
		free( rendered_representation.text );
	}
	else
	{
		/*useless to render the full template here, maybe we can just do an update
		or check if it was modified ?*/
	}

	uint32_t length = template_data.length;

	for( uint8_t i = 0; i < tags.items; i++)
	{
		length -= tags[i]->length;
		length += tags[i]->content.length;
	}

	rendered_representation.text = (char*)( malloc( length ) );

	if( rendered_representation.text == NULL )
	{
		rendered_representation.length = 0;
		return rendered_representation;
	}

	rendered_representation.length = length;

	char* next_stop = tags[0]->position;
	unsigned char tag = 0;

	for( char* i = rendered_representation.text,
			*j = template_data.text;
	j < template_data.text + template_data.length;
	j++)
	{
		if( j == next_stop )
		{
			for( char* c = tags[tag]->content.text;
			c < tags[tag]->content.text + tags[tag]->content.length;
			c++ )
			{
				*i++ = *c;
			}
			j += tags[tag]->length;
			if(tag < tags.items - 1)
			{
				next_stop = tags[++tag]->position;
			}
		}
		else
		{
			*i++ = *j;
		}
	}

	return rendered_representation;

}

string< uint32_t> Representation::render( char* start, char* stop )
{

	free( rendered_representation.text );
	rendered_representation.text = (char*)( malloc( stop - start ) );

	if( rendered_representation.text == NULL )
	{
		rendered_representation.length = 0;
		return rendered_representation;
	}

	rendered_representation.length = stop - start;

	unsigned char tag = 0;
	int  content_delta = 0; //risk of overflow/underflow if delta is too great
	uint32_t offset = 0;
	for( ; tag < tags.items ; tag++)
	{
		if( start - content_delta <= tags[tag]->position )
		{
			break;
		}
		else if( start - content_delta < tags[tag]->position + tags[tag]->content.length )
		{
			offset = tags[tag]->position + tags[tag]->content.length - start - content_delta;
			start = tags[tag]->position - offset;
			break;
		}
		content_delta -= tags[tag]->length;
		content_delta += tags[tag]->content.length;
	}

	char* next_stop;

	if( tag <= tags.items )
	{
		next_stop = tags[tag]->position;
	}
	else
	{
		next_stop = template_data.text + template_data.length;
	}

	/*char* template_ptr = template_data.text + (start - content_delta);


	for( char* index = rendered_representation.text + start + offset; ; template_ptr++)
	{
		if( template_ptr == next_stop )
		{
			if( tag >= tags.items )
			{
				break;
			}
			for( char* c = tags[tag]->content.text + offset; c< tags[tag]->content.length; c++ )
			{
				*index=*c;
			}
			j += tags[tag]->length;
			tag++;
			if( tag < tags.items )
			{
				next_stop = tags[tag]->position;
			}
		}
		else
		{
			*index = *template_ptr;
		}
	}*/

	return rendered_representation;
}
