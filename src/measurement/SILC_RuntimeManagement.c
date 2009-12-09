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


#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>


/**
 * @file        SILC_RuntimeManagement.c
 * @maintainer  Bert Wesarg <Bert.Wesarg@tu-dresden.de>
 *
 * @brief   Declaration of event recording functions to be used by the
 *          adapter layer.
 *
 *
 */


#include <SILC_Error.h>
#include <SILC_RuntimeManagement.h>
#include <SILC_Adapter.h>
#include <SILC_Config.h>
#include <SILC_Timing.h>

#include <OTF2_EvtWriter.h>

#include "silc_types.h"
#include "silc_adapter.h"


OTF2_EvtWriter* silc_local_event_writer;
uint64_t        silc_local_id;

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

/** atexit handler for finalization */
static void
silc_finalize( void );

/**
 * Return true if SILC_InitMeasurement() has been executed.
 */
bool
SILC_IsInitialized
(
)
{
    fprintf( stderr, "%s\n", __func__ );

    return silc_initialized;
}


static uint64_t
post_flush( void* unUsed );

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

    fprintf( stderr, "%s\n", __func__ );

    if ( silc_initialized )
    {
        return;
    }

    if ( silc_finalized )
    {
        _exit( EXIT_FAILURE );
    }

    error = SILC_ConfigRegister( NULL, silc_configs );

    SILC_InitTimer();

    if ( SILC_SUCCESS != error )
    {
        SILC_ERROR( error, "Can't register core config variables" );
        _exit( EXIT_FAILURE );
    }

    /* call register functions for all adapters */
    for ( size_t i = 0; silc_adapters[ i ]; ++i )
    {
        if ( silc_adapters[ i ]->adapter_register )
        {
            error = silc_adapters[ i ]->adapter_register();
        }

        if ( SILC_SUCCESS != error )
        {
            SILC_ERROR( error, "Can't register %s adapter",
                        silc_adapters[ i ]->adapter_name );
            _exit( EXIT_FAILURE );
        }
    }

    /* all config variables are registers => parse configure once */

    if ( silc_verbose )
    {
        fprintf( stderr, "SILC running in verbose mode\n" );
    }

    /* call initialization functions for all adapters */
    for ( size_t i = 0; silc_adapters[ i ]; ++i )
    {
        if ( silc_adapters[ i ]->adapter_init )
        {
            error = silc_adapters[ i ]->adapter_init();
        }

        if ( silc_verbose )
        {
            fprintf( stderr, "SILC initialize %s adapter: %s\n",
                     silc_adapters[ i ]->adapter_name,
                     SILC_Error_GetDescription( error ) );
        }

        if ( SILC_SUCCESS != error )
        {
            SILC_ERROR( error, "Can't initialize %s adapter",
                        silc_adapters[ i ]->adapter_name );
            _exit( EXIT_FAILURE );
        }
    }


    /* create location */

    local_event_writer = OTF2_EvtWriter_New( 1 << 24,
                                             NULL,
                                             0,
                                             "silc",
                                             post_flush );

    if ( !silc_local_event_writer )
    {
        SILC_ERROR( SILC_ERROR_ENOMEM, "Can't create event buffer" );
        _exit( EXIT_FAILURE );
    }

    /* call initialization functions for all adapters */
    for ( size_t i = 0; silc_adapters[ i ]; ++i )
    {
        if ( silc_adapters[ i ]->adapter_init_location )
        {
            error = silc_adapters[ i ]->adapter_init_location();
        }

        if ( silc_verbose )
        {
            fprintf( stderr, "SILC initialize location %s adapter: %s\n",
                     silc_adapters[ i ]->adapter_name,
                     SILC_Error_GetDescription( error ) );
        }

        if ( SILC_SUCCESS != error )
        {
            SILC_ERROR( error, "Can't initialize location for %s adapter",
                        silc_adapters[ i ]->adapter_name );
            _exit( EXIT_FAILURE );
        }
    }

    /* all done, report successful initialization */
    silc_initialized = true;

    /* register finalization handler */
    atexit( silc_finalize );
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
    fprintf( stderr, "%s\n", __func__ );

    silc_finalize();
}


/**
 * Special initialization of the measurement system when using MPI.
 */
void
SILC_InitMeasurementMPI
(
    int rank
)
{
    fprintf( stderr, "%s\n", __func__ );

    silc_local_id = rank;
    OTF2_EvtWriter_SetLocationID( silc_local_event_writer, silc_local_id );
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
    fprintf( stderr, "%s\n", __func__ );
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
    fprintf( stderr, "%s\n", __func__ );
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
    fprintf( stderr, "%s\n", __func__ );

    return false;
}

static void
silc_finalize( void )
{
    fprintf( stderr, "%s\n", __func__ );

    if ( !silc_initialized || silc_finalized )
    {
        return;
    }

    if ( silc_local_event_writer )
    {
        OTF2_EvtWriter_Delete( silc_local_event_writer );
    }
    silc_local_event_writer = NULL;

    silc_finalized = true;
}

static uint64_t
post_flush( void* unUsed )
{
    return SILC_GetWallClockTime();
}
