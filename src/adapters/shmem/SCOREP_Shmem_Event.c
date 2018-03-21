/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2013-2015, 2017,
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
 * @brief C interface wrappers for event routines
 *        (release, post, test, or wait for an event)
 */

#include <config.h>

#include "scorep_shmem_internal.h"
#include <SCOREP_InMeasurement.h>
#include <SCOREP_Events.h>


/* *INDENT-OFF* */

#define MANAGE_EVENT( FUNCNAME )                                            \
    void                                                                    \
    SCOREP_LIBWRAP_FUNC_NAME( FUNCNAME ) ( long * event )                   \
    {                                                                       \
        SCOREP_IN_MEASUREMENT_INCREMENT();                                  \
                                                                            \
        const int event_gen_active = SCOREP_SHMEM_IS_EVENT_GEN_ON;          \
                                                                            \
        if ( event_gen_active )                                             \
        {                                                                   \
            SCOREP_SHMEM_EVENT_GEN_OFF();                                   \
                                                                            \
            SCOREP_EnterWrappedRegion( scorep_shmem_region__ ## FUNCNAME ); \
        }                                                                   \
                                                                            \
        SCOREP_ENTER_WRAPPED_REGION();                                      \
        SCOREP_LIBWRAP_FUNC_CALL( FUNCNAME, ( event ) );                    \
        SCOREP_EXIT_WRAPPED_REGION();                                       \
                                                                            \
        if ( event_gen_active )                                             \
        {                                                                   \
            SCOREP_ExitRegion( scorep_shmem_region__ ## FUNCNAME );         \
                                                                            \
            SCOREP_SHMEM_EVENT_GEN_ON();                                    \
        }                                                                   \
                                                                            \
        SCOREP_IN_MEASUREMENT_DECREMENT();                                  \
    }

/* *INDENT-ON* */

#if SHMEM_HAVE_DECL( SHMEM_CLEAR_EVENT )
MANAGE_EVENT( shmem_clear_event )
#endif
#if SHMEM_HAVE_DECL( SHMEM_SET_EVENT )
MANAGE_EVENT( shmem_set_event )
#endif
#if SHMEM_HAVE_DECL( SHMEM_WAIT_EVENT )
MANAGE_EVENT( shmem_wait_event )
#endif


/* *INDENT-OFF* */

#define TEST_EVENT( FUNCNAME )                                              \
    int                                                                     \
    SCOREP_LIBWRAP_FUNC_NAME( FUNCNAME ) ( long * event )                   \
    {                                                                       \
        SCOREP_IN_MEASUREMENT_INCREMENT();                                  \
        int ret;                                                            \
                                                                            \
        const int event_gen_active = SCOREP_SHMEM_IS_EVENT_GEN_ON;          \
                                                                            \
        if ( event_gen_active )                                             \
        {                                                                   \
            SCOREP_SHMEM_EVENT_GEN_OFF();                                   \
                                                                            \
            SCOREP_EnterWrappedRegion( scorep_shmem_region__ ## FUNCNAME ); \
        }                                                                   \
                                                                            \
        SCOREP_ENTER_WRAPPED_REGION();                                      \
        ret = SCOREP_LIBWRAP_FUNC_CALL( FUNCNAME, ( event ) );              \
        SCOREP_EXIT_WRAPPED_REGION();                                       \
                                                                            \
        if ( event_gen_active )                                             \
        {                                                                   \
            SCOREP_ExitRegion( scorep_shmem_region__ ## FUNCNAME );         \
                                                                            \
            SCOREP_SHMEM_EVENT_GEN_ON();                                    \
        }                                                                   \
                                                                            \
        SCOREP_IN_MEASUREMENT_DECREMENT();                                  \
        return ret;                                                         \
    }

/* *INDENT-ON* */

#if SHMEM_HAVE_DECL( SHMEM_TEST_EVENT )
TEST_EVENT( shmem_test_event )
#endif
