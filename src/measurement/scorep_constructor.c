/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2014, 2019,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2014, 2016,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */


/**
 * @file
 *
 */

#include <config.h>

#include <UTILS_Error.h>
#define SCOREP_DEBUG_MODULE_NAME CORE
#include <UTILS_Debug.h>

#include <SCOREP_RuntimeManagement.h>
#include <SCOREP_InMeasurement.h>

#include "scorep_environment.h"
#include <stdio.h>

#if SCOREP_COMPILER_CONSTRUCTOR_MODE == SCOREP_COMPILER_CONSTRUCTOR_MODE_ATTRIBUTE

void
__attribute__( ( constructor ) )
scorep_constructor( SCOREP_COMPILER_CONSTRUCTOR_PROTO_ARGS );

#elif SCOREP_COMPILER_CONSTRUCTOR_MODE == SCOREP_COMPILER_CONSTRUCTOR_MODE_PRAGMA

void
scorep_constructor( SCOREP_COMPILER_CONSTRUCTOR_PROTO_ARGS );

#pragma init(scorep_constructor)

#endif

void
scorep_constructor( SCOREP_COMPILER_CONSTRUCTOR_PROTO_ARGS )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();

    UTILS_DEBUG_ENTRY();

    if ( SCOREP_IS_MEASUREMENT_PHASE( PRE ) )
    {
        SCOREP_InitMeasurementWithArgs( SCOREP_COMPILER_CONSTRUCTOR_PROXY_ARGS );

        if ( SCOREP_Env_RunVerbose() )
        {
            fprintf( stderr, "[Score-P] initialized via constructor\n" );
#if HAVE( COMPILER_CONSTRUCTOR_ARGS )
            fprintf( stderr, "[Score-P] program: %s", argv[ 0 ] );
            for ( int i = 1; i < argc; i++ )
            {
                fprintf( stderr, " %s", argv[ i ] );
            }
            fprintf( stderr, "\n" );
#endif
        }
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
