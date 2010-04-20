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
 * @file       SILC_Mpi_Init.c
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 * @status     ALPHA
 * @ingroup    MPI_Wrapper
 *
 * @brief Contains the initialization function implementations for the measurement system.
 */

#include "SILC_Mpi_Init.h"
#include "SILC_Mpi_Reg.h"
#include "SILC_Config.h"
#include "SILC_Mpi.h"

#include <stdio.h>

/** Stores the value of the Fortran MPI constant MPI_STATUS_SIZE. It is used for
    Fortran-C conversions.
 */
int silc_mpi_status_size;

/** External fortran function to retrieve the constant value MPI_STATUS_SIZE defined
    in Fortran MPI. It is used for Fortran-C conversions.
    @param mpi_status_size Address of a variable where the value is to be stored.
 */
extern void
silc_mpi_get_status_size___( int* status_size );

/* Mapping of string keys to enabling group IDs
 * @note The values are sorted in decreasing order, to beautify the debug
 * output. Ie.: if all groups are enabled we get "ALL", because it matches first.
 */
static const SILC_ConfigType_SetEntry silc_mpi_enable_groups[] = {
    { "ALL",     SILC_MPI_ENABLED_ALL     },
    { "DEFAULT", SILC_MPI_ENABLED_DEFAULT },
    { "TYPE",    SILC_MPI_ENABLED_TYPE    },
    { "TOPO",    SILC_MPI_ENABLED_TOPO    },
    { "SPAWN",   SILC_MPI_ENABLED_SPAWN   },
    { "RMA",     SILC_MPI_ENABLED_RMA     },
    { "P2P",     SILC_MPI_ENABLED_P2P     },
    { "MISC",    SILC_MPI_ENABLED_MISC    },
    { "IO",      SILC_MPI_ENABLED_IO      },
    { "EXT",     SILC_MPI_ENABLED_EXT     },
    { "ERR",     SILC_MPI_ENABLED_ERR     },
    { "ENV",     SILC_MPI_ENABLED_ENV     },
    { "COLL",    SILC_MPI_ENABLED_COLL    },
    { "CG",      SILC_MPI_ENABLED_CG      },
    { NULL,      0                        }
};


/** Array of configuration variables.
    They are registered to the measurement system and are filled during until the
    initialization function is called.
 */
SILC_ConfigVariable silc_mpi_configs[] = {
    {
        "enable_groups",
        SILC_CONFIG_TYPE_BITSET,
        &silc_mpi_enabled,
        ( void* )silc_mpi_enable_groups,
        "DEFAULT",
        "The names of the function groups which are measured.",
        "The names of the function groups which are measured.\nOther functions are not measured.\nPossible groups are:\n All: All MPI functions\n CG: Communicator and group management\n COLL: Collective functions\n DEFAULT: Default configuration\n ENV: Environmental management\n ERR: MPI Error handling\n EXT: External interface functions\n IO: MPI file I/O\n MISC: Miscellaneous\n P2P: Peer-to-peer communication\n RMA: One sided communication\n SPAWN: Process management\n TOPO: Topology\n TYPE: MPI datatype functions\n",
    },
    SILC_CONFIG_TERMINATOR
};

/** Implementation of the adapter_register function of the @ref SILC_Adapter struct
    for the initialization process of the MPI adapter.
 */
SILC_Error_Code
silc_mpi_register
    ()
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_MPI | SILC_DEBUG_FUNCTION_ENTRY,
                       "In silc_mpi_register\n" );
    SILC_ConfigRegister( "mpi", silc_mpi_configs );
    return SILC_SUCCESS;
}

/** Implementation of the adapter_init function of the @ref SILC_Adapter struct
    for the initialization process of the MPI adapter.
 */
SILC_Error_Code
silc_mpi_init_adapter
    ()
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_MPI | SILC_DEBUG_FUNCTION_ENTRY,
                       "In silc_mpi_init_adapter\n" );
    silc_mpi_get_status_size___( &silc_mpi_status_size );
    silc_mpi_register_regions();
    return SILC_SUCCESS;
}

