/** @file SILC_Mpi_Init.h
    @author Daniel Lorenz
    @email d.lorenz@fz-juelich.de

    Contains the initialization function definitions for the measurement system.

    The measurement system calls three adapter functions during initializtaion. First,
    SILC_Mpi_Register is called, which should retrieve the configuration from the
    measurement system. Second, SILC_Mpi_InitAdapter is called, which should initialize
    the adapter itself. Finally, SILC_Mpi_InitLocation is called.
 */

#ifndef SILC_MPI_INIT_H
#define SILC_MPI_INIT_H

/** The register function */
void
SILC_Mpi_Register
    ();

/** The initialization function */
void
SILC_Mpi_InitAdapter
    ();

/** The intialization of location functions */
void
SILC_Mpi_InitLocation
    ();

/** The struct which contain the init functions */
extern const SILC_Adapter_InitFunctions SILC_Mpi_InitFunctions;

#endif // SILC_MPI_INIT_H
