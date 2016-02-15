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
 *  @file
 *
 *  @brief Interface for the feature configuration of the OpenCL adapter.
 */

#ifndef SCOREP_OPENCL_CONFIG_H
#define SCOREP_OPENCL_CONFIG_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>


/**
 * OpenCL features (to be enabled/disabled via environment variables)
 *
 * @def SCOREP_OPENCL_FEATURE_API
 * Record OpenCL API calls
 *
 * @def SCOREP_OPENCL_FEATURE_KERNEL
 * Record OpenCL kernels
 *
 * @def SCOREP_OPENCL_FEATURE_MEMCPY
 * Record OpenCL memory tansfers
 *
 * @def SCOREP_OPENCL_FEATURE_SYNC
 * Record OpenCL synchronization operations
 *
 * @def SCOREP_OPENCL_FEATURE_DEFAULT
 * Record the default set of OpenCL features (API, KERNEL, MEMCPY)
 */
#define SCOREP_OPENCL_FEATURE_API                 ( 1 << 0 )
#define SCOREP_OPENCL_FEATURE_KERNEL              ( 1 << 1 )
#define SCOREP_OPENCL_FEATURE_MEMCPY              ( 1 << 2 )
#define SCOREP_OPENCL_FEATURE_SYNC                ( 1 << 4 )
#define SCOREP_OPENCL_FEATURES_DEFAULT \
    ( SCOREP_OPENCL_FEATURE_API | SCOREP_OPENCL_FEATURE_KERNEL | \
      SCOREP_OPENCL_FEATURE_MEMCPY )

/**
 * Specifies the recorded OpenCL features in a bit mask.
 * See SCOREP_ConfigType_SetEntry of OPENCL adapter.
 */
extern uint64_t scorep_opencl_features;

/** Enable/Disable recording of OpenCL API calls */
extern bool scorep_opencl_record_api;

/**
 * Is OpenCL kernel measurement enabled?
 *  SCOREP_OPENCL_NO_KERNEL : no
 *  SCOREP_OPENCL_KERNEL : measure execution time
 *  SCOREP_OPENCL_KERNEL_AND_COUNTER : write additional counters (grid size, blocks, threads, shared mem, etc.)
 */
extern bool scorep_opencl_record_kernels;

/** flag: Are OpenCL memory copies recorded? */
extern bool scorep_opencl_record_memcpy;

/** flag: Is implicit synchronization exposed? */
extern bool scorep_opencl_record_sync;

/** Total size for the OpenCL buffer */
extern size_t scorep_opencl_buffer_size;

/** Buffer chunks used for OpenCL queues */
extern size_t scorep_opencl_queue_size;

/** ID of the OpenCL subsystem */
extern size_t scorep_opencl_subsystem_id;

/**
 * Parses user settings and sets up the bit mask of recorded OpenCL features.
 */
void
scorep_opencl_set_features( void );

/**
 * Disables recording of all OpenCL features.
 */
void
scorep_opencl_disable( void );

#endif /* SCOREP_OPENCL_CONFIG_H */
