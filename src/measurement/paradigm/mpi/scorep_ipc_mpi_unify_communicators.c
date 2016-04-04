/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2015,
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

void
scorep_mpi_unify_define_mpi_locations( void )
{
    if ( SCOREP_Ipc_GetRank() != 0 )
    {
        return;
    }

    /*
     * Define the list of locations which are MPI ranks.
     *
     * If we support MPI_THREAD_FUNNELED, this needs to be the
     * location, which has called MPI_Init/MPI_Thread_init.
     * For the moment, the location and rank ids match.
     *
     * This needs to be called early, so that the resulting definition
     * is before any other group definition of type SCOREP_GROUP_MPI_GROUP.
     */
    SCOREP_Definitions_NewUnifiedGroupFrom32( SCOREP_GROUP_MPI_LOCATIONS,
                                              "",
                                              scorep_mpi_world.size,
                                              ( const uint32_t* )scorep_mpi_world.ranks );
}

static uint32_t
create_local_mappings( uint32_t comm_world_size,
                       uint32_t rank );

static void
define_comms( uint32_t comm_world_size,
              uint32_t rank,
              uint32_t total_number_of_root_comms );

static void
define_self_likes( uint32_t rank );

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
    uint32_t total_number_of_root_comms;

    UTILS_BUG_ON( comm_world_size == 0, "Invalid MPI_COMM_WORLD size of 0" );

    /* 1) Generate mapping from local to global communicators */
    total_number_of_root_comms = create_local_mappings( comm_world_size, rank );

    /* 2) Generate definitions for MPI groups and non-self-like communicators */
    define_comms( comm_world_size,
                  rank,
                  total_number_of_root_comms );

    /* 3) Append information about self communicators */
    define_self_likes( rank );
}

static uint32_t
create_local_mappings( uint32_t comm_world_size,
                       uint32_t rank )
{
    /* Allocate memory for offsets */
    uint32_t* number_of_root_comms_per_rank =
        calloc( comm_world_size, sizeof( *number_of_root_comms_per_rank ) );
    UTILS_BUG_ON( number_of_root_comms_per_rank == NULL,
                  "Can't allocate comm_world_size array of size %u",
                  comm_world_size );

    /* Gather communicator counts from all ranks */
    SCOREP_Ipc_Allgather( &scorep_mpi_number_of_root_comms,
                          number_of_root_comms_per_rank,
                          1, SCOREP_IPC_UINT32_T );

    /*
     * Calculate the total number of communicators, which is also the id of
     * the first unified self like communicator. And calculate the exclusive
     * prefix sum of the number of comms in the ranks, which will serve
     * as the offset to the global communicator for comms where this rank was
     * root in the communicator.
     */
    uint32_t total_number_of_root_comms = 0;
    for ( uint32_t i = 0; i < comm_world_size; i++ )
    {
        uint32_t comms_in_rank = number_of_root_comms_per_rank[ i ];

        number_of_root_comms_per_rank[ i ] = total_number_of_root_comms;

        total_number_of_root_comms += comms_in_rank;
    }
    /*
     * This array now holds the exclusive prefix sum of
     * number_of_root_comms_per_rank, which is used as an offset from the
     * the root comms I created into the global id of these communicators.
     */
    uint32_t* offsets = number_of_root_comms_per_rank;


    /* Create mapping tables
     * Every process calculates its own mappings from the offsets.
     */
    uint32_t* interim_communicator_mapping =
        scorep_local_definition_manager.interim_communicator.mapping;
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

        uint32_t global_comm_id = comm_payload->root_id;
        if ( !comm_payload->is_self_like )
        {
            global_comm_id += offsets[ comm_payload->global_root_rank ];
        }
        else
        {
            global_comm_id += total_number_of_root_comms;
            UTILS_BUG_ON( comm_payload->global_root_rank != rank,
                          "Invalid interim communicator definition, roots don't match: %u != %u",
                          comm_payload->global_root_rank, rank );
        }
        interim_communicator_mapping[ definition->sequence_number ] =
            global_comm_id;
    }
    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_END();

    free( number_of_root_comms_per_rank );

    /* The total number will be passed to the next phase */
    return total_number_of_root_comms;
}


