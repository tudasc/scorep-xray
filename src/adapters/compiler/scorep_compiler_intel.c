/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2015,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2013, 2017,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2014,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2013,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 */

/**
 * @file
 *
 * @brief Support for Intel Compiler
 * Will be triggered by the '-fcollect' flag of the intel
 * compiler.
 */

#include <config.h>

#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include <UTILS_Error.h>
#define SCOREP_DEBUG_MODULE_NAME COMPILER
#include <UTILS_Debug.h>

#include <SCOREP_RuntimeManagement.h>
#include <SCOREP_InMeasurement.h>
#include <SCOREP_Events.h>
#include <SCOREP_Mutex.h>
#include <SCOREP_Definitions.h>
#include <SCOREP_Filtering.h>

#include "SCOREP_Compiler_Init.h"


/* Register a new region to the measurement system */
static SCOREP_RegionHandle
register_region( const char* str )
{
    /* str is supposed to be of the form "file_name:region_name".
     * There are cases where the compiler just provides "file_name:";
     * for the user these 'regions' are of little use as they can't
     * be linked to the source code. Thus, filter them. */
    uint64_t    file_name_len = 0;
    const char* region_name   = strchr( str, ':' );
    if ( region_name )
    {
        file_name_len = region_name - str;
        region_name++;
        if ( strlen( region_name ) == 0 )
        {
            UTILS_WARNING( "The Intel compiler provided \"%s\" as file:region "
                           "identification. Without the region part we are unable "
                           "to link to the source code, thus, we filter this region. "
                           "The Intel compiler shows this behavior for functions "
                           "declared inside an anonymous namespace.",
                           str );
            return SCOREP_FILTERED_REGION;
        }
    }
    else
    {
        /*
         * We expected a ':', but did not find one, use the whole string
         * as region name.
         */
        region_name = str;
        UTILS_WARNING( "Malformed region string from Intel instrumentation: %s",
                       str );
    }

    /* Get file name */
    char file_name[ file_name_len + 1 ];
    memcpy( file_name, str, file_name_len );
    file_name[ file_name_len ] = '\0';

    /* Filter on file name early to avoid unused SourceFile definitions. */
    if ( SCOREP_Filtering_MatchFile( file_name ) )
    {
        return SCOREP_FILTERED_REGION;
    }

    /* Get file handle */
    SCOREP_SourceFileHandle file_handle = SCOREP_Definitions_NewSourceFile( file_name );

    /* Register file */
    SCOREP_RegionHandle region_handle = SCOREP_FILTERED_REGION;
    if ( ( strncmp( region_name, "POMP", 4 ) != 0 ) &&
         ( strncmp( region_name, "Pomp", 4 ) != 0 ) &&
         ( strncmp( region_name, "pomp", 4 ) != 0 ) &&
         ( !SCOREP_Filtering_MatchFunction( region_name, NULL ) ) )
    {
        region_handle = SCOREP_Definitions_NewRegion( region_name,
                                                      NULL,
                                                      file_handle,
                                                      SCOREP_INVALID_LINE_NO,
                                                      SCOREP_INVALID_LINE_NO,
                                                      SCOREP_PARADIGM_COMPILER,
                                                      SCOREP_REGION_FUNCTION );
    }

    return region_handle;
}


/* ***************************************************************************************
   Implementation of functions called by compiler instrumentation
*****************************************************************************************/
/*
 *  This function is called at the entry of each function
 */

void
__VT_IntelEntry( char*     str,
                 uint32_t* id,
                 uint32_t* id2 )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    if ( SCOREP_IS_MEASUREMENT_PHASE( PRE ) )
    {
        SCOREP_InitMeasurement();
    }
    if ( !SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) || SCOREP_IsUnwindingEnabled() )
    {
        SCOREP_IN_MEASUREMENT_DECREMENT();
        return;
    }

    UTILS_DEBUG_ENTRY( "%s, %u", str, *id );

    /* Register new region if unknown */
    if ( *id == 0 )
    {
        SCOREP_MutexLock( scorep_compiler_region_mutex );
        if ( *id == 0 )
        {
            *id = register_region( str );
        }
        SCOREP_MutexUnlock( scorep_compiler_region_mutex );
    }

    /* Enter event */
    if ( *id != SCOREP_FILTERED_REGION )
    {
        UTILS_DEBUG( "enter the region with id %u ", *id );
        SCOREP_EnterRegion( ( SCOREP_RegionHandle ) * id );
    }

    /* Set exit id */
    *id2 = *id;

    SCOREP_IN_MEASUREMENT_DECREMENT();
}

void
VT_IntelEntry( char*     str,
               uint32_t* id,
               uint32_t* id2 )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    __VT_IntelEntry( str, id, id2 );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}


/*
 * This function is called at the exit of each function
 */

void
__VT_IntelExit( uint32_t* id2 )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    if ( !SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) || SCOREP_IsUnwindingEnabled() )
    {
        SCOREP_IN_MEASUREMENT_DECREMENT();
        return;
    }

    UTILS_DEBUG_ENTRY( "%u", *id2 );

    /* Check if function is filtered */
    if ( *id2 == SCOREP_FILTERED_REGION )
    {
        SCOREP_IN_MEASUREMENT_DECREMENT();
        return;
    }

    SCOREP_ExitRegion( ( SCOREP_RegionHandle ) * id2 );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}

void
VT_IntelExit( uint32_t* id2 )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    __VT_IntelExit( id2 );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}

/*
 * This function is called when an exception is caught
 */

void
__VT_IntelCatch( uint32_t* id2 )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    if ( !SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) || SCOREP_IsUnwindingEnabled() )
    {
        SCOREP_IN_MEASUREMENT_DECREMENT();
        return;
    }

    UTILS_DEBUG_ENTRY( "%u", *id2 );

    /* Check if function is filtered */
    if ( *id2 != SCOREP_FILTERED_REGION )
    {
        SCOREP_ExitRegion( ( SCOREP_RegionHandle ) * id2 );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}

void
VT_IntelCatch( uint32_t* id2 )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    __VT_IntelCatch( id2 );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}

void
__VT_IntelCheck( uint32_t* id2 )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    if ( !SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) || SCOREP_IsUnwindingEnabled() )
    {
        SCOREP_IN_MEASUREMENT_DECREMENT();
        return;
    }

    UTILS_DEBUG_ENTRY( "%u", *id2 );

    /* Check if function is filtered */
    if ( *id2 != SCOREP_FILTERED_REGION )
    {
        SCOREP_ExitRegion( ( SCOREP_RegionHandle ) * id2 );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}

void
VT_IntelCheck( uint32_t* id2 )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    __VT_IntelCheck( id2 );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
