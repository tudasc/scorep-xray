/*
 * This file is part of the SILC project (http://www.silc.de)
 *
 * Copyright (c) 2009-2011,
 *    RWTH Aachen, Germany
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *    Technische Universitaet Dresden, Germany
 *    University of Oregon, Eugene, USA
 *    Forschungszentrum Juelich GmbH, Germany
 *    Technische Universitaet Muenchen, Germany
 *
 * See the COPYING file in the package base directory for details.
 *
 */


/**
 * @file       internal_thread_handling.c
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status alpha
 *
 */


#include <stdio.h>
#include <inttypes.h>
#include "omp_test.c.opari.inc"


#define PRAGMA_OMP_PARALLEL_1( tpd ) _Pragma( STR( omp parallel POMP_DLIST_00001 num_threads( pomp_num_threads ) copyin( tpd ) ) )


void
foo()
{
    printf( "thread %d in foo.      pomp_tpd = %" PRIu64 " \n", omp_get_thread_num(), FORTRAN_MANGLED( pomp_tpd ) );
}


int
main()
{
    printf( "thread %d in main.     pomp_tpd = %" PRIu64 " \n", omp_get_thread_num(), FORTRAN_MANGLED( pomp_tpd ) );
    int pomp_num_threads = omp_get_max_threads();
    POMP_Parallel_fork( pomp_region_1, pomp_num_threads );
    PRAGMA_OMP_PARALLEL_1( FORTRAN_MANGLED( pomp_tpd ) )
    {
        printf( "thread %d before foo.  pomp_tpd = %" PRIu64 " \n", omp_get_thread_num(), FORTRAN_MANGLED( pomp_tpd ) );
        foo();
    }
    printf( "thread %d before join. pomp_tpd = %" PRIu64 " \n", omp_get_thread_num(), FORTRAN_MANGLED( pomp_tpd ) );
    POMP_Parallel_join( pomp_region_1 );
    printf( "thread %d after join.  pomp_tpd = %" PRIu64 " \n", omp_get_thread_num(), FORTRAN_MANGLED( pomp_tpd ) );

    return 0;
}
