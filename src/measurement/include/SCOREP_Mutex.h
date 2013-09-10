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
 *     SCOREP_Mutex lock;
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

SCOREP_ErrorCode
SCOREP_MutexCreate( SCOREP_Mutex* scorepMutex );

SCOREP_ErrorCode
SCOREP_MutexDestroy( SCOREP_Mutex* scorepMutex );

SCOREP_ErrorCode
SCOREP_MutexLock( SCOREP_Mutex scorepMutex );

SCOREP_ErrorCode
SCOREP_MutexUnlock( SCOREP_Mutex scorepMutex );

#endif /* SCOREP_MUTEX_H */
