/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2022-2023,
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

#include <string.h>
#include <stddef.h>
#include <stdio.h>


#include <SCOREP_RuntimeManagement.h>
#include <SCOREP_InMeasurement.h>
#include <SCOREP_Subsystem.h>
#include <SCOREP_Memory.h>
#include <SCOREP_Paradigms.h>
#include <SCOREP_Addr2line.h>
#include <SCOREP_Definitions.h>


/* *INDENT-OFF* */
static int initialize_tool( ompt_function_lookup_t lookup, int initialDeviceNum, ompt_data_t *toolData );
static void register_event_callbacks_host( ompt_set_callback_t setCallback );
static void register_event_callbacks_device( ompt_set_callback_t setCallback );
static void initialize_parameters( void );
static void finalize_tool( ompt_data_t *toolData );
static void cb_registration_status( char* name, ompt_set_result_t status );
/* *INDENT-ON* */


size_t scorep_ompt_subsystem_id;

ompt_get_task_info_t        scorep_ompt_mgmt_get_task_info;
static ompt_finalize_tool_t ompt_finalize_tool;

static bool tool_initialized;
bool        scorep_ompt_record_events   = false;
bool        scorep_ompt_finalizing_tool = false;

SCOREP_ParameterHandle scorep_ompt_parameter_loop_type = SCOREP_INVALID_PARAMETER;


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
    /* TODO decide against initialization if env var is set? */
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

    /* Lookup runtime entry points: */
    /* According to the specification (Monitoring Activity on the Host with
       OMPT), calling the ompt_set_callback runtime entry point is only safe
       from a tool's initializer. This is unfortunate as registering at
       ompt_subsystem_begin and de-registering at ompt_substytem_end would be
       what we need; now we might get events after subsystem_end that we need
       to take care of by maintaining  'scorep_ompt_record_events'. */
    ompt_set_callback_t set_callback =
        ( ompt_set_callback_t )lookup( "ompt_set_callback" );
    UTILS_BUG_ON( set_callback == 0 );
    scorep_ompt_mgmt_get_task_info =
        ( ompt_get_task_info_t )lookup( "ompt_get_task_info" );
    UTILS_BUG_ON( scorep_ompt_mgmt_get_task_info == 0 );
    ompt_finalize_tool =
        ( ompt_finalize_tool_t )lookup( "ompt_finalize_tool" );
    UTILS_BUG_ON( ompt_finalize_tool == 0 );

    /* Use runtime entry point set_callback to register callbacks:
       TODO provide means to selectively register (groups of) callbacks? */
    register_event_callbacks_host( set_callback );
    register_event_callbacks_device( set_callback );
    initialize_parameters();

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


/* *INDENT-OFF* */
#if 0
callbacks available in 5.2:
  ompt_callback_thread_begin            = 1,
  ompt_callback_thread_end              = 2,
  ompt_callback_parallel_begin          = 3,
  ompt_callback_parallel_end            = 4,
  ompt_callback_task_create             = 5,
  ompt_callback_task_schedule           = 6,
  ompt_callback_implicit_task           = 7,
  ompt_callback_target                  = 8,
  ompt_callback_target_data_op          = 9,
  ompt_callback_target_submit           = 10,
  ompt_callback_control_tool            = 11,
  ompt_callback_device_initialize       = 12,
  ompt_callback_device_finalize         = 13,
  ompt_callback_device_load             = 14,
  ompt_callback_device_unload           = 15,
  ompt_callback_sync_region_wait        = 16,
  ompt_callback_mutex_released          = 17,
  ompt_callback_dependences             = 18,
  ompt_callback_task_dependence         = 19,
  ompt_callback_work                    = 20,
  ompt_callback_masked                  = 21,
  ompt_callback_master /*(deprecated)*/ = ompt_callback_masked,
  ompt_callback_target_map              = 22,
  ompt_callback_sync_region             = 23,
  ompt_callback_lock_init               = 24,
  ompt_callback_lock_destroy            = 25,
  ompt_callback_mutex_acquire           = 26,
  ompt_callback_mutex_acquired          = 27,
  ompt_callback_nest_lock               = 28,
  ompt_callback_flush                   = 29,
  ompt_callback_cancel                  = 30,
  ompt_callback_reduction               = 31,
  ompt_callback_dispatch                = 32,
  ompt_callback_target_emi              = 33,
  ompt_callback_target_data_op_emi      = 34,
  ompt_callback_target_submit_emi       = 35,
  ompt_callback_target_map_emi          = 36,
  ompt_callback_error                   = 37
#endif
/* *INDENT-ON* */


