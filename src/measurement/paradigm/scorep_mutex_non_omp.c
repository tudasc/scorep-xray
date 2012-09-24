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



/**
 * @file       src/measurement/paradigm/scorep_lock_non_omp.c
 * @maintainer Bert Wesarg <bert.wesarg@tu-dresden.de>
 *
 * @status alpha
 *
 * Provides a lock implemention for non-OpenMP programs.
 * I.e no locking at all.
 *
 *
 */


#include <config.h>


#include <stdlib.h>


#include <SCOREP_Mutex.h>


/** Magic number to identify an initialized lock */
#define SCOREP_MUTEXT_MARKER ( void* )0x10C88c01


/**
 * Emtpy stub for empty lock.
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

    /* mark this lock initialized */
    *scorepMutex = SCOREP_MUTEXT_MARKER;

    return SCOREP_SUCCESS;
}


/**
 * Emtpy stub for empty lock.
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

    if ( *scorepMutex == NULL )
    {
        return SCOREP_SUCCESS;
    }

    if ( *scorepMutex != SCOREP_MUTEXT_MARKER )
    {
        return UTILS_ERROR( SCOREP_ERROR_INVALID_ARGUMENT, "Invalid lock" );
    }

    *scorepMutex = NULL;

    return SCOREP_SUCCESS;
}


/**
 * Emtpy stub for empty lock.
 *
 */
SCOREP_ErrorCode
SCOREP_MutexLock( SCOREP_Mutex scorepMutex )
{
    if ( !scorepMutex )
    {
        return UTILS_ERROR( SCOREP_ERROR_INVALID_ARGUMENT,
                            "Invalid mutex handle given." );
    }

    if ( scorepMutex != SCOREP_MUTEXT_MARKER )
    {
        return UTILS_ERROR( SCOREP_ERROR_INVALID_ARGUMENT, "Invalid lock" );
    }

    return SCOREP_SUCCESS;
}


/**
 * Emtpy stub for empty lock.
 *
 */
SCOREP_ErrorCode
SCOREP_MutexUnlock( SCOREP_Mutex scorepMutex )
{
    if ( !scorepMutex )
    {
        return UTILS_ERROR( SCOREP_ERROR_INVALID_ARGUMENT,
                            "Invalid mutex handle given." );
    }

    if ( scorepMutex != SCOREP_MUTEXT_MARKER )
    {
        return UTILS_ERROR( SCOREP_ERROR_INVALID_ARGUMENT, "Invalid lock" );
    }

    return SCOREP_SUCCESS;
}
