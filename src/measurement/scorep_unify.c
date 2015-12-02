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
#include "scorep_unify.h"

#include <SCOREP_Config.h>
#include "scorep_environment.h"
#include "scorep_status.h"
#include <SCOREP_Definitions.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <inttypes.h>
#include "scorep_subsystem.h"

#include <UTILS_Error.h>
#define SCOREP_DEBUG_MODULE_NAME UNIFY
#include <UTILS_Debug.h>
#include <SCOREP_Memory.h>


void
SCOREP_Unify( void )
{
    /* This prepares the unified definition manager */
    SCOREP_Unify_CreateUnifiedDefinitionManager();

    /* Let the subsystems do some stuff */
    scorep_subsystems_pre_unify();

    /* Unify the local definitions */
    SCOREP_Unify_Locally();

    if ( SCOREP_Status_IsMpp() )
    {
        /* unify the definitions with all processes. */
        SCOREP_Unify_Mpp();
    }

    /* Let the subsystems do some stuff */
    scorep_subsystems_post_unify();

    /* fool linker, so that the scorep_unify_helpers.c unit is always linked
       into the library/binary. */
    UTILS_FOOL_LINKER( scorep_unify_helpers );
}


/**
 * Copies all definitions of type @a type to the unified definition manager.
 *
 * @needs Variable named @a definition_manager of type @a SCOREP_DefinitionManager*.
 *        The definitions to be copied live here.
 */
