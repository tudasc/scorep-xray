/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2013-2014,
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
#include <SCOREP_Events.h>


/* *INDENT-OFF* */

#define REMOTE_PTR( FUNCNAME )                                              \
    void*                                                                   \
    SCOREP_LIBWRAP_FUNC_NAME( FUNCNAME ) ( void * target,                   \
                                           int    pe )                      \
    {                                                                       \
        void* ret;                                                          \
                                                                            \
        if ( SCOREP_SHMEM_IS_EVENT_GEN_ON )                                 \
        {                                                                   \
            SCOREP_SHMEM_EVENT_GEN_OFF();                                   \
                                                                            \
            SCOREP_EnterRegion( scorep_shmem_region__ ## FUNCNAME );        \
                                                                            \
            ret = SCOREP_LIBWRAP_FUNC_CALL( lw, FUNCNAME, ( target, pe ) ); \
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
        return ret;                                                         \
    }

/* *INDENT-ON* */

#if SHMEM_HAVE_DECL( SHMEM_PTR )
REMOTE_PTR( shmem_ptr )
#endif