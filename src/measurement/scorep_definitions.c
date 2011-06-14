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
 * @file       scorep_definitions.c
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status alpha
 *
 */


#include <config.h>


#include "scorep_definitions.h"

#include "scorep_environment.h"
#include "scorep_definition_structs.h"
#include "scorep_definition_macros.h"
#include "scorep_runtime_management.h"
#include "scorep_thread.h"
#include <scorep_utility/SCOREP_Omp.h>
#include <scorep/SCOREP_PublicTypes.h>
#include <SCOREP_Timing.h>
#include <SCOREP_Mutex.h>
#include <otf2/otf2.h>
#include <stdbool.h>
#include <stdint.h>
#include <scorep_utility/SCOREP_Debug.h>
#include "scorep_mpi.h"
#include "scorep_status.h"
#include <jenkins_hash.h>
#include "tracing/scorep_tracing_definitions.h"


SCOREP_DefinitionManager  scorep_local_definition_manager;
SCOREP_DefinitionManager* scorep_unified_definition_manager = 0;
SCOREP_DefinitionManager* scorep_remote_definition_manager  = 0;
static bool               scorep_definitions_initialized    = false;


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
SCOREP_Definitions_Initialize()
{
    if ( scorep_definitions_initialized )
    {
        return;
    }
    scorep_definitions_initialized = true;

    SCOREP_MutexCreate( &definitions_lock );

    SCOREP_DefinitionManager* local_definition_manager = &scorep_local_definition_manager;
    assert( local_definition_manager );
    SCOREP_InitializeDefinitionManager( &local_definition_manager,
                                        SCOREP_Memory_GetLocalDefinitionPageManager(),
                                        false );
}


void
SCOREP_InitializeDefinitionManager( SCOREP_DefinitionManager**    definitionManager,
                                    SCOREP_Allocator_PageManager* pageManager,
                                    bool                          allocHashTables )
{
    assert( definitionManager );
    assert( pageManager );

    if ( *definitionManager )
    {
        memset( *definitionManager, 0, sizeof( SCOREP_DefinitionManager ) );
    }
    else
    {
        *definitionManager = calloc( 1, sizeof( SCOREP_DefinitionManager ) );
    }

    ( *definitionManager )->page_manager = pageManager;
    ( *definitionManager )->mappings     = 0;

    SCOREP_INIT_DEFINITION_MANAGER_MEMBERS( string, *definitionManager );
    SCOREP_ALLOC_DEFINITION_MANAGER_HASH_TABLE( string, *definitionManager );

    SCOREP_INIT_DEFINITION_MANAGER_MEMBERS( system_tree_node, *definitionManager );
    SCOREP_INIT_DEFINITION_MANAGER_MEMBERS( location_group, *definitionManager );
    SCOREP_INIT_DEFINITION_MANAGER_MEMBERS( location, *definitionManager );
    SCOREP_INIT_DEFINITION_MANAGER_MEMBERS( source_file, *definitionManager );
    SCOREP_INIT_DEFINITION_MANAGER_MEMBERS( region, *definitionManager );
    SCOREP_INIT_DEFINITION_MANAGER_MEMBERS( group, *definitionManager );
    SCOREP_INIT_DEFINITION_MANAGER_MEMBERS( local_mpi_communicator, *definitionManager );
    SCOREP_INIT_DEFINITION_MANAGER_MEMBERS( mpi_communicator, *definitionManager );
    SCOREP_INIT_DEFINITION_MANAGER_MEMBERS( mpi_window, *definitionManager );
    SCOREP_INIT_DEFINITION_MANAGER_MEMBERS( mpi_cartesian_coords, *definitionManager );
    SCOREP_INIT_DEFINITION_MANAGER_MEMBERS( mpi_cartesian_topology, *definitionManager );
    SCOREP_INIT_DEFINITION_MANAGER_MEMBERS( counter_group, *definitionManager );
    SCOREP_INIT_DEFINITION_MANAGER_MEMBERS( counter, *definitionManager );
    SCOREP_INIT_DEFINITION_MANAGER_MEMBERS( io_file_group, *definitionManager );
    SCOREP_INIT_DEFINITION_MANAGER_MEMBERS( io_file, *definitionManager );
    SCOREP_INIT_DEFINITION_MANAGER_MEMBERS( marker_group, *definitionManager );
    SCOREP_INIT_DEFINITION_MANAGER_MEMBERS( marker, *definitionManager );
    SCOREP_INIT_DEFINITION_MANAGER_MEMBERS( parameter, *definitionManager );
    SCOREP_INIT_DEFINITION_MANAGER_MEMBERS( callpath, *definitionManager );

    if ( allocHashTables )
    {
        SCOREP_ALLOC_DEFINITION_MANAGER_HASH_TABLE( system_tree_node, *definitionManager );
        SCOREP_ALLOC_DEFINITION_MANAGER_HASH_TABLE( source_file, *definitionManager );
        SCOREP_ALLOC_DEFINITION_MANAGER_HASH_TABLE( region, *definitionManager );
        SCOREP_ALLOC_DEFINITION_MANAGER_HASH_TABLE( group, *definitionManager );
        SCOREP_ALLOC_DEFINITION_MANAGER_HASH_TABLE( mpi_window, *definitionManager );
        SCOREP_ALLOC_DEFINITION_MANAGER_HASH_TABLE( mpi_cartesian_topology, *definitionManager );
        SCOREP_ALLOC_DEFINITION_MANAGER_HASH_TABLE( mpi_cartesian_coords, *definitionManager );
        SCOREP_ALLOC_DEFINITION_MANAGER_HASH_TABLE( counter_group, *definitionManager );
        SCOREP_ALLOC_DEFINITION_MANAGER_HASH_TABLE( counter, *definitionManager );
        SCOREP_ALLOC_DEFINITION_MANAGER_HASH_TABLE( io_file_group, *definitionManager );
        SCOREP_ALLOC_DEFINITION_MANAGER_HASH_TABLE( io_file, *definitionManager );
        SCOREP_ALLOC_DEFINITION_MANAGER_HASH_TABLE( marker_group, *definitionManager );
        SCOREP_ALLOC_DEFINITION_MANAGER_HASH_TABLE( marker, *definitionManager );
        SCOREP_ALLOC_DEFINITION_MANAGER_HASH_TABLE( parameter, *definitionManager );
        SCOREP_ALLOC_DEFINITION_MANAGER_HASH_TABLE( callpath, *definitionManager );
    }
}

