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
 * @file       SCOREP_Pomp_Lib.c
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 * @status     alpha
 * @ingroup    POMP2
 *
 * @brief Implementation of the POMP2 OpenMP adapter functions.
 */


#include <config.h>
#include <SCOREP_Events.h>
#include <SCOREP_RuntimeManagement.h>
#include <SCOREP_Pomp_Init.h>
#include <opari2/pomp2_lib.h>
#include "SCOREP_Pomp_RegionInfo.h"
#include <scorep_utility/SCOREP_Utils.h>
#include "SCOREP_Pomp_Variables.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SCOREP_Pomp_Lock.h"

/** Id of the currently executing task*/
POMP2_Task_handle pomp_current_task = 0;
#pragma omp threadprivate(pomp_current_task)

/** Counter of tasks used to determine task ids for newly created tasks*/
POMP2_Task_handle pomp_task_counter = 1;
#pragma omp threadprivate(pomp_task_counter)

POMP2_Task_handle
POMP2_Get_new_task_handle()
{
    return ( ( POMP2_Task_handle )omp_get_thread_num() << 32 ) + pomp_task_counter++;
}

void
POMP2_Set_current_task( POMP2_Task_handle task )
{
    pomp_current_task = task;
}

POMP2_Task_handle
POMP2_Get_current_task()
{
    return pomp_current_task;
}

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
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_OPENMP, "In POMP2_Atomic_enter" );
    SCOREP_POMP2_ENSURE_INITIALIZED;
    if ( scorep_pomp_is_tracing_on )
    {
        SCOREP_Pomp_Region* region = *( SCOREP_Pomp_Region** )pomp_handle;
        SCOREP_EnterRegion( region->outerBlock );
    }
}

void
POMP2_Atomic_exit( POMP2_Region_handle* pomp_handle )
{
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_OPENMP, "In POMP2_Atomic_exit" );
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
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_OPENMP, "In POMP2_Barrier_enter" );
    SCOREP_POMP2_ENSURE_INITIALIZED;

    *pomp_old_task = pomp_current_task;
    if ( scorep_pomp_is_tracing_on )
    {
        SCOREP_Pomp_Region* region = *( SCOREP_Pomp_Region** )pomp_handle;
        SCOREP_EnterRegion( region->outerBlock );
    }
}

void
POMP2_Barrier_exit( POMP2_Region_handle* pomp_handle,
                    POMP2_Task_handle    pomp_old_task )
{
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_OPENMP, "In POMP2_Barrier_exit" );

    pomp_current_task = pomp_old_task;
    if ( scorep_pomp_is_tracing_on )
    {
        SCOREP_Pomp_Region* region = *( SCOREP_Pomp_Region** )pomp_handle;
        SCOREP_ExitRegion( region->outerBlock );
    }
}

void
POMP2_Implicit_barrier_enter( POMP2_Region_handle* pomp_handle,
                              POMP2_Task_handle*   pomp_old_task )
{
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_OPENMP, "In POMP2_Implicit_barrier_enter" );
    SCOREP_POMP2_ENSURE_INITIALIZED;

    // For now identical to POMP2_Barrier_enter
    *pomp_old_task = pomp_current_task;
    if ( scorep_pomp_is_tracing_on )
    {
        SCOREP_EnterRegion( scorep_pomp_implicit_barrier_region );
    }
}

void
POMP2_Implicit_barrier_exit( POMP2_Region_handle* pomp_handle,
                             POMP2_Task_handle    pomp_old_task )
{
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_OPENMP, "In POMP2_Implicit_barrier_exit" );

    // For now identical to POMP2_Barrier_exit
    pomp_current_task = pomp_old_task;
    if ( scorep_pomp_is_tracing_on )
    {
        SCOREP_ExitRegion( scorep_pomp_implicit_barrier_region );
    }
}

void
POMP2_Flush_enter( POMP2_Region_handle* pomp_handle,
                   const char           ctc_string[] )
{
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_OPENMP, "In POMP2_Flush_enter" );
    SCOREP_POMP2_ENSURE_INITIALIZED;
    if ( scorep_pomp_is_tracing_on )
    {
        SCOREP_Pomp_Region* region = *( SCOREP_Pomp_Region** )pomp_handle;
        SCOREP_EnterRegion( region->outerBlock );
    }
}

