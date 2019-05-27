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


static SCOREP_SystemTreeNodeHandle
define_system_tree_node( SCOREP_DefinitionManager*   definition_manager,
                         SCOREP_SystemTreeNodeHandle parent,
                         SCOREP_SystemTreeDomain     domains,
                         SCOREP_StringHandle         name,
                         SCOREP_StringHandle         class );


static bool
equal_system_tree_node( const SCOREP_SystemTreeNodeDef* existingDefinition,
                        const SCOREP_SystemTreeNodeDef* newDefinition );


SCOREP_SystemTreeNodeHandle
SCOREP_Definitions_NewSystemTreeNode( SCOREP_SystemTreeNodeHandle parent,
                                      SCOREP_SystemTreeDomain     domains,
                                      const char*                 class,
                                      const char*                 name )
{
    UTILS_DEBUG_ENTRY( "node %s, class %s", name, class );

    SCOREP_Definitions_Lock();

    SCOREP_SystemTreeNodeHandle new_handle = define_system_tree_node(
        &scorep_local_definition_manager,
        parent,
        domains,
        scorep_definitions_new_string(
            &scorep_local_definition_manager,
            name ? name : "<unnamed system tree node>", NULL ),
        scorep_definitions_new_string(
            &scorep_local_definition_manager,
            class ? class : "<unnamed system tree class>", NULL ) );

    SCOREP_Definitions_Unlock();

    return new_handle;
}


void
scorep_definitions_unify_system_tree_node( SCOREP_SystemTreeNodeDef*     definition,
                                           SCOREP_Allocator_PageManager* handlesPageManager )
{
    UTILS_ASSERT( definition );
    UTILS_ASSERT( handlesPageManager );

    SCOREP_SystemTreeNodeHandle unified_parent_handle = SCOREP_INVALID_SYSTEM_TREE_NODE;
    if ( definition->parent_handle != SCOREP_INVALID_SYSTEM_TREE_NODE )
    {
        unified_parent_handle = SCOREP_HANDLE_GET_UNIFIED(
            definition->parent_handle,
            SystemTreeNode,
            handlesPageManager );
        UTILS_BUG_ON( unified_parent_handle == SCOREP_INVALID_SYSTEM_TREE_NODE,
                      "Invalid unification order of system tree definition: parent not yet unified" );
    }

    definition->unified = define_system_tree_node(
        scorep_unified_definition_manager,
        unified_parent_handle,
        definition->domains,
        SCOREP_HANDLE_GET_UNIFIED(
            definition->name_handle,
            String,
            handlesPageManager ),
        SCOREP_HANDLE_GET_UNIFIED(
            definition->class_handle,
            String,
            handlesPageManager ) );
}


bool
equal_system_tree_node( const SCOREP_SystemTreeNodeDef* existingDefinition,
                        const SCOREP_SystemTreeNodeDef* newDefinition )
{
    return existingDefinition->parent_handle == newDefinition->parent_handle &&
           existingDefinition->domains       == newDefinition->domains &&
           existingDefinition->class_handle  == newDefinition->class_handle &&
           existingDefinition->name_handle   == newDefinition->name_handle;
}


SCOREP_SystemTreeNodeHandle
define_system_tree_node( SCOREP_DefinitionManager*   definition_manager,
                         SCOREP_SystemTreeNodeHandle parent,
                         SCOREP_SystemTreeDomain     domains,
                         SCOREP_StringHandle         name,
                         SCOREP_StringHandle         class )
{
    UTILS_ASSERT( definition_manager );

    SCOREP_SystemTreeNodeDef*   new_definition = NULL;
    SCOREP_SystemTreeNodeHandle new_handle     = SCOREP_INVALID_SYSTEM_TREE_NODE;

    SCOREP_DEFINITION_ALLOC( SystemTreeNode );

    new_definition->parent_handle = parent;
    if ( new_definition->parent_handle != SCOREP_INVALID_SYSTEM_TREE_NODE )
    {
        HASH_ADD_HANDLE( new_definition, parent_handle, SystemTreeNode );
    }

    new_definition->domains = domains;
    HASH_ADD_POD( new_definition, domains );

    new_definition->name_handle = name;
    HASH_ADD_HANDLE( new_definition, name_handle, String );

    new_definition->class_handle = class;
    HASH_ADD_HANDLE( new_definition, class_handle, String );

    new_definition->properties      = SCOREP_INVALID_SYSTEM_TREE_NODE_PROPERTY;
    new_definition->properties_tail = &new_definition->properties;

    /* Does return if it is a duplicate */
    SCOREP_DEFINITIONS_MANAGER_ADD_DEFINITION( SystemTreeNode, system_tree_node );

    if ( definition_manager == &scorep_local_definition_manager )
    {
        SCOREP_CALL_SUBSTRATE_MGMT( NewDefinitionHandle, NEW_DEFINITION_HANDLE,
                                    ( new_handle, SCOREP_HANDLE_TYPE_SYSTEM_TREE_NODE ) );
    }

    return new_handle;
}
