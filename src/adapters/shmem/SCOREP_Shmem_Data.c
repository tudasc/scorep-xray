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
 * @brief C interface wrappers for data transfer routines
 */

#include <config.h>

#include "scorep_shmem_internal.h"
#include <SCOREP_InMeasurement.h>
#include <SCOREP_Events.h>


/* *INDENT-OFF* */

#define ELEMENTAL_PUT_ROUTINE( FUNCNAME, DATATYPE )                                 \
    void                                                                            \
    SCOREP_LIBWRAP_FUNC_NAME( FUNCNAME ) ( DATATYPE * addr,                         \
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
            SCOREP_RmaPut( scorep_shmem_world_window_handle,                        \
                           pe, sizeof( DATATYPE ),                                  \
                           scorep_shmem_rma_op_matching_id );                       \
            SCOREP_SHMEM_RMA_OP_COMPLETE_RECORD_ON();                               \
        }                                                                           \
                                                                                    \
        SCOREP_ENTER_WRAPPED_REGION();                                              \
        SCOREP_LIBWRAP_FUNC_CALL( FUNCNAME, ( addr, value, pe ) );                  \
        SCOREP_EXIT_WRAPPED_REGION();                                               \
                                                                                    \
        if ( event_gen_active )                                                     \
        {                                                                           \
            SCOREP_RmaOpCompleteBlocking( scorep_shmem_world_window_handle,         \
                                          scorep_shmem_rma_op_matching_id );        \
            SCOREP_ExitRegion( scorep_shmem_region__ ## FUNCNAME );                 \
                                                                                    \
            SCOREP_SHMEM_EVENT_GEN_ON();                                            \
        }                                                                           \
                                                                                    \
        SCOREP_IN_MEASUREMENT_DECREMENT();                                          \
    }                                                                               \

/* *INDENT-ON* */

/**
 * Elemental Put Routines
 *
 * @{
 */

#if SHMEM_HAVE_DECL( SHMEM_CHAR_P )
ELEMENTAL_PUT_ROUTINE( shmem_char_p,       char )
#endif

#if SHMEM_HAVE_DECL( SHMEM_SHORT_P )
ELEMENTAL_PUT_ROUTINE( shmem_short_p,      short )
#endif

#if SHMEM_HAVE_DECL( SHMEM_INT_P )
ELEMENTAL_PUT_ROUTINE( shmem_int_p,        int )
#endif

#if SHMEM_HAVE_DECL( SHMEM_LONG_P )
ELEMENTAL_PUT_ROUTINE( shmem_long_p,       long )
#endif

#if SHMEM_HAVE_DECL( SHMEM_FLOAT_P )
ELEMENTAL_PUT_ROUTINE( shmem_float_p,      float )
#endif

#if SHMEM_HAVE_DECL( SHMEM_DOUBLE_P )
ELEMENTAL_PUT_ROUTINE( shmem_double_p,     double )
#endif

#if SHMEM_HAVE_DECL( SHMEM_LONGLONG_P )
ELEMENTAL_PUT_ROUTINE( shmem_longlong_p,   long long )
#endif

#if SHMEM_HAVE_DECL( SHMEM_LONGDOUBLE_P )
ELEMENTAL_PUT_ROUTINE( shmem_longdouble_p, long double )
#endif

/**
 * @}
 */

/* *INDENT-OFF* */

