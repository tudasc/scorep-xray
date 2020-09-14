/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2020,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

#ifndef STATIC_INLINE
#error STATIC_INLINE needs to be defined
#endif /* STATIC_INLINE */


STATIC_INLINE SCOREP_ErrorCode
SCOREP_MutexCreate( SCOREP_Mutex* scorepMutex )
{
    if ( !scorepMutex )
    {
        return UTILS_ERROR( SCOREP_ERROR_INVALID_ARGUMENT,
                            "Invalid mutex handle given." );
    }

    bool** lock = ( bool** )scorepMutex;
    *lock = calloc( 1, sizeof( **lock ) );
    if ( !*lock )
    {
        return UTILS_ERROR_POSIX( "Can't allocate lock object" );
    }

    return SCOREP_SUCCESS;
}

STATIC_INLINE SCOREP_ErrorCode
SCOREP_MutexDestroy( SCOREP_Mutex* scorepMutex )
{
    if ( !scorepMutex )
    {
        return UTILS_ERROR( SCOREP_ERROR_INVALID_ARGUMENT, "" );
    }

    bool** lock = ( bool** )scorepMutex;
    if ( !*lock )
    {
        return SCOREP_SUCCESS;
    }

    // Destroy even if locked?
    free( *lock );

    *scorepMutex = SCOREP_INVALID_MUTEX;

    return SCOREP_SUCCESS;
}

STATIC_INLINE SCOREP_ErrorCode
SCOREP_MutexLock( SCOREP_Mutex scorepMutex )
{
    if ( !scorepMutex )
    {
        return UTILS_ERROR( SCOREP_ERROR_INVALID_ARGUMENT,
                            "Invalid mutex handle given." );
    }

    bool* lock = ( bool* )scorepMutex;
    /* test-and-test-and-set lock */
    while ( true )
    {
        while ( SCOREP_Atomic_LoadN_bool( lock, SCOREP_ATOMIC_RELAXED ) == true )
        {
            SCOREP_CPU_RELAX;
        }
        if ( SCOREP_Atomic_TestAndSet( lock, SCOREP_ATOMIC_ACQUIRE ) != true )
        {
            break;
        }
        SCOREP_CPU_RELAX;
    }

    return SCOREP_SUCCESS;
}

STATIC_INLINE SCOREP_ErrorCode
SCOREP_MutexUnlock( SCOREP_Mutex scorepMutex )
{
    if ( !scorepMutex )
    {
        return UTILS_ERROR( SCOREP_ERROR_INVALID_ARGUMENT,
                            "Invalid mutex handle given." );
    }

    bool* lock = ( bool* )scorepMutex;
    SCOREP_Atomic_clear( lock, SCOREP_ATOMIC_RELEASE );

    return SCOREP_SUCCESS;
}
