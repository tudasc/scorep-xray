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
 * Copyright (c) 2009-2014, 2021,
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

#ifndef SCOREP_CUDA_DEFINES_H
#define SCOREP_CUDA_DEFINES_H

#include <stdint.h>
#include <stdbool.h>

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
#define SCOREP_CUDA_FEATURE_DONTFLUSHATEXIT     ( 1 << 12 )
#define SCOREP_CUDA_FEATURE_KERNEL_CALLSITE     ( 1 << 13 )
#define SCOREP_CUDA_FEATURES_DEFAULT \
    ( SCOREP_CUDA_FEATURE_DRIVER_API | SCOREP_CUDA_FEATURE_KERNEL | \
      SCOREP_CUDA_FEATURE_KERNEL_COUNTER | SCOREP_CUDA_FEATURE_MEMCPY | \
      SCOREP_CUDA_FEATURE_IDLE | SCOREP_CUDA_FEATURE_SYNC | \
      SCOREP_CUDA_FEATURE_GPUMEMUSAGE | SCOREP_CUDA_FEATURE_REFERENCES | \
      SCOREP_CUDA_FEATURE_KERNEL_CALLSITE )


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

#endif /* SCOREP_CUDA_DEFINES_H */
