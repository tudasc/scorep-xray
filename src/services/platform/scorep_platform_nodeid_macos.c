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
 * @file
 * @maintainer Ronny Tschueter <ronny.tschueter@tu-dresden.de>
 *
 * @status alpha
 *
 */


#include <config.h>

#include <sys/types.h>
#include <sys/sysctl.h>

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
        vt_error_msg( "sysctl[KERN_HOSTID] failed: %s", strerror( errno ) );
    }
}
