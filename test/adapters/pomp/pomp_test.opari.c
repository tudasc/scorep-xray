#ifdef _POMP
#  undef _POMP
#endif
#define _POMP 200110

#include "pomp_test.c.opari.inc"
#line 1 "pomp_test.c"

int
main( int   argc,
      char* argv[] )
{
    POMP_Init();
    int a;
    POMP_Parallel_fork( pomp_region_1 );
#line 6 "pomp_test.c"
  #pragma omp parallel POMP_DLIST_00001
    {
        POMP_Parallel_begin( pomp_region_1 );
#line 7 "pomp_test.c"
        {
            int i;
            POMP_For_enter( pomp_region_2 );
#line 9 "pomp_test.c"
    #pragma omp for nowait
            for ( i = 0; i < 1000; i++ )
            {
                a++;
            }
            POMP_Barrier_enter( pomp_region_2 );
#pragma omp barrier
            POMP_Barrier_exit( pomp_region_2 );
            POMP_For_exit( pomp_region_2 );
#line 14 "pomp_test.c"

#line 15 "pomp_test.c"
    #pragma omp master
            { POMP_Master_begin( pomp_region_3 );
#line 16 "pomp_test.c"
              {
                  a++;
              }
              POMP_Master_end( pomp_region_3 );
            }
#line 19 "pomp_test.c"

            POMP_Barrier_enter( pomp_region_4 );
#line 20 "pomp_test.c"
    #pragma omp barrier
            POMP_Barrier_exit( pomp_region_4 );
#line 21 "pomp_test.c"

            POMP_Critical_enter( pomp_region_5 );
#line 22 "pomp_test.c"
#pragma omp critical(test1)
            { POMP_Critical_begin( pomp_region_5 );
#line 23 "pomp_test.c"
              {
                  a++;
              }
              POMP_Critical_end( pomp_region_5 );
            }
            POMP_Critical_exit( pomp_region_5 );
#line 26 "pomp_test.c"

            POMP_Atomic_enter( pomp_region_6 );
#line 27 "pomp_test.c"
    #pragma omp atomic
            a++;
            POMP_Atomic_exit( pomp_region_6 );
#line 29 "pomp_test.c"

            POMP_Sections_enter( pomp_region_7 );
#line 30 "pomp_test.c"
    #pragma omp sections nowait
            {
#line 32 "pomp_test.c"
      #pragma omp section
                { POMP_Section_begin( pomp_region_7 );
#line 33 "pomp_test.c"
                  {
                      a++;
                  }
                  POMP_Section_end( pomp_region_7 );
                }
#line 36 "pomp_test.c"
      #pragma omp section
                { POMP_Section_begin( pomp_region_7 );
#line 37 "pomp_test.c"
                  {
                      a += 2;
                  }
                  POMP_Section_end( pomp_region_7 );
                }
#line 40 "pomp_test.c"
            }
            POMP_Barrier_enter( pomp_region_7 );
#pragma omp barrier
            POMP_Barrier_exit( pomp_region_7 );
            POMP_Sections_exit( pomp_region_7 );
#line 41 "pomp_test.c"
        }
        POMP_Barrier_enter( pomp_region_1 );
#pragma omp barrier
        POMP_Barrier_exit( pomp_region_1 );
        POMP_Parallel_end( pomp_region_1 );
    }
    POMP_Parallel_join( pomp_region_1 );
#line 42 "pomp_test.c"
    POMP_Finalize();
}
