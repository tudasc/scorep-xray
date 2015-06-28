/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2014,
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
size_t   scorep_opencl_buffer_size;
size_t   scorep_opencl_queue_size;


/*
 * Mapping of options for OpenCL measurement to internal representation
 * (bit mask).
 */
static const SCOREP_ConfigType_SetEntry scorep_opencl_enable_groups[] =
{
    { "api",     SCOREP_OPENCL_FEATURE_API            },
    { "kernel",  SCOREP_OPENCL_FEATURE_KERNEL         },
    { "memcpy",  SCOREP_OPENCL_FEATURE_MEMCPY         },
    { "sync",    SCOREP_OPENCL_FEATURE_SYNC           },
    { "default", SCOREP_OPENCL_FEATURES_DEFAULT       },
    { "1",       SCOREP_OPENCL_FEATURES_DEFAULT       },
    { "yes",     SCOREP_OPENCL_FEATURES_DEFAULT       },
    { "true",    SCOREP_OPENCL_FEATURES_DEFAULT       },
    { NULL,      0                                    }
};


/*
 *  Configuration variables for the OpenCL adapter.
 */
static SCOREP_ConfigVariable scorep_opencl_configs[] =
{
    {
        "enable",
        SCOREP_CONFIG_TYPE_BITSET, /* type */
        &scorep_opencl_features,   /* pointer to target variable */
        ( void* )scorep_opencl_enable_groups,
        "no",                      /* default value */
        "OpenCL measurement features",
        "Sets the OpenCL measurement mode to capture:\n"
        "  api:            OpenCL runtime API\n"
        "  kernel:         OpenCL kernels\n"
        "  memcpy:         OpenCL buffer reads/writes\n"
        /*"  sync:           Record implicit and explicit OpenCL synchronization\n"*/
        "  memusage:       Record OpenCL memory (de)allocations as a counter\n"
        "  default/yes/1:  OpenCL API and GPU activities\n"
        "  no:             Disable OpenCL measurement"
    },
    {
        "buffer",
        SCOREP_CONFIG_TYPE_SIZE,
        &scorep_opencl_buffer_size,
        NULL,
        "1M",
        "Total memory in bytes for the OpenCL record buffer",
        ""
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
