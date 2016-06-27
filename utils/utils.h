/* utils.h - Header file for the framework's basic data structures.
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

#ifndef UTILS_H_
#define UTILS_H_

#include <stdint.h>
#include <stdlib.h>

/** Defines the capacity of the list and dictionary.*/
#define CAPACITY 5

/** The type of the size of the list.*/
typedef uint8_t list_type;

/** The maximum size the list can have.*/
#define MAX_SIZE 255

namespace Utils
{
	/* Since SUCCESS is pretty generic enum entry,
	 * a namespace is used to prevent conflicts when user other code bases.*/

	/** A structure representing the different results an operation on a data
	 * structure can have.*/
	enum OPERATION_RESULT
	{
		SUCCESS = 0,
		POSITION_INVALID,
		STRUCTURE_FULL,
		ITEM_INVALID,
		ITEM_EXISTS,
		OUT_OF_MEMORY
	};
}

/**
 * A generic list. Only to be used as a base class for a template class.
 * */
class GenericList
{
	protected:

#ifdef STATIC_LIST // If the list is static (ie. it will not grow in size).
		/**
		 * The array that holds the items kept in the list.
		 * */
		void* list[CAPACITY];
#else
		void** list;
#endif

#ifdef STATIC_LIST
		/// The capacity of the list.
		const uint8_t capacity = CAPACITY;
#else
		list_type capacity;
#endif

		/**
		 * The number of items in the list.
		 * */
		list_type items;

	public:

		/// Class destructor.
		~GenericList();

		/**
		 * Appends an item to the end of the list.
		 * @param item the item to append.
		 * @return the result of the operation.
		 * */
		inline Utils::OPERATION_RESULT append(void* item)
		{
			return insert(item, get_item_count());
		}

		/**
		 * Inserts an item at a given position in the list.
		 * @param item the item to insert.
		 * @param position the position to insert the item at.
		 * @return the result of the operation.
		 * */
		Utils::OPERATION_RESULT insert(void* item, list_type position);

        /**
         * Removes a given item from the list.
         * @param item the item to remove.
         * @return the item that was removed or NULL if it was not found.
         * */
        void* remove_item(void* item);

        /**
         * Removes the item a given position in the list.
         * @param index the 0 based index of the item to remove.
         * @return the removed item or NULL if there was no item at this index.
         * */
		void* remove(list_type index);

		/**
		 *  Index operator method for retrieving items based on their indexes.
		 * */
		void* operator[](list_type i) const;

		/** @return the number of items in the list. */
		inline list_type get_item_count(void) const { return items; }

	protected:

		/**
		 * Class constructor. Made protected to prevent this class from being
		 * instantiated.
		 * */
		GenericList();

		/**
		 * Compacts the list.
		 * */
		void compact(void);

	private:

#ifndef STATIC_LIST
		/**
		 * Grow the list.
		 * @return the result of the operation.
		 * */
		Utils::OPERATION_RESULT _grow(void);
#endif
};

/** A templated version of the generic list. */
template<class T> class List: public GenericList
{
	public:
		/**
		 * Appends an item to the end of the list.
		 * @param item the item to append.
		 * @return the result of the operation.
		 * */
		inline Utils::OPERATION_RESULT append(T item)
		{
			return GenericList::append((void*)item);
		}

		/**
		 * Inserts an item at a given position in the list.
		 * @param item the item to insert.
		 * @param position the position to insert the item at.
		 * @return the result of the operation.
		 * */
        inline Utils::OPERATION_RESULT insert(T item, list_type position)
        {
            return GenericList::insert((void*)item, position);
        }

        /**
         * Removes a given item from the list.
         * @param item the item to remove.
         * @return the item that was removed or NULL if it was not found.
         * */
        inline T remove_item(T item)
        {
        	return (T)GenericList::remove_item((void*)item);
        }

        /**
         * Removes the item a given position in the list.
         * @param index the 0 based index of the item to remove.
         * @return the removed item or NULL if there was no item at this index.
         * */
		inline T remove(list_type index)
        {
        	return (T)GenericList::remove(index);
        }

		/**
		 *  Index operator method for retrieving items based on their indexes.
		 * */
		inline T operator[](list_type i) const
		{
			return (T)GenericList::operator[](i);
		}

		/** Remove all items from the list and delete them. Only call this method
		 * if the list contains objects created using the new operator.*/
		inline void delete_all(void) { _purge(false); }

		/** Remove all items from the list and free them. Only call this method
		 * if the list contains data allocated using the malloc operator.*/
		inline void free_all(void) { _purge(true); }

	private:

		/** Remove all items and delete/free them.
		 * @param use_free true if free should be used, false if delete should be
		 * used. */
		void _purge(bool use_free)
		{
			// Do not use remove(0) because it is slower.

			// free or delete each item.
			for(uint8_t i = 0; i < items; i++)
			{
				void* item = list[i];

				use_free ? free(item): delete (T)item;
				list[i] = NULL;
			}

			items = 0; // The list is now empty.

			compact(); // Compact the list to shrink it.
		}
};