void
SCOREP_Definitions_Finalize()
{
    if ( !scorep_definitions_initialized )
    {
        return;
    }

    free( scorep_local_definition_manager.string_definition_hash_table );
    if ( scorep_unified_definition_manager )
    {
        free( scorep_unified_definition_manager->string_definition_hash_table );
        free( scorep_unified_definition_manager->location_definition_hash_table );
        free( scorep_unified_definition_manager->location_group_definition_hash_table );
        free( scorep_unified_definition_manager->system_tree_node_definition_hash_table );
        free( scorep_unified_definition_manager->source_file_definition_hash_table );
        free( scorep_unified_definition_manager->region_definition_hash_table );
        free( scorep_unified_definition_manager->group_definition_hash_table );
        free( scorep_unified_definition_manager->local_mpi_communicator_definition_hash_table );
        free( scorep_unified_definition_manager->mpi_communicator_definition_hash_table );
        free( scorep_unified_definition_manager->mpi_window_definition_hash_table );
        free( scorep_unified_definition_manager->mpi_cartesian_topology_definition_hash_table );
        free( scorep_unified_definition_manager->mpi_cartesian_coords_definition_hash_table );
        free( scorep_unified_definition_manager->counter_group_definition_hash_table );
        free( scorep_unified_definition_manager->counter_definition_hash_table );
        free( scorep_unified_definition_manager->io_file_group_definition_hash_table );
        free( scorep_unified_definition_manager->io_file_definition_hash_table );
        free( scorep_unified_definition_manager->marker_group_definition_hash_table );
        free( scorep_unified_definition_manager->marker_definition_hash_table );
        free( scorep_unified_definition_manager->parameter_definition_hash_table );
        free( scorep_unified_definition_manager->callpath_definition_hash_table );
    }
    free( scorep_unified_definition_manager );
    if ( scorep_remote_definition_manager )
    {
        free( scorep_remote_definition_manager->string_definition_hash_table );
    }
    free( scorep_remote_definition_manager );
    // the contents of the definition managers is allocated using
    // SCOREP_Memory_AllocForDefinitions, so we don't need to free it
    // explicitly.

    SCOREP_MutexDestroy( &definitions_lock );

    scorep_definitions_initialized = false;
}


