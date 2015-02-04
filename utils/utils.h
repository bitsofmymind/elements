/* utils.h - Implements a few basic data structures
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

/** Defines the capacity of the list and dictionary.*/
#define CAPACITY 5

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
		uint8_t capacity;
#endif

	public:

		/// Class destructor.
		~GenericList();

		/**
		 * The number of items in the list.
		 * */
		uint8_t items;

		/**
		 * Appends an item to the end of the list.
		 * @param item the item to append.
		 * @return 0 if appending was sucessful, 1 if the list is full and 2
		 * 	if the item is invalid.
		 * */
		int8_t append( void* item );

		/**
		 * Inserts an item at a given position in the list.
		 * @param item the item to insert.
		 * @param position the position to insert the item at.
		 * @return 0 if the insertion was a success, -1 if the position was
		 * 	invalid, -2 if there is no more space in the list and -3 if the
		 * 	item is invalid.
		 * */
        int8_t insert( void* item, uint8_t position );

        /**
         * Removes a given item from the list.
         * @param item the item to remove.
         * @return the item that was removed or NULL if it was not found.
         * */
        void* remove_item( void* item );

        /**
         * Removes the item a given position in the list.
         * @param index the 0 based index of the item to remove.
         * @return the removed item or NULL if there was no item at this index.
         * */
		void* remove( uint8_t index );

		/**
		 *  Index operator method for retrieving items based on their indexes.
		 * */
		void* operator[]( uint8_t i ) const;

	protected:

		/**
		 * Class constructor. Made protected to prevent this class from being
		 * instantiated.
		 * */
		GenericList();

		/**
		 * Compacts the list.
		 * */
		void compact( void );

	private:

#ifndef STATIC_LIST
		/**
		 * Grow the list.
		 * @return 0 if the growing the list succeeded.
		 * */
		int8_t grow(void);
#endif

};

/** A templated version of the generic list. */
template<class T> class List: public GenericList
{
	public:
		/**
		 * Appends an item to the end of the list.
		 * @param item the item to append.
		 * @return 0 if appending was successful, 1 if the list is full and 2
		 * 	if the item is invalid.
		 * */
		int8_t append( T item );

		/**
		 * Inserts an item at a given position in the list.
		 * @param item the item to insert.
		 * @param position the position to insert the item at.
		 * @return 0 if the insertion was a success, -1 if the position was
		 * 	invalid, -2 if there is no more space in the list and -3 if the
		 * 	item is invalid.
		 * */
        int8_t insert(T item, uint8_t position);

        /**
         * Removes a given item from the list.
         * @param item the item to remove.
         * @return the item that was removed or NULL if it was not found.
         * */
        T remove_item( T item );

        /**
         * Removes the item a given position in the list.
         * @param index the 0 based index of the item to remove.
         * @return the removed item or NULL if there was no item at this index.
         * */
		T remove( uint8_t index );

		/**
		 *  Index operator method for retrieving items based on their indexes.
		 * */
		T operator[](uint8_t i) const;
};

///todo move to class definition.
template<class T>
int8_t List<T>::append(T item)
{
	return GenericList::append((void*)item);
}

///todo move to class definition.
template<class T>
int8_t  List<T>::insert(T item, uint8_t position)
{
    return GenericList::insert((void*)item, position);
}

///todo move to class definition.
template<class T>
T List<T>::remove_item( T item )
{
	return (T)GenericList::remove_item( (void*)item );
}

///todo move to class definition.
template< class T>
T List<T>::remove( uint8_t index )
{
	return (T)GenericList::remove( index );
}

///todo move to class definition.
template< class T >
T List<T>::operator[](uint8_t i) const
{
	return (T)GenericList::operator[](i);
}

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
class GenericDictionary
{
	protected:

		/**
		 * The list that holds the key/value pairs of the dictionary.
		 * */
		key_value_pair<void*> list[CAPACITY];

	public:

		/**
		 * The number of items in the dictionary.
		 * */
		uint8_t items;

		/**
		 * Adds an entry in the dictionary.
		 * @param key the name or key of the entry.
		 * @param value the value of the entry.
		 * @return 0 if successful or 1 if the dictionary is full.
		 * */
		int8_t add(const char* key, void* value );

		/**
		 * Removes an entry.
		 * @param key the key of the entry.
		 * @return the value of the entry or NULL if it was not found.
		 * */
		void* remove( const char* key);

		/**
		 * Finds an entry.
		 * @param key the key to the entry.
		 * @return the value of the entry or NULL if it was not found.
		 * */
		void* find( const char* key );

		/**
		 * Finds a value.
		 * @param value the value to find.
		 * @return the key of the value or NULL if the value does not exist.
		 * */
        const char* find_val( void* value );

        /**
         * Indexing operator method.
         * todo make the returned key_value_pair const.
         * */
		key_value_pair<void*>* operator[](uint8_t i);

	protected:

		/**
		 * Class constructor. Protected to prevent instantiation of this class.
		 * */
		GenericDictionary();

		/**
		 * Internal method for retrieving a key/value pair.
		 * @param key the key of the pair.
		 * @return the found key/.value pair or NULL if it does not exist.
		 * */
		key_value_pair<void*>* get( const char* key );

