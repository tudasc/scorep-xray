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
 * @file        scorep_platform_bgp.c
 * @maintainer  Bert Wesarg <Bert.Wesarg@tu-dresden.de>
 *
 * @status alpha
 *
 * Implementation to obtain the system tree information from a Blue Gene/P
 * system.
 */


#include <config.h>


#include <UTILS_Error.h>


#include <SCOREP_Platform.h>


#include "scorep_platform_system_tree.h"


#include <common/bgp_personality.h>
#include <common/bgp_personality_inlines.h>
#include <spi/kernel_interface.h>


static _BGP_Personality_t                mybgp;
static _BGP_UniversalComponentIdentifier uci;
static bool                              is_bgp_personality_initialized = false;


static inline void
initialize_bqp_personality( void )
{
    if ( !is_bgp_personality_initialized )
    {
        Kernel_GetPersonality( &mybgp, sizeof( _BGP_Personality_t ) );
        uci.UCI = mybgp.Kernel_Config.UniversalComponentIdentifier;

        is_bgp_personality_initialized = true;
    }
}


SCOREP_ErrorCode
SCOREP_Platform_GetPathInSystemTree( SCOREP_Platform_SystemTreePathElement** root )
{
    if ( !root )
    {
        return UTILS_ERROR( SCOREP_ERROR_INVALID_ARGUMENT,
                            "Invalid system tree root reference given." );
    }
    *root = NULL;
    SCOREP_Platform_SystemTreePathElement** tail = root;
    SCOREP_Platform_SystemTreePathElement*  node;


    initialize_bqp_personality();


    node = scorep_platform_system_tree_top_down_add( &tail,
                                                     SCOREP_SYSTEM_TREE_DOMAIN_MACHINE,
                                                     "machine",
                                                     0, "Blue Gene/P" );
    if ( !node )
    {
        goto fail;
    }

    SCOREP_Platform_SystemTreeProperty* property =
        scorep_platform_system_tree_add_property( node,
                                                  "type",
                                                  0, "Blue Gene/P" );
    if ( !property )
    {
        goto fail;
    }

    if ( ( uci.ComputeCard.Component == _BGP_UCI_Component_ComputeCard ) ||
         ( uci.IOCard.Component == _BGP_UCI_Component_IOCard ) )
    {
        unsigned rack_row = uci.ComputeCard.RackRow;
        unsigned rack_col = uci.ComputeCard.RackColumn;
        unsigned midplane = uci.ComputeCard.Midplane;
        unsigned nodecard = uci.ComputeCard.NodeCard;

        node = scorep_platform_system_tree_top_down_add( &tail,
                                                         SCOREP_SYSTEM_TREE_DOMAIN_NONE,
                                                         "rack row",
                                                         16, "%u", rack_row );
        if ( !node )
        {
            goto fail;
        }

        node = scorep_platform_system_tree_top_down_add( &tail,
                                                         SCOREP_SYSTEM_TREE_DOMAIN_NONE,
                                                         "rack column",
                                                         16, "%u", rack_col );
        if ( !node )
        {
            goto fail;
        }

        node = scorep_platform_system_tree_top_down_add( &tail,
                                                         SCOREP_SYSTEM_TREE_DOMAIN_NONE,
                                                         "midplane",
                                                         16, "%u", midplane );
        if ( !node )
        {
            goto fail;
        }

        node = scorep_platform_system_tree_top_down_add( &tail,
                                                         SCOREP_SYSTEM_TREE_DOMAIN_SHARED_MEMORY,
                                                         "nodecard",
                                                         16, "%u", nodecard );
        if ( !node )
        {
            goto fail;
        }
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
    initialize_bqp_personality();

    /*
     * Use upper part of UCI (up to NodeCard, ignore lower 14bits).
     * However, use only the 13 bits (1FFF) that describe row,col,mp,nc.
     */
    return ( uci.UCI >> 14 ) & 0x1FFF;
}
