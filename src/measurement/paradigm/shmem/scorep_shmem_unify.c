/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2013-2015,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2014,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
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
#include <assert.h>
#include <stdint.h>
#include <inttypes.h>

#include <SCOREP_Shmem.h>

#include <SCOREP_Definitions.h>
#include <SCOREP_Memory.h>
#include <scorep_status.h>
#include <scorep_ipc.h>

#include <UTILS_Debug.h>


#define GROUP_WORLD_NAME    "All processing elements"
#define GROUP_SELF_NAME     "Individual processing element"
#define COMM_WORLD_NAME     "Communicator (world)"
#define COMM_SELF_NAME      "Communicator (self-like)"
#define WIN_WORLD_NAME      "RMA window (world)"
#define WIN_SELF_NAME       "RMA window (self-like)"

/* *********************************************************************
 * Prototypes of auxiliary functions
 * ****************************************************************** */

static uint32_t
prepare_communicator_information( uint32_t** recvCnts,
                                  uint32_t** offsets );

static void
create_definitions( uint32_t  totalNumberOfRootComms,
                    uint32_t* recvCnts,
                    uint32_t* offsets );

static void
create_comm_self_definitions( void );


/* *********************************************************************
 * Pre-unify actions
 * ****************************************************************** */

/**
 * Define group of all SHMEM locations.
 */
void
scorep_shmem_define_shmem_locations( void )
{
    /* Array of members used for all group definitions */
    uint32_t members[ scorep_shmem_number_of_pes ];

    if ( scorep_shmem_my_rank == 0 )
    {
        /*
         * Define the list of locations which are SHMEM processing elements.
         *
         * This needs to be called early, so that the resulting definition
         * is before any other group definition of type SCOREP_GROUP_SHMEM_GROUP.
         */
        for ( uint32_t i = 0; i < scorep_shmem_number_of_pes; i++ )
        {
            members[ i ] = i;
        }
        SCOREP_Definitions_NewUnifiedGroupFrom32( SCOREP_GROUP_SHMEM_LOCATIONS,
                                                  "",
                                                  scorep_shmem_number_of_pes,
                                                  members );
    }
}


/* *********************************************************************
 * Post-unify actions
 * ****************************************************************** */

/**
 * This is a post-unify function and contains definition and
 * mapping of RMA windows and communicator related to SHMEM.
 */
void
scorep_shmem_define_shmem_group( void )
{
    uint32_t* recvcnts = NULL;
    uint32_t* offsets  = NULL;
    uint32_t  total_number_of_root_comms;

    /* 1) Generate mapping from local to global communicators */
    total_number_of_root_comms = prepare_communicator_information( &recvcnts, &offsets );

    /* 2) Create definitions for all communicators and RMA window except 'self-like' ones */
    create_definitions( total_number_of_root_comms, recvcnts, offsets );

    free( recvcnts );
    free( offsets );

    /* 3) Handle self-like communicators and windows */
    create_comm_self_definitions();
}


/* *********************************************************************
 * Auxiliary functions
 * ****************************************************************** */

/**
 * Collect information about SHMEM related communicators.
 * Calculate the total number of communicators (except
 * 'self-like' ones).
 *
 * @param[out] recvCnts         Array with the number of communicators.
 *                              recvCnts[i] contains the number of
 *                              communicator with PE i as 'root'.
 * @param[out] offsets          Array with the number of preceding
 *                              communicators. offsets[i] contains the
 *                              number of communicators which had
 *                              already been created by PEs < i.
 *
 * @return Total number of communicators.
 */
