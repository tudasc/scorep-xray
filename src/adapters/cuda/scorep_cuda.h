/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
 *    RWTH Aachen University, Germany
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *    Technische Universitaet Dresden, Germany
 *    University of Oregon, Eugene, USA
 *    Forschungszentrum Juelich GmbH, Germany
 *    German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *    Technische Universitaet Muenchen, Germany
 *
 * See the COPYING file in the package base directory for details.
 *
 */

/**
 *  @status     alpha
 *  @file       src/adapters/cuda/scorep_cuda.h
 *  @maintainer Robert Dietrich <robert.dietrich@zih.tu-dresden.de>
 *
 *  This file provides commonly used definitions and functionality in the CUDA
 *  adapter.
 */

#ifndef SCOREP_CUDA_H
#define SCOREP_CUDA_H

#include <stdint.h>
#include <stdbool.h>

/*
 * CUDA features (to be enabled/disabled via environment variables)
 */
#define SCOREP_CUDA_RECORD_RUNTIME_API   ( 1 << 0 )
#define SCOREP_CUDA_RECORD_DRIVER_API    ( 1 << 1 )
#define SCOREP_CUDA_RECORD_KERNEL        ( 1 << 2 )
#define SCOREP_CUDA_RECORD_MEMCPY        ( 1 << 3 )
#define SCOREP_CUDA_RECORD_IDLE          ( 1 << 4 )
#define SCOREP_CUDA_RECORD_DEFAULT \
    ( SCOREP_CUDA_RECORD_RUNTIME_API | SCOREP_CUDA_RECORD_KERNEL | \
      SCOREP_CUDA_RECORD_MEMCPY )

/*
 * Specifies the CUDA tracing mode with a bit mask.
 * See SCOREP_ConfigType_SetEntry of CUDA adapter.
 */
extern uint64_t scorep_cuda_features;

/*
 * Is CUDA kernel measurement enabled?
 *  0 : no
 *  1 : measure execution time
 *  2 : write additional counters (grid size, blocks, threads, shared mem, etc.)
 *      NOT YET IMPLEMENTED
 */
extern uint8_t scorep_cuda_record_kernels;

/*
 * flag: Write records for GPU idle time?
 */
extern bool scorep_cuda_record_idle;

/*
 * flag: Are CUDA memory copies recorded?
 */
extern bool scorep_cuda_record_memcpy;

/*
 * The default buffer size for the CUPTI activity buffer.
 */
extern uint64_t scorep_cupti_activity_buffer_size;

#endif  /* SCOREP_CUDA_H */
