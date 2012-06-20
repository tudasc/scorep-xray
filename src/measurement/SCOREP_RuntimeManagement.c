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
 * @brief   Declaration of event recording functions to be used by the
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
#include <sys/stat.h>
#include <errno.h>
#include <assert.h>
#include <string.h>
#include <inttypes.h>

#include <SCOREP_Debug.h>
#include <SCOREP_Memory.h>
#include <SCOREP_Subsystem.h>
#include <SCOREP_Metric_Management.h>
#include <SCOREP_Config.h>
#include <SCOREP_Timing.h>
#include <scorep_openmp.h>
#include <SCOREP_Profile.h>
#include <tracing/SCOREP_Tracing.h>
#include <tracing/SCOREP_Tracing_Events.h>
#include <SCOREP_Filter_Init.h>
#include <scorep_unify.h>
#include <scorep_openmp.h>
#include <SCOREP_OA_Init.h>

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

/** @brief Measurement system initialized? */
static bool scorep_initialized = false;


/** @brief Measurement system finalized? */
static bool scorep_finalized = false;

#define scorep_max_exit_callbacks 8
static SCOREP_ExitCallback scorep_exit_callbacks[ scorep_max_exit_callbacks ];
static int                 scorep_n_exit_callbacks = 0;

bool scorep_recording_enabled = true;

/* *INDENT-OFF* */
/** atexit handler for finalization */
static void scorep_finalize( void );
static void scorep_otf2_initialize();
static void scorep_otf2_finalize();
static void scorep_set_otf2_archive_master_slave();
static void scorep_initialization_sanity_checks();
static void scorep_profile_initialize();
static void scorep_profile_finalize();
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
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_FUNCTION_ENTRY, "" );

    return scorep_initialized && !scorep_finalized;
}


/**
 * Initialize the measurement system from the subsystem layer.
 */
void
SCOREP_InitMeasurement()
{
    if ( scorep_initialized )
    {
        return;
    }

    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_FUNCTION_ENTRY, "" );

    // even if we are not ready with the initialization we must prevent recursive
    // calls e.g. during the subsystem initialization.
    scorep_initialized = true;
    scorep_initialization_sanity_checks();

    /* initialize the config system */
    SCOREP_ConfigInit();

    /* Register all config variables */
    SCOREP_Env_RegisterCoreEnvironmentVariables();
    SCOREP_Filter_Register();
    scorep_subsystems_register();
    SCOREP_Profile_Register();
    SCOREP_Tracing_Register();
    SCOREP_OA_Register();

    /* Parse the environment */
    SCOREP_ConfigApplyEnv();

    if ( SCOREP_Env_RunVerbose() )
    {
        fprintf( stderr, "SCOREP running in verbose mode\n" );
    }

    SCOREP_Status_Initialize();
    SCOREP_Timer_Initialize();
    SCOREP_BeginEpoch();
    SCOREP_CreateExperimentDir();

    // Need to be called before the first use of any SCOREP_Alloc function, in
    // particular before SCOREP_Thread_Initialize
    SCOREP_Memory_Initialize( SCOREP_Env_GetTotalMemory(), SCOREP_Env_GetPageSize() );

    // initialize before SCOREP_Thread_Initialize() because latter may create a
    // writer that needs the archive.
    scorep_otf2_initialize();

    // initialize before SCOREP_Thread_Initialize() because latter creates at least a
    // location definition.
    SCOREP_Definitions_Initialize();

    SCOREP_Thread_Initialize();

    if ( !SCOREP_Mpi_HasMpi() )
    {
        scorep_set_otf2_archive_master_slave();
        SCOREP_SynchronizeClocks();
    }

    SCOREP_Filter_Initialize();

    scorep_subsystems_initialize();
    scorep_subsystems_initialize_location( SCOREP_Location_GetCurrentCPULocation() ); // not sure if this should be triggered by thread management

    scorep_profile_initialize();

    /* Register finalization handler, also called in SCOREP_InitMeasurementMPI() and
     * SCOREP_FinalizeMeasurementMPI(). We need to make sure that our handler is
     * called before the MPI one. */
    atexit( scorep_finalize );
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
        SCOREP_ERROR( SCOREP_ERROR_INTEGRITY_FAULT, "Can't initialize measurement core from within parallel region." );
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
scorep_profile_initialize()
{
    if ( !SCOREP_IsProfilingEnabled() )
    {
        return;
    }

    uint32_t             number_of_metrics = 0;
    SCOREP_MetricHandle* metrics           = NULL;

    SCOREP_SamplingSetHandle sampling_set_handle = SCOREP_Metric_GetSamplingSet();
    if ( sampling_set_handle != SCOREP_INVALID_SAMPLING_SET )
    {
        SCOREP_SamplingSet_Definition* sampling_set
                          = SCOREP_LOCAL_HANDLE_DEREF( sampling_set_handle, SamplingSet );
        number_of_metrics = sampling_set->number_of_metrics;
        metrics           = sampling_set->metric_handles;
    }
    SCOREP_Profile_Initialize( number_of_metrics, metrics );

    SCOREP_Profile_OnLocationCreation( SCOREP_Location_GetCurrentCPULocation(), NULL ); // called also from scorep_thread_call_externals_on_new_location
    SCOREP_Profile_OnThreadActivation( SCOREP_Location_GetCurrentCPULocation(), NULL ); // called also from scorep_thread_call_externals_on_thread_activation
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
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_FUNCTION_ENTRY, "" );
    SCOREP_FinalizeMeasurementMPI();
    scorep_finalize();
}


