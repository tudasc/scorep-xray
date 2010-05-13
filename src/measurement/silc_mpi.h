#ifndef SILC_INTERNAL_MPI_H
#define SILC_INTERNAL_MPI_H

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
 * @file silc_mpi.h Declarations of wrappers around MPI functions that may be
 * used in non-MPI cases also. There will be two implementations,
 * silc_mpi_enabled.c and silc_mpi_disabled.c
 *
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status ALPHA
 *
 */

#include <stdint.h>
#include <stdbool.h>

/**
 * Get the rank of the process.
 *
 * @return In MPI mode we will get the rank of the process in MPI_COMM_WORLD's
 * group, i.e. MPI_Comm_rank(MPI_COMM_WORLD, &rank). In any other mode we will
 * get 0.
 */
uint64_t
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
 * Indicates whether MPI_Init was already called or not. In non-MPI always true
 *
 * @return In MPI mode true if MPI_Init has been called before, false otherwise.
 *         In non-MPI mode always true.
 */
bool
SILC_Mpi_IsInitialized();


#endif /* SILC_INTERNAL_MPI_H */
