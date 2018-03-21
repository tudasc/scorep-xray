/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2015, 2017,
 * Technische Universitaet Dresden, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

#include <config.h>

#include "scorep_unwinding_mgmt.h"

#include <SCOREP_Unwinding.h>
#include <SCOREP_InMeasurement.h>
#include <SCOREP_RuntimeManagement.h>
#include <SCOREP_Memory.h>
#include <SCOREP_Config.h>
#include <SCOREP_Definitions.h>
#include <SCOREP_Subsystem.h>
#include <SCOREP_Location.h>

#define SCOREP_DEBUG_MODULE_NAME UNWINDING
#include <UTILS_Debug.h>
#include <UTILS_CStr.h>
#include <UTILS_IO.h>

#include <unistd.h>
#include <string.h>

#include "scorep_unwinding_region.h"
#include "scorep_unwinding_cpu.h"
#include "scorep_unwinding_gpu.h"

#include "scorep_unwinding_confvars.inc.c"

/** Our subsystem id, used to address our per-location unwinding data */
size_t scorep_unwinding_subsystem_id;

/* *********************************************************************
 * Public functions
 * ****************************************************************** */

void
SCOREP_Unwinding_PushWrapper( SCOREP_Location*    location,
                              SCOREP_RegionHandle regionHandle,
                              uint64_t            wrapperIp,
                              size_t              framesToSkip )
{
    SCOREP_LocationType location_type = SCOREP_Location_GetType( location );
    UTILS_BUG_ON( location_type != SCOREP_LOCATION_TYPE_CPU_THREAD, "This function should not have been called for non-cpu locations." );
    void* location_data = SCOREP_Location_GetSubsystemData( location, scorep_unwinding_subsystem_id );
    scorep_unwinding_cpu_push_wrapper( location_data,
                                       regionHandle,
                                       wrapperIp,
                                       framesToSkip );
}

void
SCOREP_Unwinding_PopWrapper( SCOREP_Location*    location,
                             SCOREP_RegionHandle regionHandle )
{
    SCOREP_LocationType location_type = SCOREP_Location_GetType( location );
    UTILS_BUG_ON( location_type != SCOREP_LOCATION_TYPE_CPU_THREAD, "This function should not have been called for non-cpu locations." );
    void* location_data = SCOREP_Location_GetSubsystemData( location, scorep_unwinding_subsystem_id );
    scorep_unwinding_cpu_pop_wrapper( location_data, regionHandle );
}

/**
 * This function is triggered by enter/leave events of instrumented
 * functions or sampling interrupts. It will unwind and update the stack
 * accordingly.
 *
 * If this function was triggered by an instrumented function
 * @instrumentedRegionHandle references its region handle.
 *
 * If this function was triggered by a sampling interrupt
 * @instrumentedRegionHandle should be SCOREP_INVALID_REGION.
 *
 * Tasks of this function:
 * (A) In case of an enter event:
 *     Update stack (e.g., to include the recently entered instrumented
 *     function)
 *     Write updated stack to events (e.g., as enter or sample records)
 * (B) In case of a leave event:
 *     Get current stack and remove all stack entries until the
 *     instrumented function which has to be left is reached
 *     Write this stack (still including the instrumented function which
 *     has to be left) to events
 *     Update stack and remove the instrumented functions which has to
 *     be left
 * (C) In case of a sample:
 *     Get current stack
 *     Write stack to events
 */