void
POMP2_Flush_exit( POMP2_Region_handle* pomp_handle )
{
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_OPENMP, "In POMP2_Flush_exit" );
    if ( scorep_pomp_is_tracing_on )
    {
        SCOREP_Pomp_Region* region = *( SCOREP_Pomp_Region** )pomp_handle;
        SCOREP_ExitRegion( region->outerBlock );
    }
}

void
POMP2_Critical_begin( POMP2_Region_handle* pomp_handle )
{
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_OPENMP, "In POMP2_Critical_begin" );
    SCOREP_POMP2_ENSURE_INITIALIZED;
    if ( scorep_pomp_is_tracing_on )
    {
        SCOREP_Pomp_Region* region = *( SCOREP_Pomp_Region** )pomp_handle;
        SCOREP_OmpAcquireLock( region->lock->handle, region->lock->acquisition_order );
        SCOREP_EnterRegion( region->innerBlock );
    }
}

void
POMP2_Critical_end( POMP2_Region_handle* pomp_handle )
{
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_OPENMP, "In POMP2_Critical_end" );
    if ( scorep_pomp_is_tracing_on )
    {
        SCOREP_Pomp_Region* region = *( SCOREP_Pomp_Region** )pomp_handle;
        SCOREP_ExitRegion( region->innerBlock );
        SCOREP_OmpReleaseLock( region->lock->handle, region->lock->acquisition_order );
    }
}

void
POMP2_Critical_enter( POMP2_Region_handle* pomp_handle,
                      const char           ctc_string[]  )
{
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_OPENMP, "In POMP2_Critical_enter" );
    SCOREP_POMP2_ENSURE_INITIALIZED;
    if ( scorep_pomp_is_tracing_on )
    {
        SCOREP_Pomp_Region* region = *( SCOREP_Pomp_Region** )pomp_handle;
        SCOREP_EnterRegion( region->outerBlock );
    }
}

void
POMP2_Critical_exit( POMP2_Region_handle* pomp_handle )
{
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_OPENMP, "In POMP2_Critical_exit" );
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
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_OPENMP, "In POMP2_For_enter" );
    SCOREP_POMP2_ENSURE_INITIALIZED;
    if ( scorep_pomp_is_tracing_on )
    {
        SCOREP_Pomp_Region* region = *( SCOREP_Pomp_Region** )pomp_handle;
        SCOREP_EnterRegion( region->outerBlock );
    }
}

void
POMP2_For_exit( POMP2_Region_handle* pomp_handle )
{
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_OPENMP, "In POMP2_For_exit" );
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
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_OPENMP, "In POMP2_Master_begin" );
    SCOREP_POMP2_ENSURE_INITIALIZED;
    if ( scorep_pomp_is_tracing_on )
    {
        SCOREP_Pomp_Region* region = *( SCOREP_Pomp_Region** )pomp_handle;
        SCOREP_EnterRegion( region->innerBlock );
    }
}

void
POMP2_Master_end( POMP2_Region_handle* pomp_handle )
{
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_OPENMP, "In POMP2_Master_end" );
    if ( scorep_pomp_is_tracing_on )
    {
        SCOREP_Pomp_Region* region = *( SCOREP_Pomp_Region** )pomp_handle;
        SCOREP_ExitRegion( region->innerBlock );
    }
}

void
POMP2_Parallel_begin( POMP2_Region_handle* pomp_handle )
{
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_OPENMP, "In POMP2_Parallel_begin" );
    SCOREP_POMP2_ENSURE_INITIALIZED;
    // always use task ID 0 for implicit tasks created at the begin
    // of a parallel region
    pomp_current_task = 0;

    if ( !scorep_pomp_is_finalized )
    {
        SCOREP_Pomp_Region* region = *( SCOREP_Pomp_Region** )pomp_handle;
        SCOREP_EnterRegion( region->innerParallel );
    }
}

void
POMP2_Parallel_end( POMP2_Region_handle* pomp_handle )
{
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_OPENMP, "In POMP2_Parallel_end" );
    if ( !scorep_pomp_is_finalized )
    {
        SCOREP_Pomp_Region* region = *( SCOREP_Pomp_Region** )pomp_handle;
        SCOREP_ExitRegion( region->innerParallel );
    }
}

void
POMP2_Parallel_fork( POMP2_Region_handle* pomp_handle,
                     int                  if_clause,
                     int                  num_threads,
                     POMP2_Task_handle*   pomp_old_task,
                     const char           ctc_string[] )
{
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_OPENMP, "In POMP2_Parallel_fork" );
    SCOREP_POMP2_ENSURE_INITIALIZED;

    *pomp_old_task = pomp_current_task;
    /* Generate fork event */
    if ( !scorep_pomp_is_finalized )
    {
        SCOREP_Pomp_Region* region = *( SCOREP_Pomp_Region** )pomp_handle;
        SCOREP_ASSERT( region != NULL );
        SCOREP_OmpFork( num_threads );
    }
}

