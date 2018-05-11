/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2014,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2015, 2018,
 * Forschungszentrum Juelich GmbH, Germany
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
#include <stddef.h>

struct SCOREP_Location;

/**
 * Returns the thread id of the @a task object, i.e., the id that was
 * passed to SCOREP_Task_Create().
 * Together with the generation number it identifies the task instance.
 * @param task  The task handle.
 * @returns the thread id of the @a task object.
 */
uint32_t
SCOREP_Task_GetThreadId( SCOREP_TaskHandle task );

/**
 * Returns the generation number of the @a task object, i.e., the
 * number that was passed to SCOREP_Task_Create().
 * Together with the thread id it identifies the task instance.
 * @param task  The task handle.
 * @returns the generation number of the @a task object.
 */
uint32_t
SCOREP_Task_GetGenerationNumber( SCOREP_TaskHandle task );

/**
 * Returns the currently executed task for @a location.
 * @param location The location.
 * @returns the currently executed task for @a location.
 */
SCOREP_TaskHandle
SCOREP_Task_GetCurrentTask( struct SCOREP_Location* location );

/**
 * Returns the region handle on top of the stack for @a task.
 * @param task The task.
 * @returns the region handle on top of the stack for @a task.
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

/**
 * Adds @a region to the region stack of the current task executed by @a location.
 * @param location The location which executes the enter event.
 * @param region   The region handle of the entered region.
 */
void
SCOREP_Task_Enter( struct SCOREP_Location* location,
                   SCOREP_RegionHandle     region );

/**
 * Moves the stack pointer of the current task one element down.
 * @param location The location that executes the exit.
 */
void
SCOREP_Task_Exit( struct SCOREP_Location* location );

/**
 * Creates exit events for all regions on the stack for @a task using
 * SCOREP_ExitRegion(), assuming current location equals passed
 * @a location.
 * @param location  The location that processes the exits.
 * @param task      The task.
 */
void
SCOREP_Task_ExitAllRegions( struct SCOREP_Location* location,
                            SCOREP_TaskHandle       task );

/**
 * Creates exit events for all regions on the stack for @a task using
 * SCOREP_Location_ExitRegion(). Current location and passed @a location
 *  may differ in phase POST only.
 * @param location  The location for which the exits are triggered.
 * @param task      The task.
 * @param timestamp The timestamp for all exits.
 */
void
SCOREP_Location_Task_ExitAllRegions( struct SCOREP_Location* location,
                                     SCOREP_TaskHandle       task,
                                     uint64_t                timestamp );

/**
 * Returns the substrate specific data for @a task.
 * @param task        The task handle from which we get the substrate data.
 * @param substrateId The Id of the substrate.
 *
 * @returns the substrate specific data for @a task.
 *          If the substrate not enabled, it returns NULL.
 */
void*
SCOREP_Task_GetSubstrateData( SCOREP_TaskHandle task,
                              size_t            substrateId );

/**
 * Sets the profiling specific data for @a task.
 * @param task        The task for which we set the data.
 * @param substrateId The Id of the substrate.
 * @param data        The profiling data.
 */
void
SCOREP_Task_SetSubstrateData( SCOREP_TaskHandle task,
                              size_t            substrateId,
                              void*             data );

#endif /* SCOREP_TASK_H */
