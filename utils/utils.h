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

#define CAPACITY 5

class GenericList
{
	protected:
		void* list[CAPACITY];
	public:
		uint8_t items;

		GenericList();

		int8_t append( void* item );
        int8_t insert(void* item, uint8_t position);
        void* remove_item( void* item );
		void* remove( uint8_t index );
		void* operator[](uint8_t i);
	protected:
		void compact( void );
};

template< class T> class List: public GenericList
{
	public:
		int8_t append( T item );
        int8_t insert(T item, uint8_t position);
        T remove_item( T item );
		T remove( uint8_t index );
		T operator[](uint8_t i);
};

template< class U > struct key_value_pair
{
	const char* key;
	U value;
};

class GenericDictionary
{

	protected:
		key_value_pair<void*> list[CAPACITY];
	public:
		uint8_t items;

		GenericDictionary();

		int8_t add(const char* key, void* value );
		//virtual bool insert( char* key, uint8_t length, T* value );
		//virtual bool insert( char* key, T* value );
		void* remove( const char* key);
		void* find( const char* key );
        const char* find_val( void* value );
		key_value_pair<void*>* operator[](uint8_t i);
		//virtual T* operator[]( string<uint8_t> key);
		//virtual T* operator[]( char* key );

	protected:
		key_value_pair<void*>* get(  const char* key );
		void compact( void );

};

template< class T> class Dictionary: public GenericDictionary
{
	public:
		int8_t add(const char*, T value );
		//virtual bool insert( char* key, uint8_t length, T* value );
		//virtual bool insert( char* key, T* value );
		T remove( const char* key);
		T find( const char* key );
        const char* find_val( T value );
		key_value_pair<T>* operator[](uint8_t i);
		//virtual T* operator[]( string<uint8_t> key);
		//virtual T* operator[]( char* key );
};

template< class T > class Queue: public List<T>
{
	public:
		int8_t queue( T object );
		T dequeue( void );
		T peek( void );

};

class GenericLinkedList
{
	private:
		struct entry {void* item; entry* next;} * start;

	public:

		GenericLinkedList();

		uint8_t items(void);
		int8_t append( void* item );
        int8_t insert(void* item, uint8_t position);
		void* remove( uint8_t position );

	private:
		entry* get(uint8_t position);
};

template<class T> class LinkedList: public GenericLinkedList
{
	int8_t append( T item );
    int8_t insert(T item, uint8_t position);
	T remove( uint8_t index );
};

template< class T>
int8_t List<T>::append(T item)
{
	return GenericList::append((void*)item);
}

template<class T>
int8_t  List<T>::insert(T item, uint8_t position)
{
    return GenericList::insert((void*)item, position);
}
template< class T>
T List<T>::remove_item( T item )
{
	return (T*)GenericList::remove_item( (void*)item );
}
template< class T>
T List<T>::remove( uint8_t index )
{
	return (T)GenericList::remove( index );
}

template< class T >
T List<T>::operator[](uint8_t i)
{
	return (T)GenericList::operator[](i);
}

template< class T>
int8_t Queue<T>::queue(T object)
{
	return List<T>::append(object);
}


template< class T>
T Queue<T>::dequeue(void)
{
	return (T)List<T>::remove(0);
}

template< class T>
T Queue<T>::peek(void)
{
	return (T)List<T>::operator[](0);
}

template< class T>
int8_t Dictionary<T>::add( const char* key, T value)
{
	return GenericDictionary::add( key, (void*)value);
}

template< class T>
T Dictionary< T >::remove( const char* key )
{
	return (T)GenericDictionary::remove(key);
}

template< class T>
T Dictionary< T >::find( const char* key )
{
	return (T)GenericDictionary::find(key);
}

template< class T>
const char* Dictionary< T >::find_val( T value )
{
	return GenericDictionary::find_val((void*)value);
}

template< class T>
key_value_pair<T>* Dictionary<T>::operator[](uint8_t i)
{
	return (key_value_pair<T>*)GenericDictionary::operator[](i);
}

template<class T> int8_t LinkedList<T>::append(T item)
{
	return this->GenericLinkedList::append( (void*)item );
}

template<class T> int8_t LinkedList<T>::insert(T item, uint8_t position)
{
	return this->GenericLinkedList::insert( (void*)item, position );
}

template<class T> T LinkedList<T>::remove(uint8_t position)
{
	return this->GenericLinkedList::remove( position );
}

#endif /* UTILS_H_ */
