/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
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
 *          adapter layer.
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

#include <scorep_utility/SCOREP_Error.h>
#include <scorep_utility/SCOREP_Debug.h>
#include <SCOREP_Memory.h>
#include <SCOREP_Adapter.h>
#include <SCOREP_Config.h>
#include <SCOREP_Timing.h>
#include <scorep_utility/SCOREP_Omp.h>
#include <SCOREP_Profile.h>
#include <SCOREP_Tracing.h>
#include <SCOREP_Filter.h>
#include <scorep_unify.h>
#include <scorep_openmp.h>

#include "scorep_types.h"
#include "scorep_adapter.h"
#include "scorep_definitions.h"
#include "scorep_environment.h"
#include "scorep_status.h"
#include "scorep_mpi.h"
#include "scorep_thread.h"
#include "scorep_runtime_management.h"
#include "scorep_system_tree.h"

#include <otf2/otf2.h>


/** @brief Measurement system initialized? */
static bool scorep_initialized;


/** @brief Measurement system finalized? */
static bool scorep_finalized;

#define scorep_max_exit_callbacks 8
static SCOREP_ExitCallback scorep_exit_callbacks[ scorep_max_exit_callbacks ];
static int                 scorep_n_exit_callbacks = 0;

bool                       scorep_recording_enabled = true;


/* *INDENT-OFF* */
/** atexit handler for finalization */
static void scorep_finalize( void );
static void scorep_otf2_initialize();
static void scorep_otf2_finalize();
static void scorep_set_otf2_archive_master_slave();
static void scorep_adapters_register();
static void scorep_adapters_deregister();
static void scorep_adapters_initialize();
static void scorep_adapters_finalize();
static void scorep_adapters_initialize_location(); // needed?
static void scorep_adapters_finalize_location(); // needed?
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
 * Initialize the measurement system from the adapter layer.
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
    // calls e.g. during the adapter initialization.
    scorep_initialized = true;
    scorep_initialization_sanity_checks();

    /* initialize the config system */
    SCOREP_ConfigInit();

    /* Register all config variables */
    SCOREP_Env_RegisterCoreEnvironmentVariables();
    SCOREP_Filter_Register();
    scorep_adapters_register();
    SCOREP_Profile_Register();

    /* Parse the environment */
    SCOREP_ConfigApplyEnv();

    if ( SCOREP_Env_RunVerbose() )
    {
        fprintf( stderr, "SCOREP running in verbose mode\n" );
    }

    SCOREP_Status_Initialize();
    SCOREP_Timer_Initialize();
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

    scorep_profile_initialize();

    if ( !SCOREP_Mpi_HasMpi() )
    {
        scorep_set_otf2_archive_master_slave();
    }

    SCOREP_Filter_Initialize();

    scorep_adapters_initialize();
    scorep_adapters_initialize_location(); // not sure if this should be triggered by thread management

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

    if ( omp_in_parallel() )
    {
        SCOREP_ERROR( SCOREP_ERROR_INTEGRITY_FAULT, "Can't initialize measurement core from within parallel region." );
        _Exit( EXIT_FAILURE );
    }
}


void
scorep_otf2_initialize()
{
    if ( !SCOREP_IsTracingEnabled() )
    {
        return;
    }
    SCOREP_Tracing_RegisterSionCallbacks();
    scorep_otf2_archive = OTF2_Archive_New( SCOREP_GetExperimentDirName(),
                                            "traces",
                                            OTF2_FILEMODE_WRITE,
                                            SCOREP_Tracing_GetChunkSize(),
                                            SCOREP_Tracing_GetFileSubstrate(),
                                            0,           // allocate
                                            0,           // free
                                            0            // allocaterData
                                            );
    assert( scorep_otf2_archive );
}


