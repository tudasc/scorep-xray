/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2014, 2019, 2022,
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


static SCOREP_LocationGroupHandle
define_location_group( SCOREP_DefinitionManager*   definition_manager,
                       SCOREP_StringHandle         name,
                       SCOREP_SystemTreeNodeHandle systemTreeParent,
                       SCOREP_LocationGroupType    locationGroupType,
                       SCOREP_LocationGroupHandle  creatingLocationGroup );


/**
 * Registers a new local location group into the definitions.
 *
 * @in internal
 */
SCOREP_LocationGroupHandle
SCOREP_Definitions_NewLocationGroup( const char*                 name,
                                     SCOREP_SystemTreeNodeHandle systemTreeParent,
                                     SCOREP_LocationGroupType    locationGroupType,
                                     SCOREP_LocationGroupHandle  creatingLocationGroup )
{
    SCOREP_Definitions_Lock();

    /* In early stage 'global location group ID' and 'name' are set to invalid dummies.
     * Correct values must be set later on. */
    SCOREP_LocationGroupHandle new_handle = define_location_group(
        &scorep_local_definition_manager,
        scorep_definitions_new_string(
            &scorep_local_definition_manager,
            name ? name : "<unknown location group>" ),
        systemTreeParent,
        locationGroupType,
        creatingLocationGroup );

    SCOREP_Definitions_Unlock();

    return new_handle;
}


void
SCOREP_LocationGroupHandle_SetName( SCOREP_LocationGroupHandle handle,
                                    const char*                name )
{
    SCOREP_LOCAL_HANDLE_DEREF(
        handle, LocationGroup )->name_handle = SCOREP_Definitions_NewString( name );
}


void
scorep_definitions_unify_location_group( SCOREP_LocationGroupDef*      definition,
                                         SCOREP_Allocator_PageManager* handlesPageManager )
{
    UTILS_ASSERT( definition );
    UTILS_ASSERT( handlesPageManager );

    if ( !definition->has_children )
    {
        /* In case this location group has no locations. if this group doesn't have a
         * location it shouldn't be unified.
         */
        return;
    }

    SCOREP_SystemTreeNodeHandle unified_system_tree_parent = SCOREP_INVALID_SYSTEM_TREE_NODE;
    if ( definition->system_tree_parent != SCOREP_INVALID_SYSTEM_TREE_NODE )
    {
        unified_system_tree_parent = SCOREP_HANDLE_GET_UNIFIED(
            definition->system_tree_parent,
            SystemTreeNode,
            handlesPageManager );
        UTILS_BUG_ON( unified_system_tree_parent == SCOREP_INVALID_SYSTEM_TREE_NODE,
                      "Invalid unification order of location group definition: system tree parent not yet unified" );
    }

    SCOREP_LocationGroupHandle unified_creating_location_group = SCOREP_INVALID_LOCATION_GROUP;
    if ( definition->creating_location_group != SCOREP_INVALID_LOCATION_GROUP )
    {
        unified_creating_location_group = SCOREP_HANDLE_GET_UNIFIED(
            definition->creating_location_group,
            LocationGroup,
            handlesPageManager );
        UTILS_BUG_ON( unified_creating_location_group == SCOREP_INVALID_LOCATION_GROUP,
                      "Invalid unification order of location group definition: creating location group not yet unified" );
    }

    definition->unified = define_location_group(
        scorep_unified_definition_manager,
        SCOREP_HANDLE_GET_UNIFIED(
            definition->name_handle,
            String,
            handlesPageManager ),
        unified_system_tree_parent,
        definition->location_group_type,
        unified_creating_location_group );
}


static inline bool
equal_location_group( const SCOREP_LocationGroupDef* existingDefinition,
                      const SCOREP_LocationGroupDef* newDefinition )
{
    return false;
}


SCOREP_LocationGroupHandle
define_location_group( SCOREP_DefinitionManager*   definition_manager,
                       SCOREP_StringHandle         name,
                       SCOREP_SystemTreeNodeHandle systemTreeParent,
                       SCOREP_LocationGroupType    locationGroupType,
                       SCOREP_LocationGroupHandle  creatingLocationGroup )
{
    UTILS_ASSERT( definition_manager );

    SCOREP_LocationGroupDef*   new_definition = NULL;
    SCOREP_LocationGroupHandle new_handle     = SCOREP_INVALID_LOCATION_GROUP;

    SCOREP_DEFINITION_ALLOC( LocationGroup );

    /* location groups wont be unified, therefore no hash value needed */
    new_definition->name_handle             = name;
    new_definition->system_tree_parent      = systemTreeParent;
    new_definition->location_group_type     = locationGroupType;
    new_definition->creating_location_group = creatingLocationGroup;
    /* Will be set to true from any child locations */
    new_definition->has_children = false;

    /* Does return if it is a duplicate */
    SCOREP_DEFINITIONS_MANAGER_ADD_DEFINITION( LocationGroup, location_group );

    if ( definition_manager == &scorep_local_definition_manager )
    {
        SCOREP_CALL_SUBSTRATE_MGMT( NewDefinitionHandle, NEW_DEFINITION_HANDLE,
                                    ( new_handle, SCOREP_HANDLE_TYPE_LOCATION_GROUP ) );
    }

    return new_handle;
}
