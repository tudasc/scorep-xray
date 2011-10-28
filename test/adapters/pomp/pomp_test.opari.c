#ifdef _POMP2
#  undef _POMP2
#endif
#define _POMP2 200110

#include <config.h>
#include "pomp_test.c.opari.inc"
#line 1 "/home/peterp/currentwork/silc/tasking/test/adapters/pomp/pomp_test.c"
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

#include <config.h>


int
main( int   argc,
      char* argv[] )
{
    int a;
    {
        int               pomp_num_threads = omp_get_max_threads();
        int               pomp_if          = 1;
        POMP2_Task_handle pomp2_old_task;
        POMP2_Parallel_fork( &pomp2_region_1, pomp_if, pomp_num_threads, &pomp2_old_task, "179*regionType=parallel*sscl=/home/peterp/currentwork/silc/tasking/test/adapters/pomp/pomp_test.c:26:26*escl=/home/peterp/currentwork/silc/tasking/test/adapters/pomp/pomp_test.c:0:0**" );
#line 26 "/home/peterp/currentwork/silc/tasking/test/adapters/pomp/pomp_test.c"
  #pragma omp parallel POMP2_DLIST_00001 firstprivate(pomp2_old_task) if(pomp_if) num_threads(pomp_num_threads)
        { POMP2_Parallel_begin( &pomp2_region_1 );
#line 27 "/home/peterp/currentwork/silc/tasking/test/adapters/pomp/pomp_test.c"
          {
              int i;
              POMP2_For_enter( &pomp2_region_2, "174*regionType=for*sscl=/home/peterp/currentwork/silc/tasking/test/adapters/pomp/pomp_test.c:29:29*escl=/home/peterp/currentwork/silc/tasking/test/adapters/pomp/pomp_test.c:0:0**"  );
#line 29 "/home/peterp/currentwork/silc/tasking/test/adapters/pomp/pomp_test.c"
    #pragma omp for nowait
              for ( i = 0; i < 1000; i++ )
              {
                  a++;
              }
              { POMP2_Task_handle pomp2_old_task;
                POMP2_Implicit_barrier_enter( &pomp2_region_2, &pomp2_old_task );
#pragma omp barrier
                POMP2_Implicit_barrier_exit( &pomp2_region_2, pomp2_old_task );
              }
              POMP2_For_exit( &pomp2_region_2 );
#line 34 "/home/peterp/currentwork/silc/tasking/test/adapters/pomp/pomp_test.c"

#line 35 "/home/peterp/currentwork/silc/tasking/test/adapters/pomp/pomp_test.c"
    #pragma omp master
              { POMP2_Master_begin( &pomp2_region_3, "177*regionType=master*sscl=/home/peterp/currentwork/silc/tasking/test/adapters/pomp/pomp_test.c:35:35*escl=/home/peterp/currentwork/silc/tasking/test/adapters/pomp/pomp_test.c:0:0**"  );
#line 36 "/home/peterp/currentwork/silc/tasking/test/adapters/pomp/pomp_test.c"
                {
                    a++;
                }
                POMP2_Master_end( &pomp2_region_3 );
              }
#line 39 "/home/peterp/currentwork/silc/tasking/test/adapters/pomp/pomp_test.c"

              { POMP2_Task_handle pomp2_old_task;
                POMP2_Barrier_enter( &pomp2_region_4, &pomp2_old_task, "178*regionType=barrier*sscl=/home/peterp/currentwork/silc/tasking/test/adapters/pomp/pomp_test.c:40:40*escl=/home/peterp/currentwork/silc/tasking/test/adapters/pomp/pomp_test.c:0:0**"  );
#line 40 "/home/peterp/currentwork/silc/tasking/test/adapters/pomp/pomp_test.c"
    #pragma omp barrier
                POMP2_Barrier_exit( &pomp2_region_4, pomp2_old_task );
              }
#line 41 "/home/peterp/currentwork/silc/tasking/test/adapters/pomp/pomp_test.c"

              POMP2_Critical_enter( &pomp2_region_5, "198*regionType=critical*sscl=/home/peterp/currentwork/silc/tasking/test/adapters/pomp/pomp_test.c:42:42*escl=/home/peterp/currentwork/silc/tasking/test/adapters/pomp/pomp_test.c:0:0*criticalName=test1**"  );
#line 42 "/home/peterp/currentwork/silc/tasking/test/adapters/pomp/pomp_test.c"
    #pragma omp critical(test1)
              { POMP2_Critical_begin( &pomp2_region_5 );
#line 43 "/home/peterp/currentwork/silc/tasking/test/adapters/pomp/pomp_test.c"
                {
                    a++;
                }
                POMP2_Critical_end( &pomp2_region_5 );
              }
              POMP2_Critical_exit( &pomp2_region_5 );
#line 46 "/home/peterp/currentwork/silc/tasking/test/adapters/pomp/pomp_test.c"

              POMP2_Atomic_enter( &pomp2_region_6, "177*regionType=atomic*sscl=/home/peterp/currentwork/silc/tasking/test/adapters/pomp/pomp_test.c:47:47*escl=/home/peterp/currentwork/silc/tasking/test/adapters/pomp/pomp_test.c:0:0**"  );
#line 47 "/home/peterp/currentwork/silc/tasking/test/adapters/pomp/pomp_test.c"
    #pragma omp atomic
              a++;
              POMP2_Atomic_exit( &pomp2_region_6 );
#line 49 "/home/peterp/currentwork/silc/tasking/test/adapters/pomp/pomp_test.c"

              POMP2_Sections_enter( &pomp2_region_7, "193*regionType=sections*sscl=/home/peterp/currentwork/silc/tasking/test/adapters/pomp/pomp_test.c:50:50*escl=/home/peterp/currentwork/silc/tasking/test/adapters/pomp/pomp_test.c:0:0*numSections=0**"  );
#line 50 "/home/peterp/currentwork/silc/tasking/test/adapters/pomp/pomp_test.c"
    #pragma omp sections nowait
              {
#line 52 "/home/peterp/currentwork/silc/tasking/test/adapters/pomp/pomp_test.c"
      #pragma omp section
                  { POMP2_Section_begin( &pomp2_region_7, "193*regionType=sections*sscl=/home/peterp/currentwork/silc/tasking/test/adapters/pomp/pomp_test.c:50:50*escl=/home/peterp/currentwork/silc/tasking/test/adapters/pomp/pomp_test.c:0:0*numSections=0**"  );
#line 53 "/home/peterp/currentwork/silc/tasking/test/adapters/pomp/pomp_test.c"
                    {
                        a++;
                    }
                    POMP2_Section_end( &pomp2_region_7 );
                  }
#line 56 "/home/peterp/currentwork/silc/tasking/test/adapters/pomp/pomp_test.c"
      #pragma omp section
                  { POMP2_Section_begin( &pomp2_region_7, "193*regionType=sections*sscl=/home/peterp/currentwork/silc/tasking/test/adapters/pomp/pomp_test.c:50:50*escl=/home/peterp/currentwork/silc/tasking/test/adapters/pomp/pomp_test.c:0:0*numSections=1**"  );
#line 57 "/home/peterp/currentwork/silc/tasking/test/adapters/pomp/pomp_test.c"
                    {
                        a += 2;
                    }
                    POMP2_Section_end( &pomp2_region_7 );
                  }
#line 60 "/home/peterp/currentwork/silc/tasking/test/adapters/pomp/pomp_test.c"
              }
              { POMP2_Task_handle pomp2_old_task;
                POMP2_Implicit_barrier_enter( &pomp2_region_7, &pomp2_old_task );
#pragma omp barrier
                POMP2_Implicit_barrier_exit( &pomp2_region_7, pomp2_old_task );
              }
              POMP2_Sections_exit( &pomp2_region_7 );
#line 61 "/home/peterp/currentwork/silc/tasking/test/adapters/pomp/pomp_test.c"
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
#line 62 "/home/peterp/currentwork/silc/tasking/test/adapters/pomp/pomp_test.c"
    return 0;
}
