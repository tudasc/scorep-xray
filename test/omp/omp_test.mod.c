/*
 * This file is part of the SILC project (http://www.silc.de)
 *
 * Copyright (c) 2009-2011,
 *    RWTH Aachen, Germany
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *    Technische Universitaet Dresden, Germany
 *    University of Oregon, Eugene USA
 *    Forschungszentrum Juelich GmbH, Germany
 *    Technische Universitaet Muenchen, Germany
 *
 * See the COPYING file in the package base directory for details.
 *
 */

#ifdef _POMP
#  undef _POMP
#endif
#define _POMP 200110

#include <config.h>

#include "omp_test.c.opari.inc"
#line 1 "omp_test.c"


/**
 * @file       omp_test.c
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status alpha
 *
 */


#include <stdio.h>


/* *INDENT-OFF* */
/* *INDENT-ON*  */


#define PRAGMA_OMP_PARALLEL_1( tpd ) _Pragma( STR( omp parallel POMP_DLIST_00001 num_threads( pomp_num_threads ) copyin( tpd ) ) )
#define PRAGMA_OMP_PARALLEL_2( tpd ) _Pragma( STR( omp parallel POMP_DLIST_00003 num_threads( pomp_num_threads ) copyin( tpd ) ) )
#define PRAGMA_OMP_PARALLEL_3( tpd ) _Pragma( STR( omp parallel private ( i, j, xx, yy, xx2, yy2 ) POMP_DLIST_00001 num_threads( pomp_num_threads ) copyin( tpd ) ) )
#define PRAGMA_OMP_PARALLEL_4( tpd ) _Pragma( STR( omp parallel private ( j, i ) POMP_DLIST_00003 num_threads( pomp_num_threads ) copyin( tpd ) ) )


void
foo()
{
    printf( "thread %d in foo\n", omp_get_thread_num() );
}


int
main()
{
    printf( "in main\n" );
    {
        int pomp_num_threads = omp_get_max_threads();
        POMP2_Parallel_fork( pomp_region_1, pomp_num_threads );
#line 45 "omp_test.c"
        PRAGMA_OMP_PARALLEL_1( FORTRAN_MANGLED( pomp_tpd ) )
        {
            POMP2_Parallel_begin( pomp_region_1 );
#line 46 "omp_test.c"
            {
                foo();
            }
            POMP2_Barrier_enter( pomp_region_1 );
#pragma omp barrier
            POMP2_Barrier_exit( pomp_region_1 );
            POMP2_Parallel_end( pomp_region_1 );
        }
        POMP2_Parallel_join( pomp_region_1 );
    }
#line 49 "omp_test.c"

    return 0;
}
