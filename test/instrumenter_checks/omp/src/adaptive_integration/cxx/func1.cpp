#include <math.h>
#include <sys/time.h>
#include <unistd.h>
#include <omp.h>
#include "func1.h"

func1::func1()
{
    f_count = 0;
}

double
func1::eval( double x )
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

int
func1::get_count()
{
    return f_count;
}
