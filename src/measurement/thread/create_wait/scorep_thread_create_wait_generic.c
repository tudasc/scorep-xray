/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2014-2015,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2014-2016,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2015,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */


/**
 * @file
 */


#include <config.h>

#include <SCOREP_ThreadCreateWait_Event.h>
#include <scorep_thread_generic.h>
#include <scorep_thread_model_specific.h>
#include <scorep_thread_create_wait_model_specific.h>

#include <scorep_substrates_definition.h>

#include <scorep_subsystem.h>
#include <SCOREP_Mutex.h>
#include <SCOREP_Definitions.h>
#include <scorep_events_common.h>
#include <SCOREP_Memory.h>
#include <SCOREP_Paradigms.h>
#include <SCOREP_Properties.h>
#include <scorep_unify_helpers.h>

#define SCOREP_DEBUG_MODULE_NAME THREAD_CREATE_WAIT
#include <UTILS_Debug.h>

#include <UTILS_Error.h>


#include <stdbool.h>
#include <stdint.h>
#include <inttypes.h>
#include <stddef.h>
#include <string.h>


/* *INDENT-OFF* */
static SCOREP_ErrorCode create_wait_subsystem_init( void );
static SCOREP_ErrorCode create_wait_subsystem_pre_unify( void );
static SCOREP_ErrorCode create_wait_subsystem_post_unify( void );
static void create_wait_subsystem_finalize( void );
/* *INDENT-ON*  */


static SCOREP_InterimCommunicatorHandle thread_team = SCOREP_INVALID_INTERIM_COMMUNICATOR;
static bool is_initialized;
static bool is_finalized;

static uint32_t     active_locations;
static SCOREP_Mutex thread_create_mutex;


const SCOREP_Subsystem SCOREP_Subsystem_ThreadCreateWait =
{
    .subsystem_name       = "THREAD CREATE WAIT",
    .subsystem_init       = &create_wait_subsystem_init,
    .subsystem_pre_unify  = &create_wait_subsystem_pre_unify,
    .subsystem_post_unify = &create_wait_subsystem_post_unify,
    .subsystem_finalize   = &create_wait_subsystem_finalize,
};


static SCOREP_ErrorCode
create_wait_subsystem_init( void )
{
    UTILS_DEBUG_ENTRY();
    if ( is_initialized )
    {
        return SCOREP_SUCCESS;
    }
    is_initialized = true;

    SCOREP_ErrorCode result = SCOREP_MutexCreate( &thread_create_mutex );
    UTILS_BUG_ON( result != SCOREP_SUCCESS, "" );

    thread_team = SCOREP_Definitions_NewInterimCommunicator(
        SCOREP_INVALID_INTERIM_COMMUNICATOR,
        scorep_thread_get_paradigm(),
        0,
        NULL );
    struct scorep_thread_private_data* tpd = scorep_thread_get_private_data();
    scorep_thread_set_team( tpd, thread_team );

    UTILS_DEBUG_EXIT();
    return SCOREP_SUCCESS;
}

static SCOREP_ErrorCode
create_wait_subsystem_pre_unify( void )
{
    UTILS_DEBUG_ENTRY();
    // todo CR: everything that mentions PTHREAD should come from
    // create_wait_pthread.c. Check fork-join as well.

    /* Count the number of CPU locations locally, we assume that all of them
     * where create/wait threads
     */
    uint32_t n_locations = 0;
    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_BEGIN( &scorep_local_definition_manager,
                                                         Location,
                                                         location )
    {
        if ( definition->location_type != SCOREP_LOCATION_TYPE_CPU_THREAD )
        {
            continue;
        }
        n_locations++;
    }
    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_END();

    /* collect the CPU locations locally */
    uint64_t* location_ids = calloc( n_locations, sizeof( *location_ids ) );
    UTILS_ASSERT( location_ids );
    n_locations = 0;
    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_BEGIN( &scorep_local_definition_manager,
                                                         Location,
                                                         location )
    {
        if ( definition->location_type != SCOREP_LOCATION_TYPE_CPU_THREAD )
        {
            continue;
        }

        location_ids[ n_locations ] = definition->global_location_id;
        n_locations++;
    }
    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_END();

    /* Create the group of all pthread locations in all processes. */
    uint32_t offset = scorep_unify_helper_define_comm_locations(
        SCOREP_GROUP_PTHREAD_LOCATIONS,
        "PTHREAD",
        n_locations,
        location_ids );

    /* Assign this process's pthread locations the global position in the
     * SCOREP_GROUP_PTHREAD_LOCATIONS group */
    for ( uint32_t i = 0; i < n_locations; i++ )
    {
        location_ids[ i ] = i + offset;
    }

    /* In every process, create one local group of all local pthread locations */
    SCOREP_GroupHandle group_handle = SCOREP_Definitions_NewGroup(
        SCOREP_GROUP_PTHREAD_THREAD_TEAM,
        "SCOREP_GROUP_PTHREAD",
        n_locations,
        location_ids );

    /* Define the final communicator over this group */
    SCOREP_CommunicatorHandle communicator_handle =
        SCOREP_Definitions_NewCommunicator(
            group_handle,
            "",
            SCOREP_INVALID_COMMUNICATOR );

    /* Remember this communicator for use in post_unify */
    SCOREP_LOCAL_HANDLE_DEREF( thread_team,
                               InterimCommunicator )->unified =
        communicator_handle;

    return SCOREP_SUCCESS;
}


