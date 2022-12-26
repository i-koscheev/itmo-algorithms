#include "pch.h"
#include <random>

#include "../Sort.h"

constexpr int LENGTH = 5000;

TEST( InsertionSortTest, OneElement )
{
	const auto a = new int[1];
	a[0] = 42;
	insertion_sort( a, a + 0, []( int a, int b ) {return a < b; } );
	EXPECT_TRUE( a[0] == 42 );
}

TEST( QuickSortTest, OneElement )
{
	const auto a = new int[1];
	a[0] = 42;
	quick_sort( a, a + 0, []( int a, int b ) {return a < b; } );
	EXPECT_TRUE( a[0] == 42 );
}


TEST( InsertionSortTest, Int )
{
	std::default_random_engine RandomEngine( time( 0 ) );
	std::uniform_int_distribution<int> RandomIntGenerator( 1, 100 );

	const auto a = new int[LENGTH];

	for ( int i = 0; i < LENGTH; i++ )
	{
		a[i] = RandomIntGenerator( RandomEngine );
	}

	insertion_sort( a, a + LENGTH - 1, []( int a, int b ) {return a < b; } );

	for ( int i = 0; i < LENGTH - 2; i++ )
	{
		EXPECT_TRUE( a[i] <= a[i + 1] );
	}
}

TEST( QuickSortTest, Int )
{
	std::default_random_engine RandomEngine( time( 0 ) );
	std::uniform_int_distribution<int> RandomIntGenerator( 1, 100 );

	const auto a = new int[LENGTH];

	for ( int i = 0; i < LENGTH; i++ )
	{
		a[i] = RandomIntGenerator( RandomEngine );
	}

	quick_sort( a, a + LENGTH - 1, []( int a, int b ) {return a < b; } );

	for ( int i = 0; i < LENGTH - 2; i++ )
	{
		EXPECT_TRUE( a[i] <= a[i + 1] );
	}
}

TEST( HybridSortTest, Int )
{
	std::default_random_engine RandomEngine( time( 0 ) );
	std::uniform_int_distribution<int> RandomIntGenerator( 1, 100 );

	const auto a = new int[LENGTH];

	for ( int i = 0; i < LENGTH; i++ )
	{
		a[i] = RandomIntGenerator( RandomEngine );
	}

	hybrid_sort( a, a + LENGTH - 1, []( int a, int b ) {return a < b; } );

	for ( int i = 0; i < LENGTH - 2; i++ )
	{
		EXPECT_TRUE( a[i] <= a[i + 1] );
	}
}


TEST( InsertionSortTest, IntReverse )
{
	std::default_random_engine RandomEngine( time( 0 ) );
	std::uniform_int_distribution<int> RandomIntGenerator( 1, 100 );

	const auto a = new int[LENGTH];

	for ( int i = 0; i < LENGTH; i++ )
	{
		a[i] = RandomIntGenerator( RandomEngine );
	}

	insertion_sort( a, a + LENGTH - 1, []( int a, int b ) {return a > b; } );

	for ( int i = 0; i < LENGTH - 2; i++ )
	{
		EXPECT_TRUE( a[i] >= a[i + 1] );
	}
}

TEST( QuickSortTest, IntReverse )
{
	std::default_random_engine RandomEngine( time( 0 ) );
	std::uniform_int_distribution<int> RandomIntGenerator( 1, 100 );

	const auto a = new int[LENGTH];

	for ( int i = 0; i < LENGTH; i++ )
	{
		a[i] = RandomIntGenerator( RandomEngine );
	}

	quick_sort( a, a + LENGTH - 1, []( int a, int b ) {return a > b; } );

	for ( int i = 0; i < LENGTH - 2; i++ )
	{
		EXPECT_TRUE( a[i] >= a[i + 1] );
	}
}

TEST( HybridSortTest, IntReverse )
{
	std::default_random_engine RandomEngine( time( 0 ) );
	std::uniform_int_distribution<int> RandomIntGenerator( 1, 100 );

	const auto a = new int[LENGTH];

	for ( int i = 0; i < LENGTH; i++ )
	{
		a[i] = RandomIntGenerator( RandomEngine );
	}

	hybrid_sort( a, a + LENGTH - 1, []( int a, int b ) {return a > b; } );

	for ( int i = 0; i < LENGTH - 2; i++ )
	{
		EXPECT_TRUE( a[i] >= a[i + 1] );
	}
}



int nod( int a, int b )
{
	while ( a && b )
		if ( a > b )
			a %= b;
		else
			b %= a;
	return ( a + b );
}

class rational
{
public:
	rational( int numerator = 0, int denominator = 1 );
	int operator == ( const rational& x ) const { return a == x.a && b == x.b; }
	int operator != ( const rational& x ) const { return !( *this == x ); }
	int operator < ( const rational& x ) const { int n = nod( b, x.b ); return a * ( x.b / n ) < x.a * ( b / n ); } 
	int operator > ( const rational& x ) const { int n = nod( b, x.b ); return a * ( x.b / n ) > x.a * ( b / n ); } 
	int operator <= ( const rational& x ) const { return !( *this > x ); }
	int operator >= ( const rational& x ) const { return !( *this < x ); }
private:
	int a,
		b;
	void reduction();
};

rational::rational( int numerator, int denominator )
{
	a = ( denominator < 0 ) ? ( -numerator ) : numerator;
	b = ( denominator && a ) ? abs( denominator ) : 1;
	reduction();
}

void rational::reduction()
{
	int n = nod( abs( a ), b );
	a /= n;
	b /= n;
}


TEST( InsertionSortTest, Rational )
{
	std::default_random_engine RandomEngine( time( 0 ) );
	std::uniform_int_distribution<int> RandomIntGenerator( 1, 10 );

	const auto a = new rational[LENGTH];

	for ( int i = 0; i < LENGTH; i++ )
	{
		a[i] = rational(
			RandomIntGenerator( RandomEngine ),
			RandomIntGenerator( RandomEngine ) + 1
		);
	}

	insertion_sort( a, a + LENGTH - 1, []( rational a, rational b ) {return a < b; } );

	for ( int i = 0; i < LENGTH - 2; i++ )
	{
		EXPECT_TRUE( a[i] <= a[i + 1] );
	}
}


TEST( QuickSortTest, Rational )
{
	std::default_random_engine RandomEngine( time( 0 ) );
	std::uniform_int_distribution<int> RandomIntGenerator( 1, 10 );

	const auto a = new rational[LENGTH];

	for ( int i = 0; i < LENGTH; i++ )
	{
		a[i] = rational(
			RandomIntGenerator( RandomEngine ),
			RandomIntGenerator( RandomEngine ) + 1
		);
	}

	quick_sort( a, a + LENGTH - 1, []( rational a, rational b ) {return a < b; } );

	for ( int i = 0; i < LENGTH - 2; i++ )
	{
		EXPECT_TRUE( a[i] <= a[i + 1] );
	}
}


TEST( HybridSortTest, Rational )
{
	std::default_random_engine RandomEngine( time( 0 ) );
	std::uniform_int_distribution<int> RandomIntGenerator( 1, 10 );

	const auto a = new rational[LENGTH];

	for ( int i = 0; i < LENGTH; i++ )
	{
		a[i] = rational(
			RandomIntGenerator( RandomEngine ),
			RandomIntGenerator( RandomEngine ) + 1
		);
	}

	hybrid_sort( a, a + LENGTH - 1, []( rational a, rational b ) {return a < b; } );

	for ( int i = 0; i < LENGTH - 2; i++ )
	{
		EXPECT_TRUE( a[i] <= a[i + 1] );
	}
}