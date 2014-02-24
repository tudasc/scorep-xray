/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2011,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2011,2014
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2011,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2013,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2011,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2011,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 */

/**
 * @file
 * @ingroup    POMP2
 *
 * @brief Implementation of the POMP2 OpenMP adapter functions.
 */

#include <config.h>

#include "SCOREP_Pomp_Common.h"
#include "SCOREP_Pomp_RegionInfo.h"
#include "SCOREP_Pomp_Lock.h"

#include <SCOREP_Events.h>
#include <SCOREP_ThreadForkJoin_Event.h>

#define SCOREP_DEBUG_MODULE_NAME OPENMP
#include <UTILS_Error.h>
#include <UTILS_Debug.h>

#include <opari2/pomp2_lib.h>


/** Id of the currently executing task*/
POMP2_Task_handle pomp_current_task = 0;
#pragma omp threadprivate(pomp_current_task)

/** Counter of tasks used to determine task ids for newly created tasks*/
POMP2_Task_handle pomp_task_counter = 1;
#pragma omp threadprivate(pomp_task_counter)

POMP2_Task_handle
POMP2_Get_new_task_handle( void )
{
    return ( ( POMP2_Task_handle )omp_get_thread_num() << 32 ) + pomp_task_counter++;
}

void
POMP2_Set_current_task( POMP2_Task_handle task )
{
    pomp_current_task = task;
}

POMP2_Task_handle
POMP2_Get_current_task( void )
{
    return pomp_current_task;
}

#define pomp2_decode_task_handle( task ) \
    ( ( uint32_t )( ( task ) >> 32 ) ), \
    ( ( uint32_t )( ( task ) & 0xFFFFFFFFu ) )


/** @ingroup POMP2
    @{
 */

/* **************************************************************************************
 *                                                                   POMP event functions
 ***************************************************************************************/

void
POMP2_Atomic_enter( POMP2_Region_handle* pomp_handle,
                    const char           ctc_string[] )
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OPENMP, "In POMP2_Atomic_enter" );
    SCOREP_POMP2_ENSURE_INITIALIZED;
    if ( scorep_pomp_is_tracing_on )
    {
        SCOREP_POMP2_HANDLE_UNITIALIZED_REGION( pomp_handle, ctc_string );
        SCOREP_Pomp_Region* region = *( SCOREP_Pomp_Region** )pomp_handle;
        SCOREP_EnterRegion( region->outerBlock );
    }
}

void
POMP2_Atomic_exit( POMP2_Region_handle* pomp_handle )
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OPENMP, "In POMP2_Atomic_exit" );
    if ( scorep_pomp_is_tracing_on )
    {
        SCOREP_Pomp_Region* region = *( SCOREP_Pomp_Region** )pomp_handle;
        SCOREP_ExitRegion( region->outerBlock );
    }
}

void
POMP2_Barrier_enter( POMP2_Region_handle* pomp_handle,
                     POMP2_Task_handle*   pomp_old_task,
                     const char           ctc_string[] )
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OPENMP, "In POMP2_Barrier_enter" );
    SCOREP_POMP2_ENSURE_INITIALIZED;

    *pomp_old_task = pomp_current_task;
    if ( scorep_pomp_is_tracing_on )
    {
        SCOREP_POMP2_HANDLE_UNITIALIZED_REGION( pomp_handle, ctc_string );
        SCOREP_Pomp_Region* region = *( SCOREP_Pomp_Region** )pomp_handle;
        SCOREP_EnterRegion( region->outerBlock );
    }
}

void
POMP2_Barrier_exit( POMP2_Region_handle* pomp_handle,
                    POMP2_Task_handle    pomp_old_task )
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OPENMP, "In POMP2_Barrier_exit" );

    pomp_current_task = pomp_old_task;
    if ( scorep_pomp_is_tracing_on )
    {
        SCOREP_Pomp_Region* region = *( SCOREP_Pomp_Region** )pomp_handle;
        if ( pomp_current_task != pomp_old_task )
        {
            SCOREP_ThreadForkJoin_TaskSwitch( SCOREP_PARADIGM_OPENMP,
                                              pomp2_decode_task_handle( pomp_current_task ) );
        }
        SCOREP_ExitRegion( region->outerBlock );
    }
}

void
POMP2_Implicit_barrier_enter( POMP2_Region_handle* pomp_handle,
                              POMP2_Task_handle*   pomp_old_task )
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OPENMP, "In POMP2_Implicit_barrier_enter" );
    SCOREP_POMP2_ENSURE_INITIALIZED;

    *pomp_old_task = pomp_current_task;
    if ( scorep_pomp_is_tracing_on )
    {
        SCOREP_Pomp_Region* region = *( SCOREP_Pomp_Region** )pomp_handle;
        SCOREP_EnterRegion( region->barrier );
    }
}

