/**
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2016-2017, 2019,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2016-2017,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 */

#ifndef SCOREP_MEMORY_EVENT_FUNCTIONS_H
#define SCOREP_MEMORY_EVENT_FUNCTIONS_H

/**
 * Declaration of all __real_* functions used by the memory library wrapper
 */

#include <malloc.h>

#include "scorep_memory_mgmt.h"
#include "scorep_memory_attributes.h"
#include <scorep/SCOREP_Libwrap_Macros.h>
#include <SCOREP_InMeasurement.h>
#include <SCOREP_RuntimeManagement.h>

#define SCOREP_DEBUG_MODULE_NAME MEMORY
#include <UTILS_Debug.h>

void*
__real_malloc( size_t size );

void
__real_free( void* ptr );

void*
__real_calloc( size_t nmemb,
               size_t size );

void*
__real_realloc( void*  ptr,
                size_t size );

void*
__real_memalign( size_t alignment,
                 size_t size );

int
__real_posix_memalign( void** ptr,
                       size_t alignment,
                       size_t size );

void*
__real_valloc( size_t size );

void*
__real_aligned_alloc( size_t alignment,
                      size_t size );

/* allocation functions of hbwmalloc hbw_* */
void*
__real_hbw_malloc( size_t size );

void
__real_hbw_free( void* ptr );

void*
__real_hbw_calloc( size_t nmemb,
                   size_t size );

void*
__real_hbw_realloc( void*  ptr,
                    size_t size );

int
__real_hbw_posix_memalign( void** ptr,
                           size_t alignment,
                           size_t size );

int
__real_hbw_posix_memalign_psize( void** ptr,
                                 size_t alignment,
                                 size_t size,
                                 int    pagesize );

/* Declaration of the mangled real functions new and delete */

void*
__real__Znwm( size_t size );

void*
__real__Znwj( size_t size );

void
__real__ZdlPv( void* ptr );

void
__real__ZdlPvm( void*  ptr,
                size_t size );

void
__real__ZdlPvj( void*  ptr,
                size_t size );

void*
__real__Znam( size_t size );

void*
__real__Znaj( size_t size );

void
__real__ZdaPv( void* ptr );

void
__real__ZdaPvm( void*  ptr,
                size_t size );

void
__real__ZdaPvj( void*  ptr,
                size_t size );


/* Declaration of the mangled real functions new and delete (old PGI/EDG C++ ABI) */

void*
__real___nw__FUi( size_t size );

void*
__real___nw__FUl( size_t size );

void
__real___dl__FPv( void* ptr );

void*
__real___nwa__FUi( size_t size );

void*
__real___nwa__FUl( size_t size );

void
__real___dla__FPv( void* ptr );


