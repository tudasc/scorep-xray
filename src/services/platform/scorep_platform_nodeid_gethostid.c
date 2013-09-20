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
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 * @file
 *
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
