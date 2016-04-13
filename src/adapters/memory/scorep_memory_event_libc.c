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

/**
 * Implementation of __wrap_* functions for libc used by the memory library wrapper
 */

#include <config.h>

#include "scorep_memory_event_functions.h"

SCOREP_MEMORY_WRAP_MALLOC( malloc, MALLOC )
SCOREP_MEMORY_WRAP_FREE( free, FREE )
SCOREP_MEMORY_WRAP_MALLOC( valloc, VALLOC )

void*
SCOREP_LIBWRAP_FUNC_NAME( calloc )( size_t nmemb,
                                    size_t size )
{
    bool trigger = SCOREP_IN_MEASUREMENT_TEST_AND_INCREMENT();
    if ( !trigger ||
         !SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) ||
         !scorep_memory_recording )
    {
        SCOREP_IN_MEASUREMENT_DECREMENT();
        return __real_calloc( nmemb, size );
    }

    UTILS_DEBUG_ENTRY( "%zu, %zu", nmemb, size );

    scorep_memory_attributes_add_enter_alloc_size( nmemb * size );
    SCOREP_EnterWrappedRegion( scorep_memory_regions[ SCOREP_MEMORY_CALLOC ],
                               ( intptr_t )__real_calloc );

    SCOREP_ENTER_WRAPPED_REGION();
    void* result = __real_calloc( nmemb, size );
    SCOREP_EXIT_WRAPPED_REGION();

    if ( result )
    {
        SCOREP_AllocMetric_HandleAlloc( scorep_memory_metric,
                                        ( uint64_t )result,
                                        nmemb * size );
    }

    scorep_memory_attributes_add_exit_return_address( ( uint64_t )result );
    SCOREP_ExitRegion( scorep_memory_regions[ SCOREP_MEMORY_CALLOC ] );

    UTILS_DEBUG_EXIT( "%zu, %zu, %p", nmemb, size, result );
    SCOREP_IN_MEASUREMENT_DECREMENT();
    return result;
}

void*
SCOREP_LIBWRAP_FUNC_NAME( realloc )( void*  ptr,
                                     size_t size )
{
    bool trigger = SCOREP_IN_MEASUREMENT_TEST_AND_INCREMENT();
    if ( !trigger ||
         !SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) ||
         !scorep_memory_recording )
    {
        SCOREP_IN_MEASUREMENT_DECREMENT();
        return __real_realloc( ptr, size );
    }

    UTILS_DEBUG_ENTRY( "%p, %zu", ptr, size );

    /* The size belongs to to the result ptr in
     * scorep_memory_attributes_add_exit_return_address */
    scorep_memory_attributes_add_enter_alloc_size( size );
    /* The ptr belongs to dealloc_size in
     * scorep_memory_attributes_add_exit_dealloc_size. */
    scorep_memory_attributes_add_enter_argument_address( ( uint64_t )ptr );
    SCOREP_EnterWrappedRegion( scorep_memory_regions[ SCOREP_MEMORY_REALLOC ],
                               ( intptr_t )__real_realloc );

    void* allocation = NULL;
    if ( ptr )
    {
        SCOREP_AllocMetric_AcquireAlloc( scorep_memory_metric,
                                         ( uint64_t )ptr, &allocation );
    }

    SCOREP_ENTER_WRAPPED_REGION();
    void* result = __real_realloc( ptr, size );
    SCOREP_EXIT_WRAPPED_REGION();

    /*
     * If ptr is a null pointer, than it is like malloc.
     */
    if ( ptr == NULL && result )
    {
        SCOREP_AllocMetric_HandleAlloc( scorep_memory_metric,
                                        ( uint64_t )result,
                                        size );
        scorep_memory_attributes_add_exit_return_address( ( uint64_t )result );
    }
    /*
     * If size equals zero and ptr != NULL, than it is like free.
     */
    else if ( ptr != NULL && size == 0 )
    {
        uint64_t dealloc_size = 0;
        SCOREP_AllocMetric_HandleFree( scorep_memory_metric,
                                       allocation, &dealloc_size );
        scorep_memory_attributes_add_exit_dealloc_size( dealloc_size );
    }
    /* Otherwise it is a realloc, treat as realloc on success, ... */
    else if ( result )
    {
        uint64_t dealloc_size = 0;
        SCOREP_AllocMetric_HandleRealloc( scorep_memory_metric,
                                          ( uint64_t )result,
                                          size,
                                          allocation,
                                          &dealloc_size );
        scorep_memory_attributes_add_exit_dealloc_size( dealloc_size );
        scorep_memory_attributes_add_exit_return_address( ( uint64_t )result );
    }
    /* ... otherwise, realloc failed, ptr is not touched. *
     */
    else
    {
        scorep_memory_attributes_add_exit_return_address( 0 );
    }

    SCOREP_ExitRegion( scorep_memory_regions[ SCOREP_MEMORY_REALLOC ] );

    UTILS_DEBUG_EXIT( "%p, %zu, %p", ptr, size, result );
    SCOREP_IN_MEASUREMENT_DECREMENT();
    return result;
}

