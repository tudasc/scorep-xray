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
 * Copyright (c) 2009-2013,
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
 * @file       scorep_platform_system_tree_generic.c
 *
 *
 */


#include <config.h>


#include <stdlib.h>
#include <unistd.h>


#include <UTILS_Error.h>
#include <UTILS_IO.h>


#include <SCOREP_Platform.h>


#include "scorep_platform_system_tree.h"


SCOREP_ErrorCode
scorep_platform_get_path_in_system_tree( SCOREP_Platform_SystemTreePathElement* root )
{
    /* Get hostname */
    SCOREP_Platform_SystemTreePathElement* node = NULL;
    node = scorep_platform_system_tree_bottom_up_add( &node,
                                                      SCOREP_SYSTEM_TREE_DOMAIN_SHARED_MEMORY,
                                                      "node",
                                                      256, "" );
    if ( !node )
    {
        SCOREP_Platform_FreePath( root );
        return UTILS_ERROR( SCOREP_ERROR_MEM_FAULT, "Failed to add hostname node" );
    }

    /* Hook this path up to the root node */
    root->next = node;

    if ( UTILS_IO_GetHostname( node->node_name, 256 ) != 0 )
    {
        SCOREP_Platform_FreePath( root );
        return UTILS_ERROR( SCOREP_ERROR_PROCESSED_WITH_FAULTS,
                            "UTILS_IO_GetHostname() failed." );
    }

    return SCOREP_SUCCESS;
}