void
SCOREP_Unwinding_GetCallingContext( SCOREP_Location*             location,
                                    void*                        contextPtr,
                                    SCOREP_Unwinding_Origin      origin,
                                    SCOREP_RegionHandle          instrumentedRegionHandle,
                                    SCOREP_CallingContextHandle* currentCallingContext,
                                    SCOREP_CallingContextHandle* previousCallingContext,
                                    uint32_t*                    unwindDistance )
{
    *currentCallingContext  = SCOREP_INVALID_CALLING_CONTEXT;
    *previousCallingContext = SCOREP_INVALID_CALLING_CONTEXT;

    if ( location == NULL )
    {
        return;
    }

    SCOREP_ErrorCode    result        = SCOREP_SUCCESS;
    SCOREP_LocationType location_type = SCOREP_Location_GetType( location );
    void*               location_data = SCOREP_Location_GetSubsystemData( location, scorep_unwinding_subsystem_id );

    switch ( location_type )
    {
        case SCOREP_LOCATION_TYPE_CPU_THREAD:
            /* Handle CPU locations */

            switch ( origin )
            {
                case SCOREP_UNWINDING_ORIGIN_SAMPLE:
                case SCOREP_UNWINDING_ORIGIN_INSTRUMENTED_ENTER:
                    UTILS_BUG_ON( origin == SCOREP_UNWINDING_ORIGIN_SAMPLE
                                  && instrumentedRegionHandle != SCOREP_INVALID_REGION,
                                  "Region handle provided for sample." );
                    UTILS_BUG_ON( origin == SCOREP_UNWINDING_ORIGIN_INSTRUMENTED_ENTER
                                  && instrumentedRegionHandle == SCOREP_INVALID_REGION,
                                  "No region handle provided for enter." );
                    result = scorep_unwinding_cpu_handle_enter( location_data,
                                                                contextPtr,
                                                                instrumentedRegionHandle,
                                                                currentCallingContext,
                                                                unwindDistance,
                                                                previousCallingContext );
                    break;

                case SCOREP_UNWINDING_ORIGIN_INSTRUMENTED_EXIT:
                    result = scorep_unwinding_cpu_handle_exit( location_data,
                                                               currentCallingContext,
                                                               unwindDistance,
                                                               previousCallingContext );
                    break;
            }
            break;

        case SCOREP_LOCATION_TYPE_GPU:
            /* Handle non-CPU location (e.g., locations of GPU streams) */

            switch ( origin )
            {
                case SCOREP_UNWINDING_ORIGIN_SAMPLE:
                    UTILS_BUG( "Processing samples on non-CPU location is currently not supported" );
                    break;

                case SCOREP_UNWINDING_ORIGIN_INSTRUMENTED_ENTER:
                    result = scorep_unwinding_gpu_handle_enter( location_data,
                                                                instrumentedRegionHandle,
                                                                currentCallingContext,
                                                                unwindDistance,
                                                                previousCallingContext );
                    break;

                case SCOREP_UNWINDING_ORIGIN_INSTRUMENTED_EXIT:
                    result = scorep_unwinding_gpu_handle_exit( location_data,
                                                               currentCallingContext,
                                                               unwindDistance,
                                                               previousCallingContext );
                    break;
            }
            break;

        default:
            UTILS_BUG( "Calling context request for invalid location: %u", location_type );
            break;
    }

    if ( SCOREP_SUCCESS != result )
    {
        UTILS_ERROR( result, "Could not process calling context request" );
    }
}

typedef struct process_calling_context_data_struct
{
    SCOREP_Location*                location;
    uint64_t                        timestamp;
    uint64_t*                       metrics;
    SCOREP_CallingContextHandle     previous_calling_context;
    SCOREP_Substrates_EnterRegionCb enter;
    SCOREP_Substrates_ExitRegionCb  exit;
} process_calling_context_data;

static void
process_calling_context( process_calling_context_data* data,
                         SCOREP_CallingContextHandle   callingContext,
                         uint32_t                      counter )
{
    if ( counter == 1 )
    {
        /* we reached the unwind context of the current calling context
           this node must exists in the previous_calling_context, exit all functions
           in the previous calling context */
        SCOREP_CallingContextHandle cct            = data->previous_calling_context;
        SCOREP_CallingContextHandle unwind_context = callingContext;
        if ( unwind_context != SCOREP_INVALID_CALLING_CONTEXT )
        {
            unwind_context = SCOREP_CallingContextHandle_GetParent( unwind_context );
        }
        while ( cct != unwind_context )
        {
            SCOREP_CallingContextHandle parent = SCOREP_CallingContextHandle_GetParent( cct );
            if ( callingContext != SCOREP_INVALID_CALLING_CONTEXT
                 && parent == unwind_context )
            {
                break;
            }

            data->exit( data->location,
                        data->timestamp,
                        SCOREP_CallingContextHandle_GetRegion( cct ),
                        data->metrics );

            cct = parent;
        }
        return;
    }

    process_calling_context( data,
                             SCOREP_CallingContextHandle_GetParent( callingContext ),
                             counter - 1 );

    data->enter( data->location,
                 data->timestamp,
                 SCOREP_CallingContextHandle_GetRegion( callingContext ),
                 data->metrics );
}

