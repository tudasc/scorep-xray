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

#include <mpi.h>

#include <scorep_mpi.h>
#include <scorep_unify.h>
#include <scorep_environment.h>
#include <scorep_definitions.h>

#include <scorep_utility/SCOREP_Allocator.h>

/* *INDENT-OFF* */
static void scorep_unify_mpi_master( void );
static void scorep_unify_mpi_servant( void );
/* *INDENT-ON* */


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
    extern SCOREP_DefinitionManager scorep_local_definition_manager;

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

    free( moved_page_ids );
    free( moved_page_fills );
    free( moved_page_starts );
}
