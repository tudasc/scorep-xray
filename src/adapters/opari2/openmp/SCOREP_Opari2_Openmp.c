/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2011,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2011, 2014-2015, 2017,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2011,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2018,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2011, 2014,
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
 *
 * @ingroup OPARI2
 *
 * @brief Implementation of the OPARI2 OpenMP adapter functions.
 */

#include <config.h>

#include <SCOREP_Opari2_Region_Info.h>
#include <SCOREP_RuntimeManagement.h>
#include <SCOREP_InMeasurement.h>
#include <SCOREP_Events.h>
#include <SCOREP_ThreadForkJoin_Event.h>
#include <SCOREP_Task.h>

#define SCOREP_DEBUG_MODULE_NAME OPARI2
#include <UTILS_Error.h>
#include <UTILS_Debug.h>

#include <opari2/pomp2_lib.h>

#include "SCOREP_Opari2_Openmp_Tpd.h"
#include "SCOREP_Opari2_Openmp_Regions.h"
#include "SCOREP_Opari2_Openmp_Lock.h"

#ifdef __FUJITSU

/* Id of the currently executing task. */
__thread POMP2_Task_handle pomp_current_task = 0;

/* Counter of tasks used to determine task ids for newly created tasks .*/
__thread POMP2_Task_handle pomp_task_counter = 1;

#else /* !__FUJITSU */

/** Id of the currently executing task. */
POMP2_Task_handle pomp_current_task = 0;
#pragma omp threadprivate(pomp_current_task)

/** Counter of tasks used to determine task ids for newly created tasks. */
POMP2_Task_handle pomp_task_counter = 1;
#pragma omp threadprivate(pomp_task_counter)

#endif /* !__FUJITSU */


/*
 * NOTE: The POMP2_Task_handle changes its value and
 * type when the task execution starts. Between task creation and execution
 * POMP2_Task_handle contains a 64 bit unsigned integer value consisting of the
 * thread id and the generation number.
 * When execution starts, a data structure is allocated for the task and
 * the content of POMP2_Task_handle is replaced by a pointer to this structure.
 * Thus, Task events that happen before the start of the task execution, e.g.,
 * SCOREP_ThreadForkJoin_CreateTask(), access POMP2_Task_handle as a number.
 * Events that happen after SCOREP_ThreadForkJoin_TaskBegin() access
 * POMP2_Task_handle as a pointer to a data structure.
 */
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

#define pomp2_to_scorep_handle( task ) \
    ( ( SCOREP_TaskHandle )task )

#define scorep_to_pomp2_handle( task ) \
    ( ( POMP2_Task_handle )task )

/** @ingroup POMP2
    @{
 */

/**
   @def SCOREP_OPARI2_OMP_ENSURE_INITIALIZED
   Checks whether opari2-OpenMP adapter is initialized and if not initializes
   the measurement system.
 */
#define SCOREP_OPARI2_OMP_ENSURE_INITIALIZED()  \
    SCOREP_IN_MEASUREMENT_INCREMENT();          \
    if ( SCOREP_IS_MEASUREMENT_PHASE( PRE ) )   \
    {                                           \
        SCOREP_InitMeasurement();               \
    }

/* **************************************************************************************
 *                                                                   POMP event functions
 ***************************************************************************************/

void
POMP2_Atomic_enter( POMP2_Region_handle* pomp_handle,
                    const char           ctc_string[] )
{
    SCOREP_OPARI2_OMP_ENSURE_INITIALIZED();

    UTILS_DEBUG_ENTRY();

    if ( SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) && scorep_opari2_recording_on )
    {
        SCOREP_OPARI2_OPENMP_HANDLE_UNINITIALIZED_REGION( pomp_handle, ctc_string );
        SCOREP_Opari2_Openmp_Region* region = *( SCOREP_Opari2_Openmp_Region** )pomp_handle;
        SCOREP_EnterRegion( region->outerBlock );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}

void
POMP2_Atomic_exit( POMP2_Region_handle* pomp_handle )
{
    SCOREP_OPARI2_OMP_ENSURE_INITIALIZED();

    UTILS_DEBUG_ENTRY();

    if ( SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) && scorep_opari2_recording_on )
    {
        SCOREP_Opari2_Openmp_Region* region = *( SCOREP_Opari2_Openmp_Region** )pomp_handle;
        SCOREP_ExitRegion( region->outerBlock );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}

void
POMP2_Barrier_enter( POMP2_Region_handle* pomp_handle,
                     POMP2_Task_handle*   pomp_old_task,
                     const char           ctc_string[] )
{
    SCOREP_OPARI2_OMP_ENSURE_INITIALIZED();

    UTILS_DEBUG_ENTRY();

    *pomp_old_task = pomp_current_task;
    if ( SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) && scorep_opari2_recording_on )
    {
        SCOREP_OPARI2_OPENMP_HANDLE_UNINITIALIZED_REGION( pomp_handle, ctc_string );
        SCOREP_Opari2_Openmp_Region* region = *( SCOREP_Opari2_Openmp_Region** )pomp_handle;
        SCOREP_EnterRegion( region->outerBlock );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}

void
POMP2_Barrier_exit( POMP2_Region_handle* pomp_handle,
                    POMP2_Task_handle    pomp_old_task )
{
    SCOREP_OPARI2_OMP_ENSURE_INITIALIZED();

    UTILS_DEBUG_ENTRY();

    if ( SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) && scorep_opari2_recording_on )
    {
        SCOREP_Opari2_Openmp_Region* region = *( SCOREP_Opari2_Openmp_Region** )pomp_handle;
        if ( pomp_current_task != pomp_old_task )
        {
            SCOREP_ThreadForkJoin_TaskSwitch( SCOREP_PARADIGM_OPENMP,
                                              pomp2_to_scorep_handle( pomp_old_task ) );
        }
        SCOREP_ExitRegion( region->outerBlock );
    }
    pomp_current_task = pomp_old_task;

    SCOREP_IN_MEASUREMENT_DECREMENT();
}

