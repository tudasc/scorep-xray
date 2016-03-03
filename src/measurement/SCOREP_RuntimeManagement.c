/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2016,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2015,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2014,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2013, 2015,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

/**
 * @file
 *
 * @brief   Definition of runtime management functions to be used by the
 *          subsystem layer.
 *
 *
 */

#include <config.h>
#include <SCOREP_RuntimeManagement.h>

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/types.h>
#include <errno.h>
#include <assert.h>
#include <string.h>
#include <inttypes.h>

#include <UTILS_Error.h>
#define SCOREP_DEBUG_MODULE_NAME CORE
#include <UTILS_Debug.h>

#include <SCOREP_InMeasurement.h>
#include <SCOREP_Memory.h>
#include <SCOREP_Subsystem.h>
#include <SCOREP_Definitions.h>
#include <SCOREP_Metric_Management.h>
#include <SCOREP_Config.h>
#include <SCOREP_Timer_Ticks.h>
#include <SCOREP_Timer_Utils.h>
#include <SCOREP_Events.h>
#include <SCOREP_Filter_Init.h>
#include <scorep_unify.h>
#include <SCOREP_OA_RuntimeManagement.h>
#include <SCOREP_Substrates_Management.h>
#include <scorep_substrates_definition.h>
#include <SCOREP_ErrorCallback.h>
#include <SCOREP_Task.h>

#include "scorep_types.h"
#include "scorep_subsystem.h"
#include "scorep_environment.h"
#include "scorep_status.h"
#include "scorep_ipc.h"
#include <SCOREP_Thread_Mgmt.h>
#include "scorep_location.h"
#include "scorep_runtime_management.h"
#include "scorep_system_tree.h"
#include "scorep_clock_synchronization.h"
#include "scorep_paradigms_management.h"
#include "scorep_properties.h"
#include "scorep_runtime_management_timings.h"

/** @brief Measurement system initialized? */
static bool scorep_initialized = false;


/** @brief Measurement system finalized? */
static bool scorep_finalized = false;

SCOREP_MeasurementPhase scorep_measurement_phase = SCOREP_MEASUREMENT_PHASE_PRE;

/** @brief Location group handle */
static SCOREP_LocationGroupHandle location_group_handle = SCOREP_INVALID_LOCATION_GROUP;

/** @brief System tree path */
static SCOREP_Platform_SystemTreePathElement* system_tree_path;

#define scorep_max_exit_callbacks 1
static SCOREP_ExitCallback scorep_exit_callbacks[ scorep_max_exit_callbacks ];
static int                 scorep_n_exit_callbacks = 0;

/* Artificial regions from Score-P */

/** @brief Region handle to collect data for when measurement is disabled. */
static SCOREP_RegionHandle scorep_record_off_region;

/** @brief Region handle for the trace buffer flush region. */
static SCOREP_RegionHandle scorep_buffer_flush_region;

/** Temporally disable trace event consumption.
 *
 * Controlled by the SCOREP_EnableRecording() and SCOREP_DisableRecording()
 * functions.
 */
static bool scorep_recording_enabled = true;

/** @brief Specifies whether recoding is enabled by default */
static bool scorep_enable_recording_by_default = true;

/** @brief Specifies whether it is allowed to modify the default
 *  recording mode. After initialization, it must not be changed.
 */
static bool scorep_default_recoding_mode_changes_allowed = true;

/**
 * Indicates whether the application initiated an abortion.
 */
static bool scorep_application_aborted = false;

/* *INDENT-OFF* */
/** atexit handler for finalization */
static void scorep_finalize( void );
static void scorep_initialization_sanity_checks( void );
static void scorep_trigger_exit_callbacks( void );
static void scorep_define_measurement_regions( void );
static void scorep_define_measurement_attributes( void );
static void scorep_init_mpp( SCOREP_SynchronizationMode syncMode );
static void scorep_synchronize( SCOREP_SynchronizationMode syncMode );
/* *INDENT-ON* */

