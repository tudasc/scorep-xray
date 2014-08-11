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
 * @brief C interface wrappers for deprecated data cache routines
 */

#include <config.h>

#include "scorep_shmem_internal.h"
#include <SCOREP_Events.h>


/* *INDENT-OFF* */

#define DATA_CACHE_ROUTINE( FUNCNAME )                                  \
    void                                                                \
    SCOREP_LIBWRAP_FUNC_NAME( FUNCNAME ) ( void )                       \
    {                                                                   \
        if ( SCOREP_SHMEM_IS_EVENT_GEN_ON )                             \
        {                                                               \
            SCOREP_SHMEM_EVENT_GEN_OFF();                               \
                                                                        \
            SCOREP_EnterRegion( scorep_shmem_region__ ## FUNCNAME );    \
                                                                        \
            SCOREP_LIBWRAP_FUNC_CALL( lw, FUNCNAME, ( ) );              \
                                                                        \
            SCOREP_ExitRegion( scorep_shmem_region__ ## FUNCNAME );     \
                                                                        \
            SCOREP_SHMEM_EVENT_GEN_ON();                                \
        }                                                               \
        else                                                            \
        {                                                               \
            SCOREP_LIBWRAP_FUNC_CALL( lw, FUNCNAME, ( ) );              \
        }                                                               \
    }

/* *INDENT-ON* */

#if SHMEM_HAVE_DECL( SHMEM_CLEAR_CACHE_INV )
DATA_CACHE_ROUTINE( shmem_clear_cache_inv )
#endif

#if SHMEM_HAVE_DECL( SHMEM_SET_CACHE_INV )
DATA_CACHE_ROUTINE( shmem_set_cache_inv )
#endif

#if SHMEM_HAVE_DECL( SHMEM_UDCFLUSH )
DATA_CACHE_ROUTINE( shmem_udcflush )
#endif


/* *INDENT-OFF* */

#define DATA_CACHE_ROUTINE_WITH_ARGUMENT( FUNCNAME )                    \
    void                                                                \
    SCOREP_LIBWRAP_FUNC_NAME( FUNCNAME ) ( void * target )              \
    {                                                                   \
        if ( SCOREP_SHMEM_IS_EVENT_GEN_ON )                             \
        {                                                               \
            SCOREP_SHMEM_EVENT_GEN_OFF();                               \
                                                                        \
            SCOREP_EnterRegion( scorep_shmem_region__ ## FUNCNAME );    \
                                                                        \
            SCOREP_LIBWRAP_FUNC_CALL( lw, FUNCNAME, ( target ) );       \
                                                                        \
            SCOREP_ExitRegion( scorep_shmem_region__ ## FUNCNAME );     \
                                                                        \
            SCOREP_SHMEM_EVENT_GEN_ON();                                \
        }                                                               \
        else                                                            \
        {                                                               \
            SCOREP_LIBWRAP_FUNC_CALL( lw, FUNCNAME, ( target ) );       \
        }                                                               \
    }

/* *INDENT-ON* */

#if SHMEM_HAVE_DECL( SHMEM_CLEAR_CACHE_LINE_INV )
DATA_CACHE_ROUTINE_WITH_ARGUMENT( shmem_clear_cache_line_inv )
#endif

#if SHMEM_HAVE_DECL( SHMEM_SET_CACHE_LINE_INV )
DATA_CACHE_ROUTINE_WITH_ARGUMENT( shmem_set_cache_line_inv )
#endif

#if SHMEM_HAVE_DECL( SHMEM_UDCFLUSH_LINE )
DATA_CACHE_ROUTINE_WITH_ARGUMENT( shmem_udcflush_line )
#endif
