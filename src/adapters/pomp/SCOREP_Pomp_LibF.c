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
 * @file       SCOREP_Pomp_UserF.c
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 * @status     alpha
 * @ingroup    POMP2
 *
 * @brief Implementation of the POMP2 fortran user adapter functions and initialization.
 */

#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <opari2/pomp2_lib.h>
#include "SCOREP_Fortran_Wrapper.h"
#include "SCOREP_Pomp_Variables.h"
#include "SCOREP_Pomp_Fortran.h"

/*
 * Fortran wrappers calling the C versions
 */
/* *INDENT-OFF*  */
void FSUB(POMP2_Atomic_enter)( POMP2_Region_handle_fortran* region_handle,
                               char*                        ctc_string )
{
    if ( scorep_pomp_is_tracing_on )
    {
      POMP2_Atomic_enter( SCOREP_POMP_F2C_REGION( region_handle ),\
                          NULL );
    }
}

void FSUB(POMP2_Atomic_exit)( POMP2_Region_handle_fortran* region_handle )
{
    if ( scorep_pomp_is_tracing_on )
    {
        POMP2_Atomic_exit( SCOREP_POMP_F2C_REGION( region_handle ));
    }
}

void FSUB(POMP2_Barrier_enter)( POMP2_Region_handle_fortran* region_handle,
                                POMP2_Task_handle_fortran*   pomp_old_task,
                                char*                        ctc_string )
{
    if ( scorep_pomp_is_tracing_on )
    {
      POMP2_Barrier_enter( SCOREP_POMP_F2C_REGION( region_handle ),
                           SCOREP_POMP_F2C_TASK( pomp_old_task ),
                           NULL );
    }
}

void FSUB(POMP2_Barrier_exit)( POMP2_Region_handle_fortran* region_handle,
                               POMP2_Task_handle_fortran*   pomp_old_task )
{
    if ( scorep_pomp_is_tracing_on )
    {
      POMP2_Barrier_exit( SCOREP_POMP_F2C_REGION( region_handle ),
                          *SCOREP_POMP_F2C_TASK( pomp_old_task ) );
    }
}

void FSUB(POMP2_Implicit_barrier_enter)( POMP2_Region_handle_fortran* region_handle,
                                         POMP2_Task_handle_fortran*   pomp_old_task )
{
    if ( scorep_pomp_is_tracing_on )
    {
      POMP2_Implicit_barrier_enter( SCOREP_POMP_F2C_REGION( region_handle ),
                                    SCOREP_POMP_F2C_TASK( pomp_old_task ) );
    }
}

void FSUB(POMP2_Implicit_barrier_exit)( POMP2_Region_handle_fortran* region_handle,
                                        POMP2_Task_handle_fortran*   pomp_old_task )
{
    if ( scorep_pomp_is_tracing_on )
    {
      POMP2_Implicit_barrier_exit( SCOREP_POMP_F2C_REGION( region_handle ),
                                   *SCOREP_POMP_F2C_TASK( pomp_old_task ) );
    }
}

void FSUB(POMP2_Flush_enter)( POMP2_Region_handle_fortran* region_handle,
                              char*                        ctc_string )
{
    if ( scorep_pomp_is_tracing_on )
    {
      POMP2_Flush_enter( SCOREP_POMP_F2C_REGION( region_handle ),
                         NULL );
    }
}

void FSUB(POMP2_Flush_exit)( POMP2_Region_handle_fortran* region_handle )
{
    if ( scorep_pomp_is_tracing_on )
    {
        POMP2_Flush_exit( SCOREP_POMP_F2C_REGION( region_handle ));
    }
}

void FSUB(POMP2_Critical_begin)( POMP2_Region_handle_fortran* region_handle )
{
    if ( scorep_pomp_is_tracing_on )
    {
        POMP2_Critical_begin( SCOREP_POMP_F2C_REGION( region_handle ));
    }
}

void FSUB(POMP2_Critical_end)( POMP2_Region_handle_fortran* region_handle )
{
    if ( scorep_pomp_is_tracing_on )
    {
        POMP2_Critical_end( SCOREP_POMP_F2C_REGION( region_handle ));
    }
}

