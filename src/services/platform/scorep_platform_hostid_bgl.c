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
 * @file       scorep_platform_hostid_bgl.c
 * @maintainer Ronny Tschueter <ronny.tschueter@tu-dresden.de>
 *
 * @status alpha
 *
 */


#include <config.h>

#include <SCOREP_Platform.h>

#include <bglpersonality.h>


int32_t
SCOREP_Platform_GetHostId( void )
{
    BGLPersonality mybgl;
    rts_get_personality( &mybgl, sizeof( BGLPersonality ) );

    return ( mybgl.location >> 6 ) & 0x1fff;
}
