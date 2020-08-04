/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2022,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

#include <config.h>

#include "scorep_hip_confvars.h"

#include <SCOREP_Config.h>

uint64_t scorep_hip_features;
uint64_t scorep_hip_activity_buffer_size;


/*
 * Mapping of options for HIP measurement to internal representation
 * (bit mask).
 */
static const SCOREP_ConfigType_SetEntry hip_enable_groups[] =
{
    {
        "api",
        SCOREP_HIP_FEATURE_API,
        "All HIP API calls"
    },
    {
        "kernels",
        SCOREP_HIP_FEATURE_KERNELS,
        "HIP kernels"
    },
    {
        "malloc",
        SCOREP_HIP_FEATURE_MALLOC,
        "HIP allocations"
    },
    {
        "default/yes/1/true",
        SCOREP_HIP_FEATURES_DEFAULT,
        "HIP tracing"
    },
    { NULL, 0, NULL }
};


/*
 *  Configuration variables for the HIP adapter.
 */
static SCOREP_ConfigVariable scorep_hip_confvars[] =
{
    {
        "enable",
        SCOREP_CONFIG_TYPE_BITSET, /* type */
        &scorep_hip_features,      /* pointer to target variable */
        ( void* )hip_enable_groups,
        "yes",                     /* default value */
        "HIP measurement features",
        "Sets the HIP measurement mode to capture:"
    },
    {
        "activity_buffer_size",
        SCOREP_CONFIG_TYPE_SIZE,          /* type */
        &scorep_hip_activity_buffer_size, /* pointer to target variable */
        NULL,
        "4096",                           /* default value */
        "HIP device activity buffer size",
        "Buffer size for device activity events. Must be a power-of-2."
    },
    SCOREP_CONFIG_TERMINATOR
};
