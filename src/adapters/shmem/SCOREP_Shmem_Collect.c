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
 * @brief C interface wrappers for collect routines
 */

#include <config.h>

#include "scorep_shmem_internal.h"
#include <SCOREP_InMeasurement.h>
#include <SCOREP_Events.h>


/* *INDENT-OFF* */

#define COLLECT_ROUTINE( FUNCNAME, NBYTES )                                                                             \
    void                                                                                                                \
    SCOREP_LIBWRAP_FUNC_NAME( FUNCNAME ) ( void*       target,                                                          \
                                           const void* source,                                                          \
                                           size_t      nelems,                                                          \
                                           int         peStart,                                                         \
                                           int         logPeStride,                                                     \
                                           int         peSize,                                                          \
                                           long*       pSync )                                                          \
    {                                                                                                                   \
        SCOREP_IN_MEASUREMENT_INCREMENT();                                                                              \
                                                                                                                        \
        const int event_gen_active = SCOREP_SHMEM_IS_EVENT_GEN_ON;                                                      \
                                                                                                                        \
        if ( event_gen_active )                                                                                         \
        {                                                                                                               \
            SCOREP_SHMEM_EVENT_GEN_OFF();                                                                               \
                                                                                                                        \
            SCOREP_EnterWrappedRegion( scorep_shmem_region__ ## FUNCNAME );                                             \
                                                                                                                        \
            SCOREP_RmaCollectiveBegin();                                                                                \
        }                                                                                                               \
                                                                                                                        \
        SCOREP_ENTER_WRAPPED_REGION();                                                                                  \
        SCOREP_LIBWRAP_FUNC_CALL( FUNCNAME, ( target, source, nelems, peStart, logPeStride, peSize, pSync ) );          \
        SCOREP_EXIT_WRAPPED_REGION();                                                                                   \
                                                                                                                        \
        if ( event_gen_active )                                                                                         \
        {                                                                                                               \
            SCOREP_RmaCollectiveEnd( SCOREP_COLLECTIVE_ALLGATHER,                                                       \
                                     SCOREP_RMA_SYNC_LEVEL_PROCESS | SCOREP_RMA_SYNC_LEVEL_MEMORY,                      \
                                     scorep_shmem_get_pe_group( peStart, logPeStride, peSize ),                         \
                                     NO_PROCESSING_ELEMENT,                                                             \
                                     ( peSize - 1 ) * nelems * NBYTES,                                                  \
                                     ( peSize - 1 ) * nelems * NBYTES );                                                \
                                                                                                                        \
            SCOREP_ExitRegion( scorep_shmem_region__ ## FUNCNAME );                                                     \
                                                                                                                        \
            SCOREP_SHMEM_EVENT_GEN_ON();                                                                                \
        }                                                                                                               \
                                                                                                                        \
        SCOREP_IN_MEASUREMENT_DECREMENT();                                                                              \
    }

/* *INDENT-ON* */

#if SHMEM_HAVE_DECL( SHMEM_COLLECT32 )
COLLECT_ROUTINE( shmem_collect32,  4 )
#endif

#if SHMEM_HAVE_DECL( SHMEM_COLLECT64 )
COLLECT_ROUTINE( shmem_collect64,  8 )
#endif

#if SHMEM_HAVE_DECL( SHMEM_FCOLLECT32 )
COLLECT_ROUTINE( shmem_fcollect32, 4 )
#endif

#if SHMEM_HAVE_DECL( SHMEM_FCOLLECT64 )
COLLECT_ROUTINE( shmem_fcollect64, 8 )
#endif
