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
 * @file       scorep_platform_hostid_crayxt.c
 * @maintainer Ronny Tschueter <ronny.tschueter@tu-dresden.de>
 *
 * @status alpha
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
SCOREP_Platform_GetHostId( void )
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
