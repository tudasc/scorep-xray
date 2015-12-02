/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2012,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2012, 2015,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2012,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2013,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2012,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2012,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 */


/**
 * @file
 *
 * @brief Compiler adapter version for NEC SX compiler
 */

#include <config.h>

#include <stdlib.h>
#include <string.h>

#include <UTILS_Error.h>
#define SCOREP_DEBUG_MODULE_NAME COMPILER
#include <UTILS_Debug.h>

#include <SCOREP_RuntimeManagement.h>
#include <SCOREP_InMeasurement.h>
#include <SCOREP_Events.h>
#include <SCOREP_Mutex.h>
#include <SCOREP_Filter.h>

#include "SCOREP_Compiler_Init.h"
#include "scorep_compiler_data.h"


extern void*
scorep_ftrace_getname( void );
extern int
scorep_ftrace_getname_len( void );


void
_ftrace_enter2_( void );
void
_ftrace_exit2_( void );
void
_ftrace_stop2_( void );

/*
 * This function is called at the entry of each function
 * The call is generated by the NEC SX compilers
 */

void
_ftrace_enter2_( void )
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

    char* region_name = scorep_ftrace_getname();
    int   name_len    = scorep_ftrace_getname_len();
    UTILS_DEBUG_ENTRY( "name: %s, length: %i", region_name, name_len );

    scorep_compiler_hash_node* hash_node = scorep_compiler_hash_get( ( long )region_name );
    if ( !hash_node )
    {
        scorep_compiler_hash_put( ( long )region_name, region_name, region_name, "", name_len );
    }

    if ( ( hash_node = scorep_compiler_hash_get( ( long )region_name ) ) )
    {
        if ( hash_node->region_handle == SCOREP_INVALID_REGION )
        {
            SCOREP_MutexLock( scorep_compiler_region_mutex );
            if ( hash_node->region_handle == SCOREP_INVALID_REGION )
            {
                /* Check for filtered regions */
                if ( ( strncmp( region_name, "POMP", 4 ) == 0 ) ||
                     ( strncmp( region_name, "Pomp", 4 ) == 0 ) ||
                     ( strncmp( region_name, "pomp", 4 ) == 0 ) ||
                     SCOREP_Filter_Match( NULL, region_name, NULL ) )
                {
                    hash_node->region_handle = SCOREP_FILTERED_REGION;
                }

                /* Region entered the first time, register region */
                else
                {
                    scorep_compiler_register_region( hash_node );
                    UTILS_ASSERT( hash_node->region_handle != SCOREP_FILTERED_REGION );
                    UTILS_ASSERT( hash_node->region_handle != SCOREP_INVALID_REGION );
                }
            }
            SCOREP_MutexUnlock( scorep_compiler_region_mutex );
        }
        if ( hash_node->region_handle != SCOREP_FILTERED_REGION )
        {
            SCOREP_EnterRegion( hash_node->region_handle );
        }
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}


/*
 * This function is called at the exit of each function
 * The call is generated by the NEC SX compilers
 */

void
_ftrace_exit2_( void )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    if ( !SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) || SCOREP_IsUnwindingEnabled() )
    {
        SCOREP_IN_MEASUREMENT_DECREMENT();
        return;
    }

    char* region_name = scorep_ftrace_getname();
    long  key         = ( long )region_name;
    UTILS_DEBUG( "name: %s, key: %ld", region_name, key );

    scorep_compiler_hash_node* hash_node = scorep_compiler_hash_get( key );
    if ( hash_node )
    {
        if ( hash_node->region_handle != SCOREP_FILTERED_REGION )
        {
            SCOREP_ExitRegion( hash_node->region_handle );
        }
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}

/*
 * This function is called at the exit of the program
 * The call is generated by the NEC SX compilers
 */
void
_ftrace_stop2_( void )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    UTILS_DEBUG_ENTRY();
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
