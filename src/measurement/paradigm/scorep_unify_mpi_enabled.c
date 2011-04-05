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

/* *INDENT-OFF* */
static void scorep_unify_mpi_master( void );
static void scorep_unify_mpi_servant( void );
static void scorep_unify_mpi_communicators( void );
static void scorep_unify_mpi_define_groups( uint64_t global_communicator_number, uint64_t max_number_of_self_ids );
/* *INDENT-ON* */

extern SCOREP_DefinitionManager scorep_local_definition_manager;
extern SCOREP_DefinitionManager* scorep_unified_definition_manager;

#define SEND_MAPPING( type ) \
    do \
    { \
        if ( remote_definition_manager->type ## _definition_counter > 0 ) \
        { \
            SCOREP_Mpi_Send( remote_definition_manager->mappings->type ## _mappings, \
                             remote_definition_manager->type ## _definition_counter, \
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
            SCOREP_Mpi_Recv( scorep_local_definition_manager.mappings->type ## _mappings, \
                             scorep_local_definition_manager.type ## _definition_counter, \
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

            SCOREP_Mpi_Recv( page_memory,
                             moved_page_fills[ page ],
                             SCOREP_MPI_UNSIGNED_CHAR,
                             rank,
                             SCOREP_MPI_STATUS_IGNORE );
        }


        // receive remote definitions from rank and store them in remote_definition_manager
        SCOREP_CopyDefinitionsToUnified( remote_definition_manager );
        SCOREP_CreateDefinitionMappings( remote_definition_manager );
        SCOREP_AssignDefinitionMappingsFromUnified( remote_definition_manager );

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
    uint32_t number_of_used_pages =
        SCOREP_Allocator_GetNumberOfUsedPages( scorep_local_definition_manager.page_manager );

    uint32_t* moved_page_ids = calloc( number_of_used_pages, sizeof( *moved_page_ids ) );
    assert( moved_page_ids );
    uint32_t* moved_page_fills = calloc( number_of_used_pages, sizeof( *moved_page_fills ) );
    assert( moved_page_fills );
    void**    moved_page_starts = calloc( number_of_used_pages, sizeof( *moved_page_starts ) );
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

    RECV_MAPPING( region );
    RECV_MAPPING( group );
    RECV_MAPPING( callpath );
    RECV_MAPPING( parameter );

    free( moved_page_ids );
    free( moved_page_fills );
    free( moved_page_starts );
}

/**
   Unifies the communicator ids. The (root, local_id) pair is already unique.
   Arrange all ids in an array sorted by root_process, and second by local id.
   Thus, the sum of communicator definitions on ranks with a lower number
   gives the offset which is added to the local id to get the unified global id.
   Self-communicators are appended to this list.
 */
static void
scorep_unify_mpi_communicators( void )
{
    extern SCOREP_MPICommunicatorHandle
    SCOREP_DefineUnifiedMPICommunicator( const uint64_t globalRootRank,
                                         const uint64_t local_id );

    uint32_t  local_offset           = 0;
    uint32_t  max_number_of_self_ids = 0;                   // Maximum number of references to the self communicator
    // among all processes.
    uint32_t  offset_of_first_self = 0;
    uint32_t* offsets;                                      // Array with the offsets from every rank
    int32_t   comm_world_size = SCOREP_Mpi_GetCommWorldSize();
    int32_t   my_rank         = SCOREP_Mpi_GetRank();

    /* get the maximum number of different MPI_COMM_SELF references
     * on a single process */
    SCOREP_Mpi_Allreduce( &scorep_number_of_self_comms, &max_number_of_self_ids, 1, SCOREP_MPI_UNSIGNED, SCOREP_MPI_MAX );

    /*
     * An MPI_Scan is used here to
     * (a) emulate an MPI_Exscan, needed for the offsets, and
     * (b) compute the first global id for self communicators
     */
    SCOREP_Mpi_Scan( &scorep_number_of_root_comms, &local_offset, 1, SCOREP_MPI_UNSIGNED, SCOREP_MPI_SUM );

    /* Due to MPI_Scan, the local_offset of rank n-1 holds the first self id */
    offset_of_first_self = local_offset;
    SCOREP_Mpi_Bcast( &offset_of_first_self, 1, SCOREP_MPI_UNSIGNED, comm_world_size - 1 );
    /* final step in MPI_Exscan emulation to obtain real offsets */
    local_offset -= scorep_number_of_root_comms;
    assert( local_offset <= offset_of_first_self );

    /* allocate memory for offsets */
    offsets = ( unsigned int* )malloc( comm_world_size * sizeof( unsigned int ) );
    /* gather offsets from processes */
    SCOREP_Mpi_Allgather( &local_offset, 1, SCOREP_MPI_UNSIGNED,
                          offsets, 1, SCOREP_MPI_UNSIGNED );

    /* Create mapping tables
       Every process calculates its own mappings from the offsets.*/
    SCOREP_ALLOC_MAPPINGS_ARRAY( mpi_communicator, &scorep_local_definition_manager );
    SCOREP_DEFINITION_FOREACH_DO( &scorep_local_definition_manager, MPICommunicator, mpi_communicator )
    uint32_t global_id = definition->root_id;
    if ( definition->number_of_ranks > 1 )
    {
        global_id += offsets[ definition->global_root_rank ];
    }
    else
    {
        global_id += offset_of_first_self;
    }
    scorep_local_definition_manager.mappings->mpi_communicator_mappings[ definition->sequence_number ] = global_id;
    SCOREP_DEFINITION_FOREACH_WHILE();

    /* Store unified communicator definitions, reuse offsets buffer for storing the number of
       communcators per rank */
    SCOREP_Mpi_Gather( &scorep_number_of_root_comms, 1, SCOREP_MPI_UNSIGNED, offsets, 1, SCOREP_MPI_UNSIGNED, 0 );

    if ( my_rank == 0 )
    {
        uint64_t rank    = 0;  // Loop over all ranks
        uint64_t comm_id = 0;  // Loop over all communicators per rank

        for ( rank = 0; rank < comm_world_size; rank++ )
        {
            for ( comm_id = 0; comm_id < offsets[ rank ]; comm_id++ )
            {
                SCOREP_DefineUnifiedMPICommunicator( rank, comm_id );
            }
        }
    }

    /* Append information about self communicators */
    if ( my_rank == 0 )
    {
        uint64_t comm_id; // iterate over local self comm reference number

        for ( comm_id = 0; comm_id < max_number_of_self_ids; comm_id++ )
        {
            SCOREP_DefineUnifiedMPICommunicator( -1, comm_id );
        }
    }

    /* free memory for offsets */
    free( offsets );

    /* Create and associate process groups to communicators */
    scorep_unify_mpi_define_groups( offset_of_first_self, max_number_of_self_ids );
}

/**
   Helper function which checks for a global communicator id whether this rank
   is part of the communicator. If it is, it returns its rank according to the
   communicator specified by @a global_comm_id. If it is not,
   it returns -1.
   @param global_comm_id A unified communicator id.
 */
static int
scorep_unify_mpi_is_this_rank_in_communicator( uint32_t global_comm_id )
{
    int32_t i;

    for ( i = 0; i < scorep_local_definition_manager.mpi_communicator_definition_counter; i++ )
    {
        if ( scorep_local_definition_manager.mappings->mpi_communicator_mappings[ i ] == global_comm_id )
        {
            SCOREP_DEFINITION_FOREACH_DO( &scorep_local_definition_manager, MPICommunicator, mpi_communicator )
            if ( definition->sequence_number == i )
            {
                return definition->local_rank;
            }
            SCOREP_DEFINITION_FOREACH_WHILE();
        }
    }
    return -1;
}

/**
   This function assigns the group handle @a group to a unified communicator defininition with
   sequence number @a sequence_number and sets the number of processes acossiated with the
   communicator.
   This function is a helper function for scorep_unify_mpi_define_groups. It expects that all
   communicator are called sequentially and only once. Thus, it remembers the last communicator
   handle, and does not search for the right one.
 */
static void
scorep_map_communicator_to_group( uint64_t           sequence_number,
                                  SCOREP_GroupHandle group,
                                  uint64_t           number_of_ranks )
{
    SCOREP_MPICommunicator_Definition* definition = NULL;

    /* Store handle from last visit. They should be accessed in sequential order */
    static SCOREP_MPICommunicatorHandle handle = SCOREP_INVALID_MPI_COMMUNICATOR;

    /* Initialize handle at first visit */
    if ( handle == SCOREP_INVALID_MPI_COMMUNICATOR )
    {
        handle = scorep_unified_definition_manager->mpi_communicator_definition_head;
    }

    assert( handle != SCOREP_INVALID_MPI_COMMUNICATOR );

    definition = SCOREP_HANDLE_DEREF( handle, MPICommunicator,
                                      scorep_unified_definition_manager->page_manager );

    /* If this assertion fails the sequence numbers in the unified definitions are not
       consistent */
    assert( definition->sequence_number == sequence_number );

    definition->group           = group;
    definition->number_of_ranks = number_of_ranks;

    /* Remember next defintion */
    handle = definition->next;
}

/**
   Collates the information from the communicator definitions on every process to
   obtain the process groups. Creates the group definitions for the process groups and
   creates the mappings from the communicators to the process group. All the data is
   gathered to rank 0. Other ranks to not have the group defintions and mapping from
   communicator afterwards.
   This function requires that the communicator ids are already unified,
 */
static void
scorep_unify_mpi_define_groups( uint64_t global_communicator_number,
                                uint64_t max_number_of_self_ids )
{
    int32_t  comm_world_size = SCOREP_Mpi_GetCommWorldSize();
    int32_t  i               = 0;         // Loop counter outer loop
    int32_t  j               = 0;         // Loop counter inner loop
    int32_t* buffer          = NULL;      // Buffer for collate ranks information
    int32_t  my_rank         = -1;        // Input of this rank: -1 if not part of communicator
    // global rank otherwise
    int32_t  size    = 0;                 // number of ranks members
    int32_t* ranks   = NULL;              // Vector of rank in group
    bool     is_root = ( SCOREP_Mpi_GetRank() == 0 );

    /* Allocate memory for the array to exchange the comminicator belonginge */
    buffer = ( int* )malloc( comm_world_size * sizeof( int ) );
    assert( buffer );

    if ( is_root )
    {
        /* Allocate memory for the rank list. */
        ranks = ( int32_t* )malloc( comm_world_size * sizeof( uint32_t ) );
        assert( ranks );
    }

    for ( i = 0; i < global_communicator_number; i++ )
    {
        SCOREP_GroupHandle group = SCOREP_INVALID_GROUP;
        /* Check whether this rank belongs to the communicator */
        my_rank = scorep_unify_mpi_is_this_rank_in_communicator( i );

        /* gather communicator information */
        SCOREP_Mpi_Gather( &my_rank, 1, SCOREP_MPI_INT, buffer, 1, SCOREP_MPI_INT, 0 );

        if ( is_root )
        {
            /* Create rank list. The list contains at position i the global rank of
               process i in the coommunicator */
            size = 0;
            for ( j = 0; j < comm_world_size; j++ )
            {
                if ( buffer[ j ] != -1 )
                {
                    ranks[ buffer[ j ] ] = j;
                    size++;
                }
            }

            /* Define ranks */
            group = SCOREP_DefineUnifiedMPIGroup( SCOREP_GROUP_MPI_GROUP,
                                                  size, ranks );

            /* Map communicator to group */
            scorep_map_communicator_to_group( i, group, size );
        }
    }

    if ( is_root )
    {
        /* Create group for comm self */
        SCOREP_GroupHandle self
            = SCOREP_DefineUnifiedMPIGroup( SCOREP_GROUP_COMM_SELF, 0, NULL );

        /* Map self communicators */
        for ( i = 0; i < max_number_of_self_ids; i++ )
        {
            scorep_map_communicator_to_group( i + global_communicator_number, self, 1 );
        }
    }

    /* clean up */
    free( ranks );
    free( buffer );
}
