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
 * @file       scorep_platform_hostid_gethostid.c
 * @maintainer Ronny Tschueter <ronny.tschueter@tu-dresden.de>
 *
 * @status alpha
 *
 */


#include <config.h>

#include <SCOREP_Platform.h>

#define SCOREP_MAX_GETHOSTID_RETRIES 10


/* Provide gethostid declaration if we are able to
 * link against gethostid, but found no declaration */
#if !( HAVE( DECL_GETHOSTID ) ) && HAVE( GETHOSTID )
long
gethostid( void );

#endif


static int32_t host_id = 0;


int32_t
SCOREP_Platform_GetNodeId( void )
{
    uint32_t hostid_retries = 0;

    /* On some machines gethostid() may fail if
     * called only once, try it multiple times */
    while ( !host_id && ( hostid_retries++ < SCOREP_MAX_GETHOSTID_RETRIES ) )
    {
        host_id = ( int32_t )gethostid();
    }

    if ( !host_id )
    {
        UTILS_WARNING( "Maximum retries (%i) for gethostid exceeded!",
                       SCOREP_MAX_GETHOSTID_RETRIES );
    }

    return host_id;
}
