/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2013, 2015, 2019,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2013,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2013,
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
 * @brief Support for XL IBM-Compiler
 * Will be triggered by the function trace option by the xl
 * compiler.
 */

#include <config.h>

#include <stdlib.h>
#include <string.h>

#include <UTILS_Error.h>
#define SCOREP_DEBUG_MODULE_NAME COMPILER
#include <UTILS_Debug.h>

#include <UTILS_IO.h>
#include <UTILS_CStr.h>

#include <SCOREP_RuntimeManagement.h>
#include <SCOREP_InMeasurement.h>
#include <SCOREP_Events.h>
#include <SCOREP_Mutex.h>
#include <SCOREP_Filtering.h>

#include "SCOREP_Compiler_Init.h"
#include "scorep_compiler_data.h"

#if HAVE( DEMANGLE )
/* Declaration of external demangling function */
/* It is contained in "demangle.h" */
extern char*
cplus_demangle( const char* mangled,
                int         options );

/* cplus_demangle options */
#define SCOREP_COMPILER_DEMANGLE_NO_OPTS   0
#define SCOREP_COMPILER_DEMANGLE_PARAMS    ( 1 << 0 )  /* include function arguments */
#define SCOREP_COMPILER_DEMANGLE_ANSI      ( 1 << 1 )  /* include const, volatile, etc. */
#define SCOREP_COMPILER_DEMANGLE_VERBOSE   ( 1 << 3 )  /* include implementation details */
#define SCOREP_COMPILER_DEMANGLE_TYPES     ( 1 << 4 )  /* include type encodings */

/* Demangling style. */
static int scorep_compiler_demangle_style = SCOREP_COMPILER_DEMANGLE_PARAMS  |
                                            SCOREP_COMPILER_DEMANGLE_ANSI    |
                                            SCOREP_COMPILER_DEMANGLE_VERBOSE |
                                            SCOREP_COMPILER_DEMANGLE_TYPES;
#endif /* HAVE( DEMANGLE ) */

/**
 * Looks up the region name in the hash table, registers the region
 * if it is not already registered and returns the region handle.
 * If the region is filtered it returns SCOREP_INVALID_REGION.
 * @ param region_name function name
 * @ param file_name   file name
 * @ param line_no     line number
 */
static inline SCOREP_RegionHandle
get_region_handle( const char* region_name,
                   const char* file_name,
                   int         line_no )
{
    UTILS_DEBUG_ENTRY( "%s in %s:%d", region_name, file_name, line_no );

    /* put function to list */
    scorep_compiler_hash_node* hash_node;
    long                       region_key = ( long )region_name;
    if ( ( hash_node = scorep_compiler_hash_get( region_key ) ) == 0 )
    {
        /* The IBM compiler instruments outlined functions of OpenMP parallel regions.
           These functions are called at a stage, where locks do not yet work. Thus,
           make sure that in case of race conditions, functions can only get filtered.
         */
#if defined( __IBMC__ ) && __IBMC__ <= 1100
        SCOREP_MutexLock( scorep_compiler_region_mutex );
#endif
        if ( ( hash_node = scorep_compiler_hash_get( region_key ) ) == 0 )
        {
            char* file = UTILS_CStr_dup( file_name );
            UTILS_IO_SimplifyPath( file );

            const char* region_name_demangled = NULL;
#if HAVE( DEMANGLE )
            /* use demangled name if possible */
            region_name_demangled = cplus_demangle( region_name,
                                                    scorep_compiler_demangle_style );
#endif      /* HAVE( DEMANGLE ) */
            if ( region_name_demangled == NULL )
            {
                region_name_demangled = region_name;
                region_name           = NULL;
            }

            hash_node = scorep_compiler_hash_put( region_key,
                                                  region_name,
                                                  region_name_demangled,
                                                  file, line_no );
            UTILS_DEBUG( "number %ld and put name -- %s -- to list",
                         region_key, region_name );

            /* Check for filters:
                 1. In case OpenMP is used, the XL compiler creates some
                    functions like <func_name>:<func_name>$OL$OL.1 on BG/P or
                    <func_name>@OL@1 on AIX which cause the measurement system
                    to crash. Thus, filter functions which names contain a '$'
                    or '@' symbol.
                    or the symbol starts with '__xl_' and has '_OL_' in it
                 2. POMP and POMP2 functions.
             */
            if ( strchr( region_name_demangled, '$' ) ||
                 strchr( region_name_demangled, '@' ) ||
                 strncmp( region_name_demangled, "POMP", 4 ) == 0 ||
                 strncmp( region_name_demangled, "Pomp", 4 ) == 0 ||
                 strncmp( region_name_demangled, "pomp", 4 ) == 0 ||
                 ( strncmp( region_name_demangled, "__xl_", 5 ) == 0 && strstr( region_name_demangled, "_OL_" ) ) ||
                 SCOREP_Filtering_Match( file, region_name_demangled, region_name ) )
            {
                hash_node->region_handle = SCOREP_INVALID_REGION;
            }
            else
            {
                scorep_compiler_register_region( hash_node );
            }

            free( file );
        }
#if defined( __IBMC__ ) && __IBMC__ <= 1100
        SCOREP_MutexUnlock( scorep_compiler_region_mutex );
#endif
    }
    return hash_node->region_handle;
}