static uint32_t
prepare_communicator_information( uint32_t** recvCnts,
                                  uint32_t** offsets )
{
    /* Allocate memory */
    *recvCnts = calloc( scorep_shmem_number_of_pes, sizeof( **recvCnts ) );
    *offsets  = calloc( scorep_shmem_number_of_pes, sizeof( **offsets ) );
    assert( *recvCnts );
    assert( *offsets );

    /* Gather communicator counts from all ranks */
    SCOREP_Ipc_Allgather( scorep_shmem_number_of_root_comms,
                          *recvCnts,
                          1, SCOREP_IPC_UINT32_T );

    /*
     * Calculate the total number of communicators, which is also the id of
     * the first unified self like communicator. And calculate the exclusive
     * prefix sum of the number of comms in the ranks, which will serve
     * as the offset to the global communicator for comms where this rank was
     * root in the communicator.
     */
    uint32_t total_number_of_root_comms = 0;
    for ( uint32_t i = 0; i < scorep_shmem_number_of_pes; i++ )
    {
        ( *offsets )[ i ]           = total_number_of_root_comms;
        total_number_of_root_comms += ( *recvCnts )[ i ];
    }

    return total_number_of_root_comms;
}

/**
 * Define SHMEM related groups, communicators, and RMA windows.
 * 'Self-like' entities are not handled by this function
 * (@see create_comm_self_definitions). Finally, apply mapping
 * for defined groups, communicators, and RMA windows.
 *
 * @param totalNumberOfRootComms
 * @param recvCnts                  Array with the number of communicators.
 *                                  recvCnts[i] contains the number of
 *                                  communicator with PE i as 'root'.
 * @param offsets                   Array with the number of preceding
 *                                  communicators. offsets[i] contains the
 *                                  number of communicators which had
 *                                  already been created by PEs < i.
 */
