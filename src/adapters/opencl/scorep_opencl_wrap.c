/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2014-2015,
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
#ifdef SCOREP_LIBWRAP_SHARED
#include "scorep_opencl_function_pointers.h"
#endif

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
 * Handles wrapped OpenCL data transfer functions. This macro returns the
 * function that uses it! It evaluates the return value of the given function
 * 'call' and uses it in the return statement.
 *
 * @param regionHandle          region handle for the wrapped function
 * @param direction             data transfer direction
 * @param bytes                 number of bytes to be transfered
 * @param clQueue               OpenCL command queue
 * @param clEvt                 address of the OpenCL profiling event
 * @param isBlocking            true, if function is blocking, otherwise false
 * @param call                  call of the 'real' function
 */
#define SCOREP_OPENCL_ENQUEUE_BUFFER( regionHandle, direction, bytes, clQueue, \
                                      clEvt, isBlocking, call )                \
    {                                                                          \
        cl_int           ret;                                                  \
        SCOREP_Location* location = NULL;                                      \
        uint64_t         time     = 0;                                         \
                                                                               \
        if ( !SCOREP_IsInitialized() )                                         \
        {                                                                      \
            /* Initialize the measurement system */                            \
            SCOREP_InitMeasurement();                                          \
        }                                                                      \
                                                                               \
        if ( scorep_opencl_record_api )                                        \
        {                                                                      \
            SCOREP_EnterRegion( regionHandle );                                \
        }                                                                      \
                                                                               \
        /* add the buffer transfer to activity buffer */                       \
        if ( scorep_opencl_record_memcpy )                                     \
        {                                                                      \
            scorep_opencl_queue* queue =                                       \
                scorep_opencl_queue_get( clQueue );                            \
            scorep_opencl_buffer_entry* mcpy =                                 \
                scorep_opencl_get_buffer_entry( queue );                       \
                                                                               \
            if ( mcpy && clEvt == NULL )                                       \
            {                                                                  \
                clEvt = &( mcpy->event );                                      \
            }                                                                  \
            ret = call;                                                        \
            if ( queue && mcpy && CL_SUCCESS == ret )                          \
            {                                                                  \
                scorep_opencl_retain_buffer( queue, mcpy, direction, bytes );  \
            }                                                                  \
        }                                                                      \
        else                                                                   \
        {                                                                      \
            ret = call;                                                        \
        }                                                                      \
                                                                               \
        if ( scorep_opencl_record_api )                                        \
        {                                                                      \
            SCOREP_ExitRegion( regionHandle );                                 \
        }                                                                      \
        return ret;                                                            \
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
