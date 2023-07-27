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

#ifndef SCOREP_OMPT_CALLBACKS_HOST_H
#define SCOREP_OMPT_CALLBACKS_HOST_H

#include <UTILS_Mutex.h>
#include <SCOREP_DefinitionHandles.h>

extern SCOREP_ParameterHandle scorep_ompt_parameter_loop_type;

typedef struct scorep_ompt_cpu_location_data
{
    struct task_t* task;
    UTILS_Mutex    protect_task_exchange; /* task and potentially task members
                                             need to be modified in an atomic way;
                                             cannot be done with UTILS_Atomic_ExchangeN. */

    /* Have the mutexes on different cache lines */
    char        pad[ SCOREP_CACHELINESIZE -
                     ( sizeof( struct task_t* ) + sizeof( UTILS_Mutex ) ) ];
    UTILS_Mutex preserve_order; /* preserve event order by preventing
                                    trigger_overdue to write itask_begin of new
                                    parallel region into location before
                                    ibarrier_end and itask_end of previous
                                    region are processed. */

    bool is_ompt_location;      /* ignore non-ompt location in overdue processing */
} scorep_ompt_cpu_location_data;


/* sort cb declarations alphabetically */
/* keep_ompt_names_and_naming_scheme */

void
scorep_ompt_cb_host_implicit_task( ompt_scope_endpoint_t endpoint,
                                   ompt_data_t*          parallel_data,
                                   ompt_data_t*          task_data,
                                   unsigned int          actual_parallelism,
                                   unsigned int          index,
                                   int                   flags );

void
scorep_ompt_cb_host_parallel_begin( ompt_data_t*        encountering_task_data,
                                    const ompt_frame_t* encountering_task_frame,
                                    ompt_data_t*        parallel_data,
                                    unsigned int        requested_parallelism,
                                    int                 flags,
                                    const void*         codeptr_ra );

void
scorep_ompt_cb_host_parallel_end( ompt_data_t* parallel_data,
                                  ompt_data_t* encountering_task_data,
                                  int          flags,
                                  const void*  codeptr_ra );

void
scorep_ompt_cb_host_sync_region( ompt_sync_region_t    kind,
                                 ompt_scope_endpoint_t endpoint,
                                 ompt_data_t*          parallel_data,
                                 ompt_data_t*          task_data,
                                 const void*           codeptr_ra );

void
scorep_ompt_cb_host_task_create( ompt_data_t*        encountering_task_data,
                                 const ompt_frame_t* encountering_task_frame,
                                 ompt_data_t*        new_task_data,
                                 int                 flags,
                                 int                 has_dependences,
                                 const void*         codeptr_ra );

void
scorep_ompt_cb_host_task_schedule( ompt_data_t*       prior_task_data,
                                   ompt_task_status_t prior_task_status,
                                   ompt_data_t*       next_task_data );

void
scorep_ompt_cb_host_thread_begin( ompt_thread_t thread_type,
                                  ompt_data_t*  thread_data );

void
scorep_ompt_cb_host_thread_end( ompt_data_t* thread_data );

void
scorep_ompt_cb_host_work( ompt_work_t           work_type,
                          ompt_scope_endpoint_t endpoint,
                          ompt_data_t*          parallel_data,
                          ompt_data_t*          task_data,
                          uint64_t              count,
                          const void*           codeptr_ra );

void
scorep_ompt_cb_host_masked( ompt_scope_endpoint_t endpoint,
                            ompt_data_t*          parallel_data,
                            ompt_data_t*          task_data,
                            const void*           codeptr_ra );

void
scorep_ompt_cb_host_mutex_acquire( ompt_mutex_t   kind,
                                   unsigned int   hint,
                                   unsigned int   impl,
                                   ompt_wait_id_t wait_id,
                                   const void*    codeptr_ra );

void
scorep_ompt_cb_host_mutex_acquired( ompt_mutex_t   kind,
                                    ompt_wait_id_t wait_id,
                                    const void*    codeptr_ra );

void
scorep_ompt_cb_host_mutex_released( ompt_mutex_t   kind,
                                    ompt_wait_id_t wait_id,
                                    const void*    codeptr_ra );

void
scorep_ompt_cb_host_lock_init( ompt_mutex_t   kind,
                               unsigned int   hint,
                               unsigned int   impl,
                               ompt_wait_id_t wait_id,
                               const void*    codeptr_ra );

void
scorep_ompt_cb_host_lock_destroy( ompt_mutex_t   kind,
                                  ompt_wait_id_t wait_id,
                                  const void*    codeptr_ra );

void
scorep_ompt_cb_host_nest_lock( ompt_scope_endpoint_t endpoint,
                               ompt_wait_id_t        wait_id,
                               const void*           codeptr_ra );

void
scorep_ompt_cb_host_dispatch( ompt_data_t*    parallel_data,
                              ompt_data_t*    task_data,
                              ompt_dispatch_t kind,
                              ompt_data_t     instance );

/* Callback triggered by the flush event that occurs in a thread that encounters
   the flush construct. No relation to trace buffers. */
void
scorep_ompt_cb_host_flush( ompt_data_t* thread_data,
                           const void*  codeptr_ra );

#endif /* SCOREP_OMPT_CALLBACKS_HOST_H */
