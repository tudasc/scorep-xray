/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2017, 2019,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2015, 2017-2018,
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

#if HAVE( PLATFORM_MAC )
# if HAVE( MACOS_GETEXEC )
#  include <sys/param.h>
#  include <mach-o/dyld.h>
# endif
# if HAVE( HAVE_MACOS_LIBPROC )
#  include <libproc.h>
# endif
#endif

#include <UTILS_Error.h>
#define SCOREP_DEBUG_MODULE_NAME CORE
#include <UTILS_Debug.h>
#include <UTILS_CStr.h>
#include <UTILS_IO.h>

#include <SCOREP_InMeasurement.h>
#include <SCOREP_Memory.h>
#include <SCOREP_Subsystem.h>
#include <SCOREP_Definitions.h>
#include <SCOREP_Metric_Management.h>
#include <SCOREP_Config.h>
#include <SCOREP_Timer_Ticks.h>
#include <SCOREP_Timer_Utils.h>
#include <SCOREP_Events.h>
#include <SCOREP_Filtering_Management.h>
#include <scorep_unify.h>
#include <SCOREP_OA_RuntimeManagement.h>
#include <SCOREP_Substrates_Management.h>
#include <scorep_substrates_definition.h>
#include <SCOREP_ErrorCallback.h>
#include <SCOREP_Task.h>

#include "scorep_type_utils.h"
#include "scorep_subsystem_management.h"
#include "scorep_environment.h"
#include "scorep_status.h"
#include "scorep_ipc.h"
#include <SCOREP_Thread_Mgmt.h>
#include "scorep_location_management.h"
#include "scorep_runtime_management.h"
#include "scorep_system_tree.h"
#include "scorep_clock_synchronization.h"
#include "scorep_paradigms_management.h"
#include "scorep_properties_management.h"
#include "scorep_runtime_management_timings.h"
#include "scorep_libwrap_management.h"

/** @brief Measurement system initialized? */
static bool initialized = false;


/** @brief Measurement system finalized? */
static bool finalized = false;

volatile SCOREP_MeasurementPhase scorep_measurement_phase = SCOREP_MEASUREMENT_PHASE_PRE;

/** @brief Location group handle */
static SCOREP_LocationGroupHandle location_group_handle = SCOREP_INVALID_LOCATION_GROUP;

#define max_exit_callbacks 1
static SCOREP_ExitCallback exit_callbacks[ max_exit_callbacks ];
static int                 n_exit_callbacks = 0;

/* Artificial regions from Score-P */

/** @brief Region handle to collect data for when measurement is disabled. */
static SCOREP_RegionHandle record_off_region = SCOREP_INVALID_REGION;

/** @brief Region handle for the trace buffer flush region. */
static SCOREP_RegionHandle buffer_flush_region = SCOREP_INVALID_REGION;

/* Region handle for the program begin/end events. */
static SCOREP_RegionHandle program_region = SCOREP_INVALID_REGION;

/** Temporally disable trace event consumption.
 *
 * Controlled by the SCOREP_EnableRecording() and SCOREP_DisableRecording()
 * functions.
 */
static bool recording_enabled = true;

/** @brief Specifies whether recoding is enabled by default */
static bool enable_recording_by_default = true;

/** @brief Specifies whether it is allowed to modify the default
 *  recording mode. After initialization, it must not be changed.
 */
static bool default_recoding_mode_changes_allowed = true;

/**
 * Indicates whether the application initiated an abortion.
 */
static bool application_aborted = false;

/**
 * Remember the location of the main thread.
 */
static SCOREP_Location* main_thread_location;

/* *INDENT-OFF* */
/** atexit handler for finalization */
static void scorep_finalize( void );
static void initialization_sanity_checks( void );
static void trigger_exit_callbacks( void );
static void define_measurement_regions( int argc, char* argv[] );
static void define_measurement_attributes( void );
static void init_mpp( SCOREP_SynchronizationMode syncMode );
static void synchronize( SCOREP_SynchronizationMode syncMode );
static void local_cleanup( void );
/* *INDENT-ON* */

static char* executable_name;
static bool executable_name_is_file;


