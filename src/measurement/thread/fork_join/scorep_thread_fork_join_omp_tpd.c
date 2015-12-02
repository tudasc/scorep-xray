/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2015,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2015,
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
 */

#include <config.h>
#include <scorep_thread_generic.h>
#include <scorep_thread_model_specific.h>
#include <scorep_thread_fork_join_model_specific.h>

#include <SCOREP_Timer_Ticks.h>
#include <scorep_location.h>
#include <SCOREP_RuntimeManagement.h>
#include <SCOREP_Definitions.h>
#include <SCOREP_Types.h>

#include <UTILS_Error.h>

#include <omp.h>
#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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


typedef struct scorep_thread_private_data scorep_thread_private_data;

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
    uint32_t parent_reuse_count;             /**< Helps us to find the correct fork sequence count in join if we had
                                              * previous parallel regions without additional parallelism. */
};


size_t
scorep_thread_get_sizeof_model_data( void )
{
    return sizeof( scorep_thread_private_data_omp_tpd );
}


void
scorep_thread_on_create_private_data( scorep_thread_private_data* tpd,
                                      void*                       modelData )
{
}


void
scorep_thread_on_initialize( scorep_thread_private_data* initialTpd )
{
    UTILS_BUG_ON( initialTpd == 0, "" );
    UTILS_BUG_ON( scorep_thread_get_model_data( initialTpd ) == 0, "" );

    scorep_thread_create_first_fork_locations_mutex();

    set_tpd_to( initialTpd );
    UTILS_BUG_ON( TPD == 0, "" );
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
    scorep_thread_destroy_first_fork_locations_mutex();
}


void
scorep_thread_on_fork( uint32_t            nRequestedThreads,
                       SCOREP_ParadigmType paradigm,
                       void*               modelData,
                       SCOREP_Location*    location )
{
    UTILS_BUG_ON( paradigm != SCOREP_PARADIGM_OPENMP, "" );
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
}


/* *INDENT-OFF* */
#define SCOREP_THREAD_ASSERT_TIMESTAMPS_IN_ORDER( location ) \
    do \
    { \
        uint64_t current_timestamp = SCOREP_Timer_GetClockTicks(); \
        UTILS_BUG_ON( SCOREP_Location_GetLastTimestamp( location ) > current_timestamp, \
                      "Wrong timestamp order: %" PRIu64 " (last recorded) > %" PRIu64 " (current).", \
                      SCOREP_Location_GetLastTimestamp( location ), current_timestamp ); \
    } \
    while ( 0 )
/* *INDENT-ON* */


scorep_thread_private_data*
scorep_thread_on_team_begin_get_parent( void )
{
    /* here we rely on the 'copyin(pomp_tpd)' OPARI2 instrumentation,
     * i.e., the parent is copied into the parallel region.. */
    UTILS_BUG_ON( TPD == 0, "TPD was copied in, so it must not be NULL." );
    return TPD;
}


void
scorep_thread_on_team_begin( scorep_thread_private_data*  parentTpd,
                             scorep_thread_private_data** currentTpd,
                             SCOREP_ParadigmType          paradigm,
                             int*                         threadId,
                             SCOREP_Location**            firstForkLocations,
                             bool*                        locationIsCreated )
{
    *threadId = omp_get_thread_num();
    /* Begin of portability-hack:
     * OpenMP implementations on XL/AIX use the atexit mechanism to
     * shut-down the OpenMP runtime. The atexit handler is registered
     * during the first usage of OpenMP, usually after the Score-P at_exit
     * handler. I.e. the OpenMP runtime is shut down *before* the Score-P
     * finalization, preventing Score-P from accessing e.g. OpenMP
     * threadprivate variables. To solve this issue we re-register the
     * Score-P atexit handler so that it is executed *before* the OpenMP
     * runtime is shut down. */
    static bool exit_handler_re_registered = false;
    if ( *threadId == 0 &&
         scorep_thread_is_initial_thread( parentTpd ) &&
         !exit_handler_re_registered )
    {
        exit_handler_re_registered = true;
        SCOREP_RegisterExitHandler();
    }
    /* End of portability-hack */

    UTILS_BUG_ON( paradigm != SCOREP_PARADIGM_OPENMP, "" );

    scorep_thread_private_data_omp_tpd* parent_model_data =
        scorep_thread_get_model_data( parentTpd );

    if ( omp_get_num_threads() == 1 )
    {
        /* There is no additional parallelism in this parallel
         * region. Reuse the parent tpd (to gracefully handle recursion e.g.). */
        ++( parent_model_data->parent_reuse_count );
        *currentTpd = parentTpd;
    }
    else
    {
        *locationIsCreated = false;

        /* Set TPD to a child of itself, create new one if necessary */
        UTILS_BUG_ON( *threadId >= parent_model_data->n_children,
                      "More threads created than requested: %u >= %u",
                      *threadId, parent_model_data->n_children );

        *currentTpd = parent_model_data->children[ *threadId ];

        if ( *currentTpd == 0 )
        {
            /* Never been here before. */

            SCOREP_Location* location;

            if ( *threadId == 0 )
            {
                /* for the master, reuse parents location data. */
                location = scorep_thread_get_location( parentTpd );
            }
            else
            {
                if ( firstForkLocations )
                {
                    /* For the first fork, use locations created in order corresponding to threadId. */
                    UTILS_ASSERT( firstForkLocations[ *threadId - 1 ] );
                    location = firstForkLocations[ *threadId - 1 ];
                }
                else
                {
                    /* For nested or when a fork created more threads than the first fork,
                     * create locations on a first comes, first served basis. */
                    char location_name[ 80 ];
                    scorep_thread_create_location_name( location_name, 80, *threadId, parentTpd );
                    location = SCOREP_Location_CreateCPULocation( location_name );
                }
                /* We need to assign *currentTpd to the TPD first, before we
                 * can notify about the new location. */
                *locationIsCreated = true;
            }

            *currentTpd =
                scorep_thread_create_private_data( parentTpd, location );
            scorep_thread_set_location( *currentTpd, location );
            parent_model_data->children[ *threadId ] = *currentTpd;
        }

        set_tpd_to( *currentTpd );
    }

    SCOREP_THREAD_ASSERT_TIMESTAMPS_IN_ORDER( scorep_thread_get_location( *currentTpd ) );
}


