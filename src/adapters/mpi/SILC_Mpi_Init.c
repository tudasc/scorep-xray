/** @file SILC_Mpi_Init.h
    @author Daniel Lorenz
    @email d.lorenz@fz-juelich.de

    Contains the initialization function implementations for the measurement system.
 */

#include "SILC_Mpi_Init.h"
#include "SILC_Mpi_Reg.h"
#include "SILC_Config.h"

/** Stores the value of the Fortran MPI constant MPI_STATUS_SIZE. It is used for
    Fortran-C conversions.
 */
int silc_mpi_status_size;

/** External fortran function to retrieve the constant value MPI_STATUS_SIZE defined
    in Fortran MPI. It is used for Fortran-C conversions.
    @param mpi_status_size Address of a variable where the value is to be stored.
 */
extern void
silc_mpi_get_status_size( int* status_size );

#include <stdio.h>

const SILC_Adapter SILC_Mpi_Adapter =
{
    SILC_ADAPTER_MPI,
    "MPI",
    &SILC_Mpi_Register,
    &SILC_Mpi_InitAdapter,
    &SILC_Mpi_InitLocation,
    &SILC_Mpi_FinalLocation,
    &SILC_Mpi_Finalize,
    &SILC_Mpi_Deregister
};


/** Contains the configuration string of enabled mpi function groups. It is filled
    by the measurement system after registration of configuration variables.
 */
char** silc_mpi_config_groups = NULL;

/** Array of configuration variables.
    They are registered to the measurement system and are filled during until the
    initialization function is called.
 */
SILC_ConfigVariable silc_mpi_configs[] = {
    {
        "mpi",
        "enable_groups",
        SILC_CONFIG_TYPE_SET,
        ( void* )&silc_mpi_config_groups,
        NULL,
        "DEFAULT",
        "The names of the function groups which are measured.",
        "The names of the function groups which are measured.\nOther functions are not measured.\nPossible groups are:\n All: All MPI functions\n CG: Communicator and group management\n COLL: Collective functions\n DEFAULT: Default configuration\n ENV: Environmental management\n ERR: MPI Error handling\n EXT: External interface functions\n IO: MPI file I/O\n MISC: Miscellaneous\n P2P: Peer-to-peer communication\n RMA: One sided communication\n SPAWN: Process management\n TOPO: Topology\n TYPE: MPI datatype functions\n",
    }
};


SILC_Error_Code
SILC_Mpi_Register
    ()
{
    printf( "In SILC_Mpi_Register\n" );
    SILC_ConfigRegister( &silc_mpi_configs, 1 );
    return SILC_SUCCESS;
}

SILC_Error_Code
SILC_Mpi_InitAdapter
    ()
{
    printf( "In SILC_Mpi_InitAdapter\n" );
    silc_mpi_get_status_size___( &silc_mpi_status_size );
    silc_mpi_register_regions();
    return SILC_SUCCESS;
}

SILC_Error_Code
SILC_Mpi_InitLocation
    ()
{
    printf( "In SILC_Mpi_InitLocation\n" );
    return SILC_SUCCESS;
}

void
SILC_Mpi_FinalLocation
    ()
{
    printf( "In SILC_Mpi_FinalAdapter\n" );
}

void
SILC_Mpi_Finalize
    ()
{
    printf( "In SILC_Mpi_Finalize\n" );
}

void
SILC_Mpi_Deregister
    ()
{
    printf( "In SILC_Mpi_Deregister\n" );
}
