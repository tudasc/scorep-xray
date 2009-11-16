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

#include "silc_types.h"
#include "silc_adapter.h"

/** @brief Measurement system initialized? */
static bool silc_initialized;

/** @brief Run in verbose mode */
static bool silc_verbose;

/** @brief Measurement system configure variables */
static SILC_ConfigVariable silc_configs[] = {
    {
        NULL, // is in global namespace
        "verbose",
        SILC_CONFIG_TYPE_BOOL,
        &silc_verbose,
        NULL,
        "false",
        "Be verbose",
        "Long help"
    },
};

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

    error = SILC_ConfigRegister( silc_configs,
                                 sizeof( silc_configs ) / sizeof( silc_configs[ 0 ] ) );

    if ( SILC_SUCCESS != error )
    {
        SILC_ERROR( error, "Can't register core config variables" );
        _exit( 1 );
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
            _exit( 1 );
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
            _exit( 1 );
        }
    }


    /* create location */

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
            _exit( 1 );
        }
    }

    /* all done, report successful initialization */
    silc_initialized = true;
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
}


/**
 * Special initialization of the measurement system when using MPI.
 */
void
SILC_InitMeasurementMPI
(
    void
)
{
    fprintf( stderr, "%s\n", __func__ );
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
