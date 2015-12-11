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
 * @brief C interface wrappers for reduction routines
 */

#include <config.h>

#include "scorep_shmem_internal.h"
#include <SCOREP_InMeasurement.h>
#include <SCOREP_Events.h>


/* *INDENT-OFF* */

#define REDUCTION( FUNCNAME, DATATYPE, SRC_DATATYPE, SIZE_TYPE )                                                                \
    void                                                                                                                        \
    SCOREP_LIBWRAP_FUNC_NAME( FUNCNAME ) ( DATATYPE *     target,                                                               \
                                           SRC_DATATYPE * source,                                                               \
                                           SIZE_TYPE      nreduce,                                                              \
                                           int            peStart,                                                              \
                                           int            logPeStride,                                                          \
                                           int            peSize,                                                               \
                                           DATATYPE *     pWork,                                                                \
                                           long*          pSync )                                                               \
    {                                                                                                                           \
        SCOREP_IN_MEASUREMENT_INCREMENT();                                                                                      \
                                                                                                                                \
        if ( SCOREP_SHMEM_IS_EVENT_GEN_ON )                                                                                     \
        {                                                                                                                       \
            SCOREP_SHMEM_EVENT_GEN_OFF();                                                                                       \
                                                                                                                                \
            SCOREP_InterimRmaWindowHandle window_handle                                                                         \
                = scorep_shmem_get_pe_group( peStart, logPeStride, peSize );                                                    \
                                                                                                                                \
            SCOREP_EnterWrappedRegion( scorep_shmem_region__ ## FUNCNAME,                                                       \
                                       ( intptr_t )CALL_SHMEM( FUNCNAME ) );                                                    \
                                                                                                                                \
            SCOREP_RmaCollectiveBegin();                                                                                        \
                                                                                                                                \
            SCOREP_ENTER_WRAPPED_REGION();                                                                                      \
            SCOREP_LIBWRAP_FUNC_CALL( lw, FUNCNAME, ( target, source, nreduce, peStart, logPeStride, peSize, pWork, pSync ) );  \
            SCOREP_EXIT_WRAPPED_REGION();                                                                                       \
                                                                                                                                \
            SCOREP_RmaCollectiveEnd( SCOREP_COLLECTIVE_REDUCE,                                                                  \
                                     SCOREP_RMA_SYNC_LEVEL_PROCESS | SCOREP_RMA_SYNC_LEVEL_MEMORY,                              \
                                     window_handle,                                                                             \
                                     NO_PROCESSING_ELEMENT,                                                                     \
                                     nreduce * ( peSize - 1 ) * sizeof( DATATYPE ),                                             \
                                     nreduce * ( peSize - 1 ) * sizeof( DATATYPE ) );                                           \
                                                                                                                                \
            SCOREP_ExitRegion( scorep_shmem_region__ ## FUNCNAME );                                                             \
                                                                                                                                \
            SCOREP_SHMEM_EVENT_GEN_ON();                                                                                        \
        }                                                                                                                       \
        else                                                                                                                    \
        {                                                                                                                       \
            SCOREP_LIBWRAP_FUNC_CALL( lw, FUNCNAME, ( target, source, nreduce, peStart, logPeStride, peSize, pWork, pSync ) );  \
        }                                                                                                                       \
                                                                                                                                \
        SCOREP_IN_MEASUREMENT_DECREMENT();                                                                                      \
    }

/* *INDENT-ON* */

/**
 * Reduction Operations (shmem_and)
 *
 * @{
 */

#if SHMEM_HAVE_DECL( SHMEM_SHORT_AND_TO_ALL )
#if HAVE( SHMEM_SHORT_AND_TO_ALL_COMPLIANT )
REDUCTION( shmem_short_and_to_all,    short, short, int )
#elif HAVE( SHMEM_SHORT_AND_TO_ALL_CONST_VARIANT )
REDUCTION( shmem_short_and_to_all,    short, const short, size_t )
#endif
#endif

#if SHMEM_HAVE_DECL( SHMEM_INT_AND_TO_ALL )
#if HAVE( SHMEM_INT_AND_TO_ALL_COMPLIANT )
REDUCTION( shmem_int_and_to_all,      int, int, int )
#elif HAVE( SHMEM_INT_AND_TO_ALL_CONST_VARIANT )
REDUCTION( shmem_int_and_to_all,      int, const int, size_t )
#endif
#endif

#if SHMEM_HAVE_DECL( SHMEM_LONG_AND_TO_ALL )
#if HAVE( SHMEM_LONG_AND_TO_ALL_COMPLIANT )
REDUCTION( shmem_long_and_to_all,     long, long, int )
#elif HAVE( SHMEM_LONG_AND_TO_ALL_CONST_VARIANT )
REDUCTION( shmem_long_and_to_all,     long, const long, size_t )
#endif
#endif

#if SHMEM_HAVE_DECL( SHMEM_LONG_AND_TO_ALL )
#if HAVE( SHMEM_LONGLONG_AND_TO_ALL_COMPLIANT )
REDUCTION( shmem_longlong_and_to_all, long long, long long, int )
#elif HAVE( SHMEM_LONGLONG_AND_TO_ALL_CONST_VARIANT )
REDUCTION( shmem_longlong_and_to_all, long long, const long long, size_t )
#endif
#endif

/**
 * @}
 */


/**
 * Reduction Operations (shmem_or)
 *
 * @{
 */

#if SHMEM_HAVE_DECL( SHMEM_SHORT_OR_TO_ALL )
#if HAVE( SHMEM_SHORT_OR_TO_ALL_COMPLIANT )
REDUCTION( shmem_short_or_to_all,       short, short, int )
#elif HAVE( SHMEM_SHORT_OR_TO_ALL_CONST_VARIANT )
REDUCTION( shmem_short_or_to_all,       short, const short, size_t )
#endif
#endif

#if SHMEM_HAVE_DECL( SHMEM_INT_OR_TO_ALL )
#if HAVE( SHMEM_INT_OR_TO_ALL_COMPLIANT )
REDUCTION( shmem_int_or_to_all,         int, int, int )
#elif HAVE( SHMEM_INT_OR_TO_ALL_CONST_VARIANT )
REDUCTION( shmem_int_or_to_all,         int, const int, size_t )
#endif
#endif

#if SHMEM_HAVE_DECL( SHMEM_LONG_OR_TO_ALL )
#if HAVE( SHMEM_LONG_OR_TO_ALL_COMPLIANT )
REDUCTION( shmem_long_or_to_all,        long, long, int )
#elif HAVE( SHMEM_LONG_OR_TO_ALL_CONST_VARIANT )
REDUCTION( shmem_long_or_to_all,        long, const long, size_t )
#endif
#endif

#if SHMEM_HAVE_DECL( SHMEM_LONG_OR_TO_ALL )
#if HAVE( SHMEM_LONGLONG_OR_TO_ALL_COMPLIANT )
REDUCTION( shmem_longlong_or_to_all,    long long, long long, int )
#elif HAVE( SHMEM_LONGLONG_OR_TO_ALL_CONST_VARIANT )
REDUCTION( shmem_longlong_or_to_all,    long long, const long long, size_t )
#endif
#endif

/**
 * @}
 */


/**
 * Reduction Operations (shmem_xor)
 *
 * @{
 */

#if SHMEM_HAVE_DECL( SHMEM_SHORT_XOR_TO_ALL )
#if HAVE( SHMEM_SHORT_XOR_TO_ALL_COMPLIANT )
REDUCTION( shmem_short_xor_to_all,      short, short, int )
#elif HAVE( SHMEM_SHORT_XOR_TO_ALL_CONST_VARIANT )
REDUCTION( shmem_short_xor_to_all,      short, const short, size_t )
#endif
#endif

#if SHMEM_HAVE_DECL( SHMEM_INT_XOR_TO_ALL )
#if HAVE( SHMEM_INT_XOR_TO_ALL_COMPLIANT )
REDUCTION( shmem_int_xor_to_all,        int, int, int )
#elif HAVE( SHMEM_INT_XOR_TO_ALL_CONST_VARIANT )
REDUCTION( shmem_int_xor_to_all,        int, const int, size_t )
#endif
#endif

#if SHMEM_HAVE_DECL( SHMEM_LONG_XOR_TO_ALL )
#if HAVE( SHMEM_LONG_XOR_TO_ALL_COMPLIANT )
REDUCTION( shmem_long_xor_to_all,       long, long, int )
#elif HAVE( SHMEM_LONG_XOR_TO_ALL_CONST_VARIANT )
REDUCTION( shmem_long_xor_to_all,       long, const long, size_t )
#endif
#endif

#if SHMEM_HAVE_DECL( SHMEM_LONG_XOR_TO_ALL )
#if HAVE( SHMEM_LONGLONG_XOR_TO_ALL_COMPLIANT )
REDUCTION( shmem_longlong_xor_to_all,   long long, long long, int )
#elif HAVE( SHMEM_LONGLONG_XOR_TO_ALL_CONST_VARIANT )
REDUCTION( shmem_longlong_xor_to_all,   long long, const long long, size_t )
#endif
#endif

/**
 * @}
 */


/**
 * Reduction Operations (shmem_max)
 *
 * @{
 */

#if SHMEM_HAVE_DECL( SHMEM_SHORT_MAX_TO_ALL )
#if HAVE( SHMEM_SHORT_MAX_TO_ALL_COMPLIANT )
REDUCTION( shmem_short_max_to_all,      short, short, int )
#elif HAVE( SHMEM_SHORT_MAX_TO_ALL_CONST_VARIANT )
REDUCTION( shmem_short_max_to_all,      short, const short, size_t )
#endif
#endif

#if SHMEM_HAVE_DECL( SHMEM_INT_MAX_TO_ALL )
#if HAVE( SHMEM_INT_MAX_TO_ALL_COMPLIANT )
REDUCTION( shmem_int_max_to_all,        int, int, int )
#elif HAVE( SHMEM_INT_MAX_TO_ALL_CONST_VARIANT )
REDUCTION( shmem_int_max_to_all,        int, const int, size_t )
#endif
#endif

#if SHMEM_HAVE_DECL( SHMEM_LONG_MAX_TO_ALL )
#if HAVE( SHMEM_LONG_MAX_TO_ALL_COMPLIANT )
REDUCTION( shmem_long_max_to_all,       long, long, int )
#elif HAVE( SHMEM_LONG_MAX_TO_ALL_CONST_VARIANT )
REDUCTION( shmem_long_max_to_all,       long, const long, size_t )
#endif
#endif

#if SHMEM_HAVE_DECL( SHMEM_FLOAT_MAX_TO_ALL )
#if HAVE( SHMEM_FLOAT_MAX_TO_ALL_COMPLIANT )
REDUCTION( shmem_float_max_to_all,      float, float, int )
#elif HAVE( SHMEM_FLOAT_MAX_TO_ALL_CONST_VARIANT )
REDUCTION( shmem_float_max_to_all,      float, const float, size_t )
#endif
#endif

#if SHMEM_HAVE_DECL( SHMEM_DOUBLE_MAX_TO_ALL )
#if HAVE( SHMEM_DOUBLE_MAX_TO_ALL_COMPLIANT )
REDUCTION( shmem_double_max_to_all,     double, double, int )
#elif HAVE( SHMEM_DOUBLE_MAX_TO_ALL_CONST_VARIANT )
REDUCTION( shmem_double_max_to_all,     double, const double, size_t )
#endif
#endif

#if SHMEM_HAVE_DECL( SHMEM_LONGLONG_MAX_TO_ALL )
#if HAVE( SHMEM_LONGLONG_MAX_TO_ALL_COMPLIANT )
REDUCTION( shmem_longlong_max_to_all, long long, long long, int )
#elif HAVE( SHMEM_LONGLONG_MAX_TO_ALL_CONST_VARIANT )
REDUCTION( shmem_longlong_max_to_all, long long, const long long, size_t )
#endif
#endif

#if SHMEM_HAVE_DECL( SHMEM_LONGDOUBLE_MAX_TO_ALL )
#if HAVE( SHMEM_LONGDOUBLE_MAX_TO_ALL_COMPLIANT )
REDUCTION( shmem_longdouble_max_to_all, long double, long double, int )
#elif HAVE( SHMEM_LONGDOUBLE_MAX_TO_ALL_CONST_VARIANT )
REDUCTION( shmem_longdouble_max_to_all, long double, const long double, size_t )
#endif
#endif

/**
 * @}
 */


/**
 * Reduction Operations (shmem_min)
 *
 * @{
 */

#if SHMEM_HAVE_DECL( SHMEM_SHORT_MIN_TO_ALL )
#if HAVE( SHMEM_SHORT_MIN_TO_ALL_COMPLIANT )
REDUCTION( shmem_short_min_to_all,      short, short, int )
#elif HAVE( SHMEM_SHORT_MIN_TO_ALL_CONST_VARIANT )
REDUCTION( shmem_short_min_to_all,      short, const short, size_t )
#endif
#endif

#if SHMEM_HAVE_DECL( SHMEM_INT_MIN_TO_ALL )
#if HAVE( SHMEM_INT_MIN_TO_ALL_COMPLIANT )
REDUCTION( shmem_int_min_to_all,        int, int, int )
#elif HAVE( SHMEM_INT_MIN_TO_ALL_CONST_VARIANT )
REDUCTION( shmem_int_min_to_all,        int, const int, size_t )
#endif
#endif

#if SHMEM_HAVE_DECL( SHMEM_LONG_MIN_TO_ALL )
#if HAVE( SHMEM_LONG_MIN_TO_ALL_COMPLIANT )
REDUCTION( shmem_long_min_to_all,       long, long, int )
#elif HAVE( SHMEM_LONG_MIN_TO_ALL_CONST_VARIANT )
REDUCTION( shmem_long_min_to_all,       long, const long, size_t )
#endif
#endif

#if SHMEM_HAVE_DECL( SHMEM_FLOAT_MIN_TO_ALL )
#if HAVE( SHMEM_FLOAT_MIN_TO_ALL_COMPLIANT )
REDUCTION( shmem_float_min_to_all,      float, float, int )
#elif HAVE( SHMEM_FLOAT_MIN_TO_ALL_CONST_VARIANT )
REDUCTION( shmem_float_min_to_all,      float, const float, size_t )
#endif
#endif

#if SHMEM_HAVE_DECL( SHMEM_DOUBLE_MIN_TO_ALL )
#if HAVE( SHMEM_DOUBLE_MIN_TO_ALL_COMPLIANT )
REDUCTION( shmem_double_min_to_all,     double, double, int )
#elif HAVE( SHMEM_DOUBLE_MIN_TO_ALL_CONST_VARIANT )
REDUCTION( shmem_double_min_to_all,     double, const double, size_t )
#endif
#endif

#if SHMEM_HAVE_DECL( SHMEM_LONGLONG_MIN_TO_ALL )
#if HAVE( SHMEM_LONGLONG_MIN_TO_ALL_COMPLIANT )
REDUCTION( shmem_longlong_min_to_all, long long, long long, int )
#elif HAVE( SHMEM_LONGLONG_MIN_TO_ALL_CONST_VARIANT )
REDUCTION( shmem_longlong_min_to_all, long long, const long long, size_t )
#endif
#endif

#if SHMEM_HAVE_DECL( SHMEM_LONGDOUBLE_MIN_TO_ALL )
#if HAVE( SHMEM_LONGDOUBLE_MIN_TO_ALL_COMPLIANT )
REDUCTION( shmem_longdouble_min_to_all, long double, long double, int )
#elif HAVE( SHMEM_LONGDOUBLE_MIN_TO_ALL_CONST_VARIANT )
REDUCTION( shmem_longdouble_min_to_all, long double, const long double, size_t )
#endif
#endif

/**
 * @}
 */


/**
 * Reduction Operations (shmem_sum)
 *
 * @{
 */

#if SHMEM_HAVE_DECL( SHMEM_SHORT_SUM_TO_ALL )
#if HAVE( SHMEM_SHORT_SUM_TO_ALL_COMPLIANT )
REDUCTION( shmem_short_sum_to_all,      short, short, int )
#elif HAVE( SHMEM_SHORT_SUM_TO_ALL_CONST_VARIANT )
REDUCTION( shmem_short_sum_to_all,      short, const short, size_t )
#endif
#endif

#if SHMEM_HAVE_DECL( SHMEM_INT_SUM_TO_ALL )
#if HAVE( SHMEM_INT_SUM_TO_ALL_COMPLIANT )
REDUCTION( shmem_int_sum_to_all,        int, int, int )
#elif HAVE( SHMEM_INT_SUM_TO_ALL_CONST_VARIANT )
REDUCTION( shmem_int_sum_to_all,        int, const int, size_t )
#endif
#endif

#if SHMEM_HAVE_DECL( SHMEM_LONG_SUM_TO_ALL )
#if HAVE( SHMEM_LONG_SUM_TO_ALL_COMPLIANT )
REDUCTION( shmem_long_sum_to_all,       long, long, int )
#elif HAVE( SHMEM_LONG_SUM_TO_ALL_CONST_VARIANT )
REDUCTION( shmem_long_sum_to_all,       long, const long, size_t )
#endif
#endif

#if SHMEM_HAVE_DECL( SHMEM_FLOAT_SUM_TO_ALL )
#if HAVE( SHMEM_FLOAT_SUM_TO_ALL_COMPLIANT )
REDUCTION( shmem_float_sum_to_all,      float, float, int )
#elif HAVE( SHMEM_FLOAT_SUM_TO_ALL_CONST_VARIANT )
REDUCTION( shmem_float_sum_to_all,      float, const float, size_t )
#endif
#endif

#if SHMEM_HAVE_DECL( SHMEM_DOUBLE_SUM_TO_ALL )
#if HAVE( SHMEM_DOUBLE_SUM_TO_ALL_COMPLIANT )
REDUCTION( shmem_double_sum_to_all,     double, double, int )
#elif HAVE( SHMEM_DOUBLE_SUM_TO_ALL_CONST_VARIANT )
REDUCTION( shmem_double_sum_to_all,     double, const double, size_t )
#endif
#endif

#if SHMEM_HAVE_DECL( SHMEM_LONGLONG_SUM_TO_ALL )
#if HAVE( SHMEM_LONGLONG_SUM_TO_ALL_COMPLIANT )
REDUCTION( shmem_longlong_sum_to_all, long long, long long, int )
#elif HAVE( SHMEM_LONGLONG_SUM_TO_ALL_CONST_VARIANT )
REDUCTION( shmem_longlong_sum_to_all, long long, const long long, size_t )
#endif
#endif

#if SHMEM_HAVE_DECL( SHMEM_LONGDOUBLE_SUM_TO_ALL )
#if HAVE( SHMEM_LONGDOUBLE_SUM_TO_ALL_COMPLIANT )
REDUCTION( shmem_longdouble_sum_to_all, long double, long double, int )
#elif HAVE( SHMEM_LONGDOUBLE_SUM_TO_ALL_CONST_VARIANT )
REDUCTION( shmem_longdouble_sum_to_all, long double, const long double, size_t )
#endif
#endif

#if SHMEM_HAVE_DECL( SHMEM_COMPLEXF_SUM_TO_ALL )
#if HAVE( SHMEM_COMPLEXF_SUM_TO_ALL_COMPLIANT )
REDUCTION( shmem_complexf_sum_to_all,   float complex, float complex, int )
#elif HAVE( SHMEM_COMPLEXF_SUM_TO_ALL_CONST_VARIANT )
REDUCTION( shmem_complexf_sum_to_all,   float complex, const float complex, size_t )
#endif
#endif

#if SHMEM_HAVE_DECL( SHMEM_COMPLEXD_SUM_TO_ALL )
#if HAVE( SHMEM_COMPLEXD_SUM_TO_ALL_COMPLIANT )
REDUCTION( shmem_complexd_sum_to_all,   double complex, double complex, int )
#elif HAVE( SHMEM_COMPLEXD_SUM_TO_ALL_CONST_VARIANT )
REDUCTION( shmem_complexd_sum_to_all,   double complex, const double complex, size_t )
#endif
#endif


/**
 * @}
 */


/**
 * Reduction Operations (shmem_prod)
 *
 * @{
 */

#if SHMEM_HAVE_DECL( SHMEM_SHORT_PROD_TO_ALL )
#if HAVE( SHMEM_SHORT_PROD_TO_ALL_COMPLIANT )
REDUCTION( shmem_short_prod_to_all,     short, short, int )
#elif HAVE( SHMEM_SHORT_PROD_TO_ALL_CONST_VARIANT )
REDUCTION( shmem_short_prod_to_all,     short, const short, size_t )
#endif
#endif

#if SHMEM_HAVE_DECL( SHMEM_INT_PROD_TO_ALL )
#if HAVE( SHMEM_INT_PROD_TO_ALL_COMPLIANT )
REDUCTION( shmem_int_prod_to_all,       int, int, int )
#elif HAVE( SHMEM_INT_PROD_TO_ALL_CONST_VARIANT )
REDUCTION( shmem_int_prod_to_all,       int, const int, size_t )
#endif
#endif

#if SHMEM_HAVE_DECL( SHMEM_LONG_PROD_TO_ALL )
#if HAVE( SHMEM_LONG_PROD_TO_ALL_COMPLIANT )
REDUCTION( shmem_long_prod_to_all,      long, long, int )
#elif HAVE( SHMEM_LONG_PROD_TO_ALL_CONST_VARIANT )
REDUCTION( shmem_long_prod_to_all,      long, const long, size_t )
#endif
#endif

#if SHMEM_HAVE_DECL( SHMEM_FLOAT_PROD_TO_ALL )
#if HAVE( SHMEM_FLOAT_PROD_TO_ALL_COMPLIANT )
REDUCTION( shmem_float_prod_to_all,     float, float, int )
#elif HAVE( SHMEM_FLOAT_PROD_TO_ALL_CONST_VARIANT )
REDUCTION( shmem_float_prod_to_all,     float, const float, size_t )
#endif
#endif

#if SHMEM_HAVE_DECL( SHMEM_DOUBLE_PROD_TO_ALL )
#if HAVE( SHMEM_DOUBLE_PROD_TO_ALL_COMPLIANT )
REDUCTION( shmem_double_prod_to_all,    double, double, int )
#elif HAVE( SHMEM_DOUBLE_PROD_TO_ALL_CONST_VARIANT )
REDUCTION( shmem_double_prod_to_all,    double, const double, size_t )
#endif
#endif

#if SHMEM_HAVE_DECL( SHMEM_LONGLONG_PROD_TO_ALL )
#if HAVE( SHMEM_LONGLONG_PROD_TO_ALL_COMPLIANT )
REDUCTION( shmem_longlong_prod_to_all,  long long, long long, int )
#elif HAVE( SHMEM_LONGLONG_PROD_TO_ALL_CONST_VARIANT )
REDUCTION( shmem_longlong_prod_to_all,  long long, const long long, size_t )
#endif
#endif

#if SHMEM_HAVE_DECL( SHMEM_LONGDOUBLE_PROD_TO_ALL )
#if HAVE( SHMEM_LONGDOUBLE_PROD_TO_ALL_COMPLIANT )
REDUCTION( shmem_longdouble_prod_to_all, long double, long double, int )
#elif HAVE( SHMEM_LONGDOUBLE_PROD_TO_ALL_CONST_VARIANT )
REDUCTION( shmem_longdouble_prod_to_all, long double, const long double, size_t )
#endif
#endif

#if SHMEM_HAVE_DECL( SHMEM_COMPLEXF_PROD_TO_ALL )
#if HAVE( SHMEM_COMPLEXF_PROD_TO_ALL_COMPLIANT )
REDUCTION( shmem_complexf_prod_to_all,   float complex, float complex, int )
#elif HAVE( SHMEM_COMPLEXF_PROD_TO_ALL_CONST_VARIANT )
REDUCTION( shmem_complexf_prod_to_all,   float complex, const float complex, size_t )
#endif
#endif

#if SHMEM_HAVE_DECL( SHMEM_COMPLEXD_PROD_TO_ALL )
#if HAVE( SHMEM_COMPLEXD_PROD_TO_ALL_COMPLIANT )
REDUCTION( shmem_complexd_prod_to_all,   double complex, double complex, int )
#elif HAVE( SHMEM_COMPLEXD_PROD_TO_ALL_CONST_VARIANT )
REDUCTION( shmem_complexd_prod_to_all,   double complex, const double complex, size_t )
#endif
#endif

/**
 * @}
 */