/**
 * A structure to hold key value pairs in a dictionary.
 * */
template< class U > struct key_value_pair
{
	/**
	 * The name or key of the entry.
	 * */
	const char* key;

	/**
	 *The value of the entry.
	 * */
	U value;
};

/**
 * A generic dictionary. Only to be used as a base class for a template class.
 * */
class GenericDictionary: protected List<key_value_pair<void*>* >
{
	public:

		/**
		 * If keys should be case sensitive.
		 * */
		bool case_sensitive_keys = true;

		/**
		 * Adds an entry in the dictionary.
		 * @param key the name or key of the entry.
		 * @param value the value of the entry.
		 * @return the result of the operation.
		 * */
		Utils::OPERATION_RESULT add(const char* key, void* value);

		/**
		 * Removes an entry.
		 * @param key the key of the entry.
		 * @param free_key if the key should be freed.
		 * @return the value of the entry or NULL if it was not found.
		 * */
		void* remove(const char* key, bool free_key);

		/**
		 * Finds an entry.
		 * @param key the key to the entry.
		 * @return the value of the entry or NULL if it was not found.
		 * */
		void* find(const char* key) const;

		/**
		 * Finds a value.
		 * @param value the value to find.
		 * @return the key of the value or NULL if the value does not exist.
		 * */
        const char* find_val(const void* value) const;

        /**
         * Indexing operator method.
         * todo make the returned key_value_pair const.
         * */
		inline const key_value_pair<void*>* operator[](list_type i) const
		{
			return List<key_value_pair<void*>* >::operator[](i);
		}

		/** @return the number of items in the dictionary. */
		inline uint8_t get_item_count(void) const { return items; }

	protected:

		/**
		 * Internal method for retrieving a key/value pair.
		 * @param key the key of the pair.
		 * @return the found key/.value pair or NULL if it does not exist.
		 * */
		key_value_pair<void*>* get(const char* key) const;
};

