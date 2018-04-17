/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2013, 2015,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2015,
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
 * @brief Initialization of the Sun/Oracle Studio compiler adapter.
 */

#include <config.h>

#include <string.h>

#define SCOREP_DEBUG_MODULE_NAME COMPILER
#include <UTILS_Debug.h>

#include <SCOREP_RuntimeManagement.h>
#include <SCOREP_Definitions.h>
#include <SCOREP_Events.h>
#include <SCOREP_Filtering.h>

#include "SCOREP_Compiler_Init.h"
#include "scorep_compiler_sun_data.h"

/**
 * Handle for the main region, which is not instrumented by the compiler.
 */
static SCOREP_RegionHandle main_region_handle = SCOREP_INVALID_REGION;


SCOREP_ErrorCode
scorep_compiler_subsystem_init( void )
{
    UTILS_DEBUG( "initialize studio compiler adapter!" );

    SCOREP_MutexCreate( &scorep_compiler_region_mutex );

    if ( !SCOREP_IsUnwindingEnabled() )
    {
        main_region_handle = scorep_compiler_register_region( "main" );
    }

    return SCOREP_SUCCESS;
}

SCOREP_ErrorCode
scorep_compiler_subsystem_begin( void )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();

    if ( !SCOREP_IsUnwindingEnabled() )
    {
        /* The studio compiler does not instrument "main" but we want to have a
           main. Note that this main is triggered by the first event that arrives
           at the measurement system. */
        SCOREP_EnterRegion( main_region_handle );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();

    return SCOREP_SUCCESS;
}

void
scorep_compiler_subsystem_end( void )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();

    if ( !SCOREP_IsUnwindingEnabled() )
    {
        /* We manually entered the artificial "main" region. We also need to exit
           it manually. See also scorep_compiler_init_adapter().
           Still no SCOREP_Location_ExitRegion() here.
         */
        SCOREP_ExitRegion( main_region_handle );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}

void
scorep_compiler_subsystem_finalize( void )
{
    UTILS_DEBUG( "finalize studio compiler adapter!" );

    SCOREP_MutexDestroy( &scorep_compiler_region_mutex );
}

SCOREP_ErrorCode
scorep_compiler_subsystem_init_location( struct SCOREP_Location* locationData,
                                         struct SCOREP_Location* parent )
{
    UTILS_DEBUG( "studio compiler adapter init location!" );

    return SCOREP_SUCCESS;
}

SCOREP_RegionHandle
scorep_compiler_register_region( char* region_name )
{
    SCOREP_RegionHandle handle = SCOREP_FILTERED_REGION;

    /* register region with Score-P and store region identifier */
    if ( ( strchr( region_name, '$' ) == NULL ) &&     /* SUN OMP runtime functions */
         ( strncmp( region_name, "__mt_", 5 ) != 0 ) &&
         ( strncmp( region_name, "POMP", 4 ) != 0 ) &&
         ( strncmp( region_name, "Pomp", 4 ) != 0 ) &&
         ( strncmp( region_name, "pomp", 4 ) != 0 ) &&
         ( !SCOREP_Filtering_Match( NULL, region_name, NULL ) ) )
    {
        handle = SCOREP_Definitions_NewRegion( region_name,
                                               NULL,
                                               SCOREP_INVALID_SOURCE_FILE,
                                               SCOREP_INVALID_LINE_NO,
                                               SCOREP_INVALID_LINE_NO,
                                               SCOREP_PARADIGM_COMPILER,
                                               SCOREP_REGION_FUNCTION );
    }
    return handle;
}
