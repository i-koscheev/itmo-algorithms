#include <iostream>
#include "Array.h"

int main()
{
    std::cout << "Hello" << std::endl;
    Array<int> a;
    for ( int i = 0; i < 10; ++i )
        a.insert( i + 1 );
    for ( int i = 0; i < a.length(); ++i )
        a[i] *= 2;
    for ( auto it = a.iterator(); it.hasCurrent(); it.next() )
        std::cout << it.get() << std::endl;
    return 0;
}