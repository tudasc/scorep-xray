/*
 * This file is part of the SILC project (http://www.silc.de)
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
 * @file        SILC_RuntimeManagement.c
 * @maintainer  Bert Wesarg <Bert.Wesarg@tu-dresden.de>
 *
 * @brief   Declaration of event recording functions to be used by the
 *          adapter layer.
 *
 *
 */


#include <SILC_RuntimeManagement.h>

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#include <SILC_Error.h>
#include <SILC_Debug.h>
#include <SILC_Memory.h>
#include <SILC_Adapter.h>
#include <SILC_Config.h>
#include <SILC_Timing.h>
#include <SILC_Omp.h>
#include <SILC_Profile.h>
#include <silc_unify.h>


#include "silc_types.h"
#include "silc_adapter.h"
#include "silc_definitions.h"
#include "silc_status.h"
#include "silc_mpi.h"
#include "silc_thread.h"
#include "silc_runtime_management.h"
#include "silc_definition_locking.h"
#include "silc_parameter_registration.h"

#include <OTF2_File.h>

#include <inttypes.h>


/** @brief Measurement system initialized? */
static bool silc_initialized;


/** @brief Measurement system finalized? */
static bool silc_finalized;


/* *INDENT-OFF* */
/** atexit handler for finalization */
static void silc_finalize( void );
static void silc_otf2_initialize();
static void silc_otf2_finalize();
static void silc_set_otf2_archive_master_slave();
static void silc_adapters_register();
static void silc_adapters_deregister();
static void silc_adapters_initialize();
static void silc_adapters_finalize();
static void silc_adapters_initialize_location(); // needed?
static void silc_adapters_finalize_location(); // needed?
static void silc_initialization_sanity_checks();
static void silc_register_config_variables( SILC_ConfigVariable configVars[] );
static void silc_profile_initialize();
static void silc_profile_finalize();
//static void silc_deregister_config_variables( SILC_ConfigVariable configVars[] ); needed?
/* *INDENT-ON* */

/**
 * Return true if SILC_InitMeasurement() has been executed.
 */
bool
SILC_IsInitialized()
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_FUNCTION_ENTRY, "" );

    return silc_initialized && !silc_finalized;
}


/**
 * Initialize the measurement system from the adapter layer.
 */
void
SILC_InitMeasurement( void )
{
    if ( silc_initialized )
    {
        return;
    }

    SILC_DEBUG_PRINTF( SILC_DEBUG_FUNCTION_ENTRY, "" );

    // even if we are not ready with the initialization we must prevent recursive
    // calls e.g. during the adapter initialization.
    silc_initialized = true;
    silc_register_config_variables( silc_env_configs );
    SILC_Status_Initialize();

    silc_initialization_sanity_checks();
    SILC_Timer_Initialize();
    SILC_CreateExperimentDir();

    // we may read total memory and pagesize from config file and pass it to
    // SILC_Memory_Initialize.
    // Need to be called before the first use of any SILC_Alloc function, in
    // particular before SILC_Thread_Initialize
    SILC_Memory_Initialize( 1200 * 1024 /* 1200 kB */, 8192 /* 8 kB */ ); // 150 pages

    // initialize before SILC_Thread_Initialize() because latter may create a
    // writer that needs the archive.
    silc_otf2_initialize();

    SILC_DefinitionLocks_Initialize();
    // initialize before SILC_Thread_Initialize() because latter creates at least a
    // location definition.
    SILC_Definitions_Initialize();

    SILC_Thread_Initialize();
    silc_parameter_table_initialize();

    silc_profile_initialize();

    if ( !SILC_Mpi_HasMpi() )
    {
        silc_set_otf2_archive_master_slave();
    }

    silc_adapters_register();
    silc_adapters_initialize();
    silc_adapters_initialize_location(); // not sure if this should be triggered by thread management

    /* register finalization handler */
    atexit( silc_finalize );
}


