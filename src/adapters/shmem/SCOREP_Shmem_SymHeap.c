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
 * @brief C interface wrappers for symmetric heap routines
 */

#include <config.h>

#include "scorep_shmem_internal.h"
#include <SCOREP_Events.h>


/* *INDENT-OFF* */

#define SHMEM_MALLOC( FUNCNAME )                                            \
    void*                                                                   \
    SCOREP_LIBWRAP_FUNC_NAME( FUNCNAME ) ( size_t size )                    \
    {                                                                       \
        void* ret;                                                          \
                                                                            \
        if ( SCOREP_SHMEM_IS_EVENT_GEN_ON )                                 \
        {                                                                   \
            SCOREP_SHMEM_EVENT_GEN_OFF();                                   \
                                                                            \
            SCOREP_EnterRegion( scorep_shmem_region__ ## FUNCNAME );        \
                                                                            \
            ret = SCOREP_LIBWRAP_FUNC_CALL( lw, FUNCNAME, ( size ) );       \
                                                                            \
            SCOREP_ExitRegion( scorep_shmem_region__ ## FUNCNAME );         \
                                                                            \
            SCOREP_SHMEM_EVENT_GEN_ON();                                    \
        }                                                                   \
        else                                                                \
        {                                                                   \
            ret = SCOREP_LIBWRAP_FUNC_CALL( lw, FUNCNAME, ( size ) );       \
        }                                                                   \
                                                                            \
        return ret;                                                         \
    }

/* *INDENT-ON* */

#if SHMEM_HAVE_DECL( SHMALLOC )
SHMEM_MALLOC( shmalloc )
#endif


/* *INDENT-OFF* */

#define SHMEM_MEMALIGN( FUNCNAME )                                                  \
    void*                                                                           \
    SCOREP_LIBWRAP_FUNC_NAME( FUNCNAME ) ( size_t alignment,                        \
                                           size_t size )                            \
    {                                                                               \
        void* ret;                                                                  \
                                                                                    \
        if ( SCOREP_SHMEM_IS_EVENT_GEN_ON )                                         \
        {                                                                           \
            SCOREP_SHMEM_EVENT_GEN_OFF();                                           \
                                                                                    \
            SCOREP_EnterRegion( scorep_shmem_region__ ## FUNCNAME );                \
                                                                                    \
            ret = SCOREP_LIBWRAP_FUNC_CALL( lw, FUNCNAME, ( alignment, size ) );    \
                                                                                    \
            SCOREP_ExitRegion( scorep_shmem_region__ ## FUNCNAME );                 \
                                                                                    \
            SCOREP_SHMEM_EVENT_GEN_ON();                                            \
        }                                                                           \
        else                                                                        \
        {                                                                           \
            ret = SCOREP_LIBWRAP_FUNC_CALL( lw, FUNCNAME, ( alignment, size ) );    \
        }                                                                           \
                                                                                    \
        return ret;                                                                 \
    }

/* *INDENT-ON* */

#if SHMEM_HAVE_DECL( SHMEMALIGN )
SHMEM_MEMALIGN( shmemalign )
#endif


/* *INDENT-OFF* */

#define SHMEM_REALLOC( FUNCNAME )                                                   \
    void*                                                                           \
    SCOREP_LIBWRAP_FUNC_NAME( FUNCNAME ) ( void * ptr,                              \
                                           size_t size )                            \
    {                                                                               \
        void* ret;                                                                  \
                                                                                    \
        if ( SCOREP_SHMEM_IS_EVENT_GEN_ON )                                         \
        {                                                                           \
            SCOREP_SHMEM_EVENT_GEN_OFF();                                           \
                                                                                    \
            SCOREP_EnterRegion( scorep_shmem_region__ ## FUNCNAME );                \
                                                                                    \
            ret = SCOREP_LIBWRAP_FUNC_CALL( lw, FUNCNAME, ( ptr, size ) );          \
                                                                                    \
            SCOREP_ExitRegion( scorep_shmem_region__ ## FUNCNAME );                 \
                                                                                    \
            SCOREP_SHMEM_EVENT_GEN_ON();                                            \
        }                                                                           \
        else                                                                        \
        {                                                                           \
            ret = SCOREP_LIBWRAP_FUNC_CALL( lw, FUNCNAME, ( ptr, size ) );          \
        }                                                                           \
                                                                                    \
        return ret;                                                                 \
    }

/* *INDENT-ON* */

#if SHMEM_HAVE_DECL( SHREALLOC )
SHMEM_REALLOC( shrealloc )
#endif


/* *INDENT-OFF* */

#define SHMEM_FREE( FUNCNAME )                                          \
    void                                                                \
    SCOREP_LIBWRAP_FUNC_NAME( FUNCNAME ) ( void * ptr )                 \
    {                                                                   \
        if ( SCOREP_SHMEM_IS_EVENT_GEN_ON )                             \
        {                                                               \
            SCOREP_SHMEM_EVENT_GEN_OFF();                               \
                                                                        \
            SCOREP_EnterRegion( scorep_shmem_region__ ## FUNCNAME );    \
                                                                        \
            SCOREP_LIBWRAP_FUNC_CALL( lw, FUNCNAME, ( ptr ) );          \
                                                                        \
            SCOREP_ExitRegion( scorep_shmem_region__ ## FUNCNAME );     \
                                                                        \
            SCOREP_SHMEM_EVENT_GEN_ON();                                \
        }                                                               \
        else                                                            \
        {                                                               \
            SCOREP_LIBWRAP_FUNC_CALL( lw, FUNCNAME, ( ptr ) );          \
        }                                                               \
    }

/* *INDENT-OFF* */

#if SHMEM_HAVE_DECL( SHFREE )
SHMEM_FREE( shfree )
#endif