void
POMP2_Implicit_barrier_enter( POMP2_Region_handle* pomp_handle,
                              POMP2_Task_handle*   pomp_old_task )
{
    SCOREP_OPARI2_OMP_ENSURE_INITIALIZED();

    UTILS_DEBUG_ENTRY();

    *pomp_old_task = pomp_current_task;

    if ( SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) && scorep_opari2_recording_on )
    {
        SCOREP_Opari2_Openmp_Region* region = *( SCOREP_Opari2_Openmp_Region** )pomp_handle;
        SCOREP_EnterRegion( region->barrier );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}

void
POMP2_Implicit_barrier_exit( POMP2_Region_handle* pomp_handle,
                             POMP2_Task_handle    pomp_old_task )
{
    SCOREP_OPARI2_OMP_ENSURE_INITIALIZED();

    UTILS_DEBUG_ENTRY();

    if ( SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) && scorep_opari2_recording_on )
    {
        SCOREP_Opari2_Openmp_Region* region = *( SCOREP_Opari2_Openmp_Region** )pomp_handle;
        if ( pomp_current_task != pomp_old_task )
        {
            SCOREP_ThreadForkJoin_TaskSwitch( SCOREP_PARADIGM_OPENMP,
                                              pomp2_to_scorep_handle( pomp_old_task ) );
        }
        SCOREP_ExitRegion( region->barrier );
    }
    pomp_current_task = pomp_old_task;

    SCOREP_IN_MEASUREMENT_DECREMENT();
}

void
POMP2_Flush_enter( POMP2_Region_handle* pomp_handle,
                   const char           ctc_string[] )
{
    SCOREP_OPARI2_OMP_ENSURE_INITIALIZED();

    UTILS_DEBUG_ENTRY();

    if ( SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) && scorep_opari2_recording_on )
    {
        SCOREP_OPARI2_OPENMP_HANDLE_UNINITIALIZED_REGION( pomp_handle, ctc_string );
        SCOREP_Opari2_Openmp_Region* region = *( SCOREP_Opari2_Openmp_Region** )pomp_handle;
        SCOREP_EnterRegion( region->outerBlock );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}

void
POMP2_Flush_exit( POMP2_Region_handle* pomp_handle )
{
    SCOREP_OPARI2_OMP_ENSURE_INITIALIZED();

    UTILS_DEBUG_ENTRY();

    if ( SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) && scorep_opari2_recording_on )
    {
        SCOREP_Opari2_Openmp_Region* region = *( SCOREP_Opari2_Openmp_Region** )pomp_handle;
        SCOREP_ExitRegion( region->outerBlock );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}

void
POMP2_Critical_begin( POMP2_Region_handle* pomp_handle )
{
    SCOREP_OPARI2_OMP_ENSURE_INITIALIZED();

    UTILS_DEBUG_ENTRY();

    if ( SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) && scorep_opari2_recording_on )
    {
        SCOREP_Opari2_Openmp_Region* region = *( SCOREP_Opari2_Openmp_Region** )pomp_handle;
        region->lock->acquisition_order++;
        SCOREP_ThreadAcquireLock( SCOREP_PARADIGM_OPENMP,
                                  region->lock->handle,
                                  region->lock->acquisition_order );
        SCOREP_EnterRegion( region->innerBlock );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}

void
POMP2_Critical_end( POMP2_Region_handle* pomp_handle )
{
    SCOREP_OPARI2_OMP_ENSURE_INITIALIZED();

    UTILS_DEBUG_ENTRY();

    if ( SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) && scorep_opari2_recording_on )
    {
        SCOREP_Opari2_Openmp_Region* region = *( SCOREP_Opari2_Openmp_Region** )pomp_handle;
        SCOREP_ExitRegion( region->innerBlock );
        SCOREP_ThreadReleaseLock( SCOREP_PARADIGM_OPENMP,
                                  region->lock->handle,
                                  region->lock->acquisition_order );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}

void
POMP2_Critical_enter( POMP2_Region_handle* pomp_handle,
                      const char           ctc_string[] )
{
    SCOREP_OPARI2_OMP_ENSURE_INITIALIZED();

    UTILS_DEBUG_ENTRY();

    if ( SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) && scorep_opari2_recording_on )
    {
        SCOREP_OPARI2_OPENMP_HANDLE_UNINITIALIZED_REGION( pomp_handle, ctc_string );
        SCOREP_Opari2_Openmp_Region* region = *( SCOREP_Opari2_Openmp_Region** )pomp_handle;
        SCOREP_EnterRegion( region->outerBlock );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}

void
POMP2_Critical_exit( POMP2_Region_handle* pomp_handle )
{
    SCOREP_OPARI2_OMP_ENSURE_INITIALIZED();

    UTILS_DEBUG_ENTRY();

    if ( SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) && scorep_opari2_recording_on )
    {
        SCOREP_Opari2_Openmp_Region* region = *( SCOREP_Opari2_Openmp_Region** )pomp_handle;
        SCOREP_ExitRegion( region->outerBlock );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}

void
POMP2_For_enter( POMP2_Region_handle* pomp_handle,
                 const char           ctc_string[] )
{
    SCOREP_OPARI2_OMP_ENSURE_INITIALIZED();

    UTILS_DEBUG_ENTRY();

    if ( SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) && scorep_opari2_recording_on )
    {
        SCOREP_OPARI2_OPENMP_HANDLE_UNINITIALIZED_REGION( pomp_handle, ctc_string );
        SCOREP_Opari2_Openmp_Region* region = *( SCOREP_Opari2_Openmp_Region** )pomp_handle;
        SCOREP_EnterRegion( region->outerBlock );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}

void
POMP2_For_exit( POMP2_Region_handle* pomp_handle )
{
    SCOREP_OPARI2_OMP_ENSURE_INITIALIZED();

    UTILS_DEBUG_ENTRY();

    if ( SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) && scorep_opari2_recording_on )
    {
        SCOREP_Opari2_Openmp_Region* region = *( SCOREP_Opari2_Openmp_Region** )pomp_handle;
        SCOREP_ExitRegion( region->outerBlock );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}

void
POMP2_Master_begin( POMP2_Region_handle* pomp_handle,
                    const char           ctc_string[] )
{
    SCOREP_OPARI2_OMP_ENSURE_INITIALIZED();

    UTILS_DEBUG_ENTRY();

    if ( SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) && scorep_opari2_recording_on )
    {
        SCOREP_OPARI2_OPENMP_HANDLE_UNINITIALIZED_REGION( pomp_handle, ctc_string );
        SCOREP_Opari2_Openmp_Region* region = *( SCOREP_Opari2_Openmp_Region** )pomp_handle;
        SCOREP_EnterRegion( region->innerBlock );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}

