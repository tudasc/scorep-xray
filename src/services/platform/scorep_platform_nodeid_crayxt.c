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
 * @file
 *
 *
 */


#include <config.h>

#include <SCOREP_Platform.h>

#ifndef __LIBCATAMOUNT__
#  include <sys/types.h>
#  include <sys/stat.h>
#  include <fcntl.h>
#endif

int32_t
SCOREP_Platform_GetNodeId( void )
{
    /* Get unique numeric SMP-node identifier */
#ifdef __LIBCATAMOUNT__
    return ( int32_t )_my_pnid;
#else
    char    buf[ 256 ];
    ssize_t bytes;
    int     fd;

    fd = open( "/proc/cray_xt/nid", O_RDONLY );
    if ( fd < 0 )
    {
        UTILS_WARNING( "Can not open file /proc/cray_xt/nid: %s",
                       strerror( errno ) );
        return 0;
    }
    bytes = read( fd, buf, 256 );
    if ( bytes <= 0 )
    {
        UTILS_WARNING( "Can not read file /proc/cray_xt/nid: %s",
                       strerror( errno ) );
        return 0;
    }
    close( fd );
    return atol( buf );
#endif
}
