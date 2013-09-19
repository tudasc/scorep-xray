/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2013,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2013,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2013,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2013,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */


/**
 * @file
 *
 *
 *
 */

#include <config.h>
#include "scorep_thread.h"

#include <omp.h>
#include <SCOREP_Memory.h>
#include <SCOREP_Timing.h>
#include "scorep_location.h"
#include <SCOREP_RuntimeManagement.h>
#include <definitions/SCOREP_Definitions.h>
#include <SCOREP_Location.h>

#include <UTILS_Error.h>

#include <string.h>
#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>

#include "paradigm/omp/scorep_omp_thread_teams.h"

/* The following define is needed to please the PGI compiler */
#define PRAGMA_OP( x ) _Pragma( x )
#define SCOREP_PRAGMA( pragma ) PRAGMA_OP( UTILS_STRINGIFY( pragma ) )
#define SCOREP_PRAGMA_OMP( omp_pragma ) SCOREP_PRAGMA( omp omp_pragma )

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

static scorep_thread_private_data * initial_tpd;
static SCOREP_Location*  initial_location;
static SCOREP_Location** first_fork_locations;

/* *INDENT-OFF* */
static void set_tpd_to( scorep_thread_private_data* newTpd );
static void create_location_name( char* locationName, int locationNameMaxLength, int threadId, scorep_thread_private_data* parentTpd );
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

    /** Holdes the reference to the current thread team */
    SCOREP_InterimCommunicatorHandle thread_team;
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
    UTILS_BUG_ON( omp_in_parallel(), "" );
    UTILS_BUG_ON( initialTpd == 0, "" );
    UTILS_BUG_ON( scorep_thread_get_model_data( initialTpd ) == 0, "" );
    UTILS_BUG_ON( initial_tpd != 0, "" );
    UTILS_BUG_ON( initial_location != 0, "" );
    UTILS_BUG_ON( first_fork_locations != 0, "" );

    set_tpd_to( initialTpd );
    initial_tpd      = initialTpd;
    initial_location = scorep_thread_get_location( initialTpd );
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
    UTILS_BUG_ON( model_data->parent_reuse_count != 0, "" );
    UTILS_BUG_ON( model_data->fork_sequence_count != 0, "" );
    initial_tpd      = 0;
    initial_location = 0;
    free( first_fork_locations );
    first_fork_locations = 0;
}


