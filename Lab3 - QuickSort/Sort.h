#pragma once


template<typename T, typename Compare>
void insertion_sort( T* first, T* last, Compare comp )
{
    for ( T* i = first; i <= last; i++ )
    {
        T current = std::move( *i );
        T* j = i - 1;
        while ( j >= first && comp( current, *j ) )
        {
            *( j + 1 ) = std::move( *j );
            j--;
        }
        *( j + 1 ) = std::move( current );
    }
}


template<typename T>
void swap( T* a, T* b ) {
    T tmp = std::move( *a );
    *a = std::move( *b );
    *b = std::move( tmp );
}

// choose median value, simultaneously sorting
template<typename T, typename Compare>
T median( T* first, T* last, Compare comp )
{
    T* middle = first + ( last - first ) / 2;
    if ( comp( *last, *first ) )
    {
        swap( first, last );
    }
    if ( comp( *middle, *first ) )
    {
        swap( first, middle );
    }
    if ( comp( *last, *middle ) )
    {
        swap( middle, last );
    }
    return *middle;
}


// Hoare partition, returns pivot pointer
template<typename T, typename Compare>
T* partition( T* first, T* last, Compare comp )
{
    T pivot_value = median( first, last, comp );
    // first and last already compared to pivot
    T* l = first + 1; 
    T* r = last - 1;
    if ( l < r )
        while( true )
        {
            while ( comp( *l, pivot_value ) )
                l++;
            while ( comp( pivot_value, *r ) )
                r--;
            if ( l >= r )
                break;
            swap( l++, r-- );
        }
    return r;
}

template<typename T, typename Compare>
void quick_sort( T* first, T* last, Compare comp )
{
    while ( last > first )
    {
        T* pivot = partition( first, last, comp );
        if ( pivot - first + 1 < last - pivot )
        {
            quick_sort( first, pivot, comp );
            first = pivot + 1;
        }
        else
        {
            quick_sort( pivot + 1, last, comp );
            last = pivot;
        }
    }

}


/* optimized quick sort */

constexpr int INSERTION_SORT_USING_POINT = 8;

template<typename T, typename Compare>
void hybrid_sort( T* first, T* last, Compare comp )
{
    while ( last - first >= INSERTION_SORT_USING_POINT )
    {
        T* pivot = partition( first, last, comp );
        if ( pivot - first + 1 < last - pivot )
        {
            quick_sort( first, pivot, comp );
            first = pivot + 1;
        }
        else
        {
            quick_sort( pivot + 1, last, comp );
            last = pivot;
        }
    }
    insertion_sort( first, last, comp );
}