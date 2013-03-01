/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 *    RWTH Aachen University, Germany
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *    Technische Universitaet Dresden, Germany
 *    University of Oregon, Eugene, USA
 *    Forschungszentrum Juelich GmbH, Germany
 *    German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *    Technische Universitaet Muenchen, Germany
 *
 * See the COPYING file in the package base directory for details.
 *
 */


/**
 * @file       src/measurement/scorep_thread_omp_tpd.c
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status alpha
 *
 *
 */

#include <config.h>
#include "scorep_thread.h"

#include <SCOREP_Omp.h>
#include <SCOREP_Memory.h>
#include <SCOREP_Timing.h>
#include "scorep_location.h"
#include <SCOREP_RuntimeManagement.h>

#include <UTILS_Error.h>

#include <string.h>
#include <inttypes.h>
#include <stdlib.h>


/* Take care of name mangling the Fortran compiler might perform. */
#define POMP_TPD_MANGLED FORTRAN_MANGLED( pomp_tpd )

/* The thread private variable that points to a
 * scorep_thread_private_data object. It needs to be a 64bit integer
 * to smoothly work with OPARI2 instrumented Fortran code. It will be
 * casted to scorep_thread_private_data* before usage, see the macro
 * TPD. Dependent on the compiler it will be pomp_tdp or pomp_tpd_,
 * see config.h and the FORTRAN_MANGLED macro. */
int64_t FORTRAN_ALIGNED POMP_TPD_MANGLED = 0;

/* We want to write #pragma omp threadprivate(POMP_TPD_MANGLED) here
 * but as POMP_TPD_MANGLED is a macro itself, we need to do some
 * preprocessor magic to be portable. */
SCOREP_PRAGMA_OMP( threadprivate( POMP_TPD_MANGLED ) )


/* Use TPD for easy access to the scorep_thread_private_data object. */
#if SIZEOF_VOID_P == 8
#define TPD ( ( scorep_thread_private_data* )POMP_TPD_MANGLED )
#elif SIZEOF_VOID_P == 4
#define TPD ( ( scorep_thread_private_data* )( uint32_t )POMP_TPD_MANGLED )
#else
#error Unsupported architecture. Only 32 bit and 64 bit architectures are supported.
#endif


/* *INDENT-OFF* */
static void set_tpd_to( scorep_thread_private_data* newTpd );
/* *INDENT-ON* */


typedef struct scorep_thread_private_data_omp_tpd scorep_thread_private_data_omp_tpd;
struct scorep_thread_private_data_omp_tpd
{
    scorep_thread_private_data** children;   /**< Children array, gets created/reallocated in subsequent
                                              * fork event. Children objects are created in
                                              * SCOREP_Thread_Begin(). */
    uint32_t n_children;                     /**< Size of the children array, initially 0. */
    uint32_t fork_sequence_count;            /**< Fork sequence count that corresponds to the
                                              * creation or re-usage of this thread.  The value
                                              * changes in each ThreadBegin if there is
                                              * additional parallelism, otherwise the
                                              * fork_sequence_counts stack is used. */
    uint32_t  parent_reuse_count;            /**< Helps us to find the correct fork sequence count in join if we had
                                              * previous parallel regions without additional parallelism. */
    uint32_t* fork_sequence_counts;          /**< Array to hold a stack of sequence counts in case where a
                                              * parallel region does not contribute to additional parallelism.  */
    uint32_t  fork_sequence_counts_capacity; /**< current capacity of fork_sequence_counts */
    uint32_t  tmp_fork_sequence_count;       /**< Used to transfer the sequence count from SCOREP_Thread_Fork()
                                              * to SCOREP_Thread_Begin(). */
};


size_t
scorep_thread_get_sizeof_model_data()
{
    return sizeof( scorep_thread_private_data_omp_tpd );
}


