/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2014, 2017,
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

#if HAVE_BACKEND( LIBWRAP_RUNTIME_SUPPORT )

#include <dlfcn.h>

# if HAVE( GNU_LIB_NAMES_H )
#  include <gnu/lib-names.h>
# endif

#endif

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <SCOREP_RuntimeManagement.h>
#include <SCOREP_InMeasurement.h>
#include <SCOREP_Definitions.h>
#include <SCOREP_Filtering.h>
#include <SCOREP_Types.h>
#include <SCOREP_Mutex.h>
#include <SCOREP_Events.h>
#include <scorep/SCOREP_Libwrap.h>
#include <scorep_libwrap_management.h>

#include <UTILS_Error.h>
#include <UTILS_CStr.h>

#include <SCOREP_Hashtab.h>


/** Data structure for library wrapper handle */
struct SCOREP_LibwrapHandle
{
    const SCOREP_LibwrapAttributes* attributes;
    SCOREP_LibwrapHandle*           next;
    SCOREP_Mutex                    region_definition_lock;
    uint32_t                        number_of_shared_lib_handles;
    void*                           shared_lib_handles[];
};

/** Library wrapper handles */
static SCOREP_LibwrapHandle* libwrap_handles;

/** Lock for definitions within Score-P library wrapping infrastructure */
static SCOREP_Mutex libwrap_object_lock;

/** Our own flag which indicates that the libwrapping is active, this
    is more or less MEASUREMENT_PHASE( WITHIN ), except that we are already
    active after SCOREP_Libwrap_Initialize was called by
    SCOREP_InitMeasurement, which is slightly earlier than entering PHASE( WITHIN ) */
static bool active;

#if HAVE( GNU_LIB_NAMES_H )
static SCOREP_Hashtab* lib_names_mapping;
#endif

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
                             int*                  regionFiltered,
                             const char*           func,
                             const char*           symbol,
                             const char*           file,
                             int                   line )
{
    if ( !active )
    {
        return;
    }

    SCOREP_MutexLock( handle->region_definition_lock );

    if ( *region != SCOREP_INVALID_REGION )
    {
        SCOREP_MutexUnlock( handle->region_definition_lock );
        return;
    }

    *region = SCOREP_Definitions_NewRegion( func,
                                            symbol,
                                            SCOREP_Definitions_NewSourceFile( file ),
                                            line,
                                            SCOREP_INVALID_LINE_NO,
                                            SCOREP_PARADIGM_LIBWRAP,
                                            SCOREP_REGION_WRAPPER );
    SCOREP_RegionHandle_SetGroup( *region, handle->attributes->display_name );

    if ( regionFiltered )
    {
        *regionFiltered = !!SCOREP_Filtering_Match( file, func, symbol );
    }

    SCOREP_MutexUnlock( handle->region_definition_lock );
}

void
SCOREP_Libwrap_Create( SCOREP_LibwrapHandle**          handle,
                       const SCOREP_LibwrapAttributes* attributes )
{
    if ( SCOREP_IS_MEASUREMENT_PHASE( PRE ) )
    {
        SCOREP_InitMeasurement();
    }
    if ( !active )
    {
        return;
    }

    if ( attributes->version != SCOREP_LIBWRAP_VERSION )
    {
        UTILS_FATAL( "Incompatible API/ABI version for library wrapper '%s' (expected: %d, actual: %d)",
                     attributes->name,
                     SCOREP_LIBWRAP_VERSION,
                     attributes->version );
    }

    SCOREP_MutexLock( libwrap_object_lock );

    if ( *handle != NULL )
    {
        SCOREP_MutexUnlock( libwrap_object_lock );
        return;
    }

    /* Get new library wrapper handle */
    *handle = malloc( sizeof( SCOREP_LibwrapHandle ) + attributes->number_of_shared_libs * sizeof( void* ) );
    UTILS_ASSERT( *handle );

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
#if HAVE_BACKEND( LIBWRAP_RUNTIME_SUPPORT )

        /* clear any recent errors */
        ( void )dlerror();
        if ( ( *handle )->attributes->number_of_shared_libs == 0 )
        {
            UTILS_FATAL( "Empty library list. Runtime wrapping not supported for library wrapper '%s'", attributes->name );
        }

        for ( int i = 0; i < ( *handle )->attributes->number_of_shared_libs; i++ )
        {
            const char* lib_name = attributes->shared_libs[ i ];
#if HAVE( GNU_LIB_NAMES_H )
            /* if this is a lib in lib_names, than it should be found without a path too */
            if ( strrchr( lib_name, '/' ) )
            {
                lib_name = strrchr( lib_name, '/' ) + 1;
            }
            SCOREP_Hashtab_Entry* result = SCOREP_Hashtab_Find( lib_names_mapping, lib_name, NULL );
            if ( result )
            {
                lib_name = result->value.ptr;
            }
            else
            {
                /* not found, use original lib name again */
                lib_name = attributes->shared_libs[ i ];
            }
#endif

            ( *handle )->shared_lib_handles[ ( *handle )->number_of_shared_lib_handles ] =
                dlopen( lib_name, RTLD_LAZY | RTLD_LOCAL );
            if ( ( *handle )->shared_lib_handles[ ( *handle )->number_of_shared_lib_handles ] == NULL )
            {
                char* error = dlerror();
                if ( !error )
                {
                    error = "success";
                }
                UTILS_ERROR( SCOREP_ERROR_DLOPEN_FAILED,
                             "unable to open library %s: %s",
                             ( *handle )->attributes->shared_libs[ i ], error );
                continue;
            }

            ( *handle )->number_of_shared_lib_handles++;
        }

#else

        UTILS_BUG( "This Score-P installation does not support dynamic linking via dlfcn.h" );

#endif
    }

    if ( attributes->init )
    {
        attributes->init( *handle );
    }

    SCOREP_MutexUnlock( libwrap_object_lock );

    return;
}