void
silc_initialization_sanity_checks()
{
    if ( silc_finalized )
    {
        _Exit( EXIT_FAILURE );
    }

    if ( omp_in_parallel() )
    {
        SILC_ERROR( SILC_ERROR_INTEGRITY_FAULT, "Can't initialize measurement core from within parallel region." );
        _Exit( EXIT_FAILURE );
    }

    if ( silc_env_verbose )
    {
        fprintf( stderr, "SILC running in verbose mode\n" );
    }
}


static void
silc_register_config_variables( SILC_ConfigVariable configVars[] )
{
    /* all config variables are registers => parse configure once */
    SILC_Error_Code error = SILC_ConfigRegister( NULL, silc_env_configs );

    if ( SILC_SUCCESS != error )
    {
        SILC_ERROR( error, "Can't register core config variables" );
        _Exit( EXIT_FAILURE );
    }
}


void
silc_otf2_initialize()
{
    if ( !SILC_IsTracingEnabled() )
    {
        return;
    }
    printf( "OTF2_Archive_New archiveName = traces, archivePath = %s\n", SILC_GetExperimentDirName() );
    silc_otf2_archive = OTF2_Archive_New( SILC_GetExperimentDirName(),
                                          "traces",
                                          OTF2_FILEMODE_WRITE,
                                          1024 * 1024, // 1MB
                                          OTF2_SUBSTRATE_POSIX );
    assert( silc_otf2_archive );
}


void
silc_profile_initialize()
{
    if ( !SILC_IsProfilingEnabled() )
    {
        return;
    }

    SILC_Profile_Register();
    SILC_Profile_Initialize( 0, NULL );
    SILC_Profile_OnLocationCreation( SILC_Thread_GetLocationData(), NULL ); // called also from silc_thread_call_externals_on_new_location
    SILC_Profile_OnThreadActivation( SILC_Thread_GetLocationData(), NULL ); // called also from silc_thread_call_externals_on_thread_activation
}


void
silc_set_otf2_archive_master_slave()
{
    if ( !SILC_IsTracingEnabled() )
    {
        return;
    }

    // call this function only once
    static bool archive_master_slave_already_set = false;
    assert( !archive_master_slave_already_set );
    archive_master_slave_already_set = true;

    SILC_Error_Code error;
    if ( SILC_Mpi_GetRank() == 0 )
    {
        printf( "OTF2_Archive_SetMasterSlaveMode from rank %d with OTF2_MASTER\n", SILC_Mpi_GetRank() );
        error = OTF2_Archive_SetMasterSlaveMode(
            silc_otf2_archive, OTF2_MASTER );
    }
    else
    {
        printf( "OTF2_Archive_SetMasterSlaveMode from rank %d with OTF2_SLAVE\n", SILC_Mpi_GetRank() );
        error = OTF2_Archive_SetMasterSlaveMode(
            silc_otf2_archive, OTF2_SLAVE );
    }
    if ( SILC_SUCCESS != error )
    {
        _Exit( EXIT_FAILURE );
    }
}


void
silc_adapters_register()
{
    SILC_Error_Code error;
    /* call register functions for all adapters */
    for ( size_t i = 0; i < silc_number_of_adapters; i++ )
    {
        if ( silc_adapters[ i ]->adapter_register )
        {
            error = silc_adapters[ i ]->adapter_register();
        }

        if ( SILC_SUCCESS != error )
        {
            SILC_ERROR( error, "Can't register %s adapter",
                        silc_adapters[ i ]->adapter_name );
            _Exit( EXIT_FAILURE );
        }
    }
}


void
silc_adapters_initialize()
{
    SILC_Error_Code error;
    /* call initialization functions for all adapters */
    for ( size_t i = 0; i < silc_number_of_adapters; i++ )
    {
        if ( silc_adapters[ i ]->adapter_init )
        {
            error = silc_adapters[ i ]->adapter_init();
        }

        if ( SILC_SUCCESS != error )
        {
            SILC_ERROR( error, "Can't initialize %s adapter",
                        silc_adapters[ i ]->adapter_name );
            _Exit( EXIT_FAILURE );
        }
        else if ( silc_env_verbose )
        {
            fprintf( stderr, "SILC successfully initialized %s adapter\n",
                     silc_adapters[ i ]->adapter_name );
        }
    }
}


