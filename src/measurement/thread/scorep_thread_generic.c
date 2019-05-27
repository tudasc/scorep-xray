/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2016, 2019,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2016, 2018,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2013,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2013,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 */

/**
 * @file
 */


#include <config.h>
#include <scorep_thread_generic.h>
#include <scorep_thread_model_specific.h>

#include <SCOREP_Mutex.h>
#include <scorep_location_management.h>
#include <scorep_subsystem_management.h>

#include <UTILS_Error.h>

#define SCOREP_DEBUG_MODULE_NAME THREAD
#include <UTILS_Debug.h>

#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

/* *INDENT-OFF* */
/* *INDENT-ON*  */



typedef struct scorep_thread_private_data scorep_thread_private_data;
struct scorep_thread_private_data
{
    struct SCOREP_Location*     location;
    scorep_thread_private_data* parent;

    /** Used to transfer the sequence count from SCOREP_Thread_Fork()
     * to SCOREP_Thread_Begin(). */
    uint32_t tmp_fork_sequence_count;

    /** Holds the reference to the current thread team */
    SCOREP_InterimCommunicatorHandle thread_team;
};


static uint32_t sequence_count = 0;

static SCOREP_Mutex sequence_count_lock;


static scorep_thread_private_data* initial_tpd;



void
SCOREP_Thread_Initialize( void )
{
    UTILS_DEBUG_ENTRY();
    SCOREP_ErrorCode result = SCOREP_MutexCreate( &sequence_count_lock );
    UTILS_BUG_ON( result != SCOREP_SUCCESS, "" );

    UTILS_BUG_ON( initial_tpd != 0, "" );

    SCOREP_Location* location = SCOREP_Location_CreateCPULocation( "Master thread" );

    initial_tpd = scorep_thread_create_private_data( 0 /* parent_tpd */,
                                                     location );
    scorep_thread_set_location( initial_tpd, location );

    scorep_thread_on_initialize( initial_tpd );

    UTILS_DEBUG_EXIT();
}


void
SCOREP_Thread_ActivateLocation( struct SCOREP_Location* location,
                                struct SCOREP_Location* parent )
{
    scorep_subsystems_activate_cpu_location( location,
                                             parent,
                                             scorep_thread_get_next_sequence_count(),
                                             SCOREP_CPU_LOCATION_PHASE_MGMT );
}


void
SCOREP_Thread_Finalize( void )
{
    UTILS_DEBUG_ENTRY();
    UTILS_BUG_ON( initial_tpd == 0, "" );
    if ( scorep_thread_get_private_data() != initial_tpd )
    {
        UTILS_WARNING( "Measurement finalization not on master thread but on location %" PRIu32 ".",
                       SCOREP_Location_GetId( SCOREP_Location_GetCurrentCPULocation() ) );
    }

    scorep_thread_on_finalize( initial_tpd );
    scorep_thread_delete_private_data( initial_tpd );

    initial_tpd = 0;

    SCOREP_ErrorCode result = SCOREP_MutexDestroy( &sequence_count_lock );
    UTILS_BUG_ON( result != SCOREP_SUCCESS );
}


uint32_t
scorep_thread_get_next_sequence_count( void )
{
    UTILS_DEBUG_ENTRY();
    SCOREP_MutexLock( sequence_count_lock );
    uint32_t tmp = sequence_count++;
    SCOREP_MutexUnlock( sequence_count_lock );
    return tmp;
}


/* Define SIZEOF_GENERIC_THREAD_PRIVATE_DATA as the smallest multiple of
 * SCOREP_ALLOCATOR_ALIGNMENT that can hold a scorep_thread_private_data object.*/
#define SIZEOF_GENERIC_THREAD_PRIVATE_DATA  \
    sizeof( scorep_thread_private_data ) +  \
    ( SCOREP_ALLOCATOR_ALIGNMENT - 1 ) -    \
    ( ( sizeof( scorep_thread_private_data ) - 1 ) % SCOREP_ALLOCATOR_ALIGNMENT )


