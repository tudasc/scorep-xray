/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2013-2015, 2017,
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
 * @brief C interface wrappers for team routines
 */

#include <config.h>

#include "scorep_shmem_internal.h"
#include <SCOREP_InMeasurement.h>
#include <SCOREP_Events.h>


/* *INDENT-OFF* */

#define TEAM_SPLIT( FUNCNAME )                                              \
    void                                                                    \
    SCOREP_LIBWRAP_FUNC_NAME( FUNCNAME ) ( shmem_team_t   team,             \
                                           int            color,            \
                                           int            key,              \
                                           shmem_team_t * newTeam )         \
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
        SCOREP_LIBWRAP_FUNC_CALL( FUNCNAME,                                 \
                                  ( team, color, key, newTeam) );           \
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

/* *INDENT-ON* */

#if SHMEM_HAVE_DECL( SHMEM_TEAM_SPLIT )
TEAM_SPLIT( shmem_team_split )
#endif


/* *INDENT-OFF* */

#define TEAM_CREATE_STRIDED( FUNCNAME )                                     \
    void                                                                    \
    SCOREP_LIBWRAP_FUNC_NAME( FUNCNAME ) ( int            peStart,          \
                                           int            peStride,         \
                                           int            peSize,           \
                                           shmem_team_t * newTeam )         \
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
        SCOREP_LIBWRAP_FUNC_CALL( FUNCNAME,                                 \
                        ( peStart, peStride, peSize, newTeam ) );           \
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

/* *INDENT-ON* */

#if SHMEM_HAVE_DECL( SHMEM_TEAM_CREATE_STRIDED )
TEAM_CREATE_STRIDED( shmem_team_create_strided )
#endif


/* *INDENT-OFF* */

#define TEAM_FREE( FUNCNAME )                                               \
    void                                                                    \
    SCOREP_LIBWRAP_FUNC_NAME( FUNCNAME ) ( shmem_team_t * team )            \
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
        SCOREP_LIBWRAP_FUNC_CALL( FUNCNAME, ( team ) );                     \
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

/* *INDENT-ON* */

#if SHMEM_HAVE_DECL( SHMEM_TEAM_FREE )
TEAM_FREE( shmem_team_free )
#endif


/* *INDENT-OFF* */

#define TEAM_QUERY( FUNCNAME )                                              \
    int                                                                     \
    SCOREP_LIBWRAP_FUNC_NAME( FUNCNAME ) ( shmem_team_t team )              \
    {                                                                       \
        SCOREP_IN_MEASUREMENT_INCREMENT();                                  \
                                                                            \
        int ret;                                                            \
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
        ret = SCOREP_LIBWRAP_FUNC_CALL( FUNCNAME, ( team ) );               \
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
        return ret;                                                         \
    }

/* *INDENT-ON* */

#if SHMEM_HAVE_DECL( SHMEM_TEAM_NPES )
TEAM_QUERY( shmem_team_npes )
#endif

#if SHMEM_HAVE_DECL( SHMEM_TEAM_MYPE )
TEAM_QUERY( shmem_team_mype )
#endif


/* *INDENT-OFF* */

#define TEAM_TRANSLATE_PE( FUNCNAME )                                       \
    int                                                                     \
    SCOREP_LIBWRAP_FUNC_NAME( FUNCNAME ) ( shmem_team_t team1,              \
                                           int          team1Pe,            \
                                           shmem_team_t team2 )             \
    {                                                                       \
        SCOREP_IN_MEASUREMENT_INCREMENT();                                  \
                                                                            \
        int ret;                                                            \
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
        ret = SCOREP_LIBWRAP_FUNC_CALL( FUNCNAME,                           \
                                  ( team1, team1Pe, team2 ) );              \
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
        return ret;                                                         \
    }

/* *INDENT-ON* */

#if SHMEM_HAVE_DECL( SHMEM_TEAM_TRANSLATE_PE )
TEAM_TRANSLATE_PE( shmem_team_translate_pe )
#endif


/* *INDENT-OFF* */

