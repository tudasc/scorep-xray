/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2011,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2011,
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
 *     SCOREP_Mutex lock = SCOREP_INVALID_MUTEX;
 *     SCOREP_MutexCreate( &lock );
 *     :
 *     SCOREP_MutexLock( lock );
 *     : <critical section>
 *     SCOREP_MutexUnlock( lock );
 *     :
 *     SCOREP_MutexDestroy( &lock );
 * @endcode
 *
 */


#include <SCOREP_ErrorCodes.h>

/**
 * We use an opaque pointer type for the lock object.
 */
typedef void* SCOREP_Mutex;

/**
 * Initialize an mutex variable to an invalid value.
 */
#define SCOREP_INVALID_MUTEX NULL

SCOREP_ErrorCode
SCOREP_MutexCreate( SCOREP_Mutex* scorepMutex );

SCOREP_ErrorCode
SCOREP_MutexDestroy( SCOREP_Mutex* scorepMutex );

SCOREP_ErrorCode
SCOREP_MutexLock( SCOREP_Mutex scorepMutex );

SCOREP_ErrorCode
SCOREP_MutexUnlock( SCOREP_Mutex scorepMutex );

#endif /* SCOREP_MUTEX_H */
