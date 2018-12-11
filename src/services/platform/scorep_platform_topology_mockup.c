/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2015-2017,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 * @file
 *
 * topology mockup file for platforms without defined hardware topology
 */


#include <config.h>

#include <SCOREP_Platform.h>
#include <SCOREP_Subsystem.h>

const SCOREP_Subsystem SCOREP_Subsystem_PlatformTopology =
{
    .subsystem_name = "Platform Topology (mockup)",
};

uint32_t
SCOREP_Platform_GetHardwareTopologyNumberOfDimensions( void )
{
    return 0;
}

bool
SCOREP_Platform_GenerateTopology( void )
{
    return false;
}

SCOREP_ErrorCode
SCOREP_Platform_GetHardwareTopologyInformation( char const** name,
                                                int          nDims,
                                                int*         procsPerDim,
                                                int*         periodicityPerDim,
                                                char*        dimNames[] )
{
    return SCOREP_ERROR_INVALID;
}

SCOREP_ErrorCode
SCOREP_Platform_GetCartCoords(  int                     nCoords,
                                int*                    coords,
                                struct SCOREP_Location* location )
{
    return SCOREP_ERROR_INVALID;
}
