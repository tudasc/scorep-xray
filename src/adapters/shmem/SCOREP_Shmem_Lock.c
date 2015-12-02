/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2013-2015,
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
 * @brief C interface wrappers for lock routines
 */

#include <config.h>

#include "scorep_shmem_internal.h"
#include <SCOREP_InMeasurement.h>
#include <SCOREP_Events.h>


/* *INDENT-OFF* */

#define SET_LOCK( FUNCNAME )                                                    \
    void                                                                        \
    SCOREP_LIBWRAP_FUNC_NAME( FUNCNAME ) ( long* lock )                         \
    {                                                                           \
        SCOREP_IN_MEASUREMENT_INCREMENT();                                      \
                                                                                \
        if ( SCOREP_SHMEM_IS_EVENT_GEN_ON )                                     \
        {                                                                       \
            SCOREP_SHMEM_EVENT_GEN_OFF();                                       \
                                                                                \
            SCOREP_EnterWrappedRegion( scorep_shmem_region__ ## FUNCNAME,       \
                                       ( intptr_t )CALL_SHMEM( FUNCNAME ) );    \
                                                                                \
            SCOREP_ENTER_WRAPPED_REGION();                                      \
            SCOREP_LIBWRAP_FUNC_CALL( lw, FUNCNAME, ( lock ) );                 \
            SCOREP_EXIT_WRAPPED_REGION();                                       \
                                                                                \
            SCOREP_RmaAcquireLock( scorep_shmem_interim_world_window_handle,    \
                                   NO_PROCESSING_ELEMENT,                       \
                                   ( uint64_t )lock,                            \
                                   SCOREP_LOCK_EXCLUSIVE );                     \
                                                                                \
            SCOREP_ExitRegion( scorep_shmem_region__ ## FUNCNAME );             \
                                                                                \
            SCOREP_SHMEM_EVENT_GEN_ON();                                        \
        }                                                                       \
        else                                                                    \
        {                                                                       \
            SCOREP_LIBWRAP_FUNC_CALL( lw, FUNCNAME, ( lock ) );                 \
        }                                                                       \
                                                                                \
        SCOREP_IN_MEASUREMENT_DECREMENT();                                      \
    }

/* *INDENT-ON* */

#if SHMEM_HAVE_DECL( SHMEM_SET_LOCK )
SET_LOCK( shmem_set_lock )
#endif


/* *INDENT-OFF* */

#define CLEAR_LOCK( FUNCNAME )                                                  \
    void                                                                        \
    SCOREP_LIBWRAP_FUNC_NAME( FUNCNAME ) ( long* lock )                         \
    {                                                                           \
        SCOREP_IN_MEASUREMENT_INCREMENT();                                      \
                                                                                \
        if ( SCOREP_SHMEM_IS_EVENT_GEN_ON )                                     \
        {                                                                       \
            SCOREP_SHMEM_EVENT_GEN_OFF();                                       \
                                                                                \
            SCOREP_EnterWrappedRegion( scorep_shmem_region__ ## FUNCNAME,       \
                                       ( intptr_t )CALL_SHMEM( FUNCNAME ) );    \
                                                                                \
            SCOREP_ENTER_WRAPPED_REGION();                                      \
            SCOREP_LIBWRAP_FUNC_CALL( lw, FUNCNAME, ( lock ) );                 \
            SCOREP_EXIT_WRAPPED_REGION();                                       \
                                                                                \
            SCOREP_RmaReleaseLock( scorep_shmem_interim_world_window_handle,    \
                                   NO_PROCESSING_ELEMENT,                       \
                                   ( uint64_t )lock );                          \
                                                                                \
            SCOREP_ExitRegion( scorep_shmem_region__ ## FUNCNAME );             \
                                                                                \
            SCOREP_SHMEM_EVENT_GEN_ON();                                        \
        }                                                                       \
        else                                                                    \
        {                                                                       \
            SCOREP_LIBWRAP_FUNC_CALL( lw, FUNCNAME, ( lock ) );                 \
        }                                                                       \
                                                                                \
        SCOREP_IN_MEASUREMENT_DECREMENT();                                      \
    }

/* *INDENT-ON* */

#if SHMEM_HAVE_DECL( SHMEM_CLEAR_LOCK )
CLEAR_LOCK( shmem_clear_lock )
#endif


/* *INDENT-OFF* */

#define TEST_LOCK( FUNCNAME )                                                       \
    int                                                                             \
    SCOREP_LIBWRAP_FUNC_NAME( FUNCNAME ) ( long* lock )                             \
    {                                                                               \
        SCOREP_IN_MEASUREMENT_INCREMENT();                                          \
                                                                                    \
        int ret;                                                                    \
                                                                                    \
        if ( SCOREP_SHMEM_IS_EVENT_GEN_ON )                                         \
        {                                                                           \
            SCOREP_SHMEM_EVENT_GEN_OFF();                                           \
                                                                                    \
            SCOREP_EnterWrappedRegion( scorep_shmem_region__ ## FUNCNAME,           \
                                       ( intptr_t )CALL_SHMEM( FUNCNAME ) );        \
                                                                                    \
            SCOREP_ENTER_WRAPPED_REGION();                                          \
            ret = SCOREP_LIBWRAP_FUNC_CALL( lw, FUNCNAME, ( lock ) );               \
            SCOREP_EXIT_WRAPPED_REGION();                                           \
                                                                                    \
            if ( ret == 0 )                                                         \
            {                                                                       \
                SCOREP_RmaRequestLock( scorep_shmem_interim_world_window_handle,    \
                                       NO_PROCESSING_ELEMENT,                       \
                                       ( uint64_t )lock,                            \
                                       SCOREP_LOCK_EXCLUSIVE );                     \
            }                                                                       \
            else                                                                    \
            {                                                                       \
                SCOREP_RmaTryLock( scorep_shmem_interim_world_window_handle,        \
                                   NO_PROCESSING_ELEMENT,                           \
                                   ( uint64_t )lock,                                \
                                   SCOREP_LOCK_EXCLUSIVE );                         \
            }                                                                       \
                                                                                    \
            SCOREP_ExitRegion( scorep_shmem_region__ ## FUNCNAME );                 \
                                                                                    \
            SCOREP_SHMEM_EVENT_GEN_ON();                                            \
        }                                                                           \
        else                                                                        \
        {                                                                           \
            ret = SCOREP_LIBWRAP_FUNC_CALL( lw, FUNCNAME, ( lock ) );               \
        }                                                                           \
                                                                                    \
        SCOREP_IN_MEASUREMENT_DECREMENT();                                          \
        return ret;                                                                 \
    }

/* *INDENT-ON* */

#if SHMEM_HAVE_DECL( SHMEM_TEST_LOCK )
TEST_LOCK( shmem_test_lock )
#endif
