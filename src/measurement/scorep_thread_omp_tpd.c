/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
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
 * @file       src/measurement/scorep_thread_omp_tpd.c
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status alpha
 *
 *
 */

#include <config.h>
#include "scorep_thread.h"
#include "scorep_location.h"
#include <scorep_definitions.h>
#include <SCOREP_Memory.h>
#include <SCOREP_Subsystem.h>
#include <SCOREP_Omp.h>
#include <SCOREP_Timing.h>
#include <scorep_mpi.h>
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include "scorep_runtime_management.h"
#include "scorep_status.h"
#include "scorep_subsystem.h"
#include <tracing/SCOREP_Tracing_ThreadInteraction.h>
#include <SCOREP_Mutex.h>

#include <UTILS_Error.h>
#include <UTILS_Debug.h>

#include "scorep_environment.h"

#define POMP_TPD_MANGLED FORTRAN_MANGLED( pomp_tpd )


// The thread private variable that points to a
// SCOREP_Thread_ThreadPrivateData object. It needs to be a 64bit integer
// to smoothly work with OPARI2 instrumented Fortran code. It will be
// casted to SCOREP_Thread_ThreadPrivateData* before usage, see the macro
// TPD. Dependent on the compiler it will be pomp_tdp or pomp_tpd_, see
// config.h.
int64_t FORTRAN_ALIGNED POMP_TPD_MANGLED = 0;


// Easy access to the thread private variable. TPD stands for ThreadPrivateData
#if SIZEOF_VOID_P == 8
#define TPD ( ( SCOREP_Thread_ThreadPrivateData* )POMP_TPD_MANGLED )
#elif SIZEOF_VOID_P == 4
#define TPD ( ( SCOREP_Thread_ThreadPrivateData* )( uint32_t )POMP_TPD_MANGLED )
#else
#error Unsupported architecture. Only 32 bit and 64 bit architectures are supported.
#endif

// We want to write #pragma omp threadprivate(POMP_TPD_MANGLED) but as
// POMP_TPD_MANGLED is a macro itself, we need to do some preprocessor
// magic to be on the safe side.
SCOREP_PRAGMA_OMP( threadprivate( POMP_TPD_MANGLED ) )


typedef struct SCOREP_Thread_ThreadPrivateData SCOREP_Thread_ThreadPrivateData;


/* *INDENT-OFF* */
static SCOREP_Location* scorep_thread_create_location_data_for(SCOREP_Thread_ThreadPrivateData* tpd);
static SCOREP_Thread_ThreadPrivateData* scorep_thread_create_thread_private_data();
static void scorep_thread_call_externals_on_location_activation(SCOREP_Location* locationData, SCOREP_Location* parent, uint32_t nestingLevel);
static void scorep_thread_call_externals_on_location_deactivation(SCOREP_Location* locationData, SCOREP_Location* parent);
static void scorep_thread_delete_thread_private_data_recursively( SCOREP_Thread_ThreadPrivateData* tpd );
static void scorep_thread_init_children_to_null(SCOREP_Thread_ThreadPrivateData** children, size_t startIndex, size_t endIndex);
static void scorep_thread_update_tpd(SCOREP_Thread_ThreadPrivateData* newTPD);
/* *INDENT-ON* */


struct SCOREP_Thread_ThreadPrivateData
{
    SCOREP_Thread_ThreadPrivateData*  parent;
    SCOREP_Thread_ThreadPrivateData** children;
    uint32_t                          n_children;
    bool                              is_active;
    uint32_t                          n_reusages;
    uint32_t                          nesting_level; // starting at 1, but SCOREP_Thread_Initialize passes 0 to profiling
    SCOREP_Location*                  location_data;
};


static struct SCOREP_Thread_ThreadPrivateData* initial_thread;


void
SCOREP_Thread_Initialize()
{
    assert( !omp_in_parallel() );
    assert( initial_thread == 0 );
    assert( POMP_TPD_MANGLED == 0 );

    initial_thread         = scorep_thread_create_thread_private_data();
    initial_thread->parent = 0;

    scorep_thread_create_location_data_for( initial_thread );

    assert( POMP_TPD_MANGLED );
    assert( TPD );
    assert( TPD->is_active );
    assert( TPD->location_data );

    scorep_thread_call_externals_on_location_activation( TPD->location_data, 0, 0 /* nesting level */ );
}


void
scorep_thread_update_tpd( SCOREP_Thread_ThreadPrivateData* newTPD )
{
#if SIZEOF_VOID_P == 8
    POMP_TPD_MANGLED = ( uint64_t )newTPD;
#elif SIZEOF_VOID_P == 4
    POMP_TPD_MANGLED = ( uint32_t )newTPD;
#else
#error Unsupported architecture. Only 32 bit and 64 bit architectures are supported.
#endif
}