void
POMP2_Parallel_join( POMP2_Region_handle* pomp_handle,
                     POMP2_Task_handle    pomp_old_task )
{
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_OPENMP, "In POMP2_Parallel_join" );

    pomp_current_task = pomp_old_task;
    if ( !scorep_pomp_is_finalized )
    {
        SCOREP_Pomp_Region* region = *( SCOREP_Pomp_Region** )pomp_handle;
        SCOREP_OmpJoin();
    }
}

void
POMP2_Section_begin( POMP2_Region_handle* pomp_handle,
                     const char           ctc_string[]  )
{
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_OPENMP, "In POMP2_Section_begin" );
    SCOREP_POMP2_ENSURE_INITIALIZED;
    if ( scorep_pomp_is_tracing_on )
    {
        SCOREP_Pomp_Region* region = *( SCOREP_Pomp_Region** )pomp_handle;
        SCOREP_EnterRegion( region->innerBlock );
    }
}

void
POMP2_Section_end( POMP2_Region_handle* pomp_handle )
{
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_OPENMP, "In POMP2_Section_end" );
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
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_OPENMP, "In POMP2_Sections_enter" );
    SCOREP_POMP2_ENSURE_INITIALIZED;
    if ( scorep_pomp_is_tracing_on )
    {
        SCOREP_Pomp_Region* region = *( SCOREP_Pomp_Region** )pomp_handle;
        SCOREP_EnterRegion( region->outerBlock );
    }
}

void
POMP2_Sections_exit( POMP2_Region_handle* pomp_handle )
{
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_OPENMP, "In POMP2_Sections_exit" );
    if ( scorep_pomp_is_tracing_on )
    {
        SCOREP_Pomp_Region* region = *( SCOREP_Pomp_Region** )pomp_handle;
        SCOREP_ExitRegion( region->outerBlock );
    }
}

void
POMP2_Single_begin( POMP2_Region_handle* pomp_handle )
{
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_OPENMP, "In POMP2_Single_begin" );
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
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_OPENMP, "In POMP2_Single_end" );
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
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_OPENMP, "In POMP2_Single_enter" );
    SCOREP_POMP2_ENSURE_INITIALIZED;
    if ( scorep_pomp_is_tracing_on )
    {
        SCOREP_Pomp_Region* region = *( SCOREP_Pomp_Region** )pomp_handle;
        SCOREP_EnterRegion( region->outerBlock );
    }
}

void
POMP2_Single_exit( POMP2_Region_handle* pomp_handle )
{
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_OPENMP, "In POMP2_Single_exit" );
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
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_OPENMP, "In POMP2_Ordered_begin" );
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
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_OPENMP, "In POMP2_Ordered_end" );
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
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_OPENMP, "In POMP2_Ordered_enter" );
    SCOREP_POMP2_ENSURE_INITIALIZED;
    if ( scorep_pomp_is_tracing_on )
    {
        SCOREP_Pomp_Region* region = *( SCOREP_Pomp_Region** )pomp_handle;
        SCOREP_EnterRegion( region->outerBlock );
    }
}

void
POMP2_Ordered_exit( POMP2_Region_handle* pomp_handle )
{
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_OPENMP, "In POMP2_Ordered_exit" );
    if ( scorep_pomp_is_tracing_on )
    {
        SCOREP_Pomp_Region* region = *( SCOREP_Pomp_Region** )pomp_handle;
        SCOREP_ExitRegion( region->innerBlock );
    }
}


void
POMP2_Task_create_begin( POMP2_Region_handle* pomp_handle,
                         POMP2_Task_handle*   pomp_new_task,
                         POMP2_Task_handle*   pomp_old_task,
                         int                  pomp_if,
                         const char           ctc_string[] )
{
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_OPENMP, "In POMP2_Task_create_begin" );
    SCOREP_POMP2_ENSURE_INITIALIZED;

    /* We must use pomp_old_task to reset the old task id after the creation.
       We cannot store the new task id in pomp_current_task, because other tasks
       maybe executed before. */
    *pomp_old_task = pomp_current_task;
    *pomp_new_task = POMP2_Get_new_task_handle();
    if ( scorep_pomp_is_tracing_on )
    {
        SCOREP_Pomp_Region* region = *( SCOREP_Pomp_Region** )pomp_handle;
        /* TODO: pomp_current_task is not needed here. We know where we are */
        SCOREP_OmpTaskCreateBegin( region->outerBlock, pomp_current_task );
    }
}

