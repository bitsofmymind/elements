/* utils.cpp - Source file for the framework's basic data structures.
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

using namespace Utils;

GenericList::GenericList(void):
	items(0)
{

#ifndef STATIC_LIST
	list = (void**)malloc(CAPACITY * sizeof(void**)); // Allocate space for the list.

	if(!list) // If the list could not be allocated.
	{
		capacity = 0;
		return;
	}

	capacity = CAPACITY;
#endif

	// Sets all the positions in the list to NULL.
	///TODO use calloc instead.
	memset(list, 0, CAPACITY * sizeof(void*));
}

GenericList::~GenericList()
{

#ifndef STATIC_LIST
	free(list);
#endif

}

OPERATION_RESULT GenericList::insert(void* item, list_type position)
{
    if(position > items) // If position goes past the number of items.
    {
        return POSITION_INVALID; // Return an error.
    }

    if(item == NULL) // If a NULL value is being inserted.
    {
    	return ITEM_INVALID; // Item is illegal.
    }

    // If there is no more space in the list..
    if(items >= capacity)
    {
#ifdef STATIC_LIST
		return STRUCTURE_FULL; // Error;
#else
		OPERATION_RESULT result = _grow(); // Grow the list.

		if(result)
		{
			return result; // Error.
		}
#endif
    }

	if(items == MAX_SIZE) // If the list is full.
	{
		return STRUCTURE_FULL;
	}

    // For each item in the list from the end to the desired position..
    for(list_type i = items; i > position; i--)
    {
    	list[i] = list[i - 1]; // Shift the item to the right.
    }

    list[position] = item; // Inserts the item in the desired position.
    items++; // We have added an item to the list.

    return SUCCESS;
}

void* GenericList::remove_item(void* item)
{
	// For each item in the list.
	for(list_type i = 0; i < items; i++)
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

void* GenericList::remove(list_type index)
{
	if(index >= items ) // If the index is invalid.
	{
		return NULL; // Item not found.
	}

	void* item = list[index]; // Retrieve the item.
	list[index] = NULL; // Blank the list at this index.
	compact(); // Compact the list.
	items--; // An item was removed.

	return item; // Success!
}


void GenericList::compact(void)
{
	// For each item in the list until the end.
	for(list_type i = 0; i < capacity - 1; i++)
	{
		if(list[i] != NULL) // If there is an item at this position.
		{
			continue; // Skip it.
		}
		// Else there is no item at this position.

		list[i] = list[i + 1]; // Move the next item to that position.

		list[i + 1] = NULL; // Blank the next position.
	}

#ifndef STATIC_LIST

	// If there is more than 2 * CAPACITY of free space after the last item.
	// TODO This seems to require a lot of free space before shrinking.
	if(capacity > 2 * CAPACITY && items < capacity - (2 * CAPACITY))
	{
		// Shrink the list.

		list_type previous_capacity = capacity; // Save the old capacity.

		capacity -= 3 * CAPACITY; // Trick grow into thinking we are growing.
		// TODO the list is shrinking, realloc will always work.
		if(_grow())
		{
			// Not enough memory to copy the list.
			capacity = previous_capacity; // Restore the capacity;
			return; // Leave the list as is.
		}
	}

#endif
}

#ifndef STATIC_LIST

OPERATION_RESULT GenericList::_grow(void)
{
	if(capacity == MAX_SIZE) // If the list has reached its maximum size.
	{
		return STRUCTURE_FULL;
	}

	void** new_list = (void**)realloc(list, (capacity + CAPACITY) * sizeof(void**));

	if(!new_list) // If there was not enough space for the new list.
	{
		return OUT_OF_MEMORY; // Error.
	}

	capacity += CAPACITY; // Increase the capacity of the list.

	list = new_list;

	return SUCCESS;
}

#endif

void* GenericList::operator[](list_type i) const
{
	// If there are no items or an index greater that the amount of item is requested.
	if(!items || i >= items)
	{
		return NULL; // Error.
	}

	return list[i]; // Return the item at the requested index.
}

OPERATION_RESULT GenericDictionary::add(const char* key, void* value)
{
	if(items == MAX_SIZE) // If the dictionary is full.
	{
		return STRUCTURE_FULL;
	}

	// Check if the entry already exists.
	key_value_pair<void*>* kv = get(key);

	if(kv == NULL) // If the entry does not exist.
	{
		kv = (key_value_pair<void*>*)malloc(sizeof(key_value_pair<void*>));

		if(!kv)
		{
			return OUT_OF_MEMORY; // Not enough memory to complete the addition.
		}

		// Instantiate an entry.
		kv->key = key;
		kv->value = value;

		append(kv);
	}
	else // The entry already exists.
	{
		/**todo thats dangerous and could lead to memory leaks. A method
		 * for replacing values should be created instead.*/
		kv->value = value;
	}

	return SUCCESS;
}

