/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2015, 2020,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2022,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */


/**
 * @file
 *
 *
 */


#include <config.h>


#include "scorep_tracing_internal.h"


#include <stddef.h>


#include <otf2/otf2.h>


#include <UTILS_Error.h>
#include <UTILS_Mutex.h>


struct OTF2_LockObject
{
    UTILS_Mutex mutex;
};


static OTF2_CallbackCode
lock_create( void*      userData,
             OTF2_Lock* lock )
{
    *lock = calloc( 1, sizeof( **lock ) );

    return OTF2_CALLBACK_SUCCESS;
}


static OTF2_CallbackCode
lock_destroy( void*     userData,
              OTF2_Lock lock )
{
    free( lock );

    return OTF2_CALLBACK_SUCCESS;
}


static OTF2_CallbackCode
lock_lock( void*     userData,
           OTF2_Lock lock )
{
    UTILS_MutexLock( &lock->mutex );

    return OTF2_CALLBACK_SUCCESS;
}


static OTF2_CallbackCode
lock_unlock( void*     userData,
             OTF2_Lock lock )
{
    UTILS_MutexUnlock( &lock->mutex );

    return OTF2_CALLBACK_SUCCESS;
}


static const OTF2_LockingCallbacks locking_callbacks =
{
    .otf2_release = NULL,
    .otf2_create  = lock_create,
    .otf2_destroy = lock_destroy,
    .otf2_lock    = lock_lock,
    .otf2_unlock  = lock_unlock
};


SCOREP_ErrorCode
scorep_tracing_set_locking_callbacks( OTF2_Archive* archive )
{
    OTF2_ErrorCode err = OTF2_Archive_SetLockingCallbacks(
        archive,
        &locking_callbacks,
        NULL );
    return OTF2_SUCCESS == err
           ? SCOREP_SUCCESS
           : SCOREP_ERROR_PROCESSED_WITH_FAULTS;
}