void
scorep_thread_call_externals_on_location_activation( SCOREP_Location* locationData,
                                                     SCOREP_Location* parent,
                                                     uint32_t         nestingLevel )
{
    if ( SCOREP_IsProfilingEnabled() )
    {
        SCOREP_Profile_OnLocationActivation( locationData, parent, nestingLevel );
    }
    SCOREP_Tracing_OnLocationActivation( locationData, parent );
}


SCOREP_Location*
scorep_thread_create_location_data_for( SCOREP_Thread_ThreadPrivateData* tpd )
{
    char*            name   = "";
    SCOREP_Location* parent = NULL;
    if ( tpd->parent )
    {
        parent = tpd->parent->location_data;
    }
    /* the tpd-based thread implementation must notify not before the tpd update. */
    SCOREP_Location* new_location = SCOREP_Location_CreateCPULocation( parent, name, true /* defer_new_location_notication */ );

    assert( tpd->location_data == 0 );
    tpd->location_data = new_location;
    scorep_thread_update_tpd( tpd );

    scorep_location_call_externals_on_new_location( new_location, name, parent );

    return new_location;
}


SCOREP_Thread_ThreadPrivateData*
scorep_thread_create_thread_private_data()
{
    // need synchronized malloc here
    SCOREP_Thread_ThreadPrivateData* new_tpd;
    new_tpd             = malloc( sizeof( SCOREP_Thread_ThreadPrivateData ) );
    new_tpd->parent     = TPD;
    new_tpd->children   = 0;
    new_tpd->n_children = 0;
    new_tpd->is_active  = true;
    new_tpd->n_reusages = 0;
    if ( TPD )
    {
        new_tpd->nesting_level = TPD->nesting_level + 1;
    }
    else
    {
        new_tpd->nesting_level = 1;
    }
    new_tpd->location_data = 0;
    return new_tpd;
}


void
SCOREP_Thread_Finalize()
{
    assert( !omp_in_parallel() );
    assert( initial_thread != 0 );
    assert( POMP_TPD_MANGLED != 0 );

    scorep_thread_delete_thread_private_data_recursively( initial_thread );

    initial_thread = 0;
}


void
scorep_thread_delete_thread_private_data_recursively( SCOREP_Thread_ThreadPrivateData* tpd )
{
    assert( tpd );
    bool have_unused_child = false;
    for ( uint32_t i = 0; i < tpd->n_children; ++i )
    {
        if ( tpd->children[ i ] )
        {
            assert( have_unused_child == false ); // no used child after first unused allowed
            assert( !tpd->children[ i ]->is_active );
            scorep_thread_delete_thread_private_data_recursively( tpd->children[ i ] );
        }
        else
        {
            if ( have_unused_child == false )
            {
                have_unused_child = true;
                continue;
            }
        }
    }
    free( tpd->children ); /// @todo remove if SCOREP_Memory is used for child allocation
    free( tpd );
}


void
SCOREP_Thread_OnThreadFork( size_t nRequestedThreads )
{
    assert( TPD->is_active );
    TPD->is_active = false;

    /// @todo replace malloc/realloc with SCOREPs memory management to get rid of
    ///  locking. we have access to TPD->location_data->page_managers.
    if ( !TPD->children || TPD->n_children < nRequestedThreads )
    {
        TPD->children = realloc( TPD->children, nRequestedThreads *
                                 sizeof( SCOREP_Thread_ThreadPrivateData* ) );
        scorep_thread_init_children_to_null( TPD->children, TPD->n_children, nRequestedThreads - 1 );
        TPD->n_children = nRequestedThreads;
    }
}


void
scorep_thread_init_children_to_null( SCOREP_Thread_ThreadPrivateData** children,
                                     size_t                            firstIndex,
                                     size_t                            lastIndex )
{
    assert( firstIndex <= lastIndex );
    for ( size_t i = firstIndex; i <= lastIndex; ++i )
    {
        children[ i ] = 0;
    }
}