void
POMP2_Implicit_barrier_exit( POMP2_Region_handle* pomp_handle,
                             POMP2_Task_handle    pomp_old_task )
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OPENMP, "In POMP2_Implicit_barrier_exit" );

    pomp_current_task = pomp_old_task;
    if ( scorep_pomp_is_tracing_on )
    {
        SCOREP_Pomp_Region* region = *( SCOREP_Pomp_Region** )pomp_handle;
        if ( pomp_current_task != pomp_old_task )
        {
            SCOREP_ThreadForkJoin_TaskSwitch( SCOREP_PARADIGM_OPENMP,
                                              pomp2_decode_task_handle( pomp_current_task ) );
        }
        SCOREP_ExitRegion( region->barrier );
    }
}

void
POMP2_Flush_enter( POMP2_Region_handle* pomp_handle,
                   const char           ctc_string[] )
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OPENMP, "In POMP2_Flush_enter" );
    SCOREP_POMP2_ENSURE_INITIALIZED;
    if ( scorep_pomp_is_tracing_on )
    {
        SCOREP_POMP2_HANDLE_UNITIALIZED_REGION( pomp_handle, ctc_string );
        SCOREP_Pomp_Region* region = *( SCOREP_Pomp_Region** )pomp_handle;
        SCOREP_EnterRegion( region->outerBlock );
    }
}

void
POMP2_Flush_exit( POMP2_Region_handle* pomp_handle )
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OPENMP, "In POMP2_Flush_exit" );
    if ( scorep_pomp_is_tracing_on )
    {
        SCOREP_Pomp_Region* region = *( SCOREP_Pomp_Region** )pomp_handle;
        SCOREP_ExitRegion( region->outerBlock );
    }
}

void
POMP2_Critical_begin( POMP2_Region_handle* pomp_handle )
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OPENMP, "In POMP2_Critical_begin" );
    SCOREP_POMP2_ENSURE_INITIALIZED;
    if ( scorep_pomp_is_tracing_on )
    {
        SCOREP_Pomp_Region* region = *( SCOREP_Pomp_Region** )pomp_handle;
        SCOREP_ThreadAcquireLock( SCOREP_PARADIGM_OPENMP,
                                  region->lock->handle,
                                  region->lock->acquisition_order );
        SCOREP_EnterRegion( region->innerBlock );
    }
}

void
POMP2_Critical_end( POMP2_Region_handle* pomp_handle )
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OPENMP, "In POMP2_Critical_end" );
    if ( scorep_pomp_is_tracing_on )
    {
        SCOREP_Pomp_Region* region = *( SCOREP_Pomp_Region** )pomp_handle;
        SCOREP_ExitRegion( region->innerBlock );
        SCOREP_ThreadReleaseLock( SCOREP_PARADIGM_OPENMP,
                                  region->lock->handle,
                                  region->lock->acquisition_order );
    }
}

void
POMP2_Critical_enter( POMP2_Region_handle* pomp_handle,
                      const char           ctc_string[]  )
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OPENMP, "In POMP2_Critical_enter" );
    SCOREP_POMP2_ENSURE_INITIALIZED;
    if ( scorep_pomp_is_tracing_on )
    {
        SCOREP_POMP2_HANDLE_UNITIALIZED_REGION( pomp_handle, ctc_string );
        SCOREP_Pomp_Region* region = *( SCOREP_Pomp_Region** )pomp_handle;
        SCOREP_EnterRegion( region->outerBlock );
    }
}

void
POMP2_Critical_exit( POMP2_Region_handle* pomp_handle )
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OPENMP, "In POMP2_Critical_exit" );
    if ( scorep_pomp_is_tracing_on )
    {
        SCOREP_Pomp_Region* region = *( SCOREP_Pomp_Region** )pomp_handle;
        SCOREP_ExitRegion( region->outerBlock );
    }
}

void
POMP2_For_enter( POMP2_Region_handle* pomp_handle,
                 const char           ctc_string[]  )
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OPENMP, "In POMP2_For_enter" );
    SCOREP_POMP2_ENSURE_INITIALIZED;
    if ( scorep_pomp_is_tracing_on )
    {
        SCOREP_POMP2_HANDLE_UNITIALIZED_REGION( pomp_handle, ctc_string );
        SCOREP_Pomp_Region* region = *( SCOREP_Pomp_Region** )pomp_handle;
        SCOREP_EnterRegion( region->outerBlock );
    }
}