static void
set_executable_name( int argc, char* argv[] )
{
    if ( executable_name == NULL )
    {
#if HAVE( PLATFORM_MAC )

#if HAVE( MACOS_GETEXEC )
        if ( executable_name == NULL )
        {
            uint32_t size = MAXPATHLEN;
            do
            {
                executable_name = realloc( executable_name, size + 1 );
            }
            while ( -1 == _NSGetExecutablePath( executable_name, &size ) );
        }
#endif  /* MACOS_GETEXEC */

#if HAVE( HAVE_MACOS_LIBPROC )
        /* Use proc_pidpath (macOS and possibly other BSD variants) */
        if ( executable_name == NULL )
        {
            executable_name = malloc( PROC_PIDPATHINFO_MAXSIZE );
            if ( 0 >= proc_pidpath( getpid(), executable_name, PROC_PIDPATHINFO_MAXSIZE ) )
            {
                UTILS_WARNING( "Could could not retrieve exec via proc_pidpath" );
                free( executable_name );
                executable_name = NULL;
            }
        }
#endif /* HAVE_MACOS_LIBPROC */

#endif /* PLATFORM_MAC */

#if HAVE( POSIX_READLINK )
        if ( executable_name == NULL )
        {
            size_t bufsize = 128;
            while ( 1 )
            {
                executable_name = realloc( executable_name, ( bufsize + 1 ) * sizeof( char ) );
                ssize_t num_bytes = readlink( "/proc/self/exe", executable_name, bufsize );
                if ( num_bytes == -1 )
                {
                    UTILS_WARNING( "Could not readlink '/proc/self/exe'" );
                    break;
                }
                if ( num_bytes == bufsize )
                {
                    bufsize *= 2;
                }
                else
                {
                    executable_name[ num_bytes ] = '\0';
                    break;
                }
            }
        }
#endif  /* POSIX_READLINK */

        if ( executable_name == NULL )
        {
            UTILS_WARNING( "Could not determine executable name via '/proc/self/exe'." );
            if ( argc > 0 )
            {
                executable_name = UTILS_IO_JoinPath( 2, SCOREP_GetWorkingDirectory(), argv[ 0 ] );
                UTILS_IO_SimplifyPath( executable_name );
            }
            else
            {
                const char* env_executable = SCOREP_Env_GetExecutable();
                if ( strlen( env_executable ) != 0 )
                {
                    executable_name = UTILS_IO_JoinPath( 2, SCOREP_GetWorkingDirectory(), env_executable );
                    UTILS_IO_SimplifyPath( executable_name );
                }
                else
                {
                    UTILS_WARNING( "Could not determine executable name, argv[0] not available and SCOREP_EXECUTABLE not set." );
                    executable_name = UTILS_CStr_dup( "PROGRAM" );
                }
            }
        }

#if HAVE( POSIX_ACCESS )
        if ( access( executable_name, X_OK ) != -1 )
        {
            executable_name_is_file = true;
        }
#endif  /* POSIX_ACCESS */
    }
}


const char*
SCOREP_GetExecutableName( bool* executableNameIsFile )
{
    UTILS_BUG_ON( executable_name == NULL,
                  "SCOREP_GetExecutableName requires set_executable_name() to be called earlier." );
    *executableNameIsFile = executable_name_is_file;
    return executable_name;
}


static void
define_program_begin_end_region( int argc, char* argv[] )
{
    bool        unused;
    const char* program_name = SCOREP_GetExecutableName( &unused );
    if ( argc > 0 )
    {
        argc--;
        argv++;
    }

    /* Create program_region, therefore create by program_canonical_name by
     * concatenating program_name and all argv. */
    size_t strlength[ argc + 1 ];
    int    displacements[ argc + 1 ];
    size_t total_length;

    strlength[ 0 ]     = strlen( program_name );
    displacements[ 0 ] = 0;
    total_length       = strlength[ 0 ] + 1;

    for ( int i = 0; i < argc; i++ )
    {
        strlength[ i + 1 ]     = strlen( argv[ i ] );
        displacements[ i + 1 ] = displacements[ i ] + strlength[ i ] + 1;
        total_length          += strlength[ i + 1 ] + 1;
    }

    char program_canonical_name[ total_length ];
    memset( &program_canonical_name[ 0 ], 0, total_length );
    memcpy( &program_canonical_name[ 0 ], program_name, strlength[ 0 ] );
    program_canonical_name[ strlength[ 0 ] ] = ' ';

    for ( int i = 0; i < argc; i++ )
    {
        memcpy( &program_canonical_name[ 0 ] + displacements[ i + 1 ],
                argv[ i ],
                strlength[ i + 1 ] );
        program_canonical_name[ displacements[ i + 1 ] + strlength[ i + 1 ] ] = ' ';
    }
    program_canonical_name[ total_length - 1 ] = '\0';

    const char* program_base_name = UTILS_IO_GetWithoutPath( program_name );

    program_region = SCOREP_Definitions_NewRegion(
        program_base_name,
        program_canonical_name,
        SCOREP_INVALID_SOURCE_FILE,
        SCOREP_INVALID_LINE_NO,
        SCOREP_INVALID_LINE_NO,
        SCOREP_PARADIGM_MEASUREMENT,
        SCOREP_REGION_ARTIFICIAL );
}


