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
#include <stdint.h>
#include <inttypes.h>

#include <scorep_ipc.h>
#include <scorep_unify.h>
#include <scorep_environment.h>
#include <SCOREP_Definitions.h>
#include <SCOREP_Definitions.h>
#include <SCOREP_Memory.h>

#include <UTILS_Error.h>


static void
unify_mpp_hierarchical( void );
static void
apply_mappings_to_local_manager( void );

void
SCOREP_Unify_Mpp( void )
{
    unify_mpp_hierarchical();

    /*
     * Now apply the mappings from the final unified definitions
     * to the local definitions
     */
    apply_mappings_to_local_manager();
}

static void
receive_mappings( int rank );

static int
calculate_comm_partners( int*  parent,
                         int** children );
static void
receive_and_unify_remote_definitions( int                           rank,
                                      SCOREP_DefinitionManager*     remote_definition_manager,
                                      SCOREP_Allocator_PageManager* remote_page_manager,
                                      uint32_t**                    moved_page_ids,
                                      uint32_t**                    moved_page_fills,
                                      uint32_t*                     max_number_of_pages );
static void
send_local_unified_definitions_to_parent( int        parent,
                                          uint32_t** moved_page_ids,
                                          uint32_t** moved_page_fills,
                                          uint32_t*  max_number_of_pages );
static void
apply_and_send_mappings( int                       rank,
                         SCOREP_DefinitionManager* remote_definition_manager );


/**
 * Hierarchical unify the definitions within MPI_COMM_WORLD.
 *
 * Uses an embedded hypercube inside COMM_WORLD.
 * Phase 1 is to purculate our own and all of my children definitions up to my
 *         parent.
 * Phase 2 is to perculate the mappings from global definitions to my
 *         definitions down from my parent to all my children.
 */
void
unify_mpp_hierarchical( void )
{
    int  num_children;
    int  child;
    int  parent;
    int  me       = SCOREP_Ipc_GetRank();
    int* children = NULL;

    num_children = calculate_comm_partners( &parent, &children );
    SCOREP_DefinitionManager* remote_definition_managers =
        calloc( num_children, sizeof( *remote_definition_managers ) );
    SCOREP_Allocator_PageManager* remote_page_manager =
        SCOREP_Memory_CreateMovedPagedMemory();

    uint32_t* moved_page_ids      = NULL;
    uint32_t* moved_page_fills    = NULL;
    uint32_t  max_number_of_pages = 0;

    /* Phase 1a: Get all definitions from my children and unify them into my. */
    for ( child = 0; child < num_children; child++ )
    {
        receive_and_unify_remote_definitions( children[ child ],
                                              &remote_definition_managers[ child ],
                                              remote_page_manager,
                                              &moved_page_ids,
                                              &moved_page_fills,
                                              &max_number_of_pages );
    }

    /*
     * Phase 1b & 2a: Purculate my unified definitions up to my parent and receive
     *           the mappings from him.
     */
    if ( parent != me )
    {
        /* Phase 1b: Send our local unified definition manager to our parent. */
        send_local_unified_definitions_to_parent( parent,
                                                  &moved_page_ids,
                                                  &moved_page_fills,
                                                  &max_number_of_pages );

        /*
         * Phase 2a: Get the mapping from our parent and store them in the local
         * unified definition manager.
         */
        receive_mappings( parent );
    }

    free( moved_page_ids );
    free( moved_page_fills );

    /*
     * Phase 2b: Apply the mapping to all of my children and send the resulting
     *           mapping to our children.
     */
    for ( child = num_children; child--; )
    {
        /*
         * Apply our mapping to the mappings of the child and send them to
         * the child.
         */
        apply_and_send_mappings( children[ child ],
                                 &remote_definition_managers[ child ] );
    }

    free( children );

    SCOREP_Allocator_DeletePageManager( remote_page_manager );
    free( remote_definition_managers );
}

/**
 * Calculate the smallest power-of-two number which is greater/equal to @a v.
 */
static unsigned int
npot( unsigned int v )
{
    v--;
    v |= v >>  1;
    v |= v >>  2;
    v |= v >>  4;
    v |= v >>  8;
    v |= v >> 16;
    return v + 1;
}


/**
 * Calculate the communication partners of me in the hypercube
 *
 * @return the number of children.
 */
