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
scorep_create_experiment_dir( const char* dirName,
                              void        ( * createDir )( const char* ) )
{
    if ( !SCOREP_Mpi_IsInitialized() )
    {
        SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_MPI, "MPI not initialized, experiment directory creation deferred." );
        return false;
    }

    if ( SCOREP_Mpi_GetRank() == 0 )
    {
        createDir( dirName );
    }
    //MPI_Bcast( dirName, dirNameSize, MPI_CHAR, 0, MPI_COMM_WORLD );

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


int*
SCOREP_Mpi_GatherNumberOfLocationsPerRank()
{
    int* n_locations_per_rank = 0;
    if ( SCOREP_Mpi_GetRank() == 0 )
    {
        n_locations_per_rank = calloc( SCOREP_Mpi_GetCommWorldSize(), sizeof( int ) );
        assert( n_locations_per_rank );
    }
    int n_local_locations = SCOREP_Thread_GetNumberOfLocations();
    PMPI_Gather( &n_local_locations,
                 1,
                 MPI_INT,
                 n_locations_per_rank,
                 1,
                 MPI_INT,
                 0,
                 scorep_mpi_comm_world );
    return n_locations_per_rank;
}


int*
SCOREP_Mpi_GatherNumberOfDefinitionsPerLocation( int* nLocationsPerRank,
                                                 int  nGlobalLocations )
{
    /// @todo refactor
    int* n_definitions_per_location = 0;
    int* diplacements               = 0;
    int  rank                       = 0;
    if ( SCOREP_Mpi_GetRank() == 0 )
    {
        // recv buf
        n_definitions_per_location = calloc( nGlobalLocations, sizeof( int ) );
        assert( n_definitions_per_location );

        // displacements
        diplacements = calloc( SCOREP_Mpi_GetCommWorldSize(), sizeof( int ) );
        assert( diplacements );
        int displacement = 0;
        for ( rank = 0; rank < SCOREP_Mpi_GetCommWorldSize(); ++rank )
        {
            diplacements[ rank ] = displacement;
            displacement        += nLocationsPerRank[ rank ];
        }
    }

    // send buf
    int* n_local_definitions = calloc( SCOREP_Thread_GetNumberOfLocations(), sizeof( int ) );
    assert( n_local_definitions );
    int  number_of_definitions = SCOREP_GetNumberOfDefinitions();
    for ( int i = 0; i < SCOREP_Thread_GetNumberOfLocations(); ++i )
    {
        // assign all locations the same number of definitions. This is a temporary solution
        // as we need to duplicate the definitions for every location until OTF2 is able
        // to handle pre-process definitions.
        n_local_definitions[ i ] = number_of_definitions;
    }

    PMPI_Gatherv( n_local_definitions,
                  SCOREP_Thread_GetNumberOfLocations(),
                  MPI_INT,
                  n_definitions_per_location,
                  nLocationsPerRank,
                  diplacements,
                  MPI_INT,
                  0,
                  scorep_mpi_comm_world );

    free( n_local_definitions );
    free( diplacements );

    return n_definitions_per_location;
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
