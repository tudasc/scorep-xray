#include <config.h>
#include "oa_omp_test.c.opari.inc"
#line 1 "/home/hpc/pr32na/lu43pib/LXCLUSTER_HOME/scorep-trunk-demo/test/OA/OMP/oa_omp_test.c"
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

/* *INDENT-OFF* */
#include <stdio.h>
#include "SCOREP_User.h"

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
  POMP2_Task_handle pomp2_new_task;
if (pomp_if)  POMP2_Task_create_begin( &pomp2_region_1, &pomp2_new_task, &pomp2_old_task, pomp_if, "205*regionType=task*sscl=/home/hpc/pr32na/lu43pib/LXCLUSTER_HOME/scorep-trunk-demo/test/OA/OMP/oa_omp_test.c:32:32*escl=/home/hpc/pr32na/lu43pib/LXCLUSTER_HOME/scorep-trunk-demo/test/OA/OMP/oa_omp_test.c:0:0**"  );
#line 32 "/home/hpc/pr32na/lu43pib/LXCLUSTER_HOME/scorep-trunk-demo/test/OA/OMP/oa_omp_test.c"
#pragma omp task POMP2_DLIST_00001 if(pomp_if) firstprivate(pomp2_new_task, pomp_if)
{ if (pomp_if)  POMP2_Task_begin( &pomp2_region_1, pomp2_new_task );
#line 33 "/home/hpc/pr32na/lu43pib/LXCLUSTER_HOME/scorep-trunk-demo/test/OA/OMP/oa_omp_test.c"
        {
            foo( depth - 1 );
        }
if (pomp_if)  POMP2_Task_end( &pomp2_region_1 ); }
if (pomp_if)  POMP2_Task_create_end( &pomp2_region_1, pomp2_old_task ); }
}
#line 36 "/home/hpc/pr32na/lu43pib/LXCLUSTER_HOME/scorep-trunk-demo/test/OA/OMP/oa_omp_test.c"
    }
{ POMP2_Task_handle pomp2_old_task;
  POMP2_Taskwait_begin( &pomp2_region_2, &pomp2_old_task, "209*regionType=taskwait*sscl=/home/hpc/pr32na/lu43pib/LXCLUSTER_HOME/scorep-trunk-demo/test/OA/OMP/oa_omp_test.c:37:37*escl=/home/hpc/pr32na/lu43pib/LXCLUSTER_HOME/scorep-trunk-demo/test/OA/OMP/oa_omp_test.c:0:0**"  );
#line 37 "/home/hpc/pr32na/lu43pib/LXCLUSTER_HOME/scorep-trunk-demo/test/OA/OMP/oa_omp_test.c"
#pragma omp taskwait
  POMP2_Taskwait_end( &pomp2_region_2, pomp2_old_task ); }
#line 38 "/home/hpc/pr32na/lu43pib/LXCLUSTER_HOME/scorep-trunk-demo/test/OA/OMP/oa_omp_test.c"
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
  int pomp_num_threads = omp_get_max_threads();
  int pomp_if = 1;
  POMP2_Task_handle pomp2_old_task;
  POMP2_Parallel_fork(&pomp2_region_3, pomp_if, pomp_num_threads, &pomp2_old_task, "209*regionType=parallel*sscl=/home/hpc/pr32na/lu43pib/LXCLUSTER_HOME/scorep-trunk-demo/test/OA/OMP/oa_omp_test.c:50:50*escl=/home/hpc/pr32na/lu43pib/LXCLUSTER_HOME/scorep-trunk-demo/test/OA/OMP/oa_omp_test.c:0:0**" );
#line 50 "/home/hpc/pr32na/lu43pib/LXCLUSTER_HOME/scorep-trunk-demo/test/OA/OMP/oa_omp_test.c"
        #pragma omp parallel POMP2_DLIST_00003 firstprivate(pomp2_old_task) if(pomp_if) num_threads(pomp_num_threads) copyin(pomp_tpd_)
{   POMP2_Parallel_begin( &pomp2_region_3 );
#line 51 "/home/hpc/pr32na/lu43pib/LXCLUSTER_HOME/scorep-trunk-demo/test/OA/OMP/oa_omp_test.c"
        {
                printf("Here is the first parallel section outside of the OA...\n");
        }
{ POMP2_Task_handle pomp2_old_task;
  POMP2_Implicit_barrier_enter( &pomp2_region_3, &pomp2_old_task );
#pragma omp barrier
  POMP2_Implicit_barrier_exit( &pomp2_region_3, pomp2_old_task ); }
  POMP2_Parallel_end( &pomp2_region_3 ); }
  POMP2_Parallel_join( &pomp2_region_3, pomp2_old_task ); }
