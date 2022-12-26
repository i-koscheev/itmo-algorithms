#pragma once

/* dynamic array */

constexpr int DEFAULT_ARRAY_CAPACITY = 8;

template<typename T>
class Array final
{
	public:
		Array();
		Array( int capacity );
		Array( const Array<T>& other );
		Array( Array<T>&& other );
		~Array();

		Array<T>& operator = ( const Array<T>& other );
		Array<T>& operator = ( Array<T>&& other );

		int insert( const T& value );
		int insert( int index, const T& value );
		void remove( int index );

		const T& operator [] ( int index ) const;
		T& operator [] ( int index );
		int length() const;


		class Iterator
		{
			public:
				Iterator( Array<T>* array, int step );
				T& get() const;
				void set( const T& value );
				void next();
				bool hasCurrent() const;
			private:
				Array<T>* array;
				int index;
				int step;
		};

		Iterator iterator();
		Iterator reverseIterator();

		class ConstIterator
		{
			public:
				ConstIterator( Array<T>* array, int step );
				const T& get() const;
				void next();
				bool hasCurrent() const;
			private:
				Array<T>* array;
				int index;
				int step;
		};

		ConstIterator iterator() const;
		ConstIterator reverseIterator() const;

	private:
		int capacity;
		int size;
		T* a;

		void clear();
		void enlarge_capacity( int shift_index );
};


/* default costructor */
template<typename T>
Array<T>::Array():
	capacity( DEFAULT_ARRAY_CAPACITY ),
	size( 0 )
{
	a = (T*) std::malloc( capacity * sizeof( T ) );
}


/* with parameter */
template<typename T>
Array<T>::Array( int capacity ):
	capacity( capacity > 0 ? capacity : DEFAULT_ARRAY_CAPACITY ),
	size( 0 )
{
	a = (T*) std::malloc( this->capacity * sizeof( T ) );
}


/* copy constructor */
template<typename T>
Array<T>::Array( const Array<T>& other ):
	capacity( other.capacity ),
	size( other.size )
{
	a = (T*) std::malloc( capacity * sizeof( T ) );
	for ( int i = 0; i < size; i++ )
	{
		new( a + i ) T( other[i] );
	}
}


/* move constructor */
template<typename T>
Array<T>::Array( Array<T>&& other ):
	capacity( other.capacity ),
	size( other.size ),
	a( other.a )
{
	other.a = nullptr;
	other.size = 0;
	other.capacity = 0;
}


/* destructor */
template<typename T>
Array<T>::~Array()
{
	clear();
}


/* copy assignment */
template<typename T>
Array<T>& Array<T>::operator = ( const Array<T>& other )
{
	Array<T> copy( other );
	std::swap( capacity, copy.capacity );
	std::swap( size, copy.size );
	std::swap( a, copy.a );
	return *this;
}


/* move assignment */
template<typename T>
Array<T>& Array<T>::operator = ( Array<T>&& other )
{
	if ( this != &other )
	{
		clear();
		capacity = other.capacity;
		size = other.size;
		a = other.a;
		other.a = nullptr;
		other.size = 0;
		other.capacity = 0;
	}
	return *this;
}


/* memory freeing */
template<typename T>
void Array<T>::clear()
{
	for ( int i = 0; i < size; i++ )
	{
		a[i].~T();
	}
	std::free( a );
}


template<typename T>
int Array<T>::insert( const T& value )
{
	if ( size == capacity )
	{
		enlarge_capacity( size );
	}
	new( a + size ) T( value );
	return size++;
}


template<typename T>
int Array<T>::insert( int index, const T& value )
{
	if ( size == capacity )
	{
		enlarge_capacity( index );
	}
	else
	{
		for ( int i = size; i > index; i-- )
		{
			new( a + i ) T( std::move( a[i - 1] ) );
			a[i - 1].~T();
		}
	}
	new( a + index ) T( value );
	size++;
	return index;
}

/* increasing capacity with 1 element shift starting from shift_index */
template<typename T>
void Array<T>::enlarge_capacity( int shift_index )
{
	capacity *= 2;
	T* a_enlarged = (T*) std::malloc( capacity * sizeof( T ) );
	int i = 0;
	while ( i < shift_index )
	{
		new( a_enlarged + i ) T( std::move( a[i] ) );
		a[i].~T();
		i++;
	}
	while ( i < size )
	{
		new( a_enlarged + i + 1 ) T( std::move( a[i] ) );
		a[i].~T();
		i++;
	}
	std::free( a );
	a = a_enlarged;
}


template<typename T>
void Array<T>::remove( int index )
{
	a[index].~T();
	for ( int i = index + 1; i < size; i++ )
	{
		new ( a + i - 1 ) T( std::move( a[i] ) );
		a[i].~T();
	}
	size--;
}


template<typename T>
const T& Array<T>::operator [] ( int index ) const
{
	return a[index];
}


template<typename T>
T& Array<T>::operator [] ( int index )
{
	return a[index];
}


template<typename T>
int Array<T>::length() const
{
	return size;
}



/* iterators */


template<typename T>
Array<T>::Iterator::Iterator( Array<T>* array, int step ):
	array( array ),
	step( step ),
	index( step > 0 ? 0 : ( array->length() - 1 ) )
{}

template<typename T>
void Array<T>::Iterator::next()
{
	index += step;
}

template<typename T>
bool Array<T>::Iterator::hasCurrent() const
{
	return ( index >= 0 ) && ( index < array->length() );
}

template<typename T>
T& Array<T>::Iterator::get() const
{
	return ( *array )[index];
}

template<typename T>
void Array<T>::Iterator::set( const T& value )
{
	( *array )[index] = value;
}


template<typename T>
Array<T>::ConstIterator::ConstIterator( Array<T>* array, int step ):
	array( array ),
	step( step ),
	index( step > 0 ? 0 : ( array->length() - 1 ) )
{}

template<typename T>
void Array<T>::ConstIterator::next()
{
	index += step;
}

template<typename T>
bool Array<T>::ConstIterator::hasCurrent() const
{
	return ( index >= 0 ) && ( index < array->length() );
}

template<typename T>
const T& Array<T>::ConstIterator::get() const
{
	return ( *array )[index];
}



template<typename T>
typename Array<T>::Iterator Array<T>::iterator()
{
	Array<T>::Iterator iterator( this, 1 );
	return iterator;
}

template<typename T>
typename Array<T>::ConstIterator Array<T>::iterator() const
{
	Array<T>::ConstIterator iterator( this, 1 );
	return iterator;
}

template<typename T>
typename Array<T>::Iterator Array<T>::reverseIterator()
{
	Array<T>::Iterator iterator( this, -1 );
	return iterator;
}

template<typename T>
typename Array<T>::ConstIterator Array<T>::reverseIterator() const
{
	Array<T>::ConstIterator iterator( this, -1 );
	return iterator;
}