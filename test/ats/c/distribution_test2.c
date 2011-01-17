#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include "distribution.h"

int
main( int   argc,
      char* argv[] )
{
    int          sz, i;
    distr_func_t df;
    distr_t*     dd;

    if ( argc != 3 )
    {
        fprintf( stderr, "usage: %s <size> <descr>\n", argv[ 0 ] );
        return 1;
    }

    sz = atoi( argv[ 1 ] );
    df = atodf( argv[ 2 ] );
    dd = atodd( argv[ 2 ] );

    if ( df == 0 )
    {
        fprintf( stderr, "unknown distribution function\n" );
        return 1;
    }

    for ( i = 0; i < sz; ++i )
    {
        printf( "%4d: %6.3f\n", i, df( i, sz, 1.0, dd ) );
    }
    return 0;
}