void
POMP2_Master_end( POMP2_Region_handle* pomp_handle )
{
    SCOREP_OPARI2_OMP_ENSURE_INITIALIZED();

    UTILS_DEBUG_ENTRY();

    if ( SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) && scorep_opari2_recording_on )
    {
        SCOREP_Opari2_Openmp_Region* region = *( SCOREP_Opari2_Openmp_Region** )pomp_handle;
        SCOREP_ExitRegion( region->innerBlock );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}

void
POMP2_Parallel_begin( POMP2_Region_handle* pomp_handle )
{
    SCOREP_OPARI2_OMP_ENSURE_INITIALIZED();

    UTILS_DEBUG_ENTRY();

    // always use task ID 0 for implicit tasks created at the begin
    // of a parallel region
    pomp_current_task = 0;

    if ( SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
#if HAVE( SCOREP_OMP_ANCESTRY )
        /* Collect ancestry info into an array. */
        int nesting_level = omp_get_level();
        /* Inside a parallel region the nesting level is at least 1. */
        UTILS_ASSERT( nesting_level > 0 );
        int ancestor_info[ nesting_level - 1 ];
        for ( int level = 1; level < nesting_level; level++ )
        {
            if ( omp_get_team_size( level ) > 1 )
            {
                ancestor_info[ level - 1 ] = omp_get_ancestor_thread_num( level );
            }
            else
            {
                ancestor_info[ level - 1 ] = -1;
            }
        }
#elif HAVE( SCOREP_OMP_TPD )
        /* Pass pomp_tpd down to be used as parent. */
        uint32_t nesting_level = 0;
        void*    ancestor_info = ( void* )POMP_TPD_MANGLED;
#else
#error "Need either SCOREP_OMP_ANCESTRY or SCOREP_OMP_TPD functionality for OpenMP."
#endif

        struct scorep_thread_private_data* new_tpd = NULL;
        SCOREP_TaskHandle                  new_task;

        int thread_num = omp_get_thread_num();
        UTILS_ASSERT( thread_num >= 0 );
        int num_threads = omp_get_num_threads();
        UTILS_ASSERT( num_threads > 0 );
        SCOREP_ThreadForkJoin_TeamBegin(
            SCOREP_PARADIGM_OPENMP,
            ( uint32_t )thread_num,
            ( uint32_t )num_threads,
            ( uint32_t )nesting_level,
            ancestor_info,
            &new_tpd,
            &new_task );
        pomp_current_task = scorep_to_pomp2_handle( new_task );
        SCOREP_OMP_SET_POMP_TPD_TO( new_tpd );
        SCOREP_Opari2_Openmp_Region* region = *( SCOREP_Opari2_Openmp_Region** )pomp_handle;
        SCOREP_EnterRegion( region->innerParallel );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}

void
POMP2_Parallel_end( POMP2_Region_handle* pomp_handle )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();

    UTILS_DEBUG_ENTRY();

    if ( SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_Opari2_Openmp_Region* region = *( SCOREP_Opari2_Openmp_Region** )pomp_handle;
        SCOREP_ExitRegion( region->innerParallel );
        SCOREP_ThreadForkJoin_TeamEnd( SCOREP_PARADIGM_OPENMP,
                                       omp_get_thread_num(),
                                       omp_get_num_threads() );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}

void
POMP2_Parallel_fork( POMP2_Region_handle* pomp_handle,
                     int                  if_clause,
                     int                  num_threads,
                     POMP2_Task_handle*   pomp_old_task,
                     const char           ctc_string[] )
{
    SCOREP_OPARI2_OMP_ENSURE_INITIALIZED();

    UTILS_DEBUG_ENTRY();

    *pomp_old_task = pomp_current_task;

    /* Generate fork event */
    if ( SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_OPARI2_OPENMP_HANDLE_UNINITIALIZED_REGION( pomp_handle, ctc_string );
        SCOREP_Opari2_Openmp_Region* region = *( SCOREP_Opari2_Openmp_Region** )pomp_handle;
        UTILS_ASSERT( region != NULL );
        SCOREP_ThreadForkJoin_Fork( SCOREP_PARADIGM_OPENMP,
                                    num_threads );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}

void
POMP2_Parallel_join( POMP2_Region_handle* pomp_handle,
                     POMP2_Task_handle    pomp_old_task )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();

    UTILS_DEBUG_ENTRY();

    pomp_current_task = pomp_old_task;
    if ( SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_Opari2_Openmp_Region*       region          = *( SCOREP_Opari2_Openmp_Region** )pomp_handle;
        struct scorep_thread_private_data* tpd_from_now_on = NULL;
        SCOREP_ThreadForkJoin_Join( SCOREP_PARADIGM_OPENMP, &tpd_from_now_on );
        SCOREP_OMP_SET_POMP_TPD_TO( tpd_from_now_on );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}

void
POMP2_Section_begin( POMP2_Region_handle* pomp_handle,
                     const char           ctc_string[] )
{
    SCOREP_OPARI2_OMP_ENSURE_INITIALIZED();

    UTILS_DEBUG_ENTRY();

    if ( SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) && scorep_opari2_recording_on )
    {
        SCOREP_OPARI2_OPENMP_HANDLE_UNINITIALIZED_REGION( pomp_handle, ctc_string );
        SCOREP_Opari2_Openmp_Region* region = *( SCOREP_Opari2_Openmp_Region** )pomp_handle;
        SCOREP_EnterRegion( region->innerBlock );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}

void
POMP2_Section_end( POMP2_Region_handle* pomp_handle )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();

    UTILS_DEBUG_ENTRY();

    if ( SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) && scorep_opari2_recording_on )
    {
        SCOREP_Opari2_Openmp_Region* region = *( SCOREP_Opari2_Openmp_Region** )pomp_handle;
        SCOREP_ExitRegion( region->innerBlock );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}

void
POMP2_Sections_enter( POMP2_Region_handle* pomp_handle,
                      const char           ctc_string[] )
{
    SCOREP_OPARI2_OMP_ENSURE_INITIALIZED();

    UTILS_DEBUG_ENTRY();

    if ( SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) && scorep_opari2_recording_on )
    {
        SCOREP_OPARI2_OPENMP_HANDLE_UNINITIALIZED_REGION( pomp_handle, ctc_string );
        SCOREP_Opari2_Openmp_Region* region = *( SCOREP_Opari2_Openmp_Region** )pomp_handle;
        SCOREP_EnterRegion( region->outerBlock );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}

void
POMP2_Sections_exit( POMP2_Region_handle* pomp_handle )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();

    UTILS_DEBUG_ENTRY();

    if ( SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) && scorep_opari2_recording_on )
    {
        SCOREP_Opari2_Openmp_Region* region = *( SCOREP_Opari2_Openmp_Region** )pomp_handle;
        SCOREP_ExitRegion( region->outerBlock );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}

