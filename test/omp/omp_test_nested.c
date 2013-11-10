/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2011,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2011,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2011,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2011,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2011,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2011,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */



/**
 * @file       omp_test_nested.c
 *
 *
 */


#include <config.h>

#include <stdio.h>
#include <omp.h>

#define N 10

int outer, middle;
#pragma omp threadprivate(outer, middle)

float
foo( int n )
{
    int   i;
    float sum, d;

    sum = 0;

    for ( i = 0; i < n; i++ )
    {
        sum += ( float )i;
    }

    d = sum / ( ( float )n * ( float )n + 1.0 );

    //  printf("%f / (%d * %d + 1.0) = %f\n",sum, n, n, d);
    return d;
}


int
main( int argc, char* argv[] )
{
    int   i;
    float sum;

    omp_set_nested( 1 );
    sum = 0;

    for ( i = 0; i < N; i++ )
    {
#pragma omp parallel num_threads(2)
        {
            outer = omp_get_thread_num();
#pragma omp parallel num_threads(2) copyin(outer)
            {
                middle = omp_get_thread_num();
#pragma omp parallel num_threads(2) copyin(outer, middle)
                {
#pragma omp atomic
                    sum += foo( i );
                }
            }
        }
    }
    printf( "Sum=%f\n", sum );

    sum = 0;
    for ( i = 0; i < N; i++ )
    {
#pragma omp parallel num_threads(4)
        {
            outer = omp_get_thread_num();
#pragma omp parallel num_threads(2) copyin(outer)
            {
                middle = omp_get_thread_num();
#pragma omp atomic
                sum += foo( i );
            }
        }
    }
    printf( "Sum=%f\n", sum );

    sum = 0;
    for ( i = 0; i < N; i++ )
    {
#pragma omp parallel num_threads(2)
        {
            outer = omp_get_thread_num();
#pragma omp parallel num_threads(4) copyin(outer)
            {
#pragma omp atomic
                sum += foo( i );
            }
        }
    }

    printf( "Sum=%f\n", sum );

    return 0;
}
