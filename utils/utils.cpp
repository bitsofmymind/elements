/*
 * utils.cpp
 *
 *  Created on: Mar 30, 2009
 *      Author: Antoine
 */


//#include "utils.h"
#include "../elements.h"
#include "types.h"
#include <stdint.h>
#include "utils.h"

using namespace Elements;

GenericList::GenericList( void )
{
	capacity = DEFAULT_INITIAL_CAPACITY;
	list = new void*[DEFAULT_INITIAL_CAPACITY];
	items = 0;
}

GenericList::~GenericList( void )
{
	delete list;
}

int8_t GenericList::append(void* item)
{
	if(items == capacity){ return 1; }
	list[items] = item;
	items++;
	return 0;
}

int8_t GenericList::insert(void* item, uint8_t position)
{
    if(position > items)
    {
        return -1;
    }
    if(items + 1 > capacity)
    {
        return -2;
    }
    for(uint8_t i = items; i > position; i--)
    {
        list[i+1] = list[i];
    }
    list[position] = item;
    items++;
}

void* GenericList::remove( uint8_t index )
{
	if(index > capacity) { return NULL; }
	void* item = list[index];
	list[index] = NULL;
	compact();
	items--;
	return item;
}


void GenericList::compact()
{
	for(uint8_t i = 0; i < capacity - 1; i++)
	{
		if(list[i] != NULL){ continue; }
		list[i] = list[i + 1];
		list[i + 1] = NULL;
	}
}


void* GenericList::operator[](uint8_t i)
{
	if(i >= items){ return NULL; }
	return list[i];
}


GenericDictionary::GenericDictionary( void )
{
	capacity = DEFAULT_INITIAL_CAPACITY;
	items = 0;
}


int8_t GenericDictionary::add( string< uint8_t > key, void* value)
{
	if(items == capacity){ return 1; }

	key_value_pair<void*>* kv = get(key);
	if( kv == NULL)
	{
		list[items].key = key;
		list[items].value = value;
		items++;
	}
	else{
		kv->value = value;}
	return 0;
}


void* GenericDictionary::remove( string< uint8_t >& key )
{
	key_value_pair<void*>* kv = get(key);
	if(kv == NULL){ return NULL; }
	void* value = kv->value;
	kv->value = NULL;
	kv->key.text = NULL;
	items--;
	compact();

	return value;
}

void* GenericDictionary::find( string< uint8_t > key ) //&
{
	key_value_pair<void*>* kv = get( key );
	if(kv == NULL){ return NULL; }
	return kv->value;
}

Elements::string< uint8_t >* GenericDictionary::find( void* value )
{
    for(uint8_t i = 0; i < items; i++)
    {
        if(list[i].value == value)
        {
            return &list[i].key;
        }
    }

    return NULL;
}


key_value_pair<void*>* GenericDictionary::get( string< uint8_t > key )//&
{
	for(uint8_t i = 0; i < items; i++)
	{
		if( list[i].key.length != key.length ) { continue; }
		if(list[i].key.text == NULL){ break; }

		for(uint8_t j = 0; j < key.length ; j++)
		{
			if(list[i].key.text[j] != key.text[j])
			{
				return NULL;
			}
		}
		return &list[i];
	}
	return NULL;
}

key_value_pair<void*>* GenericDictionary::operator[](uint8_t i)
{
	if( i > items ) { return NULL; }
	return &(list[i]);
}

void GenericDictionary::compact()
{
	for(uint8_t i = 0; i < capacity - 1; i++)
	{
		if(list[i].key.text != NULL){ continue; }
		list[i].key = list[i + 1].key;
		list[i].value = list[i + 1].value;
		list[i + 1].key.text = NULL;
		list[i + 1].value = NULL;
	}
}

GenericDynamicDictionary::GenericDynamicDictionary( void )
{
	capacity = DEFAULT_INITIAL_CAPACITY;
	items = 0;
}


int8_t GenericDynamicDictionary::add( string< uint8_t > key, void* value)
{
	if(items == capacity){ return 1; }

	key_value_pair<void*>* kv = get(key);
	if( kv == NULL)
	{
		list[items].key = key;
		list[items].value = value;
		items++;
	}
	else{
		kv->value = value;}
	return 0;
}


void* GenericDynamicDictionary::remove( string< uint8_t >& key )
{
	key_value_pair<void*>* kv = get(key);
	if(kv == NULL){ return NULL; }
	void* value = kv->value;
	kv->value = NULL;
	kv->key.text = NULL;
	items--;
	compact();

	return value;
}

void* GenericDynamicDictionary::find( string< uint8_t > key ) //&
{
	key_value_pair<void*>* kv = get( key );
	if(kv == NULL){ return NULL; }
	return &kv->value;
}


key_value_pair<void*> * GenericDynamicDictionary::get( string< uint8_t > key )//&
{
	for(uint8_t i = 0; i < items; i++)
	{
		if( list[i].key.length != key.length ) { continue; }
		if(list[i].key.text == NULL){ break; }

		for(uint8_t j = 0; j < key.length ; j++)
		{
			if(list[i].key.text[j] != key.text[j])
			{
				return NULL;
			}
		}
		return &list[i];
	}
	return NULL;
}

key_value_pair<void*>* GenericDynamicDictionary::operator[](uint8_t i)
{
	if( i > items ) { return NULL; }
	return &(list[i]);
}

void GenericDynamicDictionary::compact()
{
	for(uint8_t i = 0; i < capacity - 1; i++)
	{
		if(list[i].key.text != NULL){ continue; }
		list[i].key = list[i + 1].key;
		list[i].value = list[i + 1].value;
		list[i + 1].key.text = NULL;
		list[i + 1].value = NULL;
	}
}

GenericQueue::GenericQueue( void )
{
	start = 0;
	capacity = DEFAULT_INITIAL_CAPACITY;
	items = 0;
}

void* GenericQueue::operator[]( uint8_t index )
{
	if( index > items ){ return NULL; }
	if( index + start > capacity ){ return list[ start + index - capacity ]; }
	return list[ start + index ];
}

int8_t GenericQueue::queue(void* object)
{
	if(items == capacity){ return 1; }
	if(start + items > capacity){ list[start + items - capacity] = object;}
	else { list[start + items] = object; }
	items++;
	return 0;
}

void* GenericQueue::dequeue(void)
{
	if(items == 0) { return NULL; }
	items--;
	if(start == capacity)
	{
		start = 0;
		return list[capacity];
	}
	start++;
	return list[start - 1];
}


