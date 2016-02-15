/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2011,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2011, 2016,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2011,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2011, 2014,
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



/**
 * @file
 *
 *
 * Provides a lock implemention for OpenMP programs.
 *
 */


#include <config.h>
#include <SCOREP_Mutex.h>
#include <SCOREP_RuntimeManagement.h>

#include <stdlib.h>

#include <omp.h>

#include <UTILS_Error.h>

/**
 * Allocates and initializes a new OpenMP lock.
 *
 * @param scorepMutex Reference to a SCOREP_Mutex variable.
 *
 * @return SCOREP_SUCCESS for success, or the error code.
 *
 * @error SCOREP_ERROR_INVALID_ARGUMENT The given @a scorepMutex parameter is a
 *                                      @a NULL pointer.
 * @error scorep_errno Any errors from malloc(3) converted to an SCOREP error code.
 *
 */
SCOREP_ErrorCode
SCOREP_MutexCreate( SCOREP_Mutex* scorepMutex )
{
    if ( !scorepMutex )
    {
        return UTILS_ERROR( SCOREP_ERROR_INVALID_ARGUMENT,
                            "Invalid mutex handle given." );
    }

    omp_lock_t** omp_lock = ( omp_lock_t** )scorepMutex;

    *omp_lock = malloc( sizeof( **omp_lock ) );
    if ( !*omp_lock )
    {
        return UTILS_ERROR_POSIX( "Can't allocate lock object" );
    }

    /* this call does not give us a success status */
    omp_init_lock( *omp_lock );

    return SCOREP_SUCCESS;
}


/**
 * Destroys and Deallocates a OpenMP lock.
 *
 * @param scorepMutex Reference to a SCOREP_Mutex variable.
 *
 * @return SCOREP_SUCCESS for success, or the error code.
 *
 * @error SCOREP_ERROR_INVALID_ARGUMENT The given @a scorepMutex parameter is a
 *                                      @a NULL pointer.
 *
 */
SCOREP_ErrorCode
SCOREP_MutexDestroy( SCOREP_Mutex* scorepMutex )
{
    if ( !scorepMutex )
    {
        return UTILS_ERROR( SCOREP_ERROR_INVALID_ARGUMENT,
                            "Invalid mutex handle given." );
    }

    omp_lock_t** omp_lock = ( omp_lock_t** )scorepMutex;

    if ( !*omp_lock )
    {
        /* NULL pointers are allowed but not for omp_destroy_lock */
        return SCOREP_SUCCESS;
    }

    /* this call does not give us a success status */
#if defined ( __INTEL_COMPILER ) && ( __INTEL_COMPILER < 1120 )
    // Do nothing here. Intel OpenMP RTL shuts down at the end of main
    // function, so omp_destroy_lock, which is called after the end
    // of main from the atexit handler, causes segmentation fault. The
    // problem will be fixed in  Intel Compiler 11.1 update 6.
    // See http://software.intel.com/en-us/forums/showpost.php?p=110592
#else
    omp_destroy_lock( *omp_lock );
#endif

    free( *omp_lock );

    *scorepMutex = SCOREP_INVALID_MUTEX;

    return SCOREP_SUCCESS;
}


/**
 * Locks an SCOREP_Mutex by using the OpenMP API.
 *
 * @param scorepMutex A SCOREP_Mutex variable, previously created with @a
 *                    SCOREP_MutexCreate.
 *
 * @return SCOREP_SUCCESS for success, or the error code.
 *
 * @error SCOREP_ERROR_INVALID_ARGUMENT The given @a scorepMutex parameter is a
 *                                      @a NULL pointer.
 *
 */
SCOREP_ErrorCode
SCOREP_MutexLock( SCOREP_Mutex scorepMutex )
{
    #if defined ( __INTEL_COMPILER ) && ( __INTEL_COMPILER < 1120 )
    if ( !SCOREP_IsInitialized() )
    {
        return SCOREP_SUCCESS;
    }
    #endif

    if ( !scorepMutex )
    {
        return UTILS_ERROR( SCOREP_ERROR_INVALID_ARGUMENT,
                            "Invalid mutex handle given." );
    }

    omp_lock_t* omp_lock = ( omp_lock_t* )scorepMutex;

    /* this call does not give us a success status */
    omp_set_lock( omp_lock );

    return SCOREP_SUCCESS;
}


/**
 * Unlocks an SCOREP_Mutex by using the OpenMP API.
 *
 * @param scorepMutex A SCOREP_Mutex variable, previously created with @a
 *                    SCOREP_MutexCreate.
 *
 * @return SCOREP_SUCCESS for success, or the error code.
 *
 * @error SCOREP_ERROR_INVALID_ARGUMENT The given @a scorepMutex parameter is a
 *                                      @a NULL pointer.
 *
 */
SCOREP_ErrorCode
SCOREP_MutexUnlock( SCOREP_Mutex scorepMutex )
{
    #if defined ( __INTEL_COMPILER ) && ( __INTEL_COMPILER < 1120 )
    if ( !SCOREP_IsInitialized() )
    {
        return SCOREP_SUCCESS;
    }
    #endif

    if ( !scorepMutex )
    {
        return UTILS_ERROR( SCOREP_ERROR_INVALID_ARGUMENT,
                            "Invalid mutex handle given." );
    }

    omp_lock_t* omp_lock = ( omp_lock_t* )scorepMutex;

    /* this call does not give us a success status */
    omp_unset_lock( omp_lock );

    return SCOREP_SUCCESS;
}
