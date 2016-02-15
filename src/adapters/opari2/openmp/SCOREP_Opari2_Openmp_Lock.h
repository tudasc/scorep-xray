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
 * Copyright (c) 2009-2014,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2011,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2011,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 */

#ifndef SCOREP_OPARI2_OPENMP_H
#define SCOREP_OPARI2_OPENMP_H

/**
 * @file
 *
 * @ingroup OPARI2_LOCK
 *
 * @brief Declaration of internal functions for lock management.
 */

#include <scorep/SCOREP_PublicTypes.h>
#include <SCOREP_Mutex.h>
#include <stdint.h>

/** Definition of the type of the lock handle */
typedef uint32_t SCOREP_Opari2_Openmp_Lock_HandleType;

/** Defines an invalid value for locks */
#define SCOREP_OPARI2_OPENMP_LOCK_INVALID_LOCK -1

/** Indexes for omp lock region handles in scorep_opari2_openmp_lock_region_handles. */
enum SCOREP_Opari2_Openmp_Lock_Region_Index
{
    SCOREP_OPARI2_OPENMP_INIT_LOCK = 0,
    SCOREP_OPARI2_OPENMP_DESTROY_LOCK,
    SCOREP_OPARI2_OPENMP_SET_LOCK,
    SCOREP_OPARI2_OPENMP_UNSET_LOCK,
    SCOREP_OPARI2_OPENMP_TEST_LOCK,
    SCOREP_OPARI2_OPENMP_INIT_NEST_LOCK,
    SCOREP_OPARI2_OPENMP_DESTROY_NEST_LOCK,
    SCOREP_OPARI2_OPENMP_SET_NEST_LOCK,
    SCOREP_OPARI2_OPENMP_UNSET_NEST_LOCK,
    SCOREP_OPARI2_OPENMP_TEST_NEST_LOCK,

    SCOREP_OPARI2_OPENMP_LOCK_NUM
};

extern SCOREP_RegionHandle scorep_opari2_openmp_lock_region_handles[ SCOREP_OPARI2_OPENMP_LOCK_NUM ];

typedef struct SCOREP_Opari2_Openmp_Lock SCOREP_Opari2_Openmp_Lock;

struct SCOREP_Opari2_Openmp_Lock
{
    const void*                          lock;
    SCOREP_Opari2_Openmp_Lock_HandleType handle;
    uint32_t                             acquisition_order;
    uint32_t                             nest_level; // only used for nested locks
};

/** Mutex to ensure exclusive access to opari2_openmp_lock lock data structure.
 */
extern SCOREP_Mutex scorep_opari2_openmp_lock;

/** Initializes a new lock handle.
    @param lock The OMP lock which should be initialized
    @returns the new SCOREP lock handle.
 */
SCOREP_Opari2_Openmp_Lock*
scorep_opari2_openmp_lock_init( const void* lock );

/** Returns the scorep lock representation for a given OMP lock when acquiring the lock */
SCOREP_Opari2_Openmp_Lock*
SCOREP_Opari2_Openmp_GetAcquireLock( const void* lock );


/** Returns the scorep lock representation for a given OMP lock when releasing the lock */
SCOREP_Opari2_Openmp_Lock*
SCOREP_Opari2_Openmp_GetReleaseLock( const void* lock );


/** Returns the scorep lock representation for a given OMP lock when acquiring the nested lock */
SCOREP_Opari2_Openmp_Lock*
SCOREP_Opari2_Openmp_GetAcquireNestLock( const void* lock );


/** Returns the scorep lock representation for a given OMP lock when releasing the nested lock */
SCOREP_Opari2_Openmp_Lock*
SCOREP_Opari2_Openmp_GetReleaseNestLock( const void* lock );


/** Returns the scorep lock representation for a given OMP lock/critical */
SCOREP_Opari2_Openmp_Lock*
SCOREP_Opari2_Openmp_GetLock( const void* lock );


/** Deletes the given lock from the management system */
void
scorep_opari2_openmp_lock_destroy( const void* lock );

/** Registers omp lock regions ot the measurement system */
void
scorep_opari2_openmp_lock_initialize( void );

/** Clean up of the locking management. Frees all memory for locking managment. */
void
scorep_opari2_openmp_lock_finalize( void );

#endif /* SCOREP_OPARI2_OPENMP_LOCK_H */
