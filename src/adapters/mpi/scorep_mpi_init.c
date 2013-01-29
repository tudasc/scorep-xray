/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
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
 * @file       scorep_mpi_init.c
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 * @status     alpha
 * @ingroup    MPI_Wrapper
 *
 * @brief Contains the initialization function implementations for the measurement system.
 */

#include <config.h>

#include <stdbool.h>

#include <UTILS_Error.h>
#define SCOREP_DEBUG_MODULE_NAME MPI
#include <UTILS_Debug.h>

#include <SCOREP_Location.h>
#include <SCOREP_Mpi_Init.h>
#include <SCOREP_Mpi_Reg.h>
#include <SCOREP_Config.h>
#include <SCOREP_Mpi.h>
#include <SCOREP_RuntimeManagement.h>

#include <stdio.h>


/**
 * @def SCOREP_FORTRAN_GET_MPI_STATUS_SIZE
 * Defines the Fortran mangled form of scorep_fortran_get_mpi_status_size.
 */
#define SCOREP_FORTRAN_GET_MPI_STATUS_SIZE \
    F77_FUNC_( scorep_fortran_get_mpi_status_size, \
               SCOREP_FORTRAN_GET_MPI_STATUS_SIZE )

/**
 * @def SCOREP_FORTRAN_GET_MPI_BOTTOM
 * Defines the Fortran mangled form of scorep_fortran_get_mpi_bottom.
 */
#define SCOREP_FORTRAN_GET_MPI_BOTTOM \
    F77_FUNC_( scorep_fortran_get_mpi_bottom, \
               SCOREP_FORTRAN_GET_MPI_BOTTOM )

/**
 * @def SCOREP_FORTRAN_GET_MPI_IN_PLACE
 * Defines the Fortran mangled form of scorep_fortran_get_mpi_in_place.
 */
#define SCOREP_FORTRAN_GET_MPI_IN_PLACE \
    F77_FUNC_( scorep_fortran_get_mpi_in_place, \
               SCOREP_FORTRAN_GET_MPI_IN_PLACE )

/**
 * @def SCOREP_FORTRAN_GET_MPI_STATUS_IGNORE
 * Defines the Fortran mangled form of scorep_fortran_get_mpi_status_ignore.
 */
#define SCOREP_FORTRAN_GET_MPI_STATUS_IGNORE \
    F77_FUNC_( scorep_fortran_get_mpi_status_ignore, \
               SCOREP_FORTRAN_GET_MPI_STATUS_IGNORE )

/**
 * @def SCOREP_FORTRAN_GET_MPI_STATUSES_IGNORE
 * Defines the Fortran mangled form of scorep_fortran_get_mpi_statuses_ignore.
 */
#define SCOREP_FORTRAN_GET_MPI_STATUSES_IGNORE \
    F77_FUNC_( scorep_fortran_get_mpi_statuses_ignore, \
               SCOREP_FORTRAN_GET_MPI_STATUSES_IGNORE )

/**
 * @def SCOREP_FORTRAN_GET_MPI_UNWEIGHTED
 * Defines the Fortran mangled form of scorep_fortran_get_mpi_unweighted.
 */
#define SCOREP_FORTRAN_GET_MPI_UNWEIGHTED \
    F77_FUNC_( scorep_fortran_get_mpi_unweighted, \
               SCOREP_FORTRAN_GET_MPI_UNWEIGHTED )

/**
   Stores the value of the Fortran MPI constant MPI_STATUS_SIZE. It is used for
   Fortran-C conversions.
 */
int scorep_mpi_status_size = 0;

/**
   address of fortran variable used as reference for MPI_BOTTOM
 */
void* scorep_mpi_fortran_bottom = NULL;

/**
   address of fortran variable used as reference for MPI_IN_PLACE
 */
void* scorep_mpi_fortran_in_place = NULL;

/**
   address of fortran variable used as reference for MPI_STATUS_IGNORE
 */
void* scorep_mpi_fortran_status_ignore = NULL;

/**
   address of fortran variable used as reference for MPI_STATUSES_IGNORE
 */
void* scorep_mpi_fortran_statuses_ignore = NULL;

/**
   address of fortran variable used as reference for MPI_UNWEIGHTED
 */
void* scorep_mpi_fortran_unweighted = NULL;


/**
   External fortran function to retrieve the constant value MPI_STATUS_SIZE defined
   in Fortran MPI. It is used for Fortran-C conversions.
   @param mpi_status_size Address of a variable where the value is to be stored.
 */
extern void
SCOREP_FORTRAN_GET_MPI_STATUS_SIZE( int* status_size );

/**
   External fortran function to trigger a callback which sets MPI_BOTTOM.
 */
extern void
SCOREP_FORTRAN_GET_MPI_BOTTOM( void );

/**
   External fortran function to trigger a callback which sets MPI_IN_PLACE.
 */
extern void
SCOREP_FORTRAN_GET_MPI_IN_PLACE( void );

/**
   External fortran function to trigger a callback which sets MPI_STATUS_IGNORE.
 */
extern void
SCOREP_FORTRAN_GET_MPI_STATUS_IGNORE( void );

/**
   External fortran function to trigger a callback which sets MPI_STATUSES_IGNORE.
 */
extern void
SCOREP_FORTRAN_GET_MPI_STATUSES_IGNORE( void );

/**
   External fortran function to trigger a callback which sets MPI_STATUSES_IGNORE.
 */
extern void
SCOREP_FORTRAN_GET_MPI_UNWEIGHTED( void );