static SCOREP_ErrorCode
create_wait_subsystem_post_unify( void )
{
    UTILS_DEBUG_ENTRY();
    /* Map the local interim communicator to the unified communicator */
    scorep_local_definition_manager.interim_communicator.mapping[
        SCOREP_LOCAL_HANDLE_DEREF( thread_team,
                                   InterimCommunicator )->sequence_number ] =
        scorep_local_definition_manager.communicator.mapping[
            SCOREP_LOCAL_HANDLE_DEREF( SCOREP_LOCAL_HANDLE_DEREF(
                                           thread_team,
                                           InterimCommunicator )->unified,
                                       Communicator )->sequence_number ];
    return SCOREP_SUCCESS;
}


static void
create_wait_subsystem_finalize( void )
{
    UTILS_DEBUG_ENTRY();
    if ( !is_initialized )
    {
        return;
    }
    is_finalized = true;

    SCOREP_ErrorCode result = SCOREP_MutexDestroy( &thread_create_mutex );
    UTILS_BUG_ON( result != SCOREP_SUCCESS );
}


bool
SCOREP_Thread_InParallel( void )
{
    UTILS_DEBUG_ENTRY();
    if ( !is_initialized || is_finalized )
    {
        return false;
    }

    SCOREP_MutexLock( thread_create_mutex );
    bool in_parallel = ( active_locations > 1 );
    SCOREP_MutexUnlock( thread_create_mutex );
    return in_parallel;
}


void
SCOREP_ThreadCreateWait_Create( SCOREP_ParadigmType                 paradigm,
                                struct scorep_thread_private_data** parent,
                                uint32_t*                           sequenceCount )
{
    UTILS_DEBUG_ENTRY();
    UTILS_BUG_ON( !SCOREP_PARADIGM_TEST_CLASS( paradigm, THREAD_CREATE_WAIT ),
                  "Provided paradigm not of create/wait class " );
    /* We are in the creator thread. */

    struct scorep_thread_private_data* tpd         = scorep_thread_get_private_data();
    struct SCOREP_Location*            location    = scorep_thread_get_location( tpd );
    uint64_t                           timestamp   = scorep_get_timestamp( location );
    SCOREP_InterimCommunicatorHandle   thread_team = scorep_thread_get_team( tpd );

    *parent        = tpd;
    *sequenceCount = scorep_thread_get_next_sequence_count();

    scorep_thread_create_wait_on_create( paradigm,
                                         scorep_thread_get_model_data( tpd ),
                                         location );

    SCOREP_CALL_SUBSTRATE( ThreadCreateWaitCreate, THREAD_CREATE_WAIT_CREATE,
                           ( location, timestamp, paradigm,
                             thread_team, *sequenceCount ) );
}


void
SCOREP_ThreadCreateWait_Wait( SCOREP_ParadigmType paradigm,
                              uint32_t            sequenceCount )
{
    UTILS_DEBUG_ENTRY();
    UTILS_BUG_ON( !SCOREP_PARADIGM_TEST_CLASS( paradigm, THREAD_CREATE_WAIT ),
                  "Provided paradigm not of create/wait class" );

    struct scorep_thread_private_data* tpd         = scorep_thread_get_private_data();
    struct SCOREP_Location*            location    = scorep_thread_get_location( tpd );
    uint64_t                           timestamp   = scorep_get_timestamp( location );
    SCOREP_InterimCommunicatorHandle   thread_team = scorep_thread_get_team( tpd );

    scorep_thread_create_wait_on_wait( paradigm,
                                       scorep_thread_get_model_data( tpd ),
                                       location );

    SCOREP_CALL_SUBSTRATE( ThreadCreateWaitWait, THREAD_CREATE_WAIT_WAIT,
                           ( location, timestamp, paradigm,
                             thread_team, sequenceCount ) );
}


