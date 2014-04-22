/* *INDENT-OFF* */
#include <config.h>
#include "oa_omp_test.c.opari.inc"
#line 1 "oa_omp_test.c"
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

#include <stdio.h>
#include <scorep/SCOREP_User.h>

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
if (pomp2_if)  POMP2_Task_create_begin( &pomp2_region_1, &pomp2_new_task, &pomp2_old_task, pomp2_if, pomp2_ctc_1  );
#line 33 "oa_omp_test.c"
#pragma omp task POMP2_DLIST_00001 if(pomp2_if) firstprivate(pomp2_new_task, pomp2_if)
{ if (pomp2_if)  POMP2_Task_begin( &pomp2_region_1, pomp2_new_task );
#line 34 "oa_omp_test.c"
        {
            foo( depth - 1 );
        }
if (pomp2_if)  POMP2_Task_end( &pomp2_region_1 ); }
if (pomp2_if)  POMP2_Task_create_end( &pomp2_region_1, pomp2_old_task ); }
}
#line 37 "oa_omp_test.c"
    }
{ POMP2_Task_handle pomp2_old_task;
  POMP2_Taskwait_begin( &pomp2_region_2, &pomp2_old_task, pomp2_ctc_2  );
#line 38 "oa_omp_test.c"
#pragma omp taskwait
  POMP2_Taskwait_end( &pomp2_region_2, pomp2_old_task ); }
#line 39 "oa_omp_test.c"
    return;
}

