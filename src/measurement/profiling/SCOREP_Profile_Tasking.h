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

#ifndef SCOREP_PROFILING_TASKING
#define SCOREP_PROFILING_TASKING

/**
 * @status      alpha
 * @file        SCOREP_Profile_Tasking.h
 *
 * @brief   Declaration of the profile interface for tasking support.
 *
 */

#include <stdint.h>
#include <SCOREP_Location.h>
#include <SCOREP_Types.h>

/**
 * Task instance related data.
 */
typedef struct SCOREP_Profile_Task* SCOREP_Profile_TaskHandle;

/**
 * Implements the handling of a task switch event.
 * @param thread        Location on which this event happens.
 * @param taskHandle    Handle of the task the runtime switched to.
 * @param timestamp     Timestamp of this event.
 * @param metricValues Array of the dense metric values.
 */
void
SCOREP_Profile_TaskSwitch( SCOREP_Location*  thread,
                           uint64_t          timestamp,
                           uint64_t*         metricValues,
                           SCOREP_TaskHandle taskHandle );

/**
 * Implements the handling of a task begin event.
 * @param thread        Location this event happened on.
 * @param region        Region handle of the task region.
 * @param taskHandle    Handle of the completed task.
 * @param timestamp     Timestamp of this event.
 * @param metric_values Array of the dense metric values.
 */
void
SCOREP_Profile_TaskBegin( SCOREP_Location*    thread,
                          uint64_t            timestamp,
                          uint64_t*           metricValues,
                          SCOREP_RegionHandle regionHandle,
                          SCOREP_TaskHandle   taskHandle );

/**
 * Implements the handling of a task end event.
 * @param thread        Location this event happened on.
 * @param region        Region handle of the task region.
 * @param taskHandle    Handle of the completed task.
 * @param timestamp     Timestamp of this event.
 * @param metricValues  Array of the dense metric values.
 */
void
SCOREP_Profile_TaskEnd( SCOREP_Location*    thread,
                        uint64_t            timestamp,
                        uint64_t*           metricValues,
                        SCOREP_RegionHandle regionHandle,
                        SCOREP_TaskHandle   taskHandle );

/**
 * Creates a new task data construct and returns a handle for it.
 * The profile data is only created when the task starts execution.
 * @param location         Location on which the data is created.
 * @param threadId         Id of the this thread within the team of
 *                         threads that constitute the parallel region.
 * @param generationNumber The sequence number for this task. Each task
 *                         gets a thread private generation number of the
 *                         creating thread attached. Combined with the
 *                         @a threadId, this constitutes a unique task ID
 *                         inside the parallel region.
 * @param can_migrate      Indicate whether the task can migrate to another
 *                         location.
 * @returns A handle to the new task data.
 */
SCOREP_TaskHandle
SCOREP_Profile_CreateTaskData( SCOREP_Location* location,
                               uint32_t         threadId,
                               uint32_t         generationNumber,
                               bool             can_migrate );

/**
 * Returns the task identifier number from a task handle.
 * @param taskHandle       The task data.
 * @param threadId         Writes the Id of the this thread within the team of
 *                         threads that constitute the parallel region into this
 *                         memory location.
 * @param generationNumber Writes the sequence number for this task into this
 *                         memory location. Each task
 *                         gets a thread private generation number of the
 *                         creating thread attached. Combined with the
 *                         @a threadId, this constitutes a unique task ID
 *                         inside the parallel region.
 */
void
SCOREP_Profile_GetTaskId( SCOREP_TaskHandle taskHandle,
                          uint32_t*         threadId,
                          uint32_t*         generationNumber );

/**
 * Releases the task data associated with @a taskHandle, and make it
 * available for reuse.
 * @param location    The Score-P location that executes the free.
 * @param taskHandle  The task Handle of the task
 */
void
SCOREP_Profile_FreeTaskData( SCOREP_Location*  location,
                             SCOREP_TaskHandle taskHandle );

#endif /* SCOREP_PROFILING_TASKING */
