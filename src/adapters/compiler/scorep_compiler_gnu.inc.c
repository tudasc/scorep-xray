/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2012,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2012, 2015-2016,
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

#if !defined( SCOREP_COMPILER_GNU_FUNC_ENTER ) || !defined( SCOREP_COMPILER_GNU_FUNC_EXIT )
#error SCOREP_COMPILER_GNU_FUNC_ENTER and SCOREP_COMPILER_GNU_FUNC_EXIT must be #defined before including this template
#endif

/**
 * @file
 *
 * @brief Template for common compiler instrumention APIs
 */

#include <SCOREP_RuntimeManagement.h>
#include <SCOREP_InMeasurement.h>
#include <SCOREP_Types.h>
#include <SCOREP_Events.h>
#include <SCOREP_Mutex.h>

#define SCOREP_DEBUG_MODULE_NAME COMPILER
#include <UTILS_Debug.h>

#include "SCOREP_Compiler_Init.h"
#include "scorep_compiler_data.h"

/* ***************************************************************************************
   Implementation of functions called by compiler instrumentation
*****************************************************************************************/

/**
 * @brief This function is called just after the entry of a function.
 * @param func      The address of the start of the current function.
 * @param callsite  The call site of the current function.
 */
void
SCOREP_COMPILER_GNU_FUNC_ENTER( void* func,
                                void* callsite )
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

    UTILS_DEBUG_ENTRY( "%p, %p", func, callsite );

    /*
     * put hash table entries via mechanism for bfd symbol table
     * to calculate function addresses if measurement was not initialized
     */

    /* On ARM platform the 0-bit indicates whether it is thumb code or arm code.
       Thus, thumb code address differ from the real function address that we
       get from libbfd or nm by 1 */
#if HAVE( PLATFORM_ARM )
    /* Clear least significant bit */
    func = ( void* )( ( ( long )func | 1 ) - 1 );
#endif

    scorep_compiler_hash_node* hash_node =
        scorep_compiler_hash_get( ( long )func );
    if ( hash_node )
    {
        if ( hash_node->region_handle == SCOREP_INVALID_REGION )
        {
            SCOREP_MutexLock( scorep_compiler_region_mutex );
            if ( hash_node->region_handle == SCOREP_INVALID_REGION )
            {
                /* -- region entered the first time, register region -- */
                scorep_compiler_register_region( hash_node );
            }
            SCOREP_MutexUnlock( scorep_compiler_region_mutex );
        }
        SCOREP_EnterRegion( hash_node->region_handle );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#undef SCOREP_COMPILER_GNU_FUNC_ENTER

/**
 * @brief This function is called just before the exit of a function.
 * @param func      The address of the end of the current function.
 * @param callsite  The call site of the current function.
 */
void
SCOREP_COMPILER_GNU_FUNC_EXIT( void* func,
                               void* callsite )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();

    if ( !SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) || SCOREP_IsUnwindingEnabled() )
    {
        SCOREP_IN_MEASUREMENT_DECREMENT();
        return;
    }

    UTILS_DEBUG_ENTRY( "%p, %p", func, callsite );

    /* On ARM platform the 0-bit indicates whether it is thumb code or arm code.
       Thus, thumb code address differ from the real function address that we
       get from libbfd or nm by 1 */
#if HAVE( PLATFORM_ARM )
    /* Clear least significant bit */
    func = ( void* )( ( ( long )func | 1 ) - 1 );
#endif

    scorep_compiler_hash_node* hash_node;
    if ( ( hash_node = scorep_compiler_hash_get( ( long )func ) ) )
    {
        SCOREP_ExitRegion( hash_node->region_handle );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#undef SCOREP_COMPILER_GNU_FUNC_EXIT
