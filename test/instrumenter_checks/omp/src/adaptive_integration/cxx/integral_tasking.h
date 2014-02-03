#ifndef INTEGRAL_TASKING_H
#define INTEGRAL_TASKING_H
#include "function.h"

class integral
{
private:
    double
    integral_par(
        double a,                 /* left interval boundary  */
        double fa,                /* function value at left interval boundary  */
        double b,                 /* right interval boundary */
        double fb,                /* function value at right interval boundary */
        double tolerance );       /* error tolerance */

    function* f;

public:
    integral( function* func );

    double
    evaluate(
        double a,                      /* left interval boundary  */
        double b,                      /* right interval boundary */
        double tolerance );            /* error tolerance */

    int
    get_nr_func_eval();
};
#endif
