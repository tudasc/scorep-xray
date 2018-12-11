/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2016-2017,
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
 *
 * topology information for a Blue Gene/Q system.
 */

#include <config.h>
#include <pami.h>

#include <UTILS_Error.h>
#include <SCOREP_Platform.h>
#include "scorep_platform_system_tree.h"

/* bgq personality */
#include <spi/include/kernel/location.h>
#include <UTILS_CStr.h>

#include <SCOREP_Subsystem.h>

const SCOREP_Subsystem SCOREP_Subsystem_PlatformTopology =
{
    .subsystem_name = "Platform Topology (bgq)",
};

/* Hardware Topology */
static const uint32_t scorep_platform_number_of_dimensions = 7;
static const char*    scorep_platform_topology_name        = "BGQ 5D Torus";

uint32_t
SCOREP_Platform_GetHardwareTopologyNumberOfDimensions( void )
{
    return scorep_platform_number_of_dimensions;
}

bool
SCOREP_Platform_GenerateTopology( void )
{
    /* no additional restrictions */
    return true;
}

SCOREP_ErrorCode
SCOREP_Platform_GetHardwareTopologyInformation( char const** name,
                                                int          nDims,
                                                int*         procsPerDim,
                                                int*         periodicityPerDim,
                                                char*        dimNames[] )
{
    Personality_t pers;
    Kernel_GetPersonality( &pers, sizeof( pers ) );

    UTILS_BUG_ON( nDims != number_of_dimensions, "Length of arrays differs from number of dimensions!" );

    *name         = scorep_platform_topology_name;
    dimNames[ 0 ] = "A";
    dimNames[ 1 ] = "B";
    dimNames[ 2 ] = "C";
    dimNames[ 3 ] = "D";
    dimNames[ 4 ] = "E";
    dimNames[ 5 ] = "Core ID";
    dimNames[ 6 ] = "Hardware Thread";


    procsPerDim[ 0 ] = pers.Network_Config.Anodes;
    procsPerDim[ 1 ] = pers.Network_Config.Bnodes;
    procsPerDim[ 2 ] = pers.Network_Config.Cnodes;
    procsPerDim[ 3 ] = pers.Network_Config.Dnodes;
    procsPerDim[ 4 ] = pers.Network_Config.Enodes;
    procsPerDim[ 5 ] = 16;
    procsPerDim[ 6 ] =  4;


    uint64_t n_flags = ( uint64_t )pers.Network_Config.NetFlags;

    if ( n_flags & ND_ENABLE_TORUS_DIM_A )
    {
        periodicityPerDim[ 0 ] = 1;
    }
    else
    {
        periodicityPerDim[ 0 ] = 0;
    }
    if ( n_flags & ND_ENABLE_TORUS_DIM_B )
    {
        periodicityPerDim[ 1 ] = 1;
    }
    else
    {
        periodicityPerDim[ 1 ] = 0;
    }
    if ( n_flags & ND_ENABLE_TORUS_DIM_C )
    {
        periodicityPerDim[ 2 ] = 1;
    }
    else
    {
        periodicityPerDim[ 2 ] = 0;
    }
    if ( n_flags & ND_ENABLE_TORUS_DIM_D )
    {
        periodicityPerDim[ 3 ] = 1;
    }
    else
    {
        periodicityPerDim[ 3 ] = 0;
    }
    if ( n_flags & ND_ENABLE_TORUS_DIM_E )
    {
        periodicityPerDim[ 4 ] = 1;
    }
    else
    {
        periodicityPerDim[ 4 ] = 0;
    }
    periodicityPerDim[ 5 ] = 0;
    periodicityPerDim[ 6 ] = 0;

    return SCOREP_SUCCESS;
}

SCOREP_ErrorCode
SCOREP_Platform_GetCartCoords(  int                     nCoords,
                                int*                    coords,
                                struct SCOREP_Location* location )
{
    Personality_t pers;
    Kernel_GetPersonality( &pers, sizeof( pers ) );

    UTILS_BUG_ON( nCoords != number_of_dimensions, "Length of coordinate array differs from number of dimensions!" );

    coords[ 0 ] = pers.Network_Config.Acoord;
    coords[ 1 ] = pers.Network_Config.Bcoord;
    coords[ 2 ] = pers.Network_Config.Ccoord;
    coords[ 3 ] = pers.Network_Config.Dcoord;
    coords[ 4 ] = pers.Network_Config.Ecoord;
    coords[ 5 ] = Kernel_ProcessorCoreID();   // 0-15
    coords[ 6 ] = Kernel_ProcessorThreadID(); // 0-3

    return SCOREP_SUCCESS;
}
