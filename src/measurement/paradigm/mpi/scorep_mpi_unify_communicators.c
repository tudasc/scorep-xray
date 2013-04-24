/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
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
 * @file       src/measurement/paradigm/mpi/scorep_mpi_unify_communicators.c
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status alpha
 *
 */


#include <config.h>

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include <inttypes.h>

#include <mpi.h>

#include <scorep_ipc.h>
#include <SCOREP_Mpi.h>
#include <scorep_mpi_communicator.h>
#include <SCOREP_Definitions.h>
#include <definitions/SCOREP_Definitions.h>

#include <UTILS_Debug.h>

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
     * If we support MPI_THREADED_FUNNELED, this needs to be the
     * location, wich has called MPI_Init/MPI_Thread_init.
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

    /* 1) Generate mapping from local to global communicators */
    total_number_of_root_comms = create_local_mappings( comm_world_size, rank );

    /* 2) Generate definitions for MPI groups and non-self-like communicators */
    define_comms( comm_world_size,
                  rank,
                  total_number_of_root_comms );

    /* 3) Append information about self communicators */
    define_self_likes( rank );
}

uint32_t
create_local_mappings( uint32_t comm_world_size,
                       uint32_t rank )
{
    /* Allocate memory for offsets */
    uint32_t* offsets = calloc( comm_world_size, sizeof( *offsets ) );
    assert( offsets );

    /* Gather communicator counts from all ranks */
    SCOREP_Ipc_Allgather( &scorep_mpi_number_of_root_comms,
                          offsets,
                          1, SCOREP_IPC_UINT32 );

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
        uint32_t comms_in_rank = offsets[ i ];

        offsets[ i ] = total_number_of_root_comms;

        total_number_of_root_comms += comms_in_rank;
    }

    /* Create mapping tables
       Every process calculates its own mappings from the offsets. */
    uint32_t* interim_communicator_mappings =
        scorep_local_definition_manager.mappings->interim_communicator_mappings;
    SCOREP_DEFINITION_FOREACH_DO( &scorep_local_definition_manager,
                                  InterimCommunicator,
                                  interim_communicator )
    {
        if ( definition->adapter_type != SCOREP_ADAPTER_MPI )
        {
            continue;
        }
        scorep_mpi_comm_definition_payload* comm_payload =
            SCOREP_InterimCommunicatorGetPayload( handle );

        uint32_t global_comm_id = comm_payload->root_id;
        if ( !comm_payload->is_self_like )
        {
            global_comm_id += offsets[ comm_payload->global_root_rank ];
        }
        else
        {
            global_comm_id += total_number_of_root_comms;
            assert( comm_payload->global_root_rank == rank );
        }
        interim_communicator_mappings[ definition->sequence_number ]
            = global_comm_id;
    }
    SCOREP_DEFINITION_FOREACH_WHILE();

    free( offsets );

    /* the total number will be passed to the next phase */
    return total_number_of_root_comms;
}


/**
 * Helper function which checks for a global communicator id whether this rank
 * is part of the communicator. If it is, it returns its rank according to the
 * communicator specified by @a global_comm_id. If it is not, it returns -1.
 *
 * @param global_comm_id A unified communicator id.
 * @param global_aux_ids[out] Global ids of auxilary communicator information.
 *                            Currently:
 *                            @li [0] Id of the communicator's name
 *                            @li [1] Id of the communicator's parent
 *
 * @return My local rank in this communicator, or -1 when I'm not in
 *         this communicator.
 */
