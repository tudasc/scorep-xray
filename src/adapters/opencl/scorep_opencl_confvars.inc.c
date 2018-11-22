/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2014-2015,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */


/**
 * @file
 *
 * @brief Environment variable to configure the OpenCL adapter.
 */


uint64_t scorep_opencl_features;
size_t   scorep_opencl_queue_size;


/*
 * Mapping of options for OpenCL measurement to internal representation
 * (bit mask).
 */
static const SCOREP_ConfigType_SetEntry opencl_enable_groups[] = {
    {
        "api",
        SCOREP_OPENCL_FEATURE_API,
        "OpenCL runtime API"
    },
    {
        "kernel",
        SCOREP_OPENCL_FEATURE_KERNEL,
        "OpenCL kernels"
    },
    {
        "memcpy",
        SCOREP_OPENCL_FEATURE_MEMCPY,
        "OpenCL buffer reads/writes"
    },
    {
        "default/yes/true/1",
        SCOREP_OPENCL_FEATURES_DEFAULT,
        "OpenCL API and GPU activities.\n"
        "Includes:\n"
        "\240-\240api\n"
        "\240-\240kernel\n"
        "\240-\240  memcpy"
    },
    { NULL, 0, NULL }
};


/*
 *  Configuration variables for the OpenCL adapter.
 */
static const SCOREP_ConfigVariable scorep_opencl_confvars[] = {
    {
        "enable",
        SCOREP_CONFIG_TYPE_BITSET, /* type */
        &scorep_opencl_features,   /* pointer to target variable */
        ( void* )opencl_enable_groups,
        "no",                      /* default value */
        "OpenCL measurement features",
        "Sets the OpenCL measurement mode to capture:"
    },
    {
        "buffer_queue",
        SCOREP_CONFIG_TYPE_SIZE,
        &scorep_opencl_queue_size,
        NULL,
        "8k",
        "Memory in bytes for the OpenCL command queue buffer",
        ""
    },
    SCOREP_CONFIG_TERMINATOR
};
