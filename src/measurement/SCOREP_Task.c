/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2014-2015,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 * @file
 */

#include <config.h>
#include "scorep_task_internal.h"
#include <SCOREP_Memory.h>
#include <SCOREP_Subsystem.h>
#include <SCOREP_Events.h>
#include <SCOREP_Location.h>
#include <SCOREP_Profile_Tasking.h>
#include <UTILS_Error.h>
#include <scorep_status.h>

#define SCOREP_TASK_STACK_SIZE 30

/* ********************************************* types and variables */
typedef struct scorep_task_stack_frame
{
    SCOREP_RegionHandle             regions[ SCOREP_TASK_STACK_SIZE ];
    struct scorep_task_stack_frame* prev;
} scorep_task_stack_frame;

typedef struct SCOREP_Task
{
    scorep_task_stack_frame* current_frame;
    uint32_t                 current_index;
    uint32_t                 thread_id;
    uint32_t                 generation_number;
    SCOREP_TaskHandle        next;
    /* Keep the array of substrates the last entry. By allocating some
       space than the size of this struct, it may hold an array of void*
       pointers to the substrates' data. */
    void* substrate_data[];
} SCOREP_Task;

typedef struct scorep_location_task_data
{
    SCOREP_TaskHandle        current_task;
    SCOREP_TaskHandle        recycled_tasks;
    SCOREP_TaskHandle        implicit_task;
    scorep_task_stack_frame* recycled_frames;
} scorep_location_task_data;

static size_t task_subsystem_id;

/* ********************************************* static functions */
static inline void
recycle_stack_frame( SCOREP_Location*         location,
                     scorep_task_stack_frame* frame )
{
    scorep_location_task_data* subsystem_data
                                    = SCOREP_Location_GetSubsystemData( location, task_subsystem_id );
    frame->prev                     = subsystem_data->recycled_frames;
    subsystem_data->recycled_frames = frame;
}

static inline void
recycle_task( SCOREP_Location*  location,
              SCOREP_TaskHandle task )
{
    scorep_location_task_data* subsystem_data
                                   = SCOREP_Location_GetSubsystemData( location, task_subsystem_id );
    task->next                     = subsystem_data->recycled_tasks;
    subsystem_data->recycled_tasks = task;
}

static scorep_task_stack_frame*
alloc_new_stack_frame( SCOREP_Location* location )
{
    scorep_task_stack_frame*   new_frame;
    scorep_location_task_data* subsystem_data
        = SCOREP_Location_GetSubsystemData( location, task_subsystem_id );
    if ( subsystem_data->recycled_frames != NULL )
    {
        new_frame                       = subsystem_data->recycled_frames;
        subsystem_data->recycled_frames = subsystem_data->recycled_frames->prev;
    }
    else
    {
        new_frame = ( scorep_task_stack_frame* )
                    SCOREP_Location_AllocForMisc( location, sizeof( scorep_task_stack_frame ) );
    }

    return new_frame;
}

static SCOREP_ErrorCode
task_subsystem_register( size_t id )
{
    task_subsystem_id = id;
    return SCOREP_SUCCESS;
}

static SCOREP_ErrorCode
task_subsystem_init_location( SCOREP_Location* location )
{
    scorep_location_task_data* subsystem_data
        = SCOREP_Location_AllocForMisc( location,
                                        sizeof( scorep_location_task_data ) );

    /* Order important as scorep_task_create retrieves and accesses subsystem object. */
    subsystem_data->recycled_tasks  = NULL;
    subsystem_data->recycled_frames = NULL;
    SCOREP_Location_SetSubsystemData( location,
                                      task_subsystem_id,
                                      subsystem_data );
    subsystem_data->current_task = scorep_task_create( location,
                                                       SCOREP_Location_GetId( location ),
                                                       0 );
    subsystem_data->implicit_task = subsystem_data->current_task;

    return SCOREP_SUCCESS;
}

static void
task_subsystem_finalize_location( SCOREP_Location* location )
{
    scorep_location_task_data* subsystem_data
        = SCOREP_Location_GetSubsystemData( location, task_subsystem_id );

    scorep_task_complete( location, subsystem_data->current_task );
}

static inline void
task_pop_stack( SCOREP_Location*  location,
                SCOREP_TaskHandle task )
{
    if ( task->current_index == 0 )
    {
        scorep_task_stack_frame* old_frame = task->current_frame;
        task->current_frame = task->current_frame->prev;
        task->current_index = SCOREP_TASK_STACK_SIZE - 1;
        recycle_stack_frame( location, old_frame );
    }
    else
    {
        task->current_index--;
    }
}
/* ************************************** subsystem struct */

const SCOREP_Subsystem SCOREP_Subsystem_TaskStack =
{
    .subsystem_name              = "TASK",
    .subsystem_register          = &task_subsystem_register,
    .subsystem_init_location     = &task_subsystem_init_location,
    .subsystem_finalize_location = &task_subsystem_finalize_location,
};

/* ************************************** internal interface functions */

