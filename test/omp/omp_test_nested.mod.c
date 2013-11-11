/* *INDENT-OFF* */
#include <config.h>
#include "omp_test_nested.c.opari.inc"
#line 1 "omp_test_nested.c"
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
 * @file       omp_test_nested.c
 *
 *
 */

#include <stdio.h>

#define N 10

int outer, middle;
#line 45 "omp_test_nested.c"
#pragma omp threadprivate(outer, middle)

float foo(int n)
{
  int i;
  float sum, d;

  sum = 0;

  for (i = 0; i < n; i++)
    {
      sum+=(float)i;
    }

  d = sum/( (float)n*(float)n + 1.0 );

  //  printf("%f / (%d * %d + 1.0) = %f\n",sum, n, n, d);
  return d;
}


int main(int argc, char* argv[]){
  int i;
  float sum;

  omp_set_nested(1);
  sum=0;

  for (i = 0; i < N; i++)
    {
{
  int pomp2_num_threads = 2;
  int pomp2_if = 1;
  POMP2_Task_handle pomp2_old_task;
  POMP2_Parallel_fork(&pomp2_region_1, pomp2_if, pomp2_num_threads, &pomp2_old_task, pomp2_ctc_1 );
#line 75 "omp_test_nested.c"
#pragma omp parallel                POMP2_DLIST_00001 firstprivate(pomp2_old_task) num_threads(pomp2_num_threads) copyin(FORTRAN_MANGLED( pomp_tpd ))
{   POMP2_Parallel_begin( &pomp2_region_1 );
#line 76 "omp_test_nested.c"
      {
        outer = omp_get_thread_num();
{
  int pomp2_num_threads = 2;
  int pomp2_if = 1;
  POMP2_Task_handle pomp2_old_task;
  POMP2_Parallel_fork(&pomp2_region_2, pomp2_if, pomp2_num_threads, &pomp2_old_task, pomp2_ctc_2 );
#line 78 "omp_test_nested.c"
#pragma omp parallel                copyin(outer) POMP2_DLIST_00002 firstprivate(pomp2_old_task) num_threads(pomp2_num_threads) copyin(FORTRAN_MANGLED( pomp_tpd ))
{   POMP2_Parallel_begin( &pomp2_region_2 );
#line 79 "omp_test_nested.c"
        {
          middle = omp_get_thread_num();
{
  int pomp2_num_threads = 2;
  int pomp2_if = 1;
  POMP2_Task_handle pomp2_old_task;
  POMP2_Parallel_fork(&pomp2_region_3, pomp2_if, pomp2_num_threads, &pomp2_old_task, pomp2_ctc_3 );
#line 81 "omp_test_nested.c"
#pragma omp parallel                copyin(outer, middle) POMP2_DLIST_00003 firstprivate(pomp2_old_task) num_threads(pomp2_num_threads) copyin(FORTRAN_MANGLED( pomp_tpd ))
{   POMP2_Parallel_begin( &pomp2_region_3 );
#line 82 "omp_test_nested.c"
          {
{   POMP2_Atomic_enter( &pomp2_region_4, pomp2_ctc_4  );
#line 83 "omp_test_nested.c"
#pragma omp atomic
            sum += foo(i);
  POMP2_Atomic_exit( &pomp2_region_4 );
 }
#line 85 "omp_test_nested.c"
          }
{ POMP2_Task_handle pomp2_old_task;
  POMP2_Implicit_barrier_enter( &pomp2_region_3, &pomp2_old_task );
#pragma omp barrier
  POMP2_Implicit_barrier_exit( &pomp2_region_3, pomp2_old_task ); }
  POMP2_Parallel_end( &pomp2_region_3 ); }
  POMP2_Parallel_join( &pomp2_region_3, pomp2_old_task ); }
#line 86 "omp_test_nested.c"
        }
{ POMP2_Task_handle pomp2_old_task;
  POMP2_Implicit_barrier_enter( &pomp2_region_2, &pomp2_old_task );
#pragma omp barrier
  POMP2_Implicit_barrier_exit( &pomp2_region_2, pomp2_old_task ); }
  POMP2_Parallel_end( &pomp2_region_2 ); }
  POMP2_Parallel_join( &pomp2_region_2, pomp2_old_task ); }
#line 87 "omp_test_nested.c"
      }
{ POMP2_Task_handle pomp2_old_task;
  POMP2_Implicit_barrier_enter( &pomp2_region_1, &pomp2_old_task );
#pragma omp barrier
  POMP2_Implicit_barrier_exit( &pomp2_region_1, pomp2_old_task ); }
  POMP2_Parallel_end( &pomp2_region_1 ); }
  POMP2_Parallel_join( &pomp2_region_1, pomp2_old_task ); }
#line 88 "omp_test_nested.c"
    }
  printf("Sum=%f\n", sum);

  sum=0;
  for (i = 0; i < N; i++)
    {
{
  int pomp2_num_threads = 4;
  int pomp2_if = 1;
  POMP2_Task_handle pomp2_old_task;
  POMP2_Parallel_fork(&pomp2_region_5, pomp2_if, pomp2_num_threads, &pomp2_old_task, pomp2_ctc_5 );
#line 94 "omp_test_nested.c"
#pragma omp parallel                POMP2_DLIST_00005 firstprivate(pomp2_old_task) num_threads(pomp2_num_threads) copyin(FORTRAN_MANGLED( pomp_tpd ))
{   POMP2_Parallel_begin( &pomp2_region_5 );
#line 95 "omp_test_nested.c"
      {
        outer = omp_get_thread_num();
{
  int pomp2_num_threads = 2;
  int pomp2_if = 1;
  POMP2_Task_handle pomp2_old_task;
  POMP2_Parallel_fork(&pomp2_region_6, pomp2_if, pomp2_num_threads, &pomp2_old_task, pomp2_ctc_6 );
#line 97 "omp_test_nested.c"
#pragma omp parallel                copyin(outer) POMP2_DLIST_00006 firstprivate(pomp2_old_task) num_threads(pomp2_num_threads) copyin(FORTRAN_MANGLED( pomp_tpd ))
{   POMP2_Parallel_begin( &pomp2_region_6 );
#line 98 "omp_test_nested.c"
        {
          middle = omp_get_thread_num();
{   POMP2_Atomic_enter( &pomp2_region_7, pomp2_ctc_7  );
#line 100 "omp_test_nested.c"
#pragma omp atomic
          sum += foo(i);
  POMP2_Atomic_exit( &pomp2_region_7 );
 }
#line 102 "omp_test_nested.c"
        }
{ POMP2_Task_handle pomp2_old_task;
  POMP2_Implicit_barrier_enter( &pomp2_region_6, &pomp2_old_task );
#pragma omp barrier
  POMP2_Implicit_barrier_exit( &pomp2_region_6, pomp2_old_task ); }
  POMP2_Parallel_end( &pomp2_region_6 ); }
  POMP2_Parallel_join( &pomp2_region_6, pomp2_old_task ); }
#line 103 "omp_test_nested.c"
      }
{ POMP2_Task_handle pomp2_old_task;
  POMP2_Implicit_barrier_enter( &pomp2_region_5, &pomp2_old_task );
#pragma omp barrier
  POMP2_Implicit_barrier_exit( &pomp2_region_5, pomp2_old_task ); }
  POMP2_Parallel_end( &pomp2_region_5 ); }
  POMP2_Parallel_join( &pomp2_region_5, pomp2_old_task ); }
#line 104 "omp_test_nested.c"
    }
  printf("Sum=%f\n", sum);

  sum=0;
  for (i = 0; i < N; i++)
    {
{
  int pomp2_num_threads = 2;
  int pomp2_if = 1;
  POMP2_Task_handle pomp2_old_task;
  POMP2_Parallel_fork(&pomp2_region_8, pomp2_if, pomp2_num_threads, &pomp2_old_task, pomp2_ctc_8 );
#line 110 "omp_test_nested.c"
#pragma omp parallel                POMP2_DLIST_00008 firstprivate(pomp2_old_task) num_threads(pomp2_num_threads) copyin(FORTRAN_MANGLED( pomp_tpd ))
{   POMP2_Parallel_begin( &pomp2_region_8 );
#line 111 "omp_test_nested.c"
      {
        outer = omp_get_thread_num();
{
  int pomp2_num_threads = 4;
  int pomp2_if = 1;
  POMP2_Task_handle pomp2_old_task;
  POMP2_Parallel_fork(&pomp2_region_9, pomp2_if, pomp2_num_threads, &pomp2_old_task, pomp2_ctc_9 );
#line 113 "omp_test_nested.c"
#pragma omp parallel                copyin(outer) POMP2_DLIST_00009 firstprivate(pomp2_old_task) num_threads(pomp2_num_threads) copyin(FORTRAN_MANGLED( pomp_tpd ))
{   POMP2_Parallel_begin( &pomp2_region_9 );
#line 114 "omp_test_nested.c"
        {
{   POMP2_Atomic_enter( &pomp2_region_10, pomp2_ctc_10  );
#line 115 "omp_test_nested.c"
#pragma omp atomic
          sum += foo(i);
  POMP2_Atomic_exit( &pomp2_region_10 );
 }
#line 117 "omp_test_nested.c"
        }
{ POMP2_Task_handle pomp2_old_task;
  POMP2_Implicit_barrier_enter( &pomp2_region_9, &pomp2_old_task );
#pragma omp barrier
  POMP2_Implicit_barrier_exit( &pomp2_region_9, pomp2_old_task ); }
  POMP2_Parallel_end( &pomp2_region_9 ); }
  POMP2_Parallel_join( &pomp2_region_9, pomp2_old_task ); }
#line 118 "omp_test_nested.c"
      }
{ POMP2_Task_handle pomp2_old_task;
  POMP2_Implicit_barrier_enter( &pomp2_region_8, &pomp2_old_task );
#pragma omp barrier
  POMP2_Implicit_barrier_exit( &pomp2_region_8, pomp2_old_task ); }
  POMP2_Parallel_end( &pomp2_region_8 ); }
  POMP2_Parallel_join( &pomp2_region_8, pomp2_old_task ); }
#line 119 "omp_test_nested.c"
    }

  printf("Sum=%f\n", sum);

  return 0;
}
