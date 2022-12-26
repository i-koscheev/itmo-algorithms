#include "pch.h"

#include "../Array.h"


TEST( ArrayTest, DefaultConstructor )
{
    Array<int> A;
    EXPECT_EQ( A.length(), 0 );

    Array<std::string> B;
    EXPECT_EQ( B.length(), 0 );
}


TEST( ArrayTest, InsertAtTheEnd )
{
    Array<int> A;
    EXPECT_EQ( A.insert( 100 ), 0 );
    EXPECT_EQ( A.length(), 1 );
    EXPECT_EQ( A[0], 100 );
    EXPECT_EQ( A.insert( 42 ), 1 );
    EXPECT_EQ( A.length(), 2 );
    EXPECT_EQ( A[1], 42 );
    
    Array<std::string> B;
    EXPECT_EQ( B.insert( "Hello world" ), 0 );
    EXPECT_EQ( B.length(), 1 );
    EXPECT_EQ( B[0], "Hello world" );
}


TEST( ArrayTest, InsertInPosition )
{
    Array<int> A;
    A.insert( 0 );
    A.insert( 1 );
    A.insert( 2 );
    A.insert( 3 );
    EXPECT_EQ( A.insert( 2, 42 ), 2 );
    EXPECT_EQ( A.insert( 0, -1 ), 0 );
    EXPECT_EQ( A[3], 42 );
    EXPECT_EQ( A[1], 0 );
    EXPECT_EQ( A.length(), 6 );
    EXPECT_EQ( A.insert( 100 ), 6 );

    Array<std::string> B;
    B.insert( "0" );
    B.insert( "1" );
    B.insert( "2" );
    EXPECT_EQ( B.insert( 2, "Hello world" ), 2 );
    EXPECT_EQ( B.insert( 0, "Zero" ), 0 );
    EXPECT_EQ( B[4], "2" );
    EXPECT_EQ( B[0], "Zero" );
    EXPECT_EQ( B.length(), 5 );
}


TEST( ArrayTest, Capacity )
{
    Array<int> A(100);
    for ( int i = 0; i < 100000; i++ )
        A.insert( i + 1 );
    EXPECT_EQ( A.length(), 100000 );
    EXPECT_EQ( A[99999], 100000 );

    Array<std::string> B;
    for ( int i = 0; i < 100; i++ )
        B.insert( std::to_string( i + 1 ) );
    EXPECT_EQ( B.length(), 100 );
    EXPECT_EQ( B[99], "100" );
}


TEST( ArrayTest, Remove )
{
    Array<int> A;
    A.insert( 0 );
    A.insert( 1 );
    A.insert( 2 );
    A.insert( 3 );

    A.remove( 0 );
    EXPECT_EQ( A[0], 1 );
    A.remove( A.length() - 1 );
    EXPECT_EQ( A[A.length() - 1], 2 );
    EXPECT_EQ( A.length(), 2 );

    Array<std::string> B;
    B.insert( "0" );
    B.insert( "1" );
    B.insert( "2" );
    B.insert( "3" );
    B.insert( "4" );

    B.remove( B.length() - 1 );
    EXPECT_EQ( B[B.length() - 1], "3" );
    B.remove( 0 );
    EXPECT_EQ( B[0], "1" );
    EXPECT_EQ( B.length(), 3 );
}


TEST( ArrayTest, CopyConstructor )
{
    Array<int> A;
    A.insert( 0 );
    A.insert( 1 );
    A.insert( 2 );
    
    Array<int> X( A );

    EXPECT_EQ( A.length(), X.length() );
    for ( int i = 0; i < A.length(); i++ )
    {
        EXPECT_EQ( A[i], X[i] );
    }

    Array<std::string> B;
    B.insert( "0" );
    B.insert( "1" );
    B.insert( "2" );
    
    Array<std::string> Y( B );
 
    EXPECT_EQ( B.length(), Y.length() );
    for ( int i = 0; i < Y.length(); i++ )
    {
        EXPECT_EQ( B[i], Y[i] );
    }
}


