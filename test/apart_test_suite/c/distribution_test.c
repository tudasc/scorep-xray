#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include "distribution.h"

int
main( int   argc,
      char* argv[] )
{
    int            sz, i;
    double         low, med, high;
    val1_distr_t   dd1;
    val2_distr_t   dd2;
    val3_distr_t   dd3;
    val2_n_distr_t dd2n;

    if ( argc != 5 )
    {
        fprintf( stderr, "usage: %s <size> <low> <med> <high>\n", argv[ 0 ] );
        exit( 1 );
    }

    sz       = atoi( argv[ 1 ] );
    low      = atof( argv[ 2 ] );
    med      = atof( argv[ 3 ] );
    high     = atof( argv[ 4 ] );
    dd1.val  = dd2.low = dd2n.low = dd3.low = low;
    dd3.med  = med;
    dd2.high = dd2n.high = dd3.high = high;
    dd2n.n   = 0;

    printf( "same:\n" );
    for ( i = 0; i < sz; ++i )
    {
        printf( "%4d: %6.3f\n", i, df_same( i, sz, 1.0, &dd1 ) );
    }
    printf( "cyclic2:\n" );
    for ( i = 0; i < sz; ++i )
    {
        printf( "%4d: %6.3f\n", i, df_cyclic2( i, sz, 1.0, &dd2 ) );
    }
    printf( "block2:\n" );
    for ( i = 0; i < sz; ++i )
    {
        printf( "%4d: %6.3f\n", i, df_block2( i, sz, 1.0, &dd2 ) );
    }
    printf( "linear:\n" );
    for ( i = 0; i < sz; ++i )
    {
        printf( "%4d: %6.3f\n", i, df_linear( i, sz, 1.0, &dd2 ) );
    }
    printf( "peak:\n" );
    for ( i = 0; i < sz; ++i )
    {
        printf( "%4d: %6.3f\n", i, df_peak( i, sz, 1.0, &dd2n ) );
    }
    printf( "cyclic3:\n" );
    for ( i = 0; i < sz; ++i )
    {
        printf( "%4d: %6.3f\n", i, df_cyclic3( i, sz, 1.0, &dd3 ) );
    }
    printf( "block3:\n" );
    for ( i = 0; i < sz; ++i )
    {
        printf( "%4d: %6.3f\n", i, df_block3( i, sz, 1.0, &dd3 ) );
    }
    exit( 0 );
}
