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
 *  @file       SCOREP_Cuda_Init.c
 *  @maintainer Robert Dietrich <robert.dietrich@zih.tu-dresden.de>
 *
 *  This file contains the implementation of the initialization functions of the
 *  CUDA adapter.
 */

#include <config.h>
#include <SCOREP_RuntimeManagement.h>
#include <SCOREP_Definitions.h>
#include <SCOREP_Events.h>
#include "SCOREP_Config.h"
#include "SCOREP_Types.h"
#include "SCOREP_Cuda_Init.h"

#include <SCOREP_Debug.h>
#include <SCOREP_Timing.h>

#include "scorep_cuda.h"
#include "scorep_cupti_callbacks.h"

uint64_t scorep_cuda_features = 0;

uint64_t scorep_cuda_init_timestamp = 0;

/*
 * Mapping of options for CUDA measurement to internal representation
 * (bit mask).
 */
static const SCOREP_ConfigType_SetEntry scorep_cuda_enable_groups[] = {
    { "runtime", SCOREP_CUDA_RECORD_RUNTIME_API   },
    { "driver",  SCOREP_CUDA_RECORD_DRIVER_API    },
    { "kernel",  SCOREP_CUDA_RECORD_KERNEL        },
    { "memcpy",  SCOREP_CUDA_RECORD_MEMCPY        },
    { "gpu",     SCOREP_CUDA_RECORD_KERNEL      |
      SCOREP_CUDA_RECORD_MEMCPY },
    { "idle",    SCOREP_CUDA_RECORD_IDLE          },
    { "DEFAULT", SCOREP_CUDA_RECORD_DEFAULT       },
    { "1",       SCOREP_CUDA_RECORD_DEFAULT       },
    { "yes",     SCOREP_CUDA_RECORD_DEFAULT       },
    { "no",      0                                },
    { NULL,      0                                }
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
        "CUDA measurement features.",
        "Sets the CUDA measurement mode to capture:\n"
        "runtime: CUDA runtime API\n"
        "driver: CUDA driver API\n"
        "gpu: GPU activities\n"
        "kernel: CUDA kernels\n"
        "idle: GPU compute idle time\n"
        "memcpy: CUDA memory copies (not available yet)\n"
        "DEFAULT/yes/1: CUDA runtime API and GPU activities\n"
        "no: disable CUDA measurement (same as unset SCOREP_CUDA_ENABLE)"
    },
    SCOREP_CONFIG_TERMINATOR
};

/** Registers the required configuration variables of the CUDA adapter
    to the measurement system.
 */
SCOREP_Error_Code
scorep_cuda_register()
{
    SCOREP_DEBUG_RAW_PRINTF( SCOREP_DEBUG_CUDA,
                             "[CUDA Adapter] Register environment variables.\n" );

    return SCOREP_ConfigRegister( "cuda", scorep_cuda_configs );
}

/** De-registers the CUDA adapter. */
void
scorep_cuda_deregister()
{
}

static int
scorep_cuda_finalize_callback( void )
{
    SCOREP_DEBUG_RAW_PRINTF( SCOREP_DEBUG_CUDA,
                             "[CUDA Adapter] Register finalize callback\n" );

    scorep_cupti_callbacks_finalize();

    return 0;
}

/** Initializes the CUDA adapter. */
SCOREP_Error_Code
scorep_cuda_init()
{
    SCOREP_DEBUG_RAW_PRINTF( SCOREP_DEBUG_CUDA,
                             "[CUDA Adapter] Selected options: %llu\n", scorep_cuda_features );

    if ( scorep_cuda_features > 0 )
    {
        SCOREP_RegisterExitCallback( scorep_cuda_finalize_callback );

        scorep_cuda_init_timestamp = SCOREP_GetClockTicks();

        scorep_cupti_callbacks_init();
    }

    return SCOREP_SUCCESS;
}

/** Initializes the location specific data of the CUDA adapter */
SCOREP_Error_Code
scorep_cuda_init_location()
{
    return SCOREP_SUCCESS;
}

/** Finalizes the location specific data of the CUDA adapter. */
void
scorep_cuda_final_location( SCOREP_Location* location )
{
}

/** Finalizes the CUDA adapter. */
void
scorep_cuda_finalize()
{
    if ( scorep_cuda_features > 0 )
    {
        scorep_cupti_callbacks_finalize();
    }
}

SCOREP_Subsystem SCOREP_Cuda_Adapter =
{
    "CUDA",
    &scorep_cuda_register,
    &scorep_cuda_init,
    &scorep_cuda_init_location,
    &scorep_cuda_final_location,
    &scorep_cuda_finalize,
    &scorep_cuda_deregister
};
