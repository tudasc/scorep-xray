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
 * @status      alpha
 * @file        SCOREP_RuntimeManagement.c
 * @maintainer  Bert Wesarg <Bert.Wesarg@tu-dresden.de>
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

#include <SCOREP_Memory.h>
#include <SCOREP_Subsystem.h>
#include <SCOREP_Metric_Management.h>
#include <SCOREP_Config.h>
#include <SCOREP_Timing.h>
#include <scorep_openmp.h>
#include <SCOREP_Profile.h>
#include <SCOREP_Profile_MpiEvents.h>
#include <tracing/SCOREP_Tracing.h>
#include <tracing/SCOREP_Tracing_Events.h>
#include <SCOREP_Filter_Init.h>
#include <scorep_unify.h>
#include <scorep_openmp.h>
#include <SCOREP_OA_Init.h>

#include "scorep_error_callback.h"
#include "scorep_types.h"
#include "scorep_subsystem.h"
#include "scorep_definitions.h"
#include "scorep_environment.h"
#include "scorep_status.h"
#include "scorep_mpi.h"
#include "scorep_thread.h"
#include "scorep_runtime_management.h"
#include "scorep_system_tree.h"
#include "scorep_clock_synchronization.h"
#include "scorep_properties.h"
#include "scorep_runtime_management_timings.h"

/** @brief Measurement system initialized? */
static bool scorep_initialized = false;


/** @brief Measurement system finalized? */
static bool scorep_finalized = false;

/** @brief Location group handle */
static SCOREP_LocationGroupHandle location_group_handle = SCOREP_INVALID_LOCATION_GROUP;

/** @brief System tree path */
static SCOREP_Platform_SystemTreePathElement* system_tree_path = NULL;

#define scorep_max_exit_callbacks 8
static SCOREP_ExitCallback scorep_exit_callbacks[ scorep_max_exit_callbacks ];
static int                 scorep_n_exit_callbacks = 0;

/** Temporally disable trace event consumption.
 *
 * Controlled by the SCOREP_EnableRecording() and SCOREP_DisableRecording()
 * functions.
 */
bool scorep_recording_enabled = true;

/* *INDENT-OFF* */
/** atexit handler for finalization */
static void scorep_finalize( void );
static void scorep_otf2_initialize();
static void scorep_otf2_finalize();
static void scorep_set_otf2_archive_master_slave();
static void scorep_initialization_sanity_checks();
static void scorep_profile_initialize( SCOREP_Location* loaction );
static void scorep_profile_finalize( SCOREP_Location* loaction );
static void scorep_trigger_exit_callbacks();
static void scorep_dump_config( void );
//static void scorep_deregister_config_variables( SCOREP_ConfigVariable configVars[] ); needed?
/* *INDENT-ON* */

/**
 * Return true if SCOREP_InitMeasurement() has been executed.
 */
bool
SCOREP_IsInitialized()
{
    UTILS_DEBUG_ENTRY();

    return scorep_initialized && !scorep_finalized;
}


/**
 * Initialize the measurement system from the subsystem layer.
 */
