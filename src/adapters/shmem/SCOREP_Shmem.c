/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2013-2017,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */


/**
 * @file
 * @ingroup    SHMEM_Wrapper
 *
 * @brief C interface wrappers for initialization routines
 */

#include <config.h>

#include <UTILS_Error.h>
#define SCOREP_DEBUG_MODULE_NAME SHMEM
#include <UTILS_Debug.h>

#include "scorep_shmem_internal.h"

#include <SCOREP_RuntimeManagement.h>
#include <SCOREP_InMeasurement.h>
#include <SCOREP_Events.h>


/* *INDENT-OFF* */

#define INIT_SHMEM( FUNCNAME )                                              \
    void                                                                    \
    SCOREP_LIBWRAP_FUNC_NAME( FUNCNAME ) ( void )                           \
    {                                                                       \
        SCOREP_IN_MEASUREMENT_INCREMENT();                                  \
        if ( SCOREP_IS_MEASUREMENT_PHASE( PRE ) )                           \
        {                                                                   \
            /* Initialize the measurement system */                         \
            SCOREP_InitMeasurement();                                       \
        }                                                                   \
        SCOREP_SHMEM_EVENT_GEN_OFF();                                       \
                                                                            \
        SCOREP_EnterWrappedRegion( scorep_shmem_region__ ## FUNCNAME );     \
                                                                            \
        SCOREP_ENTER_WRAPPED_REGION();                                      \
        SCOREP_LIBWRAP_FUNC_CALL( FUNCNAME, ( ) );                          \
        SCOREP_EXIT_WRAPPED_REGION();                                       \
                                                                            \
        SCOREP_InitMppMeasurement();                                        \
                                                                            \
        SCOREP_ExitRegion( scorep_shmem_region__ ## FUNCNAME );             \
                                                                            \
        /* Enable SHMEM event generation */                                 \
        SCOREP_SHMEM_EVENT_GEN_ON();                                        \
                                                                            \
        SCOREP_IN_MEASUREMENT_DECREMENT();                                  \
    }

/* *INDENT-ON* */

#if SHMEM_HAVE_DECL( SHMEM_INIT )
INIT_SHMEM( shmem_init )
#endif


/* *INDENT-OFF* */

#define INIT_SHMEM_WITH_ARGUMENT( FUNCNAME )                                \
    void                                                                    \
    SCOREP_LIBWRAP_FUNC_NAME( FUNCNAME ) ( int npes )                       \
    {                                                                       \
        SCOREP_IN_MEASUREMENT_INCREMENT();                                  \
        if ( SCOREP_IS_MEASUREMENT_PHASE( PRE ) )                           \
        {                                                                   \
            /* Initialize the measurement system */                         \
            SCOREP_InitMeasurement();                                       \
        }                                                                   \
        SCOREP_SHMEM_EVENT_GEN_OFF();                                       \
                                                                            \
        SCOREP_EnterWrappedRegion( scorep_shmem_region__ ## FUNCNAME );     \
                                                                            \
        SCOREP_ENTER_WRAPPED_REGION();                                      \
        SCOREP_LIBWRAP_FUNC_CALL( FUNCNAME, ( npes ) );                     \
        SCOREP_EXIT_WRAPPED_REGION();                                       \
                                                                            \
        SCOREP_InitMppMeasurement();                                        \
                                                                            \
        SCOREP_ExitRegion( scorep_shmem_region__ ## FUNCNAME );             \
                                                                            \
        /* Enable SHMEM event generation */                                 \
        SCOREP_SHMEM_EVENT_GEN_ON();                                        \
                                                                            \
        SCOREP_IN_MEASUREMENT_DECREMENT();                                  \
    }

/* *INDENT-ON* */

#if SHMEM_HAVE_DECL( START_PES )
INIT_SHMEM_WITH_ARGUMENT( start_pes )
#endif


/* *INDENT-OFF* */

#define INIT_THREAD_SHMEM_ONE_ARG( FUNCNAME )                               \
    int                                                                     \
    SCOREP_LIBWRAP_FUNC_NAME( FUNCNAME ) ( int required )                   \
    {                                                                       \
        SCOREP_IN_MEASUREMENT_INCREMENT();                                  \
        if ( SCOREP_IS_MEASUREMENT_PHASE( PRE ) )                           \
        {                                                                   \
            /* Initialize the measurement system */                         \
            SCOREP_InitMeasurement();                                       \
        }                                                                   \
        SCOREP_SHMEM_EVENT_GEN_OFF();                                       \
                                                                            \
        SCOREP_EnterWrappedRegion( scorep_shmem_region__ ## FUNCNAME );     \
                                                                            \
        SCOREP_ENTER_WRAPPED_REGION();                                      \
        int ret = SCOREP_LIBWRAP_FUNC_CALL( FUNCNAME,                       \
                                            ( required ) );                 \
        SCOREP_EXIT_WRAPPED_REGION();                                       \
                                                                            \
        SCOREP_InitMppMeasurement();                                        \
                                                                            \
        SCOREP_ExitRegion( scorep_shmem_region__ ## FUNCNAME );             \
                                                                            \
        /* Enable SHMEM event generation */                                 \
        SCOREP_SHMEM_EVENT_GEN_ON();                                        \
                                                                            \
        SCOREP_IN_MEASUREMENT_DECREMENT();                                  \
        return ret;                                                         \
    }

#define INIT_THREAD_SHMEM_TWO_ARGS( FUNCNAME )                              \
    void                                                                    \
    SCOREP_LIBWRAP_FUNC_NAME( FUNCNAME ) ( int   required,                  \
                                           int * provided )                 \
    {                                                                       \
        SCOREP_IN_MEASUREMENT_INCREMENT();                                  \
        if ( SCOREP_IS_MEASUREMENT_PHASE( PRE ) )                           \
        {                                                                   \
            /* Initialize the measurement system */                         \
            SCOREP_InitMeasurement();                                       \
        }                                                                   \
        SCOREP_SHMEM_EVENT_GEN_OFF();                                       \
                                                                            \
        SCOREP_EnterWrappedRegion( scorep_shmem_region__ ## FUNCNAME );     \
                                                                            \
        SCOREP_ENTER_WRAPPED_REGION();                                      \
        SCOREP_LIBWRAP_FUNC_CALL( FUNCNAME, ( required, provided ) );       \
        SCOREP_EXIT_WRAPPED_REGION();                                       \
                                                                            \
        SCOREP_InitMppMeasurement();                                        \
                                                                            \
        SCOREP_ExitRegion( scorep_shmem_region__ ## FUNCNAME );             \
                                                                            \
        /* Enable SHMEM event generation */                                 \
        SCOREP_SHMEM_EVENT_GEN_ON();                                        \
                                                                            \
        SCOREP_IN_MEASUREMENT_DECREMENT();                                  \
    }

/* *INDENT-ON* */

#if HAVE( SHMEM_INIT_THREAD_CRAY_ONE_ARG_VARIANT )
INIT_THREAD_SHMEM_ONE_ARG( shmem_init_thread )
#elif HAVE( SHMEM_INIT_THREAD_CRAY_TWO_ARGS_VARIANT )
INIT_THREAD_SHMEM_TWO_ARGS( shmem_init_thread )
#endif

/* *INDENT-OFF* */

#define FINALIZE_SHMEM( FUNCNAME )                                      \
    void                                                                \
    SCOREP_LIBWRAP_FUNC_NAME( FUNCNAME )( void )                        \
    {                                                                   \
        SCOREP_IN_MEASUREMENT_INCREMENT();                              \
        UTILS_DEBUG_ENTRY();                                            \
                                                                        \
        const int event_gen_active = SCOREP_SHMEM_IS_EVENT_GEN_ON;      \
                                                                        \
        if ( event_gen_active )                                         \
        {                                                               \
            SCOREP_SHMEM_EVENT_GEN_OFF();                               \
            SCOREP_EnterWrappedRegion(                                  \
                scorep_shmem_region__ ## FUNCNAME );                    \
        }                                                               \
                                                                        \
        SCOREP_ENTER_WRAPPED_REGION();                                  \
        SCOREP_LIBWRAP_FUNC_CALL( shmem_barrier_all, ( ) );             \
        SCOREP_EXIT_WRAPPED_REGION();                                   \
                                                                        \
        SCOREP_RegisterExitHandler();                                   \
                                                                        \
        if ( event_gen_active )                                         \
        {                                                               \
            /* Exit shmem_finalize region */                            \
            SCOREP_ExitRegion( scorep_shmem_region__ ## FUNCNAME );     \
                                                                        \
            SCOREP_SHMEM_EVENT_GEN_ON();                                \
        }                                                               \
                                                                        \
        SCOREP_IN_MEASUREMENT_DECREMENT();                              \
    }

#define FINALIZE_SHMEM_WITH_RETURN_CODE( FUNCNAME, RETVAL )             \
    int                                                                 \
    SCOREP_LIBWRAP_FUNC_NAME( FUNCNAME )( void )                        \
    {                                                                   \
        SCOREP_IN_MEASUREMENT_INCREMENT();                              \
        UTILS_DEBUG_ENTRY();                                            \
                                                                        \
        const int event_gen_active = SCOREP_SHMEM_IS_EVENT_GEN_ON;      \
                                                                        \
        if ( event_gen_active )                                         \
        {                                                               \
            SCOREP_SHMEM_EVENT_GEN_OFF();                               \
            SCOREP_EnterWrappedRegion(                                  \
                scorep_shmem_region__ ## FUNCNAME );                    \
        }                                                               \
                                                                        \
        SCOREP_ENTER_WRAPPED_REGION();                                  \
        SCOREP_LIBWRAP_FUNC_CALL( shmem_barrier_all, ( ) );             \
        SCOREP_EXIT_WRAPPED_REGION();                                   \
                                                                        \
        SCOREP_RegisterExitHandler();                                   \
                                                                        \
        if ( event_gen_active )                                         \
        {                                                               \
            /* Exit shmem_finalize region */                            \
            SCOREP_ExitRegion( scorep_shmem_region__ ## FUNCNAME );     \
                                                                        \
            SCOREP_SHMEM_EVENT_GEN_ON();                                \
        }                                                               \
                                                                        \
        SCOREP_IN_MEASUREMENT_DECREMENT();                              \
        return RETVAL;                                                  \
    }

/* *INDENT-ON* */

/*
 * Please note that 'shmem_finalize' is not part of the OpenSHMEM standard 1.0.
 * However, we need this function call to write events like RMA window destruction.
 * That's why we always wrap 'shmem_finalize'.
 * The Score-P measurement system always implements this function.
 * If the SHMEM implementation also provides 'shmem_finalize' we wrap it and
 * call the SHMEM library function later at finalization of the measurement system.
 */
#if HAVE( SHMEM_FINALIZE_COMPLIANT ) || !HAVE( DECL_SHMEM_FINALIZE )

FINALIZE_SHMEM( shmem_finalize )

#elif HAVE( SHMEM_FINALIZE_OPENMPI_VARIANT )

/* The return value is equal to OSHMEM_SUCCESS. */
FINALIZE_SHMEM_WITH_RETURN_CODE( shmem_finalize, 0 )

#endif
