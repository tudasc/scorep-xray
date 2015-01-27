/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2012-2013,
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
#include <SCOREP_Mutex.h>

#include "SCOREP_Compiler_Init.h"

/* Called from the instrumented function, if the automatic register failed
 * for example, if the function lives in an shared library */
void
scorep_plugin_register_region( const scorep_compiler_region_description* regionDescr )
{
    if ( !scorep_compiler_initialized )
    {
        if ( scorep_compiler_finalized )
        {
            return;
        }

        /* not initialized so far */
        SCOREP_InitMeasurement();
    }

    SCOREP_MutexLock( scorep_compiler_region_mutex );

    if ( *regionDescr->handle == SCOREP_INVALID_REGION )
    {
        scorep_compiler_register_region( regionDescr );
    }

    SCOREP_MutexUnlock( scorep_compiler_region_mutex );
} /* scorep_compiler_register_region */
