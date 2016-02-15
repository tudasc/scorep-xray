/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2014,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

#ifndef SCOREP_TASK_INTERNAL_H
#define SCOREP_TASK_INTERNAL_H

/**
 * @file
 * This file defines task object management functions.
 * Over the life time of a task we expect the following events from the
 * tasking system:
 *
 * task creation
 *     The adapter should generate a unique task id number consisting,
 *     of the thread id and a generation number, that identifies the
 *     created task.
 *     An task creation event is written to the trace containing the
 *     new task id, and the task id of the parent task.
 *     The task id number 0 represents the implicit task of any new location.
 *     The region stack and the task object is not yet created, thus,
 *     you should not call scorep_task_create() yet.
 *
 * task begin
 *     This event happens when the task starts its execution. When
 *     The task starts execution, we create a new task object by calling
 *     scorep_task_create(). It returns a task handle, that is required for
 *     all further tasking events. You need to provide the location id
 *     and the generation number that were written to the task creation event
 *     in the trace.
 *
 * task switch
 *     On every task switch that happens to a tasks that was already executed
 *     before and then suspended, you need to call scorep_task_switch().
 *     You need to provide the task handle that was returned from scorep_task_create().
 *
 * task completion
 *     After a task has completed execution, call scorep_task_complete().
 *     It cleans up task data structures. It is unsafe to access the
 *     task data handle after calling scorep_task_complete().
 *     You need to provide the task handle that was returned from scorep_task_create().
 *
 * enter
 *     On every region enter the enter event calls scorep_task_enter().
 *
 * exit
 *     On every region exit the exit event calls scorep_task_exit().
 */

#include <SCOREP_Task.h>

/**
 * Creates a new task object. Should happen when the task starts execution and
 * not already on task creation, to avoid that location specific memory transfers
 * to another location.
 * @param location         The location that starts execution of the task.
 * @param threadId         The thread id of the thread that created the task. Together
 *                         with the generation number it identifies the task object.
 * @param generationNumber Used to identify the task.
 * @return the task handle of the new task data.
 */
SCOREP_TaskHandle
scorep_task_create( struct SCOREP_Location* location,
                    uint32_t                threadId,
                    uint32_t                generationNumber );

/**
 * Cleans up the task data structure if a task has completed execution.
 * @param location The location that executes the task complete event.
 * @param task     The completed task.
 */
void
scorep_task_complete( struct SCOREP_Location* location,
                      SCOREP_TaskHandle       task );

/**
 * Updates internal data structures when a task switch happens.
 * @param location The location that executes the task switch event.
 * @param newTask  The task that is executed from now on.
 */
void
scorep_task_switch( struct SCOREP_Location* location,
                    SCOREP_TaskHandle       newTask );

#endif /* SCOREP_TASK_INTERNAL_H */
