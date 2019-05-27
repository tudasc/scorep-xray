/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2014-2016, 2018,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2014-2016, 2019,
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
#include <SCOREP_Thread_Mgmt.h>

#include <scorep_substrates_definition.h>

#include <scorep_subsystem_management.h>
#include <SCOREP_Mutex.h>
#include <SCOREP_Definitions.h>
#include <scorep_events_common.h>
#include <SCOREP_Memory.h>
#include <SCOREP_Paradigms.h>
#include <SCOREP_Properties.h>
#include <scorep_unify_helpers.h>
#include <SCOREP_Task.h>
#include <SCOREP_InMeasurement.h>
#include <SCOREP_Location.h>

#define SCOREP_DEBUG_MODULE_NAME THREAD_CREATE_WAIT
#include <UTILS_Debug.h>

#include <UTILS_Error.h>


#include <stdbool.h>
#include <stdint.h>
#include <inttypes.h>
#include <stddef.h>
#include <string.h>


/* *INDENT-OFF* */
static SCOREP_ErrorCode create_wait_subsystem_register( size_t );
static SCOREP_ErrorCode create_wait_subsystem_init( void );
static void create_wait_subsystem_end( void );
static SCOREP_ErrorCode create_wait_subsystem_pre_unify( void );
static void create_wait_subsystem_finalize( void );
static bool end_latecomer( SCOREP_Location*, void* );
/* *INDENT-ON*  */


static SCOREP_InterimCommunicatorHandle thread_team = SCOREP_INVALID_INTERIM_COMMUNICATOR;
static bool is_initialized;
static bool is_finalized;

static uint32_t     active_locations;
static SCOREP_Mutex thread_create_mutex;
static SCOREP_Mutex thread_subsystem_data_mutex;
static size_t       subsystem_id;

typedef struct subsystem_data
{
    SCOREP_InterimCommunicatorHandle thread_team;
    uint32_t                         sequence_count;
    SCOREP_Location*                 parent;
    SCOREP_ParadigmType              paradigm;

    struct subsystem_data*           next;
} subsystem_data;

static subsystem_data* subsystem_data_free_list;

/** Policies when to reuse a CreateWait location. */
typedef enum create_wait_reuse_policy_type
{
    SCOREP_CREATE_WAIT_REUSE_POLICY_NEVER,
    SCOREP_CREATE_WAIT_REUSE_POLICY_SAME_START_ROUTINE,
    SCOREP_CREATE_WAIT_REUSE_POLICY_ALWAYS
} create_wait_reuse_policy_type;

static create_wait_reuse_policy_type create_wait_reuse_policy;

const SCOREP_Subsystem SCOREP_Subsystem_ThreadCreateWait =
{
    .subsystem_name      = "THREAD CREATE WAIT",
    .subsystem_register  = &create_wait_subsystem_register,
    .subsystem_init      = &create_wait_subsystem_init,
    .subsystem_end       = &create_wait_subsystem_end,
    .subsystem_pre_unify = &create_wait_subsystem_pre_unify,
    .subsystem_finalize  = &create_wait_subsystem_finalize,
};

static SCOREP_ErrorCode
create_wait_subsystem_register( size_t uniqueId )
{
    subsystem_id = uniqueId;
    return SCOREP_SUCCESS;
}

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
    UTILS_BUG_ON( result != SCOREP_SUCCESS, "Failed to create mutex." );
    result = SCOREP_MutexCreate( &thread_subsystem_data_mutex );
    UTILS_BUG_ON( result != SCOREP_SUCCESS, "Failed to create mutex." );

    thread_team = SCOREP_Definitions_NewInterimCommunicator(
        SCOREP_INVALID_INTERIM_COMMUNICATOR,
        scorep_thread_get_paradigm(),
        0,
        NULL );
    struct scorep_thread_private_data* tpd = scorep_thread_get_private_data();
    scorep_thread_set_team( tpd, thread_team );

    /* Evaluate undocumented env vars for location reuse */
    create_wait_reuse_policy = SCOREP_CREATE_WAIT_REUSE_POLICY_NEVER;
    if ( getenv( "SCOREP_THREAD_EXPERIMENTAL_REUSE" ) )
    {
        create_wait_reuse_policy = SCOREP_CREATE_WAIT_REUSE_POLICY_SAME_START_ROUTINE;
        if ( getenv( "SCOREP_THREAD_EXPERIMENTAL_REUSE_ALWAYS" ) )
        {
            create_wait_reuse_policy = SCOREP_CREATE_WAIT_REUSE_POLICY_ALWAYS;
        }
    }

    SCOREP_Paradigms_RegisterParallelParadigm(
        SCOREP_PARADIGM_ORPHAN_THREAD,
        SCOREP_PARADIGM_CLASS_THREAD_CREATE_WAIT,
        "Orphan thread",
        SCOREP_PARADIGM_FLAG_NONE );

    UTILS_DEBUG_EXIT();
    return SCOREP_SUCCESS;
}