void
SCOREP_Thread_OnThreadJoin()
{
    // We are still referencing the TPD object of the previous parallel
    // regions's master thread.

    if ( TPD->n_reusages == 0 )
    {
        assert( TPD->parent );
        scorep_thread_update_tpd( TPD->parent );
    }
    else
    {
        assert( TPD->is_active );
        // There was no parallelism in the previous parallel region
        // so we must not update TPD.
    }

    if ( !TPD->is_active )
    {
        // last parallel region used by more than one thread
        for ( size_t i = 0; i < TPD->n_children; ++i )
        {
            if ( TPD->children[ i ]->is_active )
            {
                scorep_thread_call_externals_on_location_deactivation(
                    TPD->children[ i ]->location_data, TPD->location_data );
                TPD->children[ i ]->is_active = false;
            }
        }
        uint64_t current_timestamp = SCOREP_GetClockTicks();
        UTILS_BUG_ON( SCOREP_Location_GetLastTimestamp( TPD->location_data ) > current_timestamp,
                      "Wrong timestamp order [0]: %" PRIu64 " (last recorded) > %" PRIu64 " (current).",
                      SCOREP_Location_GetLastTimestamp( TPD->location_data ), current_timestamp );
        TPD->is_active = true;
        // need activation here?
    }
    else
    {
        assert( TPD->n_reusages > 0 );
        TPD->n_reusages--;
        // no parallelism in last parallel region, parent == child
        scorep_thread_call_externals_on_location_deactivation( TPD->location_data,
                                                               TPD->location_data );
    }
}


void
scorep_thread_call_externals_on_location_deactivation( SCOREP_Location* locationData,
                                                       SCOREP_Location* parent )
{
    if ( SCOREP_IsProfilingEnabled() )
    {
        SCOREP_Profile_OnLocationDeactivation( locationData, parent );
    }
    SCOREP_Tracing_OnLocationDeactivation( locationData, parent );
}


SCOREP_Location*
SCOREP_Location_GetCurrentCPULocation()
{
    if ( TPD->is_active )
    {
        return TPD->location_data;
    }

    // entered parallel region. thread will be activated, maybe created. TPD
    // is the parent
    if ( omp_get_num_threads() == 1 )
    {
        // there is no additional parallelism in this parallel region. don't
        // update TPD with a child but reuse the parent.
        TPD->is_active = true;
        TPD->n_reusages++;
        scorep_thread_call_externals_on_location_activation( TPD->location_data,
                                                             TPD->location_data,
                                                             TPD->nesting_level ); // use same nesting level as in fork
    }
    else
    {
        // set TPD to a child of itself, create new one if neccessary
        size_t my_thread_id = omp_get_thread_num();
        assert( my_thread_id < TPD->n_children );
        SCOREP_Thread_ThreadPrivateData** my_tpd = &( TPD->children[ my_thread_id ] );
        if ( *my_tpd )
        {
            // already been in this thread
            assert( !( *my_tpd )->is_active );
            ( *my_tpd )->is_active = true;
            scorep_thread_update_tpd( *my_tpd );
            uint64_t current_timestamp = SCOREP_GetClockTicks();
            UTILS_BUG_ON( SCOREP_Location_GetLastTimestamp( TPD->location_data ) > current_timestamp,
                          "Wrong timestamp order [1]: %" PRIu64 " (last recorded) > %" PRIu64 " (current).",
                          SCOREP_Location_GetLastTimestamp( TPD->location_data ), current_timestamp );
        }
        else
        {
            // never been here before
            *my_tpd = scorep_thread_create_thread_private_data();
            if ( my_thread_id == 0 )
            {
                // reuse parents location data
                ( *my_tpd )->location_data = TPD->location_data;
                scorep_thread_update_tpd( *my_tpd );
                uint64_t current_timestamp = SCOREP_GetClockTicks();
                UTILS_BUG_ON( SCOREP_Location_GetLastTimestamp( TPD->location_data ) > current_timestamp,
                              "Wrong timestamp order [2]: %" PRIu64 " (last recorded) > %" PRIu64 " (current).",
                              SCOREP_Location_GetLastTimestamp( TPD->location_data ), current_timestamp );
            }
            else
            {
                scorep_thread_create_location_data_for( *my_tpd );
                uint64_t current_timestamp = SCOREP_GetClockTicks();
                UTILS_BUG_ON( SCOREP_Location_GetLastTimestamp( TPD->location_data ) > current_timestamp,
                              "Wrong timestamp order [3]: %" PRIu64 " (last recorded) > %" PRIu64 " (current).",
                              SCOREP_Location_GetLastTimestamp( TPD->location_data ), current_timestamp );
            }
        }
        scorep_thread_call_externals_on_location_activation( TPD->location_data,
                                                             TPD->parent->location_data,
                                                             TPD->parent->nesting_level ); // use same nesting level as in fork
    }

    return TPD->location_data;
}


uint32_t
scorep_thread_get_nesting_level()
{
    return TPD->nesting_level;
}
