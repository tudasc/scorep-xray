/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2022,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 *  @file
 *
 *  Implementation of the HIP runtime API event logging via HIP callbacks.
 */

#include <config.h>

#include "scorep_hip_callbacks.h"

#define SCOREP_DEBUG_MODULE_NAME HIP
#include <UTILS_Debug.h>
#include <UTILS_Error.h>

#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>

#include <roctracer_hip.h>

#include "scorep_hip.h"

// Macro to check ROC-tracer calls status
// Note that this applies only to calls returning `roctracer_status_t`!
#define SCOREP_ROCTRACER_CALL( call ) \
    do { \
        roctracer_status_t err = call; \
        if ( err != ROCTRACER_STATUS_SUCCESS ) { \
            UTILS_FATAL( "Call '%s' failed with: %s", #call, roctracer_error_string() ); \
        } \
    } while ( 0 )

// Init tracing routine
void
scorep_hip_callbacks_init( void )
{
    UTILS_DEBUG( "######################### Init ROC tracer" );
    UTILS_DEBUG( "    Enabled features: %" PRIx64, scorep_hip_features );
}

void
scorep_hip_callbacks_finalize( void )
{
    UTILS_DEBUG( "######################### Finalize ROC tracer" );
}

#define ENABLE_TRACING( fn, callback )      \
    do {                                    \
        SCOREP_ROCTRACER_CALL(              \
            roctracer_enable_op_callback(   \
                ACTIVITY_DOMAIN_HIP_API,    \
                HIP_API_ID_ ## fn,          \
                callback, NULL ) );         \
    } while ( 0 )

// Start tracing routine
void
scorep_hip_callbacks_enable( void )
{
    UTILS_DEBUG( "############################## Start HIP tracing" );
}

// Stop tracing routine
void
scorep_hip_callbacks_disable( void )
{
    UTILS_DEBUG( "############################## Stop HIP tracing" );
}