static void
create_wait_subsystem_end( void )
{
    /* Create-wait threads might still be active */
    uint64_t timestamp = SCOREP_Timer_GetClockTicks();
    SCOREP_Location_ForAll( end_latecomer, &timestamp );
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
    SCOREP_LOCAL_HANDLE_DEREF( thread_team,
                               InterimCommunicator )->unified =
        SCOREP_Definitions_NewCommunicator(
            group_handle,
            SCOREP_INVALID_STRING,
            SCOREP_INVALID_COMMUNICATOR,
            0 );

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
    UTILS_BUG_ON( result != SCOREP_SUCCESS, "Failed to destroy mutex." );
    result = SCOREP_MutexDestroy( &thread_subsystem_data_mutex );
    UTILS_BUG_ON( result != SCOREP_SUCCESS, "Failed to destroy mutex." );
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

    scorep_thread_create_wait_on_create( scorep_thread_get_model_data( tpd ),
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

    scorep_thread_create_wait_on_wait( scorep_thread_get_model_data( tpd ),
                                       location );

    SCOREP_CALL_SUBSTRATE( ThreadCreateWaitWait, THREAD_CREATE_WAIT_WAIT,
                           ( location, timestamp, paradigm,
                             thread_team, sequenceCount ) );
}


uintptr_t
scorep_thread_create_wait_get_reuse_key( SCOREP_ParadigmType paradigm,
                                         uintptr_t           startRoutine )
{
    UTILS_DEBUG_ENTRY();
    UTILS_BUG_ON( !SCOREP_PARADIGM_TEST_CLASS( paradigm, THREAD_CREATE_WAIT ),
                  "Provided paradigm not of create/wait class" );

    if ( create_wait_reuse_policy == SCOREP_CREATE_WAIT_REUSE_POLICY_NEVER )
    {
        return 0;
    }
    switch ( paradigm )
    {
        case SCOREP_PARADIGM_PTHREAD:
            switch ( create_wait_reuse_policy )
            {
                case SCOREP_CREATE_WAIT_REUSE_POLICY_SAME_START_ROUTINE:
                    /* When we use the address of the start routine as the key,
                     * only threads with the same start routine may share the same
                     * locations.
                     */
                    return startRoutine;
                case SCOREP_CREATE_WAIT_REUSE_POLICY_ALWAYS:
                    /* Using an arbitrary key different than 0 will reuse locations
                     * also for threads which have different start routine.
                     */
                    return 1;
                default:
                    UTILS_FATAL( "Invalid reuse-policy." );
                    return 0;
            }
        case SCOREP_PARADIGM_ORPHAN_THREAD:
            /* Use a value different from SCOREP_PARADIGM_PTHREAD to prevent
             * recording events from different paradigms into one location.
             */
            return 2;
        default:
            UTILS_FATAL( "Unsupported paradigm." );
            return 0;
    }
}

static subsystem_data*
get_subsystem_data_from_pool( void )
{
    subsystem_data* data;
    if ( subsystem_data_free_list != NULL )
    {
        data                     = subsystem_data_free_list;
        subsystem_data_free_list = subsystem_data_free_list->next;
    }
    else
    {
        data = SCOREP_Memory_AllocForMisc( sizeof( subsystem_data ) );
    }
    memset( data, 0, sizeof( subsystem_data ) );
    return data;
}


static void
release_subsystem_data_to_pool( subsystem_data* data )
{
    data->next               = subsystem_data_free_list;
    subsystem_data_free_list = data;
}


static void
init_thread( SCOREP_InterimCommunicatorHandle team,
             uint32_t                         sequenceCount,
             SCOREP_Location*                 location,
             SCOREP_Location*                 parentLocation,
             SCOREP_ParadigmType              paradigm )
{
    SCOREP_MutexLock( thread_create_mutex );
    active_locations++;
    subsystem_data* data = get_subsystem_data_from_pool();
    SCOREP_MutexUnlock( thread_create_mutex );

    data->thread_team    = team;
    data->sequence_count = sequenceCount;
    data->parent         = parentLocation;
    data->paradigm       = paradigm;

    /* Locking here probably unnecessary */
    SCOREP_MutexLock( thread_subsystem_data_mutex );
    SCOREP_Location_SetSubsystemData( location, subsystem_id, ( void* )data );
    SCOREP_MutexUnlock( thread_subsystem_data_mutex );
}


void
SCOREP_ThreadCreateWait_Begin( SCOREP_ParadigmType                paradigm,
                               struct scorep_thread_private_data* parentTpd,
                               uint32_t                           sequenceCount,
                               uintptr_t                          startRoutine,
                               SCOREP_Location**                  location )
{
    UTILS_DEBUG_ENTRY();

    UTILS_BUG_ON( !SCOREP_PARADIGM_TEST_CLASS( paradigm, THREAD_CREATE_WAIT ),
                  "Provided paradigm not of create/wait class" );
    UTILS_BUG_ON( parentTpd == 0 );
    /* We are in a new thread. */
    struct scorep_thread_private_data* current_tpd         = 0;
    bool                               location_is_created = false;

    scorep_thread_create_wait_on_begin( parentTpd,
                                        sequenceCount,
                                        scorep_thread_create_wait_get_reuse_key( paradigm, startRoutine ),
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

    init_thread( thread_team, sequenceCount, *location, parent_location, paradigm );

    scorep_thread_set_team( current_tpd, thread_team );

    /* first notify the subsystem about the coming activation */
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


static void
terminate_thread( void* data )
{
    SCOREP_MutexLock( thread_create_mutex );
    active_locations--;
    release_subsystem_data_to_pool( data );
    SCOREP_MutexUnlock( thread_create_mutex );
}


void
SCOREP_ThreadCreateWait_End( SCOREP_ParadigmType                paradigm,
                             struct scorep_thread_private_data* parentTpd,
                             uint32_t                           sequenceCount,
                             void*                              terminate )
{
    UTILS_DEBUG_ENTRY();

    UTILS_BUG_ON( !SCOREP_PARADIGM_TEST_CLASS( paradigm, THREAD_CREATE_WAIT ),
                  "Provided paradigm not of create/wait class" );
    UTILS_BUG_ON( parentTpd == 0 );

    struct scorep_thread_private_data* current_tpd      = scorep_thread_get_private_data();
    SCOREP_Location*                   current_location = scorep_thread_get_location( current_tpd );
    SCOREP_InterimCommunicatorHandle   thread_team      = scorep_thread_get_team( current_tpd );

    /* first notify the subsystems about the deactivation of the location. */
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
    scorep_thread_create_wait_on_end( parentTpd, current_tpd, sequenceCount );

    terminate_thread( terminate );

    UTILS_DEBUG_EXIT();
}


void*
SCOREP_ThreadCreateWait_TryTerminate( struct SCOREP_Location* location )
{
    /* Take this locations subsystem data as an indicator if this location
     * needs to be terminated. Ensure that only one caller gets the OK to
     * terminate the thread. */
    UTILS_BUG_ON( is_finalized,
                  "Illegal call to SCOREP_ThreadCreateWait_TryTerminate. Measurement system has already terminated." );

    SCOREP_MutexLock( thread_subsystem_data_mutex );
    void* terminate = SCOREP_Location_GetSubsystemData( location, subsystem_id );
    SCOREP_Location_SetSubsystemData( location, subsystem_id, NULL );
    SCOREP_MutexUnlock( thread_subsystem_data_mutex );
    return terminate;
}


void
scorep_thread_create_wait_orphan_begin( SCOREP_Location** location )
{
    UTILS_DEBUG_ENTRY();

    /* We are in a new thread. */
    struct scorep_thread_private_data* current_tpd         = 0;
    bool                               location_is_created = false;

    scorep_thread_create_wait_on_orphan_begin( &current_tpd,
                                               &location_is_created );
    UTILS_BUG_ON( current_tpd == 0, "Failed to create new scorep_thread_private_data object." );

    SCOREP_Location* parent_location = NULL; /* scorep_thread_get_location( parentTpd ); */
    *location = scorep_thread_get_location( current_tpd );
    uint64_t                         timestamp   = scorep_get_timestamp( *location );
    SCOREP_InterimCommunicatorHandle thread_team = scorep_thread_get_team( SCOREP_Thread_GetInitialTpd() );

    if ( location_is_created )
    {
        scorep_subsystems_initialize_location( *location,
                                               parent_location );
    }

    init_thread( thread_team, SCOREP_THREAD_INVALID_SEQUENCE_COUNT, *location, NULL, SCOREP_PARADIGM_ORPHAN_THREAD );

    scorep_thread_set_team( current_tpd, thread_team );

    /* first notify the subsystem about the coming activation */
    scorep_subsystems_activate_cpu_location( *location,
                                             parent_location,
                                             SCOREP_THREAD_INVALID_SEQUENCE_COUNT,
                                             SCOREP_CPU_LOCATION_PHASE_MGMT );

    /* second trigger the begin event. */
    SCOREP_CALL_SUBSTRATE( ThreadCreateWaitBegin, THREAD_CREATE_WAIT_BEGIN,
                           ( *location, timestamp, SCOREP_PARADIGM_ORPHAN_THREAD,
                             thread_team, SCOREP_THREAD_INVALID_SEQUENCE_COUNT ) );

    /* lastly notify the subsystems that the location can create events */
    scorep_subsystems_activate_cpu_location( *location, NULL, 0,
                                             SCOREP_CPU_LOCATION_PHASE_EVENTS );
}


void
scorep_thread_create_wait_orphan_end( void* terminate )
{
    UTILS_DEBUG_ENTRY();

    struct scorep_thread_private_data* current_tpd      = scorep_thread_get_private_data();
    SCOREP_Location*                   current_location = scorep_thread_get_location( current_tpd );
    SCOREP_InterimCommunicatorHandle   thread_team      = scorep_thread_get_team( current_tpd );

    /* first notify the subsystems about the deactivation of the location. */
    scorep_subsystems_deactivate_cpu_location( current_location,
                                               NULL,
                                               SCOREP_CPU_LOCATION_PHASE_EVENTS );

    /* Second trigger the end event in the substrates. */
    uint64_t timestamp = scorep_get_timestamp( current_location );
    SCOREP_CALL_SUBSTRATE( ThreadCreateWaitEnd, THREAD_CREATE_WAIT_END,
                           ( current_location, timestamp, SCOREP_PARADIGM_ORPHAN_THREAD,
                             thread_team, SCOREP_THREAD_INVALID_SEQUENCE_COUNT ) );

    /* Third deactivate the location. */
    scorep_subsystems_deactivate_cpu_location( current_location,
                                               NULL, /* parentLocation */
                                               SCOREP_CPU_LOCATION_PHASE_MGMT );

    /* Fourth tear down the thread. */
    scorep_thread_create_wait_on_orphan_end( current_tpd );

    terminate_thread( terminate );

    UTILS_DEBUG_EXIT();
}


static bool
end_latecomer( SCOREP_Location* location, void* payload )
{
    void* terminate = SCOREP_ThreadCreateWait_TryTerminate( location );
    if ( terminate )
    {
        uint64_t timestamp = *( uint64_t* )payload;
        /* timestamp was taken by master thread's clock. location's clock might deviate. */
        uint64_t last_timestamp = SCOREP_Location_GetLastTimestamp( location );
        if ( timestamp < last_timestamp )
        {
            timestamp = last_timestamp;
        }
        UTILS_WARNING( "Thread after main (location=%" PRIu32 ")", SCOREP_Location_GetId( location ) );

        SCOREP_Location_Task_ExitAllRegions( location, SCOREP_Task_GetCurrentTask( location ), timestamp );

        scorep_subsystems_deactivate_cpu_location( location, NULL, SCOREP_CPU_LOCATION_PHASE_EVENTS );

        subsystem_data* data = terminate;
        SCOREP_CALL_SUBSTRATE( ThreadCreateWaitEnd, THREAD_CREATE_WAIT_END,
                               ( location, timestamp, data->paradigm,
                                 data->thread_team, data->sequence_count ) );

        scorep_subsystems_deactivate_cpu_location( location, data->parent, SCOREP_CPU_LOCATION_PHASE_MGMT );

        terminate_thread( terminate );
    }
    return false;
}
