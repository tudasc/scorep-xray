/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2013, 2015-2016, 2019,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2015, 2018,
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
 *
 */


/**
 * @file
 *
 *
 */


#include <config.h>
#include <SCOREP_Thread_Mgmt.h>
#include "scorep_thread_fork_join_team.h"
#include <scorep_thread_model_specific.h>

#include <scorep_location_management.h>
#include <scorep_unify_helpers.h>
#include <scorep_ipc.h>
#include <SCOREP_Subsystem.h>
#include <SCOREP_Paradigms.h>

#include <UTILS_Error.h>
#define SCOREP_DEBUG_MODULE_NAME UNIFY
#include <UTILS_Debug.h>


size_t scorep_thread_fork_join_subsystem_id;


static uint32_t
define_fork_join_locations( uint32_t* local_to_thread_id )
{
    /* count the number of Open MP locations locally */
    uint32_t number_of_locations = 0;
    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_BEGIN( &scorep_local_definition_manager,
                                                         Location,
                                                         location )
    {
        if ( definition->location_type != SCOREP_LOCATION_TYPE_CPU_THREAD )
        {
            continue;
        }
        number_of_locations++;
    }
    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_END();

    /* collect the Open MP locations locally */
    uint64_t* my_locations;
    my_locations = calloc( number_of_locations, sizeof( *my_locations ) );
    UTILS_ASSERT( my_locations );
    number_of_locations = 0;
    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_BEGIN( &scorep_local_definition_manager,
                                                         Location,
                                                         location )
    {
        local_to_thread_id[ definition->sequence_number ] = UINT32_MAX;
        if ( definition->location_type != SCOREP_LOCATION_TYPE_CPU_THREAD )
        {
            continue;
        }

        my_locations[ number_of_locations ]               = definition->global_location_id;
        local_to_thread_id[ definition->sequence_number ] = number_of_locations;
        number_of_locations++;
    }
    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_END();

    /* Collectively define the group of the locations */
    SCOREP_GroupType group;
    const char*      group_name;
    switch ( scorep_thread_get_paradigm() )
    {
        case SCOREP_PARADIGM_OPENMP:
            group      = SCOREP_GROUP_OPENMP_LOCATIONS;
            group_name = "OpenMP";
            break;
        default:
            UTILS_BUG( "Fork-join threading component provided invalid paradigm: %u",
                       scorep_thread_get_paradigm() );
    }
    uint32_t offset_to_global = scorep_unify_helper_define_comm_locations(
        group,
        group_name,
        number_of_locations,
        my_locations );

    /* shift my thread ids to the global ids by applying my offset */
    for ( uint32_t i = 0; i < scorep_local_definition_manager.location.counter; i++ )
    {
        if ( local_to_thread_id[ i ] != UINT32_MAX )
        {
            local_to_thread_id[ i ] += offset_to_global;
        }
    }

    return number_of_locations;
}


static bool
count_total_thread_teams( SCOREP_Location* location,
                          void*            arg )
{
    uint32_t* total_thread_teams = arg;

    if ( SCOREP_Location_GetType( location ) != SCOREP_LOCATION_TYPE_CPU_THREAD )
    {
        return false;
    }
    struct scorep_thread_team_data* data =
        SCOREP_Location_GetSubsystemData( location, scorep_thread_fork_join_subsystem_id );
    *total_thread_teams += data->team_leader_counter;
    UTILS_DEBUG( "Location %u on rank %u was in %u team(s) the leader:",
                 SCOREP_Location_GetId( location ),
                 SCOREP_Ipc_GetRank(),
                 data->team_leader_counter );

    return false;
}


