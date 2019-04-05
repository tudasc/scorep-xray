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
 * Copyright (c) 2009-2016,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2013,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2013, 2015,
 * Technische Universitaet Muenchen, Germany
 *
 * Copyright (c) 2016,
 * Technische Universitaet Darmstadt, Germany
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


#include <SCOREP_Definitions.h>


#include "scorep_environment.h"
#include "scorep_runtime_management.h"
#include <scorep/SCOREP_PublicTypes.h>
#include <SCOREP_Mutex.h>
#include <SCOREP_Memory.h>
#include <scorep_system_tree_sequence.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "scorep_ipc.h"
#include <jenkins_hash.h>
#include <tracing/SCOREP_Tracing.h>

#include <UTILS_Error.h>

SCOREP_DefinitionManager  scorep_local_definition_manager;
SCOREP_DefinitionManager* scorep_unified_definition_manager = 0;
static bool               definitions_initialized           = false;


/* global definition lock */
static SCOREP_Mutex definitions_lock;

void
SCOREP_Definitions_Lock( void )
{
    SCOREP_MutexLock( definitions_lock );
}

void
SCOREP_Definitions_Unlock( void )
{
    SCOREP_MutexUnlock( definitions_lock );
}

void
SCOREP_Definitions_Initialize( void )
{
    if ( definitions_initialized )
    {
        return;
    }
    definitions_initialized = true;

    SCOREP_MutexCreate( &definitions_lock );
    scorep_definitions_create_interim_communicator_counter_lock();

    SCOREP_DefinitionManager* local_definition_manager = &scorep_local_definition_manager;
    SCOREP_Definitions_InitializeDefinitionManager( &local_definition_manager,
                                                    SCOREP_Memory_GetLocalDefinitionPageManager(),
                                                    false );

    /* ensure, that the empty string gets id 0 */
    SCOREP_Definitions_NewString( "" );
}


bool
SCOREP_Definitions_Initialized( void )
{
    return definitions_initialized;
}


void
scorep_definitions_manager_entry_alloc_hash_table( scorep_definitions_manager_entry* entry,
                                                   uint32_t                          hashTablePower )
{
    UTILS_BUG_ON( hashTablePower > 15,
                  "Hash table too big: %u", hashTablePower );
    entry->hash_table_mask = hashmask( hashTablePower );
    entry->hash_table      = calloc( hashsize( hashTablePower ), sizeof( *entry->hash_table ) );
    UTILS_BUG_ON( entry->hash_table == 0,
                  "Can't allocate hash table of size %u",
                  hashTablePower );
}

#define SCOREP_DEFINITIONS_DEFAULT_HASH_TABLE_POWER ( 8 )

/**
 * Initializes a manager entry named @a type in the definition manager @a
 * definition_manager.
 */
#define SCOREP_DEFINITIONS_MANAGER_INIT_MEMBER( definition_manager, type ) \
    scorep_definitions_manager_init_entry( &( definition_manager )->type )


/**
 * Allocates the hash table for type @a type in the given definition manager
 * with the default hash table size of @a SCOREP_DEFINITIONS_DEFAULT_HASH_TABLE_POWER.
 */
#define SCOREP_DEFINITIONS_MANAGER_ALLOC_MEMBER_HASH_TABLE( definition_manager, type ) \
    scorep_definitions_manager_entry_alloc_hash_table( &( definition_manager )->type, \
                                                       SCOREP_DEFINITIONS_DEFAULT_HASH_TABLE_POWER )