void FSUB(POMP2_Critical_enter)( POMP2_Region_handle_fortran* region_handle,
                                 const char*                  ctc_string )
{
    if ( scorep_pomp_is_tracing_on )
    {
      POMP2_Critical_enter( SCOREP_POMP_F2C_REGION( region_handle ),
                            NULL );
    }
}

void FSUB(POMP2_Critical_exit)( POMP2_Region_handle_fortran* region_handle )
{
    if ( scorep_pomp_is_tracing_on )
    {
        POMP2_Critical_exit( SCOREP_POMP_F2C_REGION( region_handle ));
    }
}

void FSUB(POMP2_Do_enter)( POMP2_Region_handle_fortran* region_handle,
                           const char*                  ctc_string )
{
    if ( scorep_pomp_is_tracing_on )
    {
      POMP2_For_enter( SCOREP_POMP_F2C_REGION( region_handle ),
                       NULL );
    }
}

void FSUB(POMP2_Do_exit)( POMP2_Region_handle_fortran* region_handle )
{
    if ( scorep_pomp_is_tracing_on )
    {
        POMP2_For_exit( SCOREP_POMP_F2C_REGION( region_handle ));
    }
}

void FSUB(POMP2_Master_begin)( POMP2_Region_handle_fortran* region_handle,
			       const char*                  ctc_string )
{
    if ( scorep_pomp_is_tracing_on )
    {
      POMP2_Master_begin( SCOREP_POMP_F2C_REGION( region_handle ),
                          NULL );
    }
}

void FSUB(POMP2_Master_end)( POMP2_Region_handle_fortran* region_handle )
{
    if ( scorep_pomp_is_tracing_on )
    {
        POMP2_Master_end( SCOREP_POMP_F2C_REGION( region_handle ));
    }
}

void FSUB(POMP2_Parallel_begin)( POMP2_Region_handle_fortran* region_handle )
{
    if ( scorep_pomp_is_tracing_on )
    {
      POMP2_Parallel_begin( SCOREP_POMP_F2C_REGION( region_handle ) );
    }
}

void FSUB(POMP2_Parallel_end)( POMP2_Region_handle_fortran* region_handle )
{
    if ( scorep_pomp_is_tracing_on )
    {
        POMP2_Parallel_end(SCOREP_POMP_F2C_REGION( region_handle ));
    }
}

void FSUB(POMP2_Parallel_fork)( POMP2_Region_handle_fortran* region_handle,
                                int*                         if_clause,
                                int*                         num_threads,
                                POMP2_Task_handle_fortran*   pomp_old_task,
                                char*                        ctc_string )
{
    if ( scorep_pomp_is_tracing_on )
      {
	POMP2_Parallel_fork( SCOREP_POMP_F2C_REGION( region_handle ),
                             *if_clause,
                             *num_threads,
                             SCOREP_POMP_F2C_TASK( pomp_old_task ),
                             NULL );
      }
}

void FSUB(POMP2_Parallel_join)( POMP2_Region_handle_fortran* region_handle,
                                POMP2_Task_handle_fortran*   pomp_old_task )
{
    if ( scorep_pomp_is_tracing_on )
    {
      POMP2_Parallel_join( SCOREP_POMP_F2C_REGION( region_handle ),
                           *SCOREP_POMP_F2C_TASK( pomp_old_task ) );
    }
}

void FSUB(POMP2_Section_begin)( POMP2_Region_handle_fortran* region_handle,
                                char*                        ctc_string)
{
    if ( scorep_pomp_is_tracing_on )
    {
      POMP2_Section_begin( SCOREP_POMP_F2C_REGION( region_handle ),
                           NULL );
    }
}

void FSUB(POMP2_Section_end)( POMP2_Region_handle_fortran* region_handle )
{
    if ( scorep_pomp_is_tracing_on )
    {
        POMP2_Section_end( SCOREP_POMP_F2C_REGION( region_handle ));
    }
}

void FSUB(POMP2_Sections_enter)( POMP2_Region_handle_fortran* region_handle,
                                 char*                        ctc_string )
{
    if ( scorep_pomp_is_tracing_on )
    {
      POMP2_Sections_enter(SCOREP_POMP_F2C_REGION( region_handle ),
                           NULL );
    }
}

