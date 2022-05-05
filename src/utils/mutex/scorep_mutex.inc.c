/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2020-2022,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2020,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

#ifndef STATIC_INLINE
#error STATIC_INLINE needs to be defined
#endif /* STATIC_INLINE */

#include <UTILS_Error.h>

STATIC_INLINE void
SCOREP_MutexLock( SCOREP_Mutex* scorepMutex )
{
    UTILS_BUG_ON( scorepMutex == NULL, "Invalid mutex handle given." );

    /* test-and-test-and-set lock */
    while ( true )
    {
        while ( UTILS_Atomic_LoadN_bool( scorepMutex, UTILS_ATOMIC_RELAXED ) == true )
        {
            UTILS_CPU_RELAX;
        }
        if ( UTILS_Atomic_TestAndSet( scorepMutex, UTILS_ATOMIC_ACQUIRE ) != true )
        {
            break;
        }
        UTILS_CPU_RELAX;
    }
}

STATIC_INLINE bool
SCOREP_MutexTrylock( SCOREP_Mutex* scorepMutex )
{
    UTILS_BUG_ON( scorepMutex == NULL, "Invalid mutex handle given." );

    if ( UTILS_Atomic_LoadN_bool( scorepMutex, UTILS_ATOMIC_RELAXED ) == true )
    {
        UTILS_CPU_RELAX;
        return false;
    }

    if ( UTILS_Atomic_TestAndSet( scorepMutex, UTILS_ATOMIC_ACQUIRE ) == true )
    {
        UTILS_CPU_RELAX;
        return false;
    }

    return true;
}

STATIC_INLINE void
SCOREP_MutexUnlock( SCOREP_Mutex* scorepMutex )
{
    UTILS_BUG_ON( scorepMutex == NULL, "Invalid mutex handle given." );

    UTILS_Atomic_clear( scorepMutex, UTILS_ATOMIC_RELEASE );
}

STATIC_INLINE void
SCOREP_MutexWait( SCOREP_Mutex* scorepMutex, UTILS_Atomic_Memorder memorder )
{
    UTILS_BUG_ON( scorepMutex == NULL, "Invalid mutex handle given." );

    while ( UTILS_Atomic_LoadN_bool( scorepMutex, ( memorder ) ) == true )
    {
        UTILS_CPU_RELAX;
    }
}