int
main( int    argc,
      char** argv )
{
    int	retVal = 0; /* return value */
    int k,i;

    for(k=0;k<30;k++)
    {
{
  int pomp2_num_threads = omp_get_max_threads();
  int pomp2_if = 1;
  POMP2_Task_handle pomp2_old_task;
  POMP2_Parallel_fork(&pomp2_region_3, pomp2_if, pomp2_num_threads, &pomp2_old_task, pomp2_ctc_3 );
#line 51 "oa_omp_test.c"
        #pragma omp parallel POMP2_DLIST_00003 firstprivate(pomp2_old_task) if(pomp2_if) num_threads(pomp2_num_threads) copyin(FORTRAN_MANGLED(pomp_tpd))
{   POMP2_Parallel_begin( &pomp2_region_3 );
#line 52 "oa_omp_test.c"
        {
                printf("Here is the first parallel section outside of the OA...\n");
        }
{ POMP2_Task_handle pomp2_old_task;
  POMP2_Implicit_barrier_enter( &pomp2_region_3, &pomp2_old_task );
#pragma omp barrier
  POMP2_Implicit_barrier_exit( &pomp2_region_3, pomp2_old_task ); }
  POMP2_Parallel_end( &pomp2_region_3 ); }
  POMP2_Parallel_join( &pomp2_region_3, pomp2_old_task ); }
#line 55 "oa_omp_test.c"

        SCOREP_USER_REGION_DEFINE( mainRegion );
        SCOREP_USER_REGION_DEFINE( region1 );
        SCOREP_USER_REGION_DEFINE( region2 );
        SCOREP_USER_OA_PHASE_BEGIN( mainRegion, "mainRegion", SCOREP_USER_REGION_TYPE_COMMON);

{
  int pomp2_num_threads = omp_get_max_threads();
  int pomp2_if = 1;
  POMP2_Task_handle pomp2_old_task;
  POMP2_Parallel_fork(&pomp2_region_4, pomp2_if, pomp2_num_threads, &pomp2_old_task, pomp2_ctc_4 );
#line 61 "oa_omp_test.c"
        #pragma omp parallel private(i) POMP2_DLIST_00004 firstprivate(pomp2_old_task) if(pomp2_if) num_threads(pomp2_num_threads) copyin(FORTRAN_MANGLED(pomp_tpd))
{   POMP2_Parallel_begin( &pomp2_region_4 );
#line 62 "oa_omp_test.c"
        {

                for(i=0;i<5;i++){
                SCOREP_USER_REGION_BEGIN( region1, "region1", SCOREP_USER_REGION_TYPE_COMMON);
                printf("OA_TEST_C:thread %d: Inside Online Access phase, iteration %d\n", omp_get_thread_num() ,i);
                SCOREP_USER_REGION_BEGIN( region2, "region2", SCOREP_USER_REGION_TYPE_COMMON);
                SCOREP_USER_REGION_END(region2);
                SCOREP_USER_REGION_END(region1);
                }
{   POMP2_Single_enter( &pomp2_region_5, pomp2_ctc_5  );
#line 71 "oa_omp_test.c"
                #pragma omp single nowait
{   POMP2_Single_begin( &pomp2_region_5 );
#line 72 "oa_omp_test.c"
                        foo( 10 );
  POMP2_Single_end( &pomp2_region_5 ); }
{ POMP2_Task_handle pomp2_old_task;
  POMP2_Implicit_barrier_enter( &pomp2_region_5, &pomp2_old_task );
#pragma omp barrier
  POMP2_Implicit_barrier_exit( &pomp2_region_5, pomp2_old_task ); }
  POMP2_Single_exit( &pomp2_region_5 );
 }
#line 73 "oa_omp_test.c"

        }
{ POMP2_Task_handle pomp2_old_task;
  POMP2_Implicit_barrier_enter( &pomp2_region_4, &pomp2_old_task );
#pragma omp barrier
  POMP2_Implicit_barrier_exit( &pomp2_region_4, pomp2_old_task ); }
  POMP2_Parallel_end( &pomp2_region_4 ); }
  POMP2_Parallel_join( &pomp2_region_4, pomp2_old_task ); }
#line 75 "oa_omp_test.c"

        int a;
{
  int pomp2_num_threads = omp_get_max_threads();
  int pomp2_if = 1;
  POMP2_Task_handle pomp2_old_task;
  POMP2_Parallel_fork(&pomp2_region_6, pomp2_if, pomp2_num_threads, &pomp2_old_task, pomp2_ctc_6 );
#line 77 "oa_omp_test.c"
	    #pragma omp parallel POMP2_DLIST_00006 firstprivate(pomp2_old_task) if(pomp2_if) num_threads(pomp2_num_threads) copyin(FORTRAN_MANGLED(pomp_tpd))
{   POMP2_Parallel_begin( &pomp2_region_6 );
#line 78 "oa_omp_test.c"
		{
			int i;
{   POMP2_For_enter( &pomp2_region_7, pomp2_ctc_7  );
#line 80 "oa_omp_test.c"
			#pragma omp for nowait
			for ( i = 0; i < 1000; i++ )
			{
				a++;
			}
{ POMP2_Task_handle pomp2_old_task;
  POMP2_Implicit_barrier_enter( &pomp2_region_7, &pomp2_old_task );
#pragma omp barrier
  POMP2_Implicit_barrier_exit( &pomp2_region_7, pomp2_old_task ); }
  POMP2_For_exit( &pomp2_region_7 );
 }
#line 85 "oa_omp_test.c"

#line 86 "oa_omp_test.c"
			#pragma omp master
{   POMP2_Master_begin( &pomp2_region_8, pomp2_ctc_8  );
#line 87 "oa_omp_test.c"
			{
				a++;
			}
  POMP2_Master_end( &pomp2_region_8 ); }
#line 90 "oa_omp_test.c"

{ POMP2_Task_handle pomp2_old_task;
  POMP2_Barrier_enter( &pomp2_region_9, &pomp2_old_task, pomp2_ctc_9  );
#line 91 "oa_omp_test.c"
			#pragma omp barrier
  POMP2_Barrier_exit( &pomp2_region_9, pomp2_old_task ); }
#line 92 "oa_omp_test.c"

{   POMP2_Critical_enter( &pomp2_region_10, pomp2_ctc_10  );
#line 93 "oa_omp_test.c"
			#pragma omp critical(test1)
{   POMP2_Critical_begin( &pomp2_region_10 );
#line 94 "oa_omp_test.c"
			{
				a++;
			}
  POMP2_Critical_end( &pomp2_region_10 ); }
  POMP2_Critical_exit( &pomp2_region_10 );
 }
#line 97 "oa_omp_test.c"

{   POMP2_Atomic_enter( &pomp2_region_11, pomp2_ctc_11  );
#line 98 "oa_omp_test.c"
			#pragma omp atomic
				a++;
  POMP2_Atomic_exit( &pomp2_region_11 );
 }
#line 100 "oa_omp_test.c"

{   POMP2_Sections_enter( &pomp2_region_12, pomp2_ctc_12  );
#line 101 "oa_omp_test.c"
			#pragma omp sections nowait
			{
#line 103 "oa_omp_test.c"
		                #pragma omp section
{   POMP2_Section_begin( &pomp2_region_12, pomp2_ctc_12  );
#line 104 "oa_omp_test.c"
				{
					a++;
				}
  POMP2_Section_end( &pomp2_region_12 ); }
#line 107 "oa_omp_test.c"
		            #pragma omp section
{   POMP2_Section_begin( &pomp2_region_12, pomp2_ctc_12  );
#line 108 "oa_omp_test.c"
				{
					a += 2;
				}
  POMP2_Section_end( &pomp2_region_12 ); }
#line 111 "oa_omp_test.c"
			}
{ POMP2_Task_handle pomp2_old_task;
  POMP2_Implicit_barrier_enter( &pomp2_region_12, &pomp2_old_task );
#pragma omp barrier
  POMP2_Implicit_barrier_exit( &pomp2_region_12, pomp2_old_task ); }
  POMP2_Sections_exit( &pomp2_region_12 );
 }
#line 112 "oa_omp_test.c"
		}
{ POMP2_Task_handle pomp2_old_task;
  POMP2_Implicit_barrier_enter( &pomp2_region_6, &pomp2_old_task );
#pragma omp barrier
  POMP2_Implicit_barrier_exit( &pomp2_region_6, pomp2_old_task ); }
  POMP2_Parallel_end( &pomp2_region_6 ); }
  POMP2_Parallel_join( &pomp2_region_6, pomp2_old_task ); }
#line 113 "oa_omp_test.c"

        SCOREP_USER_OA_PHASE_END( mainRegion );

    }

    return retVal;
}