void FSUB(POMP2_Sections_exit)( POMP2_Region_handle_fortran* region_handle )
{
    if ( scorep_pomp_is_tracing_on )
    {
        POMP2_Sections_exit(SCOREP_POMP_F2C_REGION( region_handle ));
    }
}

void FSUB(POMP2_Single_begin)( POMP2_Region_handle_fortran* region_handle )
{
    if ( scorep_pomp_is_tracing_on )
    {
        POMP2_Single_begin(SCOREP_POMP_F2C_REGION( region_handle ));
    }
}

void FSUB(POMP2_Single_end)( POMP2_Region_handle_fortran* region_handle )
{
    if ( scorep_pomp_is_tracing_on )
    {
        POMP2_Single_end(SCOREP_POMP_F2C_REGION( region_handle ));
    }
}

void FSUB(POMP2_Single_enter)( POMP2_Region_handle_fortran* region_handle,
                               char*                        ctc_string )
{
    if ( scorep_pomp_is_tracing_on )
    {
      POMP2_Single_enter(SCOREP_POMP_F2C_REGION( region_handle ),
                         NULL );
    }
}

void FSUB(POMP2_Single_exit)( POMP2_Region_handle_fortran* region_handle )
{
    if ( scorep_pomp_is_tracing_on )
    {
        POMP2_Single_exit(SCOREP_POMP_F2C_REGION( region_handle ));
    }
}

void FSUB(POMP2_Task_create_begin)( POMP2_Region_handle_fortran* region_handle,
                                    POMP2_Task_handle_fortran*   pomp2_old_task,
                                    int*                         pomp2_if,
                                    char*                        ctc_string )
{
  POMP2_Task_create_begin( SCOREP_POMP_F2C_REGION( region_handle ),
                           SCOREP_POMP_F2C_TASK( pomp2_old_task ),
                           *pomp2_if,
                           NULL);
}

void FSUB(POMP2_Task_create_end)( POMP2_Region_handle_fortran* region_handle,
                                  POMP2_Task_handle_fortran*   pomp2_old_task )
{
  POMP2_Task_create_end( SCOREP_POMP_F2C_REGION( region_handle ),
                         *SCOREP_POMP_F2C_TASK( pomp2_old_task ) );
}

void FSUB(POMP2_Task_begin)( POMP2_Region_handle_fortran* region_handle,
                             POMP2_Task_handle*           pomp_new_task )
{
  POMP2_Task_begin( SCOREP_POMP_F2C_REGION( region_handle),
                    *SCOREP_POMP_F2C_TASK( pomp_new_task ) );
}

void FSUB(POMP2_Task_end)( POMP2_Region_handle_fortran* region_handle)
{
  POMP2_Task_end( SCOREP_POMP_F2C_REGION( region_handle ) );
}

void FSUB(POMP2_Untied_task_create_begin)( POMP2_Region_handle_fortran* region_handle,
                                           POMP2_Task_handle_fortran*   pomp2_old_task,
                                           int*                         pomp2_if,
                                           char*                        ctc_string )
{
  POMP2_Task_create_begin( SCOREP_POMP_F2C_REGION( region_handle ),
                           SCOREP_POMP_F2C_TASK( pomp2_old_task ),
                           *pomp2_if,
                           NULL );
}

void FSUB(POMP2_Untied_task_create_end)( POMP2_Region_handle_fortran* region_handle,
                                         POMP2_Task_handle_fortran*   pomp2_old_task )
{
  POMP2_Task_create_end( SCOREP_POMP_F2C_REGION( region_handle ),
                         *SCOREP_POMP_F2C_TASK( pomp2_old_task ) );
}

void FSUB(POMP2_Untied_task_begin)( POMP2_Region_handle_fortran* region_handle,
                                    POMP2_Task_handle*           pomp_new_task )
{
  POMP2_Task_begin( SCOREP_POMP_F2C_REGION( region_handle ),
                    *SCOREP_POMP_F2C_TASK( pomp_new_task ) );
}