/** Implementation of the adapter_init_location function of the @ref SILC_Adapter struct
    for the initialization process of the MPI adapter.
 */
SILC_Error_Code
silc_mpi_init_location
    ()
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_MPI | SILC_DEBUG_FUNCTION_ENTRY,
                       "In silc_mpi_init_location\n" );
    return SILC_SUCCESS;
}

/** Implementation of the adapter_finalize_location function of the @ref SILC_Adapter
    struct for the initialization process of the MPI adapter.
 */
void
silc_mpi_final_location
    ()
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_MPI | SILC_DEBUG_FUNCTION_ENTRY,
                       "In silc_mpi_final_location\n" );
}

/** Implementation of the adapter_finalize function of the @ref SILC_Adapter struct
    for the initialization process of the MPI adapter.
 */
void
silc_mpi_finalize
    ()
{
    int res;

    SILC_DEBUG_PRINTF( SILC_DEBUG_MPI | SILC_DEBUG_FUNCTION_ENTRY,
                       "In silc_mpi_finalize\n" );
    SILC_MPI_EVENT_GEN_OFF();

    /* MPICH somehow creates some extra processes/threads. If PMPI_Finalize is called
       from the exit handler of SILC, these processes also try to execute MPI_Finalize.
       This causes errors, thus, we test if the call to PMPI_Finalize is save.
     */
    PMPI_Initialized( &res );
    if ( res )
    {
        PMPI_Finalized( &res );
        if ( !res )
        {
            PMPI_Finalize();
        }
    }
    SILC_MPI_EVENT_GEN_ON();
    SILC_DEBUG_PRINTF( SILC_DEBUG_MPI | SILC_DEBUG_FUNCTION_ENTRY,
                       "End of silc_mpi_finalize\n" );
}

/** Implementation of the adapter_deregister function of the @ref SILC_Adapter struct
    for the initialization process of the MPI adapter.
 */
void
silc_mpi_deregister
    ()
{
    SILC_DEBUG_PRINTF( SILC_DEBUG_MPI | SILC_DEBUG_FUNCTION_ENTRY,
                       "In silc_mpi_deregister\n" );
}

/* The initialization struct for the MPI adapter */
const SILC_Adapter SILC_Mpi_Adapter =
{
    SILC_ADAPTER_MPI,
    "MPI",
    &silc_mpi_register,
    &silc_mpi_init_adapter,
    &silc_mpi_init_location,
    &silc_mpi_final_location,
    &silc_mpi_finalize,
    &silc_mpi_deregister
};

/** Flag to indicate whether event generation is turned on or off. If it is set to 0,
    events are generated. If it is set to non-zero, no events are generated.
 */
int32_t silc_mpi_nogen = 0;

/** Array of MPI status. It it used to track
    the open requests between MPI calls.
 */
MPI_Status* silc_my_status_array = 0;

/** Size of the status array */
int32_t silc_my_status_array_size = 0;

/** Allocates or reallocates the status array of a size @a size. It it used to track
    the open requests between MPI calls.
    @param size Size of the status Array.
    @returns the status array
 */
MPI_Status*
silc_get_status_array( int32_t size )
{
    if ( silc_my_status_array_size == 0 )
    {
        /* -- never used: initialize -- */
        silc_my_status_array = malloc( size * sizeof( MPI_Status ) );
        if ( silc_my_status_array == NULL )
        {
            SILC_ERROR_POSIX();
        }
        silc_my_status_array_size = size;
    }
    else if ( size > silc_my_status_array_size )
    {
        /* -- not enough room: expand -- */
        silc_my_status_array = realloc( silc_my_status_array,
                                        size * sizeof( MPI_Status ) );
        if ( silc_my_status_array == NULL )
        {
            SILC_ERROR_POSIX();
        }
        silc_my_status_array_size = size;
    }
    return silc_my_status_array;
}
