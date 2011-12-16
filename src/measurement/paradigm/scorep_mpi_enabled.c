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


/**
 * @file       scorep_mpi_enabled.c
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status alpha
 *
 *
 */

#include <config.h>
#include "scorep_mpi.h"

#include <scorep_utility/SCOREP_Debug.h>
#include <scorep_thread.h>
#include <scorep_definitions.h>
#include <mpi.h>
#include <assert.h>
#include <stdlib.h>


static MPI_Comm scorep_mpi_comm_world;


extern void
scorep_status_initialize_mpi();


extern void
scorep_status_initialize_common();


void
SCOREP_Status_Initialize()
{
    scorep_status_initialize_common();
    scorep_status_initialize_mpi();
    // it is too early to call PMPI from here.
}


bool
scorep_create_experiment_dir( void ( * createDir )( void ) )
{
    if ( !SCOREP_Mpi_IsInitialized() )
    {
        SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_MPI, "MPI not initialized, experiment directory creation deferred." );
        return false;
    }

    if ( SCOREP_Mpi_GetRank() == 0 )
    {
        createDir();
    }

    return true;
}


bool
SCOREP_Mpi_HasMpi()
{
    return true;
}


void
SCOREP_Mpi_GlobalBarrier()
{
    assert( SCOREP_Mpi_IsInitialized() );
    assert( !SCOREP_Mpi_IsFinalized() );
    int status = PMPI_Barrier( scorep_mpi_comm_world );
    assert( status == MPI_SUCCESS );
}


void
SCOREP_Mpi_DuplicateCommWorld()
{
    assert( SCOREP_Mpi_IsInitialized() );
    assert( !SCOREP_Mpi_IsFinalized() );
    int status = PMPI_Comm_dup( MPI_COMM_WORLD, &scorep_mpi_comm_world );
    assert( status == MPI_SUCCESS );
}


int
SCOREP_Mpi_CalculateCommWorldSize()
{
    int size;
    PMPI_Comm_size( scorep_mpi_comm_world, &size );
    return size;
}


static MPI_Datatype
scorep_mpi_to_mpi_datatype( enum SCOREP_Mpi_Datatype scorep_datatype )
{
    switch ( scorep_datatype )
    {
#define SCOREP_MPI_DATATYPE( datatype ) \
    case SCOREP_ ## datatype: \
        return datatype;
        SCOREP_MPI_DATATYPES
#undef SCOREP_MPI_DATATYPE
        default:
            SCOREP_BUG( "Unknown mpi datatype" );
    }
}

/**
 * Translation table for mpi operations. The scorep value serves as index to get the
 * MPI_Op type. Thus, the entries must be in the same order as in
 * enum SCOREP_Mpi_Datatype.
 */
static MPI_Op scorep_mpi_to_mpi_operation[] =
{
    MPI_LAND,
    MPI_LOR,
    MPI_LXOR,
    MPI_BAND,
    MPI_BOR,
    MPI_BXOR,
    MPI_MIN,
    MPI_MAX,
    MPI_SUM,
    MPI_PROD,
};


int
SCOREP_Mpi_Allgather( void*               sendbuf,
                      int                 sendcount,
                      SCOREP_Mpi_Datatype sendtype,
                      void*               recvbuf,
                      int                 recvcount,
                      SCOREP_Mpi_Datatype recvtype )
{
    return PMPI_Allgather( sendbuf, sendcount,
                           scorep_mpi_to_mpi_datatype( sendtype ),
                           recvbuf, recvcount,
                           scorep_mpi_to_mpi_datatype( recvtype ),
                           scorep_mpi_comm_world );
}

int
SCOREP_Mpi_Allreduce( void*                sendbuf,
                      void*                recvbuf,
                      int                  count,
                      SCOREP_Mpi_Datatype  datatype,
                      SCOREP_Mpi_Operation scorep_operation )
{
    return PMPI_Allreduce( sendbuf, recvbuf, count,
                           scorep_mpi_to_mpi_datatype( datatype ),
                           scorep_mpi_to_mpi_operation[ scorep_operation ],
                           scorep_mpi_comm_world );
}

