#include <math.h>
#include <sys/time.h>
#include <unistd.h>
#include <omp.h>

#ifndef DELAY
#define DELAY 0.01
#endif

int f_count = 0;

/* function to integrate */
double
f( double x )
{
    double start, end;
#pragma omp critical(func)
    f_count++;

    start = omp_get_wtime();
    while ( 1 )
    {
        end = omp_get_wtime();
        if ( ( end - start ) > DELAY )
        {
            break;
        }
    }

    return 4.0 / ( 1.0 + x * x );
}