SCOREP_RegionHandle
SCOREP_GetProgramRegion( void )
{
    UTILS_BUG_ON( program_region == SCOREP_INVALID_REGION );
    return program_region;
}


static void
begin_epoch( int argc, char* argv[] )
{
    SCOREP_TIME_START_TIMING( SCOREP_BeginEpoch );

    SCOREP_BeginEpoch();

    bool                unused;
    const char*         program_name = SCOREP_GetExecutableName( &unused );
    SCOREP_StringHandle program      = SCOREP_Definitions_NewString( program_name );
    if ( argc > 0 )
    {
        argc--;
        argv++;
    }

    SCOREP_StringHandle args[ argc ];
    for ( int i = 0; i < argc; i++ )
    {
        args[ i ] = SCOREP_Definitions_NewString( argv[ i ] );
    }

    main_thread_location = SCOREP_Location_GetCurrentCPULocation();
    SCOREP_CALL_SUBSTRATE( ProgramBegin, PROGRAM_BEGIN,
                           ( main_thread_location,
                             SCOREP_GetBeginEpoch(),
                             program,
                             argc,
                             args,
                             program_region ) );
    SCOREP_TIME_STOP_TIMING( SCOREP_BeginEpoch );
}

static void
end_epoch( SCOREP_ExitStatus exitStatus )
{
    SCOREP_TIME_START_TIMING( SCOREP_EndEpoch );
    SCOREP_EndEpoch();

    /* We might execute this function on a thread different from the program's
     * main thread (e.g. main thread called pthread_exit). As PROGRAM_BEGIN and
     * PROGRAM_END conceptually belong together, we trigger the PROGRAM_END
     * event on the same location where we triggered PROGRAM_BEGIN. Thus proper
     * 'nesting' of BEGIN and END is given.
     * Before we trigger the 'last' event on main thread's location, we need to
     * make sure that all regions are exited. */
    uint64_t end_epoch_timestamp = SCOREP_GetEndEpoch();
    SCOREP_Location_Task_ExitAllRegions( main_thread_location,
                                         SCOREP_Task_GetCurrentTask( main_thread_location ),
                                         end_epoch_timestamp );

    SCOREP_CALL_SUBSTRATE( ProgramEnd, PROGRAM_END,
                           ( main_thread_location,
                             end_epoch_timestamp,
                             exitStatus,
                             program_region ) );
    SCOREP_TIME_STOP_TIMING( SCOREP_EndEpoch );
}

/**
 * Return true if SCOREP_InitMeasurement() has been executed.
 */
bool
SCOREP_IsInitialized( void )
{
    UTILS_DEBUG_ENTRY();

    return initialized && !finalized;
}

/**
 * Initialize the measurement system from the subsystem layer.
 */
void
SCOREP_InitMeasurement( void )
{
    SCOREP_InitMeasurementWithArgs( 0, NULL );
}

/**
 * Initialize the measurement system from the subsystem layer.
 */
