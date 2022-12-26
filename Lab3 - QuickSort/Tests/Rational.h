#pragma once

#include <cmath>

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


