/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2013,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2016,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */



/**
 * @file
 *
 ** Provides a spinlock implemention for programs using pthreads.
 *
 *
 */


#include <config.h>
#include <SCOREP_Mutex.h>


#include <stdlib.h>

#include <UTILS_Error.h>
#include <pthread.h>


/**
 * Allocates and initializes a new Pthread lock.
 *
 * @param scorepMutex Reference to a SCOREP_Mutex variable.
 *
 * @return SCOREP_SUCCESS for success, or the error code.
 *
 * @error SCOREP_ERROR_INVALID_ARGUMENT The given @a scorepMutex parameter is a
 *                                      @a NULL pointer.
 * @error scorep_errno Any errors from malloc(3) converted to an SCOREP error code.
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

    pthread_spinlock_t** lock = ( pthread_spinlock_t** )scorepMutex;

    *lock = malloc( sizeof( **lock ) );
    if ( !*lock )
    {
        return UTILS_ERROR_POSIX( "Can't allocate lock object" );
    }

    /* this call does not give us a success status */
    pthread_spin_init( *lock, PTHREAD_PROCESS_PRIVATE );
    return SCOREP_SUCCESS;
}

/**
 * Destroys and Deallocates a Pthread lock.
 *
 * @param scorepMutex Reference to a SCOREP_Mutex variable.
 *
 * @return SCOREP_SUCCESS for success, or the error code.
 *
 * @error SCOREP_ERROR_INVALID_ARGUMENT The given @a scorepMutex parameter is a
 *                                      @a NULL pointer.
 *
 */

SCOREP_ErrorCode
SCOREP_MutexDestroy( SCOREP_Mutex* scorepMutex )
{
    if ( !scorepMutex )
    {
        return UTILS_ERROR( SCOREP_ERROR_INVALID_ARGUMENT, "" );
    }

    pthread_spinlock_t** lock = ( pthread_spinlock_t** )scorepMutex;
    if ( !*lock )
    {
        return SCOREP_SUCCESS;
    }
    pthread_spin_destroy( *lock );
    free( ( void* )*lock );

    *scorepMutex = SCOREP_INVALID_MUTEX;

    return SCOREP_SUCCESS;
}

/**
 * Locks an SCOREP_Mutex by using the Pthread API.
 *
 * @param scorepMutex A SCOREP_Mutex variable, previously created with @a
 *                    SCOREP_MutexCreate.
 *
 * @return SCOREP_SUCCESS for success, or the error code.
 *
 * @error SCOREP_ERROR_INVALID_ARGUMENT The given @a scorepMutex parameter is a
 *                                      @a NULL pointer.
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

    pthread_spinlock_t* lock = ( pthread_spinlock_t* )scorepMutex;
    pthread_spin_lock( lock );

    return SCOREP_SUCCESS;
}

/**
 * Unlocks an SCOREP_Mutex by using the OpenMP API.
 *
 * @param scorepMutex A SCOREP_Mutex variable, previously created with @a
 *                    SCOREP_MutexCreate.
 *
 * @return SCOREP_SUCCESS for success, or the error code.
 *
 * @error SCOREP_ERROR_INVALID_ARGUMENT The given @a scorepMutex parameter is a
 *                                      @a NULL pointer.
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

    pthread_spinlock_t* lock = ( pthread_spinlock_t* )scorepMutex;

    pthread_spin_unlock( lock );

    return SCOREP_SUCCESS;
}
