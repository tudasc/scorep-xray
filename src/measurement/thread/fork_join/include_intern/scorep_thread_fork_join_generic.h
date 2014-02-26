#ifndef SCOREP_THREAD_FORK_JOIN_GENERIC_H_
#define SCOREP_THREAD_FORK_JOIN_GENERIC_H_

/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2013,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */


/**
 * @file
 */


#include <stdbool.h>
#include <stdint.h>


/**
 * Returns a process-unique sequence count starting a 0 for the
 * initial thread and being incremented thereafter. Values >= 1
 * identify a parallel execution started by either
 * SCOREP_Thread_Fork() or SCOREP_Thread_Create().
 */
uint32_t
scorep_thread_get_next_sequence_count( void );


/**
 * Create a SCOREP_Thread_PrivateData object and corresponding model
 * data accordingly.
 *
 * @note No SCOREP_Location is created as the @a parent location might
 * get reused. If there is a need for a new SCOREP_Location object,
 * create it in the model-specific functions, usually in
 * SCOREP_Thread_OnBegin().
 */
struct scorep_thread_private_data*
scorep_thread_create_private_data( struct scorep_thread_private_data* parent );


/**
 * Returns the model-specific data of the SCOREP_Thread_PrivateData
 * object @a tpd.
 */
void*
scorep_thread_get_model_data( struct scorep_thread_private_data* tpd );


/**
 * Returns true if @a tpd represents the initial thread, false
 * otherwise.
 */
bool
scorep_thread_is_initial_thread( struct scorep_thread_private_data* tpd );


/**
 * Get the parent SCOREP_Thread_PrivateData object of @a tpd. Returns
 * 0 for the initial thread.
 */
struct scorep_thread_private_data*
scorep_thread_get_parent( struct scorep_thread_private_data* tpd );


/**
 * Returns the SCOREP_Location object currently associated with @a tpd.
 */
struct SCOREP_Location*
scorep_thread_get_location( struct scorep_thread_private_data* tpd );


/**
 * Associates a SCOREP_Location object @a location with a
 * SCOREP_Thread_PrivateData object @a tpd.
 */
void
scorep_thread_set_location( struct scorep_thread_private_data* tpd,
                            struct SCOREP_Location*            location );


#endif /* SCOREP_THREAD_FORK_JOIN_GENERIC_H_ */
