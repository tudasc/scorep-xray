/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2015, 2019, 2022,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2014, 2022-2023,
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
 * @brief Declaration of definition functions to be used by the adapter layer.
 *
 */


#include <config.h>


#include <SCOREP_Definitions.h>
#include "scorep_definitions_private.h"


#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdarg.h>
#include <inttypes.h>


#include <UTILS_Error.h>
#define SCOREP_DEBUG_MODULE_NAME DEFINITIONS
#include <UTILS_Debug.h>
#include <UTILS_Mutex.h>


#include <jenkins_hash.h>


#include <SCOREP_DefinitionHandles.h>
#include <scorep_substrates_definition.h>
#include <scorep_type_utils.h>
#include <SCOREP_Memory.h>


static SCOREP_LocationHandle
define_location( SCOREP_DefinitionManager*  definition_manager,
                 uint64_t                   globalLocationId,
                 SCOREP_StringHandle        nameHandle,
                 SCOREP_LocationType        locationType,
                 SCOREP_ParadigmType        paradigm,
                 SCOREP_LocationGroupHandle locationGroupParent,
                 uint64_t                   numberOfEvents,
                 size_t                     sizeOfPayload,
                 void**                     payloadOut );

/**
 * Registers a new location into the definitions.
 */
SCOREP_LocationHandle
SCOREP_Definitions_NewLocation( SCOREP_LocationType        locationType,
                                SCOREP_ParadigmType        paradigm,
                                const char*                name,
                                SCOREP_LocationGroupHandle locationGroupParent,
                                size_t                     sizeOfPayload,
                                void**                     payload )
{
    SCOREP_Definitions_Lock();

    SCOREP_LocationHandle new_handle = define_location(
        &scorep_local_definition_manager,
        UINT64_MAX,
        scorep_definitions_new_string(
            &scorep_local_definition_manager,
            name ? name : "" ),
        locationType,
        paradigm,
        locationGroupParent,
        0,
        sizeOfPayload, payload );

    SCOREP_Definitions_Unlock();

    return new_handle;
}


void
scorep_definitions_unify_location( SCOREP_LocationDef*           definition,
                                   SCOREP_Allocator_PageManager* handlesPageManager )
{
    UTILS_ASSERT( definition );
    UTILS_ASSERT( handlesPageManager );

    SCOREP_LocationGroupHandle unified_location_group_parent = SCOREP_INVALID_LOCATION_GROUP;
    if ( definition->location_group_parent != SCOREP_INVALID_LOCATION_GROUP )
    {
        unified_location_group_parent = SCOREP_HANDLE_GET_UNIFIED(
            definition->location_group_parent,
            LocationGroup,
            handlesPageManager );

        UTILS_BUG_ON( unified_location_group_parent == SCOREP_INVALID_LOCATION_GROUP,
                      "Invalid unification order of location definition: location group not yet unified" );
    }

    definition->unified = define_location(
        scorep_unified_definition_manager,
        definition->global_location_id,
        SCOREP_HANDLE_GET_UNIFIED(
            definition->name_handle,
            String,
            handlesPageManager ),
        definition->location_type,
        definition->paradigm,
        unified_location_group_parent,
        definition->number_of_events,
        0, NULL );
}


static inline bool
equal_location( const SCOREP_LocationDef* existingDefinition,
                const SCOREP_LocationDef* newDefinition )
{
    return false;
}


static void
scorep_system_tree_node_handle_mark_used( SCOREP_SystemTreeNodeHandle handle, SCOREP_Allocator_PageManager* handlesPageManager )
{
    if ( handle == SCOREP_INVALID_SYSTEM_TREE_NODE || SCOREP_HANDLE_DEREF( handle, SystemTreeNode, handlesPageManager )->has_children == true )
    {
        /* Recursion until reaching the root node or a node that is already marked. */
        return;
    }
    SCOREP_HANDLE_DEREF( handle, SystemTreeNode, handlesPageManager )->has_children = true;
    scorep_system_tree_node_handle_mark_used( SCOREP_HANDLE_DEREF( handle, SystemTreeNode, handlesPageManager )->parent_handle, handlesPageManager );
}

static void
scorep_location_group_handle_mark_used( SCOREP_LocationGroupHandle handle, SCOREP_Allocator_PageManager* handlesPageManager )
{
    if ( handle == SCOREP_INVALID_LOCATION_GROUP )
    {
        return;
    }

    SCOREP_HANDLE_DEREF( handle, LocationGroup, handlesPageManager )->has_children = true;

    /* Mark also the creating location group if it is set for this lg. */
    scorep_location_group_handle_mark_used( SCOREP_HANDLE_DEREF( handle, LocationGroup, handlesPageManager )->creating_location_group, handlesPageManager );

    /* Recursively mark the system tree parent as used. */
    scorep_system_tree_node_handle_mark_used( SCOREP_HANDLE_DEREF( handle, LocationGroup, handlesPageManager )->system_tree_parent, handlesPageManager );
}


SCOREP_LocationHandle
define_location( SCOREP_DefinitionManager*  definition_manager,
                 uint64_t                   globalLocationId,
                 SCOREP_StringHandle        nameHandle,
                 SCOREP_LocationType        locationType,
                 SCOREP_ParadigmType        paradigm,
                 SCOREP_LocationGroupHandle locationGroupParent,
                 uint64_t                   numberOfEvents,
                 size_t                     sizeOfPayload,
                 void**                     payloadOut )
{
    UTILS_ASSERT( definition_manager );

    SCOREP_LocationDef*   new_definition = NULL;
    SCOREP_LocationHandle new_handle     = SCOREP_INVALID_LOCATION;

    if ( payloadOut )
    {
        *payloadOut = NULL;
    }

    size_t payload_offset = SCOREP_Allocator_RoundupToAlignment( sizeof( SCOREP_LocationDef ) );
    new_handle     = SCOREP_Memory_AllocForDefinitions( NULL, payload_offset + sizeOfPayload );
    new_definition = SCOREP_LOCAL_HANDLE_DEREF( new_handle, Location );
    memset( new_definition, 0, payload_offset + sizeOfPayload );
    SCOREP_INIT_DEFINITION_HEADER( new_definition );

    /* locations wont be unified, therefore no hash value needed */
    new_definition->global_location_id    = globalLocationId;
    new_definition->name_handle           = nameHandle;
    new_definition->location_type         = locationType;
    new_definition->paradigm              = paradigm;
    new_definition->location_group_parent = locationGroupParent;
    new_definition->number_of_events      = numberOfEvents;

    /* Does return if it is a duplicate */
    SCOREP_DEFINITIONS_MANAGER_ADD_DEFINITION( Location, location );

    if ( payloadOut )
    {
        *payloadOut = ( char* )new_definition + payload_offset;
    }

    if ( definition_manager == &scorep_local_definition_manager )
    {
        SCOREP_CALL_SUBSTRATE_MGMT( NewDefinitionHandle, NEW_DEFINITION_HANDLE,
                                    ( new_handle, SCOREP_HANDLE_TYPE_LOCATION ) );
    }

    /* Flag parent location group and all its parent system tree nodes as used. */
    scorep_location_group_handle_mark_used( locationGroupParent, definition_manager->page_manager );

    return new_handle;
}