void
POMP2_Single_begin( POMP2_Region_handle* pomp_handle )
{
    SCOREP_OPARI2_OMP_ENSURE_INITIALIZED();

    UTILS_DEBUG_ENTRY();

    if ( SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) && scorep_opari2_recording_on )
    {
        SCOREP_Opari2_Openmp_Region* region = *( SCOREP_Opari2_Openmp_Region** )pomp_handle;
        SCOREP_EnterRegion( region->innerBlock );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}

void
POMP2_Single_end( POMP2_Region_handle* pomp_handle )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();

    UTILS_DEBUG_ENTRY();

    if ( SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) && scorep_opari2_recording_on )
    {
        SCOREP_Opari2_Openmp_Region* region = *( SCOREP_Opari2_Openmp_Region** )pomp_handle;
        SCOREP_ExitRegion( region->innerBlock );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}

void
POMP2_Single_enter( POMP2_Region_handle* pomp_handle,
                    const char           ctc_string[] )
{
    SCOREP_OPARI2_OMP_ENSURE_INITIALIZED();

    UTILS_DEBUG_ENTRY();

    if ( SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) && scorep_opari2_recording_on )
    {
        SCOREP_OPARI2_OPENMP_HANDLE_UNINITIALIZED_REGION( pomp_handle, ctc_string );
        SCOREP_Opari2_Openmp_Region* region = *( SCOREP_Opari2_Openmp_Region** )pomp_handle;
        SCOREP_EnterRegion( region->outerBlock );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}

void
POMP2_Single_exit( POMP2_Region_handle* pomp_handle )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();

    UTILS_DEBUG_ENTRY();

    if ( SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) && scorep_opari2_recording_on )
    {
        SCOREP_Opari2_Openmp_Region* region = *( SCOREP_Opari2_Openmp_Region** )pomp_handle;
        SCOREP_ExitRegion( region->outerBlock );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}

//=======================================================================================
//=======================================================================================
//=======================================================================================

void
POMP2_Ordered_begin( POMP2_Region_handle* pomp_handle )
{
    SCOREP_OPARI2_OMP_ENSURE_INITIALIZED();

    UTILS_DEBUG_ENTRY();

    if ( SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) && scorep_opari2_recording_on )
    {
        SCOREP_Opari2_Openmp_Region* region = *( SCOREP_Opari2_Openmp_Region** )pomp_handle;
        SCOREP_EnterRegion( region->innerBlock );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}

void
POMP2_Ordered_end( POMP2_Region_handle* pomp_handle )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();

    UTILS_DEBUG_ENTRY();

    if ( SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) && scorep_opari2_recording_on )
    {
        SCOREP_Opari2_Openmp_Region* region = *( SCOREP_Opari2_Openmp_Region** )pomp_handle;
        SCOREP_ExitRegion( region->innerBlock );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}

void
POMP2_Ordered_enter( POMP2_Region_handle* pomp_handle,
                     const char           ctc_string[] )
{
    SCOREP_OPARI2_OMP_ENSURE_INITIALIZED();

    UTILS_DEBUG_ENTRY();

    if ( SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) && scorep_opari2_recording_on )
    {
        SCOREP_OPARI2_OPENMP_HANDLE_UNINITIALIZED_REGION( pomp_handle, ctc_string );
        SCOREP_Opari2_Openmp_Region* region = *( SCOREP_Opari2_Openmp_Region** )pomp_handle;
        SCOREP_EnterRegion( region->outerBlock );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}

void
POMP2_Ordered_exit( POMP2_Region_handle* pomp_handle )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();

    UTILS_DEBUG_ENTRY();

    if ( SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) && scorep_opari2_recording_on )
    {
        SCOREP_Opari2_Openmp_Region* region = *( SCOREP_Opari2_Openmp_Region** )pomp_handle;
        SCOREP_ExitRegion( region->outerBlock );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}


void
POMP2_Task_create_begin( POMP2_Region_handle* pomp_handle,
                         POMP2_Task_handle*   pomp_new_task,
                         POMP2_Task_handle*   pomp_old_task,
                         int                  pomp_if,
                         const char           ctc_string[] )
{
    SCOREP_OPARI2_OMP_ENSURE_INITIALIZED();

    UTILS_DEBUG_ENTRY();

    /* We must use pomp_old_task to reset the old task id after the creation.
       We cannot store the new task id in pomp_current_task, because other tasks
       maybe executed before. */
    *pomp_old_task = pomp_current_task;
    *pomp_new_task = POMP2_Get_new_task_handle();

    if ( SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) && scorep_opari2_recording_on )
    {
        SCOREP_OPARI2_OPENMP_HANDLE_UNINITIALIZED_REGION( pomp_handle, ctc_string );
        SCOREP_Opari2_Openmp_Region* region = *( SCOREP_Opari2_Openmp_Region** )pomp_handle;
        SCOREP_EnterRegion( region->outerBlock );
        SCOREP_ThreadForkJoin_TaskCreate( SCOREP_PARADIGM_OPENMP,
                                          pomp2_decode_task_handle( *pomp_new_task ) );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}

void
POMP2_Task_create_end( POMP2_Region_handle* pomp_handle,
                       POMP2_Task_handle    pomp_old_task )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();

    UTILS_DEBUG_ENTRY();

    if ( SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) && scorep_opari2_recording_on )
    {
        SCOREP_Opari2_Openmp_Region* region = *( SCOREP_Opari2_Openmp_Region** )pomp_handle;
        if ( pomp_current_task != pomp_old_task )
        {
            SCOREP_ThreadForkJoin_TaskSwitch( SCOREP_PARADIGM_OPENMP,
                                              pomp2_to_scorep_handle( pomp_old_task ) );
        }
        SCOREP_ExitRegion( region->outerBlock );
    }
    pomp_current_task = pomp_old_task;

    SCOREP_IN_MEASUREMENT_DECREMENT();
}