void
POMP2_For_exit( POMP2_Region_handle* pomp_handle )
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OPENMP, "In POMP2_For_exit" );
    if ( scorep_pomp_is_tracing_on )
    {
        SCOREP_Pomp_Region* region = *( SCOREP_Pomp_Region** )pomp_handle;
        SCOREP_ExitRegion( region->outerBlock );
    }
}

void
POMP2_Master_begin( POMP2_Region_handle* pomp_handle,
                    const char           ctc_string[]  )
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OPENMP, "In POMP2_Master_begin" );
    SCOREP_POMP2_ENSURE_INITIALIZED;
    if ( scorep_pomp_is_tracing_on )
    {
        SCOREP_POMP2_HANDLE_UNITIALIZED_REGION( pomp_handle, ctc_string );
        SCOREP_Pomp_Region* region = *( SCOREP_Pomp_Region** )pomp_handle;
        SCOREP_EnterRegion( region->innerBlock );
    }
}

void
POMP2_Master_end( POMP2_Region_handle* pomp_handle )
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OPENMP, "In POMP2_Master_end" );
    if ( scorep_pomp_is_tracing_on )
    {
        SCOREP_Pomp_Region* region = *( SCOREP_Pomp_Region** )pomp_handle;
        SCOREP_ExitRegion( region->innerBlock );
    }
}

void
POMP2_Parallel_begin( POMP2_Region_handle* pomp_handle )
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OPENMP, "In POMP2_Parallel_begin" );
    SCOREP_POMP2_ENSURE_INITIALIZED;
    // always use task ID 0 for implicit tasks created at the begin
    // of a parallel region
    pomp_current_task = 0;

    if ( !scorep_pomp_is_finalized )
    {
        SCOREP_ThreadForkJoin_TeamBegin( SCOREP_PARADIGM_OPENMP,
                                         omp_get_thread_num() );
        SCOREP_Pomp_Region* region = *( SCOREP_Pomp_Region** )pomp_handle;
        SCOREP_EnterRegion( region->innerParallel );
    }
}

void
POMP2_Parallel_end( POMP2_Region_handle* pomp_handle )
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OPENMP, "In POMP2_Parallel_end" );
    if ( !scorep_pomp_is_finalized )
    {
        SCOREP_Pomp_Region* region = *( SCOREP_Pomp_Region** )pomp_handle;
        SCOREP_ExitRegion( region->innerParallel );
        SCOREP_ThreadForkJoin_TeamEnd( SCOREP_PARADIGM_OPENMP );
    }
}

void
POMP2_Parallel_fork( POMP2_Region_handle* pomp_handle,
                     int                  if_clause,
                     int                  num_threads,
                     POMP2_Task_handle*   pomp_old_task,
                     const char           ctc_string[] )
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OPENMP, "In POMP2_Parallel_fork" );
    SCOREP_POMP2_ENSURE_INITIALIZED;

    *pomp_old_task = pomp_current_task;
    /* Generate fork event */
    if ( !scorep_pomp_is_finalized )
    {
        SCOREP_POMP2_HANDLE_UNITIALIZED_REGION( pomp_handle, ctc_string );
        SCOREP_Pomp_Region* region = *( SCOREP_Pomp_Region** )pomp_handle;
        UTILS_ASSERT( region != NULL );
        SCOREP_ThreadForkJoin_Fork( SCOREP_PARADIGM_OPENMP,
                                    num_threads );
    }
}

void
POMP2_Parallel_join( POMP2_Region_handle* pomp_handle,
                     POMP2_Task_handle    pomp_old_task )
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OPENMP, "In POMP2_Parallel_join" );

    pomp_current_task = pomp_old_task;
    if ( !scorep_pomp_is_finalized )
    {
        SCOREP_Pomp_Region* region = *( SCOREP_Pomp_Region** )pomp_handle;
        SCOREP_ThreadForkJoin_Join( SCOREP_PARADIGM_OPENMP );
    }
}

void
POMP2_Section_begin( POMP2_Region_handle* pomp_handle,
                     const char           ctc_string[]  )
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OPENMP, "In POMP2_Section_begin" );
    SCOREP_POMP2_ENSURE_INITIALIZED;
    if ( scorep_pomp_is_tracing_on )
    {
        SCOREP_POMP2_HANDLE_UNITIALIZED_REGION( pomp_handle, ctc_string );
        SCOREP_Pomp_Region* region = *( SCOREP_Pomp_Region** )pomp_handle;
        SCOREP_EnterRegion( region->innerBlock );
    }
}

