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
 * @maintainer Yury Oleynik <oleynik@in.tum.de>
 * @status alpha
 *
 * This file contains dummy implementation of Online Access Phase functions
 */

#include <config.h>
#include <stdio.h>

#include "scorep/SCOREP_User_Functions.h"
#include "SCOREP_Types.h"


void
SCOREP_OA_PhaseBegin
(
    SCOREP_User_RegionHandle*    handle,
    const char**                 lastFileName,
    SCOREP_SourceFileHandle*     lastFile,
    const char*                  name,
    const SCOREP_User_RegionType regionType,
    const char*                  fileName,
    const uint32_t               lineNo
)
{
    printf( "Dummy %s\n", __func__ );
}

void
SCOREP_OA_PhaseEnd
(
    const SCOREP_User_RegionHandle handle
)
{
    printf( "Dummy %s\n", __func__ );
}

void
SCOREP_OA_Register()
{
}

void
SCOREP_OA_Finalize()
{
}
