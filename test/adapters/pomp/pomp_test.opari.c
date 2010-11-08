#include <config.h>
#ifdef _POMP2
#  undef _POMP2
#endif
#define _POMP2 200110

#include "pomp_test.c.opari.inc"
#line 1 "pomp_test.c"
/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 *    RWTH Aachen, Germany
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

int
main( int   argc,
      char* argv[] )
{
    int a;
    {
        int pomp_num_threads = omp_get_max_threads();
        POMP2_Parallel_fork( &pomp2_region_1, pomp_num_threads );
#line 25 "pomp_test.c"
  #pragma omp parallel POMP2_DLIST_00001 num_threads(pomp_num_threads) copyin(FORTRAN_MANGLED(pomp_tpd))
        { POMP2_Parallel_begin( &pomp2_region_1 );
#line 26 "pomp_test.c"
          {
              int i;
              POMP2_For_enter( &pomp2_region_2 );
#line 28 "pomp_test.c"
    #pragma omp for nowait
              for ( i = 0; i < 1000; i++ )
              {
                  a++;
              }
              POMP2_Barrier_enter( &pomp2_region_2 );
#pragma omp barrier
              POMP2_Barrier_exit( &pomp2_region_2 );
              POMP2_For_exit( &pomp2_region_2 );
#line 33 "pomp_test.c"

#line 34 "pomp_test.c"
    #pragma omp master
              { POMP2_Master_begin( &pomp2_region_3 );
#line 35 "pomp_test.c"
                {
                    a++;
                }
                POMP2_Master_end( &pomp2_region_3 );
              }
#line 38 "pomp_test.c"

              POMP2_Barrier_enter( &pomp2_region_4 );
#line 39 "pomp_test.c"
    #pragma omp barrier
              POMP2_Barrier_exit( &pomp2_region_4 );
#line 40 "pomp_test.c"

              POMP2_Critical_enter( &pomp2_region_5 );
#line 41 "pomp_test.c"
    #pragma omp critical(test1)
              { POMP2_Critical_begin( &pomp2_region_5 );
#line 42 "pomp_test.c"
                {
                    a++;
                }
                POMP2_Critical_end( &pomp2_region_5 );
              }
              POMP2_Critical_exit( &pomp2_region_5 );
#line 45 "pomp_test.c"

              POMP2_Atomic_enter( &pomp2_region_6 );
#line 46 "pomp_test.c"
    #pragma omp atomic
              a++;
              POMP2_Atomic_exit( &pomp2_region_6 );
#line 48 "pomp_test.c"

              POMP2_Sections_enter( &pomp2_region_7 );
#line 49 "pomp_test.c"
    #pragma omp sections nowait
              {
#line 51 "pomp_test.c"
      #pragma omp section
                  { POMP2_Section_begin( &pomp2_region_7 );
#line 52 "pomp_test.c"
                    {
                        a++;
                    }
                    POMP2_Section_end( &pomp2_region_7 );
                  }
#line 55 "pomp_test.c"
      #pragma omp section
                  { POMP2_Section_begin( &pomp2_region_7 );
#line 56 "pomp_test.c"
                    {
                        a += 2;
                    }
                    POMP2_Section_end( &pomp2_region_7 );
                  }
#line 59 "pomp_test.c"
              }
              POMP2_Barrier_enter( &pomp2_region_7 );
#pragma omp barrier
              POMP2_Barrier_exit( &pomp2_region_7 );
              POMP2_Sections_exit( &pomp2_region_7 );
#line 60 "pomp_test.c"
          }
          POMP2_Barrier_enter( &pomp2_region_1 );
#pragma omp barrier
          POMP2_Barrier_exit( &pomp2_region_1 );
          POMP2_Parallel_end( &pomp2_region_1 );
        }
        POMP2_Parallel_join( &pomp2_region_1 );
    }
#line 61 "pomp_test.c"
    return 0;
}