void
POMP2_Task_create_end( POMP2_Region_handle* pomp_handle,
                       POMP2_Task_handle    pomp_old_task )
{
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_OPENMP, "In POMP2_Task_create_end" );

    pomp_current_task = pomp_old_task;

    if ( scorep_pomp_is_tracing_on )
    {
        SCOREP_Pomp_Region* region = *( SCOREP_Pomp_Region** )pomp_handle;
        SCOREP_OmpTaskCreateEnd( region->outerBlock, pomp_old_task );
    }
}

void
POMP2_Task_begin( POMP2_Region_handle* pomp_handle,
                  POMP2_Task_handle    pomp_task )
{
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_OPENMP, "In POMP2_Task_begin" );
    SCOREP_POMP2_ENSURE_INITIALIZED;

    pomp_current_task = pomp_task;

    if ( scorep_pomp_is_tracing_on )
    {
        SCOREP_Pomp_Region* region = *( SCOREP_Pomp_Region** )pomp_handle;
        /* TODO: Pass also pomp_parent_task to the event to allow reconstruction of
                 the parent-child relationship. */
        SCOREP_OmpTaskBegin( region->innerBlock, pomp_current_task );
    }
}

void
POMP2_Task_end( POMP2_Region_handle* pomp_handle )
{
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_OPENMP, "In POMP2_Task_end" );
    if ( scorep_pomp_is_tracing_on )
    {
        SCOREP_Pomp_Region* region = *( SCOREP_Pomp_Region** )pomp_handle;
        SCOREP_OmpTaskCompleted( region->innerBlock );
    }
}

void
POMP2_Untied_task_create_begin( POMP2_Region_handle* pomp_handle,
                                POMP2_Task_handle*   pomp_new_task,
                                POMP2_Task_handle*   pomp_old_task,
                                int                  pomp_if,
                                const char           ctc_string[] )
{
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_OPENMP, "In POMP2_Untied_task_create_begin" );
    SCOREP_POMP2_ENSURE_INITIALIZED;
    *pomp_new_task = POMP2_Get_new_task_handle();
    *pomp_old_task = pomp_current_task;

    if ( scorep_pomp_is_tracing_on )
    {
        SCOREP_Pomp_Region* region = *( SCOREP_Pomp_Region** )pomp_handle;
        /* TODO: pomp_current_task is not needed here. We know where we are */
        SCOREP_OmpTaskCreateBegin( region->outerBlock, pomp_current_task );
    }
}

void
POMP2_Untied_task_create_end( POMP2_Region_handle* pomp_handle,
                              POMP2_Task_handle    pomp_old_task )
{
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_OPENMP, "In POMP2_Untied_task_create_end" );

    pomp_current_task = pomp_old_task;
    if ( scorep_pomp_is_tracing_on )
    {
        SCOREP_Pomp_Region* region = *( SCOREP_Pomp_Region** )pomp_handle;
        SCOREP_OmpTaskCreateEnd( region->outerBlock, pomp_current_task );
    }
}

void
POMP2_Untied_task_begin( POMP2_Region_handle* pomp_handle,
                         POMP2_Task_handle    pomp_task )
{
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_OPENMP, "In POMP2_Untied_task_begin" );
    SCOREP_POMP2_ENSURE_INITIALIZED;

    pomp_current_task = pomp_task;

    if ( scorep_pomp_is_tracing_on )
    {
        SCOREP_Pomp_Region* region = *( SCOREP_Pomp_Region** )pomp_handle;
        /* TODO: Pass also pomp_parent_task to the event to allow reconstruction of
                 the parent-child relationship. */
        SCOREP_OmpTaskBegin( region->innerBlock, pomp_current_task );
    }
}

void
POMP2_Untied_task_end( POMP2_Region_handle* pomp_handle )
{
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_OPENMP, "In POMP2_Untied_task_end" );
    if ( scorep_pomp_is_tracing_on )
    {
        SCOREP_Pomp_Region* region = *( SCOREP_Pomp_Region** )pomp_handle;
        SCOREP_OmpTaskCompleted( region->innerBlock );
    }
}

