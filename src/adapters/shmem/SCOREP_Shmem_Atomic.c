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
 * @brief C interface wrappers for atomic routines
 */

#include <config.h>

#include "scorep_shmem_internal.h"
#include <SCOREP_Events.h>
#include <SCOREP_InMeasurement.h>

/**
 * Atomic Memory fetch-and-operate Routines
 *
 * @{
 */

/* *INDENT-OFF* */

#define ATOMIC_SWAP_ROUTINE( FUNCNAME, DATATYPE )                                   \
    DATATYPE                                                                        \
    SCOREP_LIBWRAP_FUNC_NAME( FUNCNAME ) ( DATATYPE * target,                       \
                                           DATATYPE   value,                        \
                                           int        pe )                          \
    {                                                                               \
        SCOREP_IN_MEASUREMENT_INCREMENT();                                          \
        DATATYPE ret;                                                               \
                                                                                    \
        const int event_gen_active = SCOREP_SHMEM_IS_EVENT_GEN_ON;                  \
                                                                                    \
        if ( event_gen_active )                                                     \
        {                                                                           \
            SCOREP_SHMEM_EVENT_GEN_OFF();                                           \
                                                                                    \
            SCOREP_EnterWrappedRegion( scorep_shmem_region__ ## FUNCNAME );         \
                                                                                    \
            SCOREP_RmaAtomic( scorep_shmem_world_window_handle, pe,                 \
                              SCOREP_RMA_ATOMIC_TYPE_SWAP,                          \
                              sizeof( DATATYPE ), sizeof( DATATYPE ),               \
                              scorep_shmem_rma_op_matching_id );                    \
        }                                                                           \
                                                                                    \
        SCOREP_ENTER_WRAPPED_REGION();                                              \
        ret = SCOREP_LIBWRAP_FUNC_CALL( FUNCNAME, ( target, value, pe ) );          \
        SCOREP_EXIT_WRAPPED_REGION();                                               \
                                                                                    \
        if ( event_gen_active )                                                     \
        {                                                                           \
            SCOREP_RmaOpCompleteBlocking( scorep_shmem_world_window_handle,         \
                                          scorep_shmem_rma_op_matching_id );        \
                                                                                    \
            SCOREP_ExitRegion( scorep_shmem_region__ ## FUNCNAME );                 \
                                                                                    \
            SCOREP_SHMEM_EVENT_GEN_ON();                                            \
        }                                                                           \
                                                                                    \
        SCOREP_IN_MEASUREMENT_DECREMENT();                                          \
        return ret;                                                                 \
    }

/* *INDENT-ON* */

#if SHMEM_HAVE_DECL( SHMEM_SHORT_SWAP )
ATOMIC_SWAP_ROUTINE( shmem_short_swap,    short )
#endif

#if SHMEM_HAVE_DECL( SHMEM_INT_SWAP )
ATOMIC_SWAP_ROUTINE( shmem_int_swap,      int )
#endif

#if SHMEM_HAVE_DECL( SHMEM_LONG_SWAP )
ATOMIC_SWAP_ROUTINE( shmem_long_swap,     long )
#endif

#if SHMEM_HAVE_DECL( SHMEM_SWAP ) && !defined( shmem_swap )
ATOMIC_SWAP_ROUTINE( shmem_swap,          long )
#endif

#if SHMEM_HAVE_DECL( SHMEM_LONGLONG_SWAP )
ATOMIC_SWAP_ROUTINE( shmem_longlong_swap, long long )
#endif

#if SHMEM_HAVE_DECL( SHMEM_FLOAT_SWAP )
ATOMIC_SWAP_ROUTINE( shmem_float_swap,    float )
#endif

#if SHMEM_HAVE_DECL( SHMEM_DOUBLE_SWAP )
ATOMIC_SWAP_ROUTINE( shmem_double_swap,   double )
#endif


/* *INDENT-OFF* */

#define ATOMIC_CSWAP_ROUTINE( FUNCNAME, DATATYPE )                                          \
    DATATYPE                                                                                \
    SCOREP_LIBWRAP_FUNC_NAME( FUNCNAME ) ( DATATYPE * target,                               \
                                           DATATYPE   cond,                                 \
                                           DATATYPE   value,                                \
                                           int        pe )                                  \
    {                                                                                       \
        SCOREP_IN_MEASUREMENT_INCREMENT();                                                  \
        DATATYPE ret;                                                                       \
                                                                                            \
        const int event_gen_active = SCOREP_SHMEM_IS_EVENT_GEN_ON;                          \
                                                                                            \
        if ( event_gen_active )                                                             \
        {                                                                                   \
            SCOREP_SHMEM_EVENT_GEN_OFF();                                                   \
                                                                                            \
            SCOREP_EnterWrappedRegion( scorep_shmem_region__ ## FUNCNAME );                 \
                                                                                            \
            SCOREP_RmaAtomic( scorep_shmem_world_window_handle, pe,                         \
                              SCOREP_RMA_ATOMIC_TYPE_COMPARE_AND_SWAP,                      \
                              2 * sizeof( DATATYPE ), sizeof( DATATYPE ),                   \
                              scorep_shmem_rma_op_matching_id );                            \
        }                                                                                   \
                                                                                            \
        SCOREP_ENTER_WRAPPED_REGION();                                                      \
        ret = SCOREP_LIBWRAP_FUNC_CALL( FUNCNAME, ( target, cond, value, pe ) );            \
        SCOREP_EXIT_WRAPPED_REGION();                                                       \
                                                                                            \
        if ( event_gen_active )                                                             \
        {                                                                                   \
            SCOREP_RmaOpCompleteBlocking( scorep_shmem_world_window_handle,                 \
                                          scorep_shmem_rma_op_matching_id );                \
                                                                                            \
            SCOREP_ExitRegion( scorep_shmem_region__ ## FUNCNAME );                         \
                                                                                            \
            SCOREP_SHMEM_EVENT_GEN_ON();                                                    \
        }                                                                                   \
                                                                                            \
        SCOREP_IN_MEASUREMENT_DECREMENT();                                                  \
        return ret;                                                                         \
    }

/* *INDENT-ON* */

#if SHMEM_HAVE_DECL( SHMEM_SHORT_CSWAP )
ATOMIC_CSWAP_ROUTINE( shmem_short_cswap,    short )
#endif

#if SHMEM_HAVE_DECL( SHMEM_INT_CSWAP )
ATOMIC_CSWAP_ROUTINE( shmem_int_cswap,      int )
#endif

#if SHMEM_HAVE_DECL( SHMEM_LONG_CSWAP )
ATOMIC_CSWAP_ROUTINE( shmem_long_cswap,     long )
#endif

#if SHMEM_HAVE_DECL( SHMEM_LONGLONG_CSWAP )
ATOMIC_CSWAP_ROUTINE( shmem_longlong_cswap, long long )
#endif


/* *INDENT-OFF* */

#define ATOMIC_FETCH_ADD_ROUTINE( FUNCNAME, DATATYPE )                              \
    DATATYPE                                                                        \
    SCOREP_LIBWRAP_FUNC_NAME( FUNCNAME ) ( DATATYPE * target,                       \
                                           DATATYPE   value,                        \
                                           int        pe )                          \
    {                                                                               \
        SCOREP_IN_MEASUREMENT_INCREMENT();                                          \
        DATATYPE ret;                                                               \
                                                                                    \
        const int event_gen_active = SCOREP_SHMEM_IS_EVENT_GEN_ON;                  \
                                                                                    \
        if ( event_gen_active )                                                     \
        {                                                                           \
            SCOREP_SHMEM_EVENT_GEN_OFF();                                           \
                                                                                    \
            SCOREP_EnterWrappedRegion( scorep_shmem_region__ ## FUNCNAME );         \
                                                                                    \
            SCOREP_RmaAtomic( scorep_shmem_world_window_handle, pe,                 \
                              SCOREP_RMA_ATOMIC_TYPE_FETCH_AND_ADD,                 \
                              sizeof( DATATYPE ), sizeof( DATATYPE ),               \
                              scorep_shmem_rma_op_matching_id );                    \
        }                                                                           \
                                                                                    \
        SCOREP_ENTER_WRAPPED_REGION();                                              \
        ret = SCOREP_LIBWRAP_FUNC_CALL( FUNCNAME, ( target, value, pe ) );          \
        SCOREP_EXIT_WRAPPED_REGION();                                               \
                                                                                    \
        if ( event_gen_active )                                                     \
        {                                                                           \
            SCOREP_RmaOpCompleteBlocking( scorep_shmem_world_window_handle,         \
                                          scorep_shmem_rma_op_matching_id );        \
                                                                                    \
            SCOREP_ExitRegion( scorep_shmem_region__ ## FUNCNAME );                 \
                                                                                    \
            SCOREP_SHMEM_EVENT_GEN_ON();                                            \
        }                                                                           \
                                                                                    \
        SCOREP_IN_MEASUREMENT_DECREMENT();                                          \
        return ret;                                                                 \
    }                                                                               \

/* *INDENT-ON* */

#if SHMEM_HAVE_DECL( SHMEM_SHORT_FADD )
ATOMIC_FETCH_ADD_ROUTINE( shmem_short_fadd,    short )
#endif

#if SHMEM_HAVE_DECL( SHMEM_INT_FADD )
ATOMIC_FETCH_ADD_ROUTINE( shmem_int_fadd,      int )
#endif

#if SHMEM_HAVE_DECL( SHMEM_LONG_FADD )
ATOMIC_FETCH_ADD_ROUTINE( shmem_long_fadd,     long )
#endif

#if SHMEM_HAVE_DECL( SHMEM_LONGLONG_FADD )
ATOMIC_FETCH_ADD_ROUTINE( shmem_longlong_fadd, long long )
#endif


/* *INDENT-OFF* */

#define ATOMIC_FETCH_INCREMENT_ROUTINE( FUNCNAME, DATATYPE )                            \
    DATATYPE                                                                            \
    SCOREP_LIBWRAP_FUNC_NAME( FUNCNAME ) ( DATATYPE * target,                           \
                                           int        pe )                              \
    {                                                                                   \
        SCOREP_IN_MEASUREMENT_INCREMENT();                                              \
        DATATYPE ret;                                                                   \
                                                                                        \
        const int event_gen_active = SCOREP_SHMEM_IS_EVENT_GEN_ON;                      \
                                                                                        \
        if ( event_gen_active )                                                         \
        {                                                                               \
            SCOREP_SHMEM_EVENT_GEN_OFF();                                               \
                                                                                        \
            SCOREP_EnterWrappedRegion( scorep_shmem_region__ ## FUNCNAME );             \
                                                                                        \
            SCOREP_RmaAtomic( scorep_shmem_world_window_handle, pe,                     \
                              SCOREP_RMA_ATOMIC_TYPE_FETCH_AND_INCREMENT,               \
                              0, sizeof( DATATYPE ),                                    \
                              scorep_shmem_rma_op_matching_id );                        \
        }                                                                               \
                                                                                        \
        SCOREP_ENTER_WRAPPED_REGION();                                                  \
        ret = SCOREP_LIBWRAP_FUNC_CALL( FUNCNAME, ( target, pe ) );                     \
        SCOREP_EXIT_WRAPPED_REGION();                                                   \
                                                                                        \
        if ( event_gen_active )                                                         \
        {                                                                               \
            SCOREP_RmaOpCompleteBlocking( scorep_shmem_world_window_handle,             \
                                          scorep_shmem_rma_op_matching_id );            \
                                                                                        \
            SCOREP_ExitRegion( scorep_shmem_region__ ## FUNCNAME );                     \
                                                                                        \
            SCOREP_SHMEM_EVENT_GEN_ON();                                                \
        }                                                                               \
                                                                                        \
        SCOREP_IN_MEASUREMENT_DECREMENT();                                              \
        return ret;                                                                     \
    }                                                                                   \

/* *INDENT-ON* */

#if SHMEM_HAVE_DECL( SHMEM_SHORT_FINC )
ATOMIC_FETCH_INCREMENT_ROUTINE( shmem_short_finc,    short )
#endif

#if SHMEM_HAVE_DECL( SHMEM_INT_FINC )
ATOMIC_FETCH_INCREMENT_ROUTINE( shmem_int_finc,      int )
#endif

#if SHMEM_HAVE_DECL( SHMEM_LONG_FINC )
ATOMIC_FETCH_INCREMENT_ROUTINE( shmem_long_finc,     long )
#endif

#if SHMEM_HAVE_DECL( SHMEM_LONGLONG_FINC )
ATOMIC_FETCH_INCREMENT_ROUTINE( shmem_longlong_finc, long long )
#endif

/**
 * @}
 */




/**
 * Atomic Memory fetch-and-operate Routines
 *
 * Atomic Memory Routines
 *
 * @{
 */

/* *INDENT-OFF* */

#define ATOMIC_ADD_ROUTINE( FUNCNAME, DATATYPE )                                    \
    void                                                                            \
    SCOREP_LIBWRAP_FUNC_NAME( FUNCNAME ) ( DATATYPE * target,                       \
                                           DATATYPE   value,                        \
                                           int        pe )                          \
    {                                                                               \
        SCOREP_IN_MEASUREMENT_INCREMENT();                                          \
                                                                                    \
        const int event_gen_active = SCOREP_SHMEM_IS_EVENT_GEN_ON;                  \
                                                                                    \
        if ( event_gen_active )                                                     \
        {                                                                           \
            SCOREP_SHMEM_EVENT_GEN_OFF();                                           \
                                                                                    \
            SCOREP_EnterWrappedRegion( scorep_shmem_region__ ## FUNCNAME );         \
                                                                                    \
            SCOREP_RmaAtomic( scorep_shmem_world_window_handle, pe,                 \
                              SCOREP_RMA_ATOMIC_TYPE_ACCUMULATE,                    \
                              sizeof( DATATYPE ), 0,                                \
                              scorep_shmem_rma_op_matching_id );                    \
        }                                                                           \
                                                                                    \
        SCOREP_ENTER_WRAPPED_REGION();                                              \
        SCOREP_LIBWRAP_FUNC_CALL( FUNCNAME, ( target, value, pe ) );                \
        SCOREP_EXIT_WRAPPED_REGION();                                               \
                                                                                    \
        if ( event_gen_active )                                                     \
        {                                                                           \
            SCOREP_RmaOpCompleteBlocking( scorep_shmem_world_window_handle,         \
                                          scorep_shmem_rma_op_matching_id );        \
                                                                                    \
            SCOREP_ExitRegion( scorep_shmem_region__ ## FUNCNAME );                 \
                                                                                    \
            SCOREP_SHMEM_EVENT_GEN_ON();                                            \
        }                                                                           \
                                                                                    \
        SCOREP_IN_MEASUREMENT_DECREMENT();                                          \
    }

/* *INDENT-ON* */

#if SHMEM_HAVE_DECL( SHMEM_SHORT_ADD )
ATOMIC_ADD_ROUTINE( shmem_short_add,    short )
#endif

#if SHMEM_HAVE_DECL( SHMEM_INT_ADD )
ATOMIC_ADD_ROUTINE( shmem_int_add,      int )
#endif

#if SHMEM_HAVE_DECL( SHMEM_LONG_ADD )
ATOMIC_ADD_ROUTINE( shmem_long_add,     long )
#endif

#if SHMEM_HAVE_DECL( SHMEM_LONGLONG_ADD )
ATOMIC_ADD_ROUTINE( shmem_longlong_add, long long )
#endif


/* *INDENT-OFF* */

/* *INDENT-OFF* */

#define ATOMIC_INCREMENT_ROUTINE( FUNCNAME, DATATYPE )                              \
    void                                                                            \
    SCOREP_LIBWRAP_FUNC_NAME( FUNCNAME ) ( DATATYPE * target,                       \
                                           int        pe )                          \
    {                                                                               \
        SCOREP_IN_MEASUREMENT_INCREMENT();                                          \
                                                                                    \
        const int event_gen_active = SCOREP_SHMEM_IS_EVENT_GEN_ON;                  \
                                                                                    \
        if ( event_gen_active )                                                     \
        {                                                                           \
            SCOREP_SHMEM_EVENT_GEN_OFF();                                           \
                                                                                    \
            SCOREP_EnterWrappedRegion( scorep_shmem_region__ ## FUNCNAME );         \
                                                                                    \
            SCOREP_RmaAtomic( scorep_shmem_world_window_handle, pe,                 \
                              SCOREP_RMA_ATOMIC_TYPE_INCREMENT,                     \
                              0, 0,                                                 \
                              scorep_shmem_rma_op_matching_id );                    \
        }                                                                           \
                                                                                    \
        SCOREP_ENTER_WRAPPED_REGION();                                              \
        SCOREP_LIBWRAP_FUNC_CALL( FUNCNAME, ( target, pe ) );                       \
        SCOREP_EXIT_WRAPPED_REGION();                                               \
                                                                                    \
        if ( event_gen_active )                                                     \
        {                                                                           \
            SCOREP_RmaOpCompleteBlocking( scorep_shmem_world_window_handle,         \
                                          scorep_shmem_rma_op_matching_id );        \
                                                                                    \
            SCOREP_ExitRegion( scorep_shmem_region__ ## FUNCNAME );                 \
                                                                                    \
            SCOREP_SHMEM_EVENT_GEN_ON();                                            \
        }                                                                           \
                                                                                    \
        SCOREP_IN_MEASUREMENT_DECREMENT();                                          \
    }

/* *INDENT-ON* */

#if SHMEM_HAVE_DECL( SHMEM_SHORT_INC )
ATOMIC_INCREMENT_ROUTINE( shmem_short_inc,    short )
#endif

#if SHMEM_HAVE_DECL( SHMEM_INT_INC )
ATOMIC_INCREMENT_ROUTINE( shmem_int_inc,      int )
#endif

#if SHMEM_HAVE_DECL( SHMEM_LONG_INC )
ATOMIC_INCREMENT_ROUTINE( shmem_long_inc,     long )
#endif

#if SHMEM_HAVE_DECL( SHMEM_LONGLONG_INC )
ATOMIC_INCREMENT_ROUTINE( shmem_longlong_inc, long long )
#endif

/**
 * @}
 */
