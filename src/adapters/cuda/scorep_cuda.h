/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2014, 2016,
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
 *  @file
 *
 *  This file provides commonly used definitions and functionality in the CUDA
 *  adapter.
 */

#ifndef SCOREP_CUDA_H
#define SCOREP_CUDA_H

#include <stdint.h>
#include <stdbool.h>

#include <SCOREP_Definitions.h>

/*
 * CUDA features (to be enabled/disabled via environment variables)
 */
#define SCOREP_CUDA_FEATURE_RUNTIME_API         ( 1 << 0 )
#define SCOREP_CUDA_FEATURE_DRIVER_API          ( 1 << 1 )
#define SCOREP_CUDA_FEATURE_KERNEL              ( 1 << 2 )
#define SCOREP_CUDA_FEATURE_KERNEL_SERIAL       ( 1 << 3 )
#define SCOREP_CUDA_FEATURE_KERNEL_COUNTER      ( 1 << 4 )
#define SCOREP_CUDA_FEATURE_MEMCPY              ( 1 << 5 )
#define SCOREP_CUDA_FEATURE_IDLE                ( 1 << 6 )
#define SCOREP_CUDA_FEATURE_PURE_IDLE           ( 1 << 7 )
#define SCOREP_CUDA_FEATURE_SYNC                ( 1 << 8 )
#define SCOREP_CUDA_FEATURE_GPUMEMUSAGE         ( 1 << 9 )
#define SCOREP_CUDA_FEATURE_FLUSHATEXIT         ( 1 << 10 )
#define SCOREP_CUDA_FEATURE_REFERENCES          ( 1 << 11 )
#define SCOREP_CUDA_FEATURES_DEFAULT \
    ( SCOREP_CUDA_FEATURE_RUNTIME_API | SCOREP_CUDA_FEATURE_KERNEL | \
      SCOREP_CUDA_FEATURE_MEMCPY )


/*
 * Defines for CUDA GPU idle types
 */
#define SCOREP_CUDA_NO_IDLE 0
#define SCOREP_CUDA_COMPUTE_IDLE 1
#define SCOREP_CUDA_PURE_IDLE 2

/*
 * Defines for CUDA kernel record level
 */
#define SCOREP_CUDA_NO_KERNEL 0
#define SCOREP_CUDA_KERNEL 1
#define SCOREP_CUDA_KERNEL_AND_COUNTER 2

/*
 * Defines for CUDA synchronization recording
 */
#define SCOREP_CUDA_NO_SYNC 0
#define SCOREP_CUDA_RECORD_SYNC 1
#define SCOREP_CUDA_RECORD_SYNC2 2 /* NOT yet implemented */
#define SCOREP_CUDA_RECORD_SYNC_FULL 3

/*
 * Defines for CUDA GPU memory usage
 */
#define SCOREP_CUDA_NO_GPUMEMUSAGE 0
#define SCOREP_CUDA_GPUMEMUSAGE 1
#define SCOREP_CUDA_GPUMEMUSAGE_AND_MISSING_FREES 2 /* NOT yet available */

/*
 * ID of the CUDA subsystem
 */
extern size_t scorep_cuda_subsystem_id;

/*
 * Specifies the CUDA tracing mode with a bit mask.
 * See SCOREP_ConfigType_SetEntry of CUDA adapter.
 */
extern uint64_t scorep_cuda_features;

/*
 * Is CUDA kernel measurement enabled?
 *  SCOREP_CUDA_NO_KERNEL : no
 *  SCOREP_CUDA_KERNEL : measure execution time
 *  SCOREP_CUDA_KERNEL_AND_COUNTER : write additional counters (grid size, blocks, threads, shared mem, etc.)
 */
extern uint8_t scorep_cuda_record_kernels;

/*
 * flag: Write records for GPU idle time?
 * SCOREP_CUDA_NO_IDLE: GPU idle time is not recorded
 * SCOREP_CUDA_COMPUTE_IDLE: GPU compute idle time is recorded
 * SCOREP_CUDA_PURE_IDLE: GPU idle time is recorded (memory copies are not idle)
 */
extern uint8_t scorep_cuda_record_idle;

/*
 * flag: Are CUDA memory copies recorded?
 */
extern bool scorep_cuda_record_memcpy;

/*
 * Defines the synchronization level. Currently only full synchronization
 * (level 3) is supported.
 */
extern uint8_t scorep_cuda_sync_level;

/*
 * Record memory usage of cudaMalloc*, cudaFree*, cuMalloc*, cuFree* functions.
 *  SCOREP_CUDA_NO_MEMUSAGE_COUNTER : no
 *  SCOREP_CUDA_MEMUSAGE_COUNTER : write counter
 *  SCOREP_CUDA_MEMUSAGE_AND_MISSING_FREES : user output for not freed CUDA memory allocations
 */
extern uint8_t scorep_cuda_record_gpumemusage;

/*
 * flag: Shall record reference information for CUDA activities?
 */
extern bool scorep_cuda_record_references;

/*
 * The default buffer size for the CUPTI activity buffer.
 */
extern size_t scorep_cupti_activity_buffer_size;

/*
 * The chunk size for CUPTI activity asynchronous buffer handling
 */
extern size_t scorep_cupti_activity_buffer_chunk_size;

extern size_t    scorep_cuda_global_location_number;
extern uint64_t* scorep_cuda_global_location_ids;

/* handles for CUDA communication unification */
extern SCOREP_InterimCommunicatorHandle scorep_cuda_interim_communicator_handle;
extern SCOREP_RmaWindowHandle           scorep_cuda_window_handle;

void
scorep_cuda_set_features( void );

void
scorep_cuda_define_cuda_locations( void );

void
scorep_cuda_define_cuda_group( void );

#endif /* SCOREP_CUDA_H */
