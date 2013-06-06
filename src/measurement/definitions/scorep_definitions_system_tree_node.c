/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
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
 * @status     alpha
 * @file       src/measurement/definitions/scorep_definitions_system_tree_node.c
 * @maintainer Bert Wesarg <Bert.Wesarg@tu-dresden.de>
 *
 * @brief Declaration of definition functions to be used by the adapter layer.
 *
 */


#include <config.h>


#include <definitions/SCOREP_Definitions.h>


#include <assert.h>
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
#include <scorep_types.h>
#include <SCOREP_Mutex.h>
#include <SCOREP_Memory.h>


SCOREP_SystemTreeNodeHandle
define_system_tree_node( SCOREP_DefinitionManager*   definition_manager,
                         SCOREP_SystemTreeNodeHandle parent,
                         SCOREP_StringHandle         name,
                         SCOREP_StringHandle         class );


static bool
equal_system_tree_node( const SCOREP_SystemTreeNodeDef* existingDefinition,
                        const SCOREP_SystemTreeNodeDef* newDefinition );


SCOREP_SystemTreeNodeHandle
SCOREP_Definitions_NewSystemTreeNode( SCOREP_SystemTreeNodeHandle parent,
                                      const char*                 name,
                                      const char*                 class )
{
    UTILS_DEBUG_ENTRY( "node %s, class %s", name, class );

    SCOREP_Definitions_Lock();

    SCOREP_SystemTreeNodeHandle new_handle = define_system_tree_node(
        &scorep_local_definition_manager,
        parent,
        scorep_definitions_new_string(
            &scorep_local_definition_manager,
            name ? name : "<unnamed system tree node>" ),
        scorep_definitions_new_string(
            &scorep_local_definition_manager,
            class ? class : "<unnamed system tree class>" ) );

    SCOREP_Definitions_Unlock();

    return new_handle;
}


void
scorep_definitions_unify_system_tree_node( SCOREP_SystemTreeNodeDef*     definition,
                                           SCOREP_Allocator_PageManager* handlesPageManager )
{
    assert( definition );
    assert( handlesPageManager );

    SCOREP_SystemTreeNodeHandle unified_parent_handle = SCOREP_INVALID_SYSTEM_TREE_NODE;
    if ( definition->parent_handle != SCOREP_INVALID_SYSTEM_TREE_NODE )
    {
        unified_parent_handle = SCOREP_HANDLE_GET_UNIFIED(
            definition->parent_handle,
            SystemTreeNode,
            handlesPageManager );
        assert( unified_parent_handle != SCOREP_MOVABLE_NULL );
    }

    definition->unified = define_system_tree_node(
        scorep_unified_definition_manager,
        unified_parent_handle,
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
           existingDefinition->class_handle  == newDefinition->class_handle &&
           existingDefinition->name_handle   == newDefinition->name_handle;
}


SCOREP_SystemTreeNodeHandle
define_system_tree_node( SCOREP_DefinitionManager*   definition_manager,
                         SCOREP_SystemTreeNodeHandle parent,
                         SCOREP_StringHandle         name,
                         SCOREP_StringHandle         class )
{
    assert( definition_manager );

    SCOREP_SystemTreeNodeDef*   new_definition = NULL;
    SCOREP_SystemTreeNodeHandle new_handle     = SCOREP_INVALID_SYSTEM_TREE_NODE;

    SCOREP_DEFINITION_ALLOC( SystemTreeNode );

    new_definition->parent_handle = parent;
    if ( new_definition->parent_handle != SCOREP_INVALID_SYSTEM_TREE_NODE )
    {
        HASH_ADD_HANDLE( new_definition, parent_handle, SystemTreeNode );
    }

    new_definition->name_handle = name;
    HASH_ADD_HANDLE( new_definition, name_handle, String );

    new_definition->class_handle = class;
    HASH_ADD_HANDLE( new_definition, class_handle, String );

    /* Does return if it is a duplicate */
    SCOREP_DEFINITIONS_MANAGER_ADD_DEFINITION( SystemTreeNode, system_tree_node );

    return new_handle;
}
