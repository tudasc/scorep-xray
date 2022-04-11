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

#include "scorep_ompt_callbacks_host.h"

#include <inttypes.h>

//#include <SCOREP_RuntimeManagement.h>
#include <SCOREP_InMeasurement.h>
#include <UTILS_Atomic.h>
//#include <SCOREP_Events.h>
//#include <SCOREP_Mutex.h>
//#include <SCOREP_Definitions.h>
//#include <SCOREP_Filtering.h>

/* Unique, adapter-local thread id. Initialized in scorep_ompt_cb_thread_begin().
   primary thread's tid == 1 to distinguish from the not-initialized state.
   In debug output, named 'atid' to distinguish from other thread ids. */
static THREAD_LOCAL_STORAGE_SPECIFIER uint32_t adapter_tid;

void
scorep_ompt_cb_host_thread_begin( ompt_thread_t thread_type,
                                  ompt_data_t*  thread_data )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();

    UTILS_BUG_ON( adapter_tid != 0 );
    static uint32_t thread_counter = 0;
    adapter_tid = UTILS_Atomic_AddFetch_uint32(
        &thread_counter, 1, UTILS_ATOMIC_SEQUENTIAL_CONSISTENT );
    thread_data->value = adapter_tid;

    UTILS_DEBUG( "[%s] atid %" PRIu32 " | thread_type %s",
                 UTILS_FUNCTION_NAME, adapter_tid, thread2string( thread_type ) );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}


void
scorep_ompt_cb_host_thread_end( ompt_data_t* thread_data )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    UTILS_DEBUG( "[%s] atid %" PRIu32,
                 UTILS_FUNCTION_NAME, thread_data->value );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