/*
 * NOTE: The POMP2_Task_handle changes its value and
 * type when the task execution starts. Between task creation and execution
 * POMP2_Task_handle contains a 64 bit unsigned integer value consisting of the
 * thread id and the generation number.
 * When execution starts, a data structure is allocated for the task and
 * the content of POMP2_Task_handle is replaced by a pointer to this structure.
 * Thus, Task events that happen before the start of the task execution, e.g.,
 * SCOREP_ThreadForkJoin_CreateTask(), access POMP2_Task_handle as a number.
 * Events that happen after SCOREP_ThreadForkJoin_TaskBegin() access
 * POMP2_Task_handle as a pointer to a data structure.
 */
void
POMP2_Task_begin( POMP2_Region_handle* pomp_handle,
                  POMP2_Task_handle    pomp_task )
{
    SCOREP_OPARI2_OMP_ENSURE_INITIALIZED();

    UTILS_DEBUG_ENTRY();

    pomp_current_task = pomp_task;

    if ( SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) && scorep_opari2_recording_on )
    {
        SCOREP_Opari2_Openmp_Region* region = *( SCOREP_Opari2_Openmp_Region** )pomp_handle;
        pomp_current_task = scorep_to_pomp2_handle(
            SCOREP_ThreadForkJoin_TaskBegin( SCOREP_PARADIGM_OPENMP,
                                             region->innerBlock,
                                             pomp2_decode_task_handle( pomp_current_task ) ) );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}

void
POMP2_Task_end( POMP2_Region_handle* pomp_handle )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();

    UTILS_DEBUG_ENTRY();

    if ( SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) && scorep_opari2_recording_on )
    {
        SCOREP_Opari2_Openmp_Region* region = *( SCOREP_Opari2_Openmp_Region** )pomp_handle;
        SCOREP_ThreadForkJoin_TaskEnd( SCOREP_PARADIGM_OPENMP,
                                       region->innerBlock,
                                       pomp2_to_scorep_handle( pomp_current_task ) );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}

void
POMP2_Untied_task_create_begin( POMP2_Region_handle* pomp_handle,
                                POMP2_Task_handle*   pomp_new_task,
                                POMP2_Task_handle*   pomp_old_task,
                                int                  pomp_if,
                                const char           ctc_string[] )
{
    SCOREP_OPARI2_OMP_ENSURE_INITIALIZED();

    UTILS_DEBUG_ENTRY();

    *pomp_new_task = POMP2_Get_new_task_handle();
    *pomp_old_task = pomp_current_task;

    if ( SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) && scorep_opari2_recording_on )
    {
        SCOREP_OPARI2_OPENMP_HANDLE_UNINITIALIZED_REGION( pomp_handle, ctc_string );
        SCOREP_Opari2_Openmp_Region* region = *( SCOREP_Opari2_Openmp_Region** )pomp_handle;
        SCOREP_EnterRegion( region->outerBlock );
        SCOREP_ThreadForkJoin_TaskCreate( SCOREP_PARADIGM_OPENMP,
                                          pomp2_decode_task_handle( *pomp_new_task ) );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}

void
POMP2_Untied_task_create_end( POMP2_Region_handle* pomp_handle,
                              POMP2_Task_handle    pomp_old_task )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();

    UTILS_DEBUG_ENTRY();

    if ( SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) && scorep_opari2_recording_on )
    {
        SCOREP_Opari2_Openmp_Region* region = *( SCOREP_Opari2_Openmp_Region** )pomp_handle;
        if ( pomp_current_task != pomp_old_task )
        {
            SCOREP_ThreadForkJoin_TaskSwitch( SCOREP_PARADIGM_OPENMP,
                                              pomp2_to_scorep_handle( pomp_old_task ) );
        }
        SCOREP_ExitRegion( region->outerBlock );
    }
    pomp_current_task = pomp_old_task;

    SCOREP_IN_MEASUREMENT_DECREMENT();
}

void
POMP2_Untied_task_begin( POMP2_Region_handle* pomp_handle,
                         POMP2_Task_handle    pomp_task )
{
    SCOREP_OPARI2_OMP_ENSURE_INITIALIZED();

    UTILS_DEBUG_ENTRY();

    pomp_current_task = pomp_task;

    if ( SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) && scorep_opari2_recording_on )
    {
        SCOREP_Opari2_Openmp_Region* region = *( SCOREP_Opari2_Openmp_Region** )pomp_handle;
        pomp_current_task = scorep_to_pomp2_handle(
            SCOREP_ThreadForkJoin_TaskBegin( SCOREP_PARADIGM_OPENMP,
                                             region->innerBlock,
                                             pomp2_decode_task_handle( pomp_current_task ) ) );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}

void
POMP2_Untied_task_end( POMP2_Region_handle* pomp_handle )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();

    UTILS_DEBUG_ENTRY();

    if ( SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) && scorep_opari2_recording_on )
    {
        SCOREP_Opari2_Openmp_Region* region = *( SCOREP_Opari2_Openmp_Region** )pomp_handle;
        SCOREP_ThreadForkJoin_TaskEnd( SCOREP_PARADIGM_OPENMP,
                                       region->innerBlock,
                                       pomp2_to_scorep_handle( pomp_current_task ) );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}

void
POMP2_Taskwait_begin( POMP2_Region_handle* pomp_handle,
                      POMP2_Task_handle*   pomp_old_task,
                      const char           ctc_string[] )
{
    SCOREP_OPARI2_OMP_ENSURE_INITIALIZED();

    UTILS_DEBUG_ENTRY();

    *pomp_old_task = pomp_current_task;

    if ( SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) && scorep_opari2_recording_on )
    {
        SCOREP_OPARI2_OPENMP_HANDLE_UNINITIALIZED_REGION( pomp_handle, ctc_string );
        SCOREP_Opari2_Openmp_Region* region = *( SCOREP_Opari2_Openmp_Region** )pomp_handle;
        SCOREP_EnterRegion( region->outerBlock );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}

