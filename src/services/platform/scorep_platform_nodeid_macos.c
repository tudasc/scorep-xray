/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2013, 2019,
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
 * @file
 *
 *
 */


#include <config.h>

#include <sys/types.h>
#include <sys/sysctl.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

#include <UTILS_Error.h>

#define MIB_LEN 2

int32_t
SCOREP_Platform_GetNodeId( void )
{
    /* MIB (management information base) array */
    int mib[ MIB_LEN ];
    /* Node id */
    uint32_t node_id;
    /* Size of node_id */
    size_t len;

    /* Get unique numeric SMP-node identifier */
    mib[ 0 ] = CTL_KERN;
    mib[ 1 ] = KERN_HOSTID;
    len      = sizeof( node_id );
    if ( sysctl( mib, MIB_LEN, &node_id, &len, NULL, 0 ) == -1 )
    {
        UTILS_FATAL( "sysctl[KERN_HOSTID] failed: %s", strerror( errno ) );
    }

    return node_id;
}
