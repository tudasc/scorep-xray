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
 * @brief C interface wrappers for broadcast routines
 */

#include <config.h>

#include "scorep_shmem_internal.h"
#include <SCOREP_Events.h>


/* *INDENT-OFF* */

#define BROADCAST_ROUTINE( FUNCNAME, NBYTES )                                                                                   \
    void                                                                                                                        \
    SCOREP_LIBWRAP_FUNC_NAME( FUNCNAME ) ( void*       target,                                                                  \
                                           const void* source,                                                                  \
                                           size_t      nlong,                                                                   \
                                           int         peRoot,                                                                  \
                                           int         peStart,                                                                 \
                                           int         logPeStride,                                                             \
                                           int         peSize,                                                                  \
                                           long*       pSync )                                                                  \
    {                                                                                                                           \
        if ( SCOREP_SHMEM_IS_EVENT_GEN_ON )                                                                                     \
        {                                                                                                                       \
            SCOREP_SHMEM_EVENT_GEN_OFF();                                                                                       \
                                                                                                                                \
            SCOREP_InterimRmaWindowHandle window_handle                                                                         \
                = scorep_shmem_get_pe_group( peStart, logPeStride, peSize );                                                    \
                                                                                                                                \
            SCOREP_EnterRegion( scorep_shmem_region__ ## FUNCNAME );                                                            \
                                                                                                                                \
            SCOREP_RmaCollectiveBegin();                                                                                        \
                                                                                                                                \
            SCOREP_LIBWRAP_FUNC_CALL( lw, FUNCNAME, ( target, source, nlong, peRoot, peStart, logPeStride, peSize, pSync ) );   \
                                                                                                                                \
            SCOREP_RmaCollectiveEnd( SCOREP_COLLECTIVE_MPI_BCAST,                                                               \
                                     SCOREP_RMA_SYNC_LEVEL_PROCESS | SCOREP_RMA_SYNC_LEVEL_MEMORY,                              \
                                     window_handle,                                                                             \
                                     NO_PROCESSING_ELEMENT,                                                                     \
                                     NBYTES * nlong * ( peSize - 1 ),                                                           \
                                     NBYTES * nlong );                                                                          \
                                                                                                                                \
            SCOREP_ExitRegion( scorep_shmem_region__ ## FUNCNAME );                                                             \
                                                                                                                                \
            SCOREP_SHMEM_EVENT_GEN_ON();                                                                                        \
        }                                                                                                                       \
        else                                                                                                                    \
        {                                                                                                                       \
            SCOREP_LIBWRAP_FUNC_CALL( lw, FUNCNAME, ( target, source, nlong, peRoot, peStart, logPeStride, peSize, pSync ) );   \
        }                                                                                                                       \
    }

/* *INDENT-ON* */

#if SHMEM_HAVE_DECL( SHMEM_BROADCAST32 )
BROADCAST_ROUTINE( shmem_broadcast32, 4 )
#endif

#if SHMEM_HAVE_DECL( SHMEM_BROADCAST64 )
BROADCAST_ROUTINE( shmem_broadcast64, 8 )
#endif
