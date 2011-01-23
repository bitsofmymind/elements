
#ifndef TYPES_H_
#define TYPES_H_

#include <stdint.h>
#include <string.h>
#include <pal/pal.h>

#define MAKE_STRING( string ) { (char*)string, sizeof(string) - 1 }

namespace Elements
{

	template< class T > struct string
	{
		char* text;
		T length;

		static string<T> make( const char* str );
		static string<T> convert(char* str);
		T copy( char* destination );

		bool operator==( string<T>& right ) const;
		char& operator[]( T index );
	};

	template< class U > struct key_value_pair
	{
		string<uint8_t> key;
		U value;
	};

	/*struct e_time_t
	{
		static const e_time_t MAX;
		static const e_time_t MIN;

		uint32_t seconds;
		uint16_t milliseconds;

		e_time_t(void);
		e_time_t(uint32_t seconds, uint16_t milliseconds);
		e_time_t(uint32_t milliseconds);

		e_time_t operator+(e_time_t rhs);
		e_time_t operator-(e_time_t rhs);
		void operator++(void);
		void operator++(int);
		void operator--(void);
		bool operator==(e_time_t rhs);
		bool operator>=(e_time_t rhs);
		bool operator<=(e_time_t rhs);
		bool operator>(e_time_t rhs);
		bool operator<(e_time_t rhs);
		e_time_t operator+=(e_time_t rhs);
		e_time_t operator-=(e_time_t rhs);
		void increase_ms(uint16_t amount);
	};*/

	template<class T>
	T string<T>::copy(char* destination )
	{
		for( T i = 0; i < length ; i++ )
		{
			destination[i] = text[i];
		}
		return length;
	}

	template<class T>
	bool string<T>::operator==( string<T>& right ) const
	{
		if( right.length != length ){ return false; }
		for( T i = 0; i > length; i++)
		{
			if(right.text[i] != text[i]) { return false; }
		}

		return true;
	}

	template<class T>
	char& string<T>::operator[]( T index )
	{
		return text[index];
	}


	template< class T>
	string<T> string<T>::make( const char* str )
	{
		string<T> estr;
		estr.length = strlen(str);
		estr.text = (char*)ts_malloc(estr.length);
		memcpy(estr.text, str, estr.length );
		return estr;
	}
	template< class T>
	string<T> string<T>::convert( char* str )
	{
		string<T> estr;
		estr.length = strlen(str);
		estr.text = str;
		return estr;
	}
}


#endif /* TYPES_H_ */