static void
silc_adapters_initialize_location()
{
    SILC_Error_Code error;
    /* create location */

    /* call initialization functions for all adapters */
    for ( size_t i = 0; i < silc_number_of_adapters; i++ )
    {
        if ( silc_adapters[ i ]->adapter_init_location )
        {
            error = silc_adapters[ i ]->adapter_init_location();
        }

        if ( SILC_SUCCESS != error )
        {
            SILC_ERROR( error, "Can't initialize location for %s adapter",
                        silc_adapters[ i ]->adapter_name );
            _Exit( EXIT_FAILURE );
        }
        else if ( silc_env_verbose )
        {
            fprintf( stderr, "SILC successfully initialized location for %s adapter\n",
                     silc_adapters[ i ]->adapter_name );
        }
    }
}


/**
 * Finalize the measurement system.
 */
void
SILC_FinalizeMeasurement
(
    void
)
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_FUNCTION_ENTRY, "" );
    SILC_FinalizeMeasurementMPI();
    silc_finalize();
}


/**
 * Special initialization of the measurement system when using MPI.
 */
void
SILC_InitMeasurementMPI( int rank )
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_FUNCTION_ENTRY, "" );

    if ( omp_in_parallel() )
    {
        SILC_ERROR( SILC_ERROR_INTEGRITY_FAULT, "Can't initialize measurement core from within parallel region." );
        _Exit( EXIT_FAILURE );
    }

    if ( SILC_Otf2_HasFlushed() )
    {
        fprintf( stderr, "ERROR: Switching to MPI mode after the first flush.\n" );
        fprintf( stderr, "       Consider to increase buffer size to prevent this.\n" );
        _Exit( EXIT_FAILURE );
    }

    SILC_Mpi_SetRankTo( rank );
    SILC_CreateExperimentDir();
    SILC_ProcessDeferredLocations();
    silc_set_otf2_archive_master_slave();
}


void
SILC_FinalizeMeasurementMPI()
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
SILC_EnableRecording
(
    void
)
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_FUNCTION_ENTRY, "" );
}


/**
 * Disable event recording for this process.
 */
void
SILC_DisableRecording
(
    void
)
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_FUNCTION_ENTRY, "" );
}


/**
 * Predicate indicating if the process is recording events or not.
 */
bool
SILC_RecordingEnabled
(
    void
)
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_FUNCTION_ENTRY, "" );

    return false;
}


static void
silc_finalize( void )
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_FUNCTION_ENTRY, "" );

    if ( !silc_initialized || silc_finalized )
    {
        return;
    }
    silc_finalized = true;

    // MPICH1 creates some extra processes that are not properely SILC
    // initialized and don't execute normal user code. We need to prevent SILC
    // finalization of these processes. See otf2:ticket:154.
    if ( SILC_Mpi_HasMpi() && !SILC_Mpi_IsInitialized() )
    {
        return;
    }

    // Calling SILC_Event.h functions after this point is considered
    // an instrumentation error.

    // order is important
    SILC_Unify();
    silc_profile_finalize();
    silc_parameter_table_finalize();
    SILC_Definitions_Write();
    SILC_Definitions_Finalize();
    SILC_DefinitionLocks_Finalize();
    silc_otf2_finalize();
    SILC_RenameExperimentDir();  // needs MPI

    silc_adapters_finalize_location();
    silc_adapters_finalize(); // here PMPI_Finalize is called
    silc_adapters_deregister();

    SILC_Thread_Finalize();
    SILC_Memory_Finalize();
}


static void
silc_profile_finalize()
{
    if ( SILC_IsProfilingEnabled() )
    {
        SILC_Profile_Process( SILC_Profile_ProcessDefault, SILC_Profile_OutputDefault );
        SILC_Profile_Finalize();
    }
}

