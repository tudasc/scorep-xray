/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2011,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2011, 2016,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2011,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2011,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2011,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2011,
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
 * Provides a lock implemention for non-OpenMP programs.
 * I.e no locking at all.
 *
 *
 */


#include <config.h>
#include <SCOREP_Mutex.h>

#include <stdlib.h>

#include <UTILS_Error.h>


typedef struct scorep_mutex
{
    int state;
} scorep_mutex;


enum
{
    SCOREP_MUTEX_MARKER_UNLOCKED,
    SCOREP_MUTEX_MARKER_LOCKED
};


/**
 * Emtpy stub for empty lock.
 *
 */
SCOREP_ErrorCode
SCOREP_MutexCreate( SCOREP_Mutex* scorepMutex )
{
#if HAVE( SCOREP_DEBUG )
    if ( !scorepMutex )
    {
        return UTILS_ERROR( SCOREP_ERROR_INVALID_ARGUMENT,
                            "Invalid mutex handle given." );
    }

    scorep_mutex** mutex = ( scorep_mutex** )scorepMutex;
    *mutex = malloc( sizeof( **mutex ) );
    if ( !*mutex )
    {
        return UTILS_ERROR_POSIX( "Can't allocate lock object" );
    }

    /* mark this lock initialized */
    ( *mutex )->state = SCOREP_MUTEX_MARKER_UNLOCKED;
#endif

    return SCOREP_SUCCESS;
}


/**
 * Emtpy stub for empty lock.
 *
 */
SCOREP_ErrorCode
SCOREP_MutexDestroy( SCOREP_Mutex* scorepMutex )
{
#if HAVE( SCOREP_DEBUG )
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

    UTILS_BUG_ON( ( *mutex )->state == SCOREP_MUTEX_MARKER_LOCKED,
                  "Trying to destroy an locked mutex." );

    free( *mutex );
#endif

    *scorepMutex = SCOREP_INVALID_MUTEX;

    return SCOREP_SUCCESS;
}


/**
 * Emtpy stub for empty lock.
 *
 */
SCOREP_ErrorCode
SCOREP_MutexLock( SCOREP_Mutex scorepMutex )
{
#if HAVE( SCOREP_DEBUG )
    if ( !scorepMutex )
    {
        return UTILS_ERROR( SCOREP_ERROR_INVALID_ARGUMENT,
                            "Invalid mutex handle given." );
    }

    scorep_mutex* mutex = ( scorep_mutex* )scorepMutex;

    UTILS_BUG_ON( mutex->state == SCOREP_MUTEX_MARKER_LOCKED,
                  "Trying to lock an already locked mutex." );

    mutex->state = SCOREP_MUTEX_MARKER_LOCKED;
#endif

    return SCOREP_SUCCESS;
}


/**
 * Emtpy stub for empty lock.
 *
 */
SCOREP_ErrorCode
SCOREP_MutexUnlock( SCOREP_Mutex scorepMutex )
{
#if HAVE( SCOREP_DEBUG )
    if ( !scorepMutex )
    {
        return UTILS_ERROR( SCOREP_ERROR_INVALID_ARGUMENT,
                            "Invalid mutex handle given." );
    }

    scorep_mutex* mutex = ( scorep_mutex* )scorepMutex;

    UTILS_BUG_ON( mutex->state == SCOREP_MUTEX_MARKER_UNLOCKED,
                  "Trying to unlock an non-locked mutex." );

    mutex->state = SCOREP_MUTEX_MARKER_UNLOCKED;
#endif

    return SCOREP_SUCCESS;
}
