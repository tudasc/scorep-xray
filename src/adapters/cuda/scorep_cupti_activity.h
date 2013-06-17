/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 *    RWTH Aachen University, Germany
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *    Technische Universitaet Dresden, Germany
 *    University of Oregon, Eugene, USA
 *    Forschungszentrum Juelich GmbH, Germany
 *    German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *    Technische Universitaet Muenchen, Germany
 *
 * See the COPYING file in the package base directory for details.
 *
 */

/**
 *  @status     alpha
 *  @file       scorep_cupti_activity.h
 *  @maintainer Robert Dietrich <robert.dietrich@zih.tu-dresden.de>
 *
 *  Propagation of the CUPTI activity control functions.
 */

#ifndef SCOREP_CUPTI_ACTIVITY_H
#define SCOREP_CUPTI_ACTIVITY_H

#include "scorep_cupti.h"    /* CUPTI common structures, functions, etc. */

/**
 * Initialize the Score-P CUPTI Activity implementation.
 */
extern void
scorep_cupti_activity_init( void );

/**
 * Finalize the Score-P CUPTI Activity implementation.
 */
extern void
scorep_cupti_activity_finalize( void );

/*
 * Enable/Disable recording of CUPTI activities. Use CUPTI mutex to lock this
 * function.
 *
 * @param enable 1 to enable recording of activities, 0 to disable
 */
extern void
scorep_cupti_activity_enable( bool enable );

/*
 * Finalize the Score-P CUPTI activity context.
 *
 * @param context the Score-P CUPTI context, which contains the activities
 */
extern void
scorep_cupti_activity_context_finalize( scorep_cupti_context_t* context );

/*
 * Setup a the Score-P CUPTI activity context. Trigger initialization and
 * enqueuing of the CUPTI activity buffer for the given context.
 *
 * @param context the Score-P CUPTI context
 */
extern void
scorep_cupti_activity_context_setup( scorep_cupti_context_t* context );

/*
 * Check for empty activity buffer.
 *
 * @param cudaContext CUDA context
 *
 * @return 1 for empty, 0 for non-empty buffer
 */
extern uint8_t
scorep_cupti_activity_is_buffer_empty( CUcontext cudaContext );

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
extern void
scorep_cupti_activity_context_flush( scorep_cupti_context_t* context );

#if ( defined( CUPTI_API_VERSION ) && ( CUPTI_API_VERSION >= 3 ) )
/*
 * Enable tracing of concurrent kernels. Disable normal kernel tracing, if
 * necessary.
 *
 * @param context pointer to the VampirTrace CUPTI context.
 */
extern void
scorep_cupti_activity_enable_concurrent_kernel( scorep_cupti_context_t* context );

#endif

#endif  /* SCOREP_CUPTI_ACTIVITY_H */