TEST( ArrayTest, MoveConstructor )
{
    Array<int> A;
    A.insert( 0 );
    A.insert( 1 );
    A.insert( 2 );
    Array<int> CopyA( A );

    Array<int> X( std::move( CopyA ) );

    EXPECT_EQ( A.length(), X.length() );
    for ( int i = 0; i < A.length(); i++ )
    {
        EXPECT_EQ( A[i], X[i] );
    }

    Array<std::string> B;
    B.insert( "0" );
    B.insert( "1" );
    B.insert( "2" );
    Array<std::string> CopyB( B );

    Array<std::string> Y( std::move( CopyB ) );

    EXPECT_EQ( B.length(), Y.length() );
    for ( int i = 0; i < Y.length(); i++ )
    {
        EXPECT_EQ( B[i], Y[i] );
    }
}


TEST( ArrayTest, CopyAssignment )
{
    Array<int> A;
    A.insert( 0 );
    A.insert( 1 );
    A.insert( 2 );
    
    Array<int> X;
    X = A;
    
    EXPECT_EQ( A.length(), X.length() );
    for ( int i = 0; i < A.length(); i++ )
    {
        EXPECT_EQ( A[i], X[i] );
    }

    Array<std::string> B;
    B.insert( "0" );
    B.insert( "1" );
    B.insert( "2" );

    Array<std::string> Y;
    Y = B;
   
    EXPECT_EQ( B.length(), Y.length() );
    for ( int i = 0; i < Y.length(); i++ )
    {
        EXPECT_EQ( B[i], Y[i] );
    }
}


TEST( ArrayTest, MoveAssignment )
{
    Array<int> A;
    A.insert( 0 );
    A.insert( 1 );
    A.insert( 2 );
    Array<int> CopyA( A );

    Array<int> X;
    X = std::move( CopyA );

    EXPECT_EQ( A.length(), X.length() );
    for ( int i = 0; i < A.length(); i++ )
    {
        EXPECT_EQ( A[i], X[i] );
    }

    Array<std::string> B;
    B.insert( "0" );
    B.insert( "1" );
    B.insert( "2" );
    Array<std::string> CopyB( B );

    Array<std::string> Y;
    Y = std::move( CopyB );

    EXPECT_EQ( B.length(), Y.length() );
    for ( int i = 0; i < Y.length(); i++ )
    {
        EXPECT_EQ( B[i], Y[i] );
    }
}


TEST( ArrayTest, Iterator )
{
    int i;
    Array<int> A;
    for ( i = 0; i < 10; i++ )
        A.insert( i );
   
    i = 0;
    for ( auto it = A.iterator(); it.hasCurrent(); it.next() )
    {
        EXPECT_EQ( it.get(), i );
        i++;
    }

    Array<std::string> B;
    for ( int i = 0; i < 10; i++ )
        B.insert( std::to_string( i ) );
    
    i = 0;
    for ( auto it = B.iterator(); it.hasCurrent(); it.next() )
    {
        EXPECT_EQ( it.get(), std::to_string( i ) );
        i++;
    }
}


TEST( ArrayTest, ReverseIterator )
{
    int i;
    Array<int> A;
    for ( i = 0; i < 10; i++ )
        A.insert( i );

    i = 10;
    for ( auto it = A.reverseIterator(); it.hasCurrent(); it.next() )
    {
        i--;
        EXPECT_EQ( it.get(), i );
    }

    Array<std::string> B;
    for ( int i = 0; i < 10; i++ )
        B.insert( std::to_string( i ) );

    i = 10;
    for ( auto it = B.reverseIterator(); it.hasCurrent(); it.next() )
    {
        i--;
        EXPECT_EQ( it.get(), std::to_string( i ) ); 
    }
}