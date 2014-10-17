/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2013,
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
 * @brief   Implements the system tree creation.
 *
 *
 */

#include <config.h>
#include <UTILS_Error.h>
#include <UTILS_IO.h>
#include <SCOREP_Platform.h>
#include <SCOREP_RuntimeManagement.h>
#include <SCOREP_Definitions.h>
#include <scorep_status.h>
#include <scorep_mpp.h>
#include <scorep_environment.h>
#include <SCOREP_Memory.h>
#include <stdio.h>
#include <string.h>


/** Root of system tree */
static SCOREP_SystemTreeNodeHandle system_tree_root_node_handle = SCOREP_INVALID_SYSTEM_TREE_NODE;

/** Root of shared memory domain */
static SCOREP_SystemTreeNodeHandle system_tree_shared_memory_node_handle = SCOREP_INVALID_SYSTEM_TREE_NODE;

SCOREP_Platform_SystemTreePathElement*
SCOREP_BuildSystemTree( void )
{
    /* Obtain system tree information from platform dependent implementation */

    SCOREP_Platform_SystemTreePathElement* path = NULL;
    SCOREP_ErrorCode                       err  =
        SCOREP_Platform_GetPathInSystemTree( &path,
                                             SCOREP_Env_GetMachineName(),
                                             SCOREP_PLATFORM_NAME );
    UTILS_BUG_ON( SCOREP_SUCCESS != err,
                  "Failed to obtain system tree information." );

    return path;
}

SCOREP_LocationGroupHandle
SCOREP_DefineSystemTree( SCOREP_Platform_SystemTreePathElement* path )
{
    /* Create SystemTreeNode definitions */
    SCOREP_SystemTreeNodeHandle            parent = SCOREP_INVALID_SYSTEM_TREE_NODE;
    SCOREP_Platform_SystemTreePathElement* node;
    SCOREP_PLATFORM_SYSTEM_TREE_FORALL( path, node )
    {
        parent = SCOREP_Definitions_NewSystemTreeNode( parent,
                                                       node->domains,
                                                       node->node_class,
                                                       node->node_name );
        if ( system_tree_root_node_handle == SCOREP_INVALID_SYSTEM_TREE_NODE )
        {
            system_tree_root_node_handle = parent;
        }
        if ( node->domains & SCOREP_SYSTEM_TREE_DOMAIN_SHARED_MEMORY )
        {
            system_tree_shared_memory_node_handle = parent;
        }

        SCOREP_Platform_SystemTreeProperty* property;
        SCOREP_PLATFORM_SYSTEM_TREE_PROPERTY_FORALL( node, property )
        {
            SCOREP_SystemTreeNodeHandle_AddProperty( parent,
                                                     property->property_name,
                                                     property->property_value );
        }
    }

    /* Create Location Group definition
     *
     * In early stage 'global location group ID' and 'name' are set to invalid dummies.
     * Correct values must be set later on. */
    return SCOREP_Definitions_NewLocationGroup( parent );
}

void
SCOREP_FreeSystemTree( SCOREP_Platform_SystemTreePathElement* path )
{
    /* System tree path is not needed anymore */
    SCOREP_Platform_FreePath( path );
}

SCOREP_SystemTreeNodeHandle
SCOREP_GetSystemTreeRootNodeHandle( void )
{
    return system_tree_root_node_handle;
}

SCOREP_SystemTreeNodeHandle
SCOREP_GetSystemTreeNodeHandleForSharedMemory( void )
{
    return system_tree_shared_memory_node_handle;
}

void
SCOREP_FinalizeLocationGroup( void )
{
    /* Update location group ID and name */

    uint32_t location_group_id = SCOREP_Status_GetRank();

    SCOREP_LocationGroupDef* location_group
        = SCOREP_LOCAL_HANDLE_DEREF( SCOREP_GetLocationGroup(), LocationGroup );

    /* In early stage 'global location group ID' and 'name' are set to invalid dummies.
     * Correct values must be set manually. */
    location_group->name_handle              = SCOREP_Definitions_NewString( SCOREP_Mpp_GetLocationGroupName() );
    location_group->global_location_group_id = location_group_id;


    /* Set location group in all locations */
    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_BEGIN( &scorep_local_definition_manager, Location, location )
    {
        definition->location_group_id = location_group_id;
    }
    SCOREP_DEFINITIONS_MANAGER_FOREACH_DEFINITION_END();
}
