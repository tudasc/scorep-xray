/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2012,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2012,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2012,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2012,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2012, 2014,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2012,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

#ifndef SCOREP_PROFILING_TASKING
#define SCOREP_PROFILING_TASKING

/**
 * @file
 *
 * @brief   Declaration of the profile interface for tasking support.
 *
 */

#include <stdint.h>
#include <SCOREP_Location.h>
#include <SCOREP_Types.h>

/**
   Implements the handling of a task switch event.
   @param thread       Location this event happened on.
   @param timestamp    Timestamp of this event.
   @param metricValues Array of the dense metric values.
   @param taskHandle   Handle of the task the runtime switched to.
 */
void
SCOREP_Profile_TaskSwitch( SCOREP_Location*  thread,
                           uint64_t          timestamp,
                           uint64_t*         metricValues,
                           SCOREP_TaskHandle taskHandle );

/**
   Implements the handling of a task begin event.
   @param thread       Location this event happened on.
   @param timestamp    Timestamp of this event.
   @param metricValues Array of the dense metric values.
   @param regionHandle Region handle of the task region.
   @param taskHandle   Handle of the completed task.
 */
void
SCOREP_Profile_TaskBegin( SCOREP_Location*    thread,
                          uint64_t            timestamp,
                          uint64_t*           metricValues,
                          SCOREP_RegionHandle regionHandle,
                          SCOREP_TaskHandle   taskHandle );

/**
   Implements the handling of a task end event.
   @param thread       Location this event happened on.
   @param timestamp    Timestamp of this event.
   @param metricValues Array of the dense metric values.
   @param regionHandle Region handle of the task region.
   @param taskHandle   Handle of the completed task.
 */
void
SCOREP_Profile_TaskEnd( SCOREP_Location*    thread,
                        uint64_t            timestamp,
                        uint64_t*           metricValues,
                        SCOREP_RegionHandle regionHandle,
                        SCOREP_TaskHandle   taskHandle );

/**
 * Creates a object of profile task data.
 * The profile data is only created when the task starts execution.
 * @param location     Location on which the data is created.
 * @param taskHandle   Handle to the new task.
 */
void
SCOREP_Profile_CreateTaskData( SCOREP_Location*  location,
                               SCOREP_TaskHandle taskHandle );


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
