/*
 * This file is part of the Scorep-P project (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 *    RWTH Aachen, Germany
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *    Technische Universitaet Dresden, Germany
 *    University of Oregon, Eugene USA
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
#include <SCOREP_Platform.h>
#include <stdlib.h>
#include <unistd.h>

SCOREP_Platform_SystemTreeNode*
SCOREP_Platform_GetSystemTree( size_t* number_of_entries )
{
    /* Initialize */
    SCOREP_Platform_SystemTreeNode* path = NULL;
    *number_of_entries = 0;

    /* Allocate Array with two elements:
       node, machine */
    path = ( SCOREP_Platform_SystemTreeNode* )
           malloc( 2 * sizeof( SCOREP_Platform_SystemTreeNode ) );
    if ( path == NULL )
    {
        return NULL;
    }

    /* Get hostname */
    path[ 0 ].name = ( char* )malloc( 256 * sizeof( char ) );
    if ( path[ 0 ].name == NULL )
    {
        free( path );
        return NULL;
    }
    if ( gethostname( path[ 0 ].name, 256 ) != 0 )
    {
        SCOREP_Platform_FreePath( path, 2 );
        return NULL;
    }
    path[ 0 ].class = "node";

    /* Set machine */
    path[ 1 ].name  = "generic cluster";
    path[ 1 ].class = "machine";

    *number_of_entries = 2;
    return path;
}

void
SCOREP_Platform_FreePath( SCOREP_Platform_SystemTreeNode* path,
                          size_t                          number_of_entries )
{
    free( path[ 0 ].name );
    free( path );
}