void
POMP2_Section_end( POMP2_Region_handle* pomp_handle )
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OPENMP, "In POMP2_Section_end" );
    if ( scorep_pomp_is_tracing_on )
    {
        SCOREP_Pomp_Region* region = *( SCOREP_Pomp_Region** )pomp_handle;
        SCOREP_ExitRegion( region->innerBlock );
    }
}

void
POMP2_Sections_enter( POMP2_Region_handle* pomp_handle,
                      const char           ctc_string[]  )
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OPENMP, "In POMP2_Sections_enter" );
    SCOREP_POMP2_ENSURE_INITIALIZED;
    if ( scorep_pomp_is_tracing_on )
    {
        SCOREP_POMP2_HANDLE_UNITIALIZED_REGION( pomp_handle, ctc_string );
        SCOREP_Pomp_Region* region = *( SCOREP_Pomp_Region** )pomp_handle;
        SCOREP_EnterRegion( region->outerBlock );
    }
}

void
POMP2_Sections_exit( POMP2_Region_handle* pomp_handle )
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OPENMP, "In POMP2_Sections_exit" );
    if ( scorep_pomp_is_tracing_on )
    {
        SCOREP_Pomp_Region* region = *( SCOREP_Pomp_Region** )pomp_handle;
        SCOREP_ExitRegion( region->outerBlock );
    }
}

void
POMP2_Single_begin( POMP2_Region_handle* pomp_handle )
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OPENMP, "In POMP2_Single_begin" );
    SCOREP_POMP2_ENSURE_INITIALIZED;
    if ( scorep_pomp_is_tracing_on )
    {
        SCOREP_Pomp_Region* region = *( SCOREP_Pomp_Region** )pomp_handle;
        SCOREP_EnterRegion( region->innerBlock );
    }
}

void
POMP2_Single_end( POMP2_Region_handle* pomp_handle )
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OPENMP, "In POMP2_Single_end" );
    if ( scorep_pomp_is_tracing_on )
    {
        SCOREP_Pomp_Region* region = *( SCOREP_Pomp_Region** )pomp_handle;
        SCOREP_ExitRegion( region->innerBlock );
    }
}

void
POMP2_Single_enter( POMP2_Region_handle* pomp_handle,
                    const char           ctc_string[]  )
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OPENMP, "In POMP2_Single_enter" );
    SCOREP_POMP2_ENSURE_INITIALIZED;
    if ( scorep_pomp_is_tracing_on )
    {
        SCOREP_POMP2_HANDLE_UNITIALIZED_REGION( pomp_handle, ctc_string );
        SCOREP_Pomp_Region* region = *( SCOREP_Pomp_Region** )pomp_handle;
        SCOREP_EnterRegion( region->outerBlock );
    }
}

void
POMP2_Single_exit( POMP2_Region_handle* pomp_handle )
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OPENMP, "In POMP2_Single_exit" );
    if ( scorep_pomp_is_tracing_on )
    {
        SCOREP_Pomp_Region* region = *( SCOREP_Pomp_Region** )pomp_handle;
        SCOREP_ExitRegion( region->outerBlock );
    }
}

//=======================================================================================
//=======================================================================================
//=======================================================================================

void
POMP2_Ordered_begin( POMP2_Region_handle* pomp_handle )
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OPENMP, "In POMP2_Ordered_begin" );
    SCOREP_POMP2_ENSURE_INITIALIZED;
    if ( scorep_pomp_is_tracing_on )
    {
        SCOREP_Pomp_Region* region = *( SCOREP_Pomp_Region** )pomp_handle;
        SCOREP_EnterRegion( region->innerBlock );
    }
}

void
POMP2_Ordered_end( POMP2_Region_handle* pomp_handle )
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OPENMP, "In POMP2_Ordered_end" );
    if ( scorep_pomp_is_tracing_on )
    {
        SCOREP_Pomp_Region* region = *( SCOREP_Pomp_Region** )pomp_handle;
        SCOREP_ExitRegion( region->innerBlock );
    }
}

void
POMP2_Ordered_enter( POMP2_Region_handle* pomp_handle,
                     const char           ctc_string[] )
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OPENMP, "In POMP2_Ordered_enter" );
    SCOREP_POMP2_ENSURE_INITIALIZED;
    if ( scorep_pomp_is_tracing_on )
    {
        SCOREP_POMP2_HANDLE_UNITIALIZED_REGION( pomp_handle, ctc_string );
        SCOREP_Pomp_Region* region = *( SCOREP_Pomp_Region** )pomp_handle;
        SCOREP_EnterRegion( region->outerBlock );
    }
}