void* GenericDictionary::remove(const char* key, bool free_key = false)
{
	key_value_pair<void*>* kv = get(key); // Finds the entry.

	if(kv == NULL) // If the entry does not exist.
	{
		return NULL;
	}

	void* value = kv->value; // Saves the value of the entry.

	if(free_key) // If the key should be freed.
	{
		free((void*)kv->key);
	}

	free(remove_item(kv));

	return value; // Success
}

void* GenericDictionary::find(const char* key) const
{
	key_value_pair<void*>* kv = get(key); // Finds the entry.

	if(kv == NULL) // If the entry was not found.
	{
		return NULL;
	}

	return kv->value; // Return the value.
}

const char* GenericDictionary::find_val(const void* value) const
{
	key_value_pair<void*>* kv;

	for(list_type i = 0; i < items; i++) // For each entry in the dictionary.
	{
		kv = (key_value_pair<void*>*)list[i];

		if(kv->value == value) // If the two values match.
		{
			return kv->key; // Return the key_value pair.
		}
	}

    return NULL; // The value was not found.
}

key_value_pair<void*>* GenericDictionary::get(const char* key) const
{
	key_value_pair<void*>* kv;

	for(list_type i = 0; i < items; i++) // For each entry in the dictionary.
	{
		kv = (key_value_pair<void*>*)list[i];

		if(!strcmp(kv->key, key)) // If the two keys match.
		{
			return kv; // Return the key_value pair.
		}
	}

	return NULL; // The entry was not found.
}

GenericLinkedList::GenericLinkedList():
	start(NULL)
{}

list_type GenericLinkedList::get_item_count(void)
{
	list_type items = 0;
	entry* current = start;

	while(current != NULL) // While there are items in the list.
	{
		items++; // One more item.
		current = current->next; // Go to the next item.
	}

	return items; // Returns the number of items.
}

OPERATION_RESULT GenericLinkedList::append(void* item)
{
	///todo I don't think we need double pointers here.

	entry** next = &start;

	 // Go to the end of the list by hoping from one item to the next.
	while(*next != NULL)
	{
		next = &(*next)->next; // Go to the next item.
	}

	// The end of the list has been reached, add a new entry.
	*next = (entry*)malloc(sizeof(entry));

	if(!*next) // If no memory could be allocated for the new entry.
	{
		return OUT_OF_MEMORY; // Return an error.
	}

	// Initialize the entry.
	(*next)->item = item;
	(*next)->next = NULL;

	return SUCCESS;
}

OPERATION_RESULT GenericLinkedList::insert(void* item, list_type position)
{
	// Allocate a new entry.
	entry* ent = (entry*)malloc(sizeof(entry));

	if(!ent) // If the new entry could not be allocated.
	{
		return OUT_OF_MEMORY; // Return an error.
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
			return POSITION_INVALID; // Return an error.
		}

		// Adds the new entry at the requested position.
		ent->next = temp->next;
		temp->next = ent;
	}

	return SUCCESS;
}

void* GenericLinkedList::remove(list_type index)
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

	free(ent); // Free the entry.

	return item; // Success.
}

GenericLinkedList::entry* GenericLinkedList::get(list_type position)
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
