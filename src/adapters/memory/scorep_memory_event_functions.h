/**
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2016,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2016,
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
#include <SCOREP_Libwrap_Macros.h>
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

/* Declaration of the mangled real functions new and delete */

void*
__real__Znwm( size_t size );

void*
__real__Znwj( size_t size );

void
__real__ZdlPv( void* ptr );

void*
__real__Znam( size_t size );

void*
__real__Znaj( size_t size );

void
__real__ZdaPv( void* ptr );


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
         !SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) || \
         !scorep_memory_recording ) \
    { \
        SCOREP_IN_MEASUREMENT_DECREMENT(); \
        return __real_##FUNCTION( size ); \
    } \
 \
    UTILS_DEBUG_ENTRY( "%zu", size ); \
 \
    scorep_memory_attributes_add_enter_alloc_size( size ); \
    SCOREP_EnterWrappedRegion( scorep_memory_regions[ SCOREP_MEMORY_##REGION ], \
                               ( intptr_t )__real_##FUNCTION ); \
 \
    SCOREP_ENTER_WRAPPED_REGION(); \
    void* result = __real_##FUNCTION( size ); \
    SCOREP_EXIT_WRAPPED_REGION(); \
 \
    if ( result ) \
    { \
        SCOREP_AllocMetric_HandleAlloc( scorep_memory_metric, \
                                        ( uint64_t )result, \
                                        size ); \
    } \
 \
    scorep_memory_attributes_add_exit_return_address( ( uint64_t )result ); \
    SCOREP_ExitRegion( scorep_memory_regions[ SCOREP_MEMORY_##REGION ] ); \
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
         !SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) || \
         !scorep_memory_recording ) \
    { \
        SCOREP_IN_MEASUREMENT_DECREMENT(); \
        __real_##FUNCTION( ptr ); \
        return; \
    } \
 \
    UTILS_DEBUG_ENTRY( "%p", ptr ); \
 \
    scorep_memory_attributes_add_enter_argument_address( ( uint64_t )ptr ); \
    SCOREP_EnterWrappedRegion( scorep_memory_regions[ SCOREP_MEMORY_##REGION ], \
                               ( intptr_t )__real_##FUNCTION ); \
 \
    void* allocation = NULL; \
    if ( ptr ) \
    { \
        SCOREP_AllocMetric_AcquireAlloc( scorep_memory_metric, \
                                         ( uint64_t )ptr, &allocation ); \
    } \
 \
    SCOREP_ENTER_WRAPPED_REGION(); \
    __real_##FUNCTION( ptr ); \
    SCOREP_EXIT_WRAPPED_REGION(); \
 \
    uint64_t dealloc_size = 0; \
    if ( ptr ) \
    { \
        SCOREP_AllocMetric_HandleFree( scorep_memory_metric, \
                                       allocation, &dealloc_size ); \
    } \
 \
    scorep_memory_attributes_add_exit_dealloc_size( dealloc_size ); \
    SCOREP_ExitRegion( scorep_memory_regions[ SCOREP_MEMORY_##REGION ] ); \
 \
    UTILS_DEBUG_EXIT(); \
    SCOREP_IN_MEASUREMENT_DECREMENT(); \
}
/* *INDENT-ON* */

#endif /* SCOREP_MEMORY_EVENT_FUNCTIONS_H */
