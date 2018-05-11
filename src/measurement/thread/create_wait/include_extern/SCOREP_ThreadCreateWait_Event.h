/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2014, 2016, 2018,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2014,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

#ifndef SCOREP_THREADCREATEWAIT_EVENT_H
#define SCOREP_THREADCREATEWAIT_EVENT_H

/**
 * @file
 */


#include <SCOREP_Types.h>

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>


struct scorep_thread_private_data;
struct SCOREP_Location;


/**
 *
 *
 * @param paradigm
 * @param parent
 * @param sequenceCount
 */
void
SCOREP_ThreadCreateWait_Create( SCOREP_ParadigmType                 paradigm,
                                struct scorep_thread_private_data** parent,
                                uint32_t*                           sequenceCount );


/**
 *
 *
 * @param paradigm
 * @param sequenceCount
 */
void
SCOREP_ThreadCreateWait_Wait( SCOREP_ParadigmType paradigm,
                              uint32_t            sequenceCount );


/**
 *
 *
 * @param paradigm
 * @param parent
 * @param sequenceCount
 * @param startRoutine Address of the thread's 'start_routine' or 0 if not
 *        available. Potentially used location-reuse.
 * @param location The newly created location object.
 */
void
SCOREP_ThreadCreateWait_Begin( SCOREP_ParadigmType                paradigm,
                               struct scorep_thread_private_data* parent,
                               uint32_t                           sequenceCount,
                               uintptr_t                          startRoutine,
                               struct SCOREP_Location**           location );


/**
 *
 *
 * @param paradigm
 * @param parentTpd
 * @param sequenceCount
 * @param terminate A void* obtained in SCOREP_ThreadCreateWait_TryTerminate().
 */
void
SCOREP_ThreadCreateWait_End( SCOREP_ParadigmType                paradigm,
                             struct scorep_thread_private_data* parentTpd,
                             uint32_t                           sequenceCount,
                             void*                              terminate );


/**
 * CreateWait thread termination is triggered by
 * 1. registered cleanup handler for instrumented threads,
 * 2. a destructor of a thread local storage key for orphan threads, or
 * 3. a cleanup routine triggered at subsystem_end.
 * 1. and 3. as well as 2. and 3. potentially run concurrently, but only
 * one entity is allowed to terminate the thread. To ensure this, every
 * entity needs to call SCOREP_ThreadCreateWait_TryTerminate() and perform
 * the shutdown only on a return value != NULL. The entities will receive
 * a void* return value form SCOREP_ThreadCreateWait_TryTerminate() that is subsequently
 * passed to SCOREP_ThreadCreateWait_End(), scorep_thread_create_wait_orphan_end(),
 * or consumed by end_latecomer().
 *
 * @see end_latecomer
 * @see SCOREP_ThreadCreateWait_End
 * @see scorep_thread_create_wait_orphan_end
 * @see cleanup_handler
 * @see orphan_dtor
 */
void*
SCOREP_ThreadCreateWait_TryTerminate( struct SCOREP_Location* location );


#endif /* SCOREP_THREADCREATEWAIT_EVENT_H */