#line 54 "/home/hpc/pr32na/lu43pib/LXCLUSTER_HOME/scorep-trunk-demo/test/OA/OMP/oa_omp_test.c"

        SCOREP_USER_REGION_DEFINE( mainRegion );
        SCOREP_USER_REGION_DEFINE( region1 );
        SCOREP_USER_REGION_DEFINE( region2 );
        SCOREP_USER_OA_PHASE_BEGIN( mainRegion, "mainRegion", SCOREP_USER_REGION_TYPE_COMMON);

{
  int pomp_num_threads = omp_get_max_threads();
  int pomp_if = 1;
  POMP2_Task_handle pomp2_old_task;
  POMP2_Parallel_fork(&pomp2_region_4, pomp_if, pomp_num_threads, &pomp2_old_task, "209*regionType=parallel*sscl=/home/hpc/pr32na/lu43pib/LXCLUSTER_HOME/scorep-trunk-demo/test/OA/OMP/oa_omp_test.c:60:60*escl=/home/hpc/pr32na/lu43pib/LXCLUSTER_HOME/scorep-trunk-demo/test/OA/OMP/oa_omp_test.c:0:0**" );
#line 60 "/home/hpc/pr32na/lu43pib/LXCLUSTER_HOME/scorep-trunk-demo/test/OA/OMP/oa_omp_test.c"
        #pragma omp parallel private(i) POMP2_DLIST_00004 firstprivate(pomp2_old_task) if(pomp_if) num_threads(pomp_num_threads) copyin(pomp_tpd_)
{   POMP2_Parallel_begin( &pomp2_region_4 );
#line 61 "/home/hpc/pr32na/lu43pib/LXCLUSTER_HOME/scorep-trunk-demo/test/OA/OMP/oa_omp_test.c"
        {

                for(i=0;i<5;i++){
                SCOREP_USER_REGION_BEGIN( region1, "region1", SCOREP_USER_REGION_TYPE_COMMON);
                printf("OA_TEST_C:thread %d: Inside Online Access phase, iteration %d\n", omp_get_thread_num() ,i);
                SCOREP_USER_REGION_BEGIN( region2, "region2", SCOREP_USER_REGION_TYPE_COMMON);
                SCOREP_USER_REGION_END(region2);
                SCOREP_USER_REGION_END(region1);
                }
{   POMP2_Single_enter( &pomp2_region_5, "207*regionType=single*sscl=/home/hpc/pr32na/lu43pib/LXCLUSTER_HOME/scorep-trunk-demo/test/OA/OMP/oa_omp_test.c:70:70*escl=/home/hpc/pr32na/lu43pib/LXCLUSTER_HOME/scorep-trunk-demo/test/OA/OMP/oa_omp_test.c:0:0**"  );
#line 70 "/home/hpc/pr32na/lu43pib/LXCLUSTER_HOME/scorep-trunk-demo/test/OA/OMP/oa_omp_test.c"
                #pragma omp single nowait
{   POMP2_Single_begin( &pomp2_region_5 );
#line 71 "/home/hpc/pr32na/lu43pib/LXCLUSTER_HOME/scorep-trunk-demo/test/OA/OMP/oa_omp_test.c"
                        foo( 10 );
  POMP2_Single_end( &pomp2_region_5 ); }
{ POMP2_Task_handle pomp2_old_task;
  POMP2_Implicit_barrier_enter( &pomp2_region_5, &pomp2_old_task );
#pragma omp barrier
  POMP2_Implicit_barrier_exit( &pomp2_region_5, pomp2_old_task ); }
  POMP2_Single_exit( &pomp2_region_5 );
 }
#line 72 "/home/hpc/pr32na/lu43pib/LXCLUSTER_HOME/scorep-trunk-demo/test/OA/OMP/oa_omp_test.c"

        }
{ POMP2_Task_handle pomp2_old_task;
  POMP2_Implicit_barrier_enter( &pomp2_region_4, &pomp2_old_task );
#pragma omp barrier
  POMP2_Implicit_barrier_exit( &pomp2_region_4, pomp2_old_task ); }
  POMP2_Parallel_end( &pomp2_region_4 ); }
  POMP2_Parallel_join( &pomp2_region_4, pomp2_old_task ); }
