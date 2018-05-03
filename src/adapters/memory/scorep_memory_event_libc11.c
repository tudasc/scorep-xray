/**
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2016-2017,
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
         !SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_IN_MEASUREMENT_DECREMENT();
        return SCOREP_LIBWRAP_FUNC_CALL( aligned_alloc, ( alignment, size ) );
    }

    UTILS_DEBUG_ENTRY( "%zu, %zu", alignment, size );

    if ( scorep_memory_recording )
    {
        scorep_memory_attributes_add_enter_alloc_size( size );
        SCOREP_EnterWrappedRegion( scorep_memory_regions[ SCOREP_MEMORY_ALIGNED_ALLOC ] );
    }
    else if ( SCOREP_IsUnwindingEnabled() )
    {
        SCOREP_EnterWrapper( scorep_memory_regions[ SCOREP_MEMORY_ALIGNED_ALLOC ] );
    }

    SCOREP_ENTER_WRAPPED_REGION();
    void* result = SCOREP_LIBWRAP_FUNC_CALL( aligned_alloc, ( alignment, size ) );
    SCOREP_EXIT_WRAPPED_REGION();

    if ( scorep_memory_recording )
    {
        if ( result )
        {
            SCOREP_AllocMetric_HandleAlloc( scorep_memory_metric,
                                            ( uint64_t )result,
                                            size );
        }

        scorep_memory_attributes_add_exit_return_address( ( uint64_t )result );
        SCOREP_ExitRegion( scorep_memory_regions[ SCOREP_MEMORY_ALIGNED_ALLOC ] );
    }
    else if ( SCOREP_IsUnwindingEnabled() )
    {
        SCOREP_ExitWrapper( scorep_memory_regions[ SCOREP_MEMORY_ALIGNED_ALLOC ] );
    }

    UTILS_DEBUG_EXIT( "%zu, %zu, %p", alignment, size, result );
    SCOREP_IN_MEASUREMENT_DECREMENT();
    return result;
}
