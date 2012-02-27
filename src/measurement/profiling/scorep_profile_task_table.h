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

#ifndef SCOREP_PROFILE_TASK_TABLE_H
#define SCOREP_PROFILE_TASK_TABLE_H

/**
 * @status      alpha
 * @file        scorep_profile_task_table.h
 * @maintainer  Daniel Lorenz <d.lorenz@fz-juelich.de>
 *
 * @brief   Implementation of the Tasking functions of the profile interface
 *
 */

/**
   We need to map task ids to their task data. Thus, we use a hash table. Because, the
   table access is a majot source of runtime overhead, we implement out own table. The
   predefined general data structure uses mallocs, but we want to use the profile page
   manager in order to avoid locking. Furthermore, we can reduce the allocation to one
   call, instead of 3 calls per item in teh general table.
 */
typedef struct scorep_profile_task_table scorep_profile_task_table;

/**
   The data type to indentify task instances.
 */
typedef uint64_t scorep_profile_task_t;

/**
   Returns whether the measured program used tasks. This function does return a
   valid result only after the parallel regions are done.
 */
int32_t
scorep_profile_has_tasks();


/**
   Initializes the tasking dats structires for a new location
   @param location The location structure of the location which is initialized.
 */
void
scorep_profile_task_initialize( SCOREP_Profile_LocationData* location );

/**
   Finalizes the tasking data structures for a location.
   @param location The location data structure which is finalized.
 */
void
scorep_profile_task_finalize( SCOREP_Profile_LocationData* location );

/**
   Creates a new task entry at location @a location.
   @param location   The location where the new task appeared.
   @param task_id    The task instance of the new task.
   @param task_root  Pointer to the profile node, where the task starts.
 */
void
scorep_profile_create_task( SCOREP_Profile_LocationData* location,
                            scorep_profile_task_t        task_id,
                            scorep_profile_node*         task_root );

#if 0
/**
   Returns the current position in the profile tree for a given task.
   @param location The current location
   @param task_id  The id of the task instance
 */
scorep_profile_node*
scorep_profile_get_task_node( SCOREP_Profile_LocationData* location,
                              scorep_profile_task_t        task_id );

/**
   Sets the current position in the profile tree for a given task.
   @param location The current location
   @param task_id  The id of the task instance
   @param node     The current position of the task in the tree
 */

void
scorep_profile_set_task_node( SCOREP_Profile_LocationData* location,
                              scorep_profile_task_t        task_id,
                              scorep_profile_node*         node );

#endif
/**
   Removes a task from the internal tasking data structures.
   @param tasks    The task table of the location.
   @param task_id  The task instance that is to be removed.
 */
void
scorep_profile_remove_task( scorep_profile_task_table* tasks,
                            scorep_profile_task_t      task_id );

/**
   Stores the depth of the current task. Needed before a task switch to maintain
   the correct depth level for each task.
 */
void
scorep_profile_store_task( SCOREP_Profile_LocationData* location );

/**
   Restores the depth of the current task. Needed after a task switch to reset the
   depth level of the new task.
 */
void
scorep_profile_restore_task( SCOREP_Profile_LocationData* location );

/**
   Checks whether @a task_id is the implicit task.
   @param location The current location
   @param task_id  The task instance that is checked.
 */
bool
scorep_profile_is_implicit_task( SCOREP_Profile_LocationData* location,
                                 scorep_profile_task_t        task_id );

/**
   Updates tasks statistics at an exit of the parallel region.
   @param thread Location data of the current location.
 */
void
scorep_profile_task_parallel_exit( SCOREP_Thread_LocationData* thread );

/**
   @def SCOREP_PROFILE_IMPLICIT_TASK_ID
   In the current POMP2 adapter, implicit tasks have always the task ID 0.
 */
#define SCOREP_PROFILE_IMPLICIT_TASK_ID ( ( scorep_profile_task_t )0 )


#endif // SCOREP_PROFILE_TASK_TABLE_H
