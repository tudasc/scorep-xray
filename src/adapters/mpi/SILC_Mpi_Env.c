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


#include "SILC_Mpi.h"
#include "config.h"

/**
 * @file  SILC_Mpi_Env.c
 *
 * @brief C interface wrappers for environmental management.
 */

/** Flag set if the measurement sysem was already opened by another adapter.
    If the measurement system is not already initilized, it is assumed that
    the mpi adapter is the only active adapter. In this case, at first an
    additional region is entered MPI_Init. Thus, all regions appear as
    childs of this region.
 */
static int silc_mpi_parallel_entered = 0;

/**
 * @name C wrappers
 * @{
 */

/**
 * Measurement wrapper for MPI_Init.
 * @note Manually adapted wrapper
 * @note C interface
 * @note Introduced with MPI-1
 * @ingroup env
 * If the measurement system is not initialized, it will iniialize the measurement
 * system and enter a special region named "parallel" which is exited when MPI is
 * finalized.
 * Generates an enter event at function start and an exit event on function end.
 */
int
MPI_Init( int*    argc,
          char*** argv )
{
    int event_gen_active = 0;          /* init is deferred to later */
    int return_val, i;
    int fflag;

    if ( !SILC_IsInitialized() )
    {
        /* Initialize the measurement system */
        SILC_InitMeasurement();

        /* Enter global MPI region */
        SILC_EnterRegion( silc_mpi_regid[ SILC_PARALLEL__MPI ] );

        /* Remember that SILC_PARALLEL__MPI was entered */
        silc_mpi_parallel_entered = 1;
    }

    event_gen_active = SILC_MPI_IS_EVENT_GEN_ON_FOR( SILC_MPI_ENABLED_ENV );

    if ( event_gen_active )
    {
        SILC_MPI_EVENT_GEN_OFF();

        /* Enter the init region */
        SILC_EnterRegion( silc_mpi_regid[ SILC__MPI_INIT ] );
    }

    return_val = PMPI_Init( argc, argv );

    /* XXXX should only continue if MPI initialised OK! */

    if ( ( PMPI_Finalized( &fflag ) == MPI_SUCCESS ) && ( fflag == 0 ) )
    {
        /* initialize communicator management and register MPI_COMM_WORLD*/
        silc_mpi_comm_init();

        /* complete initialization of measurement core and MPI event handling */
        SILC_InitMeasurementMPI();
    }

    if ( event_gen_active )
    {
        SILC_ExitRegion( silc_mpi_regid[ SILC__MPI_INIT ] );
        SILC_MPI_EVENT_GEN_ON();
    }

    return return_val;
}

#if HAVE( DECL_MPI_INIT_THREAD )
/**
 * Measurement wrapper for MPI_Init_thread, the thread-capable
 * alternative to MPI_Init.
 * @note Manually adapted wrapper
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup env
 * If the measurement system is not initialized, it will iniialize the measurement
 * system and enter a special region named "parallel" which is exited when MPI is
 * finalized.
 * Generates an enter event at function start and an exit event on function end.
 */
int
MPI_Init_thread( int*    argc,
                 char*** argv,
                 int     required,
                 int*    provided )
{
    int event_gen_active = 0;
    int return_val, i;
    int fflag;

    if ( !SILC_IsInitialized() )
    {
        /* Initialize the measurement system */
        SILC_InitMeasurement();

        /* Enter global MPI region */
        SILC_EnterRegion( silc_mpi_regid[ SILC_PARALLEL__MPI ] );

        /* Remember that SILC_PARALLEL__MPI was entered */
        silc_mpi_parallel_entered = 1;
    }

    event_gen_active = SILC_MPI_IS_EVENT_GEN_ON_FOR( SILC_MPI_ENABLED_ENV );

    if ( event_gen_active )
    {
        SILC_MPI_EVENT_GEN_OFF();

        SILC_EnterRegion( silc_mpi_regid[ SILC__MPI_INIT_THREAD ] );
    }

    return_val = PMPI_Init_thread( argc, argv, required, provided );

    /* XXXX should only continue if MPI initialised OK! */

    if ( ( return_val == MPI_SUCCESS ) && ( *provided > MPI_THREAD_FUNNELED ) )
    {
        SILC_DEBUG_PRINTF( SILC_WARNING | SILC_DEBUG_MPI,
                           "MPI environment initialized with level exceeding MPI_THREAD_FUNNELED!" );
        /* XXXX continue even though not supported by analysis */
    }

    if ( ( PMPI_Finalized( &fflag ) == MPI_SUCCESS ) && ( fflag == 0 ) )
    {
        /* initialize communicator management and register MPI_COMM_WORLD */
        silc_mpi_comm_init();

        /* complete initialization of measurement core and MPI event handling */

        /* silc_mpi_comm_determination = esd_mpi_init();*/
        SILC_InitMeasurementMPI();
    }

    if ( event_gen_active )
    {
        SILC_ExitRegion( silc_mpi_regid[ SILC__MPI_INIT_THREAD ] );
        SILC_MPI_EVENT_GEN_ON();
    }

    return return_val;
}
#endif

/**
 * Measurement wrapper for MPI_Finalize
 * @note Manually adapted wrapper
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup env
 * Generates an enter event at function start and an exit event at function end.
 * If the "parallel" region was entered in MPI_Init, it exits the "parallel" region.
 * It does not perform the MPI finalization, because MPI is still needed by the
 * measurement system, but substituts it with a barrier. The MPI finalization
 * will be done from the measurement system.
 */