void
POMP2_Ordered_exit( POMP2_Region_handle* pomp_handle )
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OPENMP, "In POMP2_Ordered_exit" );
    if ( scorep_pomp_is_tracing_on )
    {
        SCOREP_Pomp_Region* region = *( SCOREP_Pomp_Region** )pomp_handle;
        SCOREP_ExitRegion( region->outerBlock );
    }
}


void
POMP2_Task_create_begin( POMP2_Region_handle* pomp_handle,
                         POMP2_Task_handle*   pomp_new_task,
                         POMP2_Task_handle*   pomp_old_task,
                         int                  pomp_if,
                         const char           ctc_string[] )
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OPENMP, "In POMP2_Task_create_begin" );
    SCOREP_POMP2_ENSURE_INITIALIZED;

    /* We must use pomp_old_task to reset the old task id after the creation.
       We cannot store the new task id in pomp_current_task, because other tasks
       maybe executed before. */
    *pomp_old_task = pomp_current_task;
    *pomp_new_task = POMP2_Get_new_task_handle();
    if ( scorep_pomp_is_tracing_on )
    {
        SCOREP_POMP2_HANDLE_UNITIALIZED_REGION( pomp_handle, ctc_string );
        SCOREP_Pomp_Region* region = *( SCOREP_Pomp_Region** )pomp_handle;
        SCOREP_EnterRegion( region->outerBlock );
        SCOREP_ThreadForkJoin_TaskCreate( SCOREP_PARADIGM_OPENMP,
                                          pomp2_decode_task_handle( *pomp_new_task ) );
    }
}

void
POMP2_Task_create_end( POMP2_Region_handle* pomp_handle,
                       POMP2_Task_handle    pomp_old_task )
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OPENMP, "In POMP2_Task_create_end" );

    if ( scorep_pomp_is_tracing_on )
    {
        SCOREP_Pomp_Region* region = *( SCOREP_Pomp_Region** )pomp_handle;
        if ( pomp_current_task != pomp_old_task )
        {
            SCOREP_ThreadForkJoin_TaskSwitch( SCOREP_PARADIGM_OPENMP,
                                              pomp2_decode_task_handle( pomp_old_task ) );
        }
        SCOREP_ExitRegion( region->outerBlock );
    }
    pomp_current_task = pomp_old_task;
}

void
POMP2_Task_begin( POMP2_Region_handle* pomp_handle,
                  POMP2_Task_handle    pomp_task )
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OPENMP, "In POMP2_Task_begin" );
    SCOREP_POMP2_ENSURE_INITIALIZED;

    pomp_current_task = pomp_task;

    if ( scorep_pomp_is_tracing_on )
    {
        SCOREP_Pomp_Region* region = *( SCOREP_Pomp_Region** )pomp_handle;
        SCOREP_ThreadForkJoin_TaskBegin( SCOREP_PARADIGM_OPENMP,
                                         region->innerBlock,
                                         pomp2_decode_task_handle( pomp_current_task ) );
    }
}

void
POMP2_Task_end( POMP2_Region_handle* pomp_handle )
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OPENMP, "In POMP2_Task_end" );
    if ( scorep_pomp_is_tracing_on )
    {
        SCOREP_Pomp_Region* region = *( SCOREP_Pomp_Region** )pomp_handle;
        SCOREP_ThreadForkJoin_TaskEnd( SCOREP_PARADIGM_OPENMP,
                                       region->innerBlock,
                                       pomp2_decode_task_handle( pomp_current_task ) );
    }
}

void
POMP2_Untied_task_create_begin( POMP2_Region_handle* pomp_handle,
                                POMP2_Task_handle*   pomp_new_task,
                                POMP2_Task_handle*   pomp_old_task,
                                int                  pomp_if,
                                const char           ctc_string[] )
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OPENMP, "In POMP2_Untied_task_create_begin" );
    SCOREP_POMP2_ENSURE_INITIALIZED;
    *pomp_new_task = POMP2_Get_new_task_handle();
    *pomp_old_task = pomp_current_task;

    if ( scorep_pomp_is_tracing_on )
    {
        SCOREP_POMP2_HANDLE_UNITIALIZED_REGION( pomp_handle, ctc_string );
        SCOREP_Pomp_Region* region = *( SCOREP_Pomp_Region** )pomp_handle;
        SCOREP_EnterRegion( region->outerBlock );
        SCOREP_ThreadForkJoin_TaskCreate( SCOREP_PARADIGM_OPENMP,
                                          pomp2_decode_task_handle( *pomp_new_task ) );
    }
}