void
POMP2_Taskwait_begin( POMP2_Region_handle* pomp_handle,
                      POMP2_Task_handle*   pomp_old_task,
                      const char           ctc_string[] )
{
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_OPENMP, "In POMP2_Taskwait_begin" );
    SCOREP_POMP2_ENSURE_INITIALIZED;

    *pomp_old_task = pomp_current_task;
    if ( scorep_pomp_is_tracing_on )
    {
        SCOREP_Pomp_Region* region = *( SCOREP_Pomp_Region** )pomp_handle;
        SCOREP_OmpTaskSuspend( region->innerBlock );
        SCOREP_EnterRegion( region->outerBlock );
    }
}

void
POMP2_Taskwait_end( POMP2_Region_handle* pomp_handle,
                    POMP2_Task_handle    pomp_old_task )
{
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_OPENMP, "In POMP2_Taskwait_end" );

    pomp_current_task = pomp_old_task;
    if ( scorep_pomp_is_tracing_on )
    {
        SCOREP_Pomp_Region* region = *( SCOREP_Pomp_Region** )pomp_handle;
        SCOREP_OmpTaskResume( region->innerBlock, pomp_current_task );
        SCOREP_ExitRegion( region->outerBlock );
    }
}

//=======================================================================================
//=======================================================================================
//=======================================================================================


void
POMP2_Workshare_enter( POMP2_Region_handle* pomp_handle,
                       const char           ctc_string[]  )
{
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_OPENMP, "In POMP2_Workshare_enter" );
    SCOREP_POMP2_ENSURE_INITIALIZED;
    if ( scorep_pomp_is_tracing_on )
    {
        SCOREP_Pomp_Region* region = *( SCOREP_Pomp_Region** )pomp_handle;
        SCOREP_EnterRegion( region->outerBlock );
    }
}

void
POMP2_Workshare_exit( POMP2_Region_handle* pomp_handle )
{
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_OPENMP, "In POMP2_Workshare_exit" );
    if ( scorep_pomp_is_tracing_on )
    {
        SCOREP_Pomp_Region* region = *( SCOREP_Pomp_Region** )pomp_handle;
        SCOREP_ExitRegion( region->outerBlock );
    }
}

int
POMP2_Lib_get_max_threads()
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
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_OPENMP, "In POMP2_Init_lock" );
    if ( scorep_pomp_is_finalized )
    {
        omp_init_lock( s );
        return;
    }
    SCOREP_POMP2_ENSURE_INITIALIZED;
    if ( scorep_pomp_is_tracing_on )
    {
        SCOREP_EnterRegion( scorep_pomp_regid[ SCOREP_POMP_INIT_LOCK ] );
        omp_init_lock( s );
        scorep_pomp_lock_init( s );
        SCOREP_ExitRegion( scorep_pomp_regid[ SCOREP_POMP_INIT_LOCK ] );
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
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_OPENMP, "In POMP2_Destroy_lock" );
    if ( scorep_pomp_is_finalized )
    {
        omp_destroy_lock( s );
    }
    else if ( scorep_pomp_is_tracing_on )
    {
        SCOREP_EnterRegion( scorep_pomp_regid[ SCOREP_POMP_DESTROY_LOCK ] );
        omp_destroy_lock( s );
        scorep_pomp_lock_destroy( s );
        SCOREP_ExitRegion( scorep_pomp_regid[ SCOREP_POMP_DESTROY_LOCK ] );
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
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_OPENMP, "In POMP2_Set_lock" );
    if ( scorep_pomp_is_tracing_on )
    {
        SCOREP_EnterRegion( scorep_pomp_regid[ SCOREP_POMP_SET_LOCK ] );
        omp_set_lock( s );
        SCOREP_PompLock* lock = SCOREP_Pomp_GetAcquireLock( s );
        SCOREP_OmpAcquireLock( lock->handle, lock->acquisition_order );
        SCOREP_ExitRegion( scorep_pomp_regid[ SCOREP_POMP_SET_LOCK ] );
    }
    else
    {
        omp_set_lock( s );
    }
}

void
POMP2_Unset_lock( omp_lock_t* s )
{
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_OPENMP, "In POMP2_Unset_lock" );
    if ( scorep_pomp_is_tracing_on )
    {
        SCOREP_EnterRegion( scorep_pomp_regid[ SCOREP_POMP_UNSET_LOCK ] );
        SCOREP_PompLock* lock = SCOREP_Pomp_GetReleaseLock( s );
        SCOREP_OmpReleaseLock( lock->handle, lock->acquisition_order );
        omp_unset_lock( s );
        SCOREP_ExitRegion( scorep_pomp_regid[ SCOREP_POMP_UNSET_LOCK ] );
    }
    else
    {
        omp_unset_lock( s );
    }
}

