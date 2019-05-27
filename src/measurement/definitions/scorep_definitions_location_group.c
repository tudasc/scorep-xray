/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2014, 2019,
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


#include <jenkins_hash.h>


#include <SCOREP_DefinitionHandles.h>
#include <scorep_substrates_definition.h>
#include <scorep_type_utils.h>
#include <SCOREP_Mutex.h>
#include <SCOREP_Memory.h>


static SCOREP_LocationGroupHandle
define_location_group( SCOREP_DefinitionManager*   definition_manager,
                       uint32_t                    globalLocationGroupId,
                       SCOREP_SystemTreeNodeHandle parent,
                       SCOREP_StringHandle         nameHandle,
                       SCOREP_LocationGroupType    locationType );

/**
 * Registers a new local location group into the definitions.
 *
 * @in internal
 */
SCOREP_LocationGroupHandle
SCOREP_Definitions_NewLocationGroup( SCOREP_SystemTreeNodeHandle parent )
{
    SCOREP_Definitions_Lock();

    /* In early stage 'global location group ID' and 'name' are set to invalid dummies.
     * Correct values must be set later on. */
    SCOREP_LocationGroupHandle new_handle = define_location_group(
        &scorep_local_definition_manager,
        UINT32_MAX,
        parent,
        SCOREP_INVALID_STRING,
        SCOREP_LOCATION_GROUP_TYPE_PROCESS );

    SCOREP_Definitions_Unlock();

    return new_handle;
}

void
scorep_definitions_unify_location_group( SCOREP_LocationGroupDef*      definition,
                                         SCOREP_Allocator_PageManager* handlesPageManager )
{
    UTILS_ASSERT( definition );
    UTILS_ASSERT( handlesPageManager );

    SCOREP_SystemTreeNodeHandle unified_parent_handle = SCOREP_INVALID_SYSTEM_TREE_NODE;
    if ( definition->parent != SCOREP_INVALID_SYSTEM_TREE_NODE )
    {
        unified_parent_handle = SCOREP_HANDLE_GET_UNIFIED(
            definition->parent,
            SystemTreeNode,
            handlesPageManager );
        UTILS_BUG_ON( unified_parent_handle == SCOREP_INVALID_SYSTEM_TREE_NODE,
                      "Invalid unification order of location group definition: system tree parent not yet unified" );
    }

    definition->unified = define_location_group(
        scorep_unified_definition_manager,
        definition->global_location_group_id,
        unified_parent_handle,
        SCOREP_HANDLE_GET_UNIFIED(
            definition->name_handle,
            String,
            handlesPageManager ),
        definition->location_group_type );
}

static inline bool
equal_location_group( const SCOREP_LocationGroupDef* existingDefinition,
                      const SCOREP_LocationGroupDef* newDefinition )
{
    return false;
}


SCOREP_LocationGroupHandle
define_location_group( SCOREP_DefinitionManager*   definition_manager,
                       uint32_t                    globalLocationGroupId,
                       SCOREP_SystemTreeNodeHandle parent,
                       SCOREP_StringHandle         nameHandle,
                       SCOREP_LocationGroupType    locationGroupType )
{
    UTILS_ASSERT( definition_manager );

    SCOREP_LocationGroupDef*   new_definition = NULL;
    SCOREP_LocationGroupHandle new_handle     = SCOREP_INVALID_LOCATION_GROUP;

    SCOREP_DEFINITION_ALLOC( LocationGroup );

    /* location groups wont be unfied, therefore no hash value needed */
    new_definition->global_location_group_id = globalLocationGroupId;
    new_definition->parent                   = parent;
    new_definition->name_handle              = nameHandle;
    new_definition->location_group_type      = locationGroupType;

    /* Does return if it is a duplicate */
    SCOREP_DEFINITIONS_MANAGER_ADD_DEFINITION( LocationGroup, location_group );

    if ( definition_manager == &scorep_local_definition_manager )
    {
        SCOREP_CALL_SUBSTRATE_MGMT( NewDefinitionHandle, NEW_DEFINITION_HANDLE,
                                    ( new_handle, SCOREP_HANDLE_TYPE_LOCATION_GROUP ) );
    }

    return new_handle;
}
