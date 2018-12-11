/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2013, 2015, 2018,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2014,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2013,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2013,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 * @file
 *
 * @ingroup OPARI2
 *
 * @brief Implementation of the OPARI2 fortran user adapter functions and initialization.
 */

#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <opari2/pomp2_lib.h>
#include <opari2/pomp2_user_lib.h>
#include <SCOREP_InMeasurement.h>
#include <SCOREP_Opari2_Region_Info.h>
#include "SCOREP_Opari2_Openmp_Fortran.h"
#include "SCOREP_Opari2_Openmp_Regions.h"

/*
 * Fortran wrappers calling the C versions
 */
void
FSUB( POMP2_Atomic_enter )( POMP2_Region_handle_fortran* region_handle,
                            const char*                  ctc_string,
                            scorep_fortran_charlen_t     ctc_string_len )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    POMP2_Atomic_enter( SCOREP_POMP_F2C_REGION( region_handle ), ctc_string );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}

void
FSUB( POMP2_Atomic_exit )( POMP2_Region_handle_fortran* region_handle )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    POMP2_Atomic_exit( SCOREP_POMP_F2C_REGION( region_handle ) );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}

void
FSUB( POMP2_Barrier_enter )( POMP2_Region_handle_fortran* region_handle,
                             POMP2_Task_handle_fortran*   pomp_old_task,
                             const char*                  ctc_string,
                             scorep_fortran_charlen_t     ctc_string_len )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    POMP2_Barrier_enter( SCOREP_POMP_F2C_REGION( region_handle ),
                         SCOREP_POMP_F2C_TASK( pomp_old_task ),
                         ctc_string );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}

void
FSUB( POMP2_Barrier_exit )( POMP2_Region_handle_fortran* region_handle,
                            POMP2_Task_handle_fortran*   pomp_old_task )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    POMP2_Barrier_exit( SCOREP_POMP_F2C_REGION( region_handle ),
                        *SCOREP_POMP_F2C_TASK( pomp_old_task ) );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}

void
FSUB( POMP2_Implicit_barrier_enter )( POMP2_Region_handle_fortran* region_handle,
                                      POMP2_Task_handle_fortran*   pomp_old_task )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    POMP2_Implicit_barrier_enter( SCOREP_POMP_F2C_REGION( region_handle ),
                                  SCOREP_POMP_F2C_TASK( pomp_old_task ) );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}

void
FSUB( POMP2_Implicit_barrier_exit )( POMP2_Region_handle_fortran* region_handle,
                                     POMP2_Task_handle_fortran*   pomp_old_task )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    POMP2_Implicit_barrier_exit( SCOREP_POMP_F2C_REGION( region_handle ),
                                 *SCOREP_POMP_F2C_TASK( pomp_old_task ) );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}

void
FSUB( POMP2_Flush_enter )( POMP2_Region_handle_fortran* region_handle,
                           const char*                  ctc_string,
                           scorep_fortran_charlen_t     ctc_string_len )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    POMP2_Flush_enter( SCOREP_POMP_F2C_REGION( region_handle ), ctc_string );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}

void
FSUB( POMP2_Flush_exit )( POMP2_Region_handle_fortran* region_handle )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    POMP2_Flush_exit( SCOREP_POMP_F2C_REGION( region_handle ) );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}

void
FSUB( POMP2_Critical_begin )( POMP2_Region_handle_fortran* region_handle )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    POMP2_Critical_begin( SCOREP_POMP_F2C_REGION( region_handle ) );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}

void
FSUB( POMP2_Critical_end )( POMP2_Region_handle_fortran* region_handle )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    POMP2_Critical_end( SCOREP_POMP_F2C_REGION( region_handle ) );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}

void
FSUB( POMP2_Critical_enter )( POMP2_Region_handle_fortran* region_handle,
                              const char*                  ctc_string,
                              scorep_fortran_charlen_t     ctc_string_len )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    POMP2_Critical_enter( SCOREP_POMP_F2C_REGION( region_handle ), ctc_string );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}