#define BLOCK_DATA_PUT_ROUTINE( FUNCNAME, DATATYPE, NBYTES )                            \
    void                                                                                \
    SCOREP_LIBWRAP_FUNC_NAME( FUNCNAME ) ( DATATYPE *       target,                     \
                                           const DATATYPE * source,                     \
                                           size_t           nElems,                     \
                                           int              pe )                        \
    {                                                                                   \
        SCOREP_IN_MEASUREMENT_INCREMENT();                                              \
                                                                                        \
        const int event_gen_active = SCOREP_SHMEM_IS_EVENT_GEN_ON;                      \
                                                                                        \
        if ( event_gen_active )                                                         \
        {                                                                               \
            SCOREP_SHMEM_EVENT_GEN_OFF();                                               \
                                                                                        \
            SCOREP_EnterWrappedRegion( scorep_shmem_region__ ## FUNCNAME );             \
            SCOREP_RmaPut( scorep_shmem_world_window_handle,                            \
                           pe, nElems * NBYTES,                                         \
                           scorep_shmem_rma_op_matching_id );                           \
            SCOREP_SHMEM_RMA_OP_COMPLETE_RECORD_ON();                                   \
        }                                                                               \
                                                                                        \
        SCOREP_ENTER_WRAPPED_REGION();                                                  \
        SCOREP_LIBWRAP_FUNC_CALL( FUNCNAME, ( target, source, nElems, pe ) );           \
        SCOREP_EXIT_WRAPPED_REGION();                                                   \
                                                                                        \
        if ( event_gen_active )                                                         \
        {                                                                               \
            SCOREP_RmaOpCompleteBlocking( scorep_shmem_world_window_handle,             \
                                          scorep_shmem_rma_op_matching_id );            \
            SCOREP_ExitRegion( scorep_shmem_region__ ## FUNCNAME );                     \
                                                                                        \
            SCOREP_SHMEM_EVENT_GEN_ON();                                                \
        }                                                                               \
                                                                                        \
        SCOREP_IN_MEASUREMENT_DECREMENT();                                              \
    }

/* *INDENT-ON* */

/**
 * Block Data Put Routines
 *
 * @{
 */

#if SHMEM_HAVE_DECL( SHMEM_CHAR_PUT )
BLOCK_DATA_PUT_ROUTINE( shmem_char_put,       char,        sizeof( char ) )
#endif

#if SHMEM_HAVE_DECL( SHMEM_SHORT_PUT )
BLOCK_DATA_PUT_ROUTINE( shmem_short_put,      short,       sizeof( short ) )
#endif

#if SHMEM_HAVE_DECL( SHMEM_INT_PUT )
BLOCK_DATA_PUT_ROUTINE( shmem_int_put,        int,         sizeof( int ) )
#endif

#if SHMEM_HAVE_DECL( SHMEM_LONG_PUT )
BLOCK_DATA_PUT_ROUTINE( shmem_long_put,       long,        sizeof( long ) )
#endif

#if SHMEM_HAVE_DECL( SHMEM_FLOAT_PUT )
BLOCK_DATA_PUT_ROUTINE( shmem_float_put,      float,       sizeof( float ) )
#endif

#if SHMEM_HAVE_DECL( SHMEM_DOUBLE_PUT )
BLOCK_DATA_PUT_ROUTINE( shmem_double_put,     double,      sizeof( double ) )
#endif

#if SHMEM_HAVE_DECL( SHMEM_LONGLONG_PUT )
BLOCK_DATA_PUT_ROUTINE( shmem_longlong_put,   long long,   sizeof( long long ) )
#endif

#if SHMEM_HAVE_DECL( SHMEM_LONGDOUBLE_PUT ) && !defined( shmem_longdouble_put )
#if HAVE( SHMEM_LONGDOUBLE_PUT_COMPLIANT )
BLOCK_DATA_PUT_ROUTINE( shmem_longdouble_put, long double, sizeof( long double ) )
#elif HAVE( SHMEM_LONGDOUBLE_PUT_CRAY_VARIANT )
BLOCK_DATA_PUT_ROUTINE( shmem_longdouble_put, void,        sizeof( long double ) )
#endif
#endif

#if SHMEM_HAVE_DECL( SHMEM_PUT16 )
BLOCK_DATA_PUT_ROUTINE( shmem_put16,          void,        2 )
#endif

#if SHMEM_HAVE_DECL( SHMEM_PUT32 )
BLOCK_DATA_PUT_ROUTINE( shmem_put32,          void,        4 )
#endif

#if SHMEM_HAVE_DECL( SHMEM_PUT64 )
BLOCK_DATA_PUT_ROUTINE( shmem_put64,          void,        8 )
#endif

#if SHMEM_HAVE_DECL( SHMEM_PUT128 )
BLOCK_DATA_PUT_ROUTINE( shmem_put128,         void,        16 )
#endif

#if SHMEM_HAVE_DECL( SHMEM_PUTMEM )
BLOCK_DATA_PUT_ROUTINE( shmem_putmem,         void,        1 )
#endif


/**
 * @}
 */

/* *INDENT-OFF* */

#define STRIDED_PUT_ROUTINE( FUNCNAME, DATATYPE, NBYTES )                                       \
    void                                                                                        \
    SCOREP_LIBWRAP_FUNC_NAME( FUNCNAME ) ( DATATYPE *       target,                             \
                                           const DATATYPE * source,                             \
                                           ptrdiff_t        tst,                                \
                                           ptrdiff_t        sst,                                \
                                           size_t           nElems,                             \
                                           int              pe )                                \
    {                                                                                           \
        SCOREP_IN_MEASUREMENT_INCREMENT();                                                      \
                                                                                                \
        const int event_gen_active = SCOREP_SHMEM_IS_EVENT_GEN_ON;                              \
                                                                                                \
        if ( event_gen_active )                                                                 \
        {                                                                                       \
            SCOREP_SHMEM_EVENT_GEN_OFF();                                                       \
                                                                                                \
            SCOREP_EnterWrappedRegion( scorep_shmem_region__ ## FUNCNAME );                     \
            SCOREP_RmaPut( scorep_shmem_world_window_handle,                                    \
                           pe, nElems * NBYTES,                                                 \
                           scorep_shmem_rma_op_matching_id );                                   \
            SCOREP_SHMEM_RMA_OP_COMPLETE_RECORD_ON();                                           \
        }                                                                                       \
                                                                                                \
        SCOREP_ENTER_WRAPPED_REGION();                                                          \
        SCOREP_LIBWRAP_FUNC_CALL( FUNCNAME, ( target, source, tst, sst, nElems, pe ) );         \
        SCOREP_EXIT_WRAPPED_REGION();                                                           \
                                                                                                \
        if ( event_gen_active )                                                                 \
        {                                                                                       \
            SCOREP_RmaOpCompleteBlocking( scorep_shmem_world_window_handle,                     \
                                          scorep_shmem_rma_op_matching_id );                    \
            SCOREP_ExitRegion( scorep_shmem_region__ ## FUNCNAME );                             \
                                                                                                \
            SCOREP_SHMEM_EVENT_GEN_ON();                                                        \
        }                                                                                       \
                                                                                                \
        SCOREP_IN_MEASUREMENT_DECREMENT();                                                      \
    }

/* *INDENT-ON* */

/**
 * Strided Put Routines
 *
 * @{
 */

#if SHMEM_HAVE_DECL( SHMEM_SHORT_IPUT )
STRIDED_PUT_ROUTINE( shmem_short_iput,      short,       sizeof( short ) )
#endif

#if SHMEM_HAVE_DECL( SHMEM_INT_IPUT )
STRIDED_PUT_ROUTINE( shmem_int_iput,        int,         sizeof( int ) )
#endif

#if SHMEM_HAVE_DECL( SHMEM_FLOAT_IPUT )
STRIDED_PUT_ROUTINE( shmem_float_iput,      float,       sizeof( float ) )
#endif

#if SHMEM_HAVE_DECL( SHMEM_LONG_IPUT )
STRIDED_PUT_ROUTINE( shmem_long_iput,       long,        sizeof( long ) )
#endif

#if SHMEM_HAVE_DECL( SHMEM_DOUBLE_IPUT )
STRIDED_PUT_ROUTINE( shmem_double_iput,     double,      sizeof( double ) )
#endif

#if SHMEM_HAVE_DECL( SHMEM_LONGLONG_IPUT )
STRIDED_PUT_ROUTINE( shmem_longlong_iput,   long long,   sizeof( long long ) )
#endif

#if SHMEM_HAVE_DECL( SHMEM_LONGDOUBLE_IPUT ) && !defined( shmem_longdouble_iput )
#if HAVE( SHMEM_LONGDOUBLE_IPUT_COMPLIANT )
STRIDED_PUT_ROUTINE( shmem_longdouble_iput, long double, sizeof( long double ) )
#elif HAVE( SHMEM_LONGDOUBLE_PUT_CRAY_VARIANT )
STRIDED_PUT_ROUTINE( shmem_longdouble_iput, void,        sizeof( long double ) )
#endif
#endif

#if SHMEM_HAVE_DECL( SHMEM_IPUT16 )
STRIDED_PUT_ROUTINE( shmem_iput16,          void,        2 )
#endif

#if SHMEM_HAVE_DECL( SHMEM_IPUT32 )
STRIDED_PUT_ROUTINE( shmem_iput32,          void,        4 )
#endif

#if SHMEM_HAVE_DECL( SHMEM_IPUT64 )
STRIDED_PUT_ROUTINE( shmem_iput64,          void,        8 )
#endif

#if SHMEM_HAVE_DECL( SHMEM_IPUT128 )
STRIDED_PUT_ROUTINE( shmem_iput128,         void,        16 )
#endif


/**
 * @}
 */

/* *INDENT-OFF* */

#define ELEMENTAL_GET_ROUTINE( FUNCNAME, DATATYPE, DATATYPE_IN )                    \
    DATATYPE                                                                        \
    SCOREP_LIBWRAP_FUNC_NAME( FUNCNAME ) ( DATATYPE_IN * addr,                      \
                                           int           pe )                       \
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
            SCOREP_RmaGet( scorep_shmem_world_window_handle,                        \
                           pe, sizeof( DATATYPE ),                                  \
                           scorep_shmem_rma_op_matching_id );                       \
            SCOREP_SHMEM_RMA_OP_COMPLETE_RECORD_ON();                               \
        }                                                                           \
                                                                                    \
        SCOREP_ENTER_WRAPPED_REGION();                                              \
        ret = SCOREP_LIBWRAP_FUNC_CALL( FUNCNAME, ( addr, pe ) );                   \
        SCOREP_EXIT_WRAPPED_REGION();                                               \
                                                                                    \
        if ( event_gen_active )                                                     \
        {                                                                           \
            SCOREP_RmaOpCompleteBlocking( scorep_shmem_world_window_handle,         \
                                          scorep_shmem_rma_op_matching_id );        \
            SCOREP_ExitRegion( scorep_shmem_region__ ## FUNCNAME );                 \
                                                                                    \
            SCOREP_SHMEM_EVENT_GEN_ON();                                            \
        }                                                                           \
                                                                                    \
        SCOREP_IN_MEASUREMENT_DECREMENT();                                          \
        return ret;                                                                 \
    }

/* *INDENT-ON* */

/**
 * Elemental Get Routines
 *
 * @{
 */

#if SHMEM_HAVE_DECL( SHMEM_CHAR_G )
#if HAVE( SHMEM_CHAR_G_COMPLIANT )
ELEMENTAL_GET_ROUTINE( shmem_char_g,       char, char )
#elif HAVE( SHMEM_CHAR_G_CONST_VARIANT )
ELEMENTAL_GET_ROUTINE( shmem_char_g,       char, const char )
#endif
#endif

#if SHMEM_HAVE_DECL( SHMEM_SHORT_G )
#if HAVE( SHMEM_SHORT_G_COMPLIANT )
ELEMENTAL_GET_ROUTINE( shmem_short_g,      short, short )
#elif HAVE( SHMEM_SHORT_G_CONST_VARIANT )
ELEMENTAL_GET_ROUTINE( shmem_short_g,      short, const short )
#endif
#endif

#if SHMEM_HAVE_DECL( SHMEM_INT_G )
#if HAVE( SHMEM_INT_G_COMPLIANT )
ELEMENTAL_GET_ROUTINE( shmem_int_g,        int, int )
#elif HAVE( SHMEM_INT_G_CONST_VARIANT )
ELEMENTAL_GET_ROUTINE( shmem_int_g,        int, const int )
#endif
#endif

#if SHMEM_HAVE_DECL( SHMEM_LONG_G )
#if HAVE( SHMEM_LONG_G_COMPLIANT )
ELEMENTAL_GET_ROUTINE( shmem_long_g,       long, long )
#elif HAVE( SHMEM_LONG_G_CONST_VARIANT )
ELEMENTAL_GET_ROUTINE( shmem_long_g,       long, const long )
#endif
#endif

#if SHMEM_HAVE_DECL( SHMEM_FLOAT_G )
#if HAVE( SHMEM_FLOAT_G_COMPLIANT )
ELEMENTAL_GET_ROUTINE( shmem_float_g,       float, float )
#elif HAVE( SHMEM_FLOAT_G_CONST_VARIANT )
ELEMENTAL_GET_ROUTINE( shmem_float_g,       float, const float )
#endif
#endif

#if SHMEM_HAVE_DECL( SHMEM_DOUBLE_G )
#if HAVE( SHMEM_DOUBLE_G_COMPLIANT )
ELEMENTAL_GET_ROUTINE( shmem_double_g,       double, double )
#elif HAVE( SHMEM_DOUBLE_G_CONST_VARIANT )
ELEMENTAL_GET_ROUTINE( shmem_double_g,       double, const double )
#endif
#endif

#if SHMEM_HAVE_DECL( SHMEM_LONGLONG_G )
#if HAVE( SHMEM_LONGLONG_G_COMPLIANT )
ELEMENTAL_GET_ROUTINE( shmem_longlong_g,   long long, long long )
#elif HAVE( SHMEM_LONGLONG_G_CONST_VARIANT )
ELEMENTAL_GET_ROUTINE( shmem_longlong_g,   long long, const long long )
#endif
#endif

#if SHMEM_HAVE_DECL( SHMEM_LONGDOUBLE_G )
#if HAVE( SHMEM_LONGDOUBLE_G_COMPLIANT )
ELEMENTAL_GET_ROUTINE( shmem_longdouble_g, long double, long double )
#elif HAVE( SHMEM_LONGDOUBLE_G_CONST_VARIANT )
ELEMENTAL_GET_ROUTINE( shmem_longdouble_g, long double, const long double )
#endif
#endif


/**
 * @}
 */

/* *INDENT-OFF* */

#define BLOCK_DATA_GET_ROUTINE( FUNCNAME, DATATYPE, NBYTES )                            \
    void                                                                                \
    SCOREP_LIBWRAP_FUNC_NAME( FUNCNAME ) ( DATATYPE *       target,                     \
                                           const DATATYPE * source,                     \
                                           size_t           nElems,                     \
                                           int              pe )                        \
    {                                                                                   \
        SCOREP_IN_MEASUREMENT_INCREMENT();                                              \
                                                                                        \
        const int event_gen_active = SCOREP_SHMEM_IS_EVENT_GEN_ON;                      \
                                                                                        \
        if ( event_gen_active )                                                         \
        {                                                                               \
            SCOREP_SHMEM_EVENT_GEN_OFF();                                               \
                                                                                        \
            SCOREP_EnterWrappedRegion( scorep_shmem_region__ ## FUNCNAME );             \
            SCOREP_RmaGet( scorep_shmem_world_window_handle,                            \
                           pe, nElems * NBYTES,                                         \
                           scorep_shmem_rma_op_matching_id );                           \
            SCOREP_SHMEM_RMA_OP_COMPLETE_RECORD_ON();                                   \
        }                                                                               \
                                                                                        \
        SCOREP_ENTER_WRAPPED_REGION();                                                  \
        SCOREP_LIBWRAP_FUNC_CALL( FUNCNAME, ( target, source, nElems, pe ) );           \
        SCOREP_EXIT_WRAPPED_REGION();                                                   \
                                                                                        \
        if ( event_gen_active )                                                         \
        {                                                                               \
            SCOREP_RmaOpCompleteBlocking( scorep_shmem_world_window_handle,             \
                                          scorep_shmem_rma_op_matching_id );            \
            SCOREP_ExitRegion( scorep_shmem_region__ ## FUNCNAME );                     \
                                                                                        \
            SCOREP_SHMEM_EVENT_GEN_ON();                                                \
        }                                                                               \
                                                                                        \
        SCOREP_IN_MEASUREMENT_DECREMENT();                                              \
    }

/* *INDENT-ON* */

/**
 * Block Data Get Routines
 *
 * @{
 */

#if SHMEM_HAVE_DECL( SHMEM_CHAR_GET )
BLOCK_DATA_GET_ROUTINE( shmem_char_get,       char,        sizeof( char ) )
#endif

#if SHMEM_HAVE_DECL( SHMEM_SHORT_G )
BLOCK_DATA_GET_ROUTINE( shmem_short_get,      short,       sizeof( short ) )
#endif

#if SHMEM_HAVE_DECL( SHMEM_INT_GET )
BLOCK_DATA_GET_ROUTINE( shmem_int_get,        int,         sizeof( int ) )
#endif

#if SHMEM_HAVE_DECL( SHMEM_LONG_GET )
BLOCK_DATA_GET_ROUTINE( shmem_long_get,       long,        sizeof( long ) )
#endif

#if SHMEM_HAVE_DECL( SHMEM_FLOAT_GET )
BLOCK_DATA_GET_ROUTINE( shmem_float_get,      float,       sizeof( float ) )
#endif

#if SHMEM_HAVE_DECL( SHMEM_DOUBLE_GET )
BLOCK_DATA_GET_ROUTINE( shmem_double_get,     double,      sizeof( double ) )
#endif

#if SHMEM_HAVE_DECL( SHMEM_LONGLONG_GET )
BLOCK_DATA_GET_ROUTINE( shmem_longlong_get,   long long,   sizeof( long long ) )
#endif

#if SHMEM_HAVE_DECL( SHMEM_LONGDOUBLE_GET ) && !defined( shmem_longdouble_get )
#if HAVE( SHMEM_LONGDOUBLE_GET_COMPLIANT )
BLOCK_DATA_GET_ROUTINE( shmem_longdouble_get, long double, sizeof( long double ) )
#elif HAVE( SHMEM_LONGDOUBLE_GET_CRAY_VARIANT )
BLOCK_DATA_GET_ROUTINE( shmem_longdouble_get, void,        sizeof( long double ) )
#endif
#endif

#if SHMEM_HAVE_DECL( SHMEM_GET16 )
BLOCK_DATA_GET_ROUTINE( shmem_get16,          void,        2 )
#endif

#if SHMEM_HAVE_DECL( SHMEM_GET32 )
BLOCK_DATA_GET_ROUTINE( shmem_get32,          void,        4 )
#endif

#if SHMEM_HAVE_DECL( SHMEM_GET64 )
BLOCK_DATA_GET_ROUTINE( shmem_get64,          void,        8 )
#endif

#if SHMEM_HAVE_DECL( SHMEM_GET128 )
BLOCK_DATA_GET_ROUTINE( shmem_get128,         void,        16 )
#endif

#if SHMEM_HAVE_DECL( SHMEM_GETMEM )
BLOCK_DATA_GET_ROUTINE( shmem_getmem,         void,        1 )
#endif


/**
 * @}
 */

/* *INDENT-OFF* */

#define STRIDED_GET_ROUTINE( FUNCNAME, DATATYPE, NBYTES )                                       \
    void                                                                                        \
    SCOREP_LIBWRAP_FUNC_NAME( FUNCNAME ) ( DATATYPE *       target,                             \
                                           const DATATYPE * source,                             \
                                           ptrdiff_t        tst,                                \
                                           ptrdiff_t        sst,                                \
                                           size_t           nElems,                             \
                                           int              pe )                                \
    {                                                                                           \
        SCOREP_IN_MEASUREMENT_INCREMENT();                                                      \
                                                                                                \
        const int event_gen_active = SCOREP_SHMEM_IS_EVENT_GEN_ON;                              \
                                                                                                \
        if ( event_gen_active )                                                                 \
        {                                                                                       \
            SCOREP_SHMEM_EVENT_GEN_OFF();                                                       \
                                                                                                \
            SCOREP_EnterWrappedRegion( scorep_shmem_region__ ## FUNCNAME );                     \
            SCOREP_RmaGet( scorep_shmem_world_window_handle,                                    \
                           pe, nElems * NBYTES,                                                 \
                           scorep_shmem_rma_op_matching_id );                                   \
            SCOREP_SHMEM_RMA_OP_COMPLETE_RECORD_ON();                                           \
        }                                                                                       \
                                                                                                \
        SCOREP_ENTER_WRAPPED_REGION();                                                          \
        SCOREP_LIBWRAP_FUNC_CALL( FUNCNAME, ( target, source, tst, sst, nElems, pe ) );         \
        SCOREP_EXIT_WRAPPED_REGION();                                                           \
                                                                                                \
        if ( event_gen_active )                                                                 \
        {                                                                                       \
            SCOREP_RmaOpCompleteBlocking( scorep_shmem_world_window_handle,                     \
                                          scorep_shmem_rma_op_matching_id );                    \
            SCOREP_ExitRegion( scorep_shmem_region__ ## FUNCNAME );                             \
                                                                                                \
            SCOREP_SHMEM_EVENT_GEN_ON();                                                        \
        }                                                                                       \
                                                                                                \
        SCOREP_IN_MEASUREMENT_DECREMENT();                                                      \
    }

/* *INDENT-ON* */

/**
 * Strided Get Routines
 *
 * @{
 */

#if SHMEM_HAVE_DECL( SHMEM_SHORT_IGET )
STRIDED_GET_ROUTINE( shmem_short_iget,      short,       sizeof( short ) )
#endif

#if SHMEM_HAVE_DECL( SHMEM_INT_IGET )
STRIDED_GET_ROUTINE( shmem_int_iget,        int,         sizeof( int ) )
#endif

#if SHMEM_HAVE_DECL( SHMEM_FLOAT_IGET )
STRIDED_GET_ROUTINE( shmem_float_iget,      float,       sizeof( float ) )
#endif

#if SHMEM_HAVE_DECL( SHMEM_LONG_IGET )
STRIDED_GET_ROUTINE( shmem_long_iget,       long,        sizeof( long ) )
#endif

#if SHMEM_HAVE_DECL( SHMEM_DOUBLE_IGET )
STRIDED_GET_ROUTINE( shmem_double_iget,     double,      sizeof( double ) )
#endif

#if SHMEM_HAVE_DECL( SHMEM_LONGLONG_IGET )
STRIDED_GET_ROUTINE( shmem_longlong_iget,   long long,   sizeof( long long ) )
#endif

#if SHMEM_HAVE_DECL( SHMEM_LONGDOUBLE_IGET ) && !defined( shmem_longdouble_iget )
#if HAVE( SHMEM_LONGDOUBLE_IGET_COMPLIANT )
STRIDED_GET_ROUTINE( shmem_longdouble_iget, long double, sizeof( long double ) )
#elif HAVE( SHMEM_LONGDOUBLE_IGET_CRAY_VARIANT )
STRIDED_GET_ROUTINE( shmem_longdouble_iget, void,        sizeof( long double ) )
#endif
#endif

#if SHMEM_HAVE_DECL( SHMEM_IGET16 )
STRIDED_GET_ROUTINE( shmem_iget16,          void,        2 )
#endif

#if SHMEM_HAVE_DECL( SHMEM_IGET32 )
STRIDED_GET_ROUTINE( shmem_iget32,          void,        4 )
#endif

#if SHMEM_HAVE_DECL( SHMEM_IGET64 )
STRIDED_GET_ROUTINE( shmem_iget64,          void,        8 )
#endif

#if SHMEM_HAVE_DECL( SHMEM_IGET128 )
STRIDED_GET_ROUTINE( shmem_iget128,         void,        16 )
#endif

/**
 * @}
 */