void
POMP2_Taskwait_end( POMP2_Region_handle* pomp_handle,
                    POMP2_Task_handle    pomp_old_task )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();

    UTILS_DEBUG_ENTRY();

    if ( SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) && scorep_opari2_recording_on )
    {
        SCOREP_Opari2_Openmp_Region* region = *( SCOREP_Opari2_Openmp_Region** )pomp_handle;
        if ( pomp_current_task != pomp_old_task )
        {
            SCOREP_ThreadForkJoin_TaskSwitch( SCOREP_PARADIGM_OPENMP,
                                              pomp2_to_scorep_handle( pomp_old_task ) );
        }
        SCOREP_ExitRegion( region->outerBlock );
    }
    pomp_current_task = pomp_old_task;

    SCOREP_IN_MEASUREMENT_DECREMENT();
}

//=======================================================================================
//=======================================================================================
//=======================================================================================


void
POMP2_Workshare_enter( POMP2_Region_handle* pomp_handle,
                       const char           ctc_string[] )
{
    SCOREP_OPARI2_OMP_ENSURE_INITIALIZED();

    UTILS_DEBUG_ENTRY();

    if ( SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) && scorep_opari2_recording_on )
    {
        SCOREP_OPARI2_OPENMP_HANDLE_UNINITIALIZED_REGION( pomp_handle, ctc_string );
        SCOREP_Opari2_Openmp_Region* region = *( SCOREP_Opari2_Openmp_Region** )pomp_handle;
        SCOREP_EnterRegion( region->outerBlock );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}

void
POMP2_Workshare_exit( POMP2_Region_handle* pomp_handle )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();

    UTILS_DEBUG_ENTRY();

    if ( SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) && scorep_opari2_recording_on )
    {
        SCOREP_Opari2_Openmp_Region* region = *( SCOREP_Opari2_Openmp_Region** )pomp_handle;
        SCOREP_ExitRegion( region->outerBlock );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}

int
POMP2_Lib_get_max_threads( void )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    int result = omp_get_max_threads();
    SCOREP_IN_MEASUREMENT_DECREMENT();
    return result;
}

/* **************************************************************************************
 *                                                                  C wrapper for OMP API
 ***************************************************************************************/

