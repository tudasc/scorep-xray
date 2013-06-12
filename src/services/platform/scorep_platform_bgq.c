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
 * @file        scorep_platform_bgq.c
 * @maintainer  Alexandre Strube <a.strube@fz-juelich.de>
 *
 * @status alpha
 *
 * Implementation to obtain the system tree information from a Blue Gene/Q
 * system.
 */

#include <config.h>
#include <stdio.h>
#include <math.h>
#include <limits.h>
#include <string.h>
#include <pami.h>

#include <UTILS_Error.h>
#include <SCOREP_Platform.h>
#include "scorep_platform_system_tree.h"

SCOREP_ErrorCode
SCOREP_Platform_GetPathInSystemTree( SCOREP_Platform_SystemTreePathElement** root )
{
    if ( !root )
    {
        return UTILS_ERROR( SCOREP_ERROR_INVALID_ARGUMENT,
                            "Invalid system tree root reference given." );
    }
    *root = NULL;

    /* initialize the client */
    char*         clientname = "";
    pami_client_t client;
    pami_result_t result = PAMI_Client_create( clientname, &client, NULL, 0 );
    UTILS_ASSERT( result == PAMI_SUCCESS );

    /* PAMI on BG/Q returns a string (!) with Processor name and coordinates. */
    pami_configuration_t config;
    config.name = PAMI_CLIENT_PROCESSOR_NAME;
    result      = PAMI_Client_query( client, &config, 1 );
    UTILS_ASSERT( result == PAMI_SUCCESS );

    /* Map the coordinates to values */
    unsigned task;
    unsigned total_tasks;
    unsigned acoord, bcoord, ccoord, dcoord, ecoord, tcoord;
    unsigned rack;
    unsigned midplane;
    unsigned nodeboard;
    unsigned nodecard;
    sscanf( config.value.chararray,
            "Task %u of %u (%u,%u,%u,%u,%u,%u)  R%u-M%u-N%u-J%u",
            &task, &total_tasks,
            &acoord, &bcoord, &ccoord, &dcoord, &ecoord, &tcoord,
            &rack, &midplane, &nodeboard, &nodecard );

    /* finalize the client */
    result = PAMI_Client_destroy( &client );
    UTILS_ASSERT( result == PAMI_SUCCESS );

    SCOREP_Platform_SystemTreePathElement** tail = root;
    SCOREP_Platform_SystemTreePathElement*  node;

    node = scorep_platform_system_tree_top_down_add( &tail,
                                                     SCOREP_SYSTEM_TREE_DOMAIN_MACHINE,
                                                     "machine",
                                                     0, "Blue Gene/Q" );
    if ( !node )
    {
        goto fail;
    }

    SCOREP_Platform_SystemTreeProperty* property =
        scorep_platform_system_tree_add_property( node,
                                                  "type",
                                                  0, "Blue Gene/Q" );
    if ( !property )
    {
        goto fail;
    }

    // Maximum unsigned string's length
    size_t max_uint_digits = floor( log10( ( double )UINT_MAX ) ) + 1;

    node = scorep_platform_system_tree_top_down_add( &tail,
                                                     SCOREP_SYSTEM_TREE_DOMAIN_NONE,
                                                     "rack",
                                                     max_uint_digits, "%u", rack );
    if ( !node )
    {
        goto fail;
    }
    node = scorep_platform_system_tree_top_down_add( &tail,
                                                     SCOREP_SYSTEM_TREE_DOMAIN_NONE,
                                                     "midplane",
                                                     max_uint_digits, "%u", midplane );
    if ( !node )
    {
        goto fail;
    }
    node = scorep_platform_system_tree_top_down_add( &tail,
                                                     SCOREP_SYSTEM_TREE_DOMAIN_NONE,
                                                     "nodeboard",
                                                     max_uint_digits, "%u", nodeboard );
    if ( !node )
    {
        goto fail;
    }
    node = scorep_platform_system_tree_top_down_add( &tail,
                                                     SCOREP_SYSTEM_TREE_DOMAIN_SHARED_MEMORY,
                                                     "nodecard",
                                                     max_uint_digits, "%u", nodecard );
    if ( !node )
    {
        goto fail;
    }

    return SCOREP_SUCCESS;

fail:
    SCOREP_Platform_FreePath( *root );

    return UTILS_ERROR( SCOREP_ERROR_PROCESSED_WITH_FAULTS,
                        "Failed to build system tree path" );
}

SCOREP_ErrorCode
SCOREP_Platform_DefineNodeTree( SCOREP_SystemTreeNodeHandle parent )
{
    /* No further information available */
    return SCOREP_SUCCESS;
}


int32_t
SCOREP_Platform_GetHostId( void )
{
    /*
     * Kernel_GetPersonality( Personality_t*, sizet ) is defined only
     * in libmpich, use another way to determine a unique host ID
     *
     * unsigned rack;      // [0,n] n=95 for Sequoia    8 bit
     * unsigned midplane;  // [0,1]                     1 bit
     * unsigned nodeboard; // [0,15]                    4 bit
     * unsigned nodecard;  // [0,31]                    5 bit
     *
     * +--------------+------------+----------+------+-----+
     * | Compute Card | Node Board | Midplane | Rack | /// |
     * +--------------+------------+----------+------+-----+
     * 0               5            9          10     18
     */

    /* Initialize the client */
    char*         clientname = "";
    pami_client_t client;
    pami_result_t result = PAMI_Client_create( clientname, &client, NULL, 0 );
    UTILS_ASSERT( result == PAMI_SUCCESS );

    /* PAMI on BG/Q returns a string (!) with Processor name and coordinates. */
    pami_configuration_t config;
    config.name = PAMI_CLIENT_PROCESSOR_NAME;
    result      = PAMI_Client_query( client, &config, 1 );
    UTILS_ASSERT( result == PAMI_SUCCESS );

    /* Map the coordinates to values */
    unsigned task;
    unsigned total_tasks;
    unsigned acoord, bcoord, ccoord, dcoord, ecoord, tcoord;
    unsigned rack;
    unsigned midplane;
    unsigned nodeboard;
    unsigned nodecard;
    sscanf( config.value.chararray,
            "Task %u of %u (%u,%u,%u,%u,%u,%u)  R%u-M%u-N%u-J%u",
            &task, &total_tasks,
            &acoord, &bcoord, &ccoord, &dcoord, &ecoord, &tcoord,
            &rack, &midplane, &nodeboard, &nodecard );

    /* Finalize the client */
    result = PAMI_Client_destroy( &client );
    UTILS_ASSERT( result == PAMI_SUCCESS );

    int32_t host_id = 0x0;
    host_id |=   ( ( rack      & 0x00FF /* 8 bit */ ) << 10 )
               | ( ( midplane  & 0x0001 /* 1 bit */ ) << 9 )
               | ( ( nodeboard & 0x000F /* 4 bit */ ) << 5 )
               | ( nodecard  & 0x001F /* 5 bit */ );

    return host_id;
}