int
MPI_Finalize()
{
    const int event_gen_active = SILC_MPI_IS_EVENT_GEN_ON_FOR( SILC_MPI_ENABLED_ENV );
    int       return_val;

    if ( event_gen_active )
    {
        SILC_MPI_EVENT_GEN_OFF();
        SILC_EnterRegion( silc_mpi_regid[ SILC__MPI_FINALIZE ] );
    }

    /* finalize communicator and request management */
    silc_mpi_comm_finalize();
/* Asynchroneous communication not supported
   silc_mpi_request_finalize();
 */

    /* finalize MPI event handling */
    /* esd_mpi_finalize(); */

    /* fake finalization, so that MPI can be used during EPIK finalization */
    return_val = PMPI_Barrier( MPI_COMM_WORLD );

    if ( event_gen_active )
    {
        /* Exit MPI_Finalize region */
        SILC_ExitRegion( silc_mpi_regid[ SILC__MPI_FINALIZE ] );

        /* Exit the extra parallel region in case it was entered in MPI_Init */
        if ( silc_mpi_parallel_entered )
        {
            SILC_ExitRegion( silc_mpi_regid[ SILC_PARALLEL__MPI ] );
        }

        SILC_MPI_EVENT_GEN_ON();
    }

    return return_val;
}

#if HAVE( DECL_MPI_IS_THREAD_MAIN ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_ENV )
/**
 * Measurement wrapper for MPI_Is_thread_main
 * @note Auto-generated by wrapgen from template: SILC_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup env
 * It wraps the me) call with enter and exit events.
 */
int
MPI_Is_thread_main( int* flag )
{
    int return_val;

    if ( SILC_MPI_IS_EVENT_GEN_ON_FOR( SILC_MPI_ENABLED_ENV ) )
    {
        SILC_MPI_EVENT_GEN_OFF();
        SILC_EnterRegion( silc_mpi_regid[ SILC__MPI_IS_THREAD_MAIN ] );

        return_val = PMPI_Is_thread_main( flag );

        SILC_ExitRegion( silc_mpi_regid[ SILC__MPI_IS_THREAD_MAIN ] );
        SILC_MPI_EVENT_GEN_ON();
    }
    else
    {
        return_val = PMPI_Is_thread_main( flag );
    }

    return return_val;
}
#endif

#if HAVE( DECL_MPI_QUERY_THREAD ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_ENV )
/**
 * Measurement wrapper for MPI_Query_thread
 * @note Auto-generated by wrapgen from template: SILC_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup env
 * It wraps the me) call with enter and exit events.
 */
int
MPI_Query_thread( int* provided )
{
    int return_val;

    if ( SILC_MPI_IS_EVENT_GEN_ON_FOR( SILC_MPI_ENABLED_ENV ) )
    {
        SILC_MPI_EVENT_GEN_OFF();
        SILC_EnterRegion( silc_mpi_regid[ SILC__MPI_QUERY_THREAD ] );

        return_val = PMPI_Query_thread( provided );

        SILC_ExitRegion( silc_mpi_regid[ SILC__MPI_QUERY_THREAD ] );
        SILC_MPI_EVENT_GEN_ON();
    }
    else
    {
        return_val = PMPI_Query_thread( provided );
    }

    return return_val;
}
#endif


#if HAVE( DECL_MPI_FINALIZED ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_ENV )
/**
 * Measurement wrapper for MPI_Finalized
 * @note Auto-generated by wrapgen from template: SILC_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup env
 * It wraps the me) call with enter and exit events.
 */
int
MPI_Finalized( int* flag )
{
    int return_val;

    if ( SILC_MPI_IS_EVENT_GEN_ON_FOR( SILC_MPI_ENABLED_ENV ) )
    {
        SILC_MPI_EVENT_GEN_OFF();
        SILC_EnterRegion( silc_mpi_regid[ SILC__MPI_FINALIZED ] );

        return_val = PMPI_Finalized( flag );

        SILC_ExitRegion( silc_mpi_regid[ SILC__MPI_FINALIZED ] );
        SILC_MPI_EVENT_GEN_ON();
    }
    else
    {
        return_val = PMPI_Finalized( flag );
    }

    return return_val;
}
#endif

#if HAVE( DECL_MPI_INITIALIZED ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_ENV )
/**
 * Measurement wrapper for MPI_Initialized
 * @note Auto-generated by wrapgen from template: SILC_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-1
 * @ingroup env
 * It wraps the me) call with enter and exit events.
 */
int
MPI_Initialized( int* flag )
{
    int return_val;

    if ( SILC_MPI_IS_EVENT_GEN_ON_FOR( SILC_MPI_ENABLED_ENV ) )
    {
        SILC_MPI_EVENT_GEN_OFF();
        SILC_EnterRegion( silc_mpi_regid[ SILC__MPI_INITIALIZED ] );

        return_val = PMPI_Initialized( flag );

        SILC_ExitRegion( silc_mpi_regid[ SILC__MPI_INITIALIZED ] );
        SILC_MPI_EVENT_GEN_ON();
    }
    else
    {
        return_val = PMPI_Initialized( flag );
    }

    return return_val;
}
#endif


/**
 * @}
 */