/**
 * Helper function which checks whether this rank is part in the communicator
 * given by the global communicator id @p global_comm_id. If it is, it returns
 * the local rank according to the communicator. If not, it returns UINT32_MAX.
 *
 * @param      global_comm_id A unified communicator id.
 * @param[out] global_aux_ids Global ids of auxiliary communicator information.
 *                            Currently:
 *                            @li [0] Global id of the communicator's name
 *                            @li [1] Global id of the communicator's parent
 *
 * @return My local rank in this communicator, or UINT32_MAX if I'm not in
 *         this communicator.
 */
static uint32_t
is_this_rank_in_communicator( uint32_t  global_comm_id,
                              uint32_t* global_aux_ids )
{
    uint32_t* string_mapping =
        scorep_local_definition_manager.string.mapping;
    uint32_t* interim_communicator_mapping =
        scorep_local_definition_manager.interim_communicator.mapping;
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

        if ( global_comm_id ==
             interim_communicator_mapping[ definition->sequence_number ] )
        {
            if ( comm_payload->local_rank == 0 )
            {
                if ( definition->name_handle != SCOREP_INVALID_STRING )
                {
                    SCOREP_StringDef* name_definition = SCOREP_HANDLE_DEREF(
                        definition->name_handle,
                        String,
                        scorep_local_definition_manager.page_manager );
                    global_aux_ids[ 0 ] = string_mapping[ name_definition->sequence_number ];
                }
                if ( definition->parent_handle != SCOREP_INVALID_INTERIM_COMMUNICATOR )
                {
                    SCOREP_InterimCommunicatorDef* parent_definition = SCOREP_HANDLE_DEREF(
                        definition->parent_handle,
                        InterimCommunicator,
                        scorep_local_definition_manager.page_manager );
                    global_aux_ids[ 1 ] =
                        interim_communicator_mapping[ parent_definition->sequence_number ];
                }
            }
            return comm_payload->local_rank;
        }
    }
    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_END();

    return UINT32_MAX;
}


