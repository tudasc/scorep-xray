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

#include "silc_types.h"
#include "silc_adapter.h"
#include "silc_mpi.h"
#include "silc_thread.h"
#include "silc_runtime_management.h"

#include <OTF2_File.h>


/** @brief Measurement system initialized? */
static bool silc_initialized;


/** @brief Measurement system finalized? */
static bool silc_finalized;


/** @brief Run in verbose mode */
static bool silc_verbose;


/** @brief Measurement system configure variables */
static SILC_ConfigVariable silc_configs[] = {
    {
        "verbose",
        SILC_CONFIG_TYPE_BOOL,
        &silc_verbose,
        NULL,
        "false",
        "Be verbose",
        "Long help"
    },
    SILC_CONFIG_TERMINATOR
};


/* *INDENT-OFF* */
/** atexit handler for finalization */
static void silc_finalize( void );

static void silc_otf2_initialize();

static void silc_otf2_finalize();
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
SILC_InitMeasurement
(
    void
)
{
    SILC_Error_Code error;

    SILC_DEBUG_PRINTF( SILC_DEBUG_FUNCTION_ENTRY, "" );

    if ( silc_initialized )
    {
        return;
    }

    if ( silc_finalized )
    {
        _Exit( EXIT_FAILURE );
    }

    if ( omp_in_parallel() )
    {
        SILC_ERROR( SILC_ERROR_INTEGRITY_FAULT, "Can't initialize measurement core from within parallel region." );
        _Exit( EXIT_FAILURE );
    }

    error = SILC_ConfigRegister( NULL, silc_configs );

    if ( SILC_SUCCESS != error )
    {
        SILC_ERROR( error, "Can't register core config variables" );
        _Exit( EXIT_FAILURE );
    }

    SILC_InitTimer(); // init timer before call to SILC_CreateExperimentDir
    SILC_CreateExperimentDir();

    // we may read total memory and pagesize from config file and pass it to
    // SILC_Memory_Initialize.
    // Need to be called before the first use of any SILC_Alloc function, in
    // particular before SILC_Thread_Initialize
    SILC_Memory_Initialize();

    // initialize before SILC_Thread_Initialize() because latter may create a
    // writer that needs the archive.
    silc_otf2_initialize();

    SILC_Thread_Initialize();

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

    /* all config variables are registers => parse configure once */

    if ( silc_verbose )
    {
        fprintf( stderr, "SILC running in verbose mode\n" );
    }

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
        else if ( silc_verbose )
        {
            fprintf( stderr, "SILC successfully initialized %s adapter\n",
                     silc_adapters[ i ]->adapter_name );
        }
    }

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
        else if ( silc_verbose )
        {
            fprintf( stderr, "SILC successfully initialized location for %s adapter\n",
                     silc_adapters[ i ]->adapter_name );
        }
    }

    /* all done, report successful initialization */
    silc_initialized = true;

    /* register finalization handler */
    atexit( silc_finalize );
}


void
silc_otf2_initialize()
{
    if ( !silc_tracing_enabled )
    {
        return;
    }

    silc_otf2_archive = OTF2_Archive_New( "traces",
                                          OTF2_FILEMODE_MODIFY,
                                          1024 * 1024, // 1MB
                                          OTF2_SUBSTRATE_POSIX );
    assert( silc_otf2_archive );
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

    if ( flush_done )
    {
        fprintf( stderr, "ERROR: Switching to MPI mode after the first flush.\n" );
        fprintf( stderr, "       Consider to increase buffer size to prevent this.\n" );
        _Exit( EXIT_FAILURE );
    }

    assert( rank == SILC_Mpi_GetRank() );

    SILC_CreateExperimentDir();

    SILC_Thread_LocationData* locationData = SILC_Thread_GetLocationData();
    SILC_Trace_LocationData*  trace_data   = SILC_Thread_GetTraceLocationData( locationData );
    trace_data->otf_location = SILC_GetOTF2LocationId( locationData );
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

    /* we call the finalize and de-register function in reverse order */

    /* call finalization functions for all adapters */
    for ( size_t i = silc_number_of_adapters; i-- > 0; )
    {
        if ( silc_adapters[ i ]->adapter_finalize )
        {
            silc_adapters[ i ]->adapter_finalize();
        }

        if ( silc_verbose )
        {
            fprintf( stderr, "SILC finalized %s adapter\n",
                     silc_adapters[ i ]->adapter_name );
        }
    }

    /* call de-register functions for all adapters */
    for ( size_t i = silc_number_of_adapters; i-- > 0; )
    {
        if ( silc_adapters[ i ]->adapter_deregister )
        {
            silc_adapters[ i ]->adapter_deregister();
        }

        if ( silc_verbose )
        {
            fprintf( stderr, "SILC de-registered %s adapter\n",
                     silc_adapters[ i ]->adapter_name );
        }
    }

    // keep this order as thread handling uses memory management
    SILC_Thread_Finalize();
    silc_otf2_finalize();
    SILC_Memory_Finalize();

    silc_finalized = true;
}

void
silc_otf2_finalize()
{
    if ( silc_tracing_enabled )
    {
        OTF2_Archive_Delete( silc_otf2_archive );
    }
}
