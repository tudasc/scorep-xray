/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2014-2016, 2018,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2016,
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
 *  Implementation of the OpenACC profiling interface - OpenACC event handler.
 */

#include <config.h>

#include <UTILS_Error.h>
#define SCOREP_DEBUG_MODULE_NAME OPENACC
#include <UTILS_Debug.h>

#include <SCOREP_Definitions.h>
#include <SCOREP_Events.h>
#include <SCOREP_Task.h>
#include <SCOREP_RuntimeManagement.h>

#include "scorep_openacc.h"
#include "scorep_openacc_confvars.h"

/**
 * Handle OpenACC start events.
 *
 * @param profInfo
 * @param eventInfo     Information about the specific OpenACC event
 * @param apiInfo
 */
static void
handle_enter_region( acc_prof_info*  profInfo,
                     acc_event_info* eventInfo,
                     acc_api_info*   apiInfo )
{
    if ( profInfo == NULL || !scorep_openacc_features_initialized )
    {
        return;
    }

    // add additional attributes to enqueue upload/download events
    if ( profInfo->event_type == acc_ev_enqueue_upload_start ||
         profInfo->event_type == acc_ev_enqueue_download_start )
    {
        // add attribute for variable name
        if ( scorep_openacc_record_varnames &&
             eventInfo->data_event.var_name )
        {
            SCOREP_StringHandle string_handle =
                SCOREP_Definitions_NewString( eventInfo->data_event.var_name );

            SCOREP_AddAttribute( scorep_openacc_attribute_variable_name, &string_handle );
        }

        // add attribute for bytes to transfer
        if ( eventInfo->data_event.bytes )
        {
            SCOREP_AddAttribute( scorep_openacc_attribute_transfer_bytes,
                                 &( eventInfo->data_event.bytes ) );
        }
    }

    // add attributes for compute regions
    if ( profInfo->event_type == acc_ev_enqueue_launch_start &&
         scorep_openacc_record_kernel_props )
    {
        if ( eventInfo->launch_event.kernel_name )
        {
            SCOREP_StringHandle string_handle =
                SCOREP_Definitions_NewString( eventInfo->launch_event.kernel_name );

            SCOREP_AddAttribute( scorep_openacc_attribute_kernel_name,
                                 &string_handle );
        }

        if ( eventInfo->launch_event.num_gangs )
        {
            SCOREP_AddAttribute( scorep_openacc_attribute_kernel_gangs,
                                 &( eventInfo->launch_event.num_gangs ) );
        }

        if ( eventInfo->launch_event.num_workers )
        {
            SCOREP_AddAttribute( scorep_openacc_attribute_kernel_workers,
                                 &( eventInfo->launch_event.num_workers ) );
        }

        if ( eventInfo->launch_event.vector_length )
        {
            SCOREP_AddAttribute( scorep_openacc_attribute_kernel_veclen,
                                 &( eventInfo->launch_event.vector_length ) );
        }
    }

    SCOREP_RegionHandle region_handle =
        scorep_openacc_get_region_handle( profInfo->line_no,
                                          profInfo->event_type,
                                          profInfo->src_file );

    // pass Score-P region handle to corresponding leave
    // causes compiler warning!!!
    eventInfo->other_event.tool_info = ( void* )region_handle;

    // add an attribute, if this is an implicit region
    if ( eventInfo->other_event.implicit )
    {
        SCOREP_AddAttribute( scorep_openacc_attribute_implicit,
                             &( eventInfo->other_event.implicit ) );
    }

    SCOREP_EnterRegion( region_handle );
}

/**
 * Handle OpenACC leave events. As regions should be perfectly nested, we just
 * need to the exit event with the region handle passed in from the tool info
 * field.
 *
 * @param profInfo
 * @param eventInfo
 * @param apiInfo
 *
 */
static void
handle_leave_region( acc_prof_info*  profInfo,
                     acc_event_info* eventInfo,
                     acc_api_info*   apiInfo )
{
    SCOREP_RegionHandle regionHandle = SCOREP_INVALID_REGION;

    if ( eventInfo )
    {
        regionHandle = ( uint32_t )eventInfo->other_event.tool_info;
    }

    if ( regionHandle )
    {
        SCOREP_ExitRegion( regionHandle );
    }
    else
    {
        UTILS_WARN_ONCE( "[OpenACC] \"tool_info\" has not been set!" );

        SCOREP_Location*    location     = SCOREP_Location_GetCurrentCPULocation();
        SCOREP_TaskHandle   curr_task    = SCOREP_Task_GetCurrentTask( location );
        SCOREP_RegionHandle regionHandle = SCOREP_Task_GetTopRegion( curr_task );

        if ( SCOREP_INVALID_REGION != regionHandle &&
             SCOREP_RegionHandle_GetParadigmType( regionHandle ) == SCOREP_PARADIGM_OPENACC )
        {
            SCOREP_ExitRegion( regionHandle );
        }
        else
        {
            UTILS_WARNING( "[OpenACC] Region handle (leave) for %s:%i invalid ",
                           ( profInfo && profInfo->src_file ) ? profInfo->src_file : "",
                           ( profInfo && profInfo->line_no ) ? profInfo->line_no : SCOREP_INVALID_LINE_NO );
        }
    }
}