#include "scorep_mpi_confvars.inc.c"

static size_t scorep_mpi_subsystem_id;

/**
   Implementation of the adapter_register function of the @ref SCOREP_Subsystem struct
   for the initialization process of the MPI adapter.
 */
static SCOREP_ErrorCode
scorep_mpi_register( size_t subsystem_id )
{
    UTILS_DEBUG_ENTRY();

    scorep_mpi_subsystem_id = subsystem_id;

    return SCOREP_ConfigRegister( "mpi", scorep_mpi_configs );
}
/**
   Implementation of the subsystem_init function of the @ref SCOREP_Subsystem struct
   for the initialization process of the MPI adapter.
 */
static SCOREP_ErrorCode
scorep_mpi_init_adapter( void )
{
    UTILS_DEBUG_ENTRY();

    /* Set Fortran constants */
    SCOREP_FORTRAN_GET_MPI_STATUS_SIZE( &scorep_mpi_status_size );
#if HAVE( MPI_BOTTOM )
    SCOREP_FORTRAN_GET_MPI_BOTTOM();
#endif
#if HAVE( MPI_IN_PLACE )
    SCOREP_FORTRAN_GET_MPI_IN_PLACE();
#endif
#if HAVE( MPI_STATUS_IGNORE )
    SCOREP_FORTRAN_GET_MPI_STATUS_IGNORE();
#endif
#if HAVE( MPI_STATUSES_IGNORE )
    SCOREP_FORTRAN_GET_MPI_STATUSES_IGNORE();
#endif
#if HAVE( MPI_UNWEIGHTED )
    SCOREP_FORTRAN_GET_MPI_UNWEIGHTED();
#endif

    scorep_mpi_win_init();
    scorep_mpi_register_regions();
    return SCOREP_SUCCESS;
}

/**
   Implementation of the subsystem_init_location function of the @ref SCOREP_Subsystem struct
   for the initialization process of the MPI adapter.
 */
static SCOREP_ErrorCode
scorep_mpi_init_location( SCOREP_Location* locationData )
{
    UTILS_DEBUG_ENTRY();
    return SCOREP_SUCCESS;
}

/**
   Implementation of the adapter_finalize_location function of the @ref SCOREP_Subsystem
   struct for the initialization process of the MPI adapter.
 */
static void
scorep_mpi_finalize_location( SCOREP_Location* locationData )
{
    UTILS_DEBUG_ENTRY();
}

/**
   Implementation of the adapter_finalize function of the @ref SCOREP_Subsystem struct
   for the initialization process of the MPI adapter.
 */
static void
scorep_mpi_finalize( void )
{
    UTILS_DEBUG_ENTRY();

    /* Prevent all further events */
    SCOREP_MPI_EVENT_GEN_OFF();
    scorep_mpi_enabled = 0;

    /* Finalize sub-systems */
    scorep_mpi_win_finalize();
    scorep_mpi_request_finalize();
    scorep_mpi_comm_finalize();

    UTILS_DEBUG_EXIT();
}

/**
   Implementation of the adapter_deregister function of the @ref SCOREP_Subsystem struct
   for the initialization process of the MPI adapter.
 */
static void
scorep_mpi_deregister( void )
{
    int res;

    UTILS_DEBUG_ENTRY();

    /* Finalize MPI.
       MPICH somehow creates some extra processes/threads. If PMPI_Finalize is called
       from the exit handler of SCOREP, these processes also try to execute MPI_Finalize.
       This causes errors, thus, we test if the call to PMPI_Finalize is save.
     */
    PMPI_Initialized( &res );
    if ( res )
    {
        PMPI_Finalized( &res );
        if ( !res )
        {
            SCOREP_FinalizeMppMeasurement();
            PMPI_Finalize();
        }
    }
}

/* The initialization struct for the MPI adapter */
const SCOREP_Subsystem SCOREP_Mpi_Adapter =
{
    "MPI",
    &scorep_mpi_register,
    &scorep_mpi_init_adapter,
    &scorep_mpi_init_location,
    &scorep_mpi_finalize_location,
    &scorep_mpi_finalize,
    &scorep_mpi_deregister
};

/**
   Flag to indicate whether event generation is turned on or off. If
   it is set to 0, events are generated. If it is set to non-zero, no
   events are generated.
 */
bool scorep_mpi_generate_events = true;

/**
   Array of MPI status. It it used to track
   the open requests between MPI calls.
 */
MPI_Status* scorep_my_status_array = 0;

/**
   Size of the status array
 */
int32_t scorep_my_status_array_size = 0;

/**
   Allocates or reallocates the status array of a size @a size. It it used to track
   the open requests between MPI calls.
   @param size Size of the status Array.
   @returns the status array
 */
MPI_Status*
scorep_get_status_array( int32_t size )
{
    if ( scorep_my_status_array_size == 0 )
    {
        /* -- never used: initialize -- */
        scorep_my_status_array = malloc( size * sizeof( MPI_Status ) );
        if ( scorep_my_status_array == NULL )
        {
            UTILS_ERROR_POSIX();
        }
        scorep_my_status_array_size = size;
    }
    else if ( size > scorep_my_status_array_size )
    {
        /* -- not enough room: expand -- */
        scorep_my_status_array = realloc( scorep_my_status_array,
                                          size * sizeof( MPI_Status ) );
        if ( scorep_my_status_array == NULL )
        {
            UTILS_ERROR_POSIX();
        }
        scorep_my_status_array_size = size;
    }
    return scorep_my_status_array;
}
