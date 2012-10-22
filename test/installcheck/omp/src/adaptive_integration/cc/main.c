#include <stdio.h>
#include <math.h>
#include <sys/time.h>

#ifdef _OPENMP
#include <omp.h>
#endif

#define PI 3.1415926535897932384626433832795028841972

extern int f_count;

double
f( double x );

double
integral(
    double ( * f )( double ),          /* function to integrate */
    double a,                          /* left interval boundary  */
    double b,                          /* right interval boundary */
    double tolerance )                 /* error tolerance */
;

int
main( int argc, char** argv )
{
    double x_min, x_max;
    double answer = 0.0;
    double start, end;
    /*double tm;*/
    /*int i;*/

    x_min = 0.0;
    x_max = 1.0;

    start  = omp_get_wtime();
    answer = integral( f, x_min, x_max, 0.000001 );
    end    = omp_get_wtime();

    printf( "Total time used for integral() %f\n", ( end - start ) );
    printf( "The integral is approximately %.12f\n", answer );
    printf( "The exact answer is           %.12f\n", PI );
    printf( "The error is                  %.12f\n", fabs( PI - answer ) );
    printf( "Number of function evaluations %d\n", f_count );

    return 0;
}