/**
 * Return true if SCOREP_InitMeasurement() has been executed.
 */
bool
SCOREP_IsInitialized( void )
{
    UTILS_DEBUG_ENTRY();

    return scorep_initialized && !scorep_finalized;
}

/**
 * Initialize the measurement system from the subsystem layer.
 */
void
SCOREP_InitMeasurement( void )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();

    SCOREP_InitErrorCallback();

    if ( SCOREP_IN_SIGNAL_CONTEXT() )
    {
        UTILS_FATAL( "Can't initialize measurement from the signal handler." );
    }

    if ( scorep_initialized )
    {
        SCOREP_IN_MEASUREMENT_DECREMENT();
        return;
    }

    // even if we are not ready with the initialization we must prevent recursive
    // calls e.g. during the subsystem initialization.
    scorep_initialized = true;
    scorep_initialization_sanity_checks();

    UTILS_DEBUG_ENTRY();

    /* == Initialize the configuration variables and read them from the environment == */

    /* initialize the config system */
    SCOREP_ConfigInit();

    /* Register all config variables */
    SCOREP_RegisterAllConfigVariables();

    /* Parse the environment */
    if ( SCOREP_ConfigApplyEnv() != SCOREP_SUCCESS )
    {
        UTILS_FATAL( "Error while parsing environment variables. Please check "
                     "the error messages above for invalid values of Score-P "
                     "environment variables. A comprehensive list of variables "
                     "and valid values is available via "
                     "\'scorep-info config-vars --full\'." );
    }

    /* Timer needs environment variables */
    SCOREP_Timer_Initialize();
    SCOREP_TIME_START_TIMING( SCOREP_InitMeasurement );

    if ( SCOREP_Env_RunVerbose() )
    {
        fprintf( stderr, "[Score-P] running in verbose mode\n" );
    }

    /* == Initialize core components == */

    /* Build system tree before initialize the status, as the latter needs the
     * node ID, which may be generated by the system tree */
    system_tree_path = SCOREP_BuildSystemTree();

    /*
     * @dependsOn SystemTree for node IDs
     */
    SCOREP_TIME( SCOREP_Status_Initialize, ( ) );

    SCOREP_TIME( SCOREP_Memory_Initialize,
                 ( SCOREP_Env_GetTotalMemory(), SCOREP_Env_GetPageSize() ) );

    SCOREP_TIME( SCOREP_Paradigms_Initialize, ( ) );

    /* == Initialize definitions and create some core specific definitions == */

    /*
     * @dependsOn Memory
     */
    SCOREP_TIME( SCOREP_Definitions_Initialize, ( ) );

    /*
     * @dependsOn Definitions
     */
    SCOREP_TIME( scorep_properties_initialize, ( ) );

    /*
     * Define system tree out of the path and get location group handle
     *
     * @dependsOn SystemTree
     */
    location_group_handle = SCOREP_DefineSystemTree( system_tree_path );

    /* Data structure containing path in system tree is not needed any longer */
    SCOREP_FreeSystemTree( system_tree_path );
    system_tree_path = NULL;

    /*
     * Define artificial regions.
     *
     * @dependsOn Definitions
     */
    scorep_define_measurement_regions();

    /*
     * Define attributes.
     *
     * @dependsOn Definitions
     */
    scorep_define_measurement_attributes();

    /* == Initialize substrates and subsystems == */

    /* Let the filtering service read its filter file early */
    SCOREP_TIME( SCOREP_Filter_Initialize, ( ) );

    /*
     * @dependsOn Mutex
     */
    SCOREP_TIME( SCOREP_Location_Initialize, ( ) );

    /* == initialize threading system and create the master thread == */

    /*
     * @dependsOn Mutex
     * @dependsOn Memory
     * @dependsOn Status
     * @dependsOn Location
     * @dependsOn Definitions
     * @dependsOn Profiling
     * @dependsOn Tracing
     */
    SCOREP_TIME( SCOREP_Thread_Initialize, ( ) );

    /*
     * @dependsOn Filter
     * @dependsOn Thread (PAPI needs current location, but does not
     *            need to be activated yet)
     */
    SCOREP_TIME( scorep_subsystems_initialize, ( ) );

    /* Register finalization handler, also called in SCOREP_InitMppMeasurement() and
     * SCOREP_FinalizeMppMeasurement(). We need to make sure that our handler is
     * called before the MPI one. */
    SCOREP_RegisterExitHandler();

    /*
     * @dependsOn Metric
     * @dependsOn Epoch
     */
    SCOREP_TIME( SCOREP_Location_ActivateInitLocations, ( ) );

    /* == begin epoch, events are only allowed to happen inside the epoch == */

    SCOREP_TIME_STOP_TIMING( SCOREP_InitMeasurement );
    SCOREP_TIME_START_TIMING( MeasurementDuration );
    SCOREP_TIME( SCOREP_BeginEpoch, ( ) );

    scorep_default_recoding_mode_changes_allowed = false;
    if ( !scorep_enable_recording_by_default )
    {
        /*
         * @dependsOn Epoch
         * @dependsOn Thread_ActiveMaster
         */
        SCOREP_DisableRecording();
    }

    /*
     * Notify all interesting subsystems that the party started.
     */
    scorep_subsystems_begin();

    /*
     * Now it is time to fully activate the master thread and allow events from
     * all subsystems. parent and fork-seq needs only be provided for the MGMT call.
     */
    scorep_subsystems_activate_cpu_location( SCOREP_Location_GetCurrentCPULocation(),
                                             NULL, 0,
                                             SCOREP_CPU_LOCATION_PHASE_EVENTS );
    /*
     * And now we allow also events from outside the measurement system.
     */
    scorep_measurement_phase = SCOREP_MEASUREMENT_PHASE_WITHIN;

    if ( !SCOREP_Status_IsMpp() )
    {
        SCOREP_TIME_START_TIMING( SCOREP_InitMppMeasurement );

        scorep_init_mpp( SCOREP_SYNCHRONIZATION_MODE_BEGIN );

        SCOREP_TIME_STOP_TIMING( SCOREP_InitMppMeasurement );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}