void
POMP2_Untied_task_create_end( POMP2_Region_handle* pomp_handle,
                              POMP2_Task_handle    pomp_old_task )
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OPENMP, "In POMP2_Untied_task_create_end" );

    if ( scorep_pomp_is_tracing_on )
    {
        SCOREP_Pomp_Region* region = *( SCOREP_Pomp_Region** )pomp_handle;
        if ( pomp_current_task != pomp_old_task )
        {
            SCOREP_ThreadForkJoin_TaskSwitch( SCOREP_PARADIGM_OPENMP,
                                              pomp2_decode_task_handle( pomp_old_task ) );
        }
        SCOREP_ExitRegion( region->outerBlock );
    }
    pomp_current_task = pomp_old_task;
}

void
POMP2_Untied_task_begin( POMP2_Region_handle* pomp_handle,
                         POMP2_Task_handle    pomp_task )
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OPENMP, "In POMP2_Untied_task_begin" );
    SCOREP_POMP2_ENSURE_INITIALIZED;

    pomp_current_task = pomp_task;

    if ( scorep_pomp_is_tracing_on )
    {
        SCOREP_Pomp_Region* region = *( SCOREP_Pomp_Region** )pomp_handle;
        SCOREP_ThreadForkJoin_TaskBegin( SCOREP_PARADIGM_OPENMP,
                                         region->innerBlock,
                                         pomp2_decode_task_handle( pomp_current_task ) );
    }
}

void
POMP2_Untied_task_end( POMP2_Region_handle* pomp_handle )
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OPENMP, "In POMP2_Untied_task_end" );
    if ( scorep_pomp_is_tracing_on )
    {
        SCOREP_Pomp_Region* region = *( SCOREP_Pomp_Region** )pomp_handle;
        SCOREP_ThreadForkJoin_TaskEnd( SCOREP_PARADIGM_OPENMP,
                                       region->innerBlock,
                                       pomp2_decode_task_handle( pomp_current_task ) );
    }
}

void
POMP2_Taskwait_begin( POMP2_Region_handle* pomp_handle,
                      POMP2_Task_handle*   pomp_old_task,
                      const char           ctc_string[] )
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OPENMP, "In POMP2_Taskwait_begin" );
    SCOREP_POMP2_ENSURE_INITIALIZED;

    *pomp_old_task = pomp_current_task;
    if ( scorep_pomp_is_tracing_on )
    {
        SCOREP_POMP2_HANDLE_UNITIALIZED_REGION( pomp_handle, ctc_string );
        SCOREP_Pomp_Region* region = *( SCOREP_Pomp_Region** )pomp_handle;
        SCOREP_EnterRegion( region->outerBlock );
    }
}

void
POMP2_Taskwait_end( POMP2_Region_handle* pomp_handle,
                    POMP2_Task_handle    pomp_old_task )
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OPENMP, "In POMP2_Taskwait_end" );

    if ( scorep_pomp_is_tracing_on )
    {
        SCOREP_Pomp_Region* region = *( SCOREP_Pomp_Region** )pomp_handle;
        if ( pomp_current_task != pomp_old_task )
        {
            SCOREP_ThreadForkJoin_TaskSwitch( SCOREP_PARADIGM_OPENMP,
                                              pomp2_decode_task_handle( pomp_old_task ) );
        }
        SCOREP_ExitRegion( region->outerBlock );
    }
    pomp_current_task = pomp_old_task;
}

//=======================================================================================
//=======================================================================================
//=======================================================================================


void
POMP2_Workshare_enter( POMP2_Region_handle* pomp_handle,
                       const char           ctc_string[]  )
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OPENMP, "In POMP2_Workshare_enter" );
    SCOREP_POMP2_ENSURE_INITIALIZED;
    if ( scorep_pomp_is_tracing_on )
    {
        SCOREP_POMP2_HANDLE_UNITIALIZED_REGION( pomp_handle, ctc_string );
        SCOREP_Pomp_Region* region = *( SCOREP_Pomp_Region** )pomp_handle;
        SCOREP_EnterRegion( region->outerBlock );
    }
}

void
POMP2_Workshare_exit( POMP2_Region_handle* pomp_handle )
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OPENMP, "In POMP2_Workshare_exit" );
    if ( scorep_pomp_is_tracing_on )
    {
        SCOREP_Pomp_Region* region = *( SCOREP_Pomp_Region** )pomp_handle;
        SCOREP_ExitRegion( region->outerBlock );
    }
}

int
POMP2_Lib_get_max_threads( void )
{
    return omp_get_max_threads();
}

/* **************************************************************************************
 *                                                                  C wrapper for OMP API
 ***************************************************************************************/

