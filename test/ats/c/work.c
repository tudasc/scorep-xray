#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include "calibrate.h"
#include "work.h"

#define ARR_MAX 1024

static unsigned int my_r = 1676;
static int
myrand()
{
    my_r = ( my_r * 9631 ) % 21599;
    return my_r;
}

void
do_work( double secs )
{
    static int arrA[ ARR_MAX ], arrB[ ARR_MAX ];
    int        i;
    int        N;

    N = N_PER_SEC * secs;

    for ( i = 0; i < N; ++i )
    {
        arrA[ myrand() % ARR_MAX ] = arrB[ myrand() % ARR_MAX ];
    }
}