void
scorep_thread_on_create_private_data( scorep_thread_private_data* tpd,
                                      void*                       modelData,
                                      uint32_t                    forkSequenceCount )
{
    ( ( scorep_thread_private_data_omp_tpd* )modelData )->fork_sequence_count = forkSequenceCount;
}


void
scorep_thread_on_initialize( scorep_thread_private_data* initialTpd )
{
    UTILS_BUG_ON( omp_in_parallel() );
    UTILS_BUG_ON( initialTpd == 0 );
    UTILS_BUG_ON( scorep_thread_get_model_data( initialTpd ) == 0 );

    set_tpd_to( initialTpd );
    /* From here on it is save to call SCOREP_Location_GetCurrentCPULocation(). */
}


static void
set_tpd_to( scorep_thread_private_data* newTpd )
{
#if SIZEOF_VOID_P == 8
    POMP_TPD_MANGLED = ( uint64_t )newTpd;
#elif SIZEOF_VOID_P == 4
    POMP_TPD_MANGLED = ( uint32_t )newTpd;
#else
#error Unsupported architecture. Only 32 bit and 64 bit architectures are supported.
#endif
}


void
scorep_thread_on_finalize( scorep_thread_private_data* tpd )
{
    scorep_thread_private_data_omp_tpd* model_data = scorep_thread_get_model_data( tpd );
    UTILS_BUG_ON( model_data->parent_reuse_count != 0 );
    UTILS_BUG_ON( model_data->fork_sequence_count != 0 );
}


void
scorep_thread_on_fork( uint32_t           nRequestedThreads,
                       uint32_t           forkSequenceCount,
                       SCOREP_ThreadModel model,
                       void*              modelData,
                       SCOREP_Location*   location )
{
    UTILS_BUG_ON( model != SCOREP_THREAD_MODEL_OPENMP );
    scorep_thread_private_data_omp_tpd* model_data = ( scorep_thread_private_data_omp_tpd* )modelData;

    /* Create or realloc children array. */
    if ( model_data->children == 0 || model_data->n_children < nRequestedThreads )
    {
        /* @todo If we restrict OpenMP nesting levels
         * (e.g. OMP_NUM_THREADS=m,n,l (3.1 syntax)) then we might assert on the
         * n_children < nRequestedThreads condition. */

        /* Realloc children array, old memory is lost. */
        scorep_thread_private_data** tmp_children = model_data->children;
        model_data->children =
            SCOREP_Location_AllocForMisc( location, nRequestedThreads * sizeof( scorep_thread_private_data* ) );
        memcpy( model_data->children,
                tmp_children,
                model_data->n_children * sizeof( scorep_thread_private_data* ) );
        memset( &( model_data->children[ model_data->n_children ] ),
                0,
                ( nRequestedThreads - model_data->n_children ) * sizeof( scorep_thread_private_data* ) );

        model_data->n_children = nRequestedThreads;
    }
    model_data->tmp_fork_sequence_count = forkSequenceCount;
}


/* *INDENT-OFF* */
#define SCOREP_THREAD_ASSERT_TIMESTAMPS_IN_ORDER( location ) \
    do \
    { \
        uint64_t current_timestamp = SCOREP_GetClockTicks(); \
        UTILS_BUG_ON( SCOREP_Location_GetLastTimestamp( location ) > current_timestamp, \
                      "Wrong timestamp order: %" PRIu64 " (last recorded) > %" PRIu64 " (current).", \
                      SCOREP_Location_GetLastTimestamp( location ), current_timestamp ); \
    } \
    while ( 0 )
/* *INDENT-ON* */