static void
register_event_callbacks_host( ompt_set_callback_t setCallback )
{
    /* sort alphabetically */
    REGISTER_CALLBACK( host_, implicit_task );
    REGISTER_CALLBACK( host_, parallel_begin );
    REGISTER_CALLBACK( host_, parallel_end );
    REGISTER_CALLBACK( host_, sync_region );
    REGISTER_CALLBACK( host_, task_create );
    REGISTER_CALLBACK( host_, task_schedule );
    REGISTER_CALLBACK( host_, thread_begin );
    REGISTER_CALLBACK( host_, thread_end );
    REGISTER_CALLBACK( host_, work );
    REGISTER_CALLBACK( host_, masked );
    REGISTER_CALLBACK( host_, mutex_acquire );
    REGISTER_CALLBACK( host_, mutex_acquired );
    REGISTER_CALLBACK( host_, mutex_released );
    #if !HAVE( SCOREP_OMPT_WRONG_TEST_LOCK_MUTEX )
    REGISTER_CALLBACK( host_, lock_init );
    REGISTER_CALLBACK( host_, lock_destroy );
    REGISTER_CALLBACK( host_, nest_lock );
    #endif /* !HAVE( SCOREP_OMPT_WRONG_TEST_LOCK_MUTEX ) */
    REGISTER_CALLBACK( host_, dispatch );
    REGISTER_CALLBACK( host_, flush );
}


static void
register_event_callbacks_device( ompt_set_callback_t setCallback )
{
    REGISTER_CALLBACK(, device_initialize );
}

static void
initialize_parameters( void )
{
    #if !HAVE( SCOREP_OMPT_MISSING_WORK_LOOP_SCHEDULE )
    scorep_ompt_parameter_loop_type = SCOREP_Definitions_NewParameter( "schedule", SCOREP_PARAMETER_STRING );
    #endif /* !HAVE( SCOREP_OMPT_MISSING_WORK_LOOP_SCHEDULE ) */
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
ompt_subsystem_init( void )
{
    UTILS_DEBUG( "[%s] register paradigm", UTILS_FUNCTION_NAME );

    SCOREP_Paradigms_RegisterParallelParadigm(
        SCOREP_PARADIGM_OPENMP,
        SCOREP_PARADIGM_CLASS_THREAD_FORK_JOIN,
        "OpenMP",
        SCOREP_PARADIGM_FLAG_NONE );

    SCOREP_Paradigms_SetStringProperty( SCOREP_PARADIGM_OPENMP,
                                        SCOREP_PARADIGM_PROPERTY_COMMUNICATOR_TEMPLATE,
                                        "Thread team ${id}" );

    SCOREP_Addr2line_RegisterObjcloseCb( scorep_ompt_codeptr_hash_dlclose_cb );

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

    // TODO: free task and parallel (+task_generation_numbers) pools at finalization

    UTILS_DEBUG( "[%s] stop recording OMPT events", UTILS_FUNCTION_NAME );
}


static SCOREP_ErrorCode
ompt_subsystem_init_location( struct SCOREP_Location* newLocation,
                              struct SCOREP_Location* parentLocation )
{
    static bool already_called = false;
    if ( !already_called )
    {
        already_called = true;
        UTILS_DEBUG( "[%s] initial location %p", UTILS_FUNCTION_NAME, newLocation );
        UTILS_BUG_ON( tool_initialized,
                      "ompt tool initialization needs valid location" );
    }
    else
    {
        UTILS_DEBUG( "[%s] new location %p", UTILS_FUNCTION_NAME, newLocation );
    }

    if ( SCOREP_Location_GetType( newLocation ) == SCOREP_LOCATION_TYPE_CPU_THREAD )
    {
        scorep_ompt_cpu_location_data* data =
            SCOREP_Memory_AlignedAllocForMisc( SCOREP_CACHELINESIZE, sizeof( *data ) );
        memset( data, 0, sizeof( *data ) );
        SCOREP_Location_SetSubsystemData( newLocation,
                                          scorep_ompt_subsystem_id,
                                          data );
    }
    return SCOREP_SUCCESS;
}


const SCOREP_Subsystem SCOREP_Subsystem_OmptAdapter =
{
    .subsystem_name                   = "OMPT",
    .subsystem_register               = &ompt_subsystem_register,
    .subsystem_init                   = &ompt_subsystem_init,
    .subsystem_begin                  = &ompt_subsystem_begin,
    .subsystem_end                    = &ompt_subsystem_end,
    .subsystem_init_location          = &ompt_subsystem_init_location,
    .subsystem_trigger_overdue_events = &scorep_ompt_subsystem_trigger_overdue_events
};