#line 74 "/home/hpc/pr32na/lu43pib/LXCLUSTER_HOME/scorep-trunk-demo/test/OA/OMP/oa_omp_test.c"

        int a;
{
  int pomp_num_threads = omp_get_max_threads();
  int pomp_if = 1;
  POMP2_Task_handle pomp2_old_task;
  POMP2_Parallel_fork(&pomp2_region_6, pomp_if, pomp_num_threads, &pomp2_old_task, "209*regionType=parallel*sscl=/home/hpc/pr32na/lu43pib/LXCLUSTER_HOME/scorep-trunk-demo/test/OA/OMP/oa_omp_test.c:76:76*escl=/home/hpc/pr32na/lu43pib/LXCLUSTER_HOME/scorep-trunk-demo/test/OA/OMP/oa_omp_test.c:0:0**" );
#line 76 "/home/hpc/pr32na/lu43pib/LXCLUSTER_HOME/scorep-trunk-demo/test/OA/OMP/oa_omp_test.c"
	    #pragma omp parallel POMP2_DLIST_00006 firstprivate(pomp2_old_task) if(pomp_if) num_threads(pomp_num_threads) copyin(pomp_tpd_)
{   POMP2_Parallel_begin( &pomp2_region_6 );
#line 77 "/home/hpc/pr32na/lu43pib/LXCLUSTER_HOME/scorep-trunk-demo/test/OA/OMP/oa_omp_test.c"
		{
			int i;
{   POMP2_For_enter( &pomp2_region_7, "204*regionType=for*sscl=/home/hpc/pr32na/lu43pib/LXCLUSTER_HOME/scorep-trunk-demo/test/OA/OMP/oa_omp_test.c:79:79*escl=/home/hpc/pr32na/lu43pib/LXCLUSTER_HOME/scorep-trunk-demo/test/OA/OMP/oa_omp_test.c:0:0**"  );
#line 79 "/home/hpc/pr32na/lu43pib/LXCLUSTER_HOME/scorep-trunk-demo/test/OA/OMP/oa_omp_test.c"
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
#line 84 "/home/hpc/pr32na/lu43pib/LXCLUSTER_HOME/scorep-trunk-demo/test/OA/OMP/oa_omp_test.c"

#line 85 "/home/hpc/pr32na/lu43pib/LXCLUSTER_HOME/scorep-trunk-demo/test/OA/OMP/oa_omp_test.c"
			#pragma omp master
{   POMP2_Master_begin( &pomp2_region_8, "207*regionType=master*sscl=/home/hpc/pr32na/lu43pib/LXCLUSTER_HOME/scorep-trunk-demo/test/OA/OMP/oa_omp_test.c:85:85*escl=/home/hpc/pr32na/lu43pib/LXCLUSTER_HOME/scorep-trunk-demo/test/OA/OMP/oa_omp_test.c:0:0**"  );
#line 86 "/home/hpc/pr32na/lu43pib/LXCLUSTER_HOME/scorep-trunk-demo/test/OA/OMP/oa_omp_test.c"
			{
				a++;
			}
  POMP2_Master_end( &pomp2_region_8 ); }
#line 89 "/home/hpc/pr32na/lu43pib/LXCLUSTER_HOME/scorep-trunk-demo/test/OA/OMP/oa_omp_test.c"

{ POMP2_Task_handle pomp2_old_task;
  POMP2_Barrier_enter( &pomp2_region_9, &pomp2_old_task, "208*regionType=barrier*sscl=/home/hpc/pr32na/lu43pib/LXCLUSTER_HOME/scorep-trunk-demo/test/OA/OMP/oa_omp_test.c:90:90*escl=/home/hpc/pr32na/lu43pib/LXCLUSTER_HOME/scorep-trunk-demo/test/OA/OMP/oa_omp_test.c:0:0**"  );
#line 90 "/home/hpc/pr32na/lu43pib/LXCLUSTER_HOME/scorep-trunk-demo/test/OA/OMP/oa_omp_test.c"
			#pragma omp barrier
  POMP2_Barrier_exit( &pomp2_region_9, pomp2_old_task ); }
#line 91 "/home/hpc/pr32na/lu43pib/LXCLUSTER_HOME/scorep-trunk-demo/test/OA/OMP/oa_omp_test.c"

{   POMP2_Critical_enter( &pomp2_region_10, "228*regionType=critical*sscl=/home/hpc/pr32na/lu43pib/LXCLUSTER_HOME/scorep-trunk-demo/test/OA/OMP/oa_omp_test.c:92:92*escl=/home/hpc/pr32na/lu43pib/LXCLUSTER_HOME/scorep-trunk-demo/test/OA/OMP/oa_omp_test.c:0:0*criticalName=test1**"  );
#line 92 "/home/hpc/pr32na/lu43pib/LXCLUSTER_HOME/scorep-trunk-demo/test/OA/OMP/oa_omp_test.c"
			#pragma omp critical(test1)
{   POMP2_Critical_begin( &pomp2_region_10 );
#line 93 "/home/hpc/pr32na/lu43pib/LXCLUSTER_HOME/scorep-trunk-demo/test/OA/OMP/oa_omp_test.c"
			{
				a++;
			}
  POMP2_Critical_end( &pomp2_region_10 ); }
  POMP2_Critical_exit( &pomp2_region_10 );
 }
#line 96 "/home/hpc/pr32na/lu43pib/LXCLUSTER_HOME/scorep-trunk-demo/test/OA/OMP/oa_omp_test.c"

{   POMP2_Atomic_enter( &pomp2_region_11, "207*regionType=atomic*sscl=/home/hpc/pr32na/lu43pib/LXCLUSTER_HOME/scorep-trunk-demo/test/OA/OMP/oa_omp_test.c:97:97*escl=/home/hpc/pr32na/lu43pib/LXCLUSTER_HOME/scorep-trunk-demo/test/OA/OMP/oa_omp_test.c:0:0**"  );
#line 97 "/home/hpc/pr32na/lu43pib/LXCLUSTER_HOME/scorep-trunk-demo/test/OA/OMP/oa_omp_test.c"
			#pragma omp atomic
				a++;
  POMP2_Atomic_exit( &pomp2_region_11 );
 }
#line 99 "/home/hpc/pr32na/lu43pib/LXCLUSTER_HOME/scorep-trunk-demo/test/OA/OMP/oa_omp_test.c"

{   POMP2_Sections_enter( &pomp2_region_12, "225*regionType=sections*sscl=/home/hpc/pr32na/lu43pib/LXCLUSTER_HOME/scorep-trunk-demo/test/OA/OMP/oa_omp_test.c:100:100*escl=/home/hpc/pr32na/lu43pib/LXCLUSTER_HOME/scorep-trunk-demo/test/OA/OMP/oa_omp_test.c:0:0*numSections=0**"  );
#line 100 "/home/hpc/pr32na/lu43pib/LXCLUSTER_HOME/scorep-trunk-demo/test/OA/OMP/oa_omp_test.c"
			#pragma omp sections nowait
			{
#line 102 "/home/hpc/pr32na/lu43pib/LXCLUSTER_HOME/scorep-trunk-demo/test/OA/OMP/oa_omp_test.c"
		                #pragma omp section
{   POMP2_Section_begin( &pomp2_region_12, "225*regionType=sections*sscl=/home/hpc/pr32na/lu43pib/LXCLUSTER_HOME/scorep-trunk-demo/test/OA/OMP/oa_omp_test.c:100:100*escl=/home/hpc/pr32na/lu43pib/LXCLUSTER_HOME/scorep-trunk-demo/test/OA/OMP/oa_omp_test.c:0:0*numSections=0**"  );
#line 103 "/home/hpc/pr32na/lu43pib/LXCLUSTER_HOME/scorep-trunk-demo/test/OA/OMP/oa_omp_test.c"
				{
					a++;
				}
  POMP2_Section_end( &pomp2_region_12 ); }
#line 106 "/home/hpc/pr32na/lu43pib/LXCLUSTER_HOME/scorep-trunk-demo/test/OA/OMP/oa_omp_test.c"
		            #pragma omp section
{   POMP2_Section_begin( &pomp2_region_12, "225*regionType=sections*sscl=/home/hpc/pr32na/lu43pib/LXCLUSTER_HOME/scorep-trunk-demo/test/OA/OMP/oa_omp_test.c:100:100*escl=/home/hpc/pr32na/lu43pib/LXCLUSTER_HOME/scorep-trunk-demo/test/OA/OMP/oa_omp_test.c:0:0*numSections=1**"  );
#line 107 "/home/hpc/pr32na/lu43pib/LXCLUSTER_HOME/scorep-trunk-demo/test/OA/OMP/oa_omp_test.c"
				{
					a += 2;
				}
  POMP2_Section_end( &pomp2_region_12 ); }
#line 110 "/home/hpc/pr32na/lu43pib/LXCLUSTER_HOME/scorep-trunk-demo/test/OA/OMP/oa_omp_test.c"
			}
{ POMP2_Task_handle pomp2_old_task;
  POMP2_Implicit_barrier_enter( &pomp2_region_12, &pomp2_old_task );
#pragma omp barrier
  POMP2_Implicit_barrier_exit( &pomp2_region_12, pomp2_old_task ); }
  POMP2_Sections_exit( &pomp2_region_12 );
 }
#line 111 "/home/hpc/pr32na/lu43pib/LXCLUSTER_HOME/scorep-trunk-demo/test/OA/OMP/oa_omp_test.c"
		}
{ POMP2_Task_handle pomp2_old_task;
  POMP2_Implicit_barrier_enter( &pomp2_region_6, &pomp2_old_task );
#pragma omp barrier
  POMP2_Implicit_barrier_exit( &pomp2_region_6, pomp2_old_task ); }
  POMP2_Parallel_end( &pomp2_region_6 ); }
  POMP2_Parallel_join( &pomp2_region_6, pomp2_old_task ); }
#line 112 "/home/hpc/pr32na/lu43pib/LXCLUSTER_HOME/scorep-trunk-demo/test/OA/OMP/oa_omp_test.c"

        SCOREP_USER_OA_PHASE_END( mainRegion );

    }

    return retVal;
}