void
SCOREP_Libwrap_SharedPtrInit( SCOREP_LibwrapHandle* handle,
                              const char*           func,
                              void**                funcPtr )
{
    if ( !active )
    {
        return;
    }

    if ( handle->attributes->mode != SCOREP_LIBWRAP_MODE_SHARED )
    {
        return;
    }

    if ( *funcPtr )
    {
        return;
    }

#if HAVE_BACKEND( LIBWRAP_RUNTIME_SUPPORT )

    /* clear any recent errors */
    ( void )dlerror();
    for ( uint32_t i = 0; i < handle->number_of_shared_lib_handles; i++ )
    {
        *funcPtr = dlsym( handle->shared_lib_handles[ i ], func );
        if ( *funcPtr )
        {
            break;
        }
    }
    if ( !( *funcPtr ) )
    {
        char* error = dlerror();
        if ( error == NULL )
        {
            error = "success";
        }
        UTILS_FATAL( "Could not resolve symbol '%s' for library wrapper '%s': %s",
                     func, handle->attributes->name, error );
    }

#else

    UTILS_BUG( "This Score-P installation does not support dynamic linking via dlfcn.h" );

#endif
}

void
SCOREP_Libwrap_EarlySharedPtrInit( const char* func,
                                   void**      funcPtr )
{
#if HAVE( LIBWRAP_RUNTIME_SUPPORT ) && HAVE( DLFCN_RTLD_NEXT )
    /* No locking, pointer assignment should be atomic, and the result always the same */
    *funcPtr = dlsym( RTLD_NEXT, func );
#endif
}

