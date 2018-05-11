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

#ifndef SCOREP_THREAD_CREATE_WAIT_MODEL_SPECIFIC_H
#define SCOREP_THREAD_CREATE_WAIT_MODEL_SPECIFIC_H

/**
 * @file
 */


#include <SCOREP_Types.h>

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

struct SCOREP_Location;
struct scorep_thread_private_data;

/**
 *
 *
 * @param modelData
 * @param location
 */
void
scorep_thread_create_wait_on_create( void*                   modelData,
                                     struct SCOREP_Location* location );


/**
 *
 *
 * @param modelData
 * @param location
 */
void
scorep_thread_create_wait_on_wait( void*                   modelData,
                                   struct SCOREP_Location* location );


/**
 *
 *
 * @param      parentTpd
 * @param      sequenceCount
 * @param      locationReuseKey See locationReuseKey in SCOREP_ThreadCreateWait_Begin.
 * @param[out] currentTpd
 * @param[out] locationIsCreated
 */
void
scorep_thread_create_wait_on_begin( struct scorep_thread_private_data*  parentTpd,
                                    uint32_t                            sequenceCount,
                                    uintptr_t                           locationReuseKey,
                                    struct scorep_thread_private_data** currentTpd,
                                    bool*                               locationIsCreated );


/**
 *
 * @param parentTpd
 * @param currentTpd
 * @param sequenceCount
 */
void
scorep_thread_create_wait_on_end( struct scorep_thread_private_data* parentTpd,
                                  struct scorep_thread_private_data* currentTpd,
                                  uint32_t                           sequenceCount );


/**
 *
 * @param location The newly created location object.
 */
void
scorep_thread_create_wait_orphan_begin( struct SCOREP_Location** location );


/**
 * @param terminate A void* obtained in SCOREP_ThreadCreateWait_TryTerminate().
 */
void
scorep_thread_create_wait_orphan_end( void* terminate );


/**
 *
 * @param[out] currentTpd
 * @param[out] locationIsCreated
 */
void
scorep_thread_create_wait_on_orphan_begin( struct scorep_thread_private_data** currentTpd,
                                           bool*                               locationIsCreated );


/**
 *
 * @param currentTpd
 */
void
scorep_thread_create_wait_on_orphan_end( struct scorep_thread_private_data* currentTpd );


/**
 * Provide a location/tpd reuse-key based on @a paradigm, @startRoutine and the
 * create_wait_reuse_policy (derived form env var).
 *
 * @return 0, if create_wait_reuse_policy == SCOREP_CREATE_WAIT_REUSE_POLICY_NEVER.
 * 1, if paradigm == SCOREP_PARADIGM_PTHREAD and create_wait_reuse_policy == SCOREP_CREATE_WAIT_REUSE_POLICY_ALWAYS,
 * 2, if paradigm == SCOREP_PARADIGM_ORPHAN_THREAD and create_wait_reuse_policy != SCOREP_CREATE_WAIT_REUSE_POLICY_NEVER,
 * startRoutine, if paradigm == SCOREP_PARADIGM_PTHREAD and create_wait_reuse_policy == SCOREP_CREATE_WAIT_REUSE_POLICY_SAME_START_ROUTINE.
 * @param paradigm One of the THREAD_CREATE_WAIT paradigms
 * @param startRoutine Integer-representation of the thread's start routine
 */
uintptr_t
scorep_thread_create_wait_get_reuse_key( SCOREP_ParadigmType paradigm,
                                         uintptr_t           startRoutine );


#endif /* SCOREP_THREAD_CREATE_WAIT_MODEL_SPECIFIC_H */