static void
define_comms( uint32_t comm_world_size,
              uint32_t rank,
              uint32_t total_number_of_root_comms )
{
    uint32_t* ranks_in_comm = NULL;   // Vector for the communicator belongings
                                      // (only significant in root)
    uint32_t* ranks_in_group = NULL;  // Vector for the MPI group
                                      // (only significant in root)

    struct comm_definition
    {
        SCOREP_CommunicatorHandle handle;
        SCOREP_GroupHandle        group;
        const char*               name;
        uint32_t                  comm_parent;
    }* comm_definitions = NULL;

    uint32_t* topo_comm_mapping = NULL;

    const char** unified_strings = NULL;

    if ( rank == 0 )
    {
        /* Allocate memory for the arrays. */

        ranks_in_comm = calloc( comm_world_size, sizeof( *ranks_in_comm ) );
        UTILS_BUG_ON( ranks_in_comm == NULL,
                      "Can't allocate ranks_in_comm array of size %u",
                      comm_world_size );

        ranks_in_group = calloc( comm_world_size, sizeof( *ranks_in_group ) );
        UTILS_BUG_ON( ranks_in_group == NULL,
                      "Can't allocate ranks_in_group array of size %u",
                      comm_world_size );

        comm_definitions = calloc( total_number_of_root_comms, sizeof( *comm_definitions ) );
        UTILS_BUG_ON( total_number_of_root_comms != 0 && comm_definitions == NULL,
                      "Can't allocate comm_definitions array of size %u",
                      total_number_of_root_comms );

        /* Create a map from global ids to unified handles of string definitions */
        unified_strings = calloc( scorep_unified_definition_manager->string.counter,
                                  sizeof( *unified_strings ) );
        UTILS_BUG_ON( unified_strings == NULL,
                      "Can't allocate unified_strings array of size %u",
                      scorep_unified_definition_manager->string.counter );

        SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_BEGIN( scorep_unified_definition_manager,
                                                             String,
                                                             string )
        {
            unified_strings[ definition->sequence_number ] = definition->string_data;
        }
        SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_END();
    }

    topo_comm_mapping = calloc( total_number_of_root_comms + 1, sizeof( *topo_comm_mapping ) );
    UTILS_BUG_ON( topo_comm_mapping == NULL,
                  "Can't allocate unified_strings array of size %u",
                  total_number_of_root_comms + 1 );

    /*
     * Iterate over all communicators and create the corresponding MPI group
     * and define both.
     */
    for ( uint32_t global_comm_id = 0;
          global_comm_id < total_number_of_root_comms;
          global_comm_id++ )
    {
        /*
         * Check whether this rank belongs to the communicator
         *
         * It's either my local rank in this communicator, or UINT32_MAX if I'm not
         * a member of this communicator.
         *
         * If I'm the root in this communicator, store auxiliary communicator
         * properties in global_aux_ids, which I will than send to the global
         * root.
         * [0] is the communicator's name (default "" => 0)
         * [1] the parent of this communicator (default no parent => UINT32_MAX)
         */
        uint32_t global_aux_ids[ 2 ] = { 0, UINT32_MAX };
        uint32_t my_rank_in_comm     =
            is_this_rank_in_communicator( global_comm_id,
                                          global_aux_ids );

        /* gather communicator information */
        SCOREP_Ipc_Gather( &my_rank_in_comm,
                           ranks_in_comm,
                           1, SCOREP_IPC_UINT32_T,
                           0 );

        if ( rank == 0 )
        {
            /*
             * Create MPI group. The list contains at position i the global
             * rank of process i in the communicator
             */
            uint32_t size = 0;
            for ( uint32_t i = 0; i < comm_world_size; i++ )
            {
                if ( ranks_in_comm[ i ] != UINT32_MAX )
                {
                    /*
                     * The local rank 0 has auxiliary communicator
                     * information for us. Receive them. But only if myself is
                     * not the local root.
                     */
                    if ( i > 0 && ranks_in_comm[ i ] == 0 )
                    {
                        SCOREP_Ipc_Recv( global_aux_ids,
                                         2, SCOREP_IPC_UINT32_T,
                                         i );
                    }

                    ranks_in_group[ ranks_in_comm[ i ] ] = i;
                    size++;
                }
            }

            /*
             * Define the MPI group for this communicator, this may be a duplicate,
             * but the definition system already handles this for us.
             */
            comm_definitions[ global_comm_id ].group =
                SCOREP_Definitions_NewUnifiedGroupFrom32( SCOREP_GROUP_MPI_GROUP,
                                                          "",
                                                          size,
                                                          ranks_in_group );

            /*
             * We don't define the communicator now, because the parent may not
             * be defined yet. Thus just store the needed information now, and
             * than define the communciators in an topological order later.
             * The handle will be used for this topological sorting.
             */
            comm_definitions[ global_comm_id ].handle =
                SCOREP_INVALID_COMMUNICATOR;
            comm_definitions[ global_comm_id ].name =
                unified_strings[ global_aux_ids[ 0 ] ];
            comm_definitions[ global_comm_id ].comm_parent =
                global_aux_ids[ 1 ];
        }
        else if ( my_rank_in_comm == 0 )
        {
            /*
             * I'm the local root in the communicator (but not the global root),
             * send auxiliary communicator information to the global root.
             */
            SCOREP_Ipc_Send( global_aux_ids,
                             2, SCOREP_IPC_UINT32_T, 0 );
        }
    }

    /*
     * Define the communicators in a topological order based on the parent
     * relation and maintain a mapping from the previously assigned global ids
     * to the new ids.
     */
    if ( rank == 0 )
    {
        uint32_t global_comm_id  = 0;
        uint32_t number_of_comms = scorep_unified_definition_manager->communicator.counter;
        while ( global_comm_id < total_number_of_root_comms )
        {
            for ( uint32_t i = 0; i < total_number_of_root_comms; i++ )
            {
                if ( comm_definitions[ i ].handle != SCOREP_INVALID_COMMUNICATOR )
                {
                    /* already defined */
                    continue;
                }

                if ( comm_definitions[ i ].comm_parent != UINT32_MAX
                     && comm_definitions[ comm_definitions[ i ].comm_parent ].handle
                     == SCOREP_INVALID_COMMUNICATOR )
                {
                    /* have a parent, but parent not yet defined */
                    continue;
                }

                SCOREP_CommunicatorHandle comm_parent_handle =
                    SCOREP_INVALID_COMMUNICATOR;

                if ( comm_definitions[ i ].comm_parent != UINT32_MAX )
                {
                    comm_parent_handle =
                        comm_definitions[ comm_definitions[ i ].comm_parent ].handle;
                }

                /* Define the global MPI communicator with this group */
                comm_definitions[ i ].handle = SCOREP_Definitions_NewUnifiedCommunicator(
                    comm_definitions[ i ].group,
                    comm_definitions[ i ].name,
                    comm_parent_handle );
                UTILS_BUG_ON(
                    SCOREP_UNIFIED_HANDLE_TO_ID( comm_definitions[ i ].handle, Communicator )
                    != number_of_comms + global_comm_id,
                    "Definition system created out-of-order sequence number for communicator: %u != %u",
                    SCOREP_UNIFIED_HANDLE_TO_ID( comm_definitions[ i ].handle, Communicator ),
                    number_of_comms + global_comm_id );

                /*
                 * Account the already existing number of non-MPI communicators
                 * here, so that the mappings are automatically shifted
                 */
                topo_comm_mapping[ i ] = number_of_comms + global_comm_id++;
            }
        }

        /*
         * Include the number of already unified non-MPI communicators,
         * so that all ranks can shift their self-like communicators.
         */
        topo_comm_mapping[ total_number_of_root_comms ] = number_of_comms;
    }

    /* Broadcast the new global ids to all ranks */
    SCOREP_Ipc_Bcast(
        topo_comm_mapping,
        total_number_of_root_comms + 1,
        SCOREP_IPC_UINT32_T,
        0 );

    /* Apply new ids to all communicators */
    uint32_t* interim_communicator_mapping =
        scorep_local_definition_manager.interim_communicator.mapping;
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

        if ( !comm_payload->is_self_like )
        {
            uint32_t old_global_comm_id =
                interim_communicator_mapping[ definition->sequence_number ];
            interim_communicator_mapping[ definition->sequence_number ] =
                topo_comm_mapping[ old_global_comm_id ];
        }
        else
        {
            /*
             * Shift the local mapping to the unified definition with the
             * amount of already defined unified non-MPI communicators
             */
            interim_communicator_mapping[ definition->sequence_number ] +=
                topo_comm_mapping[ total_number_of_root_comms ];
        }
    }
    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_END();

    free( ranks_in_group );
    free( ranks_in_comm );
    free( comm_definitions );
    free( unified_strings );
    free( topo_comm_mapping );
}

