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
 * @brief C interface wrappers for synchronization routines
 */

#include <config.h>

#include "scorep_shmem_internal.h"
#include <SCOREP_InMeasurement.h>
#include <SCOREP_Events.h>


/**
 * Point-to-Point Synchronization Routines
 *
 * @{
 */

/* *INDENT-OFF* */

#define P2P_WAIT( FUNCNAME, ARGS )                                              \
    void                                                                        \
    SCOREP_LIBWRAP_FUNC_NAME( FUNCNAME ) ARGS                                   \
    {                                                                           \
        SCOREP_IN_MEASUREMENT_INCREMENT();                                      \
                                                                                \
        const int event_gen_active = SCOREP_SHMEM_IS_EVENT_GEN_ON;              \
                                                                                \
        if ( event_gen_active )                                                 \
        {                                                                       \
            SCOREP_SHMEM_EVENT_GEN_OFF();                                       \
                                                                                \
            SCOREP_EnterWrappedRegion( scorep_shmem_region__ ## FUNCNAME );     \
                                                                                \
            SCOREP_RmaWaitChange( scorep_shmem_world_window_handle );           \
        }                                                                       \
                                                                                \
        SCOREP_ENTER_WRAPPED_REGION();                                          \
        SCOREP_LIBWRAP_FUNC_CALL( FUNCNAME, ( addr, value ) );                  \
        SCOREP_EXIT_WRAPPED_REGION();                                           \
                                                                                \
        if ( event_gen_active )                                                 \
        {                                                                       \
            SCOREP_ExitRegion( scorep_shmem_region__ ## FUNCNAME );             \
                                                                                \
            SCOREP_SHMEM_EVENT_GEN_ON();                                        \
        }                                                                       \
                                                                                \
        SCOREP_IN_MEASUREMENT_DECREMENT();                                      \
    }

/* *INDENT-ON* */

#if SHMEM_HAVE_DECL( SHMEM_SHORT_WAIT ) && defined( SCOREP_SHMEM_SHORT_WAIT_PROTO_ARGS )
P2P_WAIT( shmem_short_wait,    SCOREP_SHMEM_SHORT_WAIT_PROTO_ARGS )
#endif

#if SHMEM_HAVE_DECL( SHMEM_INT_WAIT ) && defined( SCOREP_SHMEM_INT_WAIT_PROTO_ARGS )
P2P_WAIT( shmem_int_wait,      SCOREP_SHMEM_INT_WAIT_PROTO_ARGS )
#endif

#if SHMEM_HAVE_DECL( SHMEM_LONG_WAIT ) && defined( SCOREP_SHMEM_LONG_WAIT_PROTO_ARGS )
P2P_WAIT( shmem_long_wait,     SCOREP_SHMEM_LONG_WAIT_PROTO_ARGS )
#endif

#if SHMEM_HAVE_DECL( SHMEM_LONGLONG_WAIT ) && defined( SCOREP_SHMEM_LONGLONG_WAIT_PROTO_ARGS )
P2P_WAIT( shmem_longlong_wait, SCOREP_SHMEM_LONGLONG_WAIT_PROTO_ARGS )
#endif

#if SHMEM_HAVE_DECL( SHMEM_WAIT ) && !defined( shmem_wait ) && defined( SCOREP_SHMEM_WAIT_PROTO_ARGS )
P2P_WAIT( shmem_wait,          SCOREP_SHMEM_WAIT_PROTO_ARGS )
#endif


/* *INDENT-OFF* */

#define P2P_WAIT_UNTIL( FUNCNAME, ARGS )                                        \
    void                                                                        \
    SCOREP_LIBWRAP_FUNC_NAME( FUNCNAME ) ARGS                                   \
    {                                                                           \
        SCOREP_IN_MEASUREMENT_INCREMENT();                                      \
                                                                                \
        const int event_gen_active = SCOREP_SHMEM_IS_EVENT_GEN_ON;              \
                                                                                \
        if ( event_gen_active )                                                 \
        {                                                                       \
            SCOREP_SHMEM_EVENT_GEN_OFF();                                       \
                                                                                \
            SCOREP_EnterWrappedRegion( scorep_shmem_region__ ## FUNCNAME );     \
                                                                                \
            SCOREP_RmaWaitChange( scorep_shmem_world_window_handle );           \
        }                                                                       \
                                                                                \
        SCOREP_ENTER_WRAPPED_REGION();                                          \
        SCOREP_LIBWRAP_FUNC_CALL( FUNCNAME, ( addr, cmp, value ) );             \
        SCOREP_EXIT_WRAPPED_REGION();                                           \
                                                                                \
        if ( event_gen_active )                                                 \
        {                                                                       \
            SCOREP_ExitRegion( scorep_shmem_region__ ## FUNCNAME );             \
                                                                                \
            SCOREP_SHMEM_EVENT_GEN_ON();                                        \
        }                                                                       \
                                                                                \
        SCOREP_IN_MEASUREMENT_DECREMENT();                                      \
    }

/* *INDENT-ON* */

#if SHMEM_HAVE_DECL( SHMEM_SHORT_WAIT_UNTIL ) && defined( SCOREP_SHMEM_SHORT_WAIT_UNTIL_PROTO_ARGS )
P2P_WAIT_UNTIL( shmem_short_wait_until,    SCOREP_SHMEM_SHORT_WAIT_UNTIL_PROTO_ARGS )
#endif

#if SHMEM_HAVE_DECL( SHMEM_INT_WAIT_UNTIL ) && defined( SCOREP_SHMEM_INT_WAIT_UNTIL_PROTO_ARGS )
P2P_WAIT_UNTIL( shmem_int_wait_until,      SCOREP_SHMEM_INT_WAIT_UNTIL_PROTO_ARGS )
#endif

#if SHMEM_HAVE_DECL( SHMEM_LONG_WAIT_UNTIL ) && defined( SCOREP_SHMEM_LONG_WAIT_UNTIL_PROTO_ARGS )
P2P_WAIT_UNTIL( shmem_long_wait_until,     SCOREP_SHMEM_LONG_WAIT_UNTIL_PROTO_ARGS )
#endif

#if SHMEM_HAVE_DECL( SHMEM_LONGLONG_WAIT_UNTIL ) && defined( SCOREP_SHMEM_LONGLONG_WAIT_UNTIL_PROTO_ARGS )
P2P_WAIT_UNTIL( shmem_longlong_wait_until, SCOREP_SHMEM_LONGLONG_WAIT_UNTIL_PROTO_ARGS )
#endif

#if SHMEM_HAVE_DECL( SHMEM_WAIT_UNTIL ) && !defined( shmem_wait_until ) && defined( SCOREP_SHMEM_WAIT_UNTIL_PROTO_ARGS )
P2P_WAIT_UNTIL( shmem_wait_until,          SCOREP_SHMEM_WAIT_UNTIL_PROTO_ARGS )
#endif


/**
 * @}
 */


/**
 * Barrier Synchronization Routines
 *
 * @{
 */

/* *INDENT-OFF* */

#define SHMEM_BARRIER_ALL( FUNCNAME )                                                                   \
    void                                                                                                \
    SCOREP_LIBWRAP_FUNC_NAME( FUNCNAME ) ( void )                                                       \
    {                                                                                                   \
        SCOREP_IN_MEASUREMENT_INCREMENT();                                                              \
                                                                                                        \
        const int event_gen_active = SCOREP_SHMEM_IS_EVENT_GEN_ON;                                      \
                                                                                                        \
        if ( event_gen_active )                                                                         \
        {                                                                                               \
            SCOREP_SHMEM_EVENT_GEN_OFF();                                                               \
                                                                                                        \
            SCOREP_EnterWrappedRegion( scorep_shmem_region__ ## FUNCNAME );                             \
                                                                                                        \
            SCOREP_RmaCollectiveBegin();                                                                \
        }                                                                                               \
                                                                                                        \
        SCOREP_ENTER_WRAPPED_REGION();                                                                  \
        SCOREP_LIBWRAP_FUNC_CALL( FUNCNAME, ( ) );                                                      \
        SCOREP_EXIT_WRAPPED_REGION();                                                                   \
                                                                                                        \
        if ( event_gen_active )                                                                         \
        {                                                                                               \
            if ( SCOREP_SHMEM_IS_RMA_OP_COMPLETE_RECORD_NEEDED )                                        \
            {                                                                                           \
                SCOREP_RmaOpCompleteRemote( scorep_shmem_world_window_handle,                           \
                                            scorep_shmem_rma_op_matching_id );                          \
                scorep_shmem_rma_op_matching_id++;                                                      \
                /* Reset status flag which indicates need for a SCOREP_RmaOpCompleteRemote record */    \
                SCOREP_SHMEM_RMA_OP_COMPLETE_RECORD_OFF();                                              \
            }                                                                                           \
            SCOREP_RmaCollectiveEnd( SCOREP_COLLECTIVE_BARRIER,                                         \
                                     SCOREP_RMA_SYNC_LEVEL_PROCESS | SCOREP_RMA_SYNC_LEVEL_MEMORY,      \
                                     scorep_shmem_world_window_handle,                                  \
                                     NO_PROCESSING_ELEMENT,   /* root */                                \
                                     0,                       /* bytesSent */                           \
                                     0 );                     /* bytesReceived */                       \
                                                                                                        \
            SCOREP_ExitRegion( scorep_shmem_region__ ## FUNCNAME );                                     \
                                                                                                        \
            SCOREP_SHMEM_EVENT_GEN_ON();                                                                \
        }                                                                                               \
                                                                                                        \
        SCOREP_IN_MEASUREMENT_DECREMENT();                                                              \
    }

/* *INDENT-ON* */

#if SHMEM_HAVE_DECL( SHMEM_BARRIER_ALL )
SHMEM_BARRIER_ALL( shmem_barrier_all )
#endif


/* *INDENT-OFF* */

#define SHMEM_BARRIER( FUNCNAME )                                                                       \
    void                                                                                                \
    SCOREP_LIBWRAP_FUNC_NAME( FUNCNAME )( int    peStart,                                               \
                                          int    logPeStride,                                           \
                                          int    peSize,                                                \
                                          long * pSync )                                                \
    {                                                                                                   \
        SCOREP_IN_MEASUREMENT_INCREMENT();                                                              \
                                                                                                        \
        const int event_gen_active = SCOREP_SHMEM_IS_EVENT_GEN_ON;                                      \
                                                                                                        \
        if ( event_gen_active )                                                                         \
        {                                                                                               \
            SCOREP_SHMEM_EVENT_GEN_OFF();                                                               \
                                                                                                        \
            SCOREP_EnterWrappedRegion( scorep_shmem_region__ ## FUNCNAME );                             \
            SCOREP_RmaCollectiveBegin();                                                                \
        }                                                                                               \
                                                                                                        \
        SCOREP_ENTER_WRAPPED_REGION();                                                                  \
        SCOREP_LIBWRAP_FUNC_CALL( FUNCNAME, ( peStart, logPeStride, peSize, pSync ) );                  \
        SCOREP_EXIT_WRAPPED_REGION();                                                                   \
                                                                                                        \
        if ( event_gen_active )                                                                         \
        {                                                                                               \
            SCOREP_RmaCollectiveEnd( SCOREP_COLLECTIVE_BARRIER,                                         \
                                     SCOREP_RMA_SYNC_LEVEL_PROCESS | SCOREP_RMA_SYNC_LEVEL_MEMORY,      \
                                     scorep_shmem_get_pe_group( peStart, logPeStride, peSize ),         \
                                     NO_PROCESSING_ELEMENT, /* root */                                  \
                                     0,                     /* bytesSent */                             \
                                     0 );                   /* bytesReceived */                         \
                                                                                                        \
            SCOREP_ExitRegion( scorep_shmem_region__ ## FUNCNAME );                                     \
                                                                                                        \
            SCOREP_SHMEM_EVENT_GEN_ON();                                                                \
        }                                                                                               \
                                                                                                        \
        SCOREP_IN_MEASUREMENT_DECREMENT();                                                              \
    }

/* *INDENT-ON* */

#if SHMEM_HAVE_DECL( SHMEM_BARRIER )
SHMEM_BARRIER( shmem_barrier )
#endif


/**
 * @}
 */


/**
 * Memory Access Ordering Routines
 *
 * @{
 */

/* *INDENT-OFF* */

#define SHMEM_MEM_ORDERING( FUNCNAME )                                      \
    void                                                                    \
    SCOREP_LIBWRAP_FUNC_NAME( FUNCNAME ) ( void )                           \
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
        SCOREP_LIBWRAP_FUNC_CALL( FUNCNAME, ( ) );                          \
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

/* *INDENT-OFF* */

#if SHMEM_HAVE_DECL( SHMEM_FENCE )
SHMEM_MEM_ORDERING( shmem_fence )
#endif

#if SHMEM_HAVE_DECL( SHMEM_QUIET )
SHMEM_MEM_ORDERING( shmem_quiet )
#endif


/**
 * @}
 */
