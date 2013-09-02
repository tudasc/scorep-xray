/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 *    RWTH Aachen University, Germany
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *    Technische Universitaet Dresden, Germany
 *    University of Oregon, Eugene, USA
 *    Forschungszentrum Juelich GmbH, Germany
 *    German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *    Technische Universitaet Muenchen, Germany
 *
 * See the COPYING file in the package base directory for details.
 *
 */



/**
 * @file       task_fibonacci.c
 *
 *
 */

#include <stdint.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <scorep/SCOREP_User.h>

uint64_t
fib( uint64_t N )
{
    uint64_t a, b;

    if ( N == 0 )
    {
        return 0;
    }
    if ( N == 1 )
    {
        return 1;
    }

  #pragma omp task shared( a )
    {
        a = fib( N - 1 );
    }

  #pragma omp task shared( b )
    {
        b = fib( N - 2 );
    }

  #pragma omp taskwait

    return a + b;
}

int
main( int argc, char** argv )
{
    SCOREP_USER_FUNC_BEGIN()
    uint64_t N;
    uint64_t F;

    if ( argc < 2 )
    {
        printf( "Calculates the fibonacci number N.\nUsage: %s N\n", argv[ 0 ] );
        SCOREP_USER_FUNC_END()
        exit( EXIT_FAILURE );
    }
    N = atoi( argv[ 1 ] );

  #pragma omp parallel
    {
    #pragma omp single nowait
        {
            F = fib( N );
        }
    }
    printf( "Fibonacci number %" PRIu64 " is %" PRIu64 "\n", N, F );
    SCOREP_USER_FUNC_END()
    return 0;
}
