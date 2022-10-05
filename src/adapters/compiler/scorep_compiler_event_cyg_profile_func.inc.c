/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2012,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2012, 2015-2016, 2020,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2012,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2013, 2020-2021,
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

#if !defined( SCOREP_COMPILER_CYG_PROFILE_FUNC_ENTER ) || !defined( SCOREP_COMPILER_CYG_PROFILE_FUNC_EXIT )
#error SCOREP_COMPILER_CYG_PROFILE_FUNC_ENTER and SCOREP_COMPILER_CYG_PROFILE_FUNC_EXIT must be #defined before including this template
#endif

/**
 * @file
 *
 * @brief Template for common compiler instrumention APIs
 */

#include "scorep_compiler_event_func_addr_hash.inc.c"


/* ***************************************************************************************
   Implementation of functions called by compiler instrumentation
*****************************************************************************************/

/**
 * @brief This function is called just after the entry of a function.
 * @param func      The address of the start of the current function.
 * @param callsite  The call site of the current function.
 */
void
SCOREP_COMPILER_CYG_PROFILE_FUNC_ENTER( void* func,
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

#if ( defined __ARM_ARCH && __ARM_ARCH <= 7 ) || ( defined __arm__ )
    /* On ARMv7 and earlier, the LSB of an address indicates whether it is thumb
       code or arm code.  That is, thumb code addresses differ from the real
       function address that we get from libbfd or nm by 1.  Thus, clear LSB. */
    func = ( void* )( ( ( long )func | 1 ) - 1 );
#endif

    bool                   ignored = false;
    func_addr_hash_value_t value;
    value = func_addr_hash_get_and_insert( ( uintptr_t )func, NULL, &ignored );
    if ( value.region != SCOREP_FILTERED_REGION )
    {
        UTILS_DEBUG( "Enter %" PRIuPTR ": %s@%s:%d",
                     ( uintptr_t )func,
                     SCOREP_RegionHandle_GetName( value.region ),
                     SCOREP_RegionHandle_GetFileName( value.region ),
                     SCOREP_RegionHandle_GetBeginLine( value.region ) );
        SCOREP_EnterRegion( value.region );
    }
    else
    {
        UTILS_DEBUG( "Enter %ld: filtered", ( uintptr_t )func );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#undef SCOREP_COMPILER_CYG_PROFILE_FUNC_ENTER

/**
 * @brief This function is called just before the exit of a function.
 * @param func      The address of the end of the current function.
 * @param callsite  The call site of the current function.
 */
void
SCOREP_COMPILER_CYG_PROFILE_FUNC_EXIT( void* func,
                                       void* callsite )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();

    if ( !SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) || SCOREP_IsUnwindingEnabled() )
    {
        SCOREP_IN_MEASUREMENT_DECREMENT();
        return;
    }

#if ( defined __ARM_ARCH && __ARM_ARCH <= 7 ) || ( defined __arm__ )
    /* On ARMv7 and earlier, the LSB of an address indicates whether it is thumb
       code or arm code.  That is, thumb code addresses differ from the real
       function address that we get from libbfd or nm by 1.  Thus, clear LSB. */
    func = ( void* )( ( ( long )func | 1 ) - 1 );
#endif

    func_addr_hash_value_t value;
    if ( func_addr_hash_get( ( uintptr_t )func, &value ) )
    {
        if ( value.region != SCOREP_FILTERED_REGION )
        {
            UTILS_DEBUG( "Exit %" PRIuPTR ": %s@%s:%d",
                         ( uintptr_t )func,
                         SCOREP_RegionHandle_GetName( value.region ),
                         SCOREP_RegionHandle_GetFileName( value.region ),
                         SCOREP_RegionHandle_GetBeginLine( value.region ) );
            SCOREP_ExitRegion( value.region );
        }
        else
        {
            UTILS_DEBUG( "Exit %ld: filtered",  ( uintptr_t )func );
        }
    }
    else
    {
        UTILS_BUG( "Function %ld exited that hasn't been entered", ( uintptr_t )func );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#undef SCOREP_COMPILER_CYG_PROFILE_FUNC_EXIT
