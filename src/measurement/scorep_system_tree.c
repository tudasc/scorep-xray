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
    size_t                          tree_depth  = 0;
    SCOREP_Platform_SystemTreeNode* system_tree =
        SCOREP_Platform_GetSystemTree( &tree_depth );
    if ( system_tree == NULL )
    {
        SCOREP_ERROR_POSIX( "Failed to obtain system tree information." );
        return;
    }

    /* Create SystemTreeNode defintions */
    SCOREP_SystemTreeNodeHandle parent = SCOREP_INVALID_SYSTEM_TREE_NODE;
    for ( int i = tree_depth - 1; i >= 0; i-- )
    {
        parent = SCOREP_DefineSystemTreeNode( parent,
                                              system_tree[ i ].name,
                                              system_tree[ i ].class );
    }

    /* System tree array is not needed anymore */
    SCOREP_Platform_FreePath( system_tree, tree_depth );
    system_tree = NULL;

    /* Create Location Group definition */
    char                       name[ 32 ];
    int                        rank = SCOREP_Mpi_GetRank();
    sprintf( name, "rank %d", rank );
    SCOREP_LocationGroupHandle location_group =
        SCOREP_DefineLocationGroup( rank, parent, name );


    /* Set location group in all locations */
    SCOREP_DEFINITION_FOREACH_DO( &scorep_local_definition_manager, Location, location )
    {
        definition->location_group_id = rank;
    }
    SCOREP_DEFINITION_FOREACH_WHILE();
}