void
SCOREP_InitMeasurementWithArgs( int argc, char* argv[] )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();

    SCOREP_InitErrorCallback();

    if ( SCOREP_IN_SIGNAL_CONTEXT() )
    {
        UTILS_FATAL( "Cannnot initialize measurement from the signal handler." );
    }

    if ( initialized )
    {
        SCOREP_IN_MEASUREMENT_DECREMENT();
        return;
    }

    // even if we are not ready with the initialization we must prevent recursive
    // calls e.g. during the subsystem initialization.
    initialized = true;
    initialization_sanity_checks();

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

    /*
     * @dependsOn Environment variables
     */
    set_executable_name( argc, argv );

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
    SCOREP_Platform_SystemTreePathElement* system_tree_path = SCOREP_BuildSystemTree();

    /*
     * @dependsOn SystemTree for node IDs
     */
    SCOREP_TIME( SCOREP_Status_Initialize, ( ) );

    SCOREP_TIME( SCOREP_Memory_Initialize,
                 ( SCOREP_Env_GetTotalMemory(), SCOREP_Env_GetPageSize() ) );


    SCOREP_TIME( SCOREP_Paradigms_Initialize, ( ) );

    /*
     * Initialize external Substrates and register substrate callbacks
     * @dependsOn Environment Variables
     */
    SCOREP_TIME( SCOREP_Substrates_EarlyInitialize, ( ) );

    /* == Initialize definitions and create some core specific definitions == */

    /*
     * @dependsOn Memory
     */
    SCOREP_TIME( SCOREP_Definitions_Initialize, ( ) );

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
    define_measurement_regions( argc, argv );

    /*
     * Define attributes.
     *
     * @dependsOn Definitions
     */
    define_measurement_attributes();

    /* == Initialize substrates and subsystems == */
    /* Let the filtering service read its filter file early */
    SCOREP_TIME( SCOREP_Filtering_Initialize, ( ) );

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
     * @dependsOn Mutex
     * @dependsOn Definitions
     */
    SCOREP_TIME( SCOREP_Libwrap_Initialize, ( ) );

    /*
     * @dependsOn Filter
     * @dependsOn Thread (PAPI needs current location, but does not
     *            need to be activated yet)
     * @dependsOn Libwrap
     * @dependsOn measurement_regions
     */
    SCOREP_TIME( scorep_subsystems_initialize, ( ) );

    /*
     * @dependsOn Metric
     * @dependsOn Epoch
     */
    SCOREP_TIME( SCOREP_Location_ActivateInitLocations, ( ) );

    /* == begin epoch, events are only allowed to happen inside the epoch == */

    SCOREP_TIME_STOP_TIMING( SCOREP_InitMeasurement );
    SCOREP_TIME_START_TIMING( MeasurementDuration );
    begin_epoch( argc, argv );

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

    default_recoding_mode_changes_allowed = false;
    if ( !enable_recording_by_default )
    {
        /*
         * @dependsOn Epoch
         * @dependsOn Thread_ActiveMaster
         */
        SCOREP_DisableRecording();
    }

    /*
     * And now we allow also events from outside the measurement system.
     */
    scorep_measurement_phase = SCOREP_MEASUREMENT_PHASE_WITHIN;

    if ( !SCOREP_Status_IsMpp() )
    {
        /*
         * Register finalization handler, also called in SCOREP_InitMppMeasurement() and
         * SCOREP_FinalizeMppMeasurement(). We need to make sure that our handler is
         * called before the MPI one.
         *
         * Register the exit handler here only for non-mpp programs.
         * MPP programs register their exit handlers in SCOREP_InitMppMeasurement().
         * This prevents the exit handler registration for daemon process
         * which may forked between SCOREP_InitMeasurement and SCOREP_InitMppMeasurement.
         * Only the process which initializes the MPP will be the Score-P process.
         */
        SCOREP_RegisterExitHandler();

        SCOREP_TIME_START_TIMING( SCOREP_InitMppMeasurement );

        init_mpp( SCOREP_SYNCHRONIZATION_MODE_BEGIN );

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
    application_aborted = true;
}


void
initialization_sanity_checks( void )
{
    if ( finalized )
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

    if ( SCOREP_Status_HasOtf2Flushed() )
    {
        fprintf( stderr, "ERROR: Switching to MPI mode after the first flush.\n" );
        fprintf( stderr, "       Consider to increase buffer size to prevent this.\n" );
        _Exit( EXIT_FAILURE );
    }

    init_mpp( SCOREP_SYNCHRONIZATION_MODE_BEGIN_MPP );

    /* Register finalization handler, also called in SCOREP_InitMeasurement() and
     * SCOREP_FinalizeMppMeasurement(). We need to make sure that our handler is
     * called before the MPI one. */
    SCOREP_RegisterExitHandler();

    SCOREP_TIME_STOP_TIMING( SCOREP_InitMppMeasurement );

    SCOREP_Ipc_Barrier();
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
SCOREP_SetDefaultRecordingMode( bool enabled )
{
    UTILS_ASSERT( default_recoding_mode_changes_allowed );
    enable_recording_by_default = enabled;
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
        recording_enabled = true;

        SCOREP_CALL_SUBSTRATE( EnableRecording, ENABLE_RECORDING,
                               ( location, timestamp,
                                 record_off_region, metric_values ) );
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
                                 record_off_region, metric_values ) );

        SCOREP_Substrates_DisableRecording();
        recording_enabled = false;
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

    return recording_enabled;
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

    UTILS_BUG_ON( !SCOREP_Status_IsMppInitialized(),
                  "Trace buffer flush before MPP was initialized." );

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
                                 buffer_flush_region, metric_values ) );
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
                             buffer_flush_region, metric_values ) );
}