void
scorep_thread_on_fork( uint32_t           nRequestedThreads,
                       uint32_t           forkSequenceCount,
                       SCOREP_ThreadModel model,
                       void*              modelData,
                       SCOREP_Location*   location )
{
    UTILS_BUG_ON( model != SCOREP_THREAD_MODEL_OPENMP, "" );
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
                             uint32_t*                    forkSequenceCount,
                             SCOREP_ThreadModel           model )
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

    UTILS_BUG_ON( TPD == 0, "" );
    UTILS_BUG_ON( model != SCOREP_THREAD_MODEL_OPENMP, "" );
    UTILS_BUG_ON( *forkSequenceCount != SCOREP_THREAD_INVALID_FORK_SEQUENCE_COUNT, "" );

    *parentTpd = TPD;
    scorep_thread_private_data_omp_tpd* parent_model_data =
        scorep_thread_get_model_data( *parentTpd );
    scorep_thread_private_data_omp_tpd* current_model_data = NULL;
    *forkSequenceCount = parent_model_data->tmp_fork_sequence_count;

    int num_threads = omp_get_num_threads();
    int thread_num  = omp_get_thread_num();
    if ( num_threads == 1 )
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
                scorep_thread_get_location( *parentTpd ),
                ( *capacity ) * sizeof( uint32_t ) );
        }
        else if ( *capacity < reuse_count )
        {
            /* ... realloc by doubling capacity, old memory is lost. */
            *capacity *= 2;
            uint32_t* tmp_sequence_counts = SCOREP_Location_AllocForMisc(
                scorep_thread_get_location( *parentTpd ),
                ( *capacity ) * sizeof( uint32_t ) );
            memcpy( tmp_sequence_counts,
                    parent_model_data->fork_sequence_counts,
                    ( reuse_count - 1 ) * sizeof( uint32_t ) );
            parent_model_data->fork_sequence_counts = tmp_sequence_counts;
        }
        UTILS_ASSERT( reuse_count <= *capacity );
        parent_model_data->fork_sequence_counts[ reuse_count - 1 ] = *forkSequenceCount;

        *currentTpd        = *parentTpd;
        current_model_data = parent_model_data;
    }
    else
    {
        bool location_is_created = false;
        /* Set TPD to a child of itself, create new one if necessary */
        UTILS_BUG_ON( thread_num >= parent_model_data->n_children, "" );

        *currentTpd = parent_model_data->children[ thread_num ];

        if ( *forkSequenceCount == 1 )
        {
            #pragma omp critical ( first_fork_locations )
            {
                if ( !first_fork_locations )
                {
                    uint32_t thread_team_size = omp_get_num_threads();
                    first_fork_locations = malloc( sizeof( SCOREP_Location* ) * ( thread_team_size - 1 ) );
                    char location_name[ 80 ];
                    for ( int i = 0; i < thread_team_size - 1; ++i )
                    {
                        create_location_name( location_name, 80, i + 1, *parentTpd );
                        first_fork_locations[ i ] = SCOREP_Location_CreateCPULocation( scorep_thread_get_location( *parentTpd ),
                                                                                       location_name,
                                                                                       /* deferNewLocationNotification = */ true );
                    }
                }
            }
        }

        if ( *currentTpd != 0 )
        {
            /* Previously been in this thread. */
            current_model_data = scorep_thread_get_model_data( *currentTpd );
            UTILS_BUG_ON( current_model_data->fork_sequence_count >= *forkSequenceCount, "" );
            current_model_data->fork_sequence_count = *forkSequenceCount;
            SCOREP_THREAD_ASSERT_TIMESTAMPS_IN_ORDER( scorep_thread_get_location( *currentTpd ) );
        }
        else
        {
            /* Never been here before. */
            *currentTpd =
                scorep_thread_create_private_data( *parentTpd, *forkSequenceCount );
            parent_model_data->children[ thread_num ] = *currentTpd;
            current_model_data                        =
                scorep_thread_get_model_data( *currentTpd );

            if ( thread_num == 0 )
            {
                /* for the master, reuse parents location data. */
                scorep_thread_set_location( *currentTpd, scorep_thread_get_location( *parentTpd ) );

                SCOREP_THREAD_ASSERT_TIMESTAMPS_IN_ORDER( scorep_thread_get_location( *currentTpd ) );
            }
            else
            {
                if ( *forkSequenceCount == 1 )
                {
                    /* For the first fork, use locations created in order corresponding to threadId. */
                    UTILS_ASSERT( first_fork_locations[ thread_num - 1 ] );
                    scorep_thread_set_location( *currentTpd, first_fork_locations[ thread_num - 1 ] );
                }
                else
                {
                    /* For nested or when a fork created more threads than the first fork,
                     * create locations on a first comes, first served basis. */
                    char location_name[ 80 ];
                    create_location_name( location_name, 80, thread_num, *parentTpd );
                    scorep_thread_set_location( *currentTpd,
                                                SCOREP_Location_CreateCPULocation( scorep_thread_get_location( *parentTpd ),
                                                                                   location_name,
                                                                                   /* deferNewLocationNotification = */ true ) );
                }
                /* We need to assign *currentTpd to the TPD first, before we
                 * can notify about the new location. */
                location_is_created = true;

                SCOREP_THREAD_ASSERT_TIMESTAMPS_IN_ORDER( scorep_thread_get_location( *currentTpd ) );
            }
        }

        set_tpd_to( *currentTpd );

        if ( location_is_created )
        {
            SCOREP_Location_CallSubstratesOnNewLocation(
                scorep_thread_get_location( *currentTpd ),
                0,
                scorep_thread_get_location( *parentTpd ) );
        }
    }

    /* handles recursion into the same singleton thread-team */
    current_model_data->thread_team = scorep_omp_get_thread_team_handle(
        scorep_thread_get_location( *currentTpd ),
        parent_model_data->thread_team,
        num_threads,
        thread_num );
}


SCOREP_InterimCommunicatorHandle
scorep_thread_get_thread_team( scorep_thread_private_data* tpd )
{
    scorep_thread_private_data_omp_tpd* model_data =
        scorep_thread_get_model_data( tpd );
    return model_data->thread_team;
}