static bool
find_next_thread_team( SCOREP_Location* location,
                       void*            args )
{
    SCOREP_InterimCommunicatorHandle* next_team_leader = args;

    if ( SCOREP_Location_GetType( location ) != SCOREP_LOCATION_TYPE_CPU_THREAD )
    {
        return false;
    }
    struct scorep_thread_team_data* data =
        SCOREP_Location_GetSubsystemData( location, scorep_thread_fork_join_subsystem_id );

    /* Search in the definitions for a thread team canditate */
    SCOREP_Allocator_PageManager* page_manager =
        SCOREP_Location_GetOrCreateMemoryPageManager( location,
                                                      SCOREP_MEMORY_TYPE_DEFINITIONS );
    SCOREP_DEFINITIONS_MANAGER_ENTRY_FOREACH_DEFINITION_BEGIN(
        &data->thread_team,
        InterimCommunicator,
        page_manager )
    {
        if ( !SCOREP_PARADIGM_TEST_CLASS( definition->paradigm_type, THREAD_FORK_JOIN ) )
        {
            /*
             * unlikely, but who knows, maybe we have one day a manager entry
             * directly in the location
             */
            continue;
        }

        struct scorep_thread_team_comm_payload* payload =
            SCOREP_InterimCommunicatorHandle_GetPayload( handle );

        /* Skip non-team-leader teams */
        if ( payload->thread_num != 0 )
        {
            continue;
        }

        /* Skip already processed teams */
        if ( definition->unified != SCOREP_INVALID_COMMUNICATOR )
        {
            continue;
        }

        /*
         * topological sorting, either no parent at all,
         * or parent is already 'unified', i.e. the Communicator definition
         * was triggered, we re-use the 'unified' field for this,
         * but skip already handled teams without a parent
         */
        if ( definition->parent_handle == SCOREP_INVALID_INTERIM_COMMUNICATOR )
        {
            if ( definition->unified == SCOREP_INVALID_COMMUNICATOR )
            {
                /* not yet unified */
                *next_team_leader = handle;
                return true;
            }
        }
        else
        {
            /* check if parent is already unified */
            SCOREP_InterimCommunicatorDef* parent =
                SCOREP_Allocator_GetAddressFromMovableMemory(
                    page_manager,
                    definition->parent_handle );
            if ( parent->unified != SCOREP_INVALID_COMMUNICATOR )
            {
                /* not yet unified */
                *next_team_leader = handle;
                return true;
            }
        }
    }
    SCOREP_DEFINITIONS_MANAGER_ENTRY_FOREACH_DEFINITION_END();

    return false;
}


static bool
find_thread_team_members( SCOREP_Location* location,
                          void*            arg )
{
    void**                           args               = arg;
    SCOREP_InterimCommunicatorHandle team_leader_handle =
        *( SCOREP_InterimCommunicatorHandle* )args[ 0 ];
    uint32_t* local_to_thread_id  = args[ 1 ];
    uint64_t* thread_team_members = args[ 2 ];

    SCOREP_Allocator_PageManager* page_manager =
        SCOREP_Location_GetOrCreateMemoryPageManager( location,
                                                      SCOREP_MEMORY_TYPE_DEFINITIONS );

    SCOREP_InterimCommunicatorDef* team_leader =
        SCOREP_Allocator_GetAddressFromMovableMemory(
            page_manager,
            team_leader_handle );
    struct scorep_thread_team_comm_payload* team_leader_payload =
        SCOREP_InterimCommunicatorHandle_GetPayload( team_leader_handle );

    if ( SCOREP_Location_GetType( location ) != SCOREP_LOCATION_TYPE_CPU_THREAD )
    {
        return false;
    }
    struct scorep_thread_team_data* data =
        SCOREP_Location_GetSubsystemData( location, scorep_thread_fork_join_subsystem_id );

    SCOREP_InterimCommunicatorHandle* hash_table_bucket =
        &data->thread_team.hash_table[ team_leader->hash_value & data->thread_team.hash_table_mask ];
    SCOREP_InterimCommunicatorHandle thread_team_handle = *hash_table_bucket;
    while ( thread_team_handle != SCOREP_INVALID_INTERIM_COMMUNICATOR )
    {
        SCOREP_InterimCommunicatorDef* thread_team =
            SCOREP_Allocator_GetAddressFromMovableMemory(
                page_manager,
                thread_team_handle );
        struct scorep_thread_team_comm_payload* thread_team_payload =
            SCOREP_InterimCommunicatorHandle_GetPayload( thread_team_handle );

        /*
         * are these two thread team definitions the same
         * except for the thread num in the teams
         */
        if ( team_leader->hash_value == thread_team->hash_value
             && team_leader->parent_handle == thread_team->parent_handle
             && team_leader->paradigm_type  == thread_team->paradigm_type
             && team_leader_payload->num_threads == thread_team_payload->num_threads )
        {
            /* this location was in this thread team */
            if ( thread_team_payload->thread_num != 0 )
            {
                /* remember the team leader of this thread team */
                thread_team->unified = team_leader_handle;
            }
            thread_team_members[ thread_team_payload->thread_num ] =
                local_to_thread_id[ SCOREP_Location_GetId( location ) ];

            UTILS_DEBUG( " Location %u is thread %u in team %u{%u, %u, %u}",
                         SCOREP_Location_GetId( location ),
                         thread_team_payload->thread_num,
                         team_leader->sequence_number,
                         SCOREP_Definitions_HandleToId( team_leader->name_handle ),
                         SCOREP_Definitions_HandleToId( team_leader->parent_handle ),
                         team_leader_payload->num_threads );

            return false;
        }

        thread_team_handle = thread_team->hash_next;
    }

    return false;
}


