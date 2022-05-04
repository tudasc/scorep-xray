/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2022,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 * @file
 */

#ifndef SCOREP_OMPT_H
#define SCOREP_OMPT_H

#include <stdbool.h>
#include <omp-tools.h>

#define SCOREP_DEBUG_MODULE_NAME OMPT
#include <UTILS_Debug.h>

#include <UTILS_Error.h>


static inline size_t
scorep_ompt_get_subsystem_id( void )
{
    extern size_t scorep_ompt_subsystem_id;
    return scorep_ompt_subsystem_id;
}

/* We want to record events between ompt_subsystem_begin and ompt_substytem_end
   only, but cannot register and deregister callbacks there. Thus, pass events
   to the measurement core only if scorep_ompt_record_event() return true. */
static inline bool
scorep_ompt_record_event( void )
{
    extern bool scorep_ompt_record_events;
    return scorep_ompt_record_events;
}

/* Ignore events before ompt_subsystem_begin. Events during POST are ok when
   triggered within ompt_subsystem_end. */
#define SCOREP_OMPT_RETURN_ON_INVALID_EVENT() \
    do \
    { \
        if ( !scorep_ompt_record_event() ) \
        { \
            UTILS_DEBUG_EXIT( "Event ignored (either PRE, or POST after ompt_finalize_tool)." ); \
            SCOREP_IN_MEASUREMENT_DECREMENT(); \
            return; \
        } \
    } while ( 0 )


static inline bool
scorep_ompt_finalizing_tool_in_progress( void )
{
    extern bool scorep_ompt_finalizing_tool;
    return scorep_ompt_finalizing_tool;
}


static inline int
scorep_ompt_get_task_info( int            ancestor_level,
                           int*           flags,
                           ompt_data_t**  task_data,
                           ompt_frame_t** task_frame,
                           ompt_data_t**  parallel_data,
                           int*           thread_num )
{
    extern ompt_get_task_info_t scorep_ompt_mgmt_get_task_info;
    return scorep_ompt_mgmt_get_task_info( ancestor_level,
                                           flags,
                                           task_data,
                                           task_frame,
                                           parallel_data,
                                           thread_num );
}


struct SCOREP_Location;

SCOREP_ErrorCode
scorep_ompt_subsystem_trigger_overdue_events( struct SCOREP_Location* location );

void
scorep_ompt_codeptr_hash_dlclose_cb( void*       soHandle,
                                     const char* soFileName,
                                     uintptr_t   soBaseAddr,
                                     uint16_t    soToken );

#endif /* SCOREP_OMPT_H */
