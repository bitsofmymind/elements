/*
 * utils.h
 *
 *  Created on: Mar 30, 2009
 *      Author: Antoine
 */



#ifndef UTILS_H_
#define UTILS_H_

#include "types.h"
#include <stdint.h>

#define CAPACITY 5

using namespace Elements;

class GenericList
{
	protected:
		void* list[CAPACITY];
	public:
		uint8_t items;

		GenericList();

		int8_t append( void* item );
        int8_t insert(void* item, uint8_t position);
		void* remove( uint8_t index );
		void* operator[](uint8_t i);
	protected:
		void compact( void );
};

template< class T> class List: public GenericList
{
	public:
		int8_t append( T* item );
                int8_t insert(T* item, uint8_t position);
		T* remove( uint8_t index );
		T* operator[](uint8_t i);
};

class GenericDictionary
{

	protected:
		key_value_pair<void*> list[CAPACITY];
	public:
		uint8_t items;

		GenericDictionary();

		int8_t add(Elements::string< uint8_t > key, void* value );
		//virtual bool insert( char* key, uint8_t length, T* value );
		//virtual bool insert( char* key, T* value );
		void* remove( Elements::string< uint8_t >& key);
		void* find( Elements::string< uint8_t >& key );
        Elements::string<uint8_t>* find( void* value );
		key_value_pair<void*>* operator[](uint8_t i);
		//virtual T* operator[]( string<uint8_t> key);
		//virtual T* operator[]( char* key );

	protected:
		key_value_pair<void*>* get(  Elements::string< uint8_t >& key );
		void compact( void );

};

template< class T> class Dictionary: public GenericDictionary
{
	public:
		int8_t add(Elements::string< uint8_t > key, T* value );
		//virtual bool insert( char* key, uint8_t length, T* value );
		//virtual bool insert( char* key, T* value );
		T* remove( Elements::string< uint8_t >& key);
		T* find( Elements::string< uint8_t > key );
                Elements::string<uint8_t>* find( T* value );
		key_value_pair<T*>* operator[](uint8_t i);
		//virtual T* operator[]( string<uint8_t> key);
		//virtual T* operator[]( char* key );
};

class GenericQueue
{
	protected:
		void* list[CAPACITY];
		uint8_t start;
	public:
		uint8_t items;

		GenericQueue();

		int8_t queue( void* object );
		void* dequeue( void );

};

template< class T > class Queue: public GenericQueue
{
	public:
		int8_t queue( T* object );
		T* dequeue( void );

};

template< class T>
int8_t List<T>::append(T* item)
{
	return GenericList::append( (void*)item);
}

template<class T>
int8_t  List<T>::insert(T* item, uint8_t position)
{
    return GenericList::insert((void*)item, position);
}
template< class T>
T* List<T>::remove( uint8_t index )
{
	return (T*)GenericList::remove( index );
}

template< class T >
T* List<T>::operator[](uint8_t i)
{
	return (T*)GenericList::operator[](i);
}

template< class T>
int8_t Dictionary<T>::add( Elements::string< uint8_t > key, T* value)
{
	return GenericDictionary::add( key, value);
}

template< class T>
T* Dictionary< T >::remove( Elements::string< uint8_t >& key )
{
	return (T*)GenericDictionary::remove(key);
}

template< class T>
T* Dictionary< T >::find( Elements::string< uint8_t > key )
{
	return (T*)GenericDictionary::find(key);
}

template< class T>
Elements::string< uint8_t >* Dictionary< T >::find( T* value )
{
	return GenericDictionary::find((void*)value);
}

template< class T>
key_value_pair<T*>* Dictionary<T>::operator[](uint8_t i)
{
	return (key_value_pair<T*>*)GenericDictionary::operator[](i);
}


template< class T>
int8_t Queue<T>::queue(T* object)
{
	return this->GenericQueue::queue( (void*)object );
}


template< class T>
T* Queue<T>::dequeue(void)
{
	return (T*)this->GenericQueue::dequeue( );
}

#endif /* UTILS_H_ */
