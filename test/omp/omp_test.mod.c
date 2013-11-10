/* *INDENT-OFF* */
#include <config.h>
#include "omp_test.c.opari.inc"
#line 1 "omp_test.c"
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
 * @file       omp_test.c
 *
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
        int               pomp2_num_threads = omp_get_max_threads();
        int               pomp2_if          = 1;
        POMP2_Task_handle pomp2_old_task;
        POMP2_Parallel_fork( &pomp2_region_1, pomp2_if, pomp2_num_threads, &pomp2_old_task, pomp2_ctc_1 );
#line 49 "omp_test.c"
#pragma omp parallel POMP2_DLIST_00001 firstprivate(pomp2_old_task) if(pomp2_if) num_threads(pomp2_num_threads) copyin(FORTRAN_MANGLED(pomp_tpd))
        { POMP2_Parallel_begin( &pomp2_region_1 );
#line 50 "omp_test.c"
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
#line 53 "omp_test.c"

    return 0;
}
