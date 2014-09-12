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

#ifndef SCOREP_TASK_H
#define SCOREP_TASK_H

/**
 * @file
 * This files defines functions of the per-task region stack that may be
 * called by non-measurement core components.
 */

#include <SCOREP_Types.h>

struct SCOREP_Location;

/**
 * Returns the thread id of the @a task object, i.e., the id that was
 * passed to SCOREP_Task_Create().
 * Together with the generation number it identifies the task instance.
 * @param task  The task handle.
 */
uint32_t
SCOREP_Task_GetThreadId( SCOREP_TaskHandle task );

/**
 * Returns the generation number of the @a task object, i.e., the
 * number that was passed to SCOREP_Task_Create().
 * Together with the thread id it identifies the task instance.
 * @param task  The task handle.
 */
uint32_t
SCOREP_Task_GetGenerationNumber( SCOREP_TaskHandle task );

/**
 * Returns the currently executed task for @a location.
 * @param location The location.
 */
SCOREP_TaskHandle
SCOREP_Task_GetCurrentTask( struct SCOREP_Location* location );

/**
 * Returns the region handle on top of the stack for @a task.
 * @param task The task.
 */
SCOREP_RegionHandle
SCOREP_Task_GetTopRegion( SCOREP_TaskHandle task );

/**
 * Removes all regions from the stack of @a task.
 * @param location  The location which is used for memory operations.
 * @param task      The task.
 */
void
SCOREP_Task_ClearStack( struct SCOREP_Location* location,
                        SCOREP_TaskHandle       task );

/**
 * Removes all regions from the stack of the current task.
 */
void
SCOREP_Task_ClearCurrent( void );

#endif
