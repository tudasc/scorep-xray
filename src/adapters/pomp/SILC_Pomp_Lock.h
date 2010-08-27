/*
 * This file is part of the SILC project (http://www.silc.de)
 *
 * Copyright (c) 2009-2011,
 *    RWTH Aachen, Germany
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *    Technische Universitaet Dresden, Germany
 *    University of Oregon, Eugene USA
 *    Forschungszentrum Juelich GmbH, Germany
 *    Technische Universitaet Muenchen, Germany
 *
 * See the COPYING file in the package base directory for details.
 *
 */

#ifndef SILC_POMP_LOCK_H
#define SILC_POMP_LOCK_H

/**
 * @file       SILC_Pomp_Lock.h
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 * @status     alpha
 * @ingroup    POMP
 *
 * @brief Declaration of internal functins for lock management.
 */

#include "SILC_PublicTypes.h"

/** Definition of the type of the lock handle */
typedef uint32_t SILC_Pomp_LockHandleType;

/** Number of omp regions */
#define SILC_POMP_REGION_NUM 10

/** Indexes for omp region handles in silc_pomp_regid. */
enum SILC_Pomp_Region_Index
{
    SILC_POMP_INIT_LOCK = 0,
    SILC_POMP_DESTROY_LOCK,
    SILC_POMP_SET_LOCK,
    SILC_POMP_UNSET_LOCK,
    SILC_POMP_TEST_LOCK,
    SILC_POMP_INIT_NEST_LOCK,
    SILC_POMP_DESTROY_NEST_LOCK,
    SILC_POMP_SET_NEST_LOCK,
    SILC_POMP_UNSET_NEST_LOCK,
    SILC_POMP_TEST_NEST_LOCK
};

/** List of handles for omp regions. The handles must be stored in the same order as
    the corresponding SILC_Pomp_Region_Index.
 */
SILC_RegionHandle silc_pomp_regid[ SILC_POMP_REGION_NUM ];

/** Initializes a new lock handle.
    @param lock The OMP lock which should be initialized
    @returns the new SILC lock handle.
 */
SILC_Pomp_LockHandleType
silc_pomp_lock_init( const void* lock );

/** Returns the lock handle for a given OMP lock */
SILC_Pomp_LockHandleType
silc_pomp_get_lock_handle( const void* lock );

/** Deletes teh given lock from the management system */
void
silc_pomp_lock_destroy( const void* lock );

/** Clean up of the locking management. Frres all memeory for locking managment. */
void
silc_pomp_lock_close();

/** Registers omp lock regions ot the measurement system */
void
silc_pomp_register_lock_regions();

#endif // SILC_POMP_LOCK_H