void
SCOREP_Definitions_InitializeDefinitionManager( SCOREP_DefinitionManager**    definitionManager,
                                                SCOREP_Allocator_PageManager* pageManager,
                                                bool                          allocHashTables )
{
    UTILS_ASSERT( definitionManager );
    UTILS_ASSERT( pageManager );

    if ( *definitionManager )
    {
        memset( *definitionManager, 0, sizeof( SCOREP_DefinitionManager ) );
    }
    else
    {
        *definitionManager = calloc( 1, sizeof( SCOREP_DefinitionManager ) );
        UTILS_BUG_ON( *definitionManager == 0,
                      "Can't allocate definition manager" );
    }

    ( *definitionManager )->page_manager = pageManager;

    SCOREP_DEFINITIONS_MANAGER_INIT_MEMBER( *definitionManager, string );
    SCOREP_DEFINITIONS_MANAGER_ALLOC_MEMBER_HASH_TABLE( *definitionManager, string );

    SCOREP_DEFINITIONS_MANAGER_INIT_MEMBER( *definitionManager, source_file );
    SCOREP_DEFINITIONS_MANAGER_ALLOC_MEMBER_HASH_TABLE( *definitionManager, source_file );

    SCOREP_DEFINITIONS_MANAGER_INIT_MEMBER( *definitionManager, system_tree_node );
    SCOREP_DEFINITIONS_MANAGER_INIT_MEMBER( *definitionManager, system_tree_node_property );
    SCOREP_DEFINITIONS_MANAGER_INIT_MEMBER( *definitionManager, location_group );
    SCOREP_DEFINITIONS_MANAGER_INIT_MEMBER( *definitionManager, location );
    SCOREP_DEFINITIONS_MANAGER_INIT_MEMBER( *definitionManager, region );
    SCOREP_DEFINITIONS_MANAGER_INIT_MEMBER( *definitionManager, group );
    SCOREP_DEFINITIONS_MANAGER_INIT_MEMBER( *definitionManager, interim_communicator );
    SCOREP_DEFINITIONS_MANAGER_INIT_MEMBER( *definitionManager, communicator );
    SCOREP_DEFINITIONS_MANAGER_INIT_MEMBER( *definitionManager, rma_window );
    SCOREP_DEFINITIONS_MANAGER_INIT_MEMBER( *definitionManager, cartesian_coords );
    SCOREP_DEFINITIONS_MANAGER_INIT_MEMBER( *definitionManager, cartesian_topology );
    SCOREP_DEFINITIONS_MANAGER_INIT_MEMBER( *definitionManager, metric );
    SCOREP_DEFINITIONS_MANAGER_INIT_MEMBER( *definitionManager, sampling_set );
    SCOREP_DEFINITIONS_MANAGER_INIT_MEMBER( *definitionManager, sampling_set_recorder );
    SCOREP_DEFINITIONS_MANAGER_INIT_MEMBER( *definitionManager, io_handle );
    SCOREP_DEFINITIONS_MANAGER_INIT_MEMBER( *definitionManager, io_file );
    SCOREP_DEFINITIONS_MANAGER_INIT_MEMBER( *definitionManager, io_file_property );
    SCOREP_DEFINITIONS_MANAGER_INIT_MEMBER( *definitionManager, marker_group );
    SCOREP_DEFINITIONS_MANAGER_INIT_MEMBER( *definitionManager, marker );
    SCOREP_DEFINITIONS_MANAGER_INIT_MEMBER( *definitionManager, parameter );
    SCOREP_DEFINITIONS_MANAGER_ALLOC_MEMBER_HASH_TABLE( *definitionManager, parameter );
    SCOREP_DEFINITIONS_MANAGER_INIT_MEMBER( *definitionManager, callpath );
    SCOREP_DEFINITIONS_MANAGER_INIT_MEMBER( *definitionManager, property );
    SCOREP_DEFINITIONS_MANAGER_INIT_MEMBER( *definitionManager, attribute );
    SCOREP_DEFINITIONS_MANAGER_ALLOC_MEMBER_HASH_TABLE( *definitionManager, attribute );
    SCOREP_DEFINITIONS_MANAGER_INIT_MEMBER( *definitionManager, location_property );
    SCOREP_DEFINITIONS_MANAGER_INIT_MEMBER( *definitionManager, source_code_location );
    SCOREP_DEFINITIONS_MANAGER_ALLOC_MEMBER_HASH_TABLE( *definitionManager, source_code_location );
    SCOREP_DEFINITIONS_MANAGER_INIT_MEMBER( *definitionManager, calling_context );
    SCOREP_DEFINITIONS_MANAGER_INIT_MEMBER( *definitionManager, interrupt_generator );

    if ( allocHashTables )
    {
        SCOREP_DEFINITIONS_MANAGER_ALLOC_MEMBER_HASH_TABLE( *definitionManager, system_tree_node );
        SCOREP_DEFINITIONS_MANAGER_ALLOC_MEMBER_HASH_TABLE( *definitionManager, system_tree_node_property );
        SCOREP_DEFINITIONS_MANAGER_ALLOC_MEMBER_HASH_TABLE( *definitionManager, region );
        SCOREP_DEFINITIONS_MANAGER_ALLOC_MEMBER_HASH_TABLE( *definitionManager, group );
        SCOREP_DEFINITIONS_MANAGER_ALLOC_MEMBER_HASH_TABLE( *definitionManager, cartesian_topology );
        SCOREP_DEFINITIONS_MANAGER_ALLOC_MEMBER_HASH_TABLE( *definitionManager, cartesian_coords );
        SCOREP_DEFINITIONS_MANAGER_ALLOC_MEMBER_HASH_TABLE( *definitionManager, communicator );
        SCOREP_DEFINITIONS_MANAGER_ALLOC_MEMBER_HASH_TABLE( *definitionManager, rma_window );
        SCOREP_DEFINITIONS_MANAGER_ALLOC_MEMBER_HASH_TABLE( *definitionManager, metric );
        SCOREP_DEFINITIONS_MANAGER_ALLOC_MEMBER_HASH_TABLE( *definitionManager, sampling_set );
        SCOREP_DEFINITIONS_MANAGER_ALLOC_MEMBER_HASH_TABLE( *definitionManager, sampling_set_recorder );
        SCOREP_DEFINITIONS_MANAGER_ALLOC_MEMBER_HASH_TABLE( *definitionManager, io_handle );
        SCOREP_DEFINITIONS_MANAGER_ALLOC_MEMBER_HASH_TABLE( *definitionManager, io_file );
        SCOREP_DEFINITIONS_MANAGER_ALLOC_MEMBER_HASH_TABLE( *definitionManager, io_file_property );
        SCOREP_DEFINITIONS_MANAGER_ALLOC_MEMBER_HASH_TABLE( *definitionManager, marker_group );
        SCOREP_DEFINITIONS_MANAGER_ALLOC_MEMBER_HASH_TABLE( *definitionManager, marker );
        SCOREP_DEFINITIONS_MANAGER_ALLOC_MEMBER_HASH_TABLE( *definitionManager, callpath );
        SCOREP_DEFINITIONS_MANAGER_ALLOC_MEMBER_HASH_TABLE( *definitionManager, property );
        SCOREP_DEFINITIONS_MANAGER_ALLOC_MEMBER_HASH_TABLE( *definitionManager, location_property );
        SCOREP_DEFINITIONS_MANAGER_ALLOC_MEMBER_HASH_TABLE( *definitionManager, calling_context );
        SCOREP_DEFINITIONS_MANAGER_ALLOC_MEMBER_HASH_TABLE( *definitionManager, interrupt_generator );
    }
}