void
create_definitions( uint32_t  totalNumberOfRootComms,
                    uint32_t* recvCnts,
                    uint32_t* offsets )
{
    uint32_t my_pe_sizes[ *scorep_shmem_number_of_root_comms ];
    uint32_t my_pe_strides[ *scorep_shmem_number_of_root_comms ];
    uint32_t all_pe_sizes[ totalNumberOfRootComms ];
    uint32_t all_pe_strides[ totalNumberOfRootComms ];

    /*
     * Each communicator was assigned to a 'root' PE (the PE_start element).
     * All 'root' elements pack information about their communicators
     * (PE_size and logPE_stride). This information determines individual
     * communicators and will be distributed to all PEs later on.
     */
    uint32_t my_index = 0;
    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_BEGIN( &scorep_local_definition_manager,
                                                         InterimRmaWindow,
                                                         interim_rma_window )
    {
        /* Get associated communicator */
        SCOREP_InterimCommunicatorHandle interim_comm_handle = definition->communicator_handle;
        SCOREP_InterimCommunicatorDef*   interim_comm_def    = SCOREP_LOCAL_HANDLE_DEREF( interim_comm_handle, InterimCommunicator );

        /* Handle only windows and communicators of the SHMEM paradigm */
        if ( interim_comm_def->paradigm_type != SCOREP_PARADIGM_SHMEM )
        {
            continue;
        }

        scorep_shmem_comm_definition_payload* comm_payload =
            SCOREP_InterimCommunicatorHandle_GetPayload( interim_comm_handle );

        /*
         * Handle only RMA windows and communicators:
         *  - that are not 'SELF_LIKE' (will be handled individually)
         *  - where this processing element is root of the communicator
         */
        if ( comm_payload->pe_size == 1 ||
             comm_payload->pe_start != scorep_shmem_my_rank )
        {
            continue;
        }

        my_pe_sizes[ my_index ]   = comm_payload->pe_size;
        my_pe_strides[ my_index ] = comm_payload->log_pe_stride;

        my_index++;
    }
    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_END();
    UTILS_BUG_ON( my_index != *scorep_shmem_number_of_root_comms,
                  "Invalid number of RmaWins %u, expected %u",
                  my_index, *scorep_shmem_number_of_root_comms );

    /* Distribute communicator information */
    SCOREP_Ipc_Gatherv( my_pe_sizes,
                        *scorep_shmem_number_of_root_comms,
                        all_pe_sizes,
                        recvCnts,
                        SCOREP_IPC_UINT32_T,
                        0 );
    SCOREP_Ipc_Gatherv( my_pe_strides,
                        *scorep_shmem_number_of_root_comms,
                        all_pe_strides,
                        recvCnts,
                        SCOREP_IPC_UINT32_T,
                        0 );

    /*
     * Now define groups, communicators, and RMA windows,
     * store their IDs for later mapping
     */
    uint32_t all_global_window_ids[ totalNumberOfRootComms ];
    uint32_t all_global_communicator_ids[ totalNumberOfRootComms ];
    if ( scorep_shmem_my_rank == 0 )
    {
        /* Array to generate group of processing elements */
        uint32_t members[ scorep_shmem_number_of_pes ];

        for ( int32_t pe_index = 0; pe_index < scorep_shmem_number_of_pes; pe_index++ )
        {
            for ( int32_t elem_index = 0; elem_index < recvCnts[ pe_index ]; elem_index++ )
            {
                uint32_t index = offsets[ pe_index ] + elem_index;
                assert( index < totalNumberOfRootComms );

                /* Set up members array */
                for ( uint32_t group_member_index = 0; group_member_index < all_pe_sizes[ index ]; group_member_index++ )
                {
                    /*
                     * Generate array of PE ids which are member of this group
                     * pe_index: ID of start PE
                     */
                    members[ group_member_index ] = pe_index + ( group_member_index * ( 1 << all_pe_strides[ index ] ) );
                }

                /* Create the SHMEM group */
                SCOREP_GroupHandle shmem_group_handle =
                    SCOREP_Definitions_NewUnifiedGroupFrom32( SCOREP_GROUP_SHMEM_GROUP,
                                                              ( all_pe_sizes[ index ] == scorep_shmem_number_of_pes ) ? GROUP_WORLD_NAME : "",
                                                              all_pe_sizes[ index ],
                                                              members );

                /* Create the SHMEM communicator with this group */
                SCOREP_CommunicatorHandle shmem_communicator_handle =
                    SCOREP_Definitions_NewUnifiedCommunicator( shmem_group_handle,
                                                               ( all_pe_sizes[ index ] == scorep_shmem_number_of_pes ) ? COMM_WORLD_NAME : "",
                                                               SCOREP_INVALID_COMMUNICATOR );
                all_global_communicator_ids[ index ] =
                    SCOREP_UNIFIED_HANDLE_TO_ID( shmem_communicator_handle, Communicator );

                /* Create the SHMEM window with this communicator */
                SCOREP_RmaWindowHandle shmem_window_handle =
                    SCOREP_Definitions_NewUnifiedRmaWindow( ( all_pe_sizes[ index ] == scorep_shmem_number_of_pes ) ? WIN_WORLD_NAME : "",
                                                            shmem_communicator_handle );
                all_global_window_ids[ index ] =
                    SCOREP_UNIFIED_HANDLE_TO_ID( shmem_window_handle, RmaWindow );
            }
        }
    }

    /* Send global communicator and RMA window IDs to all processing elements */
    SCOREP_Ipc_Bcast( all_global_communicator_ids, totalNumberOfRootComms, SCOREP_IPC_UINT32_T, 0 );
    SCOREP_Ipc_Bcast( all_global_window_ids, totalNumberOfRootComms, SCOREP_IPC_UINT32_T, 0 );

    /* Apply mappings */
    uint32_t* interim_rma_window_mapping =
        scorep_local_definition_manager.interim_rma_window.mapping;
    uint32_t* interim_communicator_mapping =
        scorep_local_definition_manager.interim_communicator.mapping;
    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_BEGIN( &scorep_local_definition_manager,
                                                         InterimRmaWindow,
                                                         interim_rma_window )
    {
        /* Get associated communicator */
        SCOREP_InterimCommunicatorHandle interim_comm_handle = definition->communicator_handle;
        SCOREP_InterimCommunicatorDef*   interim_comm_def    = SCOREP_LOCAL_HANDLE_DEREF( interim_comm_handle, InterimCommunicator );

        /* Handle only windows and communicators of the SHMEM paradigm */
        if ( interim_comm_def->paradigm_type != SCOREP_PARADIGM_SHMEM )
        {
            continue;
        }

        scorep_shmem_comm_definition_payload* comm_payload =
            SCOREP_InterimCommunicatorHandle_GetPayload( interim_comm_handle );

        /* Handle only RMA windows and communicators that are not 'SELF_LIKE' */
        if ( comm_payload->pe_size == 1 )
        {
            continue;
        }

        /* Write RMA window mapping */
        interim_rma_window_mapping[ definition->sequence_number ] =
            all_global_window_ids[ offsets[ comm_payload->pe_start ] + comm_payload->root_id ];

        /* Write communicator mapping */
        interim_communicator_mapping[ interim_comm_def->sequence_number ] =
            all_global_communicator_ids[ offsets[ comm_payload->pe_start ] + comm_payload->root_id ];
    }
    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_END();
}