static bool
redirect_unified_to_collated( SCOREP_Location* location,
                              void*            arg )
{
    if ( SCOREP_Location_GetType( location ) != SCOREP_LOCATION_TYPE_CPU_THREAD )
    {
        return false;
    }
    struct scorep_thread_team_data* data =
        SCOREP_Location_GetSubsystemData( location, scorep_thread_fork_join_subsystem_id );

    SCOREP_Allocator_PageManager* page_manager =
        SCOREP_Location_GetOrCreateMemoryPageManager( location,
                                                      SCOREP_MEMORY_TYPE_DEFINITIONS );
    SCOREP_DEFINITIONS_MANAGER_ENTRY_FOREACH_DEFINITION_BEGIN(
        &data->thread_team,
        InterimCommunicator,
        page_manager )
    {
        if ( !SCOREP_PARADIGM_TEST_CLASS( definition->paradigm_type, THREAD_FORK_JOIN ) )
        {
            /*
             * unlikely, but who knows, maybe we have one day a manager entry
             * directly in the location
             */
            continue;
        }

        struct scorep_thread_team_comm_payload* payload =
            SCOREP_InterimCommunicatorHandle_GetPayload( handle );

        UTILS_BUG_ON( definition->unified == SCOREP_INVALID_COMMUNICATOR,
                      "Thread team collation missed a thread team" );

        /* For team-leader, the 'unified' already points to the collated def */
        if ( payload->thread_num == 0 )
        {
            continue;
        }

        /*
         * 'unified' points to the interim comm def of the team-leader,
         * though his 'unified' points to the collated def.
         */
        SCOREP_InterimCommunicatorDef* team_leader =
            SCOREP_LOCAL_HANDLE_DEREF(
                definition->unified,
                InterimCommunicator );
        definition->unified = team_leader->unified;
    }
    SCOREP_DEFINITIONS_MANAGER_ENTRY_FOREACH_DEFINITION_END();

    return false;
}


static bool
create_mapping( SCOREP_Location* location,
                void*            arg )
{
    uint32_t* total_thread_teams = arg;

    if ( SCOREP_Location_GetType( location ) != SCOREP_LOCATION_TYPE_CPU_THREAD )
    {
        return false;
    }
    struct scorep_thread_team_data* data =
        SCOREP_Location_GetSubsystemData( location, scorep_thread_fork_join_subsystem_id );
    scorep_unify_helper_create_interim_comm_mapping( &data->thread_team );

    return false;
}

