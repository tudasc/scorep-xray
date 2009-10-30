/** @file SILC_Mpi_Init.h
    @author Daniel Lorenz
    @email d.lorenz@fz-juelich.de

    Contains the initialization function implementations for the measurement system.
 */

#include "SILC_Mpi_Init.h"

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

SILC_Error_Code
SILC_Mpi_Register
    ()
{
    printf( "In SILC_Mpi_Register\n" );
    return SILC_SUCCESS;
}

SILC_Error_Code
SILC_Mpi_InitAdapter
    ()
{
    printf( "In SILC_Mpi_InitAdapter\n" );
    return SILC_SUCCESS;
}

SILC_Error_Code
SILC_Mpi_InitLocation
    ()
{
    printf( "In SILC_Mpi_InitAdapter\n" );
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