void
SCOREP_InitMeasurement()
{
    UTILS_DEBUG_ENTRY();

    SCOREP_InitErrorCallback();

    if ( scorep_initialized )
    {
        return;
    }

    // even if we are not ready with the initialization we must prevent recursive
    // calls e.g. during the subsystem initialization.
    scorep_initialized = true;
    scorep_initialization_sanity_checks();

    SCOREP_Timer_Initialize();
    SCOREP_TIME_START_TIMING( SCOREP_InitMeasurement );

    /* initialize the config system */
    SCOREP_TIME( SCOREP_ConfigInit, ( ) );

    /* Build system tree at an early time, because it will be used by
     * metric service to determine additional metrics (e.g. per-process
     * metrcis). */
    system_tree_path = SCOREP_BuildSystemTree();

    /* Register all config variables */
    SCOREP_TIME( SCOREP_RegisterAllConfigVariables, ( ) );

    /* Parse the environment */
    SCOREP_TIME( SCOREP_ConfigApplyEnv, ( ) );

    if ( SCOREP_Env_RunVerbose() )
    {
        fprintf( stderr, "[Score-P] running in verbose mode\n" );
    }

    SCOREP_TIME( SCOREP_Status_Initialize, ( ) );
    SCOREP_TIME( SCOREP_CreateExperimentDir, ( ) );

    // Need to be called before the first use of any SCOREP_Alloc function, in
    // particular before SCOREP_Thread_Initialize
    SCOREP_TIME( SCOREP_Memory_Initialize,
                 ( SCOREP_Env_GetTotalMemory(), SCOREP_Env_GetPageSize() ) );

    // initialize before SCOREP_Thread_Initialize() because latter may create a
    // writer that needs the archive.
    SCOREP_TIME( scorep_otf2_initialize, ( ) );

    // initialize before SCOREP_Thread_Initialize() because latter creates at least a
    // location definition.
    SCOREP_TIME( SCOREP_Definitions_Initialize, ( ) );

    /* Get location group handle from system tree */
    location_group_handle = SCOREP_DefineSystemTree( system_tree_path );

    /* Data structure containing path in system tree is not needed any longer */
    SCOREP_FreeSystemTree( system_tree_path );
    system_tree_path = NULL;

    SCOREP_TIME( SCOREP_Location_Initialize, ( ) );
    SCOREP_TIME( SCOREP_Thread_Initialize, ( ) );
    SCOREP_Location* location = SCOREP_Location_GetCurrentCPULocation();

    if ( !SCOREP_Mpi_HasMpi() )
    {
        scorep_set_otf2_archive_master_slave();
    }

    SCOREP_TIME( SCOREP_Filter_Initialize, ( ) );
    SCOREP_TIME( scorep_subsystems_initialize, ( ) );
    SCOREP_TIME( scorep_subsystems_initialize_location, ( location ) );
    SCOREP_TIME( scorep_profile_initialize, ( location ) );

    SCOREP_TIME( scorep_properties_initialize, ( ) );

    /* Register finalization handler, also called in SCOREP_InitMeasurementMPI() and
     * SCOREP_FinalizeMeasurementMPI(). We need to make sure that our handler is
     * called before the MPI one. */
    atexit( scorep_finalize );

    SCOREP_TIME( SCOREP_BeginEpoch, ( ) );
    if ( !SCOREP_Mpi_HasMpi() )
    {
        SCOREP_SynchronizeClocks();
    }

    SCOREP_TIME_STOP_TIMING( SCOREP_InitMeasurement );
    SCOREP_TIME_START_TIMING( MeasurementDuration );
}


SCOREP_LocationGroupHandle
SCOREP_GetLocationGroup()
{
    return location_group_handle;
}


void
scorep_initialization_sanity_checks()
{
    if ( scorep_finalized )
    {
        _Exit( EXIT_FAILURE );
    }

    if ( SCOREP_Omp_InParallel() )
    {
        UTILS_ERROR( SCOREP_ERROR_INTEGRITY_FAULT, "Can't initialize measurement core from within parallel region." );
        _Exit( EXIT_FAILURE );
    }
}


void
scorep_otf2_initialize()
{
    if ( SCOREP_IsTracingEnabled() )
    {
        SCOREP_Tracing_Initialize();
    }
}


void
scorep_otf2_finalize()
{
    if ( SCOREP_IsTracingEnabled() )
    {
        SCOREP_Tracing_Finalize();
    }
}


void
scorep_profile_initialize( SCOREP_Location* location )
{
    if ( !SCOREP_IsProfilingEnabled() )
    {
        return;
    }

    SCOREP_Profile_Initialize();

    SCOREP_Profile_OnLocationCreation( location, NULL ); // called also from scorep_thread_call_externals_on_new_location

    SCOREP_Profile_AddLocationSpecificMetrics( location,
                                               SCOREP_Metric_GetNumberOfAdditionalScopedMetrics( location ) );

    SCOREP_Profile_OnLocationActivation( location, NULL, 0 ); // called also from scorep_thread_call_externals_on_thread_activation
}


void
scorep_set_otf2_archive_master_slave()
{
    if ( SCOREP_IsTracingEnabled() )
    {
        SCOREP_Tracing_SetIsMaster( SCOREP_Mpi_GetRank() == 0 );
    }
}


