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
        int               pomp_num_threads = omp_get_max_threads();
        int               pomp_if          = 1;
        POMP2_Task_handle pomp2_old_task;
        POMP2_Parallel_fork( &pomp2_region_1, pomp_if, pomp_num_threads, &pomp2_old_task, "157*regionType=parallel*sscl=/home/peterp/currentwork/silc/tasking/test/omp/omp_test.c:49:49*escl=/home/peterp/currentwork/silc/tasking/test/omp/omp_test.c:0:0**" );
#line 49 "/home/peterp/currentwork/silc/tasking/test/omp/omp_test.c"
#pragma omp parallel POMP2_DLIST_00001 firstprivate(pomp2_old_task) if(pomp_if) num_threads(pomp_num_threads)
        { POMP2_Parallel_begin( &pomp2_region_1 );
#line 50 "/home/peterp/currentwork/silc/tasking/test/omp/omp_test.c"
          {
              foo();
          }
          { POMP2_Task_handle pomp2_old_task;
            POMP2_Implicit_barrier_enter( &pomp2_region_1, &pomp2_old_task );
#pragma omp barrier
            POMP2_Implicit_barrier_exit( &pomp2_region_1, pomp2_old_task );
          }
          POMP2_Parallel_end( &pomp2_region_1 );
        }
        POMP2_Parallel_join( &pomp2_region_1, pomp2_old_task );
    }
#line 53 "/home/peterp/currentwork/silc/tasking/test/omp/omp_test.c"

    return 0;
}
