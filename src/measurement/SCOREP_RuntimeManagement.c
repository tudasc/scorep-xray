/*
 * This file is part of the SCOREP project (http://www.scorep.de)
 *
 * Copyright (c) 2009-2011,
 *    RWTH Aachen, Germany
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *    Technische Universitaet Dresden, Germany
 *    University of Oregon, Eugene USA
 *    Forschungszentrum Juelich GmbH, Germany
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
#include <scorep_unify.h>


#include "scorep_types.h"
#include "scorep_adapter.h"
#include "scorep_definitions.h"
#include "scorep_environment.h"
#include "scorep_status.h"
#include "scorep_mpi.h"
#include "scorep_thread.h"
#include "scorep_runtime_management.h"
#include "scorep_definition_locking.h"
#include "scorep_parameter_registration.h"

#include <otf2/otf2.h>


/** @brief Measurement system initialized? */
static bool scorep_initialized;


/** @brief Measurement system finalized? */
static bool scorep_finalized;


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
static void scorep_register_config_variables( SCOREP_ConfigVariable configVars[] );
static void scorep_profile_initialize();
static void scorep_profile_finalize();
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
SCOREP_InitMeasurement( void )
{
    if ( scorep_initialized )
    {
        return;
    }

    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_FUNCTION_ENTRY, "" );

    // even if we are not ready with the initialization we must prevent recursive
    // calls e.g. during the adapter initialization.
    scorep_initialized = true;
    SCOREP_Env_InitializeCoreEnvironmentVariables();
    SCOREP_Status_Initialize();

    scorep_initialization_sanity_checks();
    SCOREP_Timer_Initialize();
    SCOREP_CreateExperimentDir();

    // Need to be called before the first use of any SCOREP_Alloc function, in
    // particular before SCOREP_Thread_Initialize
    SCOREP_Memory_Initialize( SCOREP_Env_GetTotalMemory(), SCOREP_Env_GetPageSize() );

    // initialize before SCOREP_Thread_Initialize() because latter may create a
    // writer that needs the archive.
    scorep_otf2_initialize();

    SCOREP_DefinitionLocks_Initialize();
    // initialize before SCOREP_Thread_Initialize() because latter creates at least a
    // location definition.
    SCOREP_Definitions_Initialize();

    SCOREP_Thread_Initialize();
    scorep_parameter_table_initialize();

    scorep_profile_initialize();

    if ( !SCOREP_Mpi_HasMpi() )
    {
        scorep_set_otf2_archive_master_slave();
    }

    scorep_adapters_register();
    scorep_adapters_initialize();
    scorep_adapters_initialize_location(); // not sure if this should be triggered by thread management

    /* register finalization handler */
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

    if ( SCOREP_Env_RunVerbose() )
    {
        fprintf( stderr, "SCOREP running in verbose mode\n" );
    }
}


void
scorep_otf2_initialize()
{
    if ( !SCOREP_IsTracingEnabled() )
    {
        return;
    }
    scorep_otf2_archive = OTF2_Archive_New( SCOREP_GetExperimentDirName(),
                                            "traces",
                                            OTF2_FILEMODE_WRITE,
                                            1024 * 1024, // 1MB
                                            OTF2_SUBSTRATE_POSIX );
    assert( scorep_otf2_archive );
}


void
scorep_profile_initialize()
{
    if ( !SCOREP_IsProfilingEnabled() )
    {
        return;
    }

    SCOREP_Profile_Register();
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
SCOREP_FinalizeMeasurement
(
    void
)
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
}


void
SCOREP_FinalizeMeasurementMPI()
{
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
SCOREP_EnableRecording
(
    void
)
{
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_FUNCTION_ENTRY, "" );
}


/**
 * Disable event recording for this process.
 */
void
SCOREP_DisableRecording
(
    void
)
{
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_FUNCTION_ENTRY, "" );
}


/**
 * Predicate indicating if the process is recording events or not.
 */
bool
SCOREP_RecordingEnabled
(
    void
)
{
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_FUNCTION_ENTRY, "" );

    return false;
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

    // MPICH1 creates some extra processes that are not properely SCOREP
    // initialized and don't execute normal user code. We need to prevent SCOREP
    // finalization of these processes. See otf2:ticket:154.
    if ( SCOREP_Mpi_HasMpi() && !SCOREP_Mpi_IsInitialized() )
    {
        return;
    }

    // Calling SCOREP_Event.h functions after this point is considered
    // an instrumentation error.

    // order is important
    SCOREP_Unify();
    scorep_profile_finalize();
    scorep_parameter_table_finalize();
    SCOREP_Definitions_Write();
    SCOREP_Definitions_Finalize();
    SCOREP_DefinitionLocks_Finalize();
    scorep_otf2_finalize();
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
        SCOREP_Profile_Process( SCOREP_Profile_ProcessDefault );
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
    /// @todo refactor
    if ( !SCOREP_IsTracingEnabled() )
    {
        return;
    }

    assert( scorep_otf2_archive );

    int* n_locations_per_rank = SCOREP_Mpi_GatherNumberOfLocationsPerRank();
    int  n_global_locations   = 0;
    if ( SCOREP_Mpi_GetRank() == 0 )
    {
        for ( int rank = 0; rank < SCOREP_Mpi_GetCommWorldSize(); ++rank )
        {
            n_global_locations += n_locations_per_rank[ rank ];
        }
    }

    int* n_definitions_per_location = 0;
    if ( !SCOREP_Env_DoUnification() )
    {
        n_definitions_per_location = SCOREP_Mpi_GatherNumberOfDefinitionsPerLocation( n_locations_per_rank, n_global_locations );
    }

    if ( SCOREP_Mpi_GetRank() == 0 )
    {
        OTF2_Archive_SetNumberOfLocations( scorep_otf2_archive, n_global_locations );

        if ( !SCOREP_Env_DoUnification() )
        {
            OTF2_GlobDefWriter* global_definition_writer =
                OTF2_Archive_GetGlobDefWriter( scorep_otf2_archive,
                                               SCOREP_OnTracePreFlush,
                                               SCOREP_OnTraceAndDefinitionPostFlush );
            assert( global_definition_writer );

            /* write def for empty string */
            SCOREP_Error_Code status = OTF2_GlobDefWriter_GlobDefString(
                global_definition_writer, 0, "" );
            assert( status == SCOREP_SUCCESS );

            int index = 0;
            for ( int rank = 0; rank < SCOREP_Mpi_GetCommWorldSize(); ++rank )
            {
                for ( int location_id = 0; location_id < n_locations_per_rank[ rank ]; ++location_id )
                {
                    uint64_t global_location_id = ( ( ( uint64_t )location_id ) << 32 ) | ( uint64_t )rank;
                    status = OTF2_GlobDefWriter_GlobDefLocation(
                        global_definition_writer,
                        global_location_id,
                        0,
                        OTF2_GLOB_LOCATION_TYPE_THREAD, // use THREAD instead of PROCESS according to Dominic
                        n_definitions_per_location[ index ] );
                    assert( status == SCOREP_SUCCESS );
                    ++index;
                }
            }
            /// @todo set archive not unified
        }
    }


    if ( n_definitions_per_location )
    {
        free( n_definitions_per_location );
    }
    if ( n_locations_per_rank )
    {
        free( n_locations_per_rank );
    }
    OTF2_Archive_Delete( scorep_otf2_archive );
}