void
scorep_thread_on_team_begin( scorep_thread_private_data** parentTpd,
                             scorep_thread_private_data** currentTpd,
                             char*                        currentName,
                             uint32_t*                    forkSequenceCount,
                             SCOREP_ThreadModel           model,
                             bool*                        locationIsCreated /* defaults to false */ )
{
    /* Begin of portability-hack:
     * OpenMP implementations on XL/AIX use the at_exit mechanism to
     * shut-down the OpenMP runtime. The at_exit handler is registered
     * during the first usage of OpenMP, usually after the Score-P at_exit
     * handler. I.e. the OpenMP runtime is shut down *before* the Score-P
     * finalization, preventing Score-P from accessing e.g. OpenMP
     * threadprivate variables. To solve this issue we re-register the
     * Score-P at_exit handler so that it is executed *before* the OpenMP
     * runtime is shut down. */
    static bool exit_handler_re_registered = false;
    if ( !exit_handler_re_registered && scorep_thread_is_initial_thread( TPD ) )
    {
        exit_handler_re_registered = true;
        SCOREP_RegisterExitHandler();
    }
    /* End of portability-hack */

    UTILS_BUG_ON( TPD == 0 );
    UTILS_BUG_ON( model != SCOREP_THREAD_MODEL_OPENMP );
    UTILS_BUG_ON( *forkSequenceCount != SCOREP_THREAD_INVALID_FORK_SEQUENCE_COUNT );
    UTILS_ASSERT( *locationIsCreated == false );

    *parentTpd = TPD;
    scorep_thread_private_data_omp_tpd* parent_model_data = scorep_thread_get_model_data( *parentTpd );
    *forkSequenceCount = parent_model_data->tmp_fork_sequence_count;

    if ( omp_get_num_threads() == 1 )
    {
        /* There is no additional parallelism in this parallel
         * region. Reuse the parent tpd (to gracefully handle recursion e.g.).
         * We need to track the fork sequence count manually using a stack. */
        const uint32_t reuse_count = ++( parent_model_data->parent_reuse_count );
        uint32_t*      capacity    = &( parent_model_data->fork_sequence_counts_capacity );
        if ( reuse_count == 1 )
        {
            /* Start with a stack of 4 and ... */
            *capacity                               = 4;
            parent_model_data->fork_sequence_counts = SCOREP_Location_AllocForMisc(
                scorep_thread_get_location( *parentTpd ),  ( *capacity ) * sizeof( uint32_t ) );
        }
        else if ( *capacity < reuse_count )
        {
            /* ... realloc by doubling capacity, old memory is lost. */
            *capacity                              *= 2;
            parent_model_data->fork_sequence_counts = SCOREP_Location_AllocForMisc(
                scorep_thread_get_location( *parentTpd ),  ( *capacity ) * sizeof( uint32_t ) );
            uint32_t* tmp_sequence_counts = parent_model_data->fork_sequence_counts;
            memcpy( parent_model_data->fork_sequence_counts,
                    tmp_sequence_counts,
                    ( reuse_count - 1 ) * sizeof( uint32_t ) );
        }
        UTILS_ASSERT( reuse_count <= *capacity );
        parent_model_data->fork_sequence_counts[ reuse_count - 1 ] = *forkSequenceCount;

        *currentTpd = *parentTpd;
    }
    else
    {
        /* Set TPD to a child of itself, create new one if necessary */
        const int current_thread_id = omp_get_thread_num();
        UTILS_BUG_ON( current_thread_id >= parent_model_data->n_children );

        *currentTpd = parent_model_data->children[ current_thread_id ];

        if ( *currentTpd != 0 )
        {
            /* Previously been in this thread. */
            scorep_thread_private_data_omp_tpd* current_model_data = scorep_thread_get_model_data( *currentTpd );
            UTILS_BUG_ON( current_model_data->fork_sequence_count >= *forkSequenceCount );
            current_model_data->fork_sequence_count = *forkSequenceCount;
            SCOREP_THREAD_ASSERT_TIMESTAMPS_IN_ORDER( scorep_thread_get_location( *currentTpd ) );
        }
        else
        {
            /* Never been here before. */
            *currentTpd = scorep_thread_create_private_data( *parentTpd, *forkSequenceCount );

            parent_model_data->children[ current_thread_id ] = *currentTpd;

            if ( current_thread_id == 0 )
            {
                /* for the master, reuse parents location data. */
                scorep_thread_set_location( *currentTpd, scorep_thread_get_location( *parentTpd ) );

                SCOREP_THREAD_ASSERT_TIMESTAMPS_IN_ORDER( scorep_thread_get_location( *currentTpd ) );
            }
            else
            {
                scorep_thread_set_location( *currentTpd,
                                            SCOREP_Location_CreateCPULocation( scorep_thread_get_location( *parentTpd ),
                                                                               currentName,
                                                                               /* deferNewLocationNotication = */ true ) );
                *locationIsCreated = true;

                SCOREP_THREAD_ASSERT_TIMESTAMPS_IN_ORDER( scorep_thread_get_location( *currentTpd ) );
            }
        }
        set_tpd_to( *currentTpd );
    }
}