/**
 * Finalize the measurement system.
 */
void
SCOREP_FinalizeMeasurement()
{
    UTILS_DEBUG_ENTRY();

    SCOREP_FinalizeMeasurementMPI();
    scorep_finalize();
}


/**
 * Special initialization of the measurement system when using MPI.
 */
void
SCOREP_InitMeasurementMPI( int rank )
{
    UTILS_DEBUG_ENTRY();

    SCOREP_TIME_START_TIMING( SCOREP_InitMeasurementMPI );

    if ( SCOREP_Omp_InParallel() )
    {
        UTILS_ERROR( SCOREP_ERROR_INTEGRITY_FAULT, "Can't initialize measurement core from within parallel region." );
        _Exit( EXIT_FAILURE );
    }

    if ( SCOREP_Otf2_HasFlushed() )
    {
        fprintf( stderr, "ERROR: Switching to MPI mode after the first flush.\n" );
        fprintf( stderr, "       Consider to increase buffer size to prevent this.\n" );
        _Exit( EXIT_FAILURE );
    }

    SCOREP_Mpi_SetRankTo( rank );
    SCOREP_CreateExperimentDir();
    SCOREP_Location_CloseDeferredDefinitions();
    SCOREP_SynchronizeClocks();
    scorep_set_otf2_archive_master_slave();

    SCOREP_Profile_InitializeMpi();

    /* Register finalization handler, also called in SCOREP_InitMeasurement() and
     * SCOREP_FinalizeMeasurementMPI(). We need to make sure that our handler is
     * called before the MPI one. */
    atexit( scorep_finalize );

    SCOREP_TIME_STOP_TIMING( SCOREP_InitMeasurementMPI );
}

void
SCOREP_RegisterExitHandler()
{
    atexit( scorep_finalize );
}


void
SCOREP_FinalizeMeasurementMPI()
{
    /* Register finalization handler, also called in SCOREP_InitMeasurement() and
     * SCOREP_InitMeasurementMPI(). We need to make sure that our handler is
     * called before the MPI one. */
    atexit( scorep_finalize );
    /*
        // What Scalasca does here:
        // mark start of finalization
        // 2. clock synchronization if necessary
            // measure offset
            // write OFFSET record
        // mark end of finalization
        // emergency exit for cases where atexit not handled
            // close on finalize
     */
}


/**
 * Enable event recording for this process.
 */
