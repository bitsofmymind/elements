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
	items = 0; //todo/ Move to initialization list.
	// Sets all the positions in the list to NULL.
	memset(list, NULL, CAPACITY * sizeof(void*));
}

int8_t GenericList::append( void* item )
{
	if(items >= CAPACITY) // If there is no more space in the list.
	{
		return 1; // Return an error.
	}

	if(item == NULL) // If a NULL value is being inserted.
	{
		return 2; // Return an error.
	}

	list[items] = item; // Inserts the item at the end of the list.
	items++; // We have added an item.

	return 0; // Success.
}

int8_t GenericList::insert( void* item, uint8_t position )
{
    if(position > items) // If position goes pas the number of items.
    {
        return -1; // Return an error.
    }

    if(items >= CAPACITY) // If the list is full.
    {
        return -2; // Return an error.
    }

    if(item == NULL) // If a NULL value is being inserted.
    {
    	return -3; // Item is illegal.
    }

    // For each item in the list from the end to the desired position..
    for(uint8_t i = items; i > position; i--)
    {
    	list[i] = list[i - 1]; // Shift the item to the right.
    }

    list[position] = item; // Inserts the item in the desired position.
    items++; // We have added an item to the list.

    return 0; // Success.
}

void* GenericList::remove_item( void* item )
{
	// For each item in the list.
	for(uint8_t i = 0; i < items; i++)
	{
		if(list[i] == item) // If this is the item that is supposed to be removed.
		{
			list[i] = NULL; // Blank the item.
			compact(); // Compact the list.
			items--; // An item was removed.

			return item; // Success.
		}
	}

	return NULL; // The item was not found.
}

void* GenericList::remove( uint8_t index )
{
	if(index >= items ) // If the index is invalid.
	{
		return NULL; // Item not found.
	}

	void* item = list[index]; // Retrieve the item.
	list[index] = NULL; // Blank the list at this index.
	compact(); // Compact the list.
	items--; // An item was removed.

	return item; // Success/
}


void GenericList::compact()
{
	// For each item in the list until the end.
	for(uint8_t i = 0; i < CAPACITY - 1; i++)
	{
		if(list[i] != NULL) // If there is an item at this position.
		{
			continue; // Skip it.
		}
		// Else there is no item at this position.

		list[i] = list[i + 1]; // Move the next item to that position.

		list[i + 1] = NULL; // Blank the next position.
	}
}


void* GenericList::operator[]( uint8_t i )
{
	// If there are no items or an index greater that the amount of item is requested.
	if(!items || i >= items)
	{
		return NULL; // Error.
	}

	return list[i]; // Return the item at the requested index.
}


GenericDictionary::GenericDictionary( void )
{
	items = 0; ///todo move to initialization list.
}


int8_t GenericDictionary::add( const char* key, void* value )
{
	if(items >= CAPACITY) // If there is no more space in the dictionary.
	{
		return 1;
	}

	// Check if the entry already exists.
	key_value_pair<void*>* kv = get(key);

	if( kv == NULL) // If the entry does not exist.
	{
		// Instantiate an entry.
		list[items].key = key;
		list[items].value = value;
		items++; // An entry was added.
	}
	else // The entry already exists.
	{
		/**todo thats dangerous and could lead to memory leaks. A method
		 * for replacing values should be created instead.*/
		kv->value = value;
	}

	return 0; // Success.
}

void* GenericDictionary::remove( const char* key )
{
	key_value_pair<void*>* kv = get(key); // Finds the entry.

	if(kv == NULL) // If the entry does not exist.
	{
		return NULL;
	}

	void* value = kv->value; // Saves the value of the entry.
	kv->value = NULL; // Blank the value.
	kv->key = NULL; // Blank the key.
	items--; // An entry was removed.
	compact(); // Compacts the storage array.

	return value; // Success
}

void* GenericDictionary::find( const char* key )
{
	key_value_pair<void*>* kv = get(key); // Finds the entry.

	if(kv == NULL) // If the entry was not found.
	{
		return NULL;
	}

	return kv->value; // Return the value.
}