static uint32_t
is_this_rank_in_communicator( uint32_t  global_comm_id,
                              uint32_t* global_aux_ids )
{
    uint32_t* string_mappings =
        scorep_local_definition_manager.mappings->string_mappings;
    uint32_t* interim_communicator_mappings =
        scorep_local_definition_manager.mappings->interim_communicator_mappings;
    SCOREP_DEFINITION_FOREACH_DO( &scorep_local_definition_manager,
                                  InterimCommunicator,
                                  interim_communicator )
    {
        if ( definition->adapter_type != SCOREP_ADAPTER_MPI )
        {
            continue;
        }
        scorep_mpi_comm_definition_payload* comm_payload =
            SCOREP_InterimCommunicatorGetPayload( handle );

        if ( global_comm_id ==
             interim_communicator_mappings[ definition->sequence_number ] )
        {
            if ( comm_payload->local_rank == 0 )
            {
                /*
                 * If this comm does still not have a name (Ie. its
                 * SCOREP_INVALID_STRING), assign it the empty string,
                 * which has global id 0, and global_aux_ids[ 0 ] is already
                 * initialized with 0.
                 */
                if ( definition->name_handle != SCOREP_INVALID_STRING )
                {
                    SCOREP_StringDef* name_definition = SCOREP_HANDLE_DEREF(
                        definition->name_handle,
                        String,
                        scorep_local_definition_manager.page_manager );
                    global_aux_ids[ 0 ] = string_mappings[ name_definition->sequence_number ];
                }
                if ( definition->parent_handle != SCOREP_INVALID_INTERIM_COMMUNICATOR )
                {
                    SCOREP_InterimCommunicatorDef* parent_definition = SCOREP_HANDLE_DEREF(
                        definition->parent_handle,
                        InterimCommunicator,
                        scorep_local_definition_manager.page_manager );
                    global_aux_ids[ 1 ] =
                        interim_communicator_mappings[ parent_definition->sequence_number ];
                }
            }
            return comm_payload->local_rank;
        }
    }
    SCOREP_DEFINITION_FOREACH_WHILE();

    return UINT32_MAX;
}