void
SCOREP_EnableRecording()
{
    UTILS_DEBUG_ENTRY();

    SCOREP_Location* location = SCOREP_Location_GetCurrentCPULocation();

    if ( !SCOREP_Omp_InParallel() )
    {
        scorep_recording_enabled = true;
        if ( SCOREP_IsTracingEnabled() )
        {
            SCOREP_Tracing_MeasurementOnOff( location,
                                             SCOREP_GetClockTicks(),
                                             true );
        }
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
SCOREP_DisableRecording()
{
    UTILS_DEBUG_ENTRY();

    SCOREP_Location* location = SCOREP_Location_GetCurrentCPULocation();

    if ( !SCOREP_Omp_InParallel() )
    {
        scorep_recording_enabled = false;
        if ( SCOREP_IsTracingEnabled() )
        {
            SCOREP_Tracing_MeasurementOnOff( location,
                                             SCOREP_GetClockTicks(),
                                             false );
        }
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
SCOREP_RecordingEnabled()
{
    UTILS_DEBUG_ENTRY();

    return scorep_recording_enabled;
}

static void
scorep_finalize( void )
{
    UTILS_DEBUG_ENTRY();

    if ( !scorep_initialized || scorep_finalized )
    {
        return;
    }
    scorep_finalized = true;
    SCOREP_Location* location = SCOREP_Location_GetCurrentCPULocation();
    SCOREP_OA_Finalize();

    SCOREP_TIME_STOP_TIMING( MeasurementDuration );
    SCOREP_TIME_START_TIMING( scorep_finalize );

    SCOREP_TIME( scorep_trigger_exit_callbacks, ( ) );

    // MPICH1 creates some extra processes that are not properly SCOREP
    // initialized and don't execute normal user code. We need to prevent SCOREP
    // finalization of these processes. See otf2:ticket:154.
    if ( SCOREP_Mpi_HasMpi() && !SCOREP_Mpi_IsInitialized() )
    {
        return;
    }

    SCOREP_TIME( SCOREP_SynchronizeClocks, ( ) );
    SCOREP_TIME( SCOREP_EndEpoch, ( ) );
    SCOREP_TIME( SCOREP_Filter_Finalize, ( ) );

    /* finalize and close all event writers */
    SCOREP_TIME( SCOREP_Tracing_FinalizeEventWriters, ( ) );

    // Calling SCOREP_Event.h functions after this point is considered
    // an instrumentation error.
    // order is important
    if ( SCOREP_IsProfilingEnabled() )
    {
        SCOREP_TIME( SCOREP_Profile_Process, ( location ) );
    }

    SCOREP_TIME( SCOREP_FinalizeLocationGroup, ( ) );

    SCOREP_TIME( SCOREP_Unify, ( ) );

    SCOREP_TIME( scorep_properties_write, ( ) );

    SCOREP_TIME( scorep_profile_finalize, ( location ) );
    SCOREP_TIME( SCOREP_Definitions_Write, ( ) );
    SCOREP_TIME( SCOREP_Definitions_Finalize, ( ) );
    SCOREP_TIME( scorep_otf2_finalize, ( ) );

    SCOREP_TIME( SCOREP_Location_Finalize, ( ) );

    SCOREP_TIME( scorep_subsystems_finalize, ( ) );  // Disables all adapters

    /* dump config variables into experiment directory */
    SCOREP_TIME( scorep_dump_config, ( ) );

    SCOREP_TIME( SCOREP_ConfigFini, ( ) );

    SCOREP_TIME( SCOREP_RenameExperimentDir, ( ) );   // needs MPI
    SCOREP_TIME( SCOREP_Status_Finalize, ( ) );       // Cleans up some measurement core MPI data

    SCOREP_TIME( scorep_subsystems_deregister, ( ) ); // here PMPI_Finalize is called

    SCOREP_TIME( SCOREP_Thread_Finalize, ( ) );
    SCOREP_TIME( SCOREP_Memory_Finalize, ( ) );

    SCOREP_TIME_STOP_TIMING( scorep_finalize );

    SCOREP_TIME_PRINT_TIMINGS();
}


static void
scorep_profile_finalize( SCOREP_Location* location )
{
    if ( SCOREP_IsProfilingEnabled() )
    {
        SCOREP_Profile_Write( location );
        SCOREP_Profile_Finalize();
    }
}

void
SCOREP_RegisterExitCallback( SCOREP_ExitCallback exitCallback )
{
    assert( scorep_n_exit_callbacks < scorep_max_exit_callbacks );
    scorep_exit_callbacks[ scorep_n_exit_callbacks ] = exitCallback;
    ++scorep_n_exit_callbacks;
}

void
scorep_trigger_exit_callbacks()
{
    assert( scorep_n_exit_callbacks < scorep_max_exit_callbacks );
    // trigger in lifo order
    for ( int i = scorep_n_exit_callbacks - 1; i >= 0; --i )
    {
        ( *( scorep_exit_callbacks[ i ] ) )();
    }
}

static void
scorep_dump_config( void )
{
    const char* experiment_dir = SCOREP_GetExperimentDirName();
    char*       dump_file_name;

    if ( SCOREP_Mpi_HasMpi() && SCOREP_Mpi_GetRank() != 0 )
    {
        return;
    }

    dump_file_name = calloc( 1, strlen( experiment_dir ) + strlen( "/scorep.cfg" ) + 1 );
    sprintf( dump_file_name, "%s%s", experiment_dir, "/scorep.cfg" );

    FILE* dump_file = fopen( dump_file_name, "w" );
    if ( !dump_file )
    {
        UTILS_ERROR( SCOREP_ERROR_FILE_CAN_NOT_OPEN,
                     "Can't write config variables into `%s'",
                     dump_file_name );

        free( dump_file_name );
        return;
    }
    free( dump_file_name );

    SCOREP_ConfigDump( dump_file );
    fclose( dump_file );
}
