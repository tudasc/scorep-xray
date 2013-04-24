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
 * @status      alpha
 * @file        src/measurement/scorep_system_tree.c
 * @maintainer  Bert Wesarg <Bert.Wesarg@tu-dresden.de>
 *
 * @brief   Implements the system tree creation.
 *
 *
 */

#include <config.h>
#include <UTILS_Error.h>
#include <SCOREP_Platform.h>
#include <SCOREP_RuntimeManagement.h>
#include <definitions/SCOREP_Definitions.h>
#include <scorep_status.h>
#include <stdio.h>
#include <string.h>


SCOREP_Platform_SystemTreePathElement*
SCOREP_BuildSystemTree()
{
    /* Obtain system tree information from platform dependent implementation */

    SCOREP_Platform_SystemTreePathElement* path = 0;
    SCOREP_ErrorCode                       err  = SCOREP_Platform_GetPathInSystemTree( &path );
    if ( err != SCOREP_SUCCESS )
    {
        UTILS_ERROR( err, "Failed to obtain system tree information." );
    }

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
        parent = SCOREP_DefineSystemTreeNode( parent,
                                              node->node_name,
                                              node->node_class );
    }

    /* Create Location Group definition
     *
     * In early stage 'global location group ID' and 'name' are set to invalid dummies.
     * Correct values must be set later on. */
    return SCOREP_DefineLocationGroup( parent );
}

void
SCOREP_FreeSystemTree( SCOREP_Platform_SystemTreePathElement* path )
{
    /* System tree path is not needed anymore */
    SCOREP_Platform_FreePath( path );
}

void
SCOREP_FinalizeLocationGroup()
{
    /* Update location group ID and name */

    char     name[ 32 ];
    uint32_t location_group_id = 0;
    if ( SCOREP_Status_IsMpp() )
    {
        int rank = SCOREP_Status_GetRank();
        sprintf( name, "MPI Rank %d", rank );
        location_group_id = rank;
    }
    else
    {
        strcpy( name, "Process" );
    }
    SCOREP_LocationGroupDef* location_group
        = SCOREP_LOCAL_HANDLE_DEREF( SCOREP_GetLocationGroup(), LocationGroup );

    /* In early stage 'global location group ID' and 'name' are set to invalid dummies.
     * Correct values must be set manually. */
    location_group->name_handle              = SCOREP_DefineString( name );
    location_group->global_location_group_id = location_group_id;


    /* Set location group in all locations */
    SCOREP_DEFINITION_FOREACH_DO( &scorep_local_definition_manager, Location, location )
    {
        definition->location_group_id = location_group_id;
    }
    SCOREP_DEFINITION_FOREACH_WHILE();
}