/* *INDENT-OFF* */
#define SCOREP_MEMORY_WRAP_MALLOC( FUNCTION, REGION ) \
void* \
SCOREP_LIBWRAP_FUNC_NAME( FUNCTION )( size_t size ) \
{ \
    bool trigger = SCOREP_IN_MEASUREMENT_TEST_AND_INCREMENT(); \
    if ( !trigger || \
         !SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) ) \
    { \
        SCOREP_IN_MEASUREMENT_DECREMENT(); \
        return SCOREP_LIBWRAP_FUNC_CALL( FUNCTION, ( size ) ); \
    } \
 \
    UTILS_DEBUG_ENTRY( "%zu", size ); \
 \
    if ( scorep_memory_recording ) \
    { \
        scorep_memory_attributes_add_enter_alloc_size( size ); \
        SCOREP_EnterWrappedRegion( scorep_memory_regions[ SCOREP_MEMORY_##REGION ] ); \
    } \
    else if ( SCOREP_IsUnwindingEnabled() ) \
    { \
        SCOREP_EnterWrapper( scorep_memory_regions[ SCOREP_MEMORY_##REGION ] ); \
    } \
 \
    SCOREP_ENTER_WRAPPED_REGION(); \
    void* result = SCOREP_LIBWRAP_FUNC_CALL( FUNCTION, ( size ) ); \
    SCOREP_EXIT_WRAPPED_REGION(); \
 \
    if ( scorep_memory_recording ) \
    { \
        if ( result ) \
        { \
            SCOREP_AllocMetric_HandleAlloc( scorep_memory_metric, \
                                            ( uint64_t )result, \
                                            size ); \
        } \
 \
        scorep_memory_attributes_add_exit_return_address( ( uint64_t )result ); \
        SCOREP_ExitRegion( scorep_memory_regions[ SCOREP_MEMORY_##REGION ] ); \
    } \
    else if ( SCOREP_IsUnwindingEnabled() ) \
    { \
        SCOREP_ExitWrapper( scorep_memory_regions[ SCOREP_MEMORY_##REGION ] ); \
    } \
 \
    UTILS_DEBUG_EXIT( "%zu, %p", size, result ); \
    SCOREP_IN_MEASUREMENT_DECREMENT(); \
    return result; \
}
/* *INDENT-ON* */

/* *INDENT-OFF* */
#define SCOREP_MEMORY_WRAP_FREE( FUNCTION, REGION ) \
void \
SCOREP_LIBWRAP_FUNC_NAME( FUNCTION )( void* ptr ) \
{ \
    bool trigger = SCOREP_IN_MEASUREMENT_TEST_AND_INCREMENT(); \
    if ( !trigger || \
         !SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) ) \
    { \
        SCOREP_IN_MEASUREMENT_DECREMENT(); \
        SCOREP_LIBWRAP_FUNC_CALL( FUNCTION, ( ptr ) ); \
        return; \
    } \
 \
    UTILS_DEBUG_ENTRY( "%p", ptr ); \
 \
    void* allocation = NULL; \
    if ( scorep_memory_recording ) \
    { \
        scorep_memory_attributes_add_enter_argument_address( ( uint64_t )ptr ); \
        SCOREP_EnterWrappedRegion( scorep_memory_regions[ SCOREP_MEMORY_##REGION ] ); \
 \
        if ( ptr ) \
        { \
            SCOREP_AllocMetric_AcquireAlloc( scorep_memory_metric, \
                                             ( uint64_t )ptr, &allocation ); \
        } \
    } \
    else if ( SCOREP_IsUnwindingEnabled() ) \
    { \
        SCOREP_EnterWrapper( scorep_memory_regions[ SCOREP_MEMORY_##REGION ] ); \
    } \
 \
    SCOREP_ENTER_WRAPPED_REGION(); \
    SCOREP_LIBWRAP_FUNC_CALL( FUNCTION, ( ptr ) ); \
    SCOREP_EXIT_WRAPPED_REGION(); \
 \
    if ( scorep_memory_recording ) \
    { \
        uint64_t dealloc_size = 0; \
        if ( ptr ) \
        { \
            SCOREP_AllocMetric_HandleFree( scorep_memory_metric, \
                                           allocation, &dealloc_size ); \
        } \
 \
        scorep_memory_attributes_add_exit_dealloc_size( dealloc_size ); \
        SCOREP_ExitRegion( scorep_memory_regions[ SCOREP_MEMORY_##REGION ] ); \
    } \
    else if ( SCOREP_IsUnwindingEnabled() ) \
    { \
        SCOREP_ExitWrapper( scorep_memory_regions[ SCOREP_MEMORY_##REGION ] ); \
    } \
 \
    UTILS_DEBUG_EXIT(); \
    SCOREP_IN_MEASUREMENT_DECREMENT(); \
}
/* *INDENT-ON* */


/* *INDENT-OFF* */
#define SCOREP_MEMORY_WRAP_FREE_SIZE( FUNCTION, REGION ) \
void \
SCOREP_LIBWRAP_FUNC_NAME( FUNCTION )( void* ptr, size_t size ) \
{ \
    bool trigger = SCOREP_IN_MEASUREMENT_TEST_AND_INCREMENT(); \
    if ( !trigger || \
         !SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) ) \
    { \
        SCOREP_IN_MEASUREMENT_DECREMENT(); \
        SCOREP_LIBWRAP_FUNC_CALL( FUNCTION, ( ptr, size ) ); \
        return; \
    } \
 \
    UTILS_DEBUG_ENTRY( "%p, %zu", ptr, size ); \
 \
    void* allocation = NULL; \
    if ( scorep_memory_recording ) \
    { \
        scorep_memory_attributes_add_enter_argument_address( ( uint64_t )ptr ); \
        scorep_memory_attributes_add_exit_dealloc_size( size ); \
        SCOREP_EnterWrappedRegion( scorep_memory_regions[ SCOREP_MEMORY_##REGION ] ); \
 \
        if ( ptr ) \
        { \
            SCOREP_AllocMetric_AcquireAlloc( scorep_memory_metric, \
                                             ( uint64_t )ptr, &allocation ); \
        } \
    } \
    else if ( SCOREP_IsUnwindingEnabled() ) \
    { \
        SCOREP_EnterWrapper( scorep_memory_regions[ SCOREP_MEMORY_##REGION ] ); \
    } \
 \
    SCOREP_ENTER_WRAPPED_REGION(); \
    SCOREP_LIBWRAP_FUNC_CALL( FUNCTION, ( ptr, size ) ); \
    SCOREP_EXIT_WRAPPED_REGION(); \
 \
    if ( scorep_memory_recording ) \
    { \
        uint64_t dealloc_size = 0; \
        if ( ptr ) \
        { \
            SCOREP_AllocMetric_HandleFree( scorep_memory_metric, \
                                           allocation, &dealloc_size ); \
        } \
 \
        scorep_memory_attributes_add_exit_dealloc_size( dealloc_size ); \
        SCOREP_ExitRegion( scorep_memory_regions[ SCOREP_MEMORY_##REGION ] ); \
    } \
    else if ( SCOREP_IsUnwindingEnabled() ) \
    { \
        SCOREP_ExitWrapper( scorep_memory_regions[ SCOREP_MEMORY_##REGION ] ); \
    } \
 \
    UTILS_DEBUG_EXIT(); \
    SCOREP_IN_MEASUREMENT_DECREMENT(); \
}
/* *INDENT-ON* */


/* *INDENT-OFF* */
#define SCOREP_MEMORY_WRAP_CALLOC( FUNCTION, REGION ) \
void* \
SCOREP_LIBWRAP_FUNC_NAME( FUNCTION )( size_t nmemb, \
                                      size_t size ) \
{ \
    bool trigger = SCOREP_IN_MEASUREMENT_TEST_AND_INCREMENT(); \
    if ( !trigger || \
         !SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) ) \
    { \
        SCOREP_IN_MEASUREMENT_DECREMENT(); \
        return SCOREP_LIBWRAP_FUNC_CALL( FUNCTION, ( nmemb, size ) ); \
    } \
 \
    UTILS_DEBUG_ENTRY( "%zu, %zu", nmemb, size ); \
 \
    if ( scorep_memory_recording ) \
    { \
        scorep_memory_attributes_add_enter_alloc_size( nmemb * size ); \
        SCOREP_EnterWrappedRegion( scorep_memory_regions[ SCOREP_MEMORY_##REGION ] ); \
    } \
    else if ( SCOREP_IsUnwindingEnabled() ) \
    { \
        SCOREP_EnterWrapper( scorep_memory_regions[ SCOREP_MEMORY_##REGION ] ); \
    } \
 \
    SCOREP_ENTER_WRAPPED_REGION(); \
    void* result = SCOREP_LIBWRAP_FUNC_CALL( FUNCTION, ( nmemb, size ) ); \
    SCOREP_EXIT_WRAPPED_REGION(); \
 \
    if ( scorep_memory_recording ) \
    { \
        if ( result ) \
        { \
            SCOREP_AllocMetric_HandleAlloc( scorep_memory_metric, \
                                            ( uint64_t )result, \
                                            nmemb * size ); \
        } \
 \
        scorep_memory_attributes_add_exit_return_address( ( uint64_t )result ); \
        SCOREP_ExitRegion( scorep_memory_regions[ SCOREP_MEMORY_##REGION ] ); \
    } \
    else if ( SCOREP_IsUnwindingEnabled() ) \
    { \
        SCOREP_ExitWrapper( scorep_memory_regions[ SCOREP_MEMORY_##REGION ] ); \
    } \
 \
    UTILS_DEBUG_EXIT( "%zu, %zu, %p", nmemb, size, result ); \
 \
    SCOREP_IN_MEASUREMENT_DECREMENT(); \
    return result; \
}
/* *INDENT-ON* */

/* *INDENT-OFF* */
#define SCOREP_MEMORY_WRAP_REALLOC( FUNCTION, REGION ) \
void* \
SCOREP_LIBWRAP_FUNC_NAME( FUNCTION )( void*  ptr, \
                                      size_t size ) \
{ \
    bool trigger = SCOREP_IN_MEASUREMENT_TEST_AND_INCREMENT(); \
    if ( !trigger || \
         !SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) ) \
    { \
        SCOREP_IN_MEASUREMENT_DECREMENT(); \
        return SCOREP_LIBWRAP_FUNC_CALL( FUNCTION, ( ptr, size ) ); \
    } \
 \
    UTILS_DEBUG_ENTRY( "%p, %zu", ptr, size ); \
 \
    void* allocation = NULL; \
    if ( scorep_memory_recording ) \
    { \
        /* The size belongs to to the result ptr in \
         * scorep_memory_attributes_add_exit_return_address */ \
        scorep_memory_attributes_add_enter_alloc_size( size ); \
        /* The ptr belongs to dealloc_size in \
         * scorep_memory_attributes_add_exit_dealloc_size. */ \
        scorep_memory_attributes_add_enter_argument_address( (  uint64_t )ptr ); \
        SCOREP_EnterWrappedRegion( scorep_memory_regions[ SCOREP_MEMORY_##REGION ] ); \
 \
        if ( ptr ) \
        { \
            SCOREP_AllocMetric_AcquireAlloc( scorep_memory_metric, \
                                             ( uint64_t )ptr, &allocation ); \
        } \
    } \
    else if ( SCOREP_IsUnwindingEnabled() ) \
    { \
        SCOREP_EnterWrapper( scorep_memory_regions[ SCOREP_MEMORY_##REGION ] ); \
    } \
 \
    SCOREP_ENTER_WRAPPED_REGION(); \
    void* result = SCOREP_LIBWRAP_FUNC_CALL( FUNCTION, ( ptr, size ) ); \
    SCOREP_EXIT_WRAPPED_REGION(); \
 \
    if ( scorep_memory_recording ) \
    { \
        /* \
         * If ptr is a null pointer, than it is like malloc. \
         */ \
        if ( ptr == NULL && result ) \
        { \
            SCOREP_AllocMetric_HandleAlloc( scorep_memory_metric, \
                                            ( uint64_t )result, \
                                            size ); \
            scorep_memory_attributes_add_exit_return_address( ( uint64_t )result ); \
        } \
        /* \
         * If size equals zero and ptr != NULL, than it is like free. \
         */ \
        else if ( ptr != NULL && size == 0 ) \
        { \
            uint64_t dealloc_size = 0; \
            SCOREP_AllocMetric_HandleFree( scorep_memory_metric, \
                                           allocation, &dealloc_size ); \
            scorep_memory_attributes_add_exit_dealloc_size( dealloc_size ); \
        } \
        /* Otherwise it is a realloc, treat as realloc on success, ... */ \
        else if ( result ) \
        { \
            uint64_t dealloc_size = 0; \
            SCOREP_AllocMetric_HandleRealloc( scorep_memory_metric, \
                                              ( uint64_t )result, \
                                              size, \
                                              allocation, \
                                              &dealloc_size ); \
            scorep_memory_attributes_add_exit_dealloc_size( dealloc_size ); \
            scorep_memory_attributes_add_exit_return_address( ( uint64_t )result ); \
        } \
        /* ... otherwise, realloc failed, ptr is not touched. * \
         */ \
        else \
        { \
            scorep_memory_attributes_add_exit_return_address( 0 ); \
        } \
 \
        SCOREP_ExitRegion( scorep_memory_regions[ SCOREP_MEMORY_##REGION ] ); \
    } \
    else if ( SCOREP_IsUnwindingEnabled() ) \
    { \
        SCOREP_ExitWrapper( scorep_memory_regions[ SCOREP_MEMORY_##REGION ] ); \
    } \
 \
    UTILS_DEBUG_EXIT( "%p, %zu, %p", ptr, size, result ); \
    SCOREP_IN_MEASUREMENT_DECREMENT(); \
    return result; \
}
/* *INDENT-ON* */

/* *INDENT-OFF* */
#define SCOREP_MEMORY_WRAP_POSIX_MEMALIGN( FUNCTION, REGION ) \
int \
SCOREP_LIBWRAP_FUNC_NAME( FUNCTION )( void** ptr, \
                                      size_t alignment, \
                                      size_t size ) \
{ \
    bool trigger = SCOREP_IN_MEASUREMENT_TEST_AND_INCREMENT(); \
    if ( !trigger || \
         !SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) ) \
    { \
        SCOREP_IN_MEASUREMENT_DECREMENT(); \
        return SCOREP_LIBWRAP_FUNC_CALL( FUNCTION, ( ptr, alignment, size ) ); \
    } \
 \
    UTILS_DEBUG_ENTRY( "%zu, %zu", alignment, size ); \
 \
    if ( scorep_memory_recording ) \
    { \
        scorep_memory_attributes_add_enter_alloc_size( size ); \
        SCOREP_EnterWrappedRegion( scorep_memory_regions[ SCOREP_MEMORY_##REGION ] ); \
    } \
    else if ( SCOREP_IsUnwindingEnabled() ) \
    { \
        SCOREP_EnterWrapper( scorep_memory_regions[ SCOREP_MEMORY_POSIX_MEMALIGN ] ); \
    } \
 \
    SCOREP_ENTER_WRAPPED_REGION(); \
    int result = SCOREP_LIBWRAP_FUNC_CALL( FUNCTION, ( ptr, alignment, size ) ); \
    SCOREP_EXIT_WRAPPED_REGION(); \
 \
    if ( scorep_memory_recording ) \
    { \
        if ( result == 0 && *ptr ) \
        { \
            SCOREP_AllocMetric_HandleAlloc( scorep_memory_metric, \
                                            ( uint64_t )*ptr, \
                                            size ); \
        } \
 \
        scorep_memory_attributes_add_exit_return_address( ( uint64_t )*ptr ); \
        SCOREP_ExitRegion( scorep_memory_regions[ SCOREP_MEMORY_##REGION ] ); \
    } \
    else if ( SCOREP_IsUnwindingEnabled() ) \
    { \
        SCOREP_ExitWrapper( scorep_memory_regions[ SCOREP_MEMORY_##REGION ] ); \
    } \
 \
    UTILS_DEBUG_EXIT( "%zu, %zu, %p", alignment, size, result ); \
    SCOREP_IN_MEASUREMENT_DECREMENT(); \
    return result; \
}
/* *INDENT-ON* */

#endif /* SCOREP_MEMORY_EVENT_FUNCTIONS_H */
