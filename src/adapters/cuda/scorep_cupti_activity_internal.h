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
 *  @file
 *
 *  This file provides macros and functions needed internally for CUPTI activity
 *  handling.
 */

#ifndef SCOREP_CUPTI_ACTIVITY_INTERNAL_H
#define SCOREP_CUPTI_ACTIVITY_INTERNAL_H

#include <stdint.h>
#include <cupti.h>

#include <SCOREP_Types.h>

/* reduce buffer size for alignment, if necessary */
#define SCOREP_CUPTI_ACTIVITY_ALIGN_SIZE ( 8 )
#define SCOREP_CUPTI_ACTIVITY_ALIGN_BUFFER( buffer ) \
    ( ( ( uintptr_t )( buffer ) & ( ( SCOREP_CUPTI_ACTIVITY_ALIGN_SIZE )-1 ) ) ? \
      ( ( buffer ) - ( ( uintptr_t )( buffer ) & ( ( SCOREP_CUPTI_ACTIVITY_ALIGN_SIZE )-1 ) ) ) : ( buffer ) )

/* Score-P region handle to record buffer flush as measurement overhead */
extern SCOREP_RegionHandle scorep_cupti_buffer_flush_region_handle;

/*
 * Check if the chunk size for the CUPTI activity buffer has a valid size.
 */
void
scorep_cupti_activity_check_chunk_size( void );

/*
 * Register buffer request and complete callbacks for CUPTI activity
 * asynchronous buffer handling.
 */
void
scorep_cupti_activity_register_callbacks( void );

/*
 * Create a Score-P CUPTI activity context.
 *
 * @param cudaContext the CUDA context
 *
 * @return pointer to created Score-P CUPTI Activity context
 */
scorep_cupti_activity*
scorep_cupti_activity_context_create( CUcontext cudaContext );

/*
 * Set the synchronization point for a Score-P CUPTI activity context.
 *
 * @param gpu
 * @param host
 */
void
scorep_cupti_set_synchronization_point( uint64_t* gpu,
                                        uint64_t* host );

/*
 * Use the CUPTI activity kernel record to write the corresponding Score-P
 * events.
 *
 * @param kernel the CUPTI activity kernel record
 * @param context the Score-P CUPTI activity context
 */
void
scorep_cupti_activity_write_kernel( CUpti_ActivityKernelType* kernel,
                                    scorep_cupti_context*     context );

/*
 * Use the CUPTI activity memory copy record to write the corresponding
 * Score-P events.
 *
 * @param memcpy the CUPTI activity memory copy record
 * @param context the Score-P CUPTI activity context
 */
void
scorep_cupti_activity_write_memcpy( CUpti_ActivityMemcpy* memcpy,
                                    scorep_cupti_context* context );

#endif /* SCOREP_CUPTI_ACTIVITY_INTERNAL_H */