void
init_mpp( SCOREP_SynchronizationMode syncMode )
{
    SCOREP_Status_OnMppInit();

    SCOREP_CreateExperimentDir();

    scorep_subsystems_initialize_mpp();

    synchronize( syncMode );
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

    if ( !initialized || finalized || application_aborted )
    {
        SCOREP_IN_MEASUREMENT_DECREMENT();
        return;
    }
    finalized = true;

    SCOREP_Location* location = SCOREP_Location_GetCurrentCPULocation();

    SCOREP_OA_Finalize();

    SCOREP_TIME( synchronize, ( SCOREP_SYNCHRONIZATION_MODE_END ) );

    /*
     * This barrier should prevent entering events from "outside".
     * Writing events from within is still allowed.
     */
    scorep_measurement_phase = SCOREP_MEASUREMENT_PHASE_POST;

    /*
     * @todo Move to task subsystems deactivate_cpu_location.
     */
    SCOREP_TIME( SCOREP_Task_ExitAllRegions, ( location, SCOREP_Task_GetCurrentTask( location ) ) );

    /* Last remaining at-exit user is TAU. Give him the chance to do something. */
    SCOREP_TIME( trigger_exit_callbacks, ( ) );

    if ( !enable_recording_by_default )
    {
        SCOREP_EnableRecording();
    }

    /*
     * Now tear down the master thread.
     * First notify the subsystems that the master location will be teared down.
     * parent needs only be provided for the MGMT call.
     */
    scorep_subsystems_deactivate_cpu_location( location, NULL, SCOREP_CPU_LOCATION_PHASE_EVENTS );

    /*
     * We are now leaving the measurement.
     */
    scorep_subsystems_end();

    /* @todo pass intercepted exit status */
    end_epoch( SCOREP_INVALID_EXIT_STATUS );
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
                         "cannot generate output." );
        SCOREP_IN_MEASUREMENT_DECREMENT();
        return;
    }

    SCOREP_TIME( SCOREP_Libwrap_Finalize, ( ) );

    SCOREP_TIME( SCOREP_Filtering_Finalize, ( ) );
    SCOREP_TIME( SCOREP_Location_FinalizeDefinitions, ( ) );
    SCOREP_TIME( SCOREP_FinalizeLocationGroup, ( ) );

    SCOREP_Memory_DumpStats( "[Score-P] ========= Just before unification ============ " );
    SCOREP_TIME( SCOREP_Unify, ( ) );
    SCOREP_Memory_DumpStats( "[Score-P] ========= Just after unification ============= " );

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

    local_cleanup();

    SCOREP_TIME_STOP_TIMING( scorep_finalize );

    SCOREP_TIME_PRINT_TIMINGS();

    SCOREP_IN_MEASUREMENT_DECREMENT();
}


static void
local_cleanup( void )
{
    free( executable_name );
}


void
SCOREP_RegisterExitCallback( SCOREP_ExitCallback exitCallback )
{
    assert( n_exit_callbacks < max_exit_callbacks );
    exit_callbacks[ n_exit_callbacks ] = exitCallback;
    ++n_exit_callbacks;
}

void
trigger_exit_callbacks( void )
{
    assert( n_exit_callbacks <= max_exit_callbacks );
    // trigger in lifo order
    for ( int i = n_exit_callbacks; i-- > 0; )
    {
        ( *( exit_callbacks[ i ] ) )();
    }
}

void
define_measurement_regions( int argc, char* argv[] )
{
    record_off_region = SCOREP_Definitions_NewRegion(
        "MEASUREMENT OFF", NULL,
        SCOREP_INVALID_SOURCE_FILE,
        SCOREP_INVALID_LINE_NO,
        SCOREP_INVALID_LINE_NO,
        SCOREP_PARADIGM_USER,
        SCOREP_REGION_ARTIFICIAL );

    buffer_flush_region = SCOREP_Definitions_NewRegion(
        "TRACE BUFFER FLUSH", NULL,
        SCOREP_INVALID_SOURCE_FILE,
        SCOREP_INVALID_LINE_NO,
        SCOREP_INVALID_LINE_NO,
        SCOREP_PARADIGM_MEASUREMENT,
        SCOREP_REGION_ARTIFICIAL );

    define_program_begin_end_region( argc, argv );
}

void
define_measurement_attributes( void )
{
    /* Resdides in SCOREP_Events.c */
    extern SCOREP_AttributeHandle scorep_source_code_location_attribute;

    scorep_source_code_location_attribute =
        SCOREP_Definitions_NewAttribute( "SOURCE_CODE_LOCATION",
                                         "Source code location",
                                         SCOREP_ATTRIBUTE_TYPE_SOURCE_CODE_LOCATION );
}

static void
synchronize( SCOREP_SynchronizationMode syncMode )
{
    scorep_subsystems_synchronize( syncMode );
    SCOREP_SynchronizeClocks();
}
