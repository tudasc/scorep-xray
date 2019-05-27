/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2015, 2019,
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


static SCOREP_SystemTreeNodePropertyHandle
add_system_tree_node_property( SCOREP_DefinitionManager*   definition_manager,
                               SCOREP_SystemTreeNodeDef*   systemTreeNode,
                               SCOREP_SystemTreeNodeHandle systemTreeNodeHandle,
                               SCOREP_StringHandle         propertyNameHandle,
                               SCOREP_StringHandle         propertyValueHandle );


static bool
equal_system_tree_node_property( const SCOREP_SystemTreeNodePropertyDef* existingDefinition,
                                 const SCOREP_SystemTreeNodePropertyDef* newDefinition );


void
SCOREP_SystemTreeNodeHandle_AddProperty( SCOREP_SystemTreeNodeHandle systemTreeNodeHandle,
                                         const char*                 propertyName,
                                         const char*                 propertyValue )
{
    UTILS_DEBUG_ENTRY( "%s=%s", propertyName, propertyValue );

    UTILS_ASSERT( systemTreeNodeHandle != SCOREP_INVALID_SYSTEM_TREE_NODE );
    UTILS_ASSERT( propertyName );
    UTILS_ASSERT( propertyValue );

    SCOREP_SystemTreeNodeDef* system_tree_node = SCOREP_LOCAL_HANDLE_DEREF(
        systemTreeNodeHandle,
        SystemTreeNode );

    SCOREP_Definitions_Lock();

    SCOREP_SystemTreeNodePropertyHandle handle =
        add_system_tree_node_property(
            &scorep_local_definition_manager,
            system_tree_node,
            systemTreeNodeHandle,
            scorep_definitions_new_string(
                &scorep_local_definition_manager,
                propertyName, NULL ),
            scorep_definitions_new_string(
                &scorep_local_definition_manager,
                propertyValue, NULL ) );

    SCOREP_Definitions_Unlock();
}


void
scorep_definitions_unify_system_tree_node_property( SCOREP_SystemTreeNodePropertyDef* definition,
                                                    SCOREP_Allocator_PageManager*     handlesPageManager )
{
    UTILS_ASSERT( definition );
    UTILS_ASSERT( handlesPageManager );

    SCOREP_SystemTreeNodeHandle unified_system_tree_node_handle = SCOREP_HANDLE_GET_UNIFIED(
        definition->system_tree_node_handle,
        SystemTreeNode,
        handlesPageManager );

    SCOREP_SystemTreeNodeDef* unified_system_tree_node = SCOREP_HANDLE_DEREF(
        unified_system_tree_node_handle,
        SystemTreeNode,
        scorep_unified_definition_manager->page_manager );

    SCOREP_SystemTreeNodePropertyHandle unified =
        add_system_tree_node_property(
            scorep_unified_definition_manager,
            unified_system_tree_node,
            unified_system_tree_node_handle,
            SCOREP_HANDLE_GET_UNIFIED(
                definition->property_name_handle,
                String,
                handlesPageManager ),
            SCOREP_HANDLE_GET_UNIFIED(
                definition->property_value_handle,
                String,
                handlesPageManager ) );
}


bool
equal_system_tree_node_property( const SCOREP_SystemTreeNodePropertyDef* existingDefinition,
                                 const SCOREP_SystemTreeNodePropertyDef* newDefinition )
{
    return existingDefinition->system_tree_node_handle == newDefinition->system_tree_node_handle &&
           existingDefinition->property_name_handle    == newDefinition->property_name_handle    &&
           existingDefinition->property_value_handle   == newDefinition->property_value_handle;
}


SCOREP_SystemTreeNodePropertyHandle
add_system_tree_node_property( SCOREP_DefinitionManager*   definition_manager,
                               SCOREP_SystemTreeNodeDef*   systemTreeNode,
                               SCOREP_SystemTreeNodeHandle systemTreeNodeHandle,
                               SCOREP_StringHandle         propertyNameHandle,
                               SCOREP_StringHandle         propertyValueHandle )
{
    UTILS_ASSERT( definition_manager );

    SCOREP_SystemTreeNodePropertyDef*   new_definition = NULL;
    SCOREP_SystemTreeNodePropertyHandle new_handle     = SCOREP_INVALID_SYSTEM_TREE_NODE_PROPERTY;

    SCOREP_DEFINITION_ALLOC( SystemTreeNodeProperty );

    new_definition->system_tree_node_handle = systemTreeNodeHandle;
    HASH_ADD_HANDLE( new_definition, system_tree_node_handle, SystemTreeNode );

    new_definition->property_name_handle = propertyNameHandle;
    HASH_ADD_HANDLE( new_definition, property_name_handle, String );

    new_definition->property_value_handle = propertyValueHandle;
    HASH_ADD_HANDLE( new_definition, property_value_handle, String );

    /* Does return if it is a duplicate */
    SCOREP_DEFINITIONS_MANAGER_ADD_DEFINITION( SystemTreeNodeProperty,
                                               system_tree_node_property );

    /* Chain this the properties of the system tree node. */
    new_definition->properties_next  = SCOREP_INVALID_SYSTEM_TREE_NODE_PROPERTY;
    *systemTreeNode->properties_tail = new_handle;
    systemTreeNode->properties_tail  = &new_definition->properties_next;

    if ( definition_manager == &scorep_local_definition_manager )
    {
        SCOREP_CALL_SUBSTRATE_MGMT( NewDefinitionHandle, NEW_DEFINITION_HANDLE,
                                    ( new_handle, SCOREP_HANDLE_TYPE_SYSTEM_TREE_NODE_PROPERTY ) );
    }
    return new_handle;
}