/**
 * Define 'self-like' groups, communicators, and RMA windows and apply
 * mapping to them.
 */
static void
create_comm_self_definitions( void )
{
#define COMM_SELF_UNIFIED_ID_INDEX  0
#define WIN_SELF_UNIFIED_ID_INDEX   1
#define LAST_INDEX                  2

    uint32_t global_ids[ LAST_INDEX ];

    if ( scorep_shmem_my_rank == 0 )
    {
        /* Create SHMEM group for self-like communicator */
        SCOREP_GroupHandle self_group_handle =
            SCOREP_Definitions_NewUnifiedGroupFrom32( SCOREP_GROUP_SHMEM_SELF,
                                                      GROUP_SELF_NAME,
                                                      0,
                                                      NULL );

        /* Create the SHMEM communicator with this group */
        SCOREP_CommunicatorHandle self_communicator_handle =
            SCOREP_Definitions_NewUnifiedCommunicator( self_group_handle,
                                                       COMM_SELF_NAME,
                                                       SCOREP_INVALID_COMMUNICATOR );

        /* Create the SHMEM window with this communicator */
        SCOREP_RmaWindowHandle self_window_handle =
            SCOREP_Definitions_NewUnifiedRmaWindow( WIN_SELF_NAME,
                                                    self_communicator_handle );

        global_ids[ COMM_SELF_UNIFIED_ID_INDEX ] =
            SCOREP_UNIFIED_HANDLE_TO_ID( self_communicator_handle, Communicator );
        global_ids[ WIN_SELF_UNIFIED_ID_INDEX ] =
            SCOREP_UNIFIED_HANDLE_TO_ID( self_window_handle, RmaWindow );
    }

    SCOREP_Ipc_Bcast( global_ids, LAST_INDEX, SCOREP_IPC_UINT32_T, 0 );

    /* Apply mappings for 'SELF-LIKE' communicators and windows */
    uint32_t* interim_rma_window_mapping =
        scorep_local_definition_manager.interim_rma_window.mapping;
    uint32_t* interim_communicator_mapping =
        scorep_local_definition_manager.interim_communicator.mapping;
    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_BEGIN( &scorep_local_definition_manager,
                                                         InterimRmaWindow,
                                                         interim_rma_window )
    {
        /* Get associated interim communicator */
        SCOREP_InterimCommunicatorHandle interim_communicator_handle     = definition->communicator_handle;
        SCOREP_InterimCommunicatorDef*   interim_communicator_definition = SCOREP_LOCAL_HANDLE_DEREF( interim_communicator_handle,
                                                                                                      InterimCommunicator );

        /* Handle only windows and communicators of the SHMEM paradigm */
        if ( interim_communicator_definition->paradigm_type != SCOREP_PARADIGM_SHMEM )
        {
            continue;
        }

        scorep_shmem_comm_definition_payload* comm_payload =
            SCOREP_InterimCommunicatorHandle_GetPayload( interim_communicator_handle );

        if ( comm_payload->pe_size == 1 )
        {
            /* Write RMA window mapping */
            interim_rma_window_mapping[ definition->sequence_number ] =
                global_ids[ WIN_SELF_UNIFIED_ID_INDEX ];

            /* Write communicator mapping */
            interim_communicator_mapping[ interim_communicator_definition->sequence_number ] =
                global_ids[ COMM_SELF_UNIFIED_ID_INDEX ];
        }
    }
    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_END();

#undef COMM_SELF_UNIFIED_ID_INDEX
#undef WIN_SELF_UNIFIED_ID_INDEX
#undef LAST_INDEX
}
