/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2013,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2013,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2014,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2013,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

#ifndef SCOREP_PROFILE_TASK_SWITCH_H
#define SCOREP_PROFILE_TASK_SWITCH_H

/**
 * @file
 * @brief   Declaration of functions related to task handling that are used by other
 *          modules inside he profiling.
 *
 */

#include <scorep_profile_node.h>
#include <SCOREP_Profile_Tasking.h>

/**
 * Data type that defines the data associated with a task instance.
 */
typedef struct scorep_profile_task
{
    scorep_profile_node*         current_node;  /**< Current position in the call tree */
    scorep_profile_node*         root_node;     /**< Root node of the task call tree */
    uint32_t                     depth;         /**< Current callpath depth */
    bool                         can_migrate;   /**< True, if the task can migrate */
    SCOREP_Profile_LocationData* creator;       /**< The location which allocated this */
    SCOREP_Profile_LocationData* last_location; /**< The location which suspended this */
    struct scorep_profile_task*  next;          /**< Used to link released objects */
} scorep_profile_task;

/**
 * Updates the profile nodes from @a node to the task root node if
 * the execution of a task is resumed after a suspension.
 * @param node          The current node of the resuming task.
 * @param timestamp     Timestamp of this event.
 * @param metric_values Array of the dense metric values.
 */
void
scorep_profile_update_on_resume( scorep_profile_node* node,
                                 uint64_t             timestamp,
                                 uint64_t*            metric_values );

/**
 * Implementation of the task switch.
 * @param location      Location on which this event happens.
 * @param task          Handle of the task the runtime switched to.
 * @param timestamp     Timestamp of this event.
 * @param metricValues  Array of the dense metric values.
 */
void
scorep_profile_task_switch( SCOREP_Profile_LocationData* location,
                            scorep_profile_task*         task,
                            uint64_t                     timestamp,
                            uint64_t*                    metricValues );

/**
 * Implementation of the task switch that is executed during the begin of a task.
 * @param location      Location on which this event happens.
 * @param task          Handle of the task the runtime switched to.
 * @param timestamp     Timestamp of this event.
 * @param metricValues Array of the dense metric values.
 */
void
scorep_profile_task_switch_start( SCOREP_Profile_LocationData* location,
                                  scorep_profile_task*         task,
                                  uint64_t                     timestamp,
                                  uint64_t*                    metricValues );

#endif /* SCOREP_PROFILE_TASK_SWITCH_H */