int
calculate_comm_partners( int*  parent,
                         int** children )
{
    unsigned int size     = SCOREP_Ipc_GetSize();
    unsigned int me       = SCOREP_Ipc_GetRank();
    unsigned int size_pot = npot( size );

    unsigned int d;
    unsigned int number_of_children = 0;

    /* Be your own parent, ie. the root, by default */
    *parent = me;

    /* Calculate the number of children for me */
    for ( d = 1; d; d <<= 1 )
    {
        /* Actually break condition */
        if ( d > size_pot )
        {
            break;
        }

        /* Check if we are actually a child of someone */
        if ( me & d )
        {
            /* Yes, set the parent to our real one, and stop */
            *parent = me ^ d;
            break;
        }

        /* Only count real children, of the virtual hypercube */
        if ( ( me ^ d ) < size )
        {
            number_of_children++;
        }
    }

    /* Put the ranks of all children into a list and return */
    *children = malloc( sizeof( **children ) * number_of_children );
    unsigned int child = number_of_children;

    d >>= 1;
    while ( d )
    {
        if ( ( me ^ d ) < size )
        {
            ( *children )[ --child ] = me ^ d;
        }
        d >>= 1;
    }

    return number_of_children;
}


void
receive_and_unify_remote_definitions( int                           rank,
                                      SCOREP_DefinitionManager*     remote_definition_manager,
                                      SCOREP_Allocator_PageManager* remote_page_manager,
                                      uint32_t**                    moved_page_ids,
                                      uint32_t**                    moved_page_fills,
                                      uint32_t*                     max_number_of_pages )
{
    // 1) Receive the remote definition manager
    SCOREP_Ipc_Recv( remote_definition_manager,
                     sizeof( *remote_definition_manager ),
                     SCOREP_IPC_BYTE,
                     rank );

    // 2) Create and receive page manager infos

    // page_manager member was overwritten by recv
    remote_definition_manager->page_manager = remote_page_manager;

    // 3) Get the number of pages we get.
    uint32_t number_of_pages;
    SCOREP_Ipc_Recv( &number_of_pages,
                     1,
                     SCOREP_IPC_UINT32_T,
                     rank );

    /* Resize receive buffers if needed */
    if ( number_of_pages > *max_number_of_pages )
    {
        *moved_page_ids = realloc( *moved_page_ids,
                                   number_of_pages
                                   * sizeof( **moved_page_ids ) );
        UTILS_BUG_ON( *moved_page_ids == NULL,
                      "Can't allocate memory for moved_page_ids array of size: %u",
                      number_of_pages );
        *moved_page_fills = realloc( *moved_page_fills,
                                     number_of_pages
                                     * sizeof( **moved_page_fills ) );
        UTILS_BUG_ON( *moved_page_fills == NULL,
                      "Can't allocate memory for moved_page_fills array of size: %u",
                      number_of_pages );
        *max_number_of_pages = number_of_pages;
    }

    // 4a) Get the remote page ids
    SCOREP_Ipc_Recv( *moved_page_ids,
                     number_of_pages,
                     SCOREP_IPC_UINT32_T,
                     rank );

    // 4b) Get page fill of remote pages
    SCOREP_Ipc_Recv( *moved_page_fills,
                     number_of_pages,
                     SCOREP_IPC_UINT32_T,
                     rank );

    // 5) Receive all remote pages from rank
    for ( uint32_t page = 0; page < number_of_pages; page++ )
    {
        /* Allocate local page */
        void* page_memory = SCOREP_Allocator_AllocMovedPage(
            remote_page_manager,
            ( *moved_page_ids )[ page ],
            ( *moved_page_fills )[ page ] );
        if ( !page_memory )
        {
            // aborts
            SCOREP_Memory_HandleOutOfMemory();
        }

        SCOREP_Ipc_Recv( page_memory,
                         ( *moved_page_fills )[ page ],
                         SCOREP_IPC_BYTE,
                         rank );
    }

    // 6) Unify received remote definitions to our one
    SCOREP_CopyDefinitionsToUnified( remote_definition_manager );
    SCOREP_CreateDefinitionMappings( remote_definition_manager );
    SCOREP_AssignDefinitionMappingsFromUnified( remote_definition_manager );

    SCOREP_Allocator_Free( remote_page_manager );
    remote_definition_manager->page_manager = NULL;
}