void
SCOREP_ThreadCreateWait_Begin( SCOREP_ParadigmType                paradigm,
                               struct scorep_thread_private_data* parentTpd,
                               uint32_t                           sequenceCount,
                               size_t                             locationReuseKey,
                               SCOREP_Location**                  location )
{
    UTILS_DEBUG_ENTRY();

    UTILS_BUG_ON( !SCOREP_PARADIGM_TEST_CLASS( paradigm, THREAD_CREATE_WAIT ),
                  "Provided paradigm not of create/wait class" );
    UTILS_BUG_ON( parentTpd == 0 );
    /* We are in a new thread. */
    struct scorep_thread_private_data* current_tpd         = 0;
    bool                               location_is_created = false;

    scorep_thread_create_wait_on_begin( paradigm,
                                        parentTpd,
                                        sequenceCount,
                                        locationReuseKey,
                                        &current_tpd,
                                        &location_is_created );
    UTILS_BUG_ON( current_tpd == 0, "Failed to create new scorep_thread_private_data object." );

    SCOREP_Location* parent_location = scorep_thread_get_location( parentTpd );
    *location = scorep_thread_get_location( current_tpd );
    uint64_t                         timestamp   = scorep_get_timestamp( *location );
    SCOREP_InterimCommunicatorHandle thread_team = scorep_thread_get_team( parentTpd );

    if ( location_is_created )
    {
        scorep_subsystems_initialize_location( *location,
                                               parent_location );
    }

    SCOREP_MutexLock( thread_create_mutex );
    active_locations++;
    SCOREP_MutexUnlock( thread_create_mutex );

    scorep_thread_set_team( current_tpd, thread_team );

    /* first notify the subsystem about the comming activation */
    scorep_subsystems_activate_cpu_location( *location,
                                             parent_location,
                                             sequenceCount,
                                             SCOREP_CPU_LOCATION_PHASE_MGMT );

    /* second trigger the begin event. */
    SCOREP_CALL_SUBSTRATE( ThreadCreateWaitBegin, THREAD_CREATE_WAIT_BEGIN,
                           ( *location, timestamp, paradigm,
                             thread_team, sequenceCount ) );

    /* lastly notify the subsystems that the location can create events */
    scorep_subsystems_activate_cpu_location( *location, NULL, 0,
                                             SCOREP_CPU_LOCATION_PHASE_EVENTS );
}


void
SCOREP_ThreadCreateWait_End( SCOREP_ParadigmType                paradigm,
                             struct scorep_thread_private_data* parentTpd,
                             uint32_t                           sequenceCount )
{
    UTILS_DEBUG_ENTRY();

    UTILS_BUG_ON( !SCOREP_PARADIGM_TEST_CLASS( paradigm, THREAD_CREATE_WAIT ),
                  "Provided paradigm not of create/wait class" );
    UTILS_BUG_ON( parentTpd == 0 );

    struct scorep_thread_private_data* current_tpd      = scorep_thread_get_private_data();
    SCOREP_Location*                   current_location = scorep_thread_get_location( current_tpd );
    SCOREP_InterimCommunicatorHandle   thread_team      = scorep_thread_get_team( current_tpd );

    /* first notify the usbsystems about the deactivation of the location. */
    scorep_subsystems_deactivate_cpu_location( current_location,
                                               NULL,
                                               SCOREP_CPU_LOCATION_PHASE_EVENTS );

    /* Second trigger the end event in the substrates. */
    uint64_t timestamp = scorep_get_timestamp( current_location );
    SCOREP_CALL_SUBSTRATE( ThreadCreateWaitEnd, THREAD_CREATE_WAIT_END,
                           ( current_location, timestamp, paradigm,
                             thread_team, sequenceCount ) );

    /* Third deactivate the location. */
    scorep_subsystems_deactivate_cpu_location( current_location,
                                               scorep_thread_get_location( parentTpd ),
                                               SCOREP_CPU_LOCATION_PHASE_MGMT );

    /* Fourth tear down the thread. */
    scorep_thread_create_wait_on_end( paradigm, parentTpd, current_tpd, sequenceCount );

    SCOREP_MutexLock( thread_create_mutex );
    active_locations--;
    SCOREP_MutexUnlock( thread_create_mutex );
    UTILS_DEBUG_EXIT();
}