void
FSUB( POMP2_Critical_exit )( POMP2_Region_handle_fortran* region_handle )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    POMP2_Critical_exit( SCOREP_POMP_F2C_REGION( region_handle ) );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}

void
FSUB( POMP2_Do_enter )( POMP2_Region_handle_fortran* region_handle,
                        const char*                  ctc_string,
                        scorep_fortran_charlen_t     ctc_string_len )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    POMP2_For_enter( SCOREP_POMP_F2C_REGION( region_handle ), ctc_string );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}

void
FSUB( POMP2_Do_exit )( POMP2_Region_handle_fortran* region_handle )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    POMP2_For_exit( SCOREP_POMP_F2C_REGION( region_handle ) );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}

void
FSUB( POMP2_Master_begin )( POMP2_Region_handle_fortran* region_handle,
                            const char*                  ctc_string,
                            scorep_fortran_charlen_t     ctc_string_len )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    POMP2_Master_begin( SCOREP_POMP_F2C_REGION( region_handle ), ctc_string );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}

void
FSUB( POMP2_Master_end )( POMP2_Region_handle_fortran* region_handle )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    POMP2_Master_end( SCOREP_POMP_F2C_REGION( region_handle ) );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}

void
FSUB( POMP2_Ordered_enter )( POMP2_Region_handle_fortran* region_handle,
                             const char*                  ctc_string,
                             scorep_fortran_charlen_t     ctc_string_len )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    POMP2_Ordered_enter( SCOREP_POMP_F2C_REGION( region_handle ), ctc_string );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}

void
FSUB( POMP2_Ordered_exit )( POMP2_Region_handle_fortran* region_handle )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    POMP2_Ordered_exit( SCOREP_POMP_F2C_REGION( region_handle ) );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}

void
FSUB( POMP2_Ordered_begin )( POMP2_Region_handle_fortran* region_handle )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    POMP2_Ordered_begin( SCOREP_POMP_F2C_REGION( region_handle ) );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}

void
FSUB( POMP2_Ordered_end )( POMP2_Region_handle_fortran* region_handle )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    POMP2_Ordered_end( SCOREP_POMP_F2C_REGION( region_handle ) );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}

void
FSUB( POMP2_Parallel_begin )( POMP2_Region_handle_fortran* region_handle )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    POMP2_Parallel_begin( SCOREP_POMP_F2C_REGION( region_handle ) );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}

void
FSUB( POMP2_Parallel_end )( POMP2_Region_handle_fortran* region_handle )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    POMP2_Parallel_end( SCOREP_POMP_F2C_REGION( region_handle ) );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}

void
FSUB( POMP2_Parallel_fork )( POMP2_Region_handle_fortran* region_handle,
                             int*                         if_clause,
                             int*                         num_threads,
                             POMP2_Task_handle_fortran*   pomp_old_task,
                             const char*                  ctc_string,
                             scorep_fortran_charlen_t     ctc_string_len )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    POMP2_Parallel_fork( SCOREP_POMP_F2C_REGION( region_handle ),
                         *if_clause,
                         *num_threads,
                         SCOREP_POMP_F2C_TASK( pomp_old_task ),
                         ctc_string );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}

void
FSUB( POMP2_Parallel_join )( POMP2_Region_handle_fortran* region_handle,
                             POMP2_Task_handle_fortran*   pomp_old_task )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    POMP2_Parallel_join( SCOREP_POMP_F2C_REGION( region_handle ),
                         *SCOREP_POMP_F2C_TASK( pomp_old_task ) );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}

void
FSUB( POMP2_Section_begin )( POMP2_Region_handle_fortran* region_handle,
                             const char*                  ctc_string,
                             scorep_fortran_charlen_t     ctc_string_len )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    POMP2_Section_begin( SCOREP_POMP_F2C_REGION( region_handle ), ctc_string );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}

