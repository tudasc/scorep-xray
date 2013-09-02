/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
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
 * @file SCOREP_OA_Dummy.c
 *
 * This file contains dummy implementation of Online Access Phase functions
 */

#include <config.h>
#include <stdio.h>

#include "SCOREP_Types.h"


void
SCOREP_OA_PhaseBegin
(
    const SCOREP_RegionHandle handle
)
{
    printf( "Dummy %s\n", __func__ );
}

void
SCOREP_OA_PhaseEnd
(
    const SCOREP_RegionHandle handle
)
{
    printf( "Dummy %s\n", __func__ );
}

void
SCOREP_OA_Register()
{
    printf( "Dummy %s\n", __func__ );
}

void
SCOREP_OA_Finalize()
{
    printf( "Dummy %s\n", __func__ );
}
