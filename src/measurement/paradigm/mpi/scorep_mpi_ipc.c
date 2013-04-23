/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
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


/**
 * @file       src/measurement/paradigm/mpi/scorep_mpi_ipc.c
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status alpha
 *
 *
 */

#include <config.h>
#include <scorep_status.h>
#include <scorep_ipc.h>

#include <UTILS_Error.h>

#include <UTILS_Debug.h>
#include <mpi.h>
#include <assert.h>
#include <stdlib.h>


static MPI_Comm comm_world_dup;

static MPI_Datatype mpi_datatypes[ SCOREP_IPC_NUMBER_OF_DATATYPES ];


bool
SCOREP_Ipc_Possible( void )
{
    return true;
}

void
SCOREP_Ipc_Init( void )
{
    assert( SCOREP_Status_IsMppInitialized() );
    assert( !SCOREP_Status_IsMppFinalized() );
    int status = PMPI_Comm_dup( MPI_COMM_WORLD, &comm_world_dup );
    assert( status == MPI_SUCCESS );

/* SCOREP_MPI_INT64_T and SCOREP_MPI_UINT64_T were detected by configure */
#define SCOREP_MPI_BYTE          MPI_BYTE
#define SCOREP_MPI_CHAR          MPI_CHAR
#define SCOREP_MPI_UNSIGNED_CHAR MPI_UNSIGNED_CHAR
#define SCOREP_MPI_INT           MPI_INT
#define SCOREP_MPI_UNSIGNED      MPI_UNSIGNED
#define SCOREP_MPI_DOUBLE        MPI_DOUBLE

#define SCOREP_IPC_DATATYPE( datatype ) \
    mpi_datatypes[ SCOREP_IPC_ ## datatype ] = SCOREP_MPI_ ## datatype;
    SCOREP_IPC_DATATYPES
#undef SCOREP_IPC_DATATYPE

#undef SCOREP_MPI_BYTE
#undef SCOREP_MPI_CHAR
#undef SCOREP_MPI_UNSIGNED_CHAR
#undef SCOREP_MPI_INT
#undef SCOREP_MPI_UNSIGNED
#undef SCOREP_MPI_DOUBLE
}

void
SCOREP_Ipc_Finalize( void )
{
    assert( SCOREP_Status_IsMppInitialized() );
    assert( !SCOREP_Status_IsMppFinalized() );
    /* Free duplicated communicator */
    PMPI_Comm_free( &comm_world_dup );
}

int
SCOREP_Ipc_GetSize( void )
{
    assert( SCOREP_Status_IsMppInitialized() );
    assert( !SCOREP_Status_IsMppFinalized() );
    int size;
    PMPI_Comm_size( comm_world_dup, &size );
    return size;
}


int
SCOREP_Ipc_GetRank( void )
{
    assert( SCOREP_Status_IsMppInitialized() );
    assert( !SCOREP_Status_IsMppFinalized() );
    int rank;
    PMPI_Comm_rank( comm_world_dup, &rank );
    return rank;
}


static inline MPI_Datatype
get_mpi_datatype( SCOREP_Ipc_Datatype datatype )
{
    UTILS_BUG_ON( datatype >= SCOREP_IPC_NUMBER_OF_DATATYPES,
                  "Invalid IPC datatype given" );

    return mpi_datatypes[ datatype ];
}


/**
 * MPI Translation table for ipc operations.
 */
static inline MPI_Op
get_mpi_operation( SCOREP_Ipc_Operation op )
{
    switch ( op )
    {
#define SCOREP_IPC_OPERATION( op ) \
    case SCOREP_IPC_ ## op: \
        return MPI_ ## op;
        SCOREP_IPC_OPERATIONS
#undef SCOREP_IPC_OPERATION
        default:
            UTILS_BUG( "Unknown IPC reduction operation" );
    }

    return MPI_OP_NULL;
}


int
SCOREP_Ipc_Send( void*               buf,
                 int                 count,
                 SCOREP_Ipc_Datatype datatype,
                 int                 dest )
{
    return PMPI_Send( buf,
                      count,
                      get_mpi_datatype( datatype ),
                      dest,
                      0,
                      comm_world_dup ) != MPI_SUCCESS;
}


int
SCOREP_Ipc_Recv( void*               buf,
                 int                 count,
                 SCOREP_Ipc_Datatype datatype,
                 int                 source )
{
    return PMPI_Recv( buf,
                      count,
                      get_mpi_datatype( datatype ),
                      source,
                      0,
                      comm_world_dup,
                      MPI_STATUS_IGNORE ) != MPI_SUCCESS;
}


int
SCOREP_Ipc_Barrier( void )
{
    return PMPI_Barrier( comm_world_dup ) != MPI_SUCCESS;
}


int
SCOREP_Ipc_Bcast( void*               buf,
                  int                 count,
                  SCOREP_Ipc_Datatype datatype,
                  int                 root )
{
    return PMPI_Bcast( buf, count,
                       get_mpi_datatype( datatype ),
                       root,
                       comm_world_dup ) != MPI_SUCCESS;
}


int
SCOREP_Ipc_Gather( void*               sendbuf,
                   void*               recvbuf,
                   int                 count,
                   SCOREP_Ipc_Datatype datatype,
                   int                 root )
{
    return PMPI_Gather( sendbuf, count,
                        get_mpi_datatype( datatype ),
                        recvbuf, count,
                        get_mpi_datatype( datatype ),
                        root,
                        comm_world_dup ) != MPI_SUCCESS;
}


int
SCOREP_Ipc_Gatherv( void*               sendbuf,
                    int                 sendcount,
                    void*               recvbuf,
                    int*                recvcnts,
                    int*                displs,
                    SCOREP_Ipc_Datatype datatype,
                    int                 root )
{
    return PMPI_Gatherv( sendbuf, sendcount,
                         get_mpi_datatype( datatype ),
                         recvbuf, recvcnts, displs,
                         get_mpi_datatype( datatype ),
                         root,
                         comm_world_dup ) != MPI_SUCCESS;
}


int
SCOREP_Ipc_Allgather( void*               sendbuf,
                      void*               recvbuf,
                      int                 count,
                      SCOREP_Ipc_Datatype datatype )
{
    return PMPI_Allgather( sendbuf, count,
                           get_mpi_datatype( datatype ),
                           recvbuf, count,
                           get_mpi_datatype( datatype ),
                           comm_world_dup ) != MPI_SUCCESS;
}


int
SCOREP_Ipc_Reduce( void*                sendbuf,
                   void*                recvbuf,
                   int                  count,
                   SCOREP_Ipc_Datatype  datatype,
                   SCOREP_Ipc_Operation operation,
                   int                  root )
{
    return PMPI_Reduce( sendbuf, recvbuf, count,
                        get_mpi_datatype( datatype ),
                        get_mpi_operation( operation ),
                        root,
                        comm_world_dup ) != MPI_SUCCESS;
}


int
SCOREP_Ipc_Allreduce( void*                sendbuf,
                      void*                recvbuf,
                      int                  count,
                      SCOREP_Ipc_Datatype  datatype,
                      SCOREP_Ipc_Operation operation )
{
    return PMPI_Allreduce( sendbuf, recvbuf, count,
                           get_mpi_datatype( datatype ),
                           get_mpi_operation( operation ),
                           comm_world_dup ) != MPI_SUCCESS;
}


int
SCOREP_Ipc_Scan( void*                sendbuf,
                 void*                recvbuf,
                 int                  count,
                 SCOREP_Ipc_Datatype  datatype,
                 SCOREP_Ipc_Operation operation )
{
    return PMPI_Scan( sendbuf, recvbuf, count,
                      get_mpi_datatype( datatype ),
                      get_mpi_operation( operation ),
                      comm_world_dup ) != MPI_SUCCESS;
}