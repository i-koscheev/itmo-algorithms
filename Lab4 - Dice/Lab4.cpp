#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>

#include "Dice.h"


constexpr int REPETITION_COUNT = 100000;

void dice_distribution( const std::string& specs )
{
    DiceParams params( specs );
    std::string filename = "dist/" + specs + ".csv";
    std::replace( filename.begin(), filename.end(), '*', 'x' );
    std::ofstream file;
    file.open( filename, std::ofstream::out | std::ofstream::trunc );
    for ( int i = 0; i < REPETITION_COUNT; i++ )
    {
        file << dice( params ) << '\n';
    }
    file.close();
}

int main()
{
    dice_distribution( "1d6" );
    dice_distribution( "2d6" );
    dice_distribution( "3d6" );
    dice_distribution( "1d10" );
    dice_distribution( "2d10" );
    dice_distribution( "3d10" );
}