void
FSUB( POMP2_Section_end )( POMP2_Region_handle_fortran* region_handle )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    POMP2_Section_end( SCOREP_POMP_F2C_REGION( region_handle ) );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}

void
FSUB( POMP2_Sections_enter )( POMP2_Region_handle_fortran* region_handle,
                              const char*                  ctc_string,
                              scorep_fortran_charlen_t     ctc_string_len )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    POMP2_Sections_enter( SCOREP_POMP_F2C_REGION( region_handle ), ctc_string );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}

void
FSUB( POMP2_Sections_exit )( POMP2_Region_handle_fortran* region_handle )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    POMP2_Sections_exit( SCOREP_POMP_F2C_REGION( region_handle ) );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}

void
FSUB( POMP2_Single_begin )( POMP2_Region_handle_fortran* region_handle )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    POMP2_Single_begin( SCOREP_POMP_F2C_REGION( region_handle ) );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}

void
FSUB( POMP2_Single_end )( POMP2_Region_handle_fortran* region_handle )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    POMP2_Single_end( SCOREP_POMP_F2C_REGION( region_handle ) );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}

void
FSUB( POMP2_Single_enter )( POMP2_Region_handle_fortran* region_handle,
                            const char*                  ctc_string,
                            scorep_fortran_charlen_t     ctc_string_len )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    POMP2_Single_enter( SCOREP_POMP_F2C_REGION( region_handle ), ctc_string );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}

void
FSUB( POMP2_Single_exit )( POMP2_Region_handle_fortran* region_handle )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    POMP2_Single_exit( SCOREP_POMP_F2C_REGION( region_handle ) );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}

void
FSUB( POMP2_Task_create_begin )( POMP2_Region_handle_fortran* region_handle,
                                 POMP2_Task_handle_fortran*   pomp2_new_task,
                                 POMP2_Task_handle_fortran*   pomp2_old_task,
                                 int*                         pomp2_if,
                                 const char*                  ctc_string,
                                 scorep_fortran_charlen_t     ctc_string_len )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    POMP2_Task_create_begin( SCOREP_POMP_F2C_REGION( region_handle ),
                             SCOREP_POMP_F2C_TASK( pomp2_new_task ),
                             SCOREP_POMP_F2C_TASK( pomp2_old_task ),
                             *pomp2_if,
                             NULL );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}

void
FSUB( POMP2_Task_create_end )( POMP2_Region_handle_fortran* region_handle,
                               POMP2_Task_handle_fortran*   pomp2_old_task )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    POMP2_Task_create_end( SCOREP_POMP_F2C_REGION( region_handle ),
                           *SCOREP_POMP_F2C_TASK( pomp2_old_task ) );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}

void
FSUB( POMP2_Task_begin )( POMP2_Region_handle_fortran* region_handle,
                          POMP2_Task_handle*           pomp_new_task )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    POMP2_Task_begin( SCOREP_POMP_F2C_REGION( region_handle ),
                      *SCOREP_POMP_F2C_TASK( pomp_new_task ) );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}

void
FSUB( POMP2_Task_end )( POMP2_Region_handle_fortran* region_handle )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    POMP2_Task_end( SCOREP_POMP_F2C_REGION( region_handle ) );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}

void
FSUB( POMP2_Untied_task_create_begin )( POMP2_Region_handle_fortran* region_handle,
                                        POMP2_Task_handle_fortran*   pomp2_new_task,
                                        POMP2_Task_handle_fortran*   pomp2_old_task,
                                        int*                         pomp2_if,
                                        const char*                  ctc_string,
                                        scorep_fortran_charlen_t     ctc_string_len )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    POMP2_Task_create_begin( SCOREP_POMP_F2C_REGION( region_handle ),
                             SCOREP_POMP_F2C_TASK( pomp2_new_task ),
                             SCOREP_POMP_F2C_TASK( pomp2_old_task ),
                             *pomp2_if,
                             ctc_string );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}

