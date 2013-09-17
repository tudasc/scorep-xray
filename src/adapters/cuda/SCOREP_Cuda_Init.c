/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2013,
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
 *  @file
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

#include <UTILS_Error.h>
#define SCOREP_DEBUG_MODULE_NAME CUDA
#include <UTILS_Debug.h>
#include <SCOREP_Timing.h>

#include "scorep_cuda.h"
#include "scorep_cupti_callbacks.h"


#include "scorep_cuda_confvars.inc.c"


/** Registers the required configuration variables of the CUDA adapter
    to the measurement system.
 */
static SCOREP_ErrorCode
scorep_cuda_register( size_t subsystem_id )
{
    UTILS_DEBUG( "Register environment variables" );

    return SCOREP_ConfigRegister( "cuda", scorep_cuda_configs );
}


/** De-registers the CUDA adapter. */
static void
scorep_cuda_deregister( void )
{
}


static int
scorep_cuda_finalize_callback( void )
{
    UTILS_DEBUG( "Register finalize callback" );

    scorep_cupti_callbacks_finalize();

    return 0;
}

/** Initializes the CUDA adapter. */
static SCOREP_ErrorCode
scorep_cuda_init( void )
{
    UTILS_DEBUG( "Selected options: %llu", scorep_cuda_features );

    if ( scorep_cuda_features > 0 )
    {
        SCOREP_RegisterExitCallback( scorep_cuda_finalize_callback );

        scorep_cupti_callbacks_init();
    }

    return SCOREP_SUCCESS;
}

/** Initializes the location specific data of the CUDA adapter */
static SCOREP_ErrorCode
scorep_cuda_init_location( SCOREP_Location* location )
{
    return SCOREP_SUCCESS;
}

/** Finalizes the location specific data of the CUDA adapter. */
static void
scorep_cuda_final_location( SCOREP_Location* location )
{
}

/** Collect locations involved in CUDA communication. */
static SCOREP_ErrorCode
scorep_cuda_pre_unify( void )
{
    /* only if CUDA communication is enabled for recording */
    if ( scorep_cuda_record_memcpy  )
    {
        scorep_cuda_define_cuda_locations();
    }

    return SCOREP_SUCCESS;
}

/** Finalizes the CUDA adapter. */
static SCOREP_ErrorCode
scorep_cuda_post_unify( void )
{
    if ( scorep_cuda_features > 0 )
    {
        scorep_cuda_define_cuda_group();
    }

    return SCOREP_SUCCESS;
}

/** Finalizes the CUDA adapter. */
static void
scorep_cuda_finalize( void )
{
    if ( scorep_cuda_features > 0 )
    {
        scorep_cupti_callbacks_finalize();
    }
}

SCOREP_Subsystem SCOREP_Subsystem_CudaAdapter =
{
    .subsystem_name              = "CUDA",
    .subsystem_register          = &scorep_cuda_register,
    .subsystem_init              = &scorep_cuda_init,
    .subsystem_init_location     = &scorep_cuda_init_location,
    .subsystem_finalize_location = &scorep_cuda_final_location,
    .subsystem_pre_unify         = &scorep_cuda_pre_unify,
    .subsystem_post_unify        = &scorep_cuda_post_unify,
    .subsystem_finalize          = &scorep_cuda_finalize,
    .subsystem_deregister        = &scorep_cuda_deregister,
    .subsystem_control           = NULL
};
