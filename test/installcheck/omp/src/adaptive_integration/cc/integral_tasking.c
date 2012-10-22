#include <stdio.h>
#include <math.h>

#ifdef _OPENMP
#include <omp.h>
#endif

double
integral_par(
    double ( * f )(),          /* function to integrate */
    double a,                  /* left interval boundary  */
    double fa,                 /* function value at left interval boundary  */
    double b,                  /* right interval boundary */
    double fb,                 /* function value at right interval boundary */
    double tolerance )         /* error tolerance */
{
    double integral_result;    /* value to return */
    double h;                  /* interval size */
    double mid;                /* center of interval */
    double fmid;               /* function value at center of interval */
    double one_trapezoid_area; /* area of one trapezoid */
    double two_trapezoid_area; /* area of two trapezoids */
    double left_area;          /* integral of left half interval */
    double right_area;         /*     "       right    "         */


    h                  = b - a;
    mid                = ( a + b ) / 2;
    fmid               = f( mid );
    one_trapezoid_area = h * ( fa + fb ) / 2.0;
    two_trapezoid_area = h / 2 * ( fa + fmid ) / 2.0 +
                         h / 2 * ( fmid + fb ) / 2.0;

    if ( fabs( one_trapezoid_area - two_trapezoid_area ) < 3.0 * tolerance )
    {
        /* error acceptable   */
        integral_result = two_trapezoid_area;
    }
    else
    {
        /* error not acceptable */
        /* put recursiv function calls for left and right areas
           into task queue */

#pragma omp task shared(left_area)
        {
            left_area = integral_par( f, a, fa, mid, fmid, tolerance / 2 );
        }

#pragma omp task shared(right_area)
        {
            right_area = integral_par( f, mid, fmid, b, fb, tolerance / 2 );
        }
#pragma omp taskwait
        integral_result = left_area + right_area;
    }

    return integral_result;
}




double
integral(
    double ( * f )( double ),          /* function to integrate */
    double a,                          /* left interval boundary  */
    double b,                          /* right interval boundary */
    double tolerance )                 /* error tolerance */
{
    double answer = 0.0;

#pragma omp parallel
    {
#pragma omp master
        {
            //printf("# of threads; %d\n",omp_get_num_threads());

            /* trying to avoid calling the first integral function from all threads */
            answer = integral_par( f, a, f( a ), b, f( b ), tolerance );
        }
    } /* omp parallel */
    return answer;
}