SCOREP_LocationGroupHandle
SCOREP_GetLocationGroup( void )
{
    return location_group_handle;
}


void
SCOREP_SetAbortFlag( void )
{
    scorep_application_aborted = true;
}


void
scorep_initialization_sanity_checks( void )
{
    if ( scorep_finalized )
    {
        _Exit( EXIT_FAILURE );
    }
}


/**
 * Finalize the measurement system.
 */
void
SCOREP_FinalizeMeasurement( void )
{
    UTILS_DEBUG_ENTRY();

    scorep_finalize();
}


/**
 * Special initialization of the measurement system when using MPI.
 */
void
SCOREP_InitMppMeasurement( void )
{
    UTILS_DEBUG_ENTRY();

    SCOREP_TIME_START_TIMING( SCOREP_InitMppMeasurement );

    if ( SCOREP_Thread_InParallel() )
    {
        UTILS_ERROR( SCOREP_ERROR_INTEGRITY_FAULT, "Can't initialize measurement core from within parallel region." );
        _Exit( EXIT_FAILURE );
    }

    if ( SCOREP_Status_HasOtf2Flushed() )
    {
        fprintf( stderr, "ERROR: Switching to MPI mode after the first flush.\n" );
        fprintf( stderr, "       Consider to increase buffer size to prevent this.\n" );
        _Exit( EXIT_FAILURE );
    }

    scorep_init_mpp( SCOREP_SYNCHRONIZATION_MODE_BEGIN_MPP );

    /* Register finalization handler, also called in SCOREP_InitMeasurement() and
     * SCOREP_FinalizeMppMeasurement(). We need to make sure that our handler is
     * called before the MPI one. */
    SCOREP_RegisterExitHandler();

    SCOREP_TIME_STOP_TIMING( SCOREP_InitMppMeasurement );
}

