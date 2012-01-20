/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
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
#include <SCOREP_Thread_Types.h>

/**
   Implements the handling of a task creation event.
   @param location     Location this event happened on.
   @param taskId       Id of the created task.
   @param timestamp    Timestamp of this event.
 */
void
SCOREP_Profile_TaskCreate( SCOREP_Thread_LocationData* location,
                           uint64_t                    timestamp,
                           uint64_t                    taskId );


/**
   Implements the handling of a task switch event.
   @param location     Location this event happened on.
   @param taskId       Id of the task the runtime switched to.
   @param timestamp    Timestamp of this event.
 */
void
SCOREP_Profile_TaskSwitch( SCOREP_Thread_LocationData* location,
                           uint64_t                    timestamp,
                           uint64_t                    taskId );


/**
   Implements the handling of a task completion event.
   @param location     Location this event happened on.
   @param taskId       Id of the completed task.
   @param timestamp    Timestamp of this event.
 */
void
SCOREP_Profile_TaskComplete( SCOREP_Thread_LocationData* location,
                             uint64_t                    timestamp,
                             uint64_t                    taskId );



#endif /* SCOREP_PROFILING_TASKING */