scorep_thread_private_data*
scorep_thread_create_private_data( scorep_thread_private_data* parent,
                                   SCOREP_Location*            location )
{
    UTILS_DEBUG_ENTRY();
    /* Create scorep_thread_private_data and the model-specific data in one
     * allocation. scorep_thread_private_data is SCOREP_ALLOCATOR_ALIGNMENT
     * aligned. The model-specific part starts at
     * ( char* )new_tpd + SIZEOF_GENERIC_THREAD_PRIVATE_DATA.
     * See scorep_thread_get_model_data().
     */
    size_t total_size = SIZEOF_GENERIC_THREAD_PRIVATE_DATA
                        + scorep_thread_get_sizeof_model_data();
    scorep_thread_private_data* new_tpd =
        SCOREP_Location_AllocForMisc( location, total_size );
    memset( new_tpd, 0, total_size );
    new_tpd->parent                  = parent;
    new_tpd->tmp_fork_sequence_count = SCOREP_THREAD_INVALID_SEQUENCE_COUNT;
    scorep_thread_on_create_private_data( new_tpd,
                                          scorep_thread_get_model_data( new_tpd ) );

    return new_tpd;
}


void*
scorep_thread_get_model_data( scorep_thread_private_data* tpd )
{
    UTILS_DEBUG_ENTRY();
    /* For memory layout, see scorep_thread_create_private_data(). */
    UTILS_ASSERT( tpd );
    return ( char* )tpd + SIZEOF_GENERIC_THREAD_PRIVATE_DATA;
}


bool
scorep_thread_is_initial_thread( scorep_thread_private_data* tpd )
{
    UTILS_DEBUG_ENTRY();
    UTILS_ASSERT( tpd );
    return tpd == initial_tpd;
}


scorep_thread_private_data*
SCOREP_Thread_GetInitialTpd( void )
{
    UTILS_DEBUG_ENTRY();
    return initial_tpd;
}


bool
SCOREP_Thread_IsIntialThread( void )
{
    return scorep_thread_is_initial_thread( scorep_thread_get_private_data() );
}

scorep_thread_private_data*
scorep_thread_get_parent( scorep_thread_private_data* tpd )
{
    UTILS_DEBUG_ENTRY();
    UTILS_ASSERT( tpd );
    return tpd->parent;
}


void
scorep_thread_set_parent( scorep_thread_private_data* tpd,
                          scorep_thread_private_data* parent )
{
    UTILS_DEBUG_ENTRY();
    UTILS_ASSERT( tpd );
    tpd->parent = parent;
}


struct SCOREP_Location*
scorep_thread_get_location( scorep_thread_private_data* tpd )
{
    UTILS_DEBUG_ENTRY();
    UTILS_ASSERT( tpd );
    return tpd->location;
}


void
scorep_thread_set_location( scorep_thread_private_data* tpd,
                            struct SCOREP_Location*     location )
{
    UTILS_DEBUG_ENTRY();
    UTILS_ASSERT( tpd && tpd->location == 0 );
    tpd->location = location;
}


uint32_t
scorep_thread_get_tmp_sequence_count( scorep_thread_private_data* tpd )
{
    UTILS_DEBUG_ENTRY();
    UTILS_ASSERT( tpd );
    return tpd->tmp_fork_sequence_count;
}


void
scorep_thread_set_tmp_sequence_count( scorep_thread_private_data* tpd,
                                      uint32_t                    sequenceCount )
{
    UTILS_DEBUG_ENTRY();
    UTILS_ASSERT( tpd );
    tpd->tmp_fork_sequence_count = sequenceCount;
}


SCOREP_InterimCommunicatorHandle
scorep_thread_get_team( struct scorep_thread_private_data* tpd )
{
    UTILS_DEBUG_ENTRY();
    UTILS_ASSERT( tpd );
    return tpd->thread_team;
}


void
scorep_thread_set_team( struct scorep_thread_private_data* tpd,
                        SCOREP_InterimCommunicatorHandle   team )
{
    UTILS_DEBUG_ENTRY();
    UTILS_ASSERT( tpd );
    tpd->thread_team = team;
}
