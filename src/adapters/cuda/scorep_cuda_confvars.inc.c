/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2015,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2014,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2013,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2013,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */


/**
 * @file
 *
 *
 */


#include "scorep_cuda.h"

uint64_t scorep_cuda_features;
size_t   scorep_cupti_activity_buffer_size;
size_t   scorep_cupti_activity_buffer_chunk_size;


/*
 * Mapping of options for CUDA measurement to internal representation
 * (bit mask).
 */
static const SCOREP_ConfigType_SetEntry cuda_enable_groups[] = {
    {
        "runtime",
        SCOREP_CUDA_FEATURE_RUNTIME_API,
        "CUDA runtime API"
    },
    {
        "driver",
        SCOREP_CUDA_FEATURE_DRIVER_API,
        "CUDA driver API"
    },
    {
        "kernel",
        SCOREP_CUDA_FEATURE_KERNEL,
        "CUDA kernels"
    },
    {
        "kernel_serial",
        SCOREP_CUDA_FEATURE_KERNEL_SERIAL,
        "Serialized kernel recording"
    },
    {
        "kernel_counter",
        SCOREP_CUDA_FEATURE_KERNEL_COUNTER,
        "Fixed CUDA kernel metrics"
    },
    {
        "memcpy",
        SCOREP_CUDA_FEATURE_MEMCPY,
        "CUDA memory copies"
    },
    {
        "sync",
        SCOREP_CUDA_FEATURE_SYNC,
        "Record implicit and explicit CUDA synchronization"
    },
    {
        "idle",
        SCOREP_CUDA_FEATURE_IDLE,
        "GPU compute idle time"
    },
    {
        "pure_idle",
        SCOREP_CUDA_FEATURE_PURE_IDLE,
        "GPU idle time (memory copies are not idle)"
    },
    {
        "gpumemusage",
        SCOREP_CUDA_FEATURE_GPUMEMUSAGE,
        "Record CUDA memory (de)allocations as a counter"
    },
    {
        "references",
        SCOREP_CUDA_FEATURE_REFERENCES,
        "Record references between CUDA activities"
    },
    {
        "flushatexit",
        SCOREP_CUDA_FEATURE_FLUSHATEXIT,
        "Flush CUDA activity buffer at program exit"
    },
    {
        "default/yes/1",
        SCOREP_CUDA_FEATURES_DEFAULT,
        "CUDA runtime API and GPU activities.\n"
        "Includes:\n"
        "\240-\240runtime\n"
        "\240-\240kernel\n"
        "\240-\240memcpy"
    },
    { NULL, 0, NULL }
};


/*
 *  Configuration variables for the CUDA adapter.
 */
static const SCOREP_ConfigVariable scorep_cuda_confvars[] = {
    {
        "enable",
        SCOREP_CONFIG_TYPE_BITSET, /* type */
        &scorep_cuda_features,     /* pointer to target variable */
        ( void* )cuda_enable_groups,
        "no",                      /* default value */
        "CUDA measurement features",
        "Sets the CUDA measurement mode to capture:"
    },
    {
        "buffer",
        SCOREP_CONFIG_TYPE_SIZE,
        &scorep_cupti_activity_buffer_size,
        NULL,
        "1M",
        "Total memory in bytes for the CUDA record buffer",
        ""
    },
    {
        "buffer_chunk",
        SCOREP_CONFIG_TYPE_SIZE,
        &scorep_cupti_activity_buffer_chunk_size,
        NULL,
        "8k",
        "Chunk size in bytes for the CUDA record buffer (ignored for CUDA\2405.5 and earlier)",
        ""
    },
    SCOREP_CONFIG_TERMINATOR
};
