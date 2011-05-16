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
 * @file       scorep_platform_dummy.c
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status alpha
 *
 */


#include <config.h>


#include <stdlib.h>
#include <unistd.h>


#include <scorep_utility/SCOREP_Error.h>


#include <SCOREP_Platform.h>


#include "scorep_platform_system_tree.h"


SCOREP_Error_Code
SCOREP_Platform_GetPathInSystemTree( SCOREP_Platform_SystemTreePathElement** root )
{
    /* Initialize */
    if ( !root )
    {
        return SCOREP_ERROR( SCOREP_ERROR_INVALID_ARGUMENT, "" );
    }
    *root = NULL;

    /* Get hostname */
    SCOREP_Platform_SystemTreePathElement* node =
        scorep_platform_system_tree_bottom_up_add( root,
                                                   "node",
                                                   256, "" );
    if ( !node )
    {
        return SCOREP_ERROR( SCOREP_ERROR_MEM_FAULT, "Failed to add hostname node" );
    }

    if ( gethostname( node->node_name, 256 ) != 0 )
    {
        int errno_safed = errno;
        SCOREP_Platform_FreePath( *root );
        errno = errno_safed;
        return SCOREP_ERROR_POSIX( "gethostname() failed." );
    }

    /* Set machine */
    node = scorep_platform_system_tree_bottom_up_add( root,
                                                      "machine",
                                                      0, "generic cluster" );
    if ( !node )
    {
        SCOREP_Platform_FreePath( *root );
        return SCOREP_ERROR( SCOREP_ERROR_PROCESSED_WITH_FAULTS,
                             "Failed to build system tree path" );
    }

    return SCOREP_SUCCESS;
}
