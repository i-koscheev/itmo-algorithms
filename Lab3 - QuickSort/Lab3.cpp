#include <iostream>
#include <fstream>
#include <ctime>
#include <ratio>
#include <chrono>

#include "Sort.h"


constexpr int REPETITION_COUNT = 50000;
constexpr int MAX_LENGTH = 32;

bool comp( int a, int b ) { return a < b; }

void benchmark()
{
    using namespace std::chrono;

    int a[MAX_LENGTH];

    std::ofstream file;
    file.open( "benchmark.csv", std::ofstream::out | std::ofstream::trunc );
    file << "count;insertion;quick;";
    file << '\n';

    for ( int last_index = 0; last_index < MAX_LENGTH; last_index++ )
    {
        double insertion_time = 0.0;
        double quick_time = 0.0;

        for ( int j = 0; j < REPETITION_COUNT; j++ )
        {
            for ( int i = 0; i <= last_index; i++ )
                a[i] = last_index - i;
            steady_clock::time_point t1 = steady_clock::now();
            insertion_sort( a, a + last_index, comp );
            steady_clock::time_point t2 = steady_clock::now();
            duration<double> insertion_span = duration_cast<duration<double>>( t2 - t1 );
            insertion_time += insertion_span.count();

            for ( int i = 0; i <= last_index; i++ )
                a[i] = last_index - i;
            steady_clock::time_point t3 = steady_clock::now();
            quick_sort( a, a + last_index, comp );
            steady_clock::time_point t4 = steady_clock::now();
            duration<double> quick_span = duration_cast<duration<double>>( t4 - t3 );
            quick_time += quick_span.count();
        }

        file << ( last_index + 1 ) << ';' << insertion_time << ';' << quick_time << ';';
        file << '\n';
    }
    file.close();
}


int main()
{
    //benchmark(); return 0;

    int a[20] = { 7, 20, 19, 5, 14, 3, 18, 6, 2, 11, 17, 10, 9, 15, 4, 13, 8, 16, 12, 1 };
    hybrid_sort( a, a + 19, []( int a, int b ) { return a < b; } );
    for( int i = 0; i < 20; i++ )
        std::cout << a[i] << ' ';
}