static void
define_self_likes( uint32_t rank )
{
    /*
     * Get the maximum number of different MPI_COMM_SELF references
     * on a single process
     */
    uint32_t max_number_of_self_ids = 0;
    SCOREP_Ipc_Reduce( &scorep_mpi_number_of_self_comms,
                       &max_number_of_self_ids,
                       1,
                       SCOREP_IPC_UINT32_T,
                       SCOREP_IPC_MAX,
                       0 );

    if ( rank == 0 )
    {
        uint32_t number_of_comms =
            scorep_unified_definition_manager->communicator.counter;

        /* Create group for comm self */
        SCOREP_GroupHandle self = SCOREP_Definitions_NewUnifiedGroupFrom32(
            SCOREP_GROUP_MPI_SELF,
            "",
            0,
            NULL );

        for ( uint32_t i = 0; i < max_number_of_self_ids; i++ )
        {
            /*
             * We can give the very first self-like comm a name, its "MPI_COMM_SELF",
             * as this comm existed in all rank.
             */
            SCOREP_CommunicatorHandle handle =
                SCOREP_Definitions_NewUnifiedCommunicator(
                    self,
                    i == 0 ? "MPI_COMM_SELF" : "",
                    SCOREP_INVALID_COMMUNICATOR );
            UTILS_BUG_ON(
                SCOREP_UNIFIED_HANDLE_TO_ID( handle, Communicator )
                != number_of_comms + i,
                "Definition system created out-of-order sequence number for self-like communicator: %u != %u",
                SCOREP_UNIFIED_HANDLE_TO_ID( handle, Communicator ),
                number_of_comms + i );
        }
    }
}
