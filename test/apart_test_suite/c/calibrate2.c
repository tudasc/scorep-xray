#include <config.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include "calibrate.h"
#include "work.h"

double
secs()
{
    struct timeval tv;
    gettimeofday( &tv, 0 );
    return tv.tv_sec + tv.tv_usec * 1.0e-6;
}

int
dcmp( const void* v1,
      const void* v2 )
{
    int i1 = *( double* )v1;
    int i2 = *( double* )v2;

    return i1 - i2;
}

#define MAX 13

int
main( int   argc,
      char* argv[] )
{
    double t1, t2, d[ MAX ], f;
    int    i, N;
    FILE*  out;

    printf( "calibration%d:\n", atoi( argv[ 1 ] ) );
    for ( i = 0; i < MAX; ++i )
    {
        printf( "." );
        fflush( stdout );
        t1 = secs();
        do_work( 3.42 );
        t2     = secs();
        d[ i ] = 3.42 / ( t2 - t1 );
    }
    printf( "\n" );

    qsort( d, MAX, sizeof( double ), dcmp );
    f = d[ MAX / 2 ];

    if ( fabs( f - 1.0 ) < 5.0e-4 )
    {
        printf( "%g wrong => close enough\n", f );
        return 1;
    }
    else
    {
        N = f * N_PER_SEC;
        printf( "%g wrong => using now %d\n\n", f, N );
    }

    if ( ( out = fopen( "calibrate.h", "w" ) ) == NULL )
    {
        perror( "calibrate.h" );
        return 1;
    }
    fprintf( out, "#define N_PER_SEC %d\n", N );
    fclose( out );

    return 0;
}