void
SCOREP_Unwinding_ProcessCallingContext( SCOREP_Location*                location,
                                        uint64_t                        timestamp,
                                        uint64_t*                       metricValues,
                                        SCOREP_CallingContextHandle     currentCallingContext,
                                        SCOREP_CallingContextHandle     previousCallingContext,
                                        uint32_t                        unwindDistance,
                                        SCOREP_Substrates_EnterRegionCb enterCb,
                                        SCOREP_Substrates_ExitRegionCb  exitCb )
{
    process_calling_context_data data;
    data.location                 = location;
    data.timestamp                = timestamp;
    data.metrics                  = metricValues;
    data.previous_calling_context = previousCallingContext;
    data.enter                    = enterCb;
    data.exit                     = exitCb;

    process_calling_context( &data,
                             currentCallingContext,
                             unwindDistance );
}

/* *********************************************************************
 * Service management
 **********************************************************************/

/** @brief Registers configuration variables for the unwinding services.
 *
 *  @param subsystemId          Identifier of unwinding subsystem.
 *
 *  @return It returns SCOREP_SUCCESS if successful,
 *          otherwise an error code will be reported.
 */
static SCOREP_ErrorCode
unwinding_subsystem_register( size_t subsystemId )
{
    UTILS_DEBUG( "register unwinding management." );

    scorep_unwinding_subsystem_id = subsystemId;

    return SCOREP_ConfigRegister( "unwinding", scorep_unwinding_confvars );
}

static SCOREP_ErrorCode
unwinding_subsystem_pre_unify( void )
{
    if ( !SCOREP_IsUnwindingEnabled() )
    {
        return SCOREP_SUCCESS;
    }

    return scorep_unwinding_unify();
}

/** @brief  Location specific initialization function for unwinding services.
 *
 *  @param location           Location data.
 *  @param parent             Location data from the creator.
 *
 *  @return It returns SCOREP_SUCCESS if successful,
 *          otherwise an error code will be reported.
 */
static SCOREP_ErrorCode
unwinding_subsystem_init_location( SCOREP_Location* location,
                                   SCOREP_Location* parent )
{
    UTILS_ASSERT( location != NULL );

    if ( !SCOREP_IsUnwindingEnabled() )
    {
        return SCOREP_SUCCESS;
    }

    SCOREP_LocationType location_type = SCOREP_Location_GetType( location );

    void* location_data;
    switch ( location_type )
    {
        case SCOREP_LOCATION_TYPE_CPU_THREAD:
            location_data = scorep_unwinding_cpu_get_location_data( location );
            break;

        case SCOREP_LOCATION_TYPE_GPU:
            location_data = scorep_unwinding_gpu_get_location_data( location );
            break;

        case SCOREP_LOCATION_TYPE_METRIC:
            /* No need to handle locations which are just used to store metrics */
            return SCOREP_SUCCESS;

        default:
            UTILS_BUG( "Could not create location data of unwinding service. Location type is not supported." );
            return SCOREP_ERROR_PROCESSED_WITH_FAULTS;
    }

    SCOREP_Location_SetSubsystemData( location,
                                      scorep_unwinding_subsystem_id,
                                      location_data );

    return SCOREP_SUCCESS;
}

static void
unwinding_subsystem_deactivate_cpu_location( SCOREP_Location*        location,
                                             SCOREP_Location*        parentLocation,
                                             SCOREP_CPULocationPhase phase )
{
    if ( !SCOREP_IsUnwindingEnabled() )
    {
        return;
    }

    if ( phase == SCOREP_CPU_LOCATION_PHASE_EVENTS )
    {
        /* The location really goes into idle state, thus tear down */
        void* location_data = SCOREP_Location_GetSubsystemData( location, scorep_unwinding_subsystem_id );
        scorep_unwinding_cpu_deactivate( location_data );
    }
}

/* *********************************************************************
 * Subsystem declaration
 **********************************************************************/

/**
 * Implementation of the unwinding service initialization/finalization struct
 */
const SCOREP_Subsystem SCOREP_Subsystem_UnwindingService =
{
    .subsystem_name                    = "UNWINDING",
    .subsystem_register                = &unwinding_subsystem_register,
    .subsystem_init_location           = &unwinding_subsystem_init_location,
    .subsystem_deactivate_cpu_location = &unwinding_subsystem_deactivate_cpu_location,
    .subsystem_pre_unify               = &unwinding_subsystem_pre_unify
};
