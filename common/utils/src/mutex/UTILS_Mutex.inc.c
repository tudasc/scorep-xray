/*
 * This file is part of the Score-P software ecosystem (http://www.score-p.org)
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

#include <stddef.h>

#include <UTILS_Error.h>

STATIC_INLINE void
UTILS_MutexLock( UTILS_Mutex* mutex )
{
    UTILS_BUG_ON( mutex == NULL, "Invalid mutex handle given." );

    /* test-and-test-and-set lock */
    while ( true )
    {
        while ( UTILS_Atomic_LoadN_bool( mutex, UTILS_ATOMIC_RELAXED ) == true )
        {
            UTILS_CPU_RELAX;
        }
        if ( UTILS_Atomic_TestAndSet( mutex, UTILS_ATOMIC_ACQUIRE ) != true )
        {
            break;
        }
        UTILS_CPU_RELAX;
    }
}

STATIC_INLINE bool
UTILS_MutexTrylock( UTILS_Mutex* mutex )
{
    UTILS_BUG_ON( mutex == NULL, "Invalid mutex handle given." );

    if ( UTILS_Atomic_LoadN_bool( mutex, UTILS_ATOMIC_RELAXED ) == true )
    {
        UTILS_CPU_RELAX;
        return false;
    }

    if ( UTILS_Atomic_TestAndSet( mutex, UTILS_ATOMIC_ACQUIRE ) == true )
    {
        UTILS_CPU_RELAX;
        return false;
    }

    return true;
}

STATIC_INLINE void
UTILS_MutexUnlock( UTILS_Mutex* mutex )
{
    UTILS_BUG_ON( mutex == NULL, "Invalid mutex handle given." );

    UTILS_Atomic_clear( mutex, UTILS_ATOMIC_RELEASE );
}

STATIC_INLINE void
UTILS_MutexWait( UTILS_Mutex* mutex, UTILS_Atomic_Memorder memorder )
{
    UTILS_BUG_ON( mutex == NULL, "Invalid mutex handle given." );

    while ( UTILS_Atomic_LoadN_bool( mutex, ( memorder ) ) == true )
    {
        UTILS_CPU_RELAX;
    }
}
