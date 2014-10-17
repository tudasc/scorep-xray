/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2013,
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
#include "scorep_thread_fork_join_team.h"

#include <scorep_thread_model_specific.h>
#include <SCOREP_Memory.h>

#define SCOREP_DEBUG_MODULE_NAME THREAD_FORK_JOIN
#include <UTILS_Debug.h>

#include <jenkins_hash.h>


#define THREAD_TEAM_HASH_POWER 6

struct scorep_thread_team_data*
scorep_thread_fork_join_create_team_data( struct SCOREP_Location* location )
{
    UTILS_DEBUG_ENTRY();
    struct scorep_thread_team_data* data = SCOREP_Location_AllocForMisc(
        location,
        sizeof( *data ) );
    scorep_definitions_manager_init_entry( &data->thread_team );
    /* don't use scorep_definitions_manager_entry_alloc_hash_table, it uses calloc */
    data->thread_team.hash_table_mask = hashmask( THREAD_TEAM_HASH_POWER );
    data->thread_team.hash_table      = SCOREP_Location_AllocForMisc(
        location,
        hashsize( THREAD_TEAM_HASH_POWER ) * sizeof( *data->thread_team.hash_table ) );

    return data;
}


static uint32_t
init_payload_fn( void* payload_, uint32_t hashValue, va_list va )
{
    struct scorep_thread_team_comm_payload* payload = payload_;

    payload->num_threads = va_arg( va, uint32_t );
    hashValue            = jenkins_hash( &payload->num_threads,
                                         sizeof( payload->num_threads ),
                                         hashValue );

    payload->thread_num = va_arg( va, uint32_t );
    /* do not hash the thread_num, we use the hash to look-up team members */

    /* Not used to identify this thread team. */
    payload->singleton_counter = 0;

    return hashValue;
}


static bool
equal_payloads_fn( const void* payloadA_,
                   const void* payloadB_ )
{
    const struct scorep_thread_team_comm_payload* payloadA = payloadA_;
    const struct scorep_thread_team_comm_payload* payloadB = payloadB_;

    return payloadA->num_threads == payloadB->num_threads
           && payloadA->thread_num == payloadB->thread_num;
}


SCOREP_InterimCommunicatorHandle
scorep_thread_get_team_handle( struct SCOREP_Location*          location,
                               SCOREP_InterimCommunicatorHandle parentThreadTeam,
                               uint32_t                         numThreads,
                               uint32_t                         threadNum )
{
    struct scorep_thread_team_data* data =
        SCOREP_Location_GetSubsystemData( location, scorep_thread_fork_join_subsystem_id );

    if ( parentThreadTeam != SCOREP_INVALID_INTERIM_COMMUNICATOR )
    {
        struct scorep_thread_team_comm_payload* payload =
            SCOREP_InterimCommunicatorHandle_GetPayload( parentThreadTeam );
        if ( payload->num_threads == 1 && numThreads == 1 )
        {
            /*
             * The parent thread team definition is our own, thus it is
             * thread safe to increment singleton_counter here.
             */
            payload->singleton_counter++;
            return parentThreadTeam;
        }
    }

    struct scorep_thread_team_comm_payload* new_payload;
    SCOREP_InterimCommunicatorHandle        new_handle =
        SCOREP_Definitions_NewInterimCommunicatorCustom(
            location,
            &data->thread_team,
            init_payload_fn,
            equal_payloads_fn,
            parentThreadTeam,
            scorep_thread_get_paradigm(),
            sizeof( *new_payload ),
            ( void** )&new_payload,
            numThreads,
            threadNum );

    if ( new_payload )
    {
        /* we encountered this thread team the first time */
        if ( threadNum == 0 )
        {
            data->team_leader_counter++;
        }
    }

    return new_handle;
}

SCOREP_InterimCommunicatorHandle
scorep_thread_get_parent_team_handle( SCOREP_InterimCommunicatorHandle threadHandle )
{
    struct scorep_thread_team_comm_payload* payload =
        SCOREP_InterimCommunicatorHandle_GetPayload( threadHandle );
    if ( payload->num_threads == 1 && payload->singleton_counter > 0 )
    {
        payload->singleton_counter--;
        return threadHandle;
    }
    return SCOREP_InterimCommunicatorHandle_GetParent( threadHandle );
}
