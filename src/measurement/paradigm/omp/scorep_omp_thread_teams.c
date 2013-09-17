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
 * Copyright (c) 2009-2013,
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

#include "scorep_omp_thread_teams.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <inttypes.h>

#include <UTILS_Error.h>
#define SCOREP_DEBUG_MODULE_NAME OPENMP
#include <UTILS_Debug.h>

#include <jenkins_hash.h>

#include <scorep_location.h>
#include <definitions/SCOREP_Definitions.h>


#define THREAD_TEAM_HASH_POWER 6


SCOREP_ErrorCode
scorep_omp_create_location_data( SCOREP_Location* location )
{
    UTILS_DEBUG_ENTRY();
    struct scorep_omp_thread_team_data* data = SCOREP_Location_AllocForMisc(
        location,
        sizeof( *data ) );
    scorep_definitions_manager_init_entry( &data->thread_team );
    /* don't use scorep_definition_manager_entry_alloc_hash_table, it uses calloc */
    data->thread_team.hash_table_mask = hashmask( THREAD_TEAM_HASH_POWER );
    data->thread_team.hash_table      = SCOREP_Location_AllocForMisc(
        location,
        hashsize( THREAD_TEAM_HASH_POWER ) * sizeof( *data->thread_team.hash_table ) );

    SCOREP_Location_SetThreadTeamData( location,
                                       data );
    return SCOREP_SUCCESS;
}


void
scorep_omp_destroy_location_data( SCOREP_Location* location )
{
    UTILS_DEBUG_ENTRY();
    /* data is allocated in the page manager, nothing to free */
    SCOREP_Location_SetThreadTeamData( location,
                                       NULL );
}


static uint32_t
init_payload_fn( void* payload_, uint32_t hashValue, va_list va )
{
    struct scorep_omp_comm_payload* payload = payload_;

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
    const struct scorep_omp_comm_payload* payloadA = payloadA_;
    const struct scorep_omp_comm_payload* payloadB = payloadB_;

    return payloadA->num_threads == payloadB->num_threads
           && payloadA->thread_num == payloadB->thread_num;
}


SCOREP_InterimCommunicatorHandle
scorep_omp_get_thread_team_handle( SCOREP_Location*                 location,
                                   SCOREP_InterimCommunicatorHandle parentThreadTeam,
                                   uint32_t                         numThreads,
                                   uint32_t                         threadNum )
{
    struct scorep_omp_thread_team_data* data =
        SCOREP_Location_GetThreadTeamData( location );

    if ( parentThreadTeam != SCOREP_INVALID_INTERIM_COMMUNICATOR )
    {
        struct scorep_omp_comm_payload* payload =
            SCOREP_InterimCommunicatorHandle_GetPayload( parentThreadTeam );
        if ( payload->num_threads == 1 && numThreads == 1 )
        {
            /*
             * The parent thread team definition is our own, thus it is
             * thread safe to inrement singleton_counter here.
             */
            payload->singleton_counter++;
            return parentThreadTeam;
        }
    }

    struct scorep_omp_comm_payload*  new_payload;
    SCOREP_InterimCommunicatorHandle new_handle =
        SCOREP_Definitions_NewInterimCommunicatorInLocation(
            location,
            parentThreadTeam,
            SCOREP_PARADIGM_OPENMP,
            init_payload_fn,
            equal_payloads_fn,
            &data->thread_team,
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
scorep_omp_get_parent_thread_team_handle( SCOREP_InterimCommunicatorHandle threadHandle )
{
    struct scorep_omp_comm_payload* payload =
        SCOREP_InterimCommunicatorHandle_GetPayload( threadHandle );
    if ( payload->num_threads == 1 && payload->singleton_counter > 0 )
    {
        payload->singleton_counter--;
        return threadHandle;
    }
    return SCOREP_InterimCommunicatorHandle_GetParent( threadHandle );
}
