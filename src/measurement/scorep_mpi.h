/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
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

#ifndef SCOREP_MPI_H
#define SCOREP_MPI_H



/**
 * @file       scorep_mpi.h
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status alpha
 *
 */


#include <stdbool.h>
#include <stdint.h>


void
SCOREP_Mpi_SetRankTo( int rank );


/**
 * Get the rank of the process.
 *
 * @return In MPI mode we will get the rank of the process in MPI_COMM_WORLD's
 * group, i.e. MPI_Comm_rank(MPI_COMM_WORLD, &rank). In any other mode we will
 * get 0. Calls to this function before MPI_Init() will fail.
 */
int
SCOREP_Mpi_GetRank();


/**
 * Indicates whether we are a MPI application or not. This eases the
 * initialization process.
 *
 * @return In MPI mode return true, false otherwise.
 */
bool
SCOREP_Mpi_HasMpi();


/**
 * Indicates whether MPI_Init() was already called or not. In non-MPI mode always true
 *
 * @return In MPI mode true if MPI_Init() has been called earlier, false otherwise.
 *         In non-MPI mode always true.
 */
bool
SCOREP_Mpi_IsInitialized();


/**
 * Indicates whether MPI_Finalize() was already called or not. In non-MPI mode always true
 *
 * @return In MPI mode true if MPI_Finalize() has been called before, false otherwise.
 *         In non-MPI mode always true.
 */
bool
SCOREP_Mpi_IsFinalized();


void
SCOREP_Mpi_GlobalBarrier();


void
SCOREP_Mpi_DuplicateCommWorld();


int
SCOREP_Mpi_CalculateCommWorldSize();


int
SCOREP_Mpi_GetCommWorldSize();


int*
SCOREP_Mpi_GatherNumberOfLocationsPerRank();


int*
SCOREP_Mpi_GatherNumberOfDefinitionsPerLocation( int* nLocationsPerRank,
                                                 int  nGlobalLocations );


// list of used mpi datatypes
#define SCOREP_MPI_DATATYPES \
    SCOREP_MPI_DATATYPE( MPI_UNSIGNED_CHAR ) \
    SCOREP_MPI_DATATYPE( MPI_UNSIGNED )

typedef enum SCOREP_Mpi_Datatype
{
    #define SCOREP_MPI_DATATYPE( datatype ) \
    SCOREP_ ## datatype,
    SCOREP_MPI_DATATYPES
    #undef SCOREP_MPI_DATATYPE
} SCOREP_Mpi_Datatype;


typedef void* SCOREP_Mpi_Status;
#define SCOREP_MPI_STATUS_IGNORE NULL


int
SCOREP_Mpi_Send( void*               buf,
                 int                 count,
                 SCOREP_Mpi_Datatype scorep_datatype,
                 int                 dest );


int
SCOREP_Mpi_Recv( void*               buf,
                 int                 count,
                 SCOREP_Mpi_Datatype scorep_datatype,
                 int                 source,
                 SCOREP_Mpi_Status   status );


#endif /* SCOREP_MPI_H */