void
SCOREP_RegisterExitHandler( void )
{
    atexit( scorep_finalize );
}


void
SCOREP_FinalizeMppMeasurement( void )
{
    SCOREP_Status_OnMppFinalize();
}

/**
 * Sets whether recording is enabled or disabled by default at measurement start.
 * Has only effect, when set during initialization.
 */
void
SCOREP_SetDefaultRecodingMode( bool enabled )
{
    UTILS_ASSERT( scorep_default_recoding_mode_changes_allowed );
    scorep_enable_recording_by_default = enabled;
}


/**
 * Enable event recording for this process.
 */
void
SCOREP_EnableRecording( void )
{
    UTILS_DEBUG_ENTRY();

    SCOREP_Location* location      = SCOREP_Location_GetCurrentCPULocation();
    uint64_t         timestamp     = SCOREP_Timer_GetClockTicks();
    uint64_t*        metric_values = SCOREP_Metric_Read( location );

    if ( !SCOREP_Thread_InParallel() )
    {
        SCOREP_Substrates_EnableRecording();
        scorep_recording_enabled = true;

        SCOREP_CALL_SUBSTRATE( EnableRecording, ENABLE_RECORDING,
                               ( location, timestamp,
                                 scorep_record_off_region, metric_values ) );
    }
    else
    {
        UTILS_ERROR( SCOREP_ERROR_SWITCH_IN_PARALLEL,
                     "Invalid request for enabling recording. "
                     "Recording is not enabled" );
        return;
    }
}


/**
 * Disable event recording for this process.
 */
void
SCOREP_DisableRecording( void )
{
    UTILS_DEBUG_ENTRY();

    SCOREP_Location* location      = SCOREP_Location_GetCurrentCPULocation();
    uint64_t         timestamp     = SCOREP_Timer_GetClockTicks();
    uint64_t*        metric_values = SCOREP_Metric_Read( location );

    if ( !SCOREP_Thread_InParallel() )
    {
        SCOREP_CALL_SUBSTRATE( DisableRecording, DISABLE_RECORDING,
                               ( location, timestamp,
                                 scorep_record_off_region, metric_values ) );

        SCOREP_Substrates_DisableRecording();
        scorep_recording_enabled = false;
    }
    else
    {
        UTILS_ERROR( SCOREP_ERROR_SWITCH_IN_PARALLEL,
                     "Invalid request for disabling recording. "
                     "Recording is not disabled" );
        return;
    }
}


/**
 * Predicate indicating if the process is recording events or not.
 */
bool
SCOREP_RecordingEnabled( void )
{
    UTILS_DEBUG_ENTRY();

    return scorep_recording_enabled;
}


/**
 * Called by the tracing component before a buffer flush happens.
 */
void
SCOREP_OnTracingBufferFlushBegin( bool final )
{
    if ( SCOREP_IN_SIGNAL_CONTEXT() )
    {
        /* Let this be just a warning, SCOREP_Memory_HandleOutOfMemory will
         * than abort for us. */
        UTILS_WARNING( "Can't flush trace buffer when taking a sample." );
        SCOREP_Memory_HandleOutOfMemory();
    }

    if ( !SCOREP_Status_IsMppInitialized() )
    {
        UTILS_FATAL( "Trace buffer flush before MPP was initialized." );
    }

    if ( !final )
    {
        SCOREP_Location* location      = SCOREP_Location_GetCurrentCPULocation();
        uint64_t         timestamp     = SCOREP_Timer_GetClockTicks();
        uint64_t*        metric_values = SCOREP_Metric_Read( location );
        /*
         * We account the flush time of non-CPU locations (i.e., CUDA streams
         * and metric locations) to the current CPU.
         */
        SCOREP_CALL_SUBSTRATE( OnTracingBufferFlushBegin, ON_TRACING_BUFFER_FLUSH_BEGIN,
                               ( location, timestamp,
                                 scorep_buffer_flush_region, metric_values ) );
    }
}

