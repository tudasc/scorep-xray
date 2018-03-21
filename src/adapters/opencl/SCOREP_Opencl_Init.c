/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2014-2015,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2015,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 *  @file
 *
 *  @brief Implementation of the initialization/finalization for the OpenCL adapter.
 */

#include <config.h>
#include <SCOREP_RuntimeManagement.h>
#include <SCOREP_InMeasurement.h>
#include <SCOREP_Definitions.h>
#include <SCOREP_Paradigms.h>
#include <SCOREP_Events.h>
#include "SCOREP_Config.h"
#include "SCOREP_Types.h"
#include "SCOREP_Opencl_Init.h"

#include <UTILS_Error.h>
#define SCOREP_DEBUG_MODULE_NAME OPENCL
#include <UTILS_Debug.h>

#include "scorep_opencl.h"
#include "scorep_opencl_config.h"
#include "scorep_opencl_regions.h"
#ifdef SCOREP_LIBWRAP_SHARED
#include "scorep_opencl_function_pointers.h"
#endif

#include "scorep_opencl_confvars.inc.c"


/**
 * Registers the required configuration variables of the OpenCL adapter
 * to the measurement system.
 *
 * @param subsystemId       Subsystem identifier
 *
 * @return SCOREP_SUCCESS if successful, otherwise an error code
 *         is returned
 */
static SCOREP_ErrorCode
opencl_subsystem_register( size_t subsystemId )
{
    scorep_opencl_subsystem_id = subsystemId;

    UTILS_DEBUG( "Register environment variables" );

    return SCOREP_ConfigRegister( "opencl", scorep_opencl_confvars );
}

/**
 * Initializes the OpenCL adapter.
 *
 * @return SCOREP_SUCCESS if successful, otherwise an error code
 *         is returned
 */
static SCOREP_ErrorCode
opencl_subsystem_init( void )
{
    UTILS_DEBUG( "Selected options: %llu", scorep_opencl_features );

    SCOREP_Paradigms_RegisterParallelParadigm(
        SCOREP_PARADIGM_OPENCL,
        SCOREP_PARADIGM_CLASS_ACCELERATOR,
        "OpenCL",
        SCOREP_PARADIGM_FLAG_RMA_ONLY );

#ifdef SCOREP_LIBWRAP_SHARED
    /* need to resolve the function pointers, also when no recording was requested */
    scorep_opencl_register_function_pointers();
#endif

    if ( scorep_opencl_features > 0 )
    {
        scorep_opencl_register_regions();

        scorep_opencl_set_features();

        scorep_opencl_init();
    }

    return SCOREP_SUCCESS;
}

/**
 * Registers the finalization callback of the OpenCL adapter.
 *
 * @return zero on success
 */
static void
opencl_subsystem_end( void )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();

    UTILS_DEBUG_ENTRY();

    if ( scorep_opencl_features > 0 )
    {
        scorep_opencl_finalize();
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}

/**
 * Initializes the location specific data of the OpenCL adapter
 *
 * @param location          Location handle
 *
 * @return SCOREP_SUCCESS if successful, otherwise an error code
 *         is returned
 */
static SCOREP_ErrorCode
opencl_subsystem_init_location( SCOREP_Location* location,
                                SCOREP_Location* parent )
{
    return SCOREP_SUCCESS;
}

/**
 * Collect locations involved in OpenCL communication
 *
 * @return SCOREP_SUCCESS if successful, otherwise an error code
 *         is returned
 */
static SCOREP_ErrorCode
opencl_subsystem_pre_unify( void )
{
    // only if OpenCL communication is enabled for recording
    if ( scorep_opencl_record_memcpy )
    {
        scorep_opencl_define_locations();
    }

    return SCOREP_SUCCESS;
}

/**
 * Finalizes the OpenCL adapter
 *
 * @return SCOREP_SUCCESS if successful, otherwise an error code
 *         is returned
 */
static SCOREP_ErrorCode
opencl_subsystem_post_unify( void )
{
    if ( scorep_opencl_features > 0 )
    {
        scorep_opencl_define_group();
    }

    return SCOREP_SUCCESS;
}

/** OpenCL adapter with its callbacks */
const SCOREP_Subsystem SCOREP_Subsystem_OpenclAdapter =
{
    .subsystem_name          = "OPENCL",
    .subsystem_register      = &opencl_subsystem_register,
    .subsystem_end           = &opencl_subsystem_end,
    .subsystem_init          = &opencl_subsystem_init,
    .subsystem_init_location = &opencl_subsystem_init_location,
    .subsystem_pre_unify     = &opencl_subsystem_pre_unify,
    .subsystem_post_unify    = &opencl_subsystem_post_unify,
};
