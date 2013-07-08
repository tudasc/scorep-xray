/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2013,
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
 * @status     alpha
 * @file       SCOREP_Cuda_AdapterMockup.c
 * @maintainer Robert Dietrich <robert.dietrich@zih.tu-dresden.de>
 *
 * @brief Compiler adapter interface support to the measurement system.
 *
 * This file contains an empty init structure that can be compiled into
 * a CUDA mockup adapter for every adapter by definiting the adapter name.
 */

#include <config.h>

#include <SCOREP_Subsystem.h>
#include <SCOREP_DefinitionHandles.h>

#include <stdbool.h>

const
SCOREP_Subsystem SCOREP_Subsystem_CudaAdapter =
{
    .subsystem_name              = "CUDA",
    .subsystem_register          = NULL,
    .subsystem_init              = NULL,
    .subsystem_init_location     = NULL,
    .subsystem_finalize_location = NULL,
    .subsystem_pre_unify         = NULL,
    .subsystem_post_unify        = NULL,
    .subsystem_finalize          = NULL,
    .subsystem_deregister        = NULL,
    .subsystem_control           = NULL
};

uint64_t scorep_cuda_features;

uint8_t scorep_cuda_record_kernels;

uint8_t scorep_cuda_record_idle;

bool scorep_cuda_record_memcpy;

uint8_t scorep_cuda_sync_level;

bool scorep_cuda_stream_reuse;

bool scorep_cuda_device_reuse;

uint8_t scorep_cuda_record_gpumemusage;

size_t scorep_cupti_activity_buffer_size;

size_t    scorep_cuda_global_location_number;
uint64_t* scorep_cuda_global_location_ids;

SCOREP_InterimCommunicatorHandle scorep_cuda_interim_communicator_handle;
SCOREP_InterimRmaWindowHandle    scorep_cuda_interim_window_handle;

void
scorep_cuda_set_features( void );

void
scorep_cuda_define_cuda_locations( void );

void
scorep_cuda_define_cuda_group( void );
