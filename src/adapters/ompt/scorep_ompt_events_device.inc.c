/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 * @file
 */

#include "scorep_ompt_callbacks_device.h"

void
scorep_ompt_cb_device_initialize( int                    device_num,
                                  const char*            type,
                                  ompt_device_t*         device,
                                  ompt_function_lookup_t lookup,
                                  const char*            documentation )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    UTILS_DEBUG( "[%s] atid %" PRIu32 " | device_num %d | type %s | lookup %p | "
                 "documentation %s", UTILS_FUNCTION_NAME, adapter_tid, device_num,
                 type, lookup, documentation );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