SCOREP_TaskHandle
scorep_task_create( SCOREP_Location* location,
                    uint32_t         threadId,
                    uint32_t         generationNumber )
{
    SCOREP_TaskHandle          new_task;
    scorep_location_task_data* subsystem_data
        = SCOREP_Location_GetSubsystemData( location, task_subsystem_id );
    if ( subsystem_data->recycled_tasks != NULL )
    {
        new_task                       = subsystem_data->recycled_tasks;
        subsystem_data->recycled_tasks = subsystem_data->recycled_tasks->next;
    }
    else
    {
        /* TODO: Currently, we use substrate specific data only for profiling. However,
           a generic solution would be to have an array of pointers to
           substrate specific data. Then we must alloc
           num_of_substrates * sizeof (void*)
           bytes in addition to hold the substrates' data */
        new_task = ( SCOREP_TaskHandle )
                   SCOREP_Location_AllocForMisc( location, sizeof( SCOREP_Task ) + sizeof( void* ) );
    }

    new_task->current_frame     = NULL;
    new_task->current_index     = SCOREP_TASK_STACK_SIZE - 1;
    new_task->thread_id         = threadId;
    new_task->generation_number = generationNumber;

    /* TODO: Currently only the profiling sustrate uses task specific data.
       A generic approach must iterate over all available substrates */
    if ( SCOREP_IsProfilingEnabled() )
    {
        SCOREP_Profile_CreateTaskData( location, new_task );
    }
    else
    {
        new_task->substrate_data[ 0 ] = NULL;
    }

    return new_task;
}

void
scorep_task_complete( SCOREP_Location*  location,
                      SCOREP_TaskHandle task )
{
    /* TODO: Currently only the profiling sustrate uses task specific data.
       A generic approach must iterate over all available substrates */
    if ( SCOREP_IsProfilingEnabled() )
    {
        SCOREP_Profile_FreeTaskData( location, task );
        task->substrate_data[ 0 ] = NULL;
    }
    recycle_task( location, task );
}

void
scorep_task_switch( SCOREP_Location*  location,
                    SCOREP_TaskHandle newTask )
{
    scorep_location_task_data* subsystem_data
                                 = SCOREP_Location_GetSubsystemData( location, task_subsystem_id );
    subsystem_data->current_task = newTask;
}

/* ************************************** external interface functions */

void
SCOREP_Task_ExitAllRegions( SCOREP_Location*  location,
                            SCOREP_TaskHandle task )
{
    while ( task->current_frame != NULL )
    {
        /* The exit removes the top region from the stack. */
        SCOREP_RegionHandle region = SCOREP_Task_GetTopRegion( task );
        if ( region != SCOREP_FILTERED_REGION )
        {
            SCOREP_ExitRegion( region );
        }
        else
        {
            task_pop_stack( location, task );
        }
    }
}

SCOREP_RegionHandle
SCOREP_Task_GetTopRegion( SCOREP_TaskHandle task )
{
    return task->current_frame == NULL ? SCOREP_INVALID_REGION :
           task->current_frame->regions[ task->current_index ];
}

uint32_t
SCOREP_Task_GetThreadId( SCOREP_TaskHandle task )
{
    return task->thread_id;
}

uint32_t
SCOREP_Task_GetGenerationNumber( SCOREP_TaskHandle task )
{
    return task->generation_number;
}

SCOREP_TaskHandle
SCOREP_Task_GetCurrentTask( SCOREP_Location* location )
{
    scorep_location_task_data* subsystem_data
        = SCOREP_Location_GetSubsystemData( location, task_subsystem_id );
    return subsystem_data->current_task;
}

void
SCOREP_Task_ClearStack( SCOREP_Location*  location,
                        SCOREP_TaskHandle task )
{
    while ( task->current_frame != NULL )
    {
        scorep_task_stack_frame* old_frame = task->current_frame;
        task->current_frame = task->current_frame->prev;
        recycle_stack_frame( location, old_frame );
    }
    task->current_index = SCOREP_TASK_STACK_SIZE - 1;
}

void
SCOREP_Task_ClearCurrent( void )
{
    SCOREP_Location* location = SCOREP_Location_GetCurrentCPULocation();
    SCOREP_Task_ClearStack( location, SCOREP_Task_GetCurrentTask( location ) );
}

void
SCOREP_Task_Enter( SCOREP_Location*    location,
                   SCOREP_RegionHandle region )
{
    scorep_location_task_data* subsystem_data
        = SCOREP_Location_GetSubsystemData( location, task_subsystem_id );
    UTILS_ASSERT( subsystem_data != NULL );
    SCOREP_TaskHandle task = subsystem_data->current_task;

    if ( task->current_index < SCOREP_TASK_STACK_SIZE - 1 )
    {
        task->current_index++;
        task->current_frame->regions[ task->current_index ] = region;
    }
    else
    {
        scorep_task_stack_frame* new_frame = alloc_new_stack_frame( location );
        new_frame->prev                   = task->current_frame;
        task->current_frame               = new_frame;
        task->current_frame->regions[ 0 ] = region;
        task->current_index               = 0;
    }
}

void
SCOREP_Task_Exit( SCOREP_Location* location )
{
    scorep_location_task_data* subsystem_data
        = SCOREP_Location_GetSubsystemData( location, task_subsystem_id );
    SCOREP_TaskHandle task = subsystem_data->current_task;

    task_pop_stack( location, task );
}

void*
SCOREP_Task_GetSubstrateData( SCOREP_TaskHandle task,
                              uint32_t          substrateId )
{
    return task->substrate_data[ substrateId ];
}

void
SCOREP_Task_SetSubstrateData( SCOREP_TaskHandle task,
                              uint32_t          substrateId,
                              void*             data )
{
    task->substrate_data[ substrateId ] = data;
}
