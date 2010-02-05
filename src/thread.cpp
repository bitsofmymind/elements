/*
 * thread.cpp
 *
 *  Created on: Jul 5, 2009
 *      Author: Antoine
 */


#include "thread.h"
#include "../elements/pal/pal.h"
#include "../elements/utils/types.h"
#include "../elements/core/representation.h"
#include <sys/types.h>
#include <time.h>
#include <iostream>
#include <stdlib.h>
using namespace std;

void PosixMainThread::processing_sleep(uint32_t milliseconds)
{
	usleep(milliseconds);
}

PosixThread::PosixThread():
	Processing(),
	disposing(false)
{
	id = (pthread_t)ids.items;

	for(uint8_t i = 0; i< ids.items; i++)
	{
		if(*ids[i] == id)
		{
			id++;
		}
	}
	ids.append(&id);

	start();
}

PosixThread::~PosixThread()
{
	void* return_value_ptr;
	disposing = true;
	switch(state)
	{
		case BUSY: pthread_join( id, &return_value_ptr ); break;
		case SLEEPING: pthread_cancel( id); break;
	}
	for(uint8_t i = 0; i < ids.items; i++)
	{
		if(*ids[i] == id)
		{
			ids.remove(i);
			break;
		}
	}
}

void PosixThread::start(void)
{
	if( pthread_create( &id, NULL, run_thread, this ))
	{
		cerr << "Error creating thread." << endl;
		return;
	}
}

void* PosixThread::run_thread( void* arg )
{
	PosixThread* thread = (PosixThread*)arg;
	thread->start();
	pthread_exit(NULL);
}

void PosixThread::processing_sleep(uint32_t milliseconds)
{
	usleep(milliseconds);
}

PosixThreadManager::PosixThreadManager( Root* root, uint32_t initial_number_of_threads)
{
	/*char temp_key_str[sizeof( "thread" ) + 10];
	char* key_str;
	PosixThread* new_thread;
	int key_size;

	for( uint32_t i = 0; i < initial_number_of_threads; i++ )
	{
		key_size = sprintf( temp_key_str, "thread%u", i );
		key_str = (char*)malloc(key_size);
		Elements::string<uint8_t> key = {key_str, key_size};
		strcpy( key_str, temp_key_str);

		new_thread = new PosixThread(root, i);

		threads.add(key, new_thread);
		children->add(key, new_thread);
	}*/
}

Elements::string<uint32_t> PosixThreadManager::render(Request* request)
{
	Elements::string<uint32_t> text = MAKE_STRING( \
			"<html>\
			<body>\
			<b>Thread Manager</b><br/>\
			Number of threads: {{number_of_threads}}<br/>\
			<ol>\
			{{list_of_threads}}\
			</ol>\
			<br/>\
			</body>\
			</html>");

	Representation representation = Representation(text);
	Dictionary<Elements::string<uint32_t> > content;

	char number_of_threads_content[2];
	number_of_threads_content[0] = (char)(threads.items + 48);
	number_of_threads_content[1] = '\0';
	Elements::string<uint32_t> number_of_threads = Elements::string<uint32_t>::make((char*)number_of_threads_content);
	Elements::string<uint8_t> number_of_threads_key = MAKE_STRING("number_of_threads");
	content.add(number_of_threads_key, &number_of_threads);

	for(uint32_t i=0; i< threads.items; i++)
	{

	}
	representation.set(&content);
	return representation.render();
}
