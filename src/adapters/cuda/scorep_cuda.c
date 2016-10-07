/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2012,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2014, 2016,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2012,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2012,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2012,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2012,
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
 *  Implementation of commonly used functionality within the CUDA adapter.
 */

#include <config.h>

#include <UTILS_Error.h>
#include <UTILS_Debug.h>

#include "scorep_cuda.h"
#include "scorep_cupti.h"

size_t scorep_cuda_subsystem_id = 0;

uint8_t scorep_cuda_record_kernels = 0;

uint8_t scorep_cuda_record_idle = 0;

bool scorep_cuda_record_memcpy = false;

uint8_t scorep_cuda_sync_level = 0;

uint8_t scorep_cuda_record_gpumemusage = 0;

bool scorep_cuda_record_references = false;

/* handles for CUDA communication unification */
SCOREP_InterimCommunicatorHandle scorep_cuda_interim_communicator_handle =
    SCOREP_INVALID_INTERIM_COMMUNICATOR;
SCOREP_RmaWindowHandle scorep_cuda_window_handle =
    SCOREP_INVALID_RMA_WINDOW;

size_t scorep_cuda_global_location_number = 0;

uint64_t* scorep_cuda_global_location_ids = NULL;

void
scorep_cuda_set_features( void )
{
    /* check for CUDA kernels */
    if ( ( scorep_cuda_features & SCOREP_CUDA_FEATURE_KERNEL ) ||
         ( scorep_cuda_features & SCOREP_CUDA_FEATURE_KERNEL_SERIAL ) ||
         ( scorep_cuda_features & SCOREP_CUDA_FEATURE_KERNEL_COUNTER ) )
    {
        scorep_cuda_record_kernels = SCOREP_CUDA_KERNEL;

        /* check for idle */
        if ( scorep_cuda_features & SCOREP_CUDA_FEATURE_IDLE )
        {
            scorep_cuda_record_idle = SCOREP_CUDA_COMPUTE_IDLE;
        }

        if ( scorep_cuda_features & SCOREP_CUDA_FEATURE_KERNEL_COUNTER )
        {
            scorep_cuda_record_kernels = SCOREP_CUDA_KERNEL_AND_COUNTER;
        }
    }

    /* check for CUDA memcpy */
    if ( scorep_cuda_features & SCOREP_CUDA_FEATURE_MEMCPY )
    {
        scorep_cuda_record_memcpy = true;

        if ( scorep_cuda_features & SCOREP_CUDA_FEATURE_PURE_IDLE )
        {
            scorep_cuda_record_idle = SCOREP_CUDA_PURE_IDLE;
        }
    }

    /* check for CUDA GPU memory usage */
    if ( scorep_cuda_features & SCOREP_CUDA_FEATURE_GPUMEMUSAGE )
    {
        scorep_cuda_record_gpumemusage = SCOREP_CUDA_GPUMEMUSAGE;
    }

    /* check for features, which are only available, when something is recorded on the device */
    if ( scorep_cuda_record_kernels || scorep_cuda_record_memcpy || scorep_cuda_record_gpumemusage )
    {
        if ( scorep_cuda_features & SCOREP_CUDA_FEATURE_SYNC )
        {
            scorep_cuda_sync_level = SCOREP_CUDA_RECORD_SYNC_FULL;
        }

        if ( scorep_cuda_features & SCOREP_CUDA_FEATURE_REFERENCES )
        {
            scorep_cuda_record_references = true;
        }
    }
    else
    {
        /* Warn if required feature is missing */
        if ( scorep_cuda_features & SCOREP_CUDA_FEATURE_SYNC )
        {
            UTILS_WARNING( "CUDA option 'sync' requires recording of  device activities." );
        }

        if ( scorep_cuda_features & SCOREP_CUDA_FEATURE_REFERENCES )
        {
            UTILS_WARNING( "CUDA option 'references' requires recording of device activities." );
        }
    }

    /* 'references' must be used together with feature 'driver' */
    if ( ( scorep_cuda_features & SCOREP_CUDA_FEATURE_REFERENCES ) &&
         !( scorep_cuda_features & SCOREP_CUDA_FEATURE_DRIVER_API ) )
    {
        UTILS_WARNING( "CUDA option 'references' requires option 'driver'." );
    }
}