#define TEAM_BARRIER( FUNCNAME )                                            \
    void                                                                    \
    SCOREP_LIBWRAP_FUNC_NAME( FUNCNAME ) ( shmem_team_t team,               \
                                           long *       pSync )             \
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
        SCOREP_LIBWRAP_FUNC_CALL( FUNCNAME, ( team, pSync ) );              \
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

/* *INDENT-ON* */

#if SHMEM_HAVE_DECL( SHMEM_TEAM_BARRIER )
TEAM_BARRIER( shmem_team_barrier )
#endif


/* *INDENT-OFF* */

#define TEAM_ALLTOALL( FUNCNAME )                                           \
    void                                                                    \
    SCOREP_LIBWRAP_FUNC_NAME( FUNCNAME ) ( void *       target,             \
                                           const void * src,                \
                                           size_t       len,                \
                                           shmem_team_t team,               \
                                           long *       pSync )             \
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
        SCOREP_LIBWRAP_FUNC_CALL( FUNCNAME,                                 \
                            ( target, src, len, team, pSync ) );            \
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

/* *INDENT-ON* */

#if SHMEM_HAVE_DECL( SHMEM_TEAM_ALLTOALL )
TEAM_ALLTOALL( shmem_team_alltoall )
#endif


/* *INDENT-OFF* */

#define TEAM_ALLTOALLV( FUNCNAME )                                                  \
    void                                                                            \
    SCOREP_LIBWRAP_FUNC_NAME( FUNCNAME ) ( void *       target,                     \
                                           size_t *     tOffsets,                   \
                                           size_t *     tSizes,                     \
                                           const void * src,                        \
                                           size_t *     sOffsets,                   \
                                           size_t *     sSizes,                     \
                                           shmem_team_t team,                       \
                                           long *       pSync )                     \
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
        }                                                                           \
                                                                                    \
        SCOREP_ENTER_WRAPPED_REGION();                                              \
        SCOREP_LIBWRAP_FUNC_CALL( FUNCNAME,                                         \
            ( target, tOffsets, tSizes, src, sOffsets, sSizes, team, pSync ) );     \
        SCOREP_EXIT_WRAPPED_REGION();                                               \
                                                                                    \
        if ( event_gen_active )                                                     \
        {                                                                           \
            SCOREP_ExitRegion( scorep_shmem_region__ ## FUNCNAME );                 \
                                                                                    \
            SCOREP_SHMEM_EVENT_GEN_ON();                                            \
        }                                                                           \
                                                                                    \
        SCOREP_IN_MEASUREMENT_DECREMENT();                                          \
    }

/* *INDENT-ON* */

#if SHMEM_HAVE_DECL( SHMEM_TEAM_ALLTOALLV )
TEAM_ALLTOALLV( shmem_team_alltoallv )
#endif


/* *INDENT-OFF* */

#define TEAM_ALLTOALLV_PACKED( FUNCNAME )                                           \
    void                                                                            \
    SCOREP_LIBWRAP_FUNC_NAME( FUNCNAME ) ( void *       target,                     \
                                           size_t       tLength,                    \
                                           size_t *     tSizes,                     \
                                           const void * src,                        \
                                           size_t *     sOffsets,                   \
                                           size_t *     sSizes,                     \
                                           shmem_team_t team,                       \
                                           long *       pSync )                     \
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
        }                                                                           \
                                                                                    \
        SCOREP_ENTER_WRAPPED_REGION();                                              \
        SCOREP_LIBWRAP_FUNC_CALL( FUNCNAME,                                         \
            ( target, tLength, tSizes, src, sOffsets, sSizes, team, pSync ) );      \
        SCOREP_EXIT_WRAPPED_REGION();                                               \
                                                                                    \
        if ( event_gen_active )                                                     \
        {                                                                           \
            SCOREP_ExitRegion( scorep_shmem_region__ ## FUNCNAME );                 \
                                                                                    \
            SCOREP_SHMEM_EVENT_GEN_ON();                                            \
        }                                                                           \
                                                                                    \
        SCOREP_IN_MEASUREMENT_DECREMENT();                                          \
    }

/* *INDENT-ON* */

#if SHMEM_HAVE_DECL( SHMEM_TEAM_ALLTOALLV_PACKED )
TEAM_ALLTOALLV_PACKED( shmem_team_alltoallv_packed )
#endif
