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

void*
SCOREP_LIBWRAP_FUNC_NAME( aligned_alloc )( size_t alignment,
                                           size_t size )
{
    bool trigger = SCOREP_IN_MEASUREMENT_TEST_AND_INCREMENT();
    if ( !trigger ||
         !SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) ||
         !scorep_memory_recording )
    {
        SCOREP_IN_MEASUREMENT_DECREMENT();
        return __real_aligned_alloc( alignment, size );
    }

    UTILS_DEBUG_ENTRY( "%zu, %zu", alignment, size );

    scorep_memory_attributes_add_enter_alloc_size( size );
    SCOREP_EnterWrappedRegion( scorep_memory_regions[ SCOREP_MEMORY_ALIGNED_ALLOC ],
                               ( intptr_t )__real_aligned_alloc );

    SCOREP_ENTER_WRAPPED_REGION();
    void* result = __real_aligned_alloc( alignment, size );
    SCOREP_EXIT_WRAPPED_REGION();

    if ( result )
    {
        SCOREP_AllocMetric_HandleAlloc( scorep_memory_metric,
                                        ( uint64_t )result,
                                        size );
    }

    scorep_memory_attributes_add_exit_return_address( ( uint64_t )result );
    SCOREP_ExitRegion( scorep_memory_regions[ SCOREP_MEMORY_ALIGNED_ALLOC ] );

    UTILS_DEBUG_EXIT( "%zu, %zu, %p", alignment, size, result );
    SCOREP_IN_MEASUREMENT_DECREMENT();
    return result;
}
