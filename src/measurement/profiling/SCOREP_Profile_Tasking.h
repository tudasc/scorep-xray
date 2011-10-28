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

/**
   Implemented the handling of a task creation starting point.
   @param regionHandle The Score-P region handle of the associated source code
                       region. E.g., the task construct enclosing region.
   @param taskId       The task identifier of the newly created task.
   @param timestamp    The timestamp of this event.
 */
void
SCOREP_Profile_TaskCreateBegin( SCOREP_RegionHandle regionHandle,
                                uint64_t            taskId,
                                uint64_t            timestamp );

/**
   Implemented the handling of a task creation end point.
   @param regionHandle The Score-P region handle of the associated source code
                       region. E.g., the task construct enclosing region.
   @param taskId       The task identifier of the creator task.
   @param timestamp    The timestamp of this event.
 */
void
SCOREP_Profile_TaskCreateEnd( SCOREP_RegionHandle regionHandle,
                              uint64_t            taskId,
                              uint64_t            timestamp );

/**
   Implemented the handling of a task begin event.
   @param regionHandle The Score-P region handle of the associated source code
                       region. E.g., the task construct's inner region.
   @param taskId       The task identifier of the new task.
   @param timestamp    The timestamp of this event.
 */
void
SCOREP_Profile_TaskBegin( SCOREP_RegionHandle regionHandle,
                          uint64_t            taskId,
                          uint64_t            timestamp );

/**
   Implemented the handling of a task resumption event. This event implies that the
   former task was suspended if no explicit task complete event preceded this event.
   @param regionHandle The Score-P region handle of the associated source code
                       region. E.g., the task construct's inner region.
   @param taskId       The task identifier of the resumed task. The task id of the
                       suspended task is the current task in the profile.
   @param timestamp    The timestamp of this event.
 */
void
SCOREP_Profile_TaskResume( SCOREP_RegionHandle regionHandle,
                           uint64_t            taskId,
                           uint64_t            timestamp );

/**
   Implemented the handling if the execution of the current task is complete.
   @param regionHandle The Score-P region handle of the associated source code
                       region. E.g., the task construct's inner region.
   @param timestamp    The timestamp of this event.
 */
void
SCOREP_Profile_TaskCompleted( SCOREP_RegionHandle regionHandle,
                              uint64_t            timestamp );


#endif /* SCOREP_PROFILING_TASKING */
