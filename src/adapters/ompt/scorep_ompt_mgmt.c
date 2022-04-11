/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2022,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 * @file
 */

#include <config.h>

#include "scorep_ompt.h"
#include "scorep_ompt_callbacks_host.h"
#include "scorep_ompt_callbacks_device.h"

#include <UTILS_Error.h>
#define SCOREP_DEBUG_MODULE_NAME OMPT
#include <UTILS_Debug.h>


#include <SCOREP_RuntimeManagement.h>
#include <SCOREP_InMeasurement.h>
#include <SCOREP_Subsystem.h>
//#include <SCOREP_Events.h>
//#include <SCOREP_Mutex.h>
//#include <SCOREP_Definitions.h>
//#include <SCOREP_Filtering.h>



/* *INDENT-OFF* */
static int initialize_tool( ompt_function_lookup_t lookup, int initialDeviceNum, ompt_data_t *toolData );
static void register_event_callbacks_host( ompt_set_callback_t setCallback );
static void register_event_callbacks_device( ompt_set_callback_t setCallback );
static void finalize_tool( ompt_data_t *toolData );
static void cb_registration_status( char* name, ompt_set_result_t status );
/* *INDENT-ON* */


size_t scorep_ompt_subsystem_id;

ompt_get_task_info_t        scorep_ompt_mgmt_get_task_info;
static ompt_finalize_tool_t ompt_finalize_tool;

static bool tool_initialized;
bool        scorep_ompt_record_events   = false;
bool        scorep_ompt_finalizing_tool = false;


/* Called by the OpenMP runtime. Everything starts from here. */
ompt_start_tool_result_t*
ompt_start_tool( unsigned int omp_version, /* == _OPENMP */
                 const char*  runtime_version )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();

    UTILS_DEBUG( "[%s] omp_version=\"%d\"; runtime_version=\"%s\"",
                 UTILS_FUNCTION_NAME, omp_version, runtime_version );
    if ( SCOREP_IS_MEASUREMENT_PHASE( PRE ) )
    {
        SCOREP_InitMeasurement();
    }
    // TODO decide against initialization if env var is set?
    static ompt_start_tool_result_t tool = { &initialize_tool,
                                             &finalize_tool,
                                             ompt_data_none };

    SCOREP_IN_MEASUREMENT_DECREMENT();
    return &tool;
}


static int
initialize_tool( ompt_function_lookup_t lookup,
                 int                    initialDeviceNum,
                 ompt_data_t*           toolData )
{
    UTILS_DEBUG( "[%s] initial_device_num=%d",
                 UTILS_FUNCTION_NAME, initialDeviceNum );

    /* According to the specification (Monitoring Activity on the Host with
       OMPT), calling the ompt_set_callback runtime entry point is only safe
       from a tool's initializer. This is unfortunate as registering at
       ompt_subsystem_begin and de-registering at ompt_substytem_end would be
       what we need; now we might get events after subsystem_end that we need
       to take care of by maintaining  'scorep_ompt_record_events'. */
    ompt_set_callback_t set_callback =
        ( ompt_set_callback_t )lookup( "ompt_set_callback" );
    // TODO provide means to selectively register (groups of) callbacks
    register_event_callbacks_host( set_callback );
    register_event_callbacks_device( set_callback );

    scorep_ompt_mgmt_get_task_info =
        ( ompt_get_task_info_t )lookup( "ompt_get_task_info" );
    UTILS_BUG_ON( scorep_ompt_mgmt_get_task_info == 0 );
    ompt_finalize_tool = ( ompt_finalize_tool_t )lookup( "ompt_finalize_tool" );
    UTILS_BUG_ON( ompt_finalize_tool == 0 );

    tool_initialized = true;
    return 1; /* non-zero indicates success */
}


static void
finalize_tool( ompt_data_t* toolData )
{
    UTILS_DEBUG( "[%s]", UTILS_FUNCTION_NAME );
}


#define REGISTER_CALLBACK( PREFIX, NAME ) \
    cb_registration_status( #NAME, \
                            setCallback( ompt_callback_ ## NAME, \
                                         ( ompt_callback_t )&scorep_ompt_cb_ ## PREFIX ## NAME ) )


static void
register_event_callbacks_host( ompt_set_callback_t setCallback )
{
    /* sort alphabetically */
    REGISTER_CALLBACK( host_, thread_begin );
    REGISTER_CALLBACK( host_, thread_end );
}


static void
register_event_callbacks_device( ompt_set_callback_t setCallback )
{
    // REGISTER_CALLBACK( , device_initialize );
}


static void
cb_registration_status( char*             name,
                        ompt_set_result_t status )
{
#if HAVE( UTILS_DEBUG )
    char* status_str = NULL;
    switch ( status )
    {
        case ompt_set_error:
            status_str = "error";
            break;
        case ompt_set_never:
            status_str = "never";
            break;
        case ompt_set_impossible:
            status_str = "impossible";
            break;
        case ompt_set_sometimes:
            status_str = "sometimes";
            break;
        case ompt_set_sometimes_paired:
            status_str = "sometimes_paired";
            break;
        case ompt_set_always:
            status_str = "always";
            break;
    }
#endif /* HAVE( UTILS_DEBUG ) */
    UTILS_DEBUG( "[%s] registering ompt_callback_%s: %s",
                 UTILS_FUNCTION_NAME, name, status_str );
}


static SCOREP_ErrorCode
ompt_subsystem_register( size_t id )
{
    UTILS_DEBUG( "[%s] OMPT subsystem id: %zu", UTILS_FUNCTION_NAME, id );
    scorep_ompt_subsystem_id = id;
    return SCOREP_SUCCESS;
}


static SCOREP_ErrorCode
ompt_subsystem_begin( void )
{
    UTILS_DEBUG( "[%s] start recording OMPT events", UTILS_FUNCTION_NAME );
    scorep_ompt_record_events = true;
    return SCOREP_SUCCESS;
}


static void
ompt_subsystem_end( void )
{
    if ( !tool_initialized )
    {
        UTILS_DEBUG( "tool wasn't initialized" );
        return;
    }

    UTILS_DEBUG( "[%s] finalizing tool, might trigger overdue OMPT events",
                 UTILS_FUNCTION_NAME );
    scorep_ompt_finalizing_tool = true;
    ompt_finalize_tool();

    /* ignore subsequent events */
    scorep_ompt_record_events = false;
    UTILS_DEBUG( "[%s] stop recording OMPT events", UTILS_FUNCTION_NAME );
}


const SCOREP_Subsystem SCOREP_Subsystem_OmptAdapter =
{
    .subsystem_name     = "OMPT",
    .subsystem_register = &ompt_subsystem_register,
    .subsystem_begin    = &ompt_subsystem_begin,
    .subsystem_end      = &ompt_subsystem_end
};