/**
 * Special initialization of the measurement system when using MPI.
 */
void
SCOREP_InitMeasurementMPI( int rank )
{
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_FUNCTION_ENTRY, "" );

    if ( SCOREP_Omp_InParallel() )
    {
        SCOREP_ERROR( SCOREP_ERROR_INTEGRITY_FAULT, "Can't initialize measurement core from within parallel region." );
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
    SCOREP_Location_ProcessDeferredOnes();
    SCOREP_SynchronizeClocks();
    scorep_set_otf2_archive_master_slave();

    /* Register finalization handler, also called in SCOREP_InitMeasurement() and
     * SCOREP_FinalizeMeasurementMPI(). We need to make sure that our handler is
     * called before the MPI one. */
    atexit( scorep_finalize );
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
    SCOREP_Location* location = SCOREP_Location_GetCurrentCPULocation();

    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_FUNCTION_ENTRY, "" );
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
        SCOREP_ERROR( SCOREP_ERROR_SWITCH_IN_PARALLEL,
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
    SCOREP_Location* location = SCOREP_Location_GetCurrentCPULocation();

    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_FUNCTION_ENTRY, "" );
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
        SCOREP_ERROR( SCOREP_ERROR_SWITCH_IN_PARALLEL,
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
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_FUNCTION_ENTRY, "" );

    return scorep_recording_enabled;
}


// entity needs to be a function with the signature void entity();
#if HAVE( SCOREP_DEBUG )
#define SCOREP_TIME( entity ) \
    uint64_t timing_start_ ## entity = SCOREP_GetClockTicks(); \
    entity(); \
    uint64_t timing_stop_ ## entity = SCOREP_GetClockTicks(); \
    double   duration_ ## entity = ( timing_stop_ ## entity - timing_start_ ## entity ) / ( double )SCOREP_GetClockResolution(); \
    printf( "SCOREP_Timing[%d]: " #entity " took %f seconds.\n", SCOREP_Mpi_GetRank(), duration_ ## entity );
#else
#define SCOREP_TIME( entity ) entity()
#endif


static void
scorep_finalize( void )
{
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_FUNCTION_ENTRY, "" );

    if ( !scorep_initialized || scorep_finalized )
    {
        return;
    }
    scorep_finalized = true;

    SCOREP_TIME( scorep_trigger_exit_callbacks );

    // MPICH1 creates some extra processes that are not properly SCOREP
    // initialized and don't execute normal user code. We need to prevent SCOREP
    // finalization of these processes. See otf2:ticket:154.
    if ( SCOREP_Mpi_HasMpi() && !SCOREP_Mpi_IsInitialized() )
    {
        return;
    }

    SCOREP_SynchronizeClocks();
    SCOREP_EndEpoch();
    SCOREP_TIME( SCOREP_Filter_Finalize );

    /* finalize and close all event writers */
    SCOREP_TIME( SCOREP_Tracing_FinalizeEventWriters );

    // Calling SCOREP_Event.h functions after this point is considered
    // an instrumentation error.
    // order is important
    if ( SCOREP_IsProfilingEnabled() )
    {
        SCOREP_Profile_Process( SCOREP_Location_GetCurrentCPULocation() );
    }

    SCOREP_TIME( SCOREP_DefineSystemTree );
    SCOREP_TIME( SCOREP_Unify );
    SCOREP_TIME( scorep_profile_finalize );
    SCOREP_TIME( SCOREP_Definitions_Write );
    SCOREP_TIME( SCOREP_Definitions_Finalize );
    SCOREP_TIME( scorep_otf2_finalize );

    SCOREP_TIME( SCOREP_Location_Finalize );

    SCOREP_TIME( scorep_subsystems_finalize );  // Disables all adapters

    /* dump config variables into experiment directory */
    SCOREP_TIME( scorep_dump_config );

    SCOREP_TIME( SCOREP_ConfigFini );

    SCOREP_TIME( SCOREP_RenameExperimentDir );   // needs MPI
    SCOREP_TIME( SCOREP_Status_Finalize );       // Cleans up some measurement core MPI data

    SCOREP_TIME( scorep_subsystems_deregister ); // here PMPI_Finalize is called

    SCOREP_TIME( SCOREP_Thread_Finalize );
    SCOREP_TIME( SCOREP_Memory_Finalize );
}


static void
scorep_profile_finalize()
{
    if ( SCOREP_IsProfilingEnabled() )
    {
        SCOREP_Profile_Write();
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
        SCOREP_ERROR( SCOREP_ERROR_FILE_CAN_NOT_OPEN,
                      "Can't write config variables into `%s'",
                      dump_file_name );

        free( dump_file_name );
        return;
    }
    free( dump_file_name );

    SCOREP_ConfigDump( dump_file );
    fclose( dump_file );
}