template< class T> class Dictionary: public GenericDictionary
{
	public:

		/**
		 * Adds an entry in the dictionary.
		 * @param key the name or key of the entry.
		 * @param value the value of the entry.
		 * @return the result of the operation.
		 * */
		inline Utils::OPERATION_RESULT add(const char* key, T value)
		{
			return GenericDictionary::add(key, (void*)value);
		}

		/**
		 * Removes an entry.
		 * @param key the key of the entry.
		 * @param free_key if the key should be freed.
		 * @return the value of the entry or NULL if it was not found.
		 * */
		inline T remove(const char* key, bool free_key = false)
		{
			return (T)GenericDictionary::remove(key, free_key);
		}

		/**
		 * Finds an entry.
		 * @param key the key to the entry.
		 * @return the value of the entry or NULL if it was not found.
		 * */
		inline T find(const char* key) const
		{
			return (T)GenericDictionary::find(key);
		}

		/**
		 * Finds a value.
		 * @param value the value to find.
		 * @return the key of the value or NULL if the value does not exist.
		 * */
        inline const char* find_val(const T value) const
        {
        	return GenericDictionary::find_val((const void*)value);
        }

        /**
         * Indexing operator method.
         * todo make the returned key_value_pair const.
         * */
		inline const key_value_pair<T>* operator[](list_type i) const
		{
			return (key_value_pair<T>*)GenericDictionary::operator[](i);
		}

		/** Remove all items from the dictionary and delete them.
		 * Only call this method if the dictionary contains values
		 * allocated using the new operator.
		 * @param free_keys, if keys should be freed as well. */
		inline void delete_all(bool free_keys) { _purge(false, free_keys); }

		/** Remove all items from the dictionary and free them.
		 * Only call this method if the dictionary contains values
		 * allocated using the malloc operator.
		 * @param free_keys, if keys should be freed as well. */
		inline void free_all(bool free_keys) { _purge(true, free_keys); }

	private:

		/** Remove all values and delete/free them.
		 * @param use_free true if free should be used, false if delete should be
		 * used.
		 * @param free_keys, if keys should be freed as well.*/
		void _purge(bool use_free, bool free_keys)
		{
			// Do not use remove() because it is slower.

			// free or delete each item.
			for(uint8_t i = 0; i < items; i++)
			{
				key_value_pair<void*>* kv = (key_value_pair<void*>*)list[i];

				if(free_keys)
				{
					free((void*)kv->key);
				}

				use_free ? free((void*)kv->value): delete (T)kv->value;

				free(kv); // Free the key_value_pair structure.

				list[i] = NULL;
			}

			items = 0; // The dictionary is now empty.
		}
};

/**
 * A list that has been adapted to be used as a queue.
 * */
template< class T > class Queue: public List<T>
{
	public:

		/**
		 * Adds an object to the end of the queue.
		 * @param object the object.
		 * @return the result of the operation.
		 * */
		inline Utils::OPERATION_RESULT queue(T object)
		{
			return List<T>::append(object);
		}

		/**
		 * Removes the object at the beginning of the queue.
		 * @return the dequeued object or NULL if the queue was empty.
		 * */
		inline T dequeue(void)
		{
			return (T)List<T>::remove(0);
		}

		/** Checks the object at the beginning of the queue.
		 * @return the object or NULL if the queue is empty.*/
		inline T peek(void) const
		{
			return (T)List<T>::operator[](0);
		}
};

/**
 * A generic linked list.
 * todo this class needs a destructor.
 * */
class GenericLinkedList
{
	private:

		/**
		 * A pointer to the start of the linked list.
		 * */
		struct entry {void* item; entry* next;} * start;

	public:

		/**
		 * Class constructor.
		 * */
		GenericLinkedList();

		/**
		 * @return the number of items in the list.
		 * */
		uint8_t get_item_count(void);

		/**
		 * Adds an item to the end of the list.
		 * @param item the item.
		 * @return the result of the operation.
		 * */
		Utils::OPERATION_RESULT append( void* item );

		/**
		 * Adds an item a given position.
		 * @param item the item.
		 * @param position the position.
		 * @return the result of the operation.
		 * */
		Utils::OPERATION_RESULT insert(void* item, list_type position);

        /**
         * Removes the item at a given position.
         * @param the position.
         * @return the removed item of NULL if there was no item at the desired
         * 	position.
         * */
		void* remove(list_type position);

	private:

		/** Retrieves the item at a given position.
		 * @param position the position.
		 * @return the linked list entry or null if the position is invalid.
		 * */
		entry* get(list_type position);
};

/** A linked list.*/
template<class T> class LinkedList: public GenericLinkedList
{
	/**
	 * Adds an item to the end of the list.
	 * @param item the item.
	 * @return the result of the operation.
	 * */
	inline Utils::OPERATION_RESULT append(T item)
	{
		return this->GenericLinkedList::append((void*)item);
	}

	/**
	 * Adds an item a given position.
	 * @param item the item.
	 * @param position the position.
	 * @return the result of the operation.
	 * */
	inline Utils::OPERATION_RESULT insert(T item, list_type position)
	{
		return this->GenericLinkedList::insert( (void*)item, position );
	}

    /**
     * Removes the item at a given position.
     * @param the position.
     * @return the removed item of NULL if there was no item at the desired
     * 	position.
     * */
	T remove(uint8_t index)
	{
		return this->GenericLinkedList::remove(index);
	}
};


#endif /* UTILS_H_ */
