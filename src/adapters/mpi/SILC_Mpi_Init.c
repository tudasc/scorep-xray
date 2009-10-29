/** @file SILC_Mpi_Init.h
    @author Daniel Lorenz
    @email d.lorenz@fz-juelich.de

    Contains the initialization function implementations for the measurement system.
 */

#include "SILC_Mpi_Init.h"
#include "SILC_Mpi_Reg.h"

const SILC_Adapter_InitFunctions SILC_Mpi_InitFunctions =
{
    &SILC_Mpi_Register,
    &SILC_Mpi_InitAdapter,
    &SILC_Mpi_InitLocation
};

void
SILC_Mpi_Register
    ()
{
    printf( "In SILC_Mpi_Register\n" );
}

void
SILC_Mpi_InitAdapter
    ()
{
    printf( "In SILC_Mpi_InitAdapter\n" );
}

void
SILC_Mpi_InitLocation
    ()
{
    printf( "In SILC_Mpi_InitAdapter\n" );
}