static void
create_location_name( char*                       locationName,
                      int                         locationNameMaxLength,
                      int                         threadId,
                      scorep_thread_private_data* parentTpd )
{
    int                         length;
    scorep_thread_private_data* tpd             = scorep_thread_get_parent( parentTpd );
    SCOREP_Location*            parent_location = scorep_thread_get_location( parentTpd );
    if ( !tpd )
    {
        /* First fork created less threads than current fork. */
        length = snprintf( locationName, locationNameMaxLength, "OMP thread %d", threadId );
        UTILS_ASSERT( length < locationNameMaxLength );
        return;
    }
    /* Nesting */
    else if ( parent_location == initial_location )
    {
        /* Children of master */
        length = 12;
        strncpy( locationName, "OMP thread 0", length + 1 );
        while ( tpd && tpd != initial_tpd )
        {
            length += 2;
            UTILS_ASSERT( length < locationNameMaxLength );
            strncat( locationName, ":0", 2 );
            tpd = scorep_thread_get_parent( tpd );
        }
    }
    else
    {
        /* Children of non-master */
        SCOREP_StringDef* parent_name =
            SCOREP_LOCAL_HANDLE_DEREF( SCOREP_LOCAL_HANDLE_DEREF( SCOREP_Location_GetLocationHandle( parent_location ),
                                                                  Location )->name_handle, String );
        length = parent_name->string_length;
        strncpy( locationName, parent_name->string_data, length + 1 );
        while ( tpd && scorep_thread_get_location( tpd ) == parent_location )
        {
            length += 2;
            UTILS_ASSERT( length < locationNameMaxLength );
            strncat( locationName, ":0", 2 );
            tpd = scorep_thread_get_parent( tpd );
        }
    }
    length = snprintf( locationName + length, locationNameMaxLength - length, ":%d", threadId );
    UTILS_ASSERT( length < locationNameMaxLength );
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
    UTILS_BUG_ON( currentTpd != TPD, "" );
    UTILS_BUG_ON( model != SCOREP_THREAD_MODEL_OPENMP, "" );

    scorep_thread_private_data_omp_tpd* model_data = scorep_thread_get_model_data( currentTpd );

    if ( omp_get_num_threads() == 1 )
    {
        /* There was no additional parallelism in this parallel
         * region. We reused the parent tpd. */
        *parentTpd = currentTpd;
        UTILS_BUG_ON( model_data->parent_reuse_count == 0, "" );
        *forkSequenceCount = model_data->fork_sequence_counts[ model_data->parent_reuse_count - 1 ];
    }
    else
    {
        *parentTpd = scorep_thread_get_parent( currentTpd );
        UTILS_BUG_ON( model_data->parent_reuse_count != 0, "" );
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
    UTILS_BUG_ON( currentTpd != TPD, "" );
    UTILS_BUG_ON( model != SCOREP_THREAD_MODEL_OPENMP, "" );

    scorep_thread_private_data_omp_tpd* model_data = scorep_thread_get_model_data( currentTpd );
    scorep_thread_private_data_omp_tpd* than_active_model_data;

    if ( model_data->parent_reuse_count != 0 )
    {
        /* There was no additional parallelism in the previous
         * parallel region. We reused the parent tpd. */
        *forkSequenceCount = model_data->fork_sequence_counts[ model_data->parent_reuse_count - 1 ];
        model_data->parent_reuse_count--;
        *tpdFromNowOn          = currentTpd;
        than_active_model_data = model_data;
    }
    else
    {
        *forkSequenceCount = model_data->fork_sequence_count;
        UTILS_BUG_ON( parentTpd == 0, "" );
        set_tpd_to( parentTpd );
        *tpdFromNowOn          = parentTpd;
        than_active_model_data = scorep_thread_get_model_data( parentTpd );
    }
    than_active_model_data->thread_team =
        scorep_omp_get_parent_thread_team_handle( model_data->thread_team );
}


SCOREP_Location*
SCOREP_Location_GetCurrentCPULocation()
{
    UTILS_BUG_ON( TPD == 0, "" );
    SCOREP_Location* location = scorep_thread_get_location( TPD );
    UTILS_BUG_ON( location == 0, "" );
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


bool
SCOREP_Thread_InParallel()
{
    return omp_in_parallel();
}
