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
 * @status      alpha
 * @file        scorep_system_tree.c
 * @maintainer  Bert Wesarg <Bert.Wesarg@tu-dresden.de>
 *
 * @brief   Implements the system tree creation.
 *
 *
 */

#include <config.h>
#include <scorep_utility/SCOREP_Utils.h>
#include <SCOREP_Platform.h>
#include <scorep_definitions.h>
#include <scorep_mpi.h>
#include <stdio.h>

extern SCOREP_DefinitionManager scorep_local_definition_manager;

void
SCOREP_DefineSystemTree()
{
    /* Obtain system tree information from platform dependent implementation */

    SCOREP_Platform_SystemTreePathElement* path;
    SCOREP_Error_Code                      err = SCOREP_Platform_GetPathInSystemTree( &path );
    if ( err != SCOREP_SUCCESS )
    {
        SCOREP_ERROR( err, "Failed to obtain system tree information." );
        return;
    }

    /* Create SystemTreeNode defintions */
    SCOREP_SystemTreeNodeHandle            parent = SCOREP_INVALID_SYSTEM_TREE_NODE;
    SCOREP_Platform_SystemTreePathElement* node;
    SCOREP_PLATFORM_SYSTEM_TREE_FORALL( path, node )
    {
        parent = SCOREP_DefineSystemTreeNode( parent,
                                              node->node_name,
                                              node->node_class );
    }

    /* System tree path is not needed anymore */
    SCOREP_Platform_FreePath( path );
    path = NULL;

    /* Create Location Group definition */
    char     name[ 32 ];
    uint64_t location_group_id = 0;
    if ( SCOREP_Mpi_HasMpi() )
    {
        int rank = SCOREP_Mpi_GetRank();
        sprintf( name, "MPI Rank %d", rank );
        location_group_id = rank;
    }
    else if ( scorep_local_definition_manager.location_definition_counter > 1 )
    {
        strcpy( name, "OpenMP Process" );
    }
    else
    {
        strcpy( name, "Process" );
    }
    SCOREP_LocationGroupHandle location_group =
        SCOREP_DefineLocationGroup( location_group_id, parent, name );


    /* Set location group in all locations */
    SCOREP_DEFINITION_FOREACH_DO( &scorep_local_definition_manager, Location, location )
    {
        definition->location_group_id = location_group_id;
    }
    SCOREP_DEFINITION_FOREACH_WHILE();
}
