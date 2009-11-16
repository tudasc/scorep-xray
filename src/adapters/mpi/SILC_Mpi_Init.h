/** @file SILC_Mpi_Init.h
    @author Daniel Lorenz
    @email d.lorenz@fz-juelich.de

    Contains the initialization function definitions for the measurement system.
 */

#ifndef SILC_MPI_INIT_H
#define SILC_MPI_INIT_H

#include "SILC_Adapter.h"

/** The register function */
SILC_Error_Code
SILC_Mpi_Register
    ();

/** The initialization function */
SILC_Error_Code
SILC_Mpi_InitAdapter
    ();

/** The intialization of location functions */
SILC_Error_Code
SILC_Mpi_InitLocation
    ();

/** The finalization of location specifics */
void
SILC_Mpi_FinalLocation
    ();

/** Finalization of the MPI adapter */
void
SILC_Mpi_Finalize
    ();

/** Deregistration of MPI adapter */
void
SILC_Mpi_Deregister
    ();

/** The struct which contain the init functions */
extern const SILC_Adapter SILC_Mpi_Adapter;

#endif // SILC_MPI_INIT_H
