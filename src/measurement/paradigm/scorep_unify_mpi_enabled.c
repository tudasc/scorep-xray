/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
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
 * @file       scorep_unify_mpi_enabled.c
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

#include <scorep_mpi.h>
#include <scorep_unify.h>
#include <scorep_environment.h>
#include <scorep_definitions.h>
#include <scorep_definition_macros.h>
#include <scorep_definition_structs.h>
#include <SCOREP_Definitions.h>

#include <scorep_utility/SCOREP_Utils.h>

static void
scorep_unify_mpi_master( void );

static void
scorep_unify_mpi_servant( void );

static void
scorep_unify_mpi_communicators( void );

extern SCOREP_DefinitionManager  scorep_local_definition_manager;
extern SCOREP_DefinitionManager* scorep_unified_definition_manager;

#define SEND_MAPPING( type ) \
    do \
    { \
        if ( remote_definition_manager->type ## _definition_counter > 0 ) \
        { \
            SCOREP_Mpi_Send( remote_definition_manager \
                             ->mappings->type ## _mappings, \
                             remote_definition_manager \
                             ->type ## _definition_counter, \
                             SCOREP_MPI_UNSIGNED, \
                             rank ); \
        } \
    } \
    while ( 0 )


#define RECV_MAPPING( type ) \
    do \
    { \
        if ( scorep_local_definition_manager.type ## _definition_counter > 0 ) \
        { \
            SCOREP_Mpi_Recv( scorep_local_definition_manager. \
                             mappings->type ## _mappings, \
                             scorep_local_definition_manager. \
                             type ## _definition_counter, \
                             SCOREP_MPI_UNSIGNED, \
                             0, \
                             SCOREP_MPI_STATUS_IGNORE ); \
        } \
    } \
    while ( 0 )


void
SCOREP_Mpi_Unify( void )
{
    if ( SCOREP_Mpi_GetRank() == 0 )
    {
        scorep_unify_mpi_master();
    }
    else
    {
        scorep_unify_mpi_servant();
    }
    scorep_unify_mpi_communicators();
}


static void
scorep_unify_mpi_master( void )
{
    SCOREP_Unify_Locally();

    SCOREP_DefinitionManager*     remote_definition_manager =
        malloc( sizeof( *remote_definition_manager ) );
    SCOREP_Allocator_PageManager* remote_page_manager =
        SCOREP_Memory_CreateMovedPagedMemory();
    assert( remote_definition_manager );
    uint32_t* moved_page_ids      = NULL;
    uint32_t* moved_page_fills    = NULL;
    uint32_t  max_number_of_pages = 0;
    for ( int rank = 1; rank < SCOREP_Mpi_GetCommWorldSize(); ++rank )
    {
        // 1) receive the remote definition manager
        SCOREP_Mpi_Recv( remote_definition_manager,
                         sizeof( *remote_definition_manager ),
                         SCOREP_MPI_UNSIGNED_CHAR,
                         rank,
                         SCOREP_MPI_STATUS_IGNORE );

        // 2) create and receive page manager infos

        // page_manager member was overwritten by recv
        remote_definition_manager->page_manager = remote_page_manager;

        uint32_t number_of_pages;
        SCOREP_Mpi_Recv( &number_of_pages,
                         1,
                         SCOREP_MPI_UNSIGNED,
                         rank,
                         SCOREP_MPI_STATUS_IGNORE );

        if ( number_of_pages > max_number_of_pages )
        {
            moved_page_ids = realloc( moved_page_ids,
                                      number_of_pages
                                      * sizeof( *moved_page_ids ) );
            assert( moved_page_ids );
            moved_page_fills = realloc( moved_page_fills,
                                        number_of_pages
                                        * sizeof( *moved_page_fills ) );
            assert( moved_page_fills );
            max_number_of_pages = number_of_pages;
        }

        SCOREP_Mpi_Recv( moved_page_ids,
                         number_of_pages,
                         SCOREP_MPI_UNSIGNED,
                         rank,
                         SCOREP_MPI_STATUS_IGNORE );

        SCOREP_Mpi_Recv( moved_page_fills,
                         number_of_pages,
                         SCOREP_MPI_UNSIGNED,
                         rank,
                         SCOREP_MPI_STATUS_IGNORE );

        // 3) receive all pages from rank
        for ( uint32_t page = 0; page < number_of_pages; page++ )
        {
            void* page_memory = SCOREP_Allocator_AllocMovedPage(
                remote_page_manager,
                moved_page_ids[ page ],
                moved_page_fills[ page ] );
            if ( !page_memory )
            {
                // aborts
                SCOREP_Memory_HandleOutOfMemory();
            }

            SCOREP_Mpi_Recv( page_memory,
                             moved_page_fills[ page ],
                             SCOREP_MPI_UNSIGNED_CHAR,
                             rank,
                             SCOREP_MPI_STATUS_IGNORE );
        }

        /**
         * receive remote definitions from rank and store them in
         * remote_definition_manager
         */
        SCOREP_CopyDefinitionsToUnified( remote_definition_manager );
        SCOREP_CreateDefinitionMappings( remote_definition_manager );
        SCOREP_AssignDefinitionMappingsFromUnified( remote_definition_manager );

        SEND_MAPPING( string );
        SEND_MAPPING( region );
        SEND_MAPPING( group );
        SEND_MAPPING( callpath );
        SEND_MAPPING( parameter );

        SCOREP_DestroyDefinitionMappings( remote_definition_manager );

        SCOREP_Allocator_Free( remote_page_manager );
    }
    free( moved_page_ids );
    free( moved_page_fills );

    SCOREP_Allocator_DeletePageManager( remote_page_manager );
    free( remote_definition_manager );
}


static void
scorep_unify_mpi_servant( void )
{
    // 1) send my local definition manager to root
    SCOREP_Mpi_Send( &scorep_local_definition_manager,
                     sizeof( scorep_local_definition_manager ),
                     SCOREP_MPI_UNSIGNED_CHAR,
                     0 );

    // 2) send the page manager infos to root
    uint32_t number_of_used_pages = SCOREP_Allocator_GetNumberOfUsedPages(
        scorep_local_definition_manager.page_manager );

    uint32_t* moved_page_ids = calloc( number_of_used_pages,
                                       sizeof( *moved_page_ids ) );
    assert( moved_page_ids );
    uint32_t* moved_page_fills = calloc( number_of_used_pages,
                                         sizeof( *moved_page_fills ) );
    assert( moved_page_fills );
    void** moved_page_starts = calloc( number_of_used_pages,
                                       sizeof( *moved_page_starts ) );
    assert( moved_page_starts );
    SCOREP_Allocator_GetPageInfos( scorep_local_definition_manager.page_manager,
                                   moved_page_ids,
                                   moved_page_fills,
                                   moved_page_starts );

    SCOREP_Mpi_Send( &number_of_used_pages,
                     1, SCOREP_MPI_UNSIGNED, 0 );
    SCOREP_Mpi_Send( moved_page_ids,
                     number_of_used_pages,
                     SCOREP_MPI_UNSIGNED, 0 );
    SCOREP_Mpi_Send( moved_page_fills,
                     number_of_used_pages,
                     SCOREP_MPI_UNSIGNED, 0 );

    // 3) send all pages to root
    for ( uint32_t page = 0; page < number_of_used_pages; page++ )
    {
        SCOREP_Mpi_Send( moved_page_starts[ page ],
                         moved_page_fills[ page ],
                         SCOREP_MPI_UNSIGNED_CHAR, 0 );
    }

    // 4) receive all mappings from root
    SCOREP_CreateDefinitionMappings( &scorep_local_definition_manager );

    RECV_MAPPING( string );
    RECV_MAPPING( region );
    RECV_MAPPING( group );
    RECV_MAPPING( callpath );
    RECV_MAPPING( parameter );

    free( moved_page_ids );
    free( moved_page_fills );
    free( moved_page_starts );
}

static uint32_t
scorep_unify_mpi_communicators_create_local_mapping( uint32_t comm_world_size,
                                                     uint32_t rank );

static void
scorep_unify_mpi_communicators_define_comms( uint32_t comm_world_size,
                                             uint32_t rank,
                                             uint32_t total_number_of_root_comms );

static void
scorep_unify_mpi_communicators_define_self_likes( uint32_t comm_world_size,
                                                  uint32_t rank );

/**
 * Unifies the communicator ids. The (root, local_id) pair is already unique.
 * Arrange all ids in an array sorted by root_process, and second by local id.
 * Thus, the sum of communicator definitions on ranks with a lower number
 * gives the offset which is added to the local id to get the unified global id.
 * Self-communicators are appended to this list.
 */
static void
scorep_unify_mpi_communicators( void )
{
    uint32_t comm_world_size = SCOREP_Mpi_GetCommWorldSize();
    uint32_t rank            = SCOREP_Mpi_GetRank();
    uint32_t total_number_of_root_comms;

    /* 1) Generate mapping from local to global communicators */
    total_number_of_root_comms =
        scorep_unify_mpi_communicators_create_local_mapping( comm_world_size,
                                                             rank );

    /* 2) Generate definitions for MPI groups and non-self-like communicators */
    scorep_unify_mpi_communicators_define_comms( comm_world_size,
                                                 rank,
                                                 total_number_of_root_comms );

    /* 3) Append information about self communicators */
    scorep_unify_mpi_communicators_define_self_likes( comm_world_size,
                                                      rank );
}

uint32_t
scorep_unify_mpi_communicators_create_local_mapping( uint32_t comm_world_size,
                                                     uint32_t rank )
{
    /* Allocate memory for offsets */
    uint32_t* offsets = calloc( comm_world_size, sizeof( *offsets ) );
    assert( offsets );

    /* Gather communicator counts from all ranks */
    SCOREP_Mpi_Allgather( &scorep_number_of_root_comms,
                          1, SCOREP_MPI_UNSIGNED,
                          offsets,
                          1, SCOREP_MPI_UNSIGNED );

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
    SCOREP_ALLOC_MAPPINGS_ARRAY( local_mpi_communicator,
                                 &scorep_local_definition_manager );
    SCOREP_DEFINITION_FOREACH_DO( &scorep_local_definition_manager,
                                  LocalMPICommunicator,
                                  local_mpi_communicator )
    {
        uint32_t global_comm_id = definition->root_id;
        if ( !definition->is_self_like )
        {
            global_comm_id += offsets[ definition->global_root_rank ];
        }
        else
        {
            global_comm_id += total_number_of_root_comms;
            assert( definition->global_root_rank == rank );
        }
        scorep_local_definition_manager.mappings
        ->local_mpi_communicator_mappings[ definition->sequence_number ]
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
 *
 * @return My local rank in this communicator, or -1 when I'm not in
 *         this communicator.
 */
static int
scorep_unify_mpi_is_this_rank_in_communicator( uint32_t global_comm_id )
{
    SCOREP_DEFINITION_FOREACH_DO( &scorep_local_definition_manager,
                                  LocalMPICommunicator,
                                  local_mpi_communicator )
    {
        if ( global_comm_id ==
             scorep_local_definition_manager.mappings->
             local_mpi_communicator_mappings[ definition->sequence_number ] )
        {
            return definition->local_rank;
        }
    }
    SCOREP_DEFINITION_FOREACH_WHILE();

    return -1;
}

void
scorep_unify_mpi_communicators_define_comms( uint32_t comm_world_size,
                                             uint32_t rank,
                                             uint32_t total_number_of_root_comms )
{
    int32_t* ranks_in_comm = NULL;   // Vector for the communicator belongings
                                     // (only significant in root)
    int32_t* ranks_in_group = NULL;  // Vector for the MPI group
                                     // (only significant in root)

    if ( rank == 0 )
    {
        /* Allocate memory for the arrays. */

        ranks_in_comm = calloc( comm_world_size, sizeof( *ranks_in_comm ) );
        assert( ranks_in_comm );

        ranks_in_group = calloc( comm_world_size, sizeof( *ranks_in_group ) );
        assert( ranks_in_group );

        /* Define the list of locations which are MPI ranks. */
        for ( uint32_t i = 0; i < comm_world_size; i++ )
        {
            /*
             * If we support MPI_THREADED_FUNNELED, this needs to be the
             * location, wich has called MPI_Init/MPI_Thread_init.
             * For the moment, the location and rank ids match.
             */
            ranks_in_group[ i ] = i;
        }
        SCOREP_DefineUnifiedMPIGroup( SCOREP_GROUP_MPI_LOCATIONS,
                                      comm_world_size, ranks_in_group );

        assert( scorep_unified_definition_manager->mpi_communicator_definition_counter == 0 );
    }

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
         * It's either my local rank in this communicator, or -1 if I'm not
         * a member of this communicator.
         */
        int32_t my_rank_in_comm =
            scorep_unify_mpi_is_this_rank_in_communicator( global_comm_id );

        /* gather communicator information */
        SCOREP_Mpi_Gather( &my_rank_in_comm,
                           1, SCOREP_MPI_INT,
                           ranks_in_comm,
                           1, SCOREP_MPI_INT,
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
                if ( ranks_in_comm[ i ] != -1 )
                {
                    ranks_in_group[ ranks_in_comm[ i ] ] = i;
                    size++;
                }
            }

            /* Define the MPI group */
            SCOREP_GroupHandle group =
                SCOREP_DefineUnifiedMPIGroup( SCOREP_GROUP_MPI_GROUP,
                                              size, ranks_in_group );

            /* Define the global MPI communicator with this group */
            SCOREP_MPICommunicatorHandle handle =
                SCOREP_DefineUnifiedMPICommunicator( group );
            assert( SCOREP_UNIFIED_HANDLE_TO_ID( handle, MPICommunicator ) ==
                    global_comm_id );
        }
    }

    free( ranks_in_group );
    free( ranks_in_comm );
}

void
scorep_unify_mpi_communicators_define_self_likes( uint32_t comm_world_size,
                                                  uint32_t rank )
{
    /*
     * Get the maximum number of different MPI_COMM_SELF references
     * on a single process
     */
    uint32_t max_number_of_self_ids = 0;
    SCOREP_Mpi_Reduce( &scorep_number_of_self_comms,
                       &max_number_of_self_ids,
                       1,
                       SCOREP_MPI_UNSIGNED,
                       SCOREP_MPI_MAX,
                       0 );

    if ( rank == 0 )
    {
        uint32_t number_of_comms =
            scorep_unified_definition_manager->mpi_communicator_definition_counter;

        /* Create group for comm self */
        SCOREP_GroupHandle self = SCOREP_DefineUnifiedMPIGroup(
            SCOREP_GROUP_COMM_SELF, 0, NULL );

        for ( uint32_t i = 0; i < max_number_of_self_ids; i++ )
        {
            SCOREP_MPICommunicatorHandle handle =
                SCOREP_DefineUnifiedMPICommunicator( self );
            assert( SCOREP_UNIFIED_HANDLE_TO_ID( handle, MPICommunicator ) ==
                    number_of_comms + i );
        }
    }
}