/**
 * @ brief This function is called at the entry of each function.
 * The call is generated by the IBM xl compilers
 *
 * @ param region_name function name
 * @ param file_name   file name
 * @ param line_no     line number
 */
#if ( defined( __IBMC__ ) && __IBMC__ > 1100 ) || defined( __ibmxl__ )

void
__func_trace_enter( const char*          region_name,
                    const char*          file_name,
                    int                  line_no,
                    SCOREP_RegionHandle* handle )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    if ( SCOREP_IS_MEASUREMENT_PHASE( PRE ) )
    {
        SCOREP_InitMeasurement();
    }
    if ( !SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) || SCOREP_IsUnwindingEnabled() )
    {
        *handle = SCOREP_INVALID_REGION;
        SCOREP_IN_MEASUREMENT_DECREMENT();
        return;
    }

    /* The IBM compiler instruments outlined functions of OpenMP parallel regions.
       These functions are called at a stage, where locks do not yet work. Thus,
       make sure that only final valid values are assigned to *handle.
     */
    if ( *handle == 0 )
    {
        SCOREP_MutexLock( scorep_compiler_region_mutex );
        if ( *handle == 0 )
        {
            SCOREP_RegionHandle region = get_region_handle( region_name, file_name, line_no );
            if ( region == SCOREP_INVALID_REGION )
            {
                *handle = SCOREP_FILTERED_REGION;
            }
            else
            {
                *handle = region;
            }
        }
        SCOREP_MutexUnlock( scorep_compiler_region_mutex );
    }
    if ( *handle != SCOREP_FILTERED_REGION )
    {
        SCOREP_EnterRegion( *handle );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}

#else

void
__func_trace_enter( const char* region_name,
                    const char* file_name,
                    int         line_no )
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

    SCOREP_RegionHandle handle = get_region_handle( region_name, file_name, line_no );
    if ( handle != SCOREP_INVALID_REGION )
    {
        SCOREP_EnterRegion( handle );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}

#endif

/**
 * @ brief This function is called at the exit of each function.
 * The call is generated by the IBM xl compilers
 *
 * @ param region_name function name
 * @ param file_name   file name
 * @ param line_no     line number
 */
#if ( defined( __IBMC__ ) && __IBMC__ > 1100 ) || defined( __ibmxl__ )

void
__func_trace_exit( const char*          region_name,
                   const char*          file_name,
                   int                  line_no,
                   SCOREP_RegionHandle* handle )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    if ( !SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) || SCOREP_IsUnwindingEnabled() )
    {
        SCOREP_IN_MEASUREMENT_DECREMENT();
        return;
    }

    UTILS_DEBUG_ENTRY();

    if ( *handle != SCOREP_FILTERED_REGION )
    {
        SCOREP_ExitRegion( *handle );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}

#else

void
__func_trace_exit( char* region_name,
                   char* file_name,
                   int   line_no )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    if ( !SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) || SCOREP_IsUnwindingEnabled() )
    {
        SCOREP_IN_MEASUREMENT_DECREMENT();
        return;
    }

    UTILS_DEBUG_ENTRY();

    scorep_compiler_hash_node* hash_node;
    if ( ( hash_node = scorep_compiler_hash_get( ( long )region_name ) ) )
    {
        /* Invalid handle marks filtered regions */
        if ( ( hash_node->region_handle != SCOREP_INVALID_REGION ) )
        {
            SCOREP_ExitRegion( hash_node->region_handle );
        }
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}

#endif
