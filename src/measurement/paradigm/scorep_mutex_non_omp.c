/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
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
 * @file       src/measurement/paradigm/scorep_lock_non_omp.c
 * @maintainer Bert Wesarg <bert.wesarg@tu-dresden.de>
 *
 * @status alpha
 *
 * Provides a lock implemention for non-OpenMP programs.
 * I.e no locking at all.
 *
 *
 */


#include <config.h>


#include <stdlib.h>


#include <SCOREP_Mutex.h>


#include <UTILS_Error.h>


typedef struct scorep_mutex
{
    int state;
} scorep_mutex;


enum
{
    SCOREP_MUTEXT_MARKER_UNLOCKED,
    SCOREP_MUTEXT_MARKER_LOCKED
};


/**
 * Emtpy stub for empty lock.
 *
 */
SCOREP_ErrorCode
SCOREP_MutexCreate( SCOREP_Mutex* scorepMutex )
{
    if ( !scorepMutex )
    {
        return UTILS_ERROR( SCOREP_ERROR_INVALID_ARGUMENT,
                            "Invalid mutex handle given." );
    }

    scorep_mutex** mutex = ( scorep_mutex** )scorepMutex;
    *mutex = malloc( sizeof( **mutex ) );
    if ( !mutex )
    {
        return UTILS_ERROR_POSIX( "Can't allocate lock object" );
    }

    /* mark this lock initialized */
    ( *mutex )->state = SCOREP_MUTEXT_MARKER_UNLOCKED;

    return SCOREP_SUCCESS;
}


/**
 * Emtpy stub for empty lock.
 *
 */
SCOREP_ErrorCode
SCOREP_MutexDestroy( SCOREP_Mutex* scorepMutex )
{
    if ( !scorepMutex )
    {
        return UTILS_ERROR( SCOREP_ERROR_INVALID_ARGUMENT,
                            "Invalid mutex handle given." );
    }

    scorep_mutex** mutex = ( scorep_mutex** )scorepMutex;
    if ( !*mutex )
    {
        /* NULL pointers are allowed. */
        return SCOREP_SUCCESS;
    }

    if ( ( *mutex )->state != SCOREP_MUTEXT_MARKER_UNLOCKED )
    {
        return UTILS_ERROR( SCOREP_ERROR_INVALID_ARGUMENT, "Trying to destroy an locked mutex." );
    }

    free( *mutex );
    *mutex = NULL;

    return SCOREP_SUCCESS;
}


/**
 * Emtpy stub for empty lock.
 *
 */
SCOREP_ErrorCode
SCOREP_MutexLock( SCOREP_Mutex scorepMutex )
{
    if ( !scorepMutex )
    {
        return UTILS_ERROR( SCOREP_ERROR_INVALID_ARGUMENT,
                            "Invalid mutex handle given." );
    }

    scorep_mutex* mutex = ( scorep_mutex* )scorepMutex;

    UTILS_BUG_ON( mutex->state == SCOREP_MUTEXT_MARKER_LOCKED,
                  "Trying to lock an already locked mutex." );

    mutex->state = SCOREP_MUTEXT_MARKER_LOCKED;

    return SCOREP_SUCCESS;
}


/**
 * Emtpy stub for empty lock.
 *
 */
SCOREP_ErrorCode
SCOREP_MutexUnlock( SCOREP_Mutex scorepMutex )
{
    if ( !scorepMutex )
    {
        return UTILS_ERROR( SCOREP_ERROR_INVALID_ARGUMENT,
                            "Invalid mutex handle given." );
    }

    scorep_mutex* mutex = ( scorep_mutex* )scorepMutex;

    UTILS_BUG_ON( mutex->state == SCOREP_MUTEXT_MARKER_UNLOCKED,
                  "Trying to unlock an non-locked mutex." );

    mutex->state = SCOREP_MUTEXT_MARKER_UNLOCKED;

    return SCOREP_SUCCESS;
}
