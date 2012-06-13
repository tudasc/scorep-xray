/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
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
 *  Propagation of the CUPTI activities control functions.
 */

#ifndef SCOREP_CUPTI_ACTIVITY_H
#define SCOREP_CUPTI_ACTIVITY_H

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

/**
 * Create and add a new context to list of contexts.
 *
 * @param cudaContext       CUDA context specifying the queue.
 * @param cudaDevice        CUDA device handle.
 */
extern void
scorep_cupti_activity_add_context( CUcontext cudaContext,
                                   CUdevice  cudaDevice );

/**
 * Handle activities buffered by CUPTI.
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
 * @param cudaContext       CUDA context, NULL to handle globally buffered activities
 */
extern void
scorep_cupti_activity_flush_context_activities( CUcontext cudaContext );

#endif  /* SCOREP_CUPTI_ACTIVITY_H */