void
FSUB( POMP2_Untied_task_create_end )( POMP2_Region_handle_fortran* region_handle,
                                      POMP2_Task_handle_fortran*   pomp2_old_task )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    POMP2_Task_create_end( SCOREP_POMP_F2C_REGION( region_handle ),
                           *SCOREP_POMP_F2C_TASK( pomp2_old_task ) );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}

void
FSUB( POMP2_Untied_task_begin )( POMP2_Region_handle_fortran* region_handle,
                                 POMP2_Task_handle*           pomp_new_task )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    POMP2_Task_begin( SCOREP_POMP_F2C_REGION( region_handle ),
                      *SCOREP_POMP_F2C_TASK( pomp_new_task ) );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}

void
FSUB( POMP2_Untied_task_end )( POMP2_Region_handle_fortran* region_handle )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    POMP2_Task_end( SCOREP_POMP_F2C_REGION( region_handle ) );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}

void
FSUB( POMP2_Taskwait_begin )( POMP2_Region_handle_fortran* region_handle,
                              POMP2_Task_handle_fortran*   pomp2_old_task,
                              const char*                  ctc_string,
                              scorep_fortran_charlen_t     ctc_string_len )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    POMP2_Taskwait_begin( SCOREP_POMP_F2C_REGION( region_handle ),
                          SCOREP_POMP_F2C_TASK( pomp2_old_task ),
                          ctc_string );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}

void
FSUB( POMP2_Taskwait_end )( POMP2_Region_handle_fortran* region_handle,
                            POMP2_Task_handle_fortran*   pomp2_old_task )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    POMP2_Taskwait_end( SCOREP_POMP_F2C_REGION( region_handle ),
                        *SCOREP_POMP_F2C_TASK( pomp2_old_task ) );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}

void
FSUB( POMP2_Workshare_enter )( POMP2_Region_handle_fortran* region_handle,
                               const char*                  ctc_string,
                               scorep_fortran_charlen_t     ctc_string_len )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    POMP2_Workshare_enter( SCOREP_POMP_F2C_REGION( region_handle ), ctc_string );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}

void
FSUB( POMP2_Workshare_exit )( POMP2_Region_handle_fortran* region_handle )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    POMP2_Workshare_exit( SCOREP_POMP_F2C_REGION( region_handle ) );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}

int
FSUB( POMP2_Lib_get_max_threads )( void )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    int result = omp_get_max_threads();
    SCOREP_IN_MEASUREMENT_DECREMENT();
    return result;
}

void
FSUB( POMP2_Init_lock )( omp_lock_t* s )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    POMP2_Init_lock( s );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}

void
FSUB( POMP2_Destroy_lock )( omp_lock_t* s )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    POMP2_Destroy_lock( s );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}

void
FSUB( POMP2_Set_lock )( omp_lock_t* s )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    POMP2_Set_lock( s );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}

void
FSUB( POMP2_Unset_lock )( omp_lock_t* s )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    POMP2_Unset_lock( s );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}

int
FSUB( POMP2_Test_lock )( omp_lock_t* s )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    int result = POMP2_Test_lock( s );
    SCOREP_IN_MEASUREMENT_DECREMENT();
    return result;
}

void
FSUB( POMP2_Init_nest_lock )( omp_nest_lock_t* s )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    POMP2_Init_nest_lock( s );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}

void
FSUB( POMP2_Destroy_nest_lock )( omp_nest_lock_t* s )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    POMP2_Destroy_nest_lock( s );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}

void
FSUB( POMP2_Set_nest_lock )( omp_nest_lock_t* s )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    POMP2_Set_nest_lock( s );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}

void
FSUB( POMP2_Unset_nest_lock )( omp_nest_lock_t* s )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    POMP2_Unset_nest_lock( s );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}

int
FSUB( POMP2_Test_nest_lock )( omp_nest_lock_t* s )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    int result = POMP2_Test_nest_lock( s );
    SCOREP_IN_MEASUREMENT_DECREMENT();
    return result;
}
