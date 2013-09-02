/* *INDENT-OFF* */
#include <config.h>
#include "omp_tasks.c.opari.inc"
#line 1 "omp_tasks.c"
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
 * @file       omp_tasks.c
 *
 *
 */



#include <stdio.h>


/* *INDENT-OFF* */
/* *INDENT-ON*  */


void
foo( int depth )
{
    printf( "in foo; depth=%d\n", depth );
    if ( depth == 1 )
    {
        return;
    }
    else
    {
        {
            int pomp2_if = 1;
            { POMP2_Task_handle pomp2_old_task;
              POMP2_Task_handle pomp2_new_task;
              if ( pomp2_if )
              {
                  POMP2_Task_create_begin( &pomp2_region_1, &pomp2_new_task, &pomp2_old_task, pomp2_if, pomp2_ctc_1  );
              }
#line 48 "omp_tasks.c"
#pragma omp task POMP2_DLIST_00001 if(pomp2_if) firstprivate(pomp2_new_task, pomp2_if)
              { if ( pomp2_if )
                {
                    POMP2_Task_begin( &pomp2_region_1, pomp2_new_task );
                }
#line 49 "omp_tasks.c"
                {
                    foo( depth - 1 );
                }
                if ( pomp2_if )
                {
                    POMP2_Task_end( &pomp2_region_1 );
                }
              }
              if ( pomp2_if )
              {
                  POMP2_Task_create_end( &pomp2_region_1, pomp2_old_task );
              }
            }
        }
#line 52 "omp_tasks.c"
    }
    { POMP2_Task_handle pomp2_old_task;
      POMP2_Taskwait_begin( &pomp2_region_2, &pomp2_old_task, pomp2_ctc_2  );
#line 53 "omp_tasks.c"
#pragma omp taskwait
      POMP2_Taskwait_end( &pomp2_region_2, pomp2_old_task );
    }
#line 54 "omp_tasks.c"
    return;
}


int
main()
{
    printf( "in main\n" );
    {
        int               pomp2_num_threads = omp_get_max_threads();
        int               pomp2_if          = 1;
        POMP2_Task_handle pomp2_old_task;
        POMP2_Parallel_fork( &pomp2_region_3, pomp2_if, pomp2_num_threads, &pomp2_old_task, pomp2_ctc_3 );
#line 62 "omp_tasks.c"
#pragma omp parallel POMP2_DLIST_00003 firstprivate(pomp2_old_task) if(pomp2_if) num_threads(pomp2_num_threads) copyin(FORTRAN_MANGLED(pomp_tpd))
        { POMP2_Parallel_begin( &pomp2_region_3 );
#line 63 "omp_tasks.c"
          {
              { POMP2_Single_enter( &pomp2_region_4, pomp2_ctc_4  );
#line 64 "omp_tasks.c"
#pragma omp single nowait
                { POMP2_Single_begin( &pomp2_region_4 );
#line 65 "omp_tasks.c"
                  foo( 10 );
                  POMP2_Single_end( &pomp2_region_4 );
                }
                { POMP2_Task_handle pomp2_old_task;
                  POMP2_Implicit_barrier_enter( &pomp2_region_4, &pomp2_old_task );
#pragma omp barrier
                  POMP2_Implicit_barrier_exit( &pomp2_region_4, pomp2_old_task );
                }
                POMP2_Single_exit( &pomp2_region_4 );
              }
#line 66 "omp_tasks.c"
          }
          { POMP2_Task_handle pomp2_old_task;
            POMP2_Implicit_barrier_enter( &pomp2_region_3, &pomp2_old_task );
#pragma omp barrier
            POMP2_Implicit_barrier_exit( &pomp2_region_3, pomp2_old_task );
          }
          POMP2_Parallel_end( &pomp2_region_3 );
        }
        POMP2_Parallel_join( &pomp2_region_3, pomp2_old_task );
    }
#line 67 "omp_tasks.c"

    return 0;
}