void
SCOREP_Definitions_Write()
{
    if ( !scorep_definitions_initialized )
    {
        return;
    }

    if ( SCOREP_IsTracingEnabled() )
    {
        SCOREP_Tracing_WriteDefinitions();
    }

    /// @todo Daniel, what to do here for profiling?
}


int
SCOREP_GetNumberOfDefinitions()
{
    assert( !omp_in_parallel() );

    int n_definitions = 0; /// @todo might overflow
    n_definitions += scorep_local_definition_manager.string_definition_counter;
    n_definitions += scorep_local_definition_manager.system_tree_node_definition_counter;
    n_definitions += scorep_local_definition_manager.location_group_definition_counter;
    n_definitions += scorep_local_definition_manager.location_definition_counter;
    n_definitions += scorep_local_definition_manager.source_file_definition_counter;
    n_definitions += scorep_local_definition_manager.region_definition_counter;
    n_definitions += scorep_local_definition_manager.group_definition_counter;
    n_definitions += scorep_local_definition_manager.local_mpi_communicator_definition_counter;
    n_definitions += scorep_local_definition_manager.mpi_communicator_definition_counter;
    n_definitions += scorep_local_definition_manager.mpi_window_definition_counter;
    n_definitions += scorep_local_definition_manager.mpi_cartesian_topology_definition_counter;
    n_definitions += scorep_local_definition_manager.mpi_cartesian_coords_definition_counter;
    n_definitions += scorep_local_definition_manager.counter_group_definition_counter;
    n_definitions += scorep_local_definition_manager.counter_definition_counter;
    n_definitions += scorep_local_definition_manager.io_file_group_definition_counter;
    n_definitions += scorep_local_definition_manager.io_file_definition_counter;
    n_definitions += scorep_local_definition_manager.marker_group_definition_counter;
    n_definitions += scorep_local_definition_manager.marker_definition_counter;
    n_definitions += scorep_local_definition_manager.parameter_definition_counter;
    n_definitions += scorep_local_definition_manager.callpath_definition_counter;

    return n_definitions;
}

uint32_t
SCOREP_GetNumberOfRegionDefinitions()
{
    assert( !omp_in_parallel() );
    return scorep_local_definition_manager.region_definition_counter;
}

uint32_t
SCOREP_GetNumberOfCounterDefinitions()
{
    assert( !omp_in_parallel() );
    return scorep_local_definition_manager.counter_definition_counter;
}

uint32_t
SCOREP_GetRegionHandleToID( SCOREP_RegionHandle handle )
{
    assert( !omp_in_parallel() );
    return SCOREP_LOCAL_HANDLE_TO_ID( handle, Region );
}

uint32_t
SCOREP_CallPathHandleToRegionID( SCOREP_CallpathHandle handle )
{
    SCOREP_Callpath_Definition* callpath = SCOREP_LOCAL_HANDLE_DEREF( handle, Callpath );

    return SCOREP_GetRegionHandleToID( callpath->callpath_argument.region_handle );
}


static void
scorep_update_location_definition_cb( SCOREP_Thread_LocationData* locationData,
                                      void*                       data )
{
    int                         number_of_definitions = *( int* )data;
    SCOREP_LocationHandle       location_handle       =
        SCOREP_Thread_GetLocationHandle( locationData );
    SCOREP_Location_Definition* location_definition =
        SCOREP_LOCAL_HANDLE_DEREF( location_handle, Location );

    location_definition->number_of_definitions = number_of_definitions;
    location_definition->number_of_events      =
        SCOREP_Trace_GetNumberOfEvents( locationData );
}

void
SCOREP_UpdateLocationDefinitions()
{
    int number_of_definitions = SCOREP_GetNumberOfDefinitions();

    SCOREP_Thread_ForAllLocations( scorep_update_location_definition_cb,
                                   &number_of_definitions );
}
