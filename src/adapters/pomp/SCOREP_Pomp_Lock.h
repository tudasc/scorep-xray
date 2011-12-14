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

#ifndef SCOREP_POMP_LOCK_H
#define SCOREP_POMP_LOCK_H

/**
 * @file       SCOREP_Pomp_Lock.h
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 * @status     alpha
 * @ingroup    POMP
 *
 * @brief Declaration of internal functins for lock management.
 */

#include <scorep/SCOREP_PublicTypes.h>
#include <stdint.h>

/** Definition of the type of the lock handle */
typedef uint32_t SCOREP_Pomp_LockHandleType;

/** Defines an invalid value for locks */
#define SCOREP_POMP_INVALID_LOCK -1

/** Number of omp regions */
#define SCOREP_POMP_REGION_NUM 10

/** Indexes for omp region handles in scorep_pomp_regid. */
enum SCOREP_Pomp_Region_Index
{
    SCOREP_POMP_INIT_LOCK = 0,
    SCOREP_POMP_DESTROY_LOCK,
    SCOREP_POMP_SET_LOCK,
    SCOREP_POMP_UNSET_LOCK,
    SCOREP_POMP_TEST_LOCK,
    SCOREP_POMP_INIT_NEST_LOCK,
    SCOREP_POMP_DESTROY_NEST_LOCK,
    SCOREP_POMP_SET_NEST_LOCK,
    SCOREP_POMP_UNSET_NEST_LOCK,
    SCOREP_POMP_TEST_NEST_LOCK
};

typedef struct SCOREP_PompLock SCOREP_PompLock;

struct SCOREP_PompLock
{
    const void*                lock;
    SCOREP_Pomp_LockHandleType handle;
    uint32_t                   acquisition_order;
    uint32_t                   nest_level; // only used for nested locks
};

/** List of handles for omp regions. The handles must be stored in the same order as
    the corresponding SCOREP_Pomp_Region_Index.
 */
SCOREP_RegionHandle scorep_pomp_regid[ SCOREP_POMP_REGION_NUM ];

/** Initializes a new lock handle.
    @param lock The OMP lock which should be initialized
    @returns the new SCOREP lock handle.
 */
SCOREP_PompLock*
scorep_pomp_lock_init( const void* lock );

/** Returns the scorep lock representation for a given OMP lock when acquiring the lock */
SCOREP_PompLock*
SCOREP_Pomp_GetAcquireLock( const void* lock );


/** Returns the scorep lock representation for a given OMP lock when releasing the lock */
SCOREP_PompLock*
SCOREP_Pomp_GetReleaseLock( const void* lock );


/** Returns the scorep lock representation for a given OMP lock when acquiring the nested lock */
SCOREP_PompLock*
SCOREP_Pomp_GetAcquireNestLock( const void* lock );


/** Returns the scorep lock representation for a given OMP lock when releasing the nested lock */
SCOREP_PompLock*
SCOREP_Pomp_GetReleaseNestLock( const void* lock );


/** Returns the scorep lock representation for a given OMP lock/critical */
SCOREP_PompLock*
SCOREP_Pomp_GetLock( const void* lock );


/** Deletes the given lock from the management system */
void
scorep_pomp_lock_destroy( const void* lock );

/** Clean up of the locking management. Frees all memory for locking managment. */
void
scorep_pomp_lock_close();

/** Registers omp lock regions ot the measurement system */
void
scorep_pomp_register_lock_regions();

#endif // SCOREP_POMP_LOCK_H
