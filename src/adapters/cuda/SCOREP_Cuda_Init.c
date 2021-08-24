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
 * Copyright (c) 2009-2015, 2021,
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

#include <UTILS_CStr.h>

#include <UTILS_Error.h>
#define SCOREP_DEBUG_MODULE_NAME CUDA
#include <UTILS_Debug.h>
#include <SCOREP_Timer_Utils.h>

#include "scorep_cuda.h"
#include "scorep_cupti_callbacks.h"


#include "scorep_cuda_confvars.inc.c"
#include "scorep_cupti.h"

#if HAVE( NVML_SUPPORT )

#include <nvml.h>

#define NVML_CALL( function, args, ... ) \
    do \
    { \
        nvmlReturn_t ret = function args; \
        if ( ret != NVML_SUCCESS ) \
        { \
            UTILS_WARNING( "[CUDA/NVML] Call to '%s' failed: %s", #function, nvmlErrorString( ret ) ); \
            __VA_ARGS__ \
        } \
    } \
    while ( 0 )

#define CUDART_CALL( function, args, ... ) \
    do \
    { \
        cudaError_t ret = function args; \
        if ( ret != cudaSuccess ) { \
            UTILS_WARNING( "[CUDA] Call to '%s' failed: %s", #function, cudaGetErrorString( ret ) ); \
            __VA_ARGS__ \
        } \
    } \
    while ( 0 )

/* Build-up CUDA Toolkit device index to NVIDIA Driver device index.
 * Will give us correct device IDs in multi-GPU/multi-process */
static SCOREP_ErrorCode
create_visible_devices_map( void )
{
    NVML_CALL( nvmlInit, ( ),
               return SCOREP_WARNING;
               );

    unsigned int nvidia_device_count;
    NVML_CALL( nvmlDeviceGetCount, ( &nvidia_device_count ),
               return SCOREP_WARNING;
               );

    int cuda_device_count;
    CUDART_CALL( cudaGetDeviceCount, ( &cuda_device_count ),
                 return SCOREP_WARNING;
                 );

    scorep_cuda_visible_devices_map = calloc( cuda_device_count, sizeof( *scorep_cuda_visible_devices_map ) );
    UTILS_ASSERT( scorep_cuda_visible_devices_map );

    int device;
    for ( device = 0; device < cuda_device_count; ++device )
    {
        struct cudaDeviceProp device_properties;
        CUDART_CALL( cudaGetDeviceProperties, ( &device_properties, device ),
                     continue;
                     );
        char* uuid_bytes = device_properties.uuid.bytes;
        char  uuid[ NVML_DEVICE_UUID_BUFFER_SIZE ];

        snprintf( uuid, sizeof( uuid ),
                  "GPU-%02hhx%02hhx%02hhx%02hhx-%02hhx%02hhx-%02hhx%02hhx-%02hhx%02hhx-%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx",
                  uuid_bytes[  0 ], uuid_bytes[  1 ], uuid_bytes[  2 ], uuid_bytes[  3 ],
                  uuid_bytes[  4 ], uuid_bytes[  5 ],
                  uuid_bytes[  6 ], uuid_bytes[  7 ],
                  uuid_bytes[  8 ], uuid_bytes[  9 ],
                  uuid_bytes[ 10 ], uuid_bytes[ 11 ], uuid_bytes[ 12 ], uuid_bytes[ 13 ], uuid_bytes[ 14 ], uuid_bytes[ 15 ] );

        nvmlDevice_t nvml_devices;
        NVML_CALL( nvmlDeviceGetHandleByUUID, ( uuid, &nvml_devices ),
                   continue;
                   );

        unsigned int nvml_device_index;
        NVML_CALL( nvmlDeviceGetIndex, ( nvml_devices, &nvml_device_index ),
                   continue;
                   );

        scorep_cuda_visible_devices_map[ device ] = nvml_device_index;
    }

    NVML_CALL( nvmlShutdown, ( ) );

    return SCOREP_SUCCESS;
}

#endif

/**
 * Map of CUDA Toolkit device index to NVIDIA Driver device index.
 *
 * Build-up using NVML library. Parsing CUDA_VISIBLE_DEVICES got complicated,
 * as it can also include GPU UUIDs, which would need to be converted to
 * NVIDIA Driver device index anyway.
 */
uint32_t* scorep_cuda_visible_devices_map = NULL;
size_t    scorep_cuda_visible_devices_len = 0;

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

#if HAVE( NVML_SUPPORT )
    if ( create_visible_devices_map() != SCOREP_SUCCESS )
    {
        UTILS_WARNING( "Query of NVML interface failed. Stream locations will keep "
                       "their logical device ids, which might differ from the "
                       "physical mapping in a multi GPU environment." );
    }
#endif

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
