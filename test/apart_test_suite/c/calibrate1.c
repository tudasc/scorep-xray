#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

double
secs()
{
    struct timeval tv;
    gettimeofday( &tv, 0 );
    return tv.tv_sec + tv.tv_usec * 1.0e-6;
}

#define ARR_MAX 1024

void
do_work_n( int N )
{
    static int arrA[ ARR_MAX ], arrB[ ARR_MAX ];
    int        i;

    for ( i = 0; i < N; ++i )
    {
        arrA[ rand() % ARR_MAX ] = arrB[ rand() % ARR_MAX ];
    }
}

int
icmp( const void* v1,
      const void* v2 )
{
    int i1 = *( int* )v1;
    int i2 = *( int* )v2;

    return i1 - i2;
}

#define C_N 100000000
#define MAX 13

int
main()
{
    double t1, t2;
    int    i, n[ MAX ], N;
    FILE*  out;

    printf( "calibration1:\n" );
    for ( i = 0; i < MAX; ++i )
    {
        printf( "." );
        fflush( stdout );
        t1 = secs();
        do_work_n( C_N );
        t2     = secs();
        n[ i ] = C_N / ( t2 - t1 );
    }
    printf( "\n" );

    qsort( n, MAX, sizeof( int ), icmp );
    N = n[ MAX / 2 ];
    printf( "using %d\n\n", n[ MAX / 2 ] );

    if ( ( out = fopen( "calibrate.h", "w" ) ) == NULL )
    {
        perror( "calibrate.h" );
        return 1;
    }
    fprintf( out, "#define N_PER_SEC %d\n", N );
    fclose( out );

    return 0;
}
