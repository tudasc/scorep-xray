/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2016,
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

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>

#include <scorep_ipc.h>
#include <scorep_mpi_communicator_mgmt.h>
#include <SCOREP_Definitions.h>
#include <SCOREP_Definitions.h>
#include <SCOREP_Memory.h>

#include <UTILS_Error.h>

/**
 * Unifies the communicator ids. The (root, local_id) pair is already unique.
 * Arrange all ids in an array sorted by root_process, and second by local id.
 * Thus, the sum of communicator definitions on ranks with a lower number
 * gives the offset which is added to the local id to get the unified global id.
 * Self-communicators are appended to this list.
 */
void
scorep_mpi_unify_communicators( void )
{
    uint32_t comm_world_size = SCOREP_Ipc_GetSize();
    uint32_t rank            = SCOREP_Ipc_GetRank();

    UTILS_BUG_ON( comm_world_size == 0, "Invalid MPI_COMM_WORLD size of 0" );

    /* Create group for comm self early, so that the order is nice in the end */
    SCOREP_GroupHandle self = SCOREP_Definitions_NewGroup(
        SCOREP_GROUP_MPI_SELF,
        "",
        0,
        NULL );

    uint32_t n_interim_comm_defs = 0;
    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_BEGIN( &scorep_local_definition_manager,
                                                         InterimCommunicator,
                                                         interim_communicator )
    {
        if ( definition->paradigm_type != SCOREP_PARADIGM_MPI )
        {
            continue;
        }
        scorep_mpi_comm_definition_payload* comm_payload =
            SCOREP_InterimCommunicatorHandle_GetPayload( handle );

        if ( comm_payload->comm_size == 1 )
        {
            continue;
        }
        n_interim_comm_defs++;
    }
    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_END();

    /* Cache all references to the InterimComm defs and their payload */
    struct
    {
        SCOREP_InterimCommunicatorDef*      definition;
        scorep_mpi_comm_definition_payload* payload;
    }* local_interim_comm_defs = calloc( n_interim_comm_defs,
                                         sizeof( *local_interim_comm_defs ) );
    UTILS_ASSERT( local_interim_comm_defs );

    uint32_t interim_comm_defs_processed = 0;
    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_BEGIN( &scorep_local_definition_manager,
                                                         InterimCommunicator,
                                                         interim_communicator )
    {
        if ( definition->paradigm_type != SCOREP_PARADIGM_MPI )
        {
            continue;
        }
        scorep_mpi_comm_definition_payload* comm_payload =
            SCOREP_InterimCommunicatorHandle_GetPayload( handle );

        if ( comm_payload->comm_size == 1 )
        {
            continue;
        }

        local_interim_comm_defs[ interim_comm_defs_processed ].definition = definition;
        local_interim_comm_defs[ interim_comm_defs_processed ].payload    = comm_payload;
        interim_comm_defs_processed++;
    }
    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_END();

    uint32_t* all_next_interim_comm_def =
        calloc( 3 * comm_world_size, sizeof( *all_next_interim_comm_def ) );
    UTILS_ASSERT( all_next_interim_comm_def );

    uint32_t* group_ranks =
        calloc( comm_world_size, sizeof( *group_ranks ) );
    UTILS_ASSERT( group_ranks );

    /* Now iterate until all ranks processed their comms */
    interim_comm_defs_processed = 0;
    while ( true )
    {
        uint32_t next_interim_comm_def[ 3 ] = { UINT32_MAX, UINT32_MAX, UINT32_MAX };
        uint32_t comm_size                  = comm_world_size;
        if ( interim_comm_defs_processed < n_interim_comm_defs )
        {
            /* We still have a interim-comm left, announce our next def */
            scorep_mpi_comm_definition_payload* payload = local_interim_comm_defs[ interim_comm_defs_processed ].payload;
            next_interim_comm_def[ 0 ] = payload->global_root_rank;
            next_interim_comm_def[ 1 ] = payload->root_id;
            next_interim_comm_def[ 2 ] = payload->local_rank;
            comm_size                  = payload->comm_size;
        }

        SCOREP_Ipc_Allgather( next_interim_comm_def,
                              all_next_interim_comm_def,
                              3, SCOREP_IPC_UINT32_T );

        /*
         * Check, if my next interim-comm def is satisfied, if we don't have a
         * next interim-comm, than this loop counts all ranks, which are also
         * already done
         */
        uint32_t ranks_participating = 0;
        for ( uint32_t i = 0; i < comm_world_size; i++ )
        {
            uint32_t* this_interim_comm_def = all_next_interim_comm_def + 3 * i;
            if ( this_interim_comm_def[ 0 ] == next_interim_comm_def[ 0 ]
                 && this_interim_comm_def[ 1 ] == next_interim_comm_def[ 1 ] )
            {
                ranks_participating++;
            }
        }

        if ( ranks_participating != comm_size )
        {
            /* Not all my ranks participate, thus I wont participate either */
            continue;
        }

        if ( next_interim_comm_def[ 0 ] == UINT32_MAX )
        {
            /*
             * All ranks participate, but none of them have an interim-comm left,
             * we are all done
             */
            break;
        }

        /* Build the MPI group definition */
        SCOREP_InterimCommunicatorDef*      definition = local_interim_comm_defs[ interim_comm_defs_processed ].definition;
        scorep_mpi_comm_definition_payload* payload    = local_interim_comm_defs[ interim_comm_defs_processed ].payload;

        for ( uint32_t i = 0; i < comm_world_size; i++ )
        {
            uint32_t* this_interim_comm_def = all_next_interim_comm_def + 3 * i;
            if ( this_interim_comm_def[ 0 ] != next_interim_comm_def[ 0 ]
                 || this_interim_comm_def[ 1 ] != next_interim_comm_def[ 1 ] )
            {
                continue;
            }
            group_ranks[ this_interim_comm_def[ 2 ] ] = i;
        }

        SCOREP_GroupHandle group = SCOREP_Definitions_NewGroupFrom32(
            SCOREP_GROUP_MPI_GROUP,
            "",
            payload->comm_size,
            ( const uint32_t* )group_ranks );

        SCOREP_CommunicatorHandle unified_parent_handle = SCOREP_INVALID_COMMUNICATOR;
        if ( definition->parent_handle != SCOREP_INVALID_INTERIM_COMMUNICATOR )
        {
            SCOREP_InterimCommunicatorDef* parent_defintion =
                SCOREP_LOCAL_HANDLE_DEREF( definition->parent_handle, InterimCommunicator );
            unified_parent_handle = parent_defintion->unified;
        }

        definition->unified = SCOREP_Definitions_NewCommunicator(
            group,
            definition->name_handle,
            unified_parent_handle,
            payload->root_id );

        interim_comm_defs_processed++;
    }

    interim_comm_defs_processed = 0;
    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_BEGIN( &scorep_local_definition_manager,
                                                         InterimCommunicator,
                                                         interim_communicator )
    {
        if ( definition->paradigm_type != SCOREP_PARADIGM_MPI )
        {
            continue;
        }
        scorep_mpi_comm_definition_payload* comm_payload =
            SCOREP_InterimCommunicatorHandle_GetPayload( handle );

        if ( comm_payload->comm_size != 1 )
        {
            continue;
        }

        /*
         * Using `root_id` as the `unifyKey`, will result in as many comm-self
         * definitions as on the rank with the most of them.
         */
        definition->unified = SCOREP_Definitions_NewCommunicator(
            self,
            definition->name_handle,
            SCOREP_INVALID_COMMUNICATOR,
            comm_payload->root_id );
        interim_comm_defs_processed++;
    }
    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_END();

    free( local_interim_comm_defs );
    free( all_next_interim_comm_def );
    free( group_ranks );
}