#ifdef _OPENMP

void
POMP2_Init_lock( omp_lock_t* s )
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OPENMP, "In POMP2_Init_lock" );
    if ( scorep_pomp_is_finalized )
    {
        omp_init_lock( s );
        return;
    }
    SCOREP_POMP2_ENSURE_INITIALIZED;
    if ( scorep_pomp_is_tracing_on )
    {
        SCOREP_EnterRegion( scorep_pomp_lock_region_handles[ SCOREP_POMP_INIT_LOCK ] );
        omp_init_lock( s );
        scorep_pomp_lock_init( s );
        SCOREP_ExitRegion( scorep_pomp_lock_region_handles[ SCOREP_POMP_INIT_LOCK ] );
    }
    else
    {
        omp_init_lock( s );
        scorep_pomp_lock_init( s );
    }
}

void
POMP2_Destroy_lock( omp_lock_t* s )
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OPENMP, "In POMP2_Destroy_lock" );
    if ( scorep_pomp_is_finalized )
    {
        omp_destroy_lock( s );
    }
    else if ( scorep_pomp_is_tracing_on )
    {
        SCOREP_EnterRegion( scorep_pomp_lock_region_handles[ SCOREP_POMP_DESTROY_LOCK ] );
        omp_destroy_lock( s );
        scorep_pomp_lock_destroy( s );
        SCOREP_ExitRegion( scorep_pomp_lock_region_handles[ SCOREP_POMP_DESTROY_LOCK ] );
    }
    else
    {
        omp_destroy_lock( s );
        scorep_pomp_lock_destroy( s );
    }
}

void
POMP2_Set_lock( omp_lock_t* s )
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OPENMP, "In POMP2_Set_lock" );
    if ( scorep_pomp_is_tracing_on )
    {
        SCOREP_EnterRegion( scorep_pomp_lock_region_handles[ SCOREP_POMP_SET_LOCK ] );
        omp_set_lock( s );

        SCOREP_MutexLock( scorep_pomp_lock_lock );
        SCOREP_PompLock* lock = SCOREP_Pomp_GetAcquireLock( s );
        SCOREP_ThreadAcquireLock( SCOREP_PARADIGM_OPENMP, lock->handle, lock->acquisition_order );
        SCOREP_MutexUnlock( scorep_pomp_lock_lock );

        SCOREP_ExitRegion( scorep_pomp_lock_region_handles[ SCOREP_POMP_SET_LOCK ] );
    }
    else
    {
        omp_set_lock( s );
    }
}

void
POMP2_Unset_lock( omp_lock_t* s )
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OPENMP, "In POMP2_Unset_lock" );
    if ( scorep_pomp_is_tracing_on )
    {
        SCOREP_EnterRegion( scorep_pomp_lock_region_handles[ SCOREP_POMP_UNSET_LOCK ] );

        SCOREP_MutexLock( scorep_pomp_lock_lock );
        SCOREP_PompLock* lock = SCOREP_Pomp_GetReleaseLock( s );
        SCOREP_ThreadReleaseLock( SCOREP_PARADIGM_OPENMP, lock->handle, lock->acquisition_order );
        SCOREP_MutexUnlock( scorep_pomp_lock_lock );

        omp_unset_lock( s );
        SCOREP_ExitRegion( scorep_pomp_lock_region_handles[ SCOREP_POMP_UNSET_LOCK ] );
    }
    else
    {
        omp_unset_lock( s );
    }
}

int
POMP2_Test_lock( omp_lock_t* s )
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OPENMP, "In POMP2_Test_lock" );
    if ( scorep_pomp_is_tracing_on )
    {
        int result;

        SCOREP_EnterRegion( scorep_pomp_lock_region_handles[ SCOREP_POMP_TEST_LOCK ] );
        result = omp_test_lock( s );
        if ( result )
        {
            SCOREP_MutexLock( scorep_pomp_lock_lock );
            SCOREP_PompLock* lock = SCOREP_Pomp_GetAcquireLock( s );
            SCOREP_ThreadAcquireLock( SCOREP_PARADIGM_OPENMP, lock->handle, lock->acquisition_order );
            SCOREP_MutexUnlock( scorep_pomp_lock_lock );
        }
        SCOREP_ExitRegion( scorep_pomp_lock_region_handles[ SCOREP_POMP_TEST_LOCK ] );
        return result;
    }
    else
    {
        return omp_test_lock( s );
    }
}

