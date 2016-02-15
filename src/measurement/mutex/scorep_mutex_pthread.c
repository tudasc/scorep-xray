/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2013-2014,
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
 ** Provides a lock implemention for programs using pthreads.
 *
 *
 */


#include <config.h>
#include <SCOREP_Mutex.h>


#include <stdlib.h>

#include <UTILS_Error.h>
#include <pthread.h>


#ifdef SCOREP_MUTEX_PTHREAD_WRAP
int
__real_pthread_mutex_init( pthread_mutex_t*,
                           const pthread_mutexattr_t* );
int
__real_pthread_mutex_destroy( pthread_mutex_t* );
int
__real_pthread_mutex_lock( pthread_mutex_t* mutex );
int
__real_pthread_mutex_unlock( pthread_mutex_t* mutex );

#define CALL_PTHREAD_REAL_FUNC( func_name ) __real_##func_name

#else

#define CALL_PTHREAD_REAL_FUNC( func_name ) func_name

#endif //SCOREP_MUTEX_PTHREAD_WRAP

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

    pthread_mutex_t** lock = ( pthread_mutex_t** )scorepMutex;

    *lock = malloc( sizeof( **lock ) );
    if ( !*lock )
    {
        return UTILS_ERROR_POSIX( "Can't allocate lock object" );
    }

    /* this call does not give us a success status */
    CALL_PTHREAD_REAL_FUNC( pthread_mutex_init )( *lock, NULL );
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

    pthread_mutex_t** lock = ( pthread_mutex_t** )scorepMutex;
    if ( !*lock )
    {
        return SCOREP_SUCCESS;
    }
    CALL_PTHREAD_REAL_FUNC( pthread_mutex_destroy )( *lock );
    free( *lock );

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

    pthread_mutex_t* lock = ( pthread_mutex_t* )scorepMutex;
    CALL_PTHREAD_REAL_FUNC( pthread_mutex_lock )( lock );

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

    pthread_mutex_t* lock = ( pthread_mutex_t* )scorepMutex;

    CALL_PTHREAD_REAL_FUNC( pthread_mutex_unlock )( lock );

    return SCOREP_SUCCESS;
}
