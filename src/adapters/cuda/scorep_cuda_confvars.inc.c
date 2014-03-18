/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2014,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2013,
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


uint64_t scorep_cuda_features;
size_t   scorep_cupti_activity_buffer_size;
size_t   scorep_cupti_activity_buffer_chunk_size;


/*
 * Mapping of options for CUDA measurement to internal representation
 * (bit mask).
 */
static const SCOREP_ConfigType_SetEntry scorep_cuda_enable_groups[] = {
    { "runtime",        SCOREP_CUDA_FEATURE_RUNTIME_API           },
    { "driver",         SCOREP_CUDA_FEATURE_DRIVER_API            },
    { "kernel",         SCOREP_CUDA_FEATURE_KERNEL                },
    { "kernel_serial",  SCOREP_CUDA_FEATURE_KERNEL_SERIAL         },
    { "kernel_counter", SCOREP_CUDA_FEATURE_KERNEL_COUNTER        },
    { "memcpy",         SCOREP_CUDA_FEATURE_MEMCPY                },
    { "sync",           SCOREP_CUDA_FEATURE_SYNC                  },
    { "idle",           SCOREP_CUDA_FEATURE_IDLE                  },
    { "pure_idle",      SCOREP_CUDA_FEATURE_PURE_IDLE             },
    { "device_reuse",   SCOREP_CUDA_FEATURE_DEVICE_REUSE          },
    { "stream_reuse",   SCOREP_CUDA_FEATURE_STREAM_REUSE          },
    { "gpumemusage",    SCOREP_CUDA_FEATURE_GPUMEMUSAGE           },
    { "references",     SCOREP_CUDA_FEATURE_REFERENCES            },
    { "flushatexit",    SCOREP_CUDA_FEATURE_FLUSHATEXIT           },
    { "DEFAULT",        SCOREP_CUDA_FEATURES_DEFAULT              },
    { "1",              SCOREP_CUDA_FEATURES_DEFAULT              },
    { "yes",            SCOREP_CUDA_FEATURES_DEFAULT              },
    { "no",             0                                         },
    { NULL,             0                                         }
};


/*
 *  Configuration variables for the CUDA adapter.
 */
static SCOREP_ConfigVariable scorep_cuda_configs[] = {
    {
        "enable",
        SCOREP_CONFIG_TYPE_BITSET, /* type */
        &scorep_cuda_features,     /* pointer to target variable */
        ( void* )scorep_cuda_enable_groups,
        "no",                      /* default value */
        "CUDA measurement features",
        "Sets the CUDA measurement mode to capture:\n"
        "  runtime:        CUDA runtime API\n"
        "  driver:         CUDA driver API\n"
        "  kernel:         CUDA kernels\n"
        "  kernel_serial:  Serialized kernel recording.\n"
        "  kernel_counter: Fixed CUDA kernel metrics.\n"
        "  idle:           GPU compute idle time\n"
        "  pure_idle:      GPU idle time (memory copies are not idle)\n"
        "  memcpy:         CUDA memory copies\n"
        "  sync:           Record implicit and explicit CUDA synchronization\n"
        "  gpumemusage:    Record CUDA memory (de)allocations as a counter\n"
        "  references:     Record references between CUDA activities\n"
        "  stream_reuse:   Reuse destroyed/closed CUDA streams\n"
        "  device_reuse:   Reuse destroyed/closed CUDA devices\n"
        "  flushatexit:    Flush CUDA activity buffer at program exit\n"
        "  default/yes/1:  CUDA runtime API and GPU activities\n"
        "  no:             Disable CUDA measurement\n"
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
        "Chunk size in bytes for the CUDA record buffer (ignored for CUDA 5.5 and earlier)",
        ""
    },
    SCOREP_CONFIG_TERMINATOR
};
