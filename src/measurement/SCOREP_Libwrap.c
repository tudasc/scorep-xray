/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2014,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 * @file
 *
 * @brief Handling of symbols in shared libraries, for shared library wrapping
 */

#include <config.h>

#if HAVE_BACKEND( DLFCN_SUPPORT )
#include <dlfcn.h>
#endif

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <SCOREP_Definitions.h>
#include <SCOREP_Filter.h>
#include <SCOREP_Types.h>
#include <SCOREP_Mutex.h>
#include <SCOREP_Events.h>
#include <SCOREP_Libwrap.h>
#include <scorep_libwrap_management.h>
#include <UTILS_Error.h>



/** Data structure for library wrapper handle */
struct SCOREP_LibwrapHandle
{
    SCOREP_LibwrapAttributes* attributes;
    SCOREP_LibwrapHandle*     next;
    SCOREP_Mutex              region_definition_lock;
    uint32_t                  number_of_shared_lib_handles;
    void*                     shared_lib_handles[];
};

/** Library wrapper handles */
static SCOREP_LibwrapHandle* libwrap_handles;

/** Lock for definitions within Score-P library wrapping infrastructure */
static SCOREP_Mutex libwrap_object_lock;


/* ****************************************************************** */
/* Prototypes                                                         */
/* ****************************************************************** */

/**
 * This function will free the allocated memory and delete the wrapper
 * handle.
 *
 * @param handle            Library wrapper handle
 */
static void
scorep_libwrap_delete( SCOREP_LibwrapHandle* handle );


/* ****************************************************************** */
/* Implementations                                                    */
/* ****************************************************************** */

void
SCOREP_Libwrap_DefineRegion( SCOREP_LibwrapHandle* handle,
                             SCOREP_RegionHandle*  region,
                             const char*           func,
                             const char*           file,
                             int                   line )
{
    SCOREP_MutexLock( handle->region_definition_lock );

    if ( *region != SCOREP_INVALID_REGION )
    {
        SCOREP_MutexUnlock( handle->region_definition_lock );
        return;
    }

    if ( SCOREP_Filter_Match( file, func, NULL )  )
    {
        *region = SCOREP_LIBWRAP_FILTERED_REGION;
    }
    else
    {
        *region = SCOREP_Definitions_NewRegion( func,
                                                NULL,
                                                SCOREP_Definitions_NewSourceFile( file ),
                                                line,
                                                SCOREP_INVALID_LINE_NO,
                                                SCOREP_PARADIGM_USER,
                                                SCOREP_REGION_FUNCTION );
    }

    SCOREP_MutexUnlock( handle->region_definition_lock );
}

void
SCOREP_Libwrap_Create( SCOREP_LibwrapHandle**    handle,
                       SCOREP_LibwrapAttributes* attributes )
{
    SCOREP_MutexLock( libwrap_object_lock );

    if ( *handle != NULL )
    {
        SCOREP_MutexUnlock( libwrap_object_lock );
        return;
    }

    /* Get new library wrapper handle */
    *handle = malloc( sizeof( SCOREP_LibwrapHandle ) + attributes->number_of_shared_libs * sizeof( void* ) );
    assert( *handle );

    /* Enqueue new library wrapper handle */
    ( *handle )->next = libwrap_handles;
    libwrap_handles   = *handle;

    /* Initialize the new library wrapper handle */
    SCOREP_MutexCreate( &( ( *handle )->region_definition_lock ) );
    ( *handle )->attributes = attributes;

    /* Initialize number_of_shared_lib_handles */
    ( *handle )->number_of_shared_lib_handles = 0;

    if ( ( *handle )->attributes->mode == SCOREP_LIBWRAP_MODE_SHARED )
    {
#if HAVE_BACKEND( DLFCN_SUPPORT )

        for ( int i = 0; i < ( *handle )->attributes->number_of_shared_libs; i++ )
        {
            ( *handle )->shared_lib_handles[ i ] = dlopen( ( *handle )->attributes->shared_libs[ i ], RTLD_LAZY | RTLD_LOCAL );
            if ( ( *handle )->shared_lib_handles[ i ] == NULL )
            {
                UTILS_ERROR( SCOREP_ERROR_DLOPEN_FAILED,
                             "unable to open library %s",
                             ( *handle )->attributes->shared_libs[ i ] );
                break;
            }

            ( *handle )->number_of_shared_lib_handles++;
        }

#else

        UTILS_BUG( "This Score-P installation does not support dynamic linking via dlfcn.h" );

#endif
    }

    SCOREP_MutexUnlock( libwrap_object_lock );

    return;
}

void
SCOREP_Libwrap_SharedPtrInit( SCOREP_LibwrapHandle* handle,
                              const char*           func,
                              void**                funcPtr )
{
    if ( handle->attributes->mode != SCOREP_LIBWRAP_MODE_SHARED )
    {
        return;
    }

#if HAVE_BACKEND( DLFCN_SUPPORT )

    for ( uint32_t i = 0; i < handle->number_of_shared_lib_handles; i++ )
    {
        *funcPtr = dlsym( handle->shared_lib_handles[ i ], func );

        if ( !( *funcPtr ) )
        {
            char* dlsym_error_msg = dlerror();
            if ( dlsym_error_msg != NULL )
            {
                UTILS_ERROR( SCOREP_ERROR_DLSYM_FAILED, "dlsym( %s ), failed: %s", func, dlsym_error_msg );
            }
            else
            {
                UTILS_ERROR( SCOREP_ERROR_DLSYM_FAILED, "dlsym( %s ), failed: unknown error", func );
            }
        }
    }

#else

    UTILS_BUG( "This Score-P installation does not support dynamic linking via dlfcn.h" );

#endif
}

static void
scorep_libwrap_delete( SCOREP_LibwrapHandle* handle )
{
    UTILS_ASSERT( handle );

    if ( handle->attributes->mode != SCOREP_LIBWRAP_MODE_SHARED )
    {
#if HAVE_BACKEND( DLFCN_SUPPORT )

        /* Clear dlerror */
        ( void )dlerror();

        for ( uint32_t i = 0; i < handle->number_of_shared_lib_handles; i++ )
        {
            if ( dlclose( handle->shared_lib_handles[ i ] ) != 0 )
            {
                UTILS_ERROR( SCOREP_ERROR_DLCLOSE_FAILED, "dlclose( %s ), failed: %s",
                             handle->attributes->shared_libs[ i ], dlerror() );
            }
        }

#else

        UTILS_BUG( "This Score-P installation does not support dynamic linking via dlfcn.h" );

#endif
    }

    SCOREP_MutexDestroy( &( handle->region_definition_lock ) );
}


void
SCOREP_Libwrap_Initialize( void )
{
    SCOREP_MutexCreate( &libwrap_object_lock );
}

void
SCOREP_Libwrap_Finalize( void )
{
    SCOREP_LibwrapHandle* temp;

    while ( libwrap_handles != NULL )
    {
        temp            = libwrap_handles;
        libwrap_handles = temp->next;

        scorep_libwrap_delete( temp );
        free( temp );
    }

    SCOREP_MutexDestroy( &libwrap_object_lock );
}

void
SCOREP_Libwrap_EnterRegion( SCOREP_RegionHandle region )
{
    SCOREP_EnterRegion( region );
}

void
SCOREP_Libwrap_ExitRegion( SCOREP_RegionHandle region )
{
    SCOREP_ExitRegion( region );
}