void
scorep_thread_create_location_name( char*                       locationName,
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
    else if ( parent_location == scorep_thread_get_location( scorep_thread_get_initial_tpd() ) )
    {
        /* Children of master */
        length = 12;
        strncpy( locationName, "OMP thread 0", length + 1 );
        while ( tpd && !scorep_thread_is_initial_thread( tpd ) )
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
        const char* parent_name = SCOREP_Location_GetName( parent_location );
        length = strlen( parent_name );
        strncpy( locationName, parent_name, length + 1 );
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
scorep_thread_get_private_data( void )
{
    return TPD;
}


void
scorep_thread_on_team_end( scorep_thread_private_data*  currentTpd,
                           scorep_thread_private_data** parentTpd,
                           int*                         threadId,
                           SCOREP_ParadigmType          paradigm )
{
    UTILS_BUG_ON( currentTpd != TPD, "" );
    UTILS_BUG_ON( paradigm != SCOREP_PARADIGM_OPENMP, "" );

    scorep_thread_private_data_omp_tpd* model_data = scorep_thread_get_model_data( currentTpd );

    *threadId = omp_get_thread_num();
    if ( omp_get_num_threads() == 1 )
    {
        /* There was no additional parallelism in this parallel
         * region. We reused the parent tpd. */
        *parentTpd = currentTpd;
        UTILS_BUG_ON( model_data->parent_reuse_count == 0, "" );
    }
    else
    {
        *parentTpd = scorep_thread_get_parent( currentTpd );
        UTILS_BUG_ON( model_data->parent_reuse_count != 0, "" );
    }
}


void
scorep_thread_on_join( scorep_thread_private_data*  currentTpd,
                       scorep_thread_private_data*  parentTpd,
                       scorep_thread_private_data** tpdFromNowOn,
                       SCOREP_ParadigmType          paradigm )
{
    UTILS_BUG_ON( currentTpd != TPD, "" );
    UTILS_BUG_ON( paradigm != SCOREP_PARADIGM_OPENMP, "" );

    scorep_thread_private_data_omp_tpd* model_data = scorep_thread_get_model_data( currentTpd );

    if ( model_data->parent_reuse_count != 0 )
    {
        /* There was no additional parallelism in the previous
         * parallel region. We reused the parent tpd. */
        model_data->parent_reuse_count--;
        *tpdFromNowOn = currentTpd;
    }
    else
    {
        UTILS_BUG_ON( parentTpd == 0, "" );
        set_tpd_to( parentTpd );
        *tpdFromNowOn = parentTpd;
    }
}


SCOREP_Location*
SCOREP_Location_GetCurrentCPULocation( void )
{
    UTILS_BUG_ON( TPD == 0, "Invalid OpenMP thread specific data object. "
                  "Please ensure that all omp parallel regions are instrumented." );
    SCOREP_Location* location = scorep_thread_get_location( TPD );
    UTILS_BUG_ON( location == 0, "Invalid location object associated with "
                  "OpenMP thread specific data object." );
    return location;
}


void
scorep_thread_delete_private_data( scorep_thread_private_data* tpd )
{
}


bool
SCOREP_Thread_InParallel( void )
{
    return omp_in_parallel();
}


uint32_t
scorep_thread_get_team_size( void )
{
    return omp_get_num_threads();
}


SCOREP_ParadigmType
scorep_thread_get_paradigm( void )
{
    return SCOREP_PARADIGM_OPENMP;
}