int
POMP2_Test_lock( omp_lock_t* s )
{
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_OPENMP, "In POMP2_Test_lock" );
    if ( scorep_pomp_is_tracing_on )
    {
        int result;

        SCOREP_EnterRegion( scorep_pomp_regid[ SCOREP_POMP_TEST_LOCK ] );
        result = omp_test_lock( s );
        if ( result )
        {
            SCOREP_PompLock* lock = SCOREP_Pomp_GetAcquireLock( s );
            SCOREP_OmpAcquireLock( lock->handle, lock->acquisition_order );
        }
        SCOREP_ExitRegion( scorep_pomp_regid[ SCOREP_POMP_TEST_LOCK ] );
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
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_OPENMP, "In POMP2_Init_nest_lock" );
    if ( scorep_pomp_is_finalized )
    {
        omp_init_nest_lock( s );
        return;
    }
    SCOREP_POMP2_ENSURE_INITIALIZED;
    if ( scorep_pomp_is_tracing_on )
    {
        SCOREP_EnterRegion( scorep_pomp_regid[ SCOREP_POMP_INIT_NEST_LOCK ] );
        omp_init_nest_lock( s );
        scorep_pomp_lock_init( s );
        SCOREP_ExitRegion( scorep_pomp_regid[ SCOREP_POMP_INIT_NEST_LOCK ] );
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
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_OPENMP, "In POMP2_Destroy_nest_lock" );
    if ( scorep_pomp_is_finalized )
    {
        omp_destroy_nest_lock( s );
    }
    else if ( scorep_pomp_is_tracing_on )
    {
        SCOREP_EnterRegion( scorep_pomp_regid[ SCOREP_POMP_DESTROY_NEST_LOCK ] );
        omp_destroy_nest_lock( s );
        scorep_pomp_lock_destroy( s );
        SCOREP_ExitRegion( scorep_pomp_regid[ SCOREP_POMP_DESTROY_NEST_LOCK ] );
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
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_OPENMP, "In POMP2_Set_nest_lock" );
    if ( scorep_pomp_is_tracing_on )
    {
        SCOREP_EnterRegion( scorep_pomp_regid[ SCOREP_POMP_SET_NEST_LOCK ] );
        omp_set_nest_lock( s );
        SCOREP_PompLock* lock = SCOREP_Pomp_GetAcquireNestLock( s );
        SCOREP_OmpAcquireLock( lock->handle, lock->acquisition_order );
        SCOREP_ExitRegion( scorep_pomp_regid[ SCOREP_POMP_SET_NEST_LOCK ] );
    }
    else
    {
        omp_set_nest_lock( s );
    }
}

void
POMP2_Unset_nest_lock( omp_nest_lock_t* s )
{
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_OPENMP, "In POMP2_Unset_nest_lock" );
    if ( scorep_pomp_is_tracing_on )
    {
        SCOREP_EnterRegion( scorep_pomp_regid[ SCOREP_POMP_UNSET_NEST_LOCK ] );
        SCOREP_PompLock* lock = SCOREP_Pomp_GetReleaseNestLock( s );
        SCOREP_OmpReleaseLock( lock->handle, lock->acquisition_order );
        omp_unset_nest_lock( s );
        SCOREP_ExitRegion( scorep_pomp_regid[ SCOREP_POMP_UNSET_NEST_LOCK ] );
    }
    else
    {
        omp_unset_nest_lock( s );
    }
}

int
POMP2_Test_nest_lock( omp_nest_lock_t* s )
{
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_OPENMP, "In POMP2_Test_nest_lock" );
    if ( scorep_pomp_is_tracing_on )
    {
        int result;

        SCOREP_EnterRegion( scorep_pomp_regid[ SCOREP_POMP_TEST_NEST_LOCK ] );
        result = omp_test_nest_lock( s );
        if ( result )
        {
            SCOREP_PompLock* lock = SCOREP_Pomp_GetAcquireNestLock( s );
            SCOREP_OmpAcquireLock( lock->handle, lock->acquisition_order );
        }

        SCOREP_ExitRegion( scorep_pomp_regid[ SCOREP_POMP_TEST_NEST_LOCK ] );
        return result;
    }
    else
    {
        return omp_test_nest_lock( s );
    }
}

#endif // _OPENMP

/** @} */
