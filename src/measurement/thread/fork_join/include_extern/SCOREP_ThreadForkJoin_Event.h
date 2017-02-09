/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2013, 2016-2017,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2014,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

#ifndef SCOREP_THREAD_FORK_JOIN_EVENT_H
#define SCOREP_THREAD_FORK_JOIN_EVENT_H

/**
 * @file
 */


#include <SCOREP_Types.h>

#include <stdint.h>

struct scorep_thread_private_data;

/**
 * Notify the measurement system about the creation of a fork-join
 * parallel execution with at max @a nRequestedThreads new
 * threads. This function needs to be triggered for every thread
 * creation in a fork-join model, e.g., #pragma omp parallel in OpenMP
 * (for create-wait models see SCOREP_ThreadCreate()).
 * SCOREP_ThreadForkJoin_Fork() needs to be called outside the parallel
 * execution from the thread creating the parallel region.
 *
 * @param paradigm One of the predefined threading models.
 *
 * @param nRequestedThreads Upper bound of threads that comprise the
 * parallel region to be created.
 *
 * @note All threads in the following parallel region including the
 * master/creator need to call SCOREP_ThreadForkJoin_TeamBegin() and
 * SCOREP_ThreadForkJoin_TeamEnd().
 *
 * @note After execution of the parallel region the master/creator
 * needs to call SCOREP_ThreadForkJoin_Join().
 *
 * @see SCOREP_ThreadCreate()
 */
void
SCOREP_ThreadForkJoin_Fork( SCOREP_ParadigmType paradigm,
                            uint32_t            nRequestedThreads );


/**
 * Notify the measurement system about the completion of a fork-join
 * parallel execution. The parallel execution was started by a call to
 * SCOREP_ThreadForkJoin_Fork().
 *
 * @param paradigm One of the predefined threading models.
 * @param[out] tpdFromNowOn This thread's TPD after the join..
 *
 * @note See the notes to SCOREP_ThreadForkJoin_Fork().
 */
void
SCOREP_ThreadForkJoin_Join( SCOREP_ParadigmType                 paradigm,
                            struct scorep_thread_private_data** tpdFromNowOn );


/**
 * Notify the measurement system about the begin of a parallel
 * execution on a thread created by either SCOREP_ThreadForkJoin_Fork() or
 * SCOREP_ThreadCreate(). In case of SCOREP_ThreadForkJoin_Fork() all created
 * threads including the master must call SCOREP_ThreadForkJoin_TeamBegin().
 *
 * @param paradigm One of the fork-join threading models.
 *
 * @param threadId Id out of [0..teamSize) within the team of threads that
 * constitute the parallel region.
 *
 * @param teamSize Number of threads that constitute the parallel region,
 * needs to be > 0.
 *
 * @param nestingLevel Nesting level of the current parallel region starting
 * at 1 for the first parallel region. Value of 0 has special meaning, see
 * below.
 *
 * @param ancestorInfo Data to obtain this thread's parent. If
 * @a nestingLevel >= 1, @a ancestorInfo is treated as an array of type
 * int and size @a nestingLevel. If nestingLevel == 0, @a ancestorInfo
 * is treated as this thread's parent scorep_thread_private_data*.
 *
 * @param[out] newTpd Pointer to the new thread's thread private data
 * handle.
 *
 * @param[out] newTask Pointer to the new thread's task handle.
 *
 * @note The end of the parallel execution will be signaled by a call
 * to SCOREP_ThreadForkJoin_TeamEnd().
 *
 * @note Per convention and as there is no parallelism for the initial
 * thread we don't call SCOREP_ThreadForkJoin_TeamBegin() and
 * SCOREP_ThreadForkJoin_TeamEnd() for the initial thread.
 */
void
SCOREP_ThreadForkJoin_TeamBegin( SCOREP_ParadigmType                 paradigm,
                                 uint32_t                            threadId,
                                 uint32_t                            teamSize,
                                 uint32_t                            nestingLevel,
                                 void*                               ancestorInfo,
                                 struct scorep_thread_private_data** newTpd,
                                 SCOREP_TaskHandle*                  newTask );


/**
 * Notify the measurement system about the end of a parallel execution
 * on a thread created by either SCOREP_ThreadForkJoin_Fork() or
 * SCOREP_ThreadCreate(). Every thread that started a parallel
 * execution via SCOREP_ThreadForkJoin_TeamBegin() needs to end via
 * SCOREP_ThreadForkJoin_TeamEnd().
 *
 * @param paradigm One of the fork-join threading models.
 *
 * @param threadId Id out of [0..teamSize) within the team of threads that
 * constitute the parallel region.
 *
 * @param teamSize Number of threads that constitute the parallel region,
 * needs to be > 0.
 *
 * @note The begin of the parallel execution was signalled by a call
 * to SCOREP_ThreadForkJoin_TeamBegin().
 *
 * @note Per convention and as there is no parallelism for the initial
 * thread we don't call SCOREP_ThreadForkJoin_TeamBegin() and
 * SCOREP_ThreadForkJoin_TeamEnd() for the initial thread.
 */
void
SCOREP_ThreadForkJoin_TeamEnd( SCOREP_ParadigmType paradigm,
                               int                 threadId,
                               int                 teamSize );




/**
 * Process a task create event in the measurement system.
 *
 * @param paradigm         One of the predefined threading models.
 * @param threadId         Id of the this thread within the team of
 *                         threads that constitute the parallel region.
 * @param generationNumber The sequence number for this task. Each task
 *                         gets a thread private generation number of the
 *                         creating thread attached. Combined with the
 *                         @a threadId, this constitutes a unique task ID
 *                         inside the parallel region.
 */
void
SCOREP_ThreadForkJoin_TaskCreate( SCOREP_ParadigmType paradigm,
                                  uint32_t            threadId,
                                  uint32_t            generationNumber );


/**
 * Process a task switch event in the measurement system.
 *
 * @param paradigm         One of the predefined threading models.
 * @param task             A handle to the resumed task.
 */
void
SCOREP_ThreadForkJoin_TaskSwitch( SCOREP_ParadigmType paradigm,
                                  SCOREP_TaskHandle   task );


/**
 * Process a task begin event in the measurement system.
 *
 * @param paradigm         One of the predefined threading models.
 * @param regionHandle     Region handle of the task region.
 * @param threadId         Id of the this thread within the team of
 *                         threads that constitute the parallel region.
 * @param generationNumber The sequence number for this task. Each task created
 *                         gets a thread private generation number attached.
 *                         Combined with the @a threadId, this constitutes a
 *                         unique task ID inside the parallel region.
 * @returns a handle to the started task that must be passed to following task
 *          events.
 */
SCOREP_TaskHandle
SCOREP_ThreadForkJoin_TaskBegin( SCOREP_ParadigmType paradigm,
                                 SCOREP_RegionHandle regionHandle,
                                 uint32_t            threadId,
                                 uint32_t            generationNumber );

/**
 * Process a task end event in the measurement system.
 *
 * @param paradigm         One of the predefined threading models.
 * @param regionHandle     Region handle of the task region.
 * @param task             A handle to the completed task.
 */
void
SCOREP_ThreadForkJoin_TaskEnd( SCOREP_ParadigmType paradigm,
                               SCOREP_RegionHandle regionHandle,
                               SCOREP_TaskHandle   task );


#endif /* SCOREP_THREAD_FORK_JOIN_EVENT_H */