void
POMP2_Init_lock( omp_lock_t* s )
{
    SCOREP_OPARI2_OMP_ENSURE_INITIALIZED();

    if ( !SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_IN_MEASUREMENT_DECREMENT();
        omp_init_lock( s );
        return;
    }

    UTILS_DEBUG_ENTRY();

    if ( scorep_opari2_recording_on )
    {
        SCOREP_EnterWrappedRegion( scorep_opari2_openmp_lock_region_handles[ SCOREP_OPARI2_OPENMP_INIT_LOCK ] );
    }
    else if ( SCOREP_IsUnwindingEnabled() )
    {
        SCOREP_EnterWrapper( scorep_opari2_openmp_lock_region_handles[ SCOREP_OPARI2_OPENMP_INIT_LOCK ] );
    }

    SCOREP_ENTER_WRAPPED_REGION();
    omp_init_lock( s );
    SCOREP_EXIT_WRAPPED_REGION();

    scorep_opari2_openmp_lock_init( s );

    if ( scorep_opari2_recording_on )
    {
        SCOREP_ExitRegion( scorep_opari2_openmp_lock_region_handles[ SCOREP_OPARI2_OPENMP_INIT_LOCK ] );
    }
    else if ( SCOREP_IsUnwindingEnabled() )
    {
        SCOREP_ExitWrapper( scorep_opari2_openmp_lock_region_handles[ SCOREP_OPARI2_OPENMP_INIT_LOCK ] );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}

void
POMP2_Destroy_lock( omp_lock_t* s )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();

    if ( !SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_IN_MEASUREMENT_DECREMENT();
        omp_destroy_lock( s );
        return;
    }

    UTILS_DEBUG_ENTRY();

    if ( scorep_opari2_recording_on )
    {
        SCOREP_EnterWrappedRegion( scorep_opari2_openmp_lock_region_handles[ SCOREP_OPARI2_OPENMP_DESTROY_LOCK ] );
    }
    else if ( SCOREP_IsUnwindingEnabled() )
    {
        SCOREP_EnterWrapper( scorep_opari2_openmp_lock_region_handles[ SCOREP_OPARI2_OPENMP_DESTROY_LOCK ] );
    }

    SCOREP_ENTER_WRAPPED_REGION();
    omp_destroy_lock( s );
    SCOREP_EXIT_WRAPPED_REGION();

    scorep_opari2_openmp_lock_destroy( s );

    if ( scorep_opari2_recording_on )
    {
        SCOREP_ExitRegion( scorep_opari2_openmp_lock_region_handles[ SCOREP_OPARI2_OPENMP_DESTROY_LOCK ] );
    }
    else if ( SCOREP_IsUnwindingEnabled() )
    {
        SCOREP_ExitWrapper( scorep_opari2_openmp_lock_region_handles[ SCOREP_OPARI2_OPENMP_DESTROY_LOCK ] );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}

void
POMP2_Set_lock( omp_lock_t* s )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();

    if ( !SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_IN_MEASUREMENT_DECREMENT();
        omp_set_lock( s );
        return;
    }

    UTILS_DEBUG_ENTRY();

    if ( scorep_opari2_recording_on )
    {
        SCOREP_EnterWrappedRegion( scorep_opari2_openmp_lock_region_handles[ SCOREP_OPARI2_OPENMP_SET_LOCK ] );
    }
    else if ( SCOREP_IsUnwindingEnabled() )
    {
        SCOREP_EnterWrapper( scorep_opari2_openmp_lock_region_handles[ SCOREP_OPARI2_OPENMP_SET_LOCK ] );
    }

    SCOREP_ENTER_WRAPPED_REGION();
    omp_set_lock( s );
    SCOREP_EXIT_WRAPPED_REGION();

    if ( scorep_opari2_recording_on )
    {
        SCOREP_MutexLock( scorep_opari2_openmp_lock );
        SCOREP_Opari2_Openmp_Lock* lock = SCOREP_Opari2_Openmp_GetAcquireLock( s );
        SCOREP_ThreadAcquireLock( SCOREP_PARADIGM_OPENMP, lock->handle, lock->acquisition_order );
        SCOREP_MutexUnlock( scorep_opari2_openmp_lock );

        SCOREP_ExitRegion( scorep_opari2_openmp_lock_region_handles[ SCOREP_OPARI2_OPENMP_SET_LOCK ] );
    }
    else if ( SCOREP_IsUnwindingEnabled() )
    {
        SCOREP_ExitWrapper( scorep_opari2_openmp_lock_region_handles[ SCOREP_OPARI2_OPENMP_SET_LOCK ] );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}

void
POMP2_Unset_lock( omp_lock_t* s )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();

    if ( !SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_IN_MEASUREMENT_DECREMENT();
        omp_unset_lock( s );
        return;
    }

    UTILS_DEBUG_ENTRY();

    if ( scorep_opari2_recording_on )
    {
        SCOREP_EnterWrappedRegion( scorep_opari2_openmp_lock_region_handles[ SCOREP_OPARI2_OPENMP_UNSET_LOCK ] );

        SCOREP_MutexLock( scorep_opari2_openmp_lock );
        SCOREP_Opari2_Openmp_Lock* lock = SCOREP_Opari2_Openmp_GetReleaseLock( s );
        SCOREP_ThreadReleaseLock( SCOREP_PARADIGM_OPENMP, lock->handle, lock->acquisition_order );
        SCOREP_MutexUnlock( scorep_opari2_openmp_lock );
    }
    else if ( SCOREP_IsUnwindingEnabled() )
    {
        SCOREP_EnterWrapper( scorep_opari2_openmp_lock_region_handles[ SCOREP_OPARI2_OPENMP_UNSET_LOCK ] );
    }

    SCOREP_ENTER_WRAPPED_REGION();
    omp_unset_lock( s );
    SCOREP_EXIT_WRAPPED_REGION();

    if ( scorep_opari2_recording_on )
    {
        SCOREP_ExitRegion( scorep_opari2_openmp_lock_region_handles[ SCOREP_OPARI2_OPENMP_UNSET_LOCK ] );
    }
    else if ( SCOREP_IsUnwindingEnabled() )
    {
        SCOREP_ExitWrapper( scorep_opari2_openmp_lock_region_handles[ SCOREP_OPARI2_OPENMP_UNSET_LOCK ] );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}

int
POMP2_Test_lock( omp_lock_t* s )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();

    if ( !SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_IN_MEASUREMENT_DECREMENT();
        return omp_test_lock( s );
    }

    UTILS_DEBUG_ENTRY();

    if ( SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) && scorep_opari2_recording_on )
    {
        SCOREP_EnterWrappedRegion( scorep_opari2_openmp_lock_region_handles[ SCOREP_OPARI2_OPENMP_TEST_LOCK ] );
    }
    else if ( SCOREP_IsUnwindingEnabled() )
    {
        SCOREP_EnterWrapper( scorep_opari2_openmp_lock_region_handles[ SCOREP_OPARI2_OPENMP_TEST_LOCK ] );
    }

    SCOREP_ENTER_WRAPPED_REGION();
    int result = omp_test_lock( s );
    SCOREP_EXIT_WRAPPED_REGION();

    if ( scorep_opari2_recording_on )
    {
        if ( result )
        {
            SCOREP_MutexLock( scorep_opari2_openmp_lock );
            SCOREP_Opari2_Openmp_Lock* lock = SCOREP_Opari2_Openmp_GetAcquireLock( s );
            SCOREP_ThreadAcquireLock( SCOREP_PARADIGM_OPENMP, lock->handle, lock->acquisition_order );
            SCOREP_MutexUnlock( scorep_opari2_openmp_lock );
        }
        SCOREP_ExitRegion( scorep_opari2_openmp_lock_region_handles[ SCOREP_OPARI2_OPENMP_TEST_LOCK ] );
    }
    else if ( SCOREP_IsUnwindingEnabled() )
    {
        SCOREP_ExitWrapper( scorep_opari2_openmp_lock_region_handles[ SCOREP_OPARI2_OPENMP_TEST_LOCK ] );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
    return result;
}

