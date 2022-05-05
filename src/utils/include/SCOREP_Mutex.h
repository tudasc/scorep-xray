/*
 * This file is part of the Score-P software (http://www.score-p.org)
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



#ifndef SCOREP_MUTEX_H
#define SCOREP_MUTEX_H


/**
 * @file
 *
 *
 * Abstract locking object.
 *
 * Usage:
 * @code
 *     SCOREP_Mutex lock = SCOREP_MUTEX_INIT;
 *     :
 *     SCOREP_MutexLock( &lock );
 *     : <critical section>
 *     SCOREP_MutexUnlock( &lock );
 * @endcode
 *
 */


#include <stdbool.h>

#include <UTILS_Atomic.h>

#if HAVE( GCC_ATOMIC_BUILTINS )
#include <stdlib.h>
#include <UTILS_Error.h>
#define STATIC_INLINE static inline
#else /* !GCC_ATOMIC_BUILTINS */
#define STATIC_INLINE
#endif /* !GCC_ATOMIC_BUILTINS */

/**
 * A lock object is just a bool. Should be 0-initialized if allocated.
 */
typedef bool SCOREP_Mutex;

/**
 * Initialize an mutex variable to the unlocked state.
 */
#define SCOREP_MUTEX_INIT false


STATIC_INLINE void
SCOREP_MutexLock( SCOREP_Mutex* scorepMutex );

/* Try to acquire the lock, returns true on success */
STATIC_INLINE bool
SCOREP_MutexTrylock( SCOREP_Mutex* scorepMutex );

STATIC_INLINE void
SCOREP_MutexUnlock( SCOREP_Mutex* scorepMutex );

/* Wait for a mutex state to be 'unlocked' */
STATIC_INLINE void
SCOREP_MutexWait( SCOREP_Mutex*         scorepMutex,
                  UTILS_Atomic_Memorder memorder );

#if HAVE( GCC_ATOMIC_BUILTINS )
#include "../mutex/scorep_mutex.inc.c"
#endif /* GCC_ATOMIC_BUILTINS */

#undef STATIC_INLINE

#endif /* SCOREP_MUTEX_H */
