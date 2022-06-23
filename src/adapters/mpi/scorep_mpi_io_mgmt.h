/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2013, 2016, 2019, 2022,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2015, 2017, 2019,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2013,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2013,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 */

#ifndef SCOREP_MPI_IO_MGMT_H
#define SCOREP_MPI_IO_MGMT_H

#include <scorep/SCOREP_PublicHandles.h>

#include <stdint.h>

#include <mpi.h>

/**
 *  Initializes the MPI I/O tracking management.
 */
void
scorep_mpi_io_init( void );

/**
 *  Finalizes the MPI I/O tracking management.
 */
void
scorep_mpi_io_finalize( void );

/**
 *  Registers an MPI split I/O operation on the given I/O handle.
 *
 *  Only one split operation per handle is allowed by the MPI standard.
 *
 *  @param ioHandle    The I/O andle on which the split operation is happening.
 *  @param matchingId  The matching ID of this split operation.
 *  @param datatype    The MPI datatype of this split operation.
 */
void
scorep_mpi_io_split_begin( SCOREP_IoHandleHandle ioHandle,
                           uint64_t              matchingId,
                           MPI_Datatype          datatype );

/**
 *  Completes an MPI split I/O operation on the given I/O handle.
 *
 *  Aborts measurement if there is no active split operation on @a ioHandle.
 *
 *  @param ioHandle         The I/O andle on which the split operation is happening.
 *  @param[out] matchingId  The matching ID of this split operation previously registered.
 *  @param[out] datatype    The MPI datatype of this split operation previously registered.
 */
void
scorep_mpi_io_split_end( SCOREP_IoHandleHandle ioHandle,
                         uint64_t*             matchingId,
                         MPI_Datatype*         datatype );

#endif /* SCOREP_MPI_IO_MGMT_H */
