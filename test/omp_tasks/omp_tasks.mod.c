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
 * @file       omp_tasks.mod.c
 * @maintainer Dirk Schmidl <schmidl@rz.rwth-aachen.de>
 *
 * @status alpha
 *
 */

#include <config.h>

#ifdef _POMP2
#  undef _POMP2
#endif
#define _POMP2 200110

#include "omp_tasks.c.opari.inc"
#line 1 "/rwthfs/rz/cluster/home/ds534486/SILC/silc-root/branches/TRY_DSCHMIDL_PPHILIPPEN_pomp2_tasking/test/omp_tasks/omp_tasks.c"

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
            int pomp_if = 1;
            { POMP2_Task_handle pomp2_old_task;
              POMP2_Task_create_begin( &pomp2_region_1, &pomp2_old_task, pomp_if, "281*regionType=task*sscl=/rwthfs/rz/cluster/home/ds534486/SILC/silc-root/branches/TRY_DSCHMIDL_PPHILIPPEN_pomp2_tasking/test/omp_tasks/omp_tasks.c:48:48*escl=/rwthfs/rz/cluster/home/ds534486/SILC/silc-root/branches/TRY_DSCHMIDL_PPHILIPPEN_pomp2_tasking/test/omp_tasks/omp_tasks.c:0:0**"  );
#line 48 "/rwthfs/rz/cluster/home/ds534486/SILC/silc-root/branches/TRY_DSCHMIDL_PPHILIPPEN_pomp2_tasking/test/omp_tasks/omp_tasks.c"
#pragma omp task POMP2_DLIST_00001 if(pomp_if) firstprivate(pomp2_old_task)
              { POMP2_Task_begin( &pomp2_region_1, pomp2_old_task );
#line 49 "/rwthfs/rz/cluster/home/ds534486/SILC/silc-root/branches/TRY_DSCHMIDL_PPHILIPPEN_pomp2_tasking/test/omp_tasks/omp_tasks.c"
                {
                    foo( depth - 1 );
                }
                POMP2_Task_end( &pomp2_region_1 );
              }
              POMP2_Task_create_end( &pomp2_region_1, pomp2_old_task );
            }
        }
#line 52 "/rwthfs/rz/cluster/home/ds534486/SILC/silc-root/branches/TRY_DSCHMIDL_PPHILIPPEN_pomp2_tasking/test/omp_tasks/omp_tasks.c"
    }
    { POMP2_Task_handle pomp2_old_task;
      POMP2_Taskwait_begin( &pomp2_region_2, &pomp2_old_task, "285*regionType=taskwait*sscl=/rwthfs/rz/cluster/home/ds534486/SILC/silc-root/branches/TRY_DSCHMIDL_PPHILIPPEN_pomp2_tasking/test/omp_tasks/omp_tasks.c:53:53*escl=/rwthfs/rz/cluster/home/ds534486/SILC/silc-root/branches/TRY_DSCHMIDL_PPHILIPPEN_pomp2_tasking/test/omp_tasks/omp_tasks.c:0:0**"  );
#line 53 "/rwthfs/rz/cluster/home/ds534486/SILC/silc-root/branches/TRY_DSCHMIDL_PPHILIPPEN_pomp2_tasking/test/omp_tasks/omp_tasks.c"
#pragma omp taskwait
      POMP2_Taskwait_end( &pomp2_region_2, pomp2_old_task );
    }
#line 54 "/rwthfs/rz/cluster/home/ds534486/SILC/silc-root/branches/TRY_DSCHMIDL_PPHILIPPEN_pomp2_tasking/test/omp_tasks/omp_tasks.c"
    return;
}


int
main()
{
    printf( "in main\n" );
    {
        int               pomp_num_threads = omp_get_max_threads();
        int               pomp_if          = 1;
        POMP2_Task_handle pomp2_old_task;
        POMP2_Parallel_fork( &pomp2_region_3, pomp_if, pomp_num_threads, &pomp2_old_task, "285*regionType=parallel*sscl=/rwthfs/rz/cluster/home/ds534486/SILC/silc-root/branches/TRY_DSCHMIDL_PPHILIPPEN_pomp2_tasking/test/omp_tasks/omp_tasks.c:62:62*escl=/rwthfs/rz/cluster/home/ds534486/SILC/silc-root/branches/TRY_DSCHMIDL_PPHILIPPEN_pomp2_tasking/test/omp_tasks/omp_tasks.c:0:0**" );
#line 62 "/rwthfs/rz/cluster/home/ds534486/SILC/silc-root/branches/TRY_DSCHMIDL_PPHILIPPEN_pomp2_tasking/test/omp_tasks/omp_tasks.c"
#pragma omp parallel POMP2_DLIST_00003 firstprivate(pomp2_old_task) if(pomp_if) num_threads(pomp_num_threads)
        { POMP2_Parallel_begin( &pomp2_region_3 );
#line 63 "/rwthfs/rz/cluster/home/ds534486/SILC/silc-root/branches/TRY_DSCHMIDL_PPHILIPPEN_pomp2_tasking/test/omp_tasks/omp_tasks.c"
          {
              POMP2_Single_enter( &pomp2_region_4, "283*regionType=single*sscl=/rwthfs/rz/cluster/home/ds534486/SILC/silc-root/branches/TRY_DSCHMIDL_PPHILIPPEN_pomp2_tasking/test/omp_tasks/omp_tasks.c:64:64*escl=/rwthfs/rz/cluster/home/ds534486/SILC/silc-root/branches/TRY_DSCHMIDL_PPHILIPPEN_pomp2_tasking/test/omp_tasks/omp_tasks.c:0:0**"  );
#line 64 "/rwthfs/rz/cluster/home/ds534486/SILC/silc-root/branches/TRY_DSCHMIDL_PPHILIPPEN_pomp2_tasking/test/omp_tasks/omp_tasks.c"
#pragma omp single nowait
              { POMP2_Single_begin( &pomp2_region_4 );
#line 65 "/rwthfs/rz/cluster/home/ds534486/SILC/silc-root/branches/TRY_DSCHMIDL_PPHILIPPEN_pomp2_tasking/test/omp_tasks/omp_tasks.c"
                foo( 10 );
                POMP2_Single_end( &pomp2_region_4 );
              }
              { POMP2_Task_handle pomp2_old_task;
                POMP2_Implicit_barrier_enter( &pomp2_region_4, &pomp2_old_task );
#pragma omp barrier
                POMP2_Implicit_barrier_exit( &pomp2_region_4, pomp2_old_task );
              }
              POMP2_Single_exit( &pomp2_region_4 );
#line 66 "/rwthfs/rz/cluster/home/ds534486/SILC/silc-root/branches/TRY_DSCHMIDL_PPHILIPPEN_pomp2_tasking/test/omp_tasks/omp_tasks.c"
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
#line 67 "/rwthfs/rz/cluster/home/ds534486/SILC/silc-root/branches/TRY_DSCHMIDL_PPHILIPPEN_pomp2_tasking/test/omp_tasks/omp_tasks.c"

    return 0;
}