/**
 * Called by the tracing component after a buffer flush happened.
 */
void
SCOREP_OnTracingBufferFlushEnd( uint64_t timestamp )
{
    /* remember that we have flushed the first time
     * after this point, we can't switch into multi-process mode anymore
     */
    SCOREP_Status_OnOtf2Flush();

    SCOREP_Location* location      = SCOREP_Location_GetCurrentCPULocation();
    uint64_t*        metric_values = SCOREP_Metric_Read( location );

    /*
     * We account the flush time of non-CPU locations (i.e., CUDA streams
     * and metric locations) to the current CPU.
     */
    SCOREP_CALL_SUBSTRATE( OnTracingBufferFlushEnd, ON_TRACING_BUFFER_FLUSH_END,
                           ( location, timestamp,
                             scorep_buffer_flush_region, metric_values ) );
}

void
scorep_init_mpp( SCOREP_SynchronizationMode syncMode )
{
    SCOREP_Status_OnMppInit();

    SCOREP_CreateExperimentDir();

    scorep_subsystems_initialize_mpp();

    scorep_synchronize( syncMode );
}


static void
scorep_finalize( void )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();

    UTILS_DEBUG_ENTRY();

    if ( SCOREP_IN_SIGNAL_CONTEXT() )
    {
        UTILS_FATAL( "Can't finalize measurement from the signal handler." );
    }

    if ( !scorep_initialized || scorep_finalized || scorep_application_aborted )
    {
        SCOREP_IN_MEASUREMENT_DECREMENT();
        return;
    }
    scorep_finalized = true;

    SCOREP_Location* location = SCOREP_Location_GetCurrentCPULocation();

    SCOREP_OA_Finalize();

    SCOREP_TIME( scorep_synchronize,
                 ( SCOREP_SYNCHRONIZATION_MODE_END ) );

    /*
     * This barrier should prevent entering events from "outside".
     * Writing events from within is still allowed.
     */
    scorep_measurement_phase = SCOREP_MEASUREMENT_PHASE_POST;

    /*
     * @todo Move to task subsystems deactivate_cpu_location.
     */
    SCOREP_TIME( SCOREP_Task_ExitAllRegions, ( location, SCOREP_Task_GetCurrentTask( location ) ) );

    /*
     * Now tear down the master thread.
     * First notify the subsystems that the master location will be teared down.
     * parent needs only be provided for the MGMT call.
     */
    scorep_subsystems_deactivate_cpu_location( location, NULL, SCOREP_CPU_LOCATION_PHASE_EVENTS );

    /* Last remaining at-exit user is TAU. Give him the chance to do something. */
    SCOREP_TIME( scorep_trigger_exit_callbacks, ( ) );

    /*
     * We are now leaving the measurement.
     */
    scorep_subsystems_end();

    if ( !scorep_enable_recording_by_default )
    {
        SCOREP_EnableRecording();
    }

    SCOREP_TIME( SCOREP_EndEpoch, ( ) );
    SCOREP_TIME_STOP_TIMING( MeasurementDuration );
    SCOREP_TIME_START_TIMING( scorep_finalize );

    /* Clock resolution might be calculated once. Do it at the beginning
     * of finalization. */
    SCOREP_Timer_GetClockResolution();

    /*
     * Second, call into the substrates to deactivate the master.
     */
    scorep_subsystems_deactivate_cpu_location( location, NULL, SCOREP_CPU_LOCATION_PHASE_MGMT );

    // MPICH1 creates some extra processes that are not properly SCOREP
    // initialized and don't execute normal user code. We need to prevent SCOREP
    // finalization of these processes. See otf2:ticket:154.
    if ( SCOREP_Status_IsMpp() && !SCOREP_Status_IsMppInitialized() )
    {
        UTILS_WARN_ONCE( "If you are using MPICH1, please ignore this warning. "
                         "If not, it seems that your interprocess communication "
                         "library (e.g., MPI) hasn't been initialized. Score-P "
                         "can't generate output." );
        SCOREP_IN_MEASUREMENT_DECREMENT();
        return;
    }

    SCOREP_TIME( SCOREP_Filter_Finalize, ( ) );
    SCOREP_TIME( SCOREP_Location_FinalizeDefinitions, ( ) );
    SCOREP_TIME( SCOREP_FinalizeLocationGroup, ( ) );

    SCOREP_TIME( SCOREP_Unify, ( ) );

    SCOREP_TIME( SCOREP_Substrates_WriteData, ( ) );

    SCOREP_TIME( SCOREP_Definitions_Finalize, ( ) );

    /* Calls scorep_subsystems_finalize_location for all locations */
    SCOREP_TIME( SCOREP_Location_FinalizeLocations, ( ) );

    /* finalize all subsystems */
    SCOREP_TIME( scorep_subsystems_finalize, ( ) );

    /* destroy all struct SCOREP_Location */
    SCOREP_TIME( SCOREP_Location_Finalize, ( ) );

    SCOREP_TIME( SCOREP_ConfigFini, ( ) );

    SCOREP_TIME( SCOREP_RenameExperimentDir, ( ) );   // needs MPI
    SCOREP_TIME( SCOREP_Status_Finalize, ( ) );

    SCOREP_TIME( scorep_subsystems_deregister, ( ) ); // here PMPI_Finalize is called

    SCOREP_TIME( SCOREP_Thread_Finalize, ( ) );
    SCOREP_TIME( SCOREP_Memory_Finalize, ( ) );

    SCOREP_TIME_STOP_TIMING( scorep_finalize );

    SCOREP_TIME_PRINT_TIMINGS();

    SCOREP_IN_MEASUREMENT_DECREMENT();
}


