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
 * Copyright (c) 2009-2015,
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
#include <SCOREP_InMeasurement.h>
#include <SCOREP_Definitions.h>
#include <SCOREP_Paradigms.h>
#include <SCOREP_Events.h>
#include "SCOREP_Config.h"
#include "SCOREP_Types.h"
#include "SCOREP_Cuda_Init.h"
#include "SCOREP_Memory.h"

#include <UTILS_Error.h>
#define SCOREP_DEBUG_MODULE_NAME CUDA
#include <UTILS_Debug.h>
#include <SCOREP_Timer_Utils.h>

#include "scorep_cuda.h"
#include "scorep_cupti_callbacks.h"


#include "scorep_cuda_confvars.inc.c"
#include "scorep_cupti.h"


/** Registers the required configuration variables of the CUDA adapter
    to the measurement system.
 */
static SCOREP_ErrorCode
cuda_subsystem_register( size_t subsystem_id )
{
    UTILS_DEBUG( "Register environment variables" );

    scorep_cuda_subsystem_id = subsystem_id;

    return SCOREP_ConfigRegister( "cuda", scorep_cuda_confvars );
}

static void
cuda_subsystem_end( void )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();

    UTILS_DEBUG_ENTRY();

    if ( scorep_cuda_features > 0 )
    {
        scorep_cupti_callbacks_finalize();
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}

/** Initializes the CUDA adapter. */
static SCOREP_ErrorCode
cuda_subsystem_init( void )
{
    UTILS_DEBUG( "Selected options: %llu", scorep_cuda_features );

    SCOREP_Paradigms_RegisterParallelParadigm(
        SCOREP_PARADIGM_CUDA,
        SCOREP_PARADIGM_CLASS_ACCELERATOR,
        "CUDA",
        SCOREP_PARADIGM_FLAG_RMA_ONLY );

    if ( scorep_cuda_features > 0 )
    {
        scorep_cupti_callbacks_init();
    }

    return SCOREP_SUCCESS;
}

/** Initializes the location specific data of the CUDA adapter */
static SCOREP_ErrorCode
cuda_subsystem_init_location( SCOREP_Location* location,
                              SCOREP_Location* parent )
{
    /* for CPU locations, create location/thread-specific CUDA data */
    if ( SCOREP_Location_GetType( location ) == SCOREP_LOCATION_TYPE_CPU_THREAD )
    {
        scorep_cuda_location_data* loc_data =
            ( scorep_cuda_location_data* )SCOREP_Memory_AllocForMisc( sizeof( scorep_cuda_location_data ) );

        loc_data->callbacksState = SCOREP_CUPTI_CALLBACKS_STATE_NONE;
        if ( scorep_record_driver_api )
        {
            loc_data->callbacksState |= SCOREP_CUPTI_CALLBACKS_STATE_DRIVER;
        }

        if ( scorep_record_runtime_api )
        {
            loc_data->callbacksState |= SCOREP_CUPTI_CALLBACKS_STATE_RUNTIME;
        }

        // RMA window has not yet been created for this location
        loc_data->rma_window_active = false;

        SCOREP_Location_SetSubsystemData( location, scorep_cuda_subsystem_id, loc_data );
    }

    return SCOREP_SUCCESS;
}

/** Collect locations involved in CUDA communication. */
static SCOREP_ErrorCode
cuda_subsystem_pre_unify( void )
{
    /* only if CUDA communication is enabled for recording */
    if ( scorep_cuda_record_memcpy )
    {
        scorep_cuda_define_cuda_locations();
    }

    return SCOREP_SUCCESS;
}

/** Finalizes the CUDA unification. */
static SCOREP_ErrorCode
cuda_subsystem_post_unify( void )
{
    if ( scorep_cuda_features > 0 )
    {
        scorep_cuda_define_cuda_group();
    }

    return SCOREP_SUCCESS;
}

const SCOREP_Subsystem SCOREP_Subsystem_CudaAdapter =
{
    .subsystem_name          = "CUDA",
    .subsystem_register      = &cuda_subsystem_register,
    .subsystem_end           = &cuda_subsystem_end,
    .subsystem_init          = &cuda_subsystem_init,
    .subsystem_init_location = &cuda_subsystem_init_location,
    .subsystem_pre_unify     = &cuda_subsystem_pre_unify,
    .subsystem_post_unify    = &cuda_subsystem_post_unify
};