static void
finalize_definition_manager( SCOREP_DefinitionManager* definitionManager )
{
    free( definitionManager->string.hash_table );
    free( definitionManager->system_tree_node.hash_table );
    free( definitionManager->system_tree_node_property.hash_table );
    free( definitionManager->source_file.hash_table );
    free( definitionManager->region.hash_table );
    free( definitionManager->group.hash_table );
    free( definitionManager->interim_communicator.hash_table );
    free( definitionManager->communicator.hash_table );
    free( definitionManager->rma_window.hash_table );
    free( definitionManager->cartesian_topology.hash_table );
    free( definitionManager->cartesian_coords.hash_table );
    free( definitionManager->metric.hash_table );
    free( definitionManager->sampling_set.hash_table );
    free( definitionManager->sampling_set_recorder.hash_table );
    free( definitionManager->io_handle.hash_table );
    free( definitionManager->io_file.hash_table );
    free( definitionManager->io_file_property.hash_table );
    free( definitionManager->marker_group.hash_table );
    free( definitionManager->marker.hash_table );
    free( definitionManager->parameter.hash_table );
    free( definitionManager->callpath.hash_table );
    free( definitionManager->property.hash_table );
    free( definitionManager->attribute.hash_table );
    free( definitionManager->location_property.hash_table );
    free( definitionManager->source_code_location.hash_table );
    free( definitionManager->calling_context.hash_table );
    free( definitionManager->interrupt_generator.hash_table );
}

void
SCOREP_Definitions_Finalize( void )
{
    if ( !definitions_initialized )
    {
        return;
    }

    finalize_definition_manager( &scorep_local_definition_manager );
    if ( scorep_unified_definition_manager )
    {
        finalize_definition_manager( scorep_unified_definition_manager );
    }
    free( scorep_unified_definition_manager );
    // the contents of the definition managers is allocated using
    // SCOREP_Memory_AllocForDefinitions, so we don't need to free it
    // explicitly.

    SCOREP_MutexDestroy( &definitions_lock );
    scorep_definitions_destroy_interim_communicator_counter_lock();
    scorep_system_tree_seq_free();

    definitions_initialized = false;
}


/**
 * Returns the number of unified metric definitions.
 */
uint32_t
SCOREP_Definitions_GetNumberOfUnifiedMetricDefinitions( void )
{
    return scorep_unified_definition_manager->metric.counter;
}


/**
 * Returns the number of unified callpath definitions.
 */
uint32_t
SCOREP_Definitions_GetNumberOfUnifiedCallpathDefinitions( void )
{
    return scorep_unified_definition_manager->callpath.counter;
}


uint32_t
SCOREP_Definitions_HandleToId( SCOREP_AnyHandle handle )
{
    if ( handle == SCOREP_MOVABLE_NULL )
    {
        return UINT32_MAX;
    }

    return SCOREP_LOCAL_HANDLE_TO_ID( handle, Any );
}
