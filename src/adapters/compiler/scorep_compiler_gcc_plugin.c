/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2012-2013, 2015-2016,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 * @file
 *
 */

#include <config.h>

#include "scorep_compiler_gcc_plugin.h"

#include <SCOREP_RuntimeManagement.h>
#include <SCOREP_InMeasurement.h>
#include <SCOREP_Events.h>
#include <SCOREP_Mutex.h>

#include "SCOREP_Compiler_Init.h"

/* Called from the instrumented function, if the automatic register failed
 * for example, if the function lives in an shared library */
void
scorep_plugin_register_region( const scorep_compiler_region_description* regionDescr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    if ( SCOREP_IS_MEASUREMENT_PHASE( PRE ) )
    {
        SCOREP_InitMeasurement();
    }
    /*
     * Do not handle SCOREP_IsUnwindingEnabled() here, will be done in
     * scorep_compiler_register_region
     */
    if ( !SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_IN_MEASUREMENT_DECREMENT();
        return;
    }

    SCOREP_MutexLock( scorep_compiler_region_mutex );

    if ( *regionDescr->handle == SCOREP_INVALID_REGION )
    {
        scorep_compiler_register_region( regionDescr );
    }

    SCOREP_MutexUnlock( scorep_compiler_region_mutex );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}

void
scorep_plugin_enter_region( SCOREP_RegionHandle regionHandle )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();

    /*
     * If SCOREP_IsUnwindingEnabled() is true, than this function will never be
     * called, because we than have filtered out all regions.
     */
    if ( SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_EnterRegion( regionHandle );
    }
    SCOREP_IN_MEASUREMENT_DECREMENT();
}

void
scorep_plugin_exit_region( SCOREP_RegionHandle regionHandle )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();

    /*
     * If SCOREP_IsUnwindingEnabled() is true, than this function will never be
     * called, because we than have filtered out all regions.
     */
    if ( SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_ExitRegion( regionHandle );
    }
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