void
POMP2_Init_nest_lock( omp_nest_lock_t* s )
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OPENMP, "In POMP2_Init_nest_lock" );
    if ( scorep_pomp_is_finalized )
    {
        omp_init_nest_lock( s );
        return;
    }
    SCOREP_POMP2_ENSURE_INITIALIZED;
    if ( scorep_pomp_is_tracing_on )
    {
        SCOREP_EnterRegion( scorep_pomp_lock_region_handles[ SCOREP_POMP_INIT_NEST_LOCK ] );
        omp_init_nest_lock( s );
        scorep_pomp_lock_init( s );
        SCOREP_ExitRegion( scorep_pomp_lock_region_handles[ SCOREP_POMP_INIT_NEST_LOCK ] );
    }
    else
    {
        omp_init_nest_lock( s );
        scorep_pomp_lock_init( s );
    }
}

void
POMP2_Destroy_nest_lock( omp_nest_lock_t* s )
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OPENMP, "In POMP2_Destroy_nest_lock" );
    if ( scorep_pomp_is_finalized )
    {
        omp_destroy_nest_lock( s );
    }
    else if ( scorep_pomp_is_tracing_on )
    {
        SCOREP_EnterRegion( scorep_pomp_lock_region_handles[ SCOREP_POMP_DESTROY_NEST_LOCK ] );
        omp_destroy_nest_lock( s );
        scorep_pomp_lock_destroy( s );
        SCOREP_ExitRegion( scorep_pomp_lock_region_handles[ SCOREP_POMP_DESTROY_NEST_LOCK ] );
    }
    else
    {
        omp_destroy_nest_lock( s );
        scorep_pomp_lock_destroy( s );
    }
}

void
POMP2_Set_nest_lock( omp_nest_lock_t* s )
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OPENMP, "In POMP2_Set_nest_lock" );
    if ( scorep_pomp_is_tracing_on )
    {
        SCOREP_EnterRegion( scorep_pomp_lock_region_handles[ SCOREP_POMP_SET_NEST_LOCK ] );
        omp_set_nest_lock( s );
        SCOREP_MutexLock( scorep_pomp_lock_lock );
        SCOREP_PompLock* lock = SCOREP_Pomp_GetAcquireNestLock( s );
        SCOREP_ThreadAcquireLock( SCOREP_PARADIGM_OPENMP, lock->handle, lock->acquisition_order );
        SCOREP_MutexUnlock( scorep_pomp_lock_lock );
        SCOREP_ExitRegion( scorep_pomp_lock_region_handles[ SCOREP_POMP_SET_NEST_LOCK ] );
    }
    else
    {
        omp_set_nest_lock( s );
    }
}

void
POMP2_Unset_nest_lock( omp_nest_lock_t* s )
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OPENMP, "In POMP2_Unset_nest_lock" );
    if ( scorep_pomp_is_tracing_on )
    {
        SCOREP_EnterRegion( scorep_pomp_lock_region_handles[ SCOREP_POMP_UNSET_NEST_LOCK ] );
        SCOREP_MutexLock( scorep_pomp_lock_lock );
        SCOREP_PompLock* lock = SCOREP_Pomp_GetReleaseNestLock( s );
        SCOREP_ThreadReleaseLock( SCOREP_PARADIGM_OPENMP, lock->handle, lock->acquisition_order );
        SCOREP_MutexUnlock( scorep_pomp_lock_lock );
        omp_unset_nest_lock( s );
        SCOREP_ExitRegion( scorep_pomp_lock_region_handles[ SCOREP_POMP_UNSET_NEST_LOCK ] );
    }
    else
    {
        omp_unset_nest_lock( s );
    }
}

int
POMP2_Test_nest_lock( omp_nest_lock_t* s )
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_OPENMP, "In POMP2_Test_nest_lock" );
    if ( scorep_pomp_is_tracing_on )
    {
        int result;

        SCOREP_EnterRegion( scorep_pomp_lock_region_handles[ SCOREP_POMP_TEST_NEST_LOCK ] );
        result = omp_test_nest_lock( s );
        if ( result )
        {
            SCOREP_MutexLock( scorep_pomp_lock_lock );
            SCOREP_PompLock* lock = SCOREP_Pomp_GetAcquireNestLock( s );
            SCOREP_ThreadAcquireLock( SCOREP_PARADIGM_OPENMP, lock->handle, lock->acquisition_order );
            SCOREP_MutexUnlock( scorep_pomp_lock_lock );
        }

        SCOREP_ExitRegion( scorep_pomp_lock_region_handles[ SCOREP_POMP_TEST_NEST_LOCK ] );
        return result;
    }
    else
    {
        return omp_test_nest_lock( s );
    }
}

#endif // _OPENMP

/** @} */