int
SCOREP_Mpi_Barrier()
{
    return PMPI_Barrier( scorep_mpi_comm_world );
}

int
SCOREP_Mpi_Bcast( void*               buf,
                  int                 count,
                  SCOREP_Mpi_Datatype scorep_datatype,
                  int                 root )
{
    return PMPI_Bcast( buf, count, scorep_mpi_to_mpi_datatype( scorep_datatype ), root,
                       scorep_mpi_comm_world );
}

int
SCOREP_Mpi_Gather( void*               sendbuf,
                   int                 sendcount,
                   SCOREP_Mpi_Datatype scorep_sendtype,
                   void*               recvbuf,
                   int                 recvcount,
                   SCOREP_Mpi_Datatype scorep_recvtype,
                   int                 root )
{
    return PMPI_Gather( sendbuf, sendcount, scorep_mpi_to_mpi_datatype( scorep_sendtype ),
                        recvbuf, recvcount, scorep_mpi_to_mpi_datatype( scorep_recvtype ),
                        root, scorep_mpi_comm_world );
}

int
SCOREP_Mpi_Gatherv( void*               sendbuf,
                    int                 sendcount,
                    SCOREP_Mpi_Datatype scorep_sendtype,
                    void*               recvbuf,
                    int*                recvcnts,
                    int*                displs,
                    SCOREP_Mpi_Datatype scorep_recvtype,
                    int                 root )
{
    return PMPI_Gatherv( sendbuf, sendcount,
                         scorep_mpi_to_mpi_datatype( scorep_sendtype ),
                         recvbuf, recvcnts, displs,
                         scorep_mpi_to_mpi_datatype( scorep_recvtype ),
                         root, scorep_mpi_comm_world );
}

int
SCOREP_Mpi_Recv( void*               buf,
                 int                 count,
                 SCOREP_Mpi_Datatype scorep_datatype,
                 int                 source,
                 SCOREP_Mpi_Status   status )
{
    return PMPI_Recv( buf,
                      count,
                      scorep_mpi_to_mpi_datatype( scorep_datatype ),
                      source,
                      0,
                      scorep_mpi_comm_world,
                      status == SCOREP_MPI_STATUS_IGNORE
                      ? MPI_STATUS_IGNORE
                      : ( MPI_Status* )status );
}

int
SCOREP_Mpi_Reduce( void*                sendbuf,
                   void*                recvbuf,
                   int                  count,
                   SCOREP_Mpi_Datatype  scorep_datatype,
                   SCOREP_Mpi_Operation scorep_operation,
                   int                  root )
{
    return PMPI_Reduce( sendbuf, recvbuf, count,
                        scorep_mpi_to_mpi_datatype( scorep_datatype ),
                        scorep_mpi_to_mpi_operation[ scorep_operation ],
                        root, scorep_mpi_comm_world );
}

int
SCOREP_Mpi_Scan( void*                sendbuf,
                 void*                recvbuf,
                 int                  count,
                 SCOREP_Mpi_Datatype  scorep_datatype,
                 SCOREP_Mpi_Operation scorep_operation )
{
    return PMPI_Scan( sendbuf, recvbuf, count,
                      scorep_mpi_to_mpi_datatype( scorep_datatype ),
                      scorep_mpi_to_mpi_operation[ scorep_operation ],
                      scorep_mpi_comm_world );
}

int
SCOREP_Mpi_Send( void*               buf,
                 int                 count,
                 SCOREP_Mpi_Datatype scorep_datatype,
                 int                 dest )
{
    return PMPI_Send( buf,
                      count,
                      scorep_mpi_to_mpi_datatype( scorep_datatype ),
                      dest,
                      0,
                      scorep_mpi_comm_world );
}