void
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
        assert( ranks_in_comm );

        ranks_in_group = calloc( comm_world_size, sizeof( *ranks_in_group ) );
        assert( ranks_in_group );

        comm_definitions = calloc( total_number_of_root_comms, sizeof( *comm_definitions ) );
        assert( comm_definitions );

        /* Create a map from global ids to unified handles of string definitions */
        unified_strings = calloc( scorep_unified_definition_manager->string_definition_counter,
                                  sizeof( *unified_strings ) );
        assert( unified_strings );
        SCOREP_DEFINITION_FOREACH_DO( scorep_unified_definition_manager,
                                      String,
                                      string )
        {
            unified_strings[ definition->sequence_number ] = definition->string_data;
        }
        SCOREP_DEFINITION_FOREACH_WHILE();
    }

    topo_comm_mapping = calloc( total_number_of_root_comms + 1, sizeof( *topo_comm_mapping ) );
    assert( topo_comm_mapping );

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
         * If I'm the root in this communicator, store auxilary communicator
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
                           1, SCOREP_IPC_UINT32,
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
                    if ( i > 0 && ranks_in_comm[ i ] == 0 )
                    {
                        SCOREP_Ipc_Recv( global_aux_ids,
                                         2, SCOREP_IPC_UINT32,
                                         i );
                    }

                    ranks_in_group[ ranks_in_comm[ i ] ] = i;
                    size++;
                }
            }

            /* Define the MPI group */
            comm_definitions[ global_comm_id ].handle =
                SCOREP_INVALID_COMMUNICATOR;

            comm_definitions[ global_comm_id ].group =
                SCOREP_Definitions_NewUnifiedGroupFrom32( SCOREP_GROUP_MPI_GROUP,
                                                          "",
                                                          size,
                                                          ranks_in_group );

            comm_definitions[ global_comm_id ].name        = unified_strings[ global_aux_ids[ 0 ] ];
            comm_definitions[ global_comm_id ].comm_parent = global_aux_ids[ 1 ];
        }
        else if ( my_rank_in_comm == 0 )
        {
            SCOREP_Ipc_Send( global_aux_ids,
                             2, SCOREP_IPC_UINT32, 0 );
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
        uint32_t number_of_comms = scorep_unified_definition_manager->communicator_definition_counter;
        while ( global_comm_id < total_number_of_root_comms )
        {
            for ( uint32_t i = 0; i < total_number_of_root_comms; i++ )
            {
                if ( comm_definitions[ i ].handle != SCOREP_INVALID_COMMUNICATOR )
                {
                    // already defined
                    continue;
                }

                if ( comm_definitions[ i ].comm_parent != -1
                     && comm_definitions[ comm_definitions[ i ].comm_parent ].handle
                     == SCOREP_INVALID_COMMUNICATOR )
                {
                    // parent not defined
                    continue;
                }

                SCOREP_CommunicatorHandle comm_parent_handle =
                    SCOREP_INVALID_COMMUNICATOR;

                if ( comm_definitions[ i ].comm_parent != -1 )
                {
                    comm_parent_handle =
                        comm_definitions[ comm_definitions[ i ].comm_parent ].handle;
                }

                /* Define the global MPI communicator with this group */
                comm_definitions[ i ].handle = SCOREP_Definitions_NewUnifiedCommunicator(
                    comm_definitions[ i ].group,
                    comm_definitions[ i ].name,
                    comm_parent_handle );
                assert( SCOREP_UNIFIED_HANDLE_TO_ID(
                            comm_definitions[ i ].handle, Communicator ) ==
                        number_of_comms + global_comm_id );

                /*
                 * Account the already existing number of non-MPI communicators
                 * here, so that the mappings are automatically shifted
                 */
                topo_comm_mapping[ i ] = number_of_comms + global_comm_id++;
            }
        }

        /*
         * Include the number of already unified non-MPI comunicators,
         * so that all ranks can shift their self-like communicators.
         */
        topo_comm_mapping[ total_number_of_root_comms ] = number_of_comms;
    }

    /* Broadcast the new global ids to all ranks */
    SCOREP_Ipc_Bcast(
        topo_comm_mapping,
        total_number_of_root_comms + 1,
        SCOREP_IPC_UINT32,
        0 );

    /* Apply new ids to all communicators */
    uint32_t* interim_communicator_mappings =
        scorep_local_definition_manager.mappings->interim_communicator_mappings;
    SCOREP_DEFINITION_FOREACH_DO( &scorep_local_definition_manager,
                                  InterimCommunicator,
                                  interim_communicator )
    {
        if ( definition->adapter_type != SCOREP_ADAPTER_MPI )
        {
            continue;
        }
        scorep_mpi_comm_definition_payload* comm_payload =
            SCOREP_InterimCommunicatorGetPayload( handle );

        if ( !comm_payload->is_self_like )
        {
            uint32_t old_global_comm_id =
                interim_communicator_mappings[ definition->sequence_number ];
            interim_communicator_mappings[ definition->sequence_number ] =
                topo_comm_mapping[ old_global_comm_id ];
        }
        else
        {
            /*
             * Shift the local mapping to the unififed definition with the
             * ammount of already defined unifided non-MPI communicators
             */
            interim_communicator_mappings[ definition->sequence_number ] +=
                topo_comm_mapping[ total_number_of_root_comms ];
        }
    }
    SCOREP_DEFINITION_FOREACH_WHILE();

    free( ranks_in_group );
    free( ranks_in_comm );
    free( comm_definitions );
    free( unified_strings );
    free( topo_comm_mapping );
}

void
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
                       SCOREP_IPC_UINT32,
                       SCOREP_IPC_MAX,
                       0 );

    if ( rank == 0 )
    {
        uint32_t number_of_comms =
            scorep_unified_definition_manager->communicator_definition_counter;

        /* Create group for comm self */
        SCOREP_GroupHandle self = SCOREP_Definitions_NewUnifiedGroupFrom32(
            SCOREP_GROUP_COMM_SELF,
            "",
            0,
            NULL );

        for ( uint32_t i = 0; i < max_number_of_self_ids; i++ )
        {
            SCOREP_CommunicatorHandle handle =
                SCOREP_Definitions_NewUnifiedCommunicator(
                    self,
                    "",
                    SCOREP_INVALID_COMMUNICATOR );
            assert( SCOREP_UNIFIED_HANDLE_TO_ID( handle, Communicator ) ==
                    number_of_comms + i );
        }
    }
}
