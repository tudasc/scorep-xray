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
 * @file       scorep_platform_hostid_mockup.c
 * @maintainer Ronny Tschueter <ronny.tschueter@tu-dresden.de>
 *
 * @status alpha
 *
 */


#include <config.h>

#include <SCOREP_Platform.h>

int32_t
SCOREP_Platform_GetNodeId( void )
{
    UTILS_WARNING( "Could not determine node ID." );

    return 0;
}