void*
SCOREP_LIBWRAP_FUNC_NAME( memalign )( size_t alignment,
                                      size_t size )
{
    bool trigger = SCOREP_IN_MEASUREMENT_TEST_AND_INCREMENT();
    if ( !trigger ||
         !SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) ||
         !scorep_memory_recording )
    {
        SCOREP_IN_MEASUREMENT_DECREMENT();
        return __real_memalign( alignment, size );
    }

    UTILS_DEBUG_ENTRY( "%zu, %zu", alignment, size );

    scorep_memory_attributes_add_enter_alloc_size( size );
    SCOREP_EnterWrappedRegion( scorep_memory_regions[ SCOREP_MEMORY_MEMALIGN ],
                               ( intptr_t )__real_memalign );

    SCOREP_ENTER_WRAPPED_REGION();
    void* result = __real_memalign( alignment, size );
    SCOREP_EXIT_WRAPPED_REGION();

    if ( result )
    {
        SCOREP_AllocMetric_HandleAlloc( scorep_memory_metric,
                                        ( uint64_t )result,
                                        size );
    }

    scorep_memory_attributes_add_exit_return_address( ( uint64_t )result );
    SCOREP_ExitRegion( scorep_memory_regions[ SCOREP_MEMORY_MEMALIGN ] );

    UTILS_DEBUG_EXIT( "%zu, %zu, %p", alignment, size, result );
    SCOREP_IN_MEASUREMENT_DECREMENT();
    return result;
}

int
SCOREP_LIBWRAP_FUNC_NAME( posix_memalign )( void** ptr,
                                            size_t alignment,
                                            size_t size )
{
    bool trigger = SCOREP_IN_MEASUREMENT_TEST_AND_INCREMENT();
    if ( !trigger ||
         !SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) ||
         !scorep_memory_recording )
    {
        SCOREP_IN_MEASUREMENT_DECREMENT();
        return __real_posix_memalign( ptr, alignment, size );
    }

    UTILS_DEBUG_ENTRY( "%zu, %zu", alignment, size );

    scorep_memory_attributes_add_enter_alloc_size( size );
    SCOREP_EnterWrappedRegion( scorep_memory_regions[ SCOREP_MEMORY_POSIX_MEMALIGN ],
                               ( intptr_t )__real_posix_memalign );

    SCOREP_ENTER_WRAPPED_REGION();
    int result = __real_posix_memalign( ptr, alignment, size );
    SCOREP_EXIT_WRAPPED_REGION();

    if ( result == 0 && *ptr )
    {
        SCOREP_AllocMetric_HandleAlloc( scorep_memory_metric,
                                        ( uint64_t )*ptr,
                                        size );
    }

    scorep_memory_attributes_add_exit_return_address( ( uint64_t )*ptr );
    SCOREP_ExitRegion( scorep_memory_regions[ SCOREP_MEMORY_POSIX_MEMALIGN ] );

    UTILS_DEBUG_EXIT( "%zu, %zu, %p", alignment, size, result );
    SCOREP_IN_MEASUREMENT_DECREMENT();
    return result;
}