void
SCOREP_RegisterExitCallback( SCOREP_ExitCallback exitCallback )
{
    assert( scorep_n_exit_callbacks < scorep_max_exit_callbacks );
    scorep_exit_callbacks[ scorep_n_exit_callbacks ] = exitCallback;
    ++scorep_n_exit_callbacks;
}

void
scorep_trigger_exit_callbacks( void )
{
    assert( scorep_n_exit_callbacks <= scorep_max_exit_callbacks );
    // trigger in lifo order
    for ( int i = scorep_n_exit_callbacks; i-- > 0; )
    {
        ( *( scorep_exit_callbacks[ i ] ) )();
    }
}

void
scorep_define_measurement_regions( void )
{
    if ( SCOREP_IsProfilingEnabled() )
    {
        scorep_record_off_region = SCOREP_Definitions_NewRegion(
            "MEASUREMENT OFF", NULL,
            SCOREP_INVALID_SOURCE_FILE,
            SCOREP_INVALID_LINE_NO,
            SCOREP_INVALID_LINE_NO,
            SCOREP_PARADIGM_USER,
            SCOREP_REGION_ARTIFICIAL );

        scorep_buffer_flush_region = SCOREP_Definitions_NewRegion(
            "TRACE BUFFER FLUSH", NULL,
            SCOREP_INVALID_SOURCE_FILE,
            SCOREP_INVALID_LINE_NO,
            SCOREP_INVALID_LINE_NO,
            SCOREP_PARADIGM_MEASUREMENT,
            SCOREP_REGION_ARTIFICIAL );
    }
}

void
scorep_define_measurement_attributes( void )
{
    /* Resdides in SCOREP_Events.c */
    extern SCOREP_AttributeHandle scorep_source_code_location_attribute;

    scorep_source_code_location_attribute =
        SCOREP_Definitions_NewAttribute( "SOURCE_CODE_LOCATION",
                                         "Source code location",
                                         SCOREP_ATTRIBUTE_TYPE_SOURCE_CODE_LOCATION );
}

void
scorep_synchronize( SCOREP_SynchronizationMode syncMode )
{
    scorep_subsystems_synchronize( syncMode );
    SCOREP_SynchronizeClocks();
}