void FSUB(POMP2_Untied_task_end)( POMP2_Region_handle_fortran* region_handle )
{
  POMP2_Task_end( SCOREP_POMP_F2C_REGION( region_handle ) );
}

void FSUB(POMP2_Taskwait_begin)( POMP2_Region_handle_fortran* region_handle,
                                 POMP2_Task_handle_fortran*   pomp2_old_task,
                                 char*                        ctc_string[] )
{
  POMP2_Taskwait_begin( SCOREP_POMP_F2C_REGION( region_handle ),
                        SCOREP_POMP_F2C_TASK( pomp2_old_task ),
                        NULL );
}

void FSUB(POMP2_Taskwait_end)( POMP2_Region_handle_fortran* region_handle,
                               POMP2_Task_handle_fortran*   pomp2_old_task )
{
  POMP2_Taskwait_end( SCOREP_POMP_F2C_REGION( region_handle ),
                      *SCOREP_POMP_F2C_TASK( pomp2_old_task ) );
}

void FSUB(POMP2_Workshare_enter)( POMP2_Region_handle_fortran* region_handle,
                                  char*                        ctc_string )
{
    if ( scorep_pomp_is_tracing_on )
    {
      POMP2_Workshare_enter(SCOREP_POMP_F2C_REGION( region_handle ),
                            NULL );
    }
}

void FSUB(POMP2_Workshare_exit)( POMP2_Region_handle_fortran* region_handle )
{
    if ( scorep_pomp_is_tracing_on )
    {
          POMP2_Workshare_exit(SCOREP_POMP_F2C_REGION( region_handle ));
    }
}

int FSUB(POMP2_Lib_get_max_threads)()
{
    return omp_get_max_threads();
}

void FSUB(POMP2_Init_lock)(omp_lock_t *s) {
  if ( scorep_pomp_is_tracing_on ) {
	  POMP2_Init_lock(s);
  }else{
	  omp_init_lock(s);
  }
}

void FSUB(POMP2_Destroy_lock)(omp_lock_t *s) {
  if ( scorep_pomp_is_tracing_on ) {
	  POMP2_Destroy_lock(s);
  }else{
	  omp_destroy_lock(s);
  }
}

void FSUB(POMP2_Set_lock)(omp_lock_t *s) {
  if ( scorep_pomp_is_tracing_on ) {
	  POMP2_Set_lock(s);
  }else{
	  omp_set_lock(s);
  }
}

void FSUB(POMP2_Unset_lock)(omp_lock_t *s) {
  if ( scorep_pomp_is_tracing_on ) {
	  POMP2_Unset_lock(s);
  }else{
	  omp_unset_lock(s);
  }
}

int  FSUB(POMP2_Test_lock)(omp_lock_t *s) {
  if ( scorep_pomp_is_tracing_on ) {
	  return POMP2_Test_lock(s);
  }else{
	  return omp_test_lock(s);
  }
}

void FSUB(POMP2_Init_nest_lock)(omp_nest_lock_t *s) {
  if ( scorep_pomp_is_tracing_on ) {
	  POMP2_Init_nest_lock(s);
  }else{
	  omp_init_nest_lock(s);
  }
}

void FSUB(POMP2_Destroy_nest_lock)(omp_nest_lock_t *s) {
  if ( scorep_pomp_is_tracing_on ) {
	  POMP2_Destroy_nest_lock(s);
  }else{
	  omp_destroy_nest_lock(s);
  }
}

void FSUB(POMP2_Set_nest_lock)(omp_nest_lock_t *s) {
  if ( scorep_pomp_is_tracing_on ) {
	  POMP2_Set_nest_lock(s);
  }else{
	  omp_set_nest_lock(s);
  }
}

void FSUB(POMP2_Unset_nest_lock)(omp_nest_lock_t *s) {
  if ( scorep_pomp_is_tracing_on ) {
	  POMP2_Unset_nest_lock(s);
  }else{
	  omp_unset_nest_lock(s);
  }
}

int  FSUB(POMP2_Test_nest_lock)(omp_nest_lock_t *s) {
  if ( scorep_pomp_is_tracing_on ) {
	  return POMP2_Test_nest_lock(s);
  }else{
	  return omp_test_nest_lock(s);
  }
}