#define UNIFY_DEFINITION( definition_manager, Type, type ) \
    do \
    { \
        UTILS_DEBUG( "Copying %s to unified", #type ); \
        SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_BEGIN( definition_manager, Type, type ) \
        { \
            scorep_definitions_unify_ ## type( definition, ( definition_manager )->page_manager ); \
        } \
        SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_END(); \
    } \
    while ( 0 )


void
SCOREP_CopyDefinitionsToUnified( SCOREP_DefinitionManager* sourceDefinitionManager )
{
    UTILS_ASSERT( sourceDefinitionManager );
    UNIFY_DEFINITION( sourceDefinitionManager, String, string );
    UNIFY_DEFINITION( sourceDefinitionManager, SystemTreeNode, system_tree_node );
    UNIFY_DEFINITION( sourceDefinitionManager, SystemTreeNodeProperty, system_tree_node_property );
    UNIFY_DEFINITION( sourceDefinitionManager, LocationGroup, location_group );
    UNIFY_DEFINITION( sourceDefinitionManager, Location, location );
    UNIFY_DEFINITION( sourceDefinitionManager, SourceFile, source_file );
    UNIFY_DEFINITION( sourceDefinitionManager, Region, region );
    UNIFY_DEFINITION( sourceDefinitionManager, Group, group );
    UNIFY_DEFINITION( sourceDefinitionManager, Communicator, communicator );
    UNIFY_DEFINITION( sourceDefinitionManager, RmaWindow, rma_window );
    UNIFY_DEFINITION( sourceDefinitionManager, Metric, metric );
    UNIFY_DEFINITION( sourceDefinitionManager, SamplingSet, sampling_set );
    UNIFY_DEFINITION( sourceDefinitionManager, SamplingSetRecorder, sampling_set_recorder );
    UNIFY_DEFINITION( sourceDefinitionManager, Parameter, parameter );
    UNIFY_DEFINITION( sourceDefinitionManager, Callpath, callpath );
    UNIFY_DEFINITION( sourceDefinitionManager, Property, property );
    UNIFY_DEFINITION( sourceDefinitionManager, Attribute, attribute );
    UNIFY_DEFINITION( sourceDefinitionManager, LocationProperty, location_property );
    UNIFY_DEFINITION( sourceDefinitionManager, SourceCodeLocation, source_code_location );
    UNIFY_DEFINITION( sourceDefinitionManager, CallingContext, calling_context );
    UNIFY_DEFINITION( sourceDefinitionManager, InterruptGenerator, interrupt_generator );
}


/**
 * Allocates the array member @a type_mappings of struct SCOREP_DefinitionMappings that lives
 * in @a definition_manager. The size of the array equals @a type_definition_counter.
 *
 */
#define ALLOC_MAPPINGS( definition_manager, type ) \
    do \
    { \
        UTILS_DEBUG( "Alloc mappings for %s", #type ); \
        scorep_definitions_manager_entry_alloc_mapping( &( definition_manager )->type ); \
    } \
    while ( 0 )


void
SCOREP_CreateDefinitionMappings( SCOREP_DefinitionManager* definitionManager )
{
    UTILS_ASSERT( definitionManager );

    #define DEF_WITH_MAPPING( Type, type ) \
    ALLOC_MAPPINGS( definitionManager, type );
    SCOREP_LIST_OF_DEFS_WITH_MAPPINGS
    #undef DEF_WITH_MAPPING
}


/**
 * Fill the mapping array member @a type_mappings @a SCOREP_DefinitionMappings with the
 * corresponding sequence numbers of the unified definition.
 */
#define ASSIGN_MAPPING( definition_manager, Type, type ) \
    do \
    { \
        UTILS_DEBUG( "Assign mapping for %s", #Type ); \
        if ( ( definition_manager )->type.counter > 0 ) \
        { \
            SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_BEGIN( definition_manager, Type, type ) \
            { \
                ( definition_manager )->type.mapping[ definition->sequence_number ] = \
                    SCOREP_UNIFIED_HANDLE_DEREF( definition->unified, Type )->sequence_number; \
            } \
            SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_END(); \
        } \
    } \
    while ( 0 )


void
SCOREP_AssignDefinitionMappingsFromUnified( SCOREP_DefinitionManager* definitionManager )
{
    UTILS_ASSERT( definitionManager );

    #define DEF_WITH_MAPPING( Type, type ) \
    ASSIGN_MAPPING( definitionManager, Type, type );
    SCOREP_LIST_OF_DEFS_WITH_MAPPINGS
    #undef DEF_WITH_MAPPING
}


/**
 * Frees the array member @a type_mappings of struct SCOREP_DefinitionMappings that lives
 * in @a definition_manager. The size of the array equals @a type_definition_counter.
 *
 */
#define FREE_MAPPING( definition_manager, type ) \
    do \
    { \
        UTILS_DEBUG( "Free mappings for %s", #type ); \
        scorep_definitions_manager_entry_free_mapping( &( definition_manager )->type ); \
    } \
    while ( 0 )


void
SCOREP_DestroyDefinitionMappings( SCOREP_DefinitionManager* definitionManager )
{
    UTILS_ASSERT( definitionManager );

    #define DEF_WITH_MAPPING( Type, type ) \
    FREE_MAPPING( definitionManager, type );
    SCOREP_LIST_OF_DEFS_WITH_MAPPINGS
    #undef DEF_WITH_MAPPING

    FREE_MAPPING( definitionManager,
                  interim_communicator );

    FREE_MAPPING( definitionManager,
                  interim_rma_window );
}


void
SCOREP_Unify_CreateUnifiedDefinitionManager( void )
{
    UTILS_BUG_ON( scorep_unified_definition_manager != NULL,
                  "Unified definition manager already created" );

    bool alloc_hash_tables = true;
    SCOREP_Definitions_InitializeDefinitionManager( &scorep_unified_definition_manager,
                                                    SCOREP_Memory_GetLocalDefinitionPageManager(),
                                                    alloc_hash_tables );

    /* ensure, that the empty string gets id 0 */
    scorep_definitions_new_string( scorep_unified_definition_manager,
                                   "", NULL );
}

void
SCOREP_Unify_Locally( void )
{
    SCOREP_CopyDefinitionsToUnified( &scorep_local_definition_manager );
    // The unified definitions might differ from the local ones if there were
    // duplicates in the local ones. By creating mappings we are on the save side.
    SCOREP_CreateDefinitionMappings( &scorep_local_definition_manager );
    SCOREP_AssignDefinitionMappingsFromUnified( &scorep_local_definition_manager );

    /*
     * Location definitions need special treatment as there global id is 64bit
     * and are not derived by the unification algorithm. We nevertheless
     * store the mapping array in the uint32_t* mappings member.
     */
    UTILS_DEBUG( "Alloc mappings for location" );
    scorep_local_definition_manager.location.mapping = malloc(
        scorep_local_definition_manager.location.counter * sizeof( uint64_t ) );

    UTILS_DEBUG( "Assign mapping for Locations" );
    if ( scorep_local_definition_manager.location.counter > 0 )
    {
        /* cast to uint64_t* to get the type right. */
        uint64_t* mapping = ( uint64_t* )scorep_local_definition_manager.location.mapping;
        SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_BEGIN( &scorep_local_definition_manager,
                                                             Location,
                                                             location )
        {
            mapping[ definition->sequence_number ] = definition->global_location_id;
        }
        SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_END();
    }

    /*
     * Allocate also mappings for the interim definitions.
     */
    ALLOC_MAPPINGS( &scorep_local_definition_manager, interim_communicator );
    ALLOC_MAPPINGS( &scorep_local_definition_manager, interim_rma_window );
}