void
scorep_profile_initialize()
{
    if ( !SCOREP_IsProfilingEnabled() )
    {
        return;
    }

    SCOREP_Profile_Initialize( 0, NULL );
    SCOREP_Profile_OnLocationCreation( SCOREP_Thread_GetLocationData(), NULL ); // called also from scorep_thread_call_externals_on_new_location
    SCOREP_Profile_OnThreadActivation( SCOREP_Thread_GetLocationData(), NULL ); // called also from scorep_thread_call_externals_on_thread_activation
}


void
scorep_set_otf2_archive_master_slave()
{
    if ( !SCOREP_IsTracingEnabled() )
    {
        return;
    }

    // call this function only once
    static bool archive_master_slave_already_set = false;
    assert( !archive_master_slave_already_set );
    archive_master_slave_already_set = true;

    SCOREP_Error_Code error;
    if ( SCOREP_Mpi_GetRank() == 0 )
    {
        error = OTF2_Archive_SetMasterSlaveMode( scorep_otf2_archive, OTF2_MASTER );
    }
    else
    {
        error = OTF2_Archive_SetMasterSlaveMode( scorep_otf2_archive, OTF2_SLAVE );
    }
    if ( SCOREP_SUCCESS != error )
    {
        _Exit( EXIT_FAILURE );
    }
}


void
scorep_adapters_register()
{
    SCOREP_Error_Code error;
    /* call register functions for all adapters */
    for ( size_t i = 0; i < scorep_number_of_adapters; i++ )
    {
        if ( scorep_adapters[ i ]->adapter_register )
        {
            error = scorep_adapters[ i ]->adapter_register();
        }

        if ( SCOREP_SUCCESS != error )
        {
            SCOREP_ERROR( error, "Can't register %s adapter",
                          scorep_adapters[ i ]->adapter_name );
            _Exit( EXIT_FAILURE );
        }
    }
}


void
scorep_adapters_initialize()
{
    SCOREP_Error_Code error;
    /* call initialization functions for all adapters */
    for ( size_t i = 0; i < scorep_number_of_adapters; i++ )
    {
        if ( scorep_adapters[ i ]->adapter_init )
        {
            error = scorep_adapters[ i ]->adapter_init();
        }

        if ( SCOREP_SUCCESS != error )
        {
            SCOREP_ERROR( error, "Can't initialize %s adapter",
                          scorep_adapters[ i ]->adapter_name );
            _Exit( EXIT_FAILURE );
        }
        else if ( SCOREP_Env_RunVerbose() )
        {
            fprintf( stderr, "SCOREP successfully initialized %s adapter\n",
                     scorep_adapters[ i ]->adapter_name );
        }
    }
}