static void
scorep_libwrap_delete( SCOREP_LibwrapHandle* handle )
{
    UTILS_ASSERT( handle );

    if ( handle->attributes->mode != SCOREP_LIBWRAP_MODE_SHARED )
    {
#if HAVE_BACKEND( LIBWRAP_RUNTIME_SUPPORT )

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

#if HAVE( GNU_LIB_NAMES_H )
    lib_names_mapping = SCOREP_Hashtab_CreateSize( 16,
                                                   SCOREP_Hashtab_HashString,
                                                   SCOREP_Hashtab_CompareStrings );

#define add_so_mapping( so ) \
    do \
    { \
        if ( strstr( so, ".so." ) ) \
        { \
            char* from = UTILS_CStr_dup( so ); \
            *( strstr( from, ".so." ) + 3 ) = '\0'; \
            char* so_copy = UTILS_CStr_dup( so ); \
            SCOREP_Hashtab_InsertPtr( lib_names_mapping, from, so_copy, NULL ); \
        } \
    } while ( 0 )
#ifdef LD_SO
    add_so_mapping( LD_SO );
#endif
#ifdef LIBANL_SO
    add_so_mapping( LIBANL_SO );
#endif
#ifdef LIBBROKENLOCALE_SO
    add_so_mapping( LIBBROKENLOCALE_SO );
#endif
#ifdef LIBCIDN_SO
    add_so_mapping( LIBCIDN_SO );
#endif
#ifdef LIBCRYPT_SO
    add_so_mapping( LIBCRYPT_SO );
#endif
#ifdef LIBC_SO
    add_so_mapping( LIBC_SO );
#endif
#ifdef LIBDL_SO
    add_so_mapping( LIBDL_SO );
#endif
#ifdef LIBGCC_S_SO
    add_so_mapping( LIBGCC_S_SO );
#endif
#ifdef LIBM_SO
    add_so_mapping( LIBM_SO );
#endif
#ifdef LIBNSL_SO
    add_so_mapping( LIBNSL_SO );
#endif
#ifdef LIBNSS_COMPAT_SO
    add_so_mapping( LIBNSS_COMPAT_SO );
#endif
#ifdef LIBNSS_DB_SO
    add_so_mapping( LIBNSS_DB_SO );
#endif
#ifdef LIBNSS_DNS_SO
    add_so_mapping( LIBNSS_DNS_SO );
#endif
#ifdef LIBNSS_FILES_SO
    add_so_mapping( LIBNSS_FILES_SO );
#endif
#ifdef LIBNSS_HESIOD_SO
    add_so_mapping( LIBNSS_HESIOD_SO );
#endif
#ifdef LIBNSS_LDAP_SO
    add_so_mapping( LIBNSS_LDAP_SO );
#endif
#ifdef LIBNSS_NISPLUS_SO
    add_so_mapping( LIBNSS_NISPLUS_SO );
#endif
#ifdef LIBNSS_NIS_SO
    add_so_mapping( LIBNSS_NIS_SO );
#endif
#ifdef LIBNSS_TEST1_SO
    add_so_mapping( LIBNSS_TEST1_SO );
#endif
#ifdef LIBPTHREAD_SO
    add_so_mapping( LIBPTHREAD_SO );
#endif
#ifdef LIBRESOLV_SO
    add_so_mapping( LIBRESOLV_SO );
#endif
#ifdef LIBRT_SO
    add_so_mapping( LIBRT_SO );
#endif
#ifdef LIBTHREAD_DB_SO
    add_so_mapping( LIBTHREAD_DB_SO );
#endif
#ifdef LIBUTIL_SO
    add_so_mapping( LIBUTIL_SO );
#endif

#undef add_so_mapping
#endif

    active = true;
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

#if HAVE( GNU_LIB_NAMES_H )
    SCOREP_Hashtab_FreeAll( lib_names_mapping,
                            SCOREP_Hashtab_DeleteFree,
                            SCOREP_Hashtab_DeleteFree );
#endif

    SCOREP_MutexDestroy( &libwrap_object_lock );

    active = false;
}

int
SCOREP_Libwrap_EnterMeasurement( void )
{
    /* Do not use `active`, as we need to ensure that events really are only
       triggered inside PHASE( WITHIN ) */
    return SCOREP_IN_MEASUREMENT_TEST_AND_INCREMENT()
           && SCOREP_IS_MEASUREMENT_PHASE( WITHIN );
}

void
SCOREP_Libwrap_ExitMeasurement( void )
{
    SCOREP_IN_MEASUREMENT_DECREMENT();
}

void
SCOREP_Libwrap_EnterRegion( SCOREP_RegionHandle region )
{
    /* The increment is needed, to account this function also as part of the wrapper */
#if HAVE( RETURN_ADDRESS )
    SCOREP_IN_MEASUREMENT_INCREMENT();
#endif
    SCOREP_EnterWrappedRegion( region );
#if HAVE( RETURN_ADDRESS )
    SCOREP_IN_MEASUREMENT_DECREMENT();
#endif
}

void
SCOREP_Libwrap_ExitRegion( SCOREP_RegionHandle region )
{
    SCOREP_ExitRegion( region );
}

void
SCOREP_Libwrap_EnterWrapper( SCOREP_RegionHandle region )
{
    /* The increment is needed, to account this function also as part of the wrapper */
#if HAVE( RETURN_ADDRESS )
    SCOREP_IN_MEASUREMENT_INCREMENT();
#endif
    if ( SCOREP_IsUnwindingEnabled() )
    {
        SCOREP_EnterWrapper( region );
    }
#if HAVE( RETURN_ADDRESS )
    SCOREP_IN_MEASUREMENT_DECREMENT();
#endif
}

void
SCOREP_Libwrap_ExitWrapper( SCOREP_RegionHandle region )
{
    if ( SCOREP_IsUnwindingEnabled() )
    {
        SCOREP_ExitWrapper( region );
    }
}

int
SCOREP_Libwrap_EnterWrappedRegion( void )
{
    SCOREP_ENTER_WRAPPED_REGION();
#if HAVE( THREAD_LOCAL_STORAGE )
    return scorep_in_measurement_save;
#else /* !HAVE( THREAD_LOCAL_STORAGE ) */
    return 0;
#endif
}

void
SCOREP_Libwrap_ExitWrappedRegion( int previous )
{
#if HAVE( THREAD_LOCAL_STORAGE )
    sig_atomic_t scorep_in_measurement_save = previous;
#endif
    SCOREP_EXIT_WRAPPED_REGION();
}