static void
silc_adapters_finalize_location()
{
    for ( size_t i = silc_number_of_adapters; i-- > 0; )
    {
        if ( silc_adapters[ i ]->adapter_finalize_location )
        {
            //silc_adapters[ i ]->adapter_finalize_location(location_ptr???);
        }

        if ( silc_env_verbose )
        {
            fprintf( stderr, "SILC finalized %s adapter location\n",
                     silc_adapters[ i ]->adapter_name );
        }
    }
}


static void
silc_adapters_finalize()
{
    /* call finalization functions for all adapters */
    for ( size_t i = silc_number_of_adapters; i-- > 0; )
    {
        if ( silc_adapters[ i ]->adapter_finalize )
        {
            silc_adapters[ i ]->adapter_finalize();
        }

        if ( silc_env_verbose )
        {
            fprintf( stderr, "SILC finalized %s adapter\n",
                     silc_adapters[ i ]->adapter_name );
        }
    }
}


static void
silc_adapters_deregister()
{
    /* call de-register functions for all adapters */
    for ( size_t i = silc_number_of_adapters; i-- > 0; )
    {
        if ( silc_adapters[ i ]->adapter_deregister )
        {
            silc_adapters[ i ]->adapter_deregister();
        }

        if ( silc_env_verbose )
        {
            fprintf( stderr, "SILC de-registered %s adapter\n",
                     silc_adapters[ i ]->adapter_name );
        }
    }
}


void
silc_otf2_finalize()
{
    /// @todo refactor
    if ( !SILC_IsTracingEnabled() )
    {
        return;
    }

    assert( silc_otf2_archive );

    int* n_locations_per_rank = SILC_Mpi_GatherNumberOfLocationsPerRank();
    int  n_global_locations   = 0;
    if ( SILC_Mpi_GetRank() == 0 )
    {
        for ( int rank = 0; rank < SILC_Mpi_GetCommWorldSize(); ++rank )
        {
            n_global_locations += n_locations_per_rank[ rank ];
        }
    }

    int* n_definitions_per_location = 0;
    if ( !silc_env_unify )
    {
        n_definitions_per_location = SILC_Mpi_GatherNumberOfDefinitionsPerLocation( n_locations_per_rank, n_global_locations );
    }

    if ( SILC_Mpi_GetRank() == 0 )
    {
        printf( "OTF2_Archive_SetNumberOfLocations from rank %d with %d locations\n", SILC_Mpi_GetRank(), n_global_locations );
        OTF2_Archive_SetNumberOfLocations( silc_otf2_archive, n_global_locations );

        if ( !silc_env_unify )
        {
            printf( "OTF2_Archive_GetGlobDefWriter\n" );
            OTF2_GlobDefWriter* global_definition_writer =
                OTF2_Archive_GetGlobDefWriter( silc_otf2_archive,
                                               SILC_OnTracePreFlush,
                                               SILC_OnTraceAndDefinitionPostFlush );
            assert( global_definition_writer );

            int index = 0;
            for ( int rank = 0; rank < SILC_Mpi_GetCommWorldSize(); ++rank )
            {
                for ( int location_id = 0; location_id < n_locations_per_rank[ rank ]; ++location_id )
                {
                    uint64_t        global_location_id = ( ( ( uint64_t )location_id ) << 32 ) | ( uint64_t )rank;
                    SILC_Error_Code status             = OTF2_GlobDefWriter_GlobDefLocation(
                        global_definition_writer,
                        global_location_id,
                        "",
                        OTF2_GLOB_LOCATION_TYPE_THREAD, // use THREAD instead of PROCESS according to Dominic
                        n_definitions_per_location[ index ] );
                    assert( status == SILC_SUCCESS );
                    printf( "OTF2_GlobDefWriter_GlobDefLocation from rank %d, location %d, global_location %" PRIu64 ", definitions %d\n", rank, location_id,  global_location_id, n_definitions_per_location[ index ] );
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
    printf( "OTF2_Archive_Delete on rank %d\n", SILC_Mpi_GetRank() );
    OTF2_Archive_Delete( silc_otf2_archive );
}
