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
   Implements the handling of a task creation event.
   @param thread       Location this event happened on.
   @param timestamp    Timestamp of this event.
   @param taskId       Id of the created task.
 */
void
SCOREP_Profile_TaskCreate( SCOREP_Location* thread,
                           uint64_t         timestamp,
                           uint32_t         threadId,
                           uint32_t         generationNumber );

/**
   Implements the handling of a task switch event.
   @param thread     Location this event happened on.
   @param timestamp    Timestamp of this event.
   @param metricValues Array of the dense metric values.
   @param taskId       Id of the task the runtime switched to.
 */
void
SCOREP_Profile_TaskSwitch( SCOREP_Location* thread,
                           uint64_t         timestamp,
                           uint64_t*        metricValues,
                           uint32_t         threadId,
                           uint32_t         generationNumber );

/**
   Implements the handling of a task begin event.
   @param thread       Location this event happened on.
   @param timestamp    Timestamp of this event.
   @param metricValues Array of the dense metric values.
   @param regionHandle Region handle of the task region.
   @param taskId       Id of the completed task.
 */
void
SCOREP_Profile_TaskBegin( SCOREP_Location*    thread,
                          uint64_t            timestamp,
                          uint64_t*           metricValues,
                          SCOREP_RegionHandle regionHandle,
                          uint32_t            threadId,
                          uint32_t            generationNumber );

/**
   Implements the handling of a task end event.
   @param thread       Location this event happened on.
   @param timestamp    Timestamp of this event.
   @param metricValues Array of the dense metric values.
   @param regionHandle Region handle of the task region.
   @param taskId       Id of the completed task.
 */
void
SCOREP_Profile_TaskEnd( SCOREP_Location*    thread,
                        uint64_t            timestamp,
                        uint64_t*           metricValues,
                        SCOREP_RegionHandle regionHandle,
                        uint32_t            threadId,
                        uint32_t            generationNumber );

#endif /* SCOREP_PROFILING_TASKING */