scorep_thread_private_data*
scorep_thread_get_private_data()
{
    return TPD;
}


void
scorep_thread_on_end( scorep_thread_private_data*  currentTpd,
                      scorep_thread_private_data** parentTpd,
                      uint32_t*                    forkSequenceCount,
                      SCOREP_ThreadModel           model )
{
    UTILS_BUG_ON( currentTpd != TPD );
    UTILS_BUG_ON( model != SCOREP_THREAD_MODEL_OPENMP );

    scorep_thread_private_data_omp_tpd* model_data = scorep_thread_get_model_data( currentTpd );

    if ( omp_get_num_threads() == 1 )
    {
        /* There was no additional parallelism in this parallel
         * region. We reused the parent tpd. */
        *parentTpd = currentTpd;
        UTILS_BUG_ON( model_data->parent_reuse_count == 0 );
        *forkSequenceCount = model_data->fork_sequence_counts[ model_data->parent_reuse_count - 1 ];
    }
    else
    {
        *parentTpd = scorep_thread_get_parent( currentTpd );
        UTILS_BUG_ON( model_data->parent_reuse_count != 0 );
        *forkSequenceCount = model_data->fork_sequence_count;
    }
}


void
scorep_thread_on_join( scorep_thread_private_data*  currentTpd,
                       scorep_thread_private_data*  parentTpd,
                       scorep_thread_private_data** tpdFromNowOn,
                       uint32_t*                    forkSequenceCount,
                       SCOREP_ThreadModel           model )
{
    UTILS_BUG_ON( currentTpd != TPD );
    UTILS_BUG_ON( model != SCOREP_THREAD_MODEL_OPENMP );

    scorep_thread_private_data_omp_tpd* model_data = scorep_thread_get_model_data( currentTpd );

    if ( model_data->parent_reuse_count != 0 )
    {
        /* There was no additional parallelism in the previous
         * parallel region. We reused the parent tpd. */
        *forkSequenceCount = model_data->fork_sequence_counts[ model_data->parent_reuse_count - 1 ];
        model_data->parent_reuse_count--;
        *tpdFromNowOn = currentTpd;
    }
    else
    {
        *forkSequenceCount = model_data->fork_sequence_count;
        UTILS_BUG_ON( parentTpd == 0 );
        set_tpd_to( parentTpd );
        *tpdFromNowOn = parentTpd;
    }
}


SCOREP_Location*
SCOREP_Location_GetCurrentCPULocation()
{
    UTILS_BUG_ON( TPD == 0 );
    SCOREP_Location* location = scorep_thread_get_location( TPD );
    UTILS_BUG_ON( location == 0 );
    return location;
}


void
scorep_thread_delete_private_data( scorep_thread_private_data* tpd )
{
    UTILS_ASSERT( tpd != 0 );

    scorep_thread_private_data_omp_tpd* model_data = scorep_thread_get_model_data( tpd );

    for ( uint32_t i = 0; i < model_data->n_children; ++i )
    {
        if ( model_data->children[ i ] != 0 )
        {
            scorep_thread_delete_private_data( model_data->children[ i ] );
        }
    }
    free( tpd );
}
