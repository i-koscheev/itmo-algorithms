#pragma once

#include <stdio.h>
#include <cstring>
#include <algorithm>
#include <random>

/* dice rolling */

class DiceParams
{
	public:
		DiceParams( int sides, int rolls, int multiply, int add );
		DiceParams( const std::string& specs );
		int rolls_count() const { return count; }
		int random_side();
		int multiply_modifier;
		int add_modifier;
	private:
		int d;
		int count;
		std::default_random_engine random_engine;
		std::uniform_int_distribution<int> dist;
		void set_random_engine();
};

DiceParams::DiceParams( int sides = 6, int rolls = 1, int multiply = 1, int add = 0 ):
	multiply_modifier( multiply ),
	add_modifier( add )
{
	d = ( sides < 1 ) ? 1 : sides;
	count = ( rolls < 1 ) ? 1 : rolls;
	set_random_engine();
}

DiceParams::DiceParams( const std::string& specs )
{
	int sides = 6,
		rolls = 1,
		multiply = 1,
		add = 0;
	std::string str = specs;
	str.erase( remove_if( str.begin(), str.end(), isspace ), str.end() );
	if ( str.find( '*' ) == std::string::npos )
	{
		sscanf_s( str.c_str(), "%d d %d %d", &rolls, &sides, &add );
	}
	else
	{
		sscanf_s( str.c_str(), "%d * %d d %d %d", &multiply, &rolls, &sides, &add );
	}
	d = ( sides < 1 ) ? 1 : sides;
	count = ( rolls < 1 ) ? 1 : rolls;
	multiply_modifier = multiply;
	add_modifier = add;
	set_random_engine();
}

void DiceParams::set_random_engine()
{
	std::random_device rd;
	random_engine = std::default_random_engine( rd() );
	dist = std::uniform_int_distribution<int>( 1, d );
	
}

int DiceParams::random_side()
{
	return dist( random_engine );
}


int dice( DiceParams& params )
{
	
	int result = 0;
	for ( int i = 0; i < params.rolls_count(); i++ )
	{
		result += params.random_side();
	}
	return ( result * params.multiply_modifier + params.add_modifier );
}