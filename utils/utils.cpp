/* utils.cpp - Implements a few basic data structures
 * Copyright (C) 2011 Antoine Mercier-Linteau
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#include <configuration.h>
#include <stdint.h>
#include <stdlib.h>
#include "utils.h"
#include <string.h>

GenericList::GenericList( void )
{
	items = 0;
	memset(list, NULL, CAPACITY * sizeof(void*));
}

int8_t GenericList::append(void* item)
{
	if(items == CAPACITY){ return 1; }
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
    if(items + 1 > CAPACITY)
    {
        return -2;
    }
    for(uint8_t i = items; i > position; i--)
    {
        list[i] = list[i - 1];
    }
    list[position] = item;
    items++;

    return 0;
}

void* GenericList::remove_item( void* item )
{
	for(uint8_t i = 0; i < items; i++)
	{
		if(list[i] == item)
		{
			list[i] = NULL;
			compact();
			items--;
			return item;
		}
	}


	return NULL;
}

void* GenericList::remove( uint8_t index )
{
	if(index >= items ) { return NULL; }
	void* item = list[index];
	list[index] = NULL;
	compact();
	items--;
	return item;
}


void GenericList::compact()
{
	for(uint8_t i = 0; i < CAPACITY - 1; i++)
	{
		if(list[i] != NULL){ continue; }
		list[i] = list[i + 1];
		list[i + 1] = NULL;
	}
}


void* GenericList::operator[](uint8_t i)
{
	if(!items || i >= items){ return NULL; }
	return list[i];
}


GenericDictionary::GenericDictionary( void )
{
	items = 0;
}


int8_t GenericDictionary::add( const char* key, void* value )
{
	if(items == CAPACITY){ return 1; }

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

void* GenericDictionary::remove( const char* key )
{
	key_value_pair<void*>* kv = get(key);
	if(kv == NULL){ return NULL; }
	void* value = kv->value;
	kv->value = NULL;
	kv->key = NULL;
	items--;
	compact();

	return value;
}

void* GenericDictionary::find( const char* key )
{
	key_value_pair<void*>* kv = get( key );
	if(kv == NULL){ return NULL; }
	return kv->value;
}


const char* GenericDictionary::find_val( void* value )
{
    for(uint8_t i = 0; i < items; i++)
    {
        if(list[i].value == value)
        {
            return list[i].key;
        }
    }

    return NULL;
}


key_value_pair<void*>* GenericDictionary::get( const char* key )
{
	const char* local_key;
	for(uint8_t i = 0; i < items; i++)
	{
		local_key = list[i].key;
		if(!strcmp(local_key, key))
		{
			return &list[i];
		}
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
	for(uint8_t i = 0; i < CAPACITY - 1; i++)
	{
		if(list[i].key != NULL){ continue; }
		list[i].key = list[i + 1].key;
		list[i].value = list[i + 1].value;
		list[i + 1].key = NULL;
		list[i + 1].value = NULL;
	}
}

GenericLinkedList::GenericLinkedList():
		start(NULL)
{}

uint8_t GenericLinkedList::items(void)
{
	uint8_t items = 0;
	entry* current = start;

	while(current != NULL)
	{
		items++;
		current = current->next;
	}

	return items;
}
int8_t GenericLinkedList::append( void* item )
{
	entry** next = &start;

	while(*next != NULL)
	{
		next = &(*next)->next;
	}

	*next = (entry*)malloc(sizeof(entry));

	if(!*next)
	{
		return 1;
	}

	(*next)->item = item;
	(*next)->next = NULL;

	return 0;
}
int8_t GenericLinkedList::insert(void* item, uint8_t position)
{
	entry* ent = (entry*)malloc(sizeof(entry));

	if(!ent)
	{
		return 1;
	}

	ent->item = item;

	if(position == 0)
	{
		ent->next = start;
		start = ent;
	}
	else
	{
		entry* temp = get(position - 1);
		if(!temp)
		{
			return 2;
		}
		ent->next = temp->next;
		temp->next = ent;
	}

	return 0;

}
void* GenericLinkedList::remove( uint8_t index )
{
	entry* ent;
	void* item;

	if(index == 0)
	{
		if(start)
		{
			ent = start;
			start = ent->next;
		}
		else
		{
			return NULL;
		}
	}
	else
	{
		entry* temp;
		ent = get(index - 1);

		if(!ent || !(ent->next))
		{
			return NULL;
		}
		temp = ent->next;
		ent->next = temp->next;
		ent = temp;
	}
	item = ent->item;
	free(ent);

	return item;
}

GenericLinkedList::entry* GenericLinkedList::get(uint8_t position)
{
	entry* current = start;
	while(  current )
	{
		current = current->next;
		if(!--position)
		{
			break;
		}
	}

	return current;
}

