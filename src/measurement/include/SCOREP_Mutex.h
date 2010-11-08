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
 * @file       src/measurement/include/SCOREP_Mutex.h
 * @maintainer Bert Wesarg <bert.wesarg@tu-dresden.de>
 *
 * @status alpha
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


#include <scorep_utility/SCOREP_Error.h>

/**
 * We use an opaque pointer type for the lock object.
 */
typedef void* SCOREP_Mutex;

SCOREP_Error_Code
SCOREP_MutexCreate( SCOREP_Mutex* scorepMutex );

SCOREP_Error_Code
SCOREP_MutexDestroy( SCOREP_Mutex* scorepMutex );

SCOREP_Error_Code
SCOREP_MutexLock( SCOREP_Mutex scorepMutex );

SCOREP_Error_Code
SCOREP_MutexUnlock( SCOREP_Mutex scorepMutex );

#endif /* SCOREP_MUTEX_H */
