/*
 * This file is part of the Score-P software ecosystem (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2011,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2011, 2020,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2011,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2011, 2020-2022,
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



#ifndef UTILS_MUTEX_H
#define UTILS_MUTEX_H


/**
 * @file
 *
 *
 * Abstract locking object.
 *
 * Usage:
 * @code
 *     UTILS_Mutex lock = UTILS_MUTEX_INIT;
 *     :
 *     UTILS_MutexLock( &lock );
 *     : <critical section>
 *     UTILS_MutexUnlock( &lock );
 * @endcode
 *
 */


#ifndef __cplusplus
#include <stdbool.h>
#endif

#include <UTILS_Atomic.h>

#if HAVE( GCC_ATOMIC_BUILTINS )
#define STATIC_INLINE static inline
#else /* !GCC_ATOMIC_BUILTINS */
#define STATIC_INLINE
#endif /* !GCC_ATOMIC_BUILTINS */

/**
 * A lock object is just a bool. Should be 0-initialized if allocated.
 */
typedef bool UTILS_Mutex;

/**
 * Initialize an mutex variable to the unlocked state.
 */
#define UTILS_MUTEX_INIT false


UTILS_BEGIN_C_DECLS

#define UTILS_MutexLock PACKAGE_MANGLE_NAME( UTILS_MutexLock )
STATIC_INLINE void
UTILS_MutexLock( UTILS_Mutex* mutex );

/* Try to acquire the lock, returns true on success */
#define UTILS_MutexTrylock PACKAGE_MANGLE_NAME( UTILS_MutexTrylock )
STATIC_INLINE bool
UTILS_MutexTrylock( UTILS_Mutex* mutex );

#define UTILS_MutexUnlock PACKAGE_MANGLE_NAME( UTILS_MutexUnlock )
STATIC_INLINE void
UTILS_MutexUnlock( UTILS_Mutex* mutex );

/* Wait for a mutex state to be 'unlocked' */
#define UTILS_MutexWait PACKAGE_MANGLE_NAME( UTILS_MutexWait )
STATIC_INLINE void
UTILS_MutexWait( UTILS_Mutex*          mutex,
                 UTILS_Atomic_Memorder memorder );

UTILS_END_C_DECLS


#if HAVE( GCC_ATOMIC_BUILTINS )
#include "../src/mutex/UTILS_Mutex.inc.c"
#endif /* GCC_ATOMIC_BUILTINS */

#undef STATIC_INLINE

#endif /* UTILS_MUTEX_H */
