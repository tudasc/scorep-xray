/**
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2017-2018,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 */

/**
 * Implementation of __wrap_* functions for hbwmalloc.h (Intel KNL library) used by the memory library wrapper
 */

#include <config.h>

#include "scorep_memory_event_functions.h"

SCOREP_MEMORY_WRAP_MALLOC( hbw_malloc, HBW_MALLOC )
SCOREP_MEMORY_WRAP_FREE( hbw_free, HBW_FREE )
SCOREP_MEMORY_WRAP_CALLOC( hbw_calloc, HBW_CALLOC )
SCOREP_MEMORY_WRAP_REALLOC( hbw_realloc, HBW_REALLOC )
SCOREP_MEMORY_WRAP_POSIX_MEMALIGN( hbw_posix_memalign, HBW_POSIX_MEMALIGN )


int
SCOREP_LIBWRAP_FUNC_NAME( hbw_posix_memalign_psize )( void** ptr,
                                                      size_t alignment,
                                                      size_t size,
                                                      int    pagesize )
{
    bool trigger = SCOREP_IN_MEASUREMENT_TEST_AND_INCREMENT();
    if ( !trigger ||
         !SCOREP_IS_MEASUREMENT_PHASE( WITHIN ) )
    {
        SCOREP_IN_MEASUREMENT_DECREMENT();
        return SCOREP_LIBWRAP_FUNC_CALL( hbw_posix_memalign_psize, ( ptr, alignment, size, pagesize ) );
    }

    UTILS_DEBUG_ENTRY( "%zu, %zu, %d", alignment, size, pagesize );

    if ( scorep_memory_recording )
    {
        scorep_memory_attributes_add_enter_alloc_size( size );
        SCOREP_EnterWrappedRegion( scorep_memory_regions[ SCOREP_MEMORY_HBW_POSIX_MEMALIGN_PSIZE ] );
    }
    else if ( SCOREP_IsUnwindingEnabled() )
    {
        SCOREP_EnterWrapper( scorep_memory_regions[ SCOREP_MEMORY_HBW_POSIX_MEMALIGN_PSIZE ] );
    }

    SCOREP_ENTER_WRAPPED_REGION();
    int result = SCOREP_LIBWRAP_FUNC_CALL( hbw_posix_memalign_psize, ( ptr, alignment, size, pagesize ) );
    SCOREP_EXIT_WRAPPED_REGION();

    if ( scorep_memory_recording )
    {
        if ( result == 0 && *ptr )
        {
            SCOREP_AllocMetric_HandleAlloc( scorep_memory_metric,
                                            ( uint64_t )*ptr,
                                            size );
        }

        scorep_memory_attributes_add_exit_return_address( ( uint64_t )*ptr );
        SCOREP_ExitRegion( scorep_memory_regions[ SCOREP_MEMORY_HBW_POSIX_MEMALIGN_PSIZE ] );
    }
    else if ( SCOREP_IsUnwindingEnabled() )
    {
        SCOREP_ExitWrapper( scorep_memory_regions[ SCOREP_MEMORY_HBW_POSIX_MEMALIGN_PSIZE ] );
    }

    UTILS_DEBUG_EXIT( "%zu, %zu, %d, %p", alignment, size, pagesize, result );
    SCOREP_IN_MEASUREMENT_DECREMENT();
    return result;
}