/**
 * Handle OpenACC device memory allocations.
 *
 * @param profInfo
 * @param eventInfo
 * @param apiInfo
 */
static void
handle_alloc( acc_prof_info*  profInfo,
              acc_event_info* eventInfo,
              acc_api_info*   apiInfo )
{
    acc_data_event_info data_info = eventInfo->data_event;

    if ( data_info.bytes )
    {
        //if( apiInfo->device_api == acc_devapi_cuda )

        // get metric handle for this device
        struct SCOREP_AllocMetric* allocMetric = scorep_openacc_get_alloc_metric_handle(
            profInfo->device_type, profInfo->device_number );

        switch ( profInfo->event_type )
        {
            case acc_ev_alloc:
                SCOREP_AllocMetric_HandleAlloc( allocMetric,
                                                ( uint64_t )( data_info.device_ptr ),
                                                data_info.bytes );

                break;

            case acc_ev_free:
            {
                void* allocation = NULL;
                SCOREP_AllocMetric_AcquireAlloc( allocMetric,
                                                 ( uint64_t )( data_info.device_ptr ),
                                                 &allocation );

                SCOREP_AllocMetric_HandleFree( allocMetric,
                                               allocation,
                                               NULL );

                break;
            }

            default:
                break;
        }

        // add attribute for variable name
        /*if ( scorep_openacc_record_varnames && data_info.var_name )
           {
            SCOREP_StringHandle string_handle =
                SCOREP_Definitions_NewString( data_info.var_name );

            SCOREP_AddAttribute( scorep_openacc_attribute_variable_name, &string_handle );
           }*/
    }
}

/**
 * This function is called by the OpenACC runtime. It triggers the
 * initialization of Score-P (if necessary) and registers for OpenACC profiling
 * events.
 *
 * @param accRegister       Function to register OpenACC profiling events
 * @param accUnregister     Function to unregister OpenACC profiling events
 * @param lookup            Function to lookup OpenACC profiling routines
 */
void
acc_register_library( acc_prof_reg    accRegister,
                      acc_prof_reg    accUnregister,
                      acc_prof_lookup lookup )
{
    UTILS_DEBUG( "[OpenACC] Register profiling library." );

    // Initialize the measurement system, if necessary
    if ( !SCOREP_IsInitialized() )
    {
        SCOREP_InitMeasurement();
    }

    if ( scorep_openacc_record_regions )
    {
        //register OpenACC device initialization and shutdown
        accRegister( acc_ev_device_init_start, handle_enter_region, 0 );
        accRegister( acc_ev_device_init_end, handle_leave_region, 0 );
        accRegister( acc_ev_device_shutdown_start, handle_enter_region, 0 );
        accRegister( acc_ev_device_shutdown_end, handle_leave_region, 0 );

        //register OpenACC regions
        accRegister( acc_ev_compute_construct_start, handle_enter_region, 0 );
        accRegister( acc_ev_update_start, handle_enter_region, 0 );
        accRegister( acc_ev_enter_data_start, handle_enter_region, 0 );
        accRegister( acc_ev_exit_data_start, handle_enter_region, 0 );
        accRegister( acc_ev_compute_construct_end, handle_leave_region, 0 );
        accRegister( acc_ev_update_end, handle_leave_region, 0 );
        accRegister( acc_ev_enter_data_end, handle_leave_region, 0 );
        accRegister( acc_ev_exit_data_end, handle_leave_region, 0 );
    }

    if ( scorep_openacc_record_enqueue )
    {
        // register kernel launch
        accRegister( acc_ev_enqueue_launch_start, handle_enter_region, 0 );
        accRegister( acc_ev_enqueue_launch_end, handle_leave_region, 0 );

        // register data transfers
        accRegister( acc_ev_enqueue_upload_start, handle_enter_region, 0 );
        accRegister( acc_ev_enqueue_upload_end, handle_leave_region, 0 );
        accRegister( acc_ev_enqueue_download_start, handle_enter_region, 0 );
        accRegister( acc_ev_enqueue_download_end, handle_leave_region, 0 );
    }

    if ( scorep_openacc_record_wait )
    {
        // register wait handlers
        accRegister( acc_ev_wait_start, handle_enter_region, 0 );
        accRegister( acc_ev_wait_end, handle_leave_region, 0 );
    }

    if ( scorep_openacc_record_device_alloc )
    {
        // register memory allocations
        accRegister( acc_ev_alloc, handle_alloc, 0 );
        accRegister( acc_ev_free, handle_alloc, 0 );
    }
} // END: acc_register_library
