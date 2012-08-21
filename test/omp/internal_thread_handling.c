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
 * @file       internal_thread_handling.c
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status alpha
 *
 */


#include <config.h>

#include <omp.h>
#include <opari2/pomp2_lib.h>
#include <stdio.h>
#include <inttypes.h>

#define POMP_TPD_MANGLED FORTRAN_MANGLED( pomp_tpd )
#define SCOREP_PRAGMA_STR_( str ) #str
#define SCOREP_PRAGMA_STR( str ) SCOREP_PRAGMA_STR_( str )
#define PRAGMA_OP( x ) _Pragma( x )
#define SCOREP_PRAGMA( pragma ) PRAGMA_OP( SCOREP_PRAGMA_STR( pragma ) )
#define SCOREP_PRAGMA_OMP( omp_pragma ) SCOREP_PRAGMA( omp omp_pragma )

extern int64_t FORTRAN_ALIGNED POMP_TPD_MANGLED;
SCOREP_PRAGMA_OMP( threadprivate( POMP_TPD_MANGLED ) )

#define POMP_DLIST_00001 shared( pomp_region_1 )
static POMP2_Region_handle pomp_region_1;

void
POMP2_Init_reg_1320069835786106_1()
{
    POMP2_Assign_handle( &pomp_region_1, "66*regionType=parallel*sscl=omp_test.c:45:45*escl=omp_test.c:48:48**" );
}

void
foo()
{
    printf( "thread %d in foo.      pomp_tpd = %" PRIu64 " \n", omp_get_thread_num(), FORTRAN_MANGLED( pomp_tpd ) );
}


int
main()
{
    printf( "thread %d in main.     pomp_tpd = %" PRIu64 " \n", omp_get_thread_num(), FORTRAN_MANGLED( pomp_tpd ) );
    int     pomp_num_threads = omp_get_max_threads();
    int64_t pomp2_old_task   = 0;
    POMP2_Parallel_fork( &pomp_region_1, 1, pomp_num_threads, &pomp2_old_task, "" );
    SCOREP_PRAGMA_OMP( parallel POMP_DLIST_00001 num_threads( pomp_num_threads ) copyin( FORTRAN_MANGLED( pomp_tpd ) ) )
    {
        printf( "thread %d before foo.  pomp_tpd = %" PRIu64 " \n", omp_get_thread_num(), FORTRAN_MANGLED( pomp_tpd ) );
        foo();
    }
    printf( "thread %d before join. pomp_tpd = %" PRIu64 " \n", omp_get_thread_num(), FORTRAN_MANGLED( pomp_tpd ) );
    POMP2_Parallel_join( &pomp_region_1, 0 );
    printf( "thread %d after join.  pomp_tpd = %" PRIu64 " \n", omp_get_thread_num(), FORTRAN_MANGLED( pomp_tpd ) );

    return 0;
}
