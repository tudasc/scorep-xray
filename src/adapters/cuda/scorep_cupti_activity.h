/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2014,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2013,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2013,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2013,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 *  @file
 *
 *  Propagation of the CUPTI activity control functions.
 */

#ifndef SCOREP_CUPTI_ACTIVITY_H
#define SCOREP_CUPTI_ACTIVITY_H

#include "scorep_cupti.h"    /* CUPTI common structures, functions, etc. */

/* CUPTI >= 4 uses CUpti_ActivityKernel2 instead of CUpti_ActivityKernel */
#if ( defined( CUPTI_API_VERSION ) && ( CUPTI_API_VERSION >= 4 ) )
# if ( defined( CUPTI_API_VERSION ) && ( CUPTI_API_VERSION >= 8 ) )
typedef CUpti_ActivityKernel3 CUpti_ActivityKernelType;
# else
typedef CUpti_ActivityKernel2 CUpti_ActivityKernelType;
# endif
#else
typedef CUpti_ActivityKernel CUpti_ActivityKernelType;
#endif

/* Defines CUPTI activity states to monitor enable/disable state */
#define SCOREP_CUPTI_ACTIVITY_STATE_KERNEL                ( 1 << 0 )
#define SCOREP_CUPTI_ACTIVITY_STATE_CONCURRENT_KERNEL     ( 1 << 1 )
#define SCOREP_CUPTI_ACTIVITY_STATE_MEMCPY                ( 1 << 2 )

/**
 * enable state of individual CUPTI activity types
 */
extern uint8_t scorep_cupti_activity_state;

/**
 * Initialize the Score-P CUPTI Activity implementation.
 *
 * We assume that this function cannot be executed concurrently by multiple
 * threads.
 */
void
scorep_cupti_activity_init( void );

/**
 * Finalize the Score-P CUPTI Activity implementation.
 *
 * We assume that this function cannot be executed concurrently by multiple
 * threads.
 */
void
scorep_cupti_activity_finalize( void );

/*
 * Enable/Disable recording of CUPTI activities. Use CUPTI mutex to lock this
 * function.
 *
 * @param enable 1 to enable recording of activities, 0 to disable
 */
void
scorep_cupti_activity_enable( bool enable );

/*
 * Finalize the Score-P CUPTI activity context.
 *
 * @param context the Score-P CUPTI context, which contains the activities
 */
void
scorep_cupti_activity_context_finalize( scorep_cupti_context* context );

/*
 * Setup a the Score-P CUPTI activity context. Trigger initialization and
 * enqueuing of the CUPTI activity buffer for the given context.
 *
 * @param context the Score-P CUPTI context
 */
void
scorep_cupti_activity_context_setup( scorep_cupti_context* context );

/*
 * Check for empty activity buffer.
 *
 * @param context Score-P CUDA context
 *
 * @return 1 for empty, 0 for non-empty buffer
 */
bool
scorep_cupti_activity_is_buffer_empty( scorep_cupti_context* context );

/*
 * Handle activities buffered by CUPTI. Lock a call to this routine!!!
 *
 * NVIDIA:
 * "Global Queue: The global queue collects all activity records that
 * are not associated with a valid context. All API activity records
 * are collected in the global queue. A buffer is enqueued in the
 * global queue by specifying \p context == NULL.
 *
 * Context Queue: Each context queue collects activity records
 * associated with that context that are not associated with a
 * specific stream or that are associated with the default stream
 * (stream ID 0). A buffer is enqueued in a context queue by
 * specifying the context and a stream ID of 0.
 *
 * Stream Queue: Each stream queue collects memcpy, memset, and kernel
 * activity records associated with the stream. A buffer is enqueued
 * in a stream queue by specifying a context and a non-zero stream ID."
 *
 * @param context Score-P CUPTI context, NULL to handle globally buffered
 * activities
 */
void
scorep_cupti_activity_context_flush( scorep_cupti_context* context );

#endif /* SCOREP_CUPTI_ACTIVITY_H */
