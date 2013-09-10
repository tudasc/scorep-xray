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
 * @file
 *
 * @brief Initialization of the Sun/Oracle Studio compiler adapter.
 */

#include <config.h>
#include <string.h>

#include <SCOREP_RuntimeManagement.h>
#include <SCOREP_Events.h>
#include <SCOREP_Filter.h>

#include <UTILS_Debug.h>

#include "SCOREP_Compiler_Init.h"
#include "scorep_compiler_sun_data.h"

/* *INDENT-OFF* */
static int on_scorep_finalize( void );
/* *INDENT-ON* */

/**
 * Handle for the main region, which is not instrumented by the compiler.
 */
SCOREP_RegionHandle scorep_compiler_main_handle = SCOREP_INVALID_REGION;


SCOREP_ErrorCode
scorep_compiler_init_adapter( void )
{
    if ( !scorep_compiler_initialized )
    {
        UTILS_DEBUG_PRINTF( SCOREP_DEBUG_COMPILER,
                            " inititialize studio compiler adapter!" );

        SCOREP_MutexCreate( &scorep_compiler_region_mutex  );
        scorep_compiler_main_handle = scorep_compiler_register_region( "main" );
        SCOREP_RegisterExitCallback( &on_scorep_finalize );

        /* Set flag */
        scorep_compiler_initialized = true;
    }

    return SCOREP_SUCCESS;
}

SCOREP_ErrorCode
scorep_compiler_init_location( struct SCOREP_Location* locationData )
{
    UTILS_DEBUG_PRINTF( SCOREP_DEBUG_COMPILER, "studio compiler adapter init location!" );
    /* The studio compiler does not instrument "main" but we want to have a
       main. Note that this main is triggered by the first event that arrives
       at the measurement system. */
    if ( 0 == SCOREP_Location_GetId( locationData ) )
    {
        /* I would like to call SCOREP_Location_EnterRegion() here,
           but we prevent this for CPU locations. We could check
           the passed locationData against
           SCOREP_Location_GetCurrentCPULocation(). */
        SCOREP_EnterRegion( scorep_compiler_main_handle );
    }
    return SCOREP_SUCCESS;
}

int
on_scorep_finalize( void )
{
    /* We manually entered the artificial "main" region. We also need to exit
       it manually. See also scorep_compiler_init_adapter().
       Still no SCOREP_Location_ExitRegion() here.
     */
    SCOREP_ExitRegion( scorep_compiler_main_handle );
    return 0;
}

void
scorep_compiler_finalize( void )
{
    /* call only, if previously initialized */
    if ( scorep_compiler_initialized )
    {
        scorep_compiler_initialized = false;
        scorep_compiler_finalized   = true;
        UTILS_DEBUG_PRINTF( SCOREP_DEBUG_COMPILER, " finalize studio compiler adapter!" );

        SCOREP_MutexDestroy( &scorep_compiler_region_mutex );
    }
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
         ( !SCOREP_Filter_Match( NULL, region_name, NULL ) ) )
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
