/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2014,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */


/**
 * @file
 * @ingroup    OpenCL_Wrapper
 *
 * @brief C interface wrappers for OpenCL routines
 */

#include <config.h>

#include <SCOREP_RuntimeManagement.h>
#include <SCOREP_Events.h>
#include "SCOREP_Libwrap_Macros.h"

#include "scorep_opencl.h"
#include "scorep_opencl_config.h"
#include "scorep_opencl_regions.h"
#include "scorep_opencl_function_pointers.h"

#include <stdio.h>


#define SCOREP_OPENCL_FUNC_ENTER( func )        \
    if ( !SCOREP_IsInitialized() )              \
    {                                           \
        /* Initialize the measurement system */ \
        SCOREP_InitMeasurement();               \
    }                                           \
    if ( scorep_opencl_record_api )             \
    {                                           \
        SCOREP_EnterRegion( func );             \
    }

#define SCOREP_OPENCL_FUNC_EXIT( func )         \
    if ( scorep_opencl_record_api )             \
    {                                           \
        SCOREP_ExitRegion( func );              \
    }

/**
 * Handles data transfer functions.
 *
 * @param _regionHandle         region handle for the wrapped function
 * @param _direction            data transfer direction
 * @param _bytes                number of bytes to be transfered
 * @param _clQueue              OpenCL command queue
 * @param _clEvt                address of the OpenCL profiling event
 * @param _isBlocking           true, if function is blocking, otherwise false
 * @param _call                 call of the 'real' function
 */
#define SCOREP_OPENCL_ENQUEUE_BUFFER( regionHandle, direction, bytes, clQueue,      \
                                      clEvt, isBlocking, call )                     \
    {                                                                               \
        SCOREP_Location* location = NULL;                                           \
        uint64_t         time     = 0;                                              \
                                                                                    \
        if ( !SCOREP_IsInitialized() )                                              \
        {                                                                           \
            /* Initialize the measurement system */                                 \
            SCOREP_InitMeasurement();                                               \
        }                                                                           \
                                                                                    \
        if ( scorep_opencl_record_api )                                             \
        {                                                                           \
            SCOREP_EnterRegion( regionHandle );                                     \
        }                                                                           \
                                                                                    \
        /* add the buffer transfer to activity buffer */                            \
        if ( scorep_opencl_record_memcpy )                                          \
        {                                                                           \
            scorep_opencl_buffer_entry* mcpy =                                      \
                scorep_opencl_enqueue_buffer( direction, bytes,  clQueue );         \
            if ( clEvt == NULL )                                                    \
            {                                                                       \
                clEvt = &( mcpy->event );                                           \
            }                                                                       \
            call                                                                    \
            mcpy->event = *clEvt;                                                   \
            SCOREP_OPENCL_CALL( clRetainEvent, ( mcpy->event ) );                   \
            mcpy->retained_event = true;                                            \
        }                                                                           \
        else                                                                        \
        {                                                                           \
            call                                                                    \
        }                                                                           \
                                                                                    \
        if ( scorep_opencl_record_api )                                             \
        {                                                                           \
            SCOREP_ExitRegion( regionHandle );                                      \
        }                                                                           \
    }


#if HAVE( OPENCL_VERSION_1_0_SUPPORT )
#include "scorep_opencl_wrap_version_1_0.inc.c"
#endif
#if HAVE( OPENCL_VERSION_1_1_SUPPORT )
#include "scorep_opencl_wrap_version_1_1.inc.c"
#endif
#if HAVE( OPENCL_VERSION_1_2_SUPPORT )
#include "scorep_opencl_wrap_version_1_2.inc.c"
#endif
#if HAVE( OPENCL_VERSION_2_0_SUPPORT )
#include "scorep_opencl_wrap_version_2_0.inc.c"
#endif