		/**
		 * Compacts the dictionary.
		 * */
		void compact( void );
};

template< class T> class Dictionary: public GenericDictionary
{
	public:

		/**
		 * Adds an entry in the dictionary.
		 * @param key the name or key of the entry.
		 * @param value the value of the entry.
		 * @return 0 if successful or 1 if the dictionary is full.
		 * */
		int8_t add(const char*, T value );

		/**
		 * Removes an entry.
		 * @param key the key of the entry.
		 * @return the value of the entry or NULL if it was not found.
		 * */
		T remove( const char* key);

		/**
		 * Finds an entry.
		 * @param key the key to the entry.
		 * @return the value of the entry or NULL if it was not found.
		 * */
		T find( const char* key );

		/**
		 * Finds a value.
		 * @param value the value to find.
		 * @return the key of the value or NULL if the value does not exist.
		 * */
        const char* find_val( T value );

        /**
         * Indexing operator method.
         * todo make the returned key_value_pair const.
         * */
		key_value_pair<T>* operator[](uint8_t i);
};

///todo move to class definition.
template< class T>
int8_t Dictionary<T>::add( const char* key, T value)
{
	return GenericDictionary::add( key, (void*)value);
}

///todo move to class definition.
template< class T>
T Dictionary< T >::remove( const char* key )
{
	return (T)GenericDictionary::remove(key);
}

///todo move to class definition.
template< class T>
T Dictionary< T >::find( const char* key )
{
	return (T)GenericDictionary::find(key);
}

///todo move to class definition.
template< class T>
const char* Dictionary< T >::find_val( T value )
{
	return GenericDictionary::find_val((void*)value);
}

///todo move to class definition.
template< class T>
key_value_pair<T>* Dictionary<T>::operator[](uint8_t i)
{
	return (key_value_pair<T>*)GenericDictionary::operator[](i);
}

/**
 * A list that has been adapted to be used as a queue.
 * */
template< class T > class Queue: public List<T>
{
	public:

		/**
		 * Adds an object to the end of the queue.
		 * @param object the object.
		 * @return 0 if queuing was successful, 1 if the list is full and 2
		 * 	if the item is invalid.
		 * */
		int8_t queue( T object );

		/**
		 * Removes the object at the beginning of the queue.
		 * @return the dequeued object or NULL if the queue was empty.
		 * */
		T dequeue( void );

		/** Checks the object at the beginning of the queue.
		 * @return the object or NULL if the queue is empty.*/
		T peek( void ) const;
};

///todo move to class definition.
template< class T>
int8_t Queue<T>::queue(T object)
{
	return List<T>::append(object);
}

///todo move to class definition.
template< class T>
T Queue<T>::dequeue(void)
{
	return (T)List<T>::remove(0);
}

///todo move to class definition.
template< class T>
T Queue<T>::peek(void) const
{
	return (T)List<T>::operator[](0);
}

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
		uint8_t items(void);

		/**
		 * Adds an item to the end of the list.
		 * @param item the item.
		 * @return 0 if appending was successful or 1 if the memory a new
		 * 	item could not be allocated.
		 * */
		int8_t append( void* item );

		/**
		 * Adds an item a given position.
		 * @param item the item.
		 * @param position the position.
		 * @return 0 if appending was successful, 1 if the memory a new
		 * 	item could not be allocated and 2 if the position is invalid..
		 * */
        int8_t insert(void* item, uint8_t position);

        /**
         * Removes the item at a given position.
         * @param the position.
         * @return the removed item of NULL if there was no item at the desired
         * 	position.
         * */
		void* remove( uint8_t position );

	private:

		/** Retrieves the item at a given position.
		 * @param position the position.
		 * @return the linked list entry or null if the position is invalid.
		 * */
		entry* get(uint8_t position);
};

/** A linked list.*/
template<class T> class LinkedList: public GenericLinkedList
{
	/**
	 * Adds an item to the end of the list.
	 * @param item the item.
	 * @return 0 if appending was successful or 1 if the memory a new
	 * 	item could not be allocated.
	 * */
	int8_t append( T item );

	/**
	 * Adds an item a given position.
	 * @param item the item.
	 * @param position the position.
	 * @return 0 if appending was successful, 1 if the memory a new
	 * 	item could not be allocated and 2 if the position is invalid..
	 * */
    int8_t insert(T item, uint8_t position);

    /**
     * Removes the item at a given position.
     * @param the position.
     * @return the removed item of NULL if there was no item at the desired
     * 	position.
     * */
	T remove( uint8_t index );
};

///todo move to class definition.
template<class T> int8_t LinkedList<T>::append(T item)
{
	return this->GenericLinkedList::append( (void*)item );
}

///todo move to class definition.
template<class T> int8_t LinkedList<T>::insert(T item, uint8_t position)
{
	return this->GenericLinkedList::insert( (void*)item, position );
}

///todo move to class definition.
template<class T> T LinkedList<T>::remove(uint8_t position)
{
	return this->GenericLinkedList::remove( position );
}

#endif /* UTILS_H_ */