static SCOREP_ErrorCode
fork_join_subsystem_pre_unify( void )
{
    uint32_t local_to_thread_id[ scorep_local_definition_manager.location.counter ];

    uint32_t max_number_of_threads = define_fork_join_locations( local_to_thread_id );

    uint64_t thread_team_members[ max_number_of_threads ];

    uint32_t total_thread_teams = 0;
    SCOREP_Location_ForAll( count_total_thread_teams, &total_thread_teams );

    uint32_t i = 0;
    while ( i < total_thread_teams )
    {
        SCOREP_InterimCommunicatorHandle current_team_leader_handle =
            SCOREP_INVALID_INTERIM_COMMUNICATOR;
        SCOREP_Location_ForAll( find_next_thread_team, &current_team_leader_handle );
        UTILS_BUG_ON( current_team_leader_handle == SCOREP_INVALID_INTERIM_COMMUNICATOR,
                      "There should be %u more thread teams!", total_thread_teams - i );

        SCOREP_InterimCommunicatorDef* current_team_leader =
            SCOREP_LOCAL_HANDLE_DEREF(
                current_team_leader_handle,
                InterimCommunicator );
        struct scorep_thread_team_comm_payload* current_team_leader_payload =
            SCOREP_InterimCommunicatorHandle_GetPayload( current_team_leader_handle );

        UTILS_DEBUG( "Next thread team %u{%u, %u, %u}",
                     current_team_leader->sequence_number,
                     SCOREP_Definitions_HandleToId( current_team_leader->name_handle ),
                     SCOREP_Definitions_HandleToId( current_team_leader->parent_handle ),
                     current_team_leader_payload->num_threads );

        /*
         * Now collect all locations which also have this thread team in its
         * definitions, note that the thread_num is not in the hash value,
         * thus we can use current_team_leader as the key for looking-up
         * in the locations hash tables.
         *
         * Fill the `thread_team_members` array with the global thread ids.
         */
        void* args[ 3 ] =
        {
            &current_team_leader_handle,
            local_to_thread_id,
            thread_team_members
        };
        SCOREP_Location_ForAll( find_thread_team_members, args );

        /* Now we can trigger the group definition */
        SCOREP_GroupType group;
        switch ( scorep_thread_get_paradigm() )
        {
            case SCOREP_PARADIGM_OPENMP:
                group = SCOREP_GROUP_OPENMP_THREAD_TEAM;
                break;
            default:
                UTILS_BUG( "Fork-join threading component provided invalid paradigm: %u",
                           scorep_thread_get_paradigm() );
        }
        SCOREP_GroupHandle group_handle =
            SCOREP_Definitions_NewGroup(
                group,
                "",
                current_team_leader_payload->num_threads,
                thread_team_members );

        /* resolve the parent thread team to the unified definition */
        SCOREP_CommunicatorHandle parent_handle = SCOREP_INVALID_COMMUNICATOR;
        if ( current_team_leader->parent_handle != SCOREP_INVALID_INTERIM_COMMUNICATOR )
        {
            SCOREP_InterimCommunicatorDef* parent_thread_team =
                SCOREP_LOCAL_HANDLE_DEREF(
                    current_team_leader->parent_handle,
                    InterimCommunicator );
            parent_handle = parent_thread_team->unified;
            struct scorep_thread_team_comm_payload* parent_thread_team_payload =
                SCOREP_InterimCommunicatorHandle_GetPayload( current_team_leader->parent_handle );
            if ( parent_thread_team_payload->thread_num != 0 )
            {
                /*
                 * The parent was not the team leader, unified points to the
                 * team leader though.
                 */
                parent_thread_team = SCOREP_LOCAL_HANDLE_DEREF(
                    parent_handle,
                    InterimCommunicator );
                parent_handle = parent_thread_team->unified;
            }
        }

        /*
         * Now we can define the communicator itself, and strore the handle in
         * the `unified` field of the thread team
         */
        current_team_leader->unified =
            SCOREP_Definitions_NewCommunicator( group_handle,
                                                current_team_leader->name_handle,
                                                parent_handle,
                                                0 );

        i++;
    }

    /*
     * Now redirect the 'unified' member of the non-team-leader defs to the
     * collated definition. This ensure, that all 'unified' members point to
     * the collated def.
     */
    SCOREP_Location_ForAll( redirect_unified_to_collated, NULL );

    return SCOREP_SUCCESS;
}

static SCOREP_ErrorCode
fork_join_subsystem_post_unify( void )
{
    SCOREP_Location_ForAll( create_mapping, NULL );

    return SCOREP_SUCCESS;
}


static SCOREP_ErrorCode
fork_join_subsystem_register( size_t subsystem_id )
{
    scorep_thread_fork_join_subsystem_id = subsystem_id;
    return SCOREP_SUCCESS;
}


static SCOREP_ErrorCode
fork_join_subsystem_init_location( SCOREP_Location* location,
                                   SCOREP_Location* parent )
{
    if ( SCOREP_LOCATION_TYPE_CPU_THREAD == SCOREP_Location_GetType( location ) )
    {
        SCOREP_Location_SetSubsystemData( location,
                                          scorep_thread_fork_join_subsystem_id,
                                          scorep_thread_fork_join_create_team_data( location ) );
    }
    return SCOREP_SUCCESS;
}


const SCOREP_Subsystem SCOREP_Subsystem_ThreadForkJoin =
{
    .subsystem_name          = "THREAD FORK JOIN",
    .subsystem_register      = &fork_join_subsystem_register,
    .subsystem_init_location = &fork_join_subsystem_init_location,
    .subsystem_pre_unify     = &fork_join_subsystem_pre_unify,
    .subsystem_post_unify    = &fork_join_subsystem_post_unify,
};
