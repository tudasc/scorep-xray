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
 * @brief C interface wrappers for remote pointer routines
 */

#include <config.h>

#include "scorep_shmem_internal.h"
#include <SCOREP_InMeasurement.h>
#include <SCOREP_Events.h>


/* *INDENT-OFF* */

#define REMOTE_PTR( FUNCNAME )                                              \
    void*                                                                   \
    SCOREP_LIBWRAP_FUNC_NAME( FUNCNAME ) ( void * target,                   \
                                           int    pe )                      \
    {                                                                       \
        SCOREP_IN_MEASUREMENT_INCREMENT();                                  \
                                                                            \
        void* ret;                                                          \
                                                                            \
        if ( SCOREP_SHMEM_IS_EVENT_GEN_ON )                                 \
        {                                                                   \
            SCOREP_SHMEM_EVENT_GEN_OFF();                                   \
                                                                            \
            SCOREP_EnterWrappedRegion( scorep_shmem_region__ ## FUNCNAME,   \
                                       ( intptr_t )CALL_SHMEM( FUNCNAME ) );\
                                                                            \
            SCOREP_ENTER_WRAPPED_REGION();                                  \
            ret = SCOREP_LIBWRAP_FUNC_CALL( lw, FUNCNAME, ( target, pe ) ); \
            SCOREP_EXIT_WRAPPED_REGION();                                   \
                                                                            \
            SCOREP_ExitRegion( scorep_shmem_region__ ## FUNCNAME );         \
                                                                            \
            SCOREP_SHMEM_EVENT_GEN_ON();                                    \
        }                                                                   \
        else                                                                \
        {                                                                   \
            ret = SCOREP_LIBWRAP_FUNC_CALL( lw, FUNCNAME, ( target, pe ) ); \
        }                                                                   \
                                                                            \
        SCOREP_IN_MEASUREMENT_DECREMENT();                                  \
        return ret;                                                         \
    }

/* *INDENT-ON* */

#if SHMEM_HAVE_DECL( SHMEM_PTR )
REMOTE_PTR( shmem_ptr )
#endif