const char* GenericDictionary::find_val( void* value )
{
    for(uint8_t i = 0; i < items; i++) // For each entry.
    {
        if(list[i].value == value) // If the value matches.
        {
            return list[i].key; // Return the key.
        }
    }

    return NULL; // The value was not found.
}

key_value_pair<void*>* GenericDictionary::get( const char* key )
{
	for(uint8_t i = 0; i < items; i++) // For each entry in the dictionary.
	{
		if(!strcmp(list[i].key, key)) // If the two keys match.
		{
			return &list[i]; // Return the key_value pair.
		}
	}

	return NULL; // The entry was not found.
}

key_value_pair<void*>* GenericDictionary::operator[]( uint8_t i )
{
	if(i > items) // If the index is invald.
	{
		return NULL;
	}

	return &(list[i]);
}

void GenericDictionary::compact()
{
	// For each entry in the dictionary.
	for(uint8_t i = 0; i < CAPACITY - 1; i++)
	{
		if(list[i].key != NULL) // If there is an item at this index.
		{
			continue;
		}

		// Shift the next item to the current index.
		list[i].key = list[i + 1].key;
		list[i].value = list[i + 1].value;
		list[i + 1].key = NULL;
		list[i + 1].value = NULL;
	}
}

GenericLinkedList::GenericLinkedList():
	start(NULL)
{}

uint8_t GenericLinkedList::items( void )
{
	uint8_t items = 0;
	entry* current = start;

	while(current != NULL) // While there are items in the list.
	{
		items++; // One more item.
		current = current->next; // Go to the next item.
	}

	return items; // Returns the number of items.
}

int8_t GenericLinkedList::append( void* item )
{
	///todo I don't think we need double pointers here.

	entry** next = &start;

	 // Go to the end of the list by hoping from one item to the next.
	while(*next != NULL)
	{
		next = &(*next)->next; // Go to the next item.
	}

	// The end of the list has been reached, add a new entry.
	///todo should be ts_malloc.
	*next = (entry*)malloc(sizeof(entry));

	if(!*next) // If no memory could be allocated for the new entry.
	{
		return 1; // Return an error.
	}

	// Initialize the entry.
	(*next)->item = item;
	(*next)->next = NULL;

	return 0; // Success.
}

int8_t GenericLinkedList::insert( void* item, uint8_t position )
{
	// Allocate a new entry.
	///todo should be ts_malloc.
	entry* ent = (entry*)malloc(sizeof(entry));

	if(!ent) // If the new entry could not be allocated.
	{
		return 1; // Return an error.
	}

	ent->item = item; // Sets the item on the new entry.

	if(position == 0) // If the entry is to be added at the beginning of the list.
	{
		ent->next = start; // Next entry will be the old start.
		start = ent; // The new start of the list is the new entry.
	}
	else // The item
	{
		entry* temp = get(position - 1); // Get the item before the position we want.

		if(!temp) // If the position is invalid.
		{
			return 2; // Return an error.
		}

		// Adds the new entry at the requested position.
		ent->next = temp->next;
		temp->next = ent;
	}

	return 0; // Success.

}

void* GenericLinkedList::remove( uint8_t index )
{
	entry* ent;
	void* item;

	if(index == 0) // If the first entry is to be removed.
	{
		if(start) // If there is a first item.
		{
			// Remove it.
			ent = start;
			start = ent->next;
		}
		else
		{
			return NULL; // The linked list is empty.
		}
	}
	else // Another entry has to be removed.
	{
		entry* temp;
		// Gets the entry before the entry that has to be removed.
		ent = get(index - 1);

		if(!ent || !(ent->next)) // If the index is invalid.
		{
			return NULL; // No entry was removed.
		}

		// Removes the entry.
		temp = ent->next;
		ent->next = temp->next;
		ent = temp;
	}

	item = ent->item; // Save the item.
	///todo use ts_free.
	free(ent); // Free the entry.

	return item; // Success.
}

GenericLinkedList::entry* GenericLinkedList::get( uint8_t position )
{
	entry* current = start;

	while(current) // While there are entries in the list.
	{
		// If this is the wanted position or the end of the list has been reached.
		if(position == 0 || current == NULL)
		{
			break; // Done.
		}

		current = current->next; // Go to the next entry.

		position--; // We are one nearer the wanted position.
	}

	return current;
}

