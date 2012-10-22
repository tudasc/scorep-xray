#include <stdio.h>
#include <math.h>
#include <sys/time.h>
#include <omp.h>
#include "func1.h"
#include "integral_tasking.h"
#define PI 3.1415926535897932384626433832795028841972

int
main( int argc, char** argv )
{
    double x_min, x_max;
    double answer = 0.0;
    double start, end;
    /*function to integrate*/
    func1    f;
    integral i( &f );

    x_min = 0.0;
    x_max = 1.0;

    start  = omp_get_wtime();
    answer = i.evaluate( x_min, x_max, 0.000001 );
    end    = omp_get_wtime();

    printf( "Total time used for integral() %f\n", ( end - start ) );
    printf( "The integral is approximately %.12f\n", answer );
    printf( "The exact answer is           %.12f\n", PI );
    printf( "The error is                  %.12f\n", fabs( PI - answer ) );
    printf( "Number of function evaluations %d\n", i.get_nr_func_eval() );

    return 0;
}