static void
scorep_adapters_initialize_location()
{
    SCOREP_Error_Code error;
    /* create location */

    /* call initialization functions for all adapters */
    for ( size_t i = 0; i < scorep_number_of_adapters; i++ )
    {
        if ( scorep_adapters[ i ]->adapter_init_location )
        {
            error = scorep_adapters[ i ]->adapter_init_location();
        }

        if ( SCOREP_SUCCESS != error )
        {
            SCOREP_ERROR( error, "Can't initialize location for %s adapter",
                          scorep_adapters[ i ]->adapter_name );
            _Exit( EXIT_FAILURE );
        }
        else if ( SCOREP_Env_RunVerbose() )
        {
            fprintf( stderr, "SCOREP successfully initialized location for %s adapter\n",
                     scorep_adapters[ i ]->adapter_name );
        }
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

    if ( omp_in_parallel() )
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
    SCOREP_ProcessDeferredLocations();
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
    SCOREP_Thread_LocationData* location = SCOREP_Thread_GetLocationData();

    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_FUNCTION_ENTRY, "" );
    if ( !SCOREP_Omp_InParallel() )
    {
        scorep_recording_enabled = true;
        if ( SCOREP_IsTracingEnabled() )
        {
            OTF2_EvtWriter_MeasurementOnOff( SCOREP_Thread_GetTraceLocationData( location )->otf_writer,
                                             NULL,
                                             SCOREP_GetClockTicks(),
                                             OTF2_MEASUREMENT_ON );
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
    SCOREP_Thread_LocationData* location = SCOREP_Thread_GetLocationData();

    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_FUNCTION_ENTRY, "" );
    if ( !SCOREP_Omp_InParallel() )
    {
        scorep_recording_enabled = false;
        if ( SCOREP_IsTracingEnabled() )
        {
            OTF2_EvtWriter_MeasurementOnOff( SCOREP_Thread_GetTraceLocationData( location )->otf_writer,
                                             NULL,
                                             SCOREP_GetClockTicks(),
                                             OTF2_MEASUREMENT_OFF );
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


static void
scorep_finalize( void )
{
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_FUNCTION_ENTRY, "" );

    if ( !scorep_initialized || scorep_finalized )
    {
        return;
    }
    scorep_finalized = true;

    scorep_trigger_exit_callbacks();

    // MPICH1 creates some extra processes that are not properly SCOREP
    // initialized and don't execute normal user code. We need to prevent SCOREP
    // finalization of these processes. See otf2:ticket:154.
    if ( SCOREP_Mpi_HasMpi() && !SCOREP_Mpi_IsInitialized() )
    {
        return;
    }

    // order is important
    if ( SCOREP_IsProfilingEnabled() )
    {
        SCOREP_Profile_Process( SCOREP_Profile_ProcessDefault );
    }

    // Calling SCOREP_Event.h functions after this point is considered
    // an instrumentation error.
    SCOREP_DefineSystemTree();
    SCOREP_Unify();
    scorep_profile_finalize();
    SCOREP_Definitions_Write();
    SCOREP_Definitions_Finalize();
    scorep_otf2_finalize();

    /* dump config variables into experiment directory */
    scorep_dump_config();

    SCOREP_ConfigFini();

    SCOREP_RenameExperimentDir();  // needs MPI

    scorep_adapters_finalize_location();
    scorep_adapters_finalize(); // here PMPI_Finalize is called
    scorep_adapters_deregister();

    SCOREP_Thread_Finalize();
    SCOREP_Memory_Finalize();
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

static void
scorep_adapters_finalize_location()
{
    for ( size_t i = scorep_number_of_adapters; i-- > 0; )
    {
        if ( scorep_adapters[ i ]->adapter_finalize_location )
        {
            //scorep_adapters[ i ]->adapter_finalize_location(location_ptr???);
        }

        if ( SCOREP_Env_RunVerbose() )
        {
            fprintf( stderr, "SCOREP finalized %s adapter location\n",
                     scorep_adapters[ i ]->adapter_name );
        }
    }
}


static void
scorep_adapters_finalize()
{
    /* call finalization functions for all adapters */
    for ( size_t i = scorep_number_of_adapters; i-- > 0; )
    {
        if ( scorep_adapters[ i ]->adapter_finalize )
        {
            scorep_adapters[ i ]->adapter_finalize();
        }

        if ( SCOREP_Env_RunVerbose() )
        {
            fprintf( stderr, "SCOREP finalized %s adapter\n",
                     scorep_adapters[ i ]->adapter_name );
        }
    }
}


static void
scorep_adapters_deregister()
{
    /* call de-register functions for all adapters */
    for ( size_t i = scorep_number_of_adapters; i-- > 0; )
    {
        if ( scorep_adapters[ i ]->adapter_deregister )
        {
            scorep_adapters[ i ]->adapter_deregister();
        }

        if ( SCOREP_Env_RunVerbose() )
        {
            fprintf( stderr, "SCOREP de-registered %s adapter\n",
                     scorep_adapters[ i ]->adapter_name );
        }
    }
}


void
scorep_otf2_finalize()
{
    if ( SCOREP_IsTracingEnabled() )
    {
        assert( scorep_otf2_archive );
        /// @todo? set archive to "unified"/"not unified"
        OTF2_Archive_Delete( scorep_otf2_archive );
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
