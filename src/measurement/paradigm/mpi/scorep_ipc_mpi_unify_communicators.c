/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2016, 2022,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2014, 2022,
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
#define SCOREP_DEBUG_MODULE_NAME MPI
#include <UTILS_Debug.h>

#define NUMBER_OF_PAYLOAD_ELEMENTS 4

/**
 * Decide for a MPI comm payload if the respective communicator is
 * MPI_COMM_SELF like based on group sizes.
 */
static bool
payload_belongs_to_comm_self( scorep_mpi_comm_definition_payload* payload )
{
    if ( payload->comm_size == 1 && payload->remote_comm_size == 0 )
    {
        return true;
    }
    return false;
}


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

        /* Sorting out intra comm selfs for later creation, while keeping one element
           groups of inter comms, as those are required. */
        if ( payload_belongs_to_comm_self( comm_payload ) )
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

        /* Sorting out intra comm selfs for later creation, while keeping one element
           groups of inter comms, as those are required. */
        if ( payload_belongs_to_comm_self( comm_payload ) )
        {
            continue;
        }

        local_interim_comm_defs[ interim_comm_defs_processed ].definition = definition;
        local_interim_comm_defs[ interim_comm_defs_processed ].payload    = comm_payload;
        interim_comm_defs_processed++;
    }
    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_END();

    uint32_t* all_next_interim_comm_def =
        calloc( NUMBER_OF_PAYLOAD_ELEMENTS * comm_world_size, sizeof( *all_next_interim_comm_def ) );
    UTILS_ASSERT( all_next_interim_comm_def );

    uint32_t* group_a_ranks =
        calloc( comm_world_size, sizeof( *group_a_ranks ) );
    UTILS_ASSERT( group_a_ranks );

    uint32_t* group_b_ranks =
        calloc( comm_world_size, sizeof( *group_b_ranks ) );
    UTILS_ASSERT( group_b_ranks );


    /* Now iterate until all ranks processed their comms */
    interim_comm_defs_processed = 0;
    #if HAVE( UTILS_DEBUG )
    uint32_t loopcount = 0;
    #endif
    UTILS_DEBUG( "[rank%d] Begin Unify loop of non-self comms", rank );
    while ( true )
    {
        UTILS_DEBUG( "[rank%d] loop iteration no=%u ================================================", rank, loopcount++ );

        uint32_t next_interim_comm_def[ NUMBER_OF_PAYLOAD_ELEMENTS ] = { UINT32_MAX, UINT32_MAX, UINT32_MAX,  UINT32_MAX };
        uint32_t comm_size                                           = comm_world_size;
        uint32_t remote_comm_size                                    = comm_world_size;
        uint32_t combined_size                                       = comm_world_size;
        uint32_t high_bit                                            = 0;
        if ( interim_comm_defs_processed < n_interim_comm_defs )
        {
            /* We still have a interim-comm left, announce our next def */
            scorep_mpi_comm_definition_payload* payload = local_interim_comm_defs[ interim_comm_defs_processed ].payload;

            /* Extract the high bit from the remote_comm_size. */
            high_bit = ( payload->remote_comm_size >> 31 ) & 1U;
            uint32_t extracted_remote_size = payload->remote_comm_size;
            extracted_remote_size &= ~( 1UL << 31 );
            UTILS_DEBUG( "[rank%d] Bit extraction -  original payload->remote_comm_size=%u high bit=%u extracted_remote_size %u",
                         rank, payload->remote_comm_size, high_bit, extracted_remote_size );

            next_interim_comm_def[ 0 ] = payload->global_root_rank;
            next_interim_comm_def[ 1 ] = payload->root_id;
            next_interim_comm_def[ 2 ] = payload->local_rank;
            next_interim_comm_def[ 3 ] = high_bit;
            comm_size                  = payload->comm_size;
            remote_comm_size           = extracted_remote_size;
            combined_size              = payload->comm_size + extracted_remote_size;
        }
        UTILS_DEBUG( "[rank%d] Before SCOREP_Ipc_Allgather own payload - global_root_rank=%u, root_id=%u, local_rank=%u, group_high=%u, comm_size=%u remote size=%u",
                     rank,  next_interim_comm_def[ 0 ],   next_interim_comm_def[ 1 ], next_interim_comm_def[ 2 ], next_interim_comm_def[ 3 ], comm_size, remote_comm_size );

        SCOREP_Ipc_Allgather( next_interim_comm_def,
                              all_next_interim_comm_def,
                              NUMBER_OF_PAYLOAD_ELEMENTS, SCOREP_IPC_UINT32_T );

        /*
         * Check, if my next interim-comm def is satisfied, if we don't have a
         * next interim-comm, then this loop counts all ranks, which are also
         * already done
         */
        uint32_t ranks_participating = 0;
        for ( uint32_t i = 0; i < comm_world_size; i++ )
        {
            uint32_t* this_interim_comm_def = all_next_interim_comm_def + NUMBER_OF_PAYLOAD_ELEMENTS * i;
            if ( this_interim_comm_def[ 0 ] == next_interim_comm_def[ 0 ]
                 && this_interim_comm_def[ 1 ] == next_interim_comm_def[ 1 ] )
            {
                ranks_participating++;
            }
        }
        UTILS_DEBUG( "[rank%d] number of participants:%u ", rank, ranks_participating );

        /*
         * The exit condition for the case that this rank isn't working on a communicator the remaining ranks are working on
         *  is based on the combined size. For intra comms this is the same as the size of their group, for inter comms this
         *  is the combined size of group a and group b as all ranks of boths groups have to be participating for this to be the
         *  correct time to process this inter comm.
         */
        if ( ranks_participating != combined_size )
        {
            /* Not all my ranks participate, thus I wont participate either */
            UTILS_DEBUG( "[rank%d] Exit condition for participation participants=%u size=%u", rank, ranks_participating, combined_size );
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
            uint32_t* this_interim_comm_def = all_next_interim_comm_def + NUMBER_OF_PAYLOAD_ELEMENTS * i;
            if ( this_interim_comm_def[ 0 ] != next_interim_comm_def[ 0 ]
                 || this_interim_comm_def[ 1 ] != next_interim_comm_def[ 1 ] )
            {
                continue;
            }
            /*
             * Add ranks to respective groups.
             * Group a is for either intra comms or lower group of a inter comm.
             */
            if ( this_interim_comm_def[ 3 ] == 0 )
            {
                group_a_ranks[ this_interim_comm_def[ 2 ] ] = i;
                UTILS_DEBUG( "[rank%d] Add '%u' to group a", rank, i );
            }
            else
            {
                group_b_ranks[ this_interim_comm_def[ 2 ] ] = i;
                UTILS_DEBUG( "[rank%d] Add '%u' to group b", rank, i );
            }
        }

        /*
         * Match output sizes based on being the low or high group - as the order of
         * group a/b has to be the same for both sides and not from the local perspective.
         */
        uint32_t output_size_a = next_interim_comm_def[ 3 ] != 1 ? comm_size : remote_comm_size;
        uint32_t output_size_b = next_interim_comm_def[ 3 ] != 1 ? remote_comm_size : comm_size;
        UTILS_DEBUG( "[rank%d] Match output size - high=%u  size_a=%u  size_b==%u",
                     rank, next_interim_comm_def[ 3 ], output_size_a, output_size_b );

        /* Group a is either the only group in an intra comm or represents group a for an inter comm. */
        SCOREP_GroupHandle group_a = SCOREP_Definitions_NewGroupFrom32(
            SCOREP_GROUP_MPI_GROUP,
            "",
            output_size_a,
            ( const uint32_t* )group_a_ranks );

        /* In case of inter comms create group b, which has to be non zero in size. */
        SCOREP_GroupHandle group_b = SCOREP_INVALID_GROUP;
        if ( output_size_b > 0 )
        {
            group_b = SCOREP_Definitions_NewGroupFrom32(
                SCOREP_GROUP_MPI_GROUP,
                "",
                output_size_b,
                ( const uint32_t* )group_b_ranks );
        }
        SCOREP_CommunicatorHandle unified_parent_handle = SCOREP_INVALID_COMMUNICATOR;
        if ( definition->parent_handle != SCOREP_INVALID_INTERIM_COMMUNICATOR )
        {
            SCOREP_InterimCommunicatorDef* parent_defintion =
                SCOREP_LOCAL_HANDLE_DEREF( definition->parent_handle, InterimCommunicator );
            unified_parent_handle = parent_defintion->unified;
        }

        /* Only for inter comms we have a group b of size > 0. */
        if ( output_size_b == 0 )
        {
            UTILS_DEBUG( "[rank%d] New intra comm definition", rank  );
            definition->unified = SCOREP_Definitions_NewCommunicator(
                group_a,
                definition->name_handle,
                unified_parent_handle,
                payload->root_id,
                SCOREP_COMMUNICATOR_FLAG_CREATE_DESTROY_EVENTS );
        }
        else
        {
            UTILS_DEBUG( "[rank%d] New inter comm definition", rank  );
            definition->unified = SCOREP_Definitions_NewInterCommunicator(
                group_a,
                group_b,
                definition->name_handle,
                unified_parent_handle,
                payload->root_id,
                SCOREP_COMMUNICATOR_FLAG_CREATE_DESTROY_EVENTS );
        }


        interim_comm_defs_processed++;
    }
    UTILS_DEBUG( "[rank%d] End Unify loop of non-self comms", rank );

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

        if ( !payload_belongs_to_comm_self( comm_payload ) )
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
            comm_payload->root_id,
            SCOREP_COMMUNICATOR_FLAG_CREATE_DESTROY_EVENTS );
        interim_comm_defs_processed++;
    }
    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_END();

    free( local_interim_comm_defs );
    free( all_next_interim_comm_def );
    free( group_a_ranks );
    free( group_b_ranks );
}