void
send_local_unified_definitions_to_parent( int        parent,
                                          uint32_t** moved_page_ids,
                                          uint32_t** moved_page_fills,
                                          uint32_t*  max_number_of_pages )
{
    // 1) Send my local unified definition manager to my parent
    SCOREP_Ipc_Send( scorep_unified_definition_manager,
                     sizeof( *scorep_unified_definition_manager ),
                     SCOREP_IPC_BYTE,
                     parent );

    // 2) Send the page manager infos to my parent
    uint32_t number_of_used_pages =
        SCOREP_Allocator_GetNumberOfUsedPages( scorep_unified_definition_manager->page_manager );

    if ( number_of_used_pages > *max_number_of_pages )
    {
        *moved_page_ids = realloc( *moved_page_ids,
                                   number_of_used_pages
                                   * sizeof( **moved_page_ids ) );
        UTILS_BUG_ON( *moved_page_ids == NULL,
                      "Can't allocate memory for moved_page_ids array of size: %u",
                      number_of_used_pages );

        *moved_page_fills = realloc( *moved_page_fills,
                                     number_of_used_pages
                                     * sizeof( **moved_page_fills ) );
        UTILS_BUG_ON( *moved_page_fills == NULL,
                      "Can't allocate memory for moved_page_fills array of size: %u",
                      number_of_used_pages );

        *max_number_of_pages = number_of_used_pages;
    }

    void** moved_page_starts = calloc( number_of_used_pages,
                                       sizeof( *moved_page_starts ) );
    UTILS_BUG_ON( moved_page_starts == NULL,
                  "Can't allocate memory for moved_page_starts array of size: %u",
                  number_of_used_pages );

    SCOREP_Allocator_GetPageInfos(
        scorep_unified_definition_manager->page_manager,
        *moved_page_ids,
        *moved_page_fills,
        moved_page_starts );

    SCOREP_Ipc_Send( &number_of_used_pages,
                     1, SCOREP_IPC_UINT32_T, parent );
    SCOREP_Ipc_Send( *moved_page_ids,
                     number_of_used_pages,
                     SCOREP_IPC_UINT32_T, parent );
    SCOREP_Ipc_Send( *moved_page_fills,
                     number_of_used_pages,
                     SCOREP_IPC_UINT32_T, parent );

    // 3) Send all pages to my parent
    for ( uint32_t page = 0; page < number_of_used_pages; page++ )
    {
        SCOREP_Ipc_Send( moved_page_starts[ page ],
                         ( *moved_page_fills )[ page ],
                         SCOREP_IPC_BYTE, parent );
    }

    // 4) Prepare manager to receive my parents mappings
    SCOREP_CreateDefinitionMappings( scorep_unified_definition_manager );

    free( moved_page_starts );
}


void
receive_mappings( int rank )
{
    #define DEF_WITH_MAPPING( Type, type ) \
    if ( scorep_unified_definition_manager->type.counter > 0 ) \
    { \
        SCOREP_Ipc_Recv( \
            scorep_unified_definition_manager->type.mapping, \
            scorep_unified_definition_manager->type.counter, \
            SCOREP_IPC_UINT32_T, \
            rank ); \
    }
    SCOREP_LIST_OF_DEFS_WITH_MAPPINGS
    #undef DEF_WITH_MAPPING
}

void
apply_mappings_to_local_manager( void )
{
    #define DEF_WITH_MAPPING( Type, type ) \
    if ( scorep_local_definition_manager.type.counter > 0 \
         && scorep_unified_definition_manager->type.mapping ) \
    { \
        for ( uint32_t i = 0; \
              i < scorep_local_definition_manager.type.counter; \
              i++ ) \
        { \
            if ( scorep_local_definition_manager.type.mapping[ i ] != UINT32_MAX ) \
            { \
                scorep_local_definition_manager.type.mapping[ i ] = \
                    scorep_unified_definition_manager->type.mapping[ \
                        scorep_local_definition_manager.type.mapping[ i ] ]; \
            } \
        } \
    }
    SCOREP_LIST_OF_DEFS_WITH_MAPPINGS
    #undef DEF_WITH_MAPPING
}

void
apply_and_send_mappings( int                       rank,
                         SCOREP_DefinitionManager* remote_definition_manager )
{
    #define DEF_WITH_MAPPING( Type, type ) \
    if ( remote_definition_manager->type.counter > 0 ) \
    { \
        if ( scorep_unified_definition_manager->type.mapping ) \
        { \
            for ( uint32_t i = 0; \
                  i < remote_definition_manager->type.counter; \
                  i++ ) \
            { \
                remote_definition_manager->type.mapping[ i ] = \
                    scorep_unified_definition_manager->type.mapping[ \
                        remote_definition_manager->type.mapping[ i ] ]; \
            } \
        } \
        SCOREP_Ipc_Send( \
            remote_definition_manager->type.mapping, \
            remote_definition_manager->type.counter, \
            SCOREP_IPC_UINT32_T, \
            rank ); \
    }
    SCOREP_LIST_OF_DEFS_WITH_MAPPINGS
    #undef DEF_WITH_MAPPING
}
