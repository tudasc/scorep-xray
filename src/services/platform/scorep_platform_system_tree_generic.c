/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2013, 2016,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2013, 2017,
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
 *
 */


#include <config.h>


#include <stdlib.h>
#include <string.h>


#include <UTILS_Error.h>
#include <UTILS_IO.h>
#include <UTILS_CStr.h>


#include <SCOREP_Platform.h>


#include "scorep_platform_system_tree.h"


SCOREP_ErrorCode
scorep_platform_get_path_in_system_tree( SCOREP_Platform_SystemTreePathElement* root )
{
    /* Get hostname */
    SCOREP_Platform_SystemTreePathElement** tail = &root->next;
    SCOREP_Platform_SystemTreePathElement*  node;

    /*
     * Use the Slurm topology/tree plugin information, if available
     * see https://slurm.schedmd.com/topology.html. The relevant environment
     * variable are SLURM_TOPOLOGY_ADDR and SLURM_TOPOLOGY_ADDR_PATTERN.
     * SLURM_TOPOLOGY_ADDR_PATTERN is of the form "switch.[switch.]node".
     * Although Slurm topology already includes a node, i.e., host, we don't
     * use it as it is not reliable among different MPI and launcher
     * combinations.
     */

    /* UTILS_CStr_dup wont bark on NULL */
    char* env_slurm_topo_addr         = UTILS_CStr_dup( getenv( "SLURM_TOPOLOGY_ADDR" ) );
    char* env_slurm_topo_addr_pattern = UTILS_CStr_dup( getenv( "SLURM_TOPOLOGY_ADDR_PATTERN" ) );

    /* process switches */
    if ( env_slurm_topo_addr && env_slurm_topo_addr_pattern )
    {
        /* We skip the first switch, as we already have the machine root. */
        bool  is_first_switch = true;
        char* addr            = env_slurm_topo_addr;
        char* pattern         = env_slurm_topo_addr_pattern;

        while ( addr && pattern )
        {
            char* addr_next = strchr( addr, '.' );
            if ( addr_next )
            {
                *addr_next++ = '\0';
            }
            char* pattern_next = strchr( pattern, '.' );
            if ( pattern_next )
            {
                *pattern_next++ = '\0';
            }

            if ( 0 == strcmp( pattern, "switch" ) )
            {
                if ( is_first_switch )
                {
                    is_first_switch = false;

                    /*
                     * We assume that there is only one root in the tree. As we
                     * already have our own root (i.e., machine) node, we just
                     * attach this first switch as property to it.
                     */
                    SCOREP_Platform_SystemTreeProperty* property =
                        scorep_platform_system_tree_add_property( root,
                                                                  pattern,
                                                                  0, addr );

                    if ( !property )
                    {
                        goto fail;
                    }
                }
                else
                {
                    node = scorep_platform_system_tree_top_down_add( &tail,
                                                                     SCOREP_SYSTEM_TREE_DOMAIN_NONE,
                                                                     pattern,
                                                                     0, addr );
                    if ( !node )
                    {
                        goto fail;
                    }
                }
            }

            addr    = addr_next;
            pattern = pattern_next;
        }
    }
    free( env_slurm_topo_addr );
    free( env_slurm_topo_addr_pattern );

    /* process host(name) */
    node = scorep_platform_system_tree_top_down_add( &tail,
                                                     SCOREP_SYSTEM_TREE_DOMAIN_SHARED_MEMORY,
                                                     "node",
                                                     256, "" );
    if ( !node )
    {
        goto fail;
    }

    if ( UTILS_IO_GetHostname( node->node_name, 256 ) != 0 )
    {
        SCOREP_Platform_FreePath( root );
        return UTILS_ERROR( SCOREP_ERROR_PROCESSED_WITH_FAULTS,
                            "UTILS_IO_GetHostname() failed." );
    }

    return SCOREP_SUCCESS;

fail:
    free( env_slurm_topo_addr );
    free( env_slurm_topo_addr_pattern );
    SCOREP_Platform_FreePath( root );

    return UTILS_ERROR( SCOREP_ERROR_PROCESSED_WITH_FAULTS,
                        "Failed to build system tree path" );
}