void
POMP2_Init_nest_lock( omp_nest_lock_t* s )
{
    SCOREP_OPARI2_OMP_ENSURE_INITIALIZED();

    if ( !SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_IN_MEASUREMENT_DECREMENT();
        omp_init_nest_lock( s );
        return;
    }

    UTILS_DEBUG_ENTRY();

    if ( scorep_opari2_recording_on )
    {
        SCOREP_EnterWrappedRegion( scorep_opari2_openmp_lock_region_handles[ SCOREP_OPARI2_OPENMP_INIT_NEST_LOCK ] );
    }
    else if ( SCOREP_IsUnwindingEnabled() )
    {
        SCOREP_EnterWrapper( scorep_opari2_openmp_lock_region_handles[ SCOREP_OPARI2_OPENMP_INIT_NEST_LOCK ] );
    }

    SCOREP_ENTER_WRAPPED_REGION();
    omp_init_nest_lock( s );
    SCOREP_EXIT_WRAPPED_REGION();

    scorep_opari2_openmp_lock_init( s );

    if ( scorep_opari2_recording_on )
    {
        SCOREP_ExitRegion( scorep_opari2_openmp_lock_region_handles[ SCOREP_OPARI2_OPENMP_INIT_NEST_LOCK ] );
    }
    else if ( SCOREP_IsUnwindingEnabled() )
    {
        SCOREP_ExitWrapper( scorep_opari2_openmp_lock_region_handles[ SCOREP_OPARI2_OPENMP_INIT_NEST_LOCK ] );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}

void
POMP2_Destroy_nest_lock( omp_nest_lock_t* s )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();

    if ( !SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_IN_MEASUREMENT_DECREMENT();
        omp_destroy_nest_lock( s );
        return;
    }

    UTILS_DEBUG_ENTRY();

    if ( scorep_opari2_recording_on )
    {
        SCOREP_EnterWrappedRegion( scorep_opari2_openmp_lock_region_handles[ SCOREP_OPARI2_OPENMP_DESTROY_NEST_LOCK ] );
    }
    else if ( SCOREP_IsUnwindingEnabled() )
    {
        SCOREP_EnterWrapper( scorep_opari2_openmp_lock_region_handles[ SCOREP_OPARI2_OPENMP_DESTROY_NEST_LOCK ] );
    }

    SCOREP_ENTER_WRAPPED_REGION();
    omp_destroy_nest_lock( s );
    SCOREP_EXIT_WRAPPED_REGION();

    scorep_opari2_openmp_lock_destroy( s );
    if ( scorep_opari2_recording_on )
    {
        SCOREP_ExitRegion( scorep_opari2_openmp_lock_region_handles[ SCOREP_OPARI2_OPENMP_DESTROY_NEST_LOCK ] );
    }
    else if ( SCOREP_IsUnwindingEnabled() )
    {
        SCOREP_ExitWrapper( scorep_opari2_openmp_lock_region_handles[ SCOREP_OPARI2_OPENMP_DESTROY_NEST_LOCK ] );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}

void
POMP2_Set_nest_lock( omp_nest_lock_t* s )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();

    if ( !SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_IN_MEASUREMENT_DECREMENT();
        omp_set_nest_lock( s );
        return;
    }

    UTILS_DEBUG_ENTRY();

    if ( scorep_opari2_recording_on )
    {
        SCOREP_EnterWrappedRegion( scorep_opari2_openmp_lock_region_handles[ SCOREP_OPARI2_OPENMP_SET_NEST_LOCK ] );
    }
    else if ( SCOREP_IsUnwindingEnabled() )
    {
        SCOREP_EnterWrapper( scorep_opari2_openmp_lock_region_handles[ SCOREP_OPARI2_OPENMP_SET_NEST_LOCK ] );
    }

    SCOREP_ENTER_WRAPPED_REGION();
    omp_set_nest_lock( s );
    SCOREP_EXIT_WRAPPED_REGION();

    if ( scorep_opari2_recording_on )
    {
        SCOREP_MutexLock( scorep_opari2_openmp_lock );
        SCOREP_Opari2_Openmp_Lock* lock = SCOREP_Opari2_Openmp_GetAcquireNestLock( s );
        SCOREP_ThreadAcquireLock( SCOREP_PARADIGM_OPENMP, lock->handle, lock->acquisition_order );
        SCOREP_MutexUnlock( scorep_opari2_openmp_lock );
        SCOREP_ExitRegion( scorep_opari2_openmp_lock_region_handles[ SCOREP_OPARI2_OPENMP_SET_NEST_LOCK ] );
    }
    else if ( SCOREP_IsUnwindingEnabled() )
    {
        SCOREP_ExitWrapper( scorep_opari2_openmp_lock_region_handles[ SCOREP_OPARI2_OPENMP_SET_NEST_LOCK ] );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}

void
POMP2_Unset_nest_lock( omp_nest_lock_t* s )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();

    if ( !SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_IN_MEASUREMENT_DECREMENT();
        omp_unset_nest_lock( s );
        return;
    }

    UTILS_DEBUG_ENTRY();

    if ( scorep_opari2_recording_on )
    {
        SCOREP_EnterWrappedRegion( scorep_opari2_openmp_lock_region_handles[ SCOREP_OPARI2_OPENMP_UNSET_NEST_LOCK ] );
        SCOREP_MutexLock( scorep_opari2_openmp_lock );
        SCOREP_Opari2_Openmp_Lock* lock = SCOREP_Opari2_Openmp_GetReleaseNestLock( s );
        SCOREP_ThreadReleaseLock( SCOREP_PARADIGM_OPENMP, lock->handle, lock->acquisition_order );
        SCOREP_MutexUnlock( scorep_opari2_openmp_lock );
    }
    else if ( SCOREP_IsUnwindingEnabled() )
    {
        SCOREP_EnterWrapper( scorep_opari2_openmp_lock_region_handles[ SCOREP_OPARI2_OPENMP_UNSET_NEST_LOCK ] );
    }

    SCOREP_ENTER_WRAPPED_REGION();
    omp_unset_nest_lock( s );
    SCOREP_EXIT_WRAPPED_REGION();

    if ( scorep_opari2_recording_on )
    {
        SCOREP_ExitRegion( scorep_opari2_openmp_lock_region_handles[ SCOREP_OPARI2_OPENMP_UNSET_NEST_LOCK ] );
    }
    else if ( SCOREP_IsUnwindingEnabled() )
    {
        SCOREP_ExitWrapper( scorep_opari2_openmp_lock_region_handles[ SCOREP_OPARI2_OPENMP_UNSET_NEST_LOCK ] );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}

int
POMP2_Test_nest_lock( omp_nest_lock_t* s )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();

    if ( !SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_IN_MEASUREMENT_DECREMENT();
        return omp_test_nest_lock( s );
    }

    UTILS_DEBUG_ENTRY();

    if ( scorep_opari2_recording_on )
    {
        SCOREP_EnterWrappedRegion( scorep_opari2_openmp_lock_region_handles[ SCOREP_OPARI2_OPENMP_TEST_NEST_LOCK ] );
    }
    else if ( SCOREP_IsUnwindingEnabled() )
    {
        SCOREP_EnterWrapper( scorep_opari2_openmp_lock_region_handles[ SCOREP_OPARI2_OPENMP_TEST_NEST_LOCK ] );
    }

    SCOREP_ENTER_WRAPPED_REGION();
    int result = omp_test_nest_lock( s );
    SCOREP_EXIT_WRAPPED_REGION();

    if ( scorep_opari2_recording_on )
    {
        if ( result )
        {
            SCOREP_MutexLock( scorep_opari2_openmp_lock );
            SCOREP_Opari2_Openmp_Lock* lock = SCOREP_Opari2_Openmp_GetAcquireNestLock( s );
            SCOREP_ThreadAcquireLock( SCOREP_PARADIGM_OPENMP, lock->handle, lock->acquisition_order );
            SCOREP_MutexUnlock( scorep_opari2_openmp_lock );
        }

        SCOREP_ExitRegion( scorep_opari2_openmp_lock_region_handles[ SCOREP_OPARI2_OPENMP_TEST_NEST_LOCK ] );
    }
    else if ( SCOREP_IsUnwindingEnabled() )
    {
        SCOREP_ExitWrapper( scorep_opari2_openmp_lock_region_handles[ SCOREP_OPARI2_OPENMP_TEST_NEST_LOCK ] );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
    return result;
}

/** @} */
