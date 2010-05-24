#ifndef SILC_STATUS_H
#define SILC_STATUS_H

/*
 * This file is part of the SILC project (http://www.silc.de)
 *
 * Copyright (c) 2009-2011,
 *    RWTH Aachen, Germany
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *    Technische Universitaet Dresden, Germany
 *    University of Oregon, Eugene, USA
 *    Forschungszentrum Juelich GmbH, Germany
 *    Technische Universitaet Muenchen, Germany
 *
 * See the COPYING file in the package base directory for details.
 *
 */


/**
 * @file       silc_status.h
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status alpha
 *
 */


#include <stdbool.h>


// different impl for MPI and non-MPI
void
SILC_Status_Initialize();


void
SILC_Mpi_SetIsInitialized();


void
SILC_Mpi_SetIsFinalized();


void
SILC_Mpi_SetRankTo( int rank );


/**
 * Get the rank of the process.
 *
 * @return In MPI mode we will get the rank of the process in MPI_COMM_WORLD's
 * group, i.e. MPI_Comm_rank(MPI_COMM_WORLD, &rank). In any other mode we will
 * get 0. Calls to this function before MPI_Init() will fail.
 */
int
SILC_Mpi_GetRank();


/**
 * Indicates whether we are a MPI application or not. This eases the
 * initialization process.
 *
 * @return In MPI mode return true, false otherwise.
 */
bool
SILC_Mpi_HasMpi();


/**
 * Indicates whether MPI_Init() was already called or not. In non-MPI mode always true
 *
 * @return In MPI mode true if MPI_Init() has been called earlier, false otherwise.
 *         In non-MPI mode always true.
 */
bool
SILC_Mpi_IsInitialized();


/**
 * Indicates whether MPI_Finalize() was already called or not. In non-MPI mode always true
 *
 * @return In MPI mode true if MPI_Finalize() has been called before, false otherwise.
 *         In non-MPI mode always true.
 */
bool
SILC_Mpi_IsFinalized();


bool
SILC_IsTracingEnabled();


bool
SILC_IsProfilingEnabled();


void
SILC_Otf2_SetHasFlushed();


bool
SILC_Otf2_HasFlushed();


bool
SILC_IsExperimentDirCreated();


void
SILC_SetExperimentDirIsCreated();

#endif /* SILC_STATUS_H */
