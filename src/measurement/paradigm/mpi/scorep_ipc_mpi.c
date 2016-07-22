/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2014,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2013, 2015,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2013,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2013,
 * Technische Universitaet Muenchen, Germany
 *
 * Copyright (c) 2016,
 * Technische Universitaet Darmstadt, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */


/**
 * @file
 *
 *
 *
 */

#include <config.h>
#include <scorep_status.h>
#include <scorep_ipc.h>

#include <UTILS_Error.h>
#include <SCOREP_Memory.h>
#include <UTILS_Debug.h>
#include <mpi.h>
#include <assert.h>
#include <stdlib.h>

#if MPI_VERSION >= 3
#define HANDLE_CONST( type, arg ) arg
#else
#define HANDLE_CONST( type, arg ) ( type* )arg
#endif

struct SCOREP_Ipc_Group
{
    MPI_Comm          comm;
    SCOREP_Ipc_Group* next;
};

/**
 * List of unused objects from freed groups.
 */
SCOREP_Ipc_Group* free_ipc_groups = NULL;


SCOREP_Ipc_Group        scorep_ipc_group_world;
static SCOREP_Ipc_Group file_group;

static inline MPI_Comm
resolve_comm( SCOREP_Ipc_Group* group )
{
    return group ? group->comm : scorep_ipc_group_world.comm;
}


static MPI_Datatype mpi_datatypes[ SCOREP_IPC_NUMBER_OF_DATATYPES ];


void
SCOREP_Ipc_Init( void )
{
    assert( SCOREP_Status_IsMppInitialized() );
    assert( !SCOREP_Status_IsMppFinalized() );
    int status = PMPI_Comm_dup( MPI_COMM_WORLD, &scorep_ipc_group_world.comm );
    assert( status == MPI_SUCCESS );

    file_group.comm = MPI_COMM_NULL;

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
    PMPI_Comm_free( &scorep_ipc_group_world.comm );
    if ( MPI_COMM_NULL != file_group.comm )
    {
        PMPI_Comm_free( &file_group.comm );
    }
}


SCOREP_Ipc_Group*
SCOREP_Ipc_GetFileGroup( int nProcsPerFile )
{
    if ( MPI_COMM_NULL == file_group.comm )
    {
        UTILS_BUG_ON( 0 == nProcsPerFile,
                      "Invalid value for number of procs per file: %d",
                      nProcsPerFile );

        #if defined( __bgp__ ) || defined( __bgq__ )

        /* MPIX_Pset_same_comm_create returns a communicator which contains
           only MPI ranks which run on nodes belonging to the same I/O node.
           For each I/O node we will create one SION file that stores the
           logical files of all MPI ranks in this communicator. */
        MPIX_Pset_same_comm_create( &file_group.comm );

        #else

        int size = SCOREP_Ipc_GetSize();
        int rank = SCOREP_Ipc_GetRank();

        /* we need at least that number of files */
        int number_of_files = size / nProcsPerFile + !!( size % nProcsPerFile );

        /* distribute the ranks evenly into the files */
        int file_number = 0;
        int rem         = size % number_of_files;
        int local_size  = size / number_of_files + !!rem;
        int local_rank  = 0;
        int local_root  = 0;
        for ( int i = 0; i < rank; i++ )
        {
            local_rank++;
            if ( local_root + local_size == i + 1 )
            {
                local_root += local_size;
                file_number++;
                local_size -= file_number == rem;
                local_rank  = 0;
            }
        }

        PMPI_Comm_split( scorep_ipc_group_world.comm,
                         file_number,
                         local_rank,
                         &file_group.comm );

        #endif
    }

    return &file_group;
}

SCOREP_Ipc_Group*
SCOREP_IpcGroup_Split( SCOREP_Ipc_Group* parent,
                       int               color,
                       int               key )
{
    SCOREP_Ipc_Group* new_group;

    /* Assume that IPC operations are serialized and need no locking */
    if ( free_ipc_groups != NULL )
    {
        new_group       = free_ipc_groups;
        free_ipc_groups = new_group->next;
    }
    else
    {
        new_group = SCOREP_Memory_AllocForMisc( sizeof( SCOREP_Ipc_Group ) );
    }
    UTILS_ASSERT( new_group );
    PMPI_Comm_split( parent->comm, color, key, &new_group->comm );
    return new_group;
}

void
SCOREP_IpcGroup_Free( SCOREP_Ipc_Group* group )
{
    PMPI_Comm_free( &group->comm );

    /* Assume that IPC operations are serialized and need no locking */
    group->next     = free_ipc_groups;
    free_ipc_groups = group;
}

int
SCOREP_IpcGroup_GetSize( SCOREP_Ipc_Group* group )
{
    assert( SCOREP_Status_IsMppInitialized() );
    assert( !SCOREP_Status_IsMppFinalized() );
    int size;
    PMPI_Comm_size( resolve_comm( group ), &size );
    return size;
}


int
SCOREP_IpcGroup_GetRank( SCOREP_Ipc_Group* group )
{
    assert( SCOREP_Status_IsMppInitialized() );
    assert( !SCOREP_Status_IsMppFinalized() );
    int rank;
    PMPI_Comm_rank( resolve_comm( group ), &rank );
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
            UTILS_BUG( "Unknown IPC reduction operation: %u", op );
    }

    return MPI_OP_NULL;
}


int
SCOREP_IpcGroup_Send( SCOREP_Ipc_Group*   group,
                      const void*         buf,
                      int                 count,
                      SCOREP_Ipc_Datatype datatype,
                      int                 dest )
{
    return PMPI_Send( HANDLE_CONST( void, buf ),
                      count,
                      get_mpi_datatype( datatype ),
                      dest,
                      0,
                      resolve_comm( group ) ) != MPI_SUCCESS;
}


int
SCOREP_IpcGroup_Recv( SCOREP_Ipc_Group*   group,
                      void*               buf,
                      int                 count,
                      SCOREP_Ipc_Datatype datatype,
                      int                 source )
{
    return PMPI_Recv( buf,
                      count,
                      get_mpi_datatype( datatype ),
                      source,
                      0,
                      resolve_comm( group ),
                      MPI_STATUS_IGNORE ) != MPI_SUCCESS;
}


int
SCOREP_IpcGroup_Barrier( SCOREP_Ipc_Group* group )
{
    return PMPI_Barrier( resolve_comm( group ) ) != MPI_SUCCESS;
}


int
SCOREP_IpcGroup_Bcast( SCOREP_Ipc_Group*   group,
                       void*               buf,
                       int                 count,
                       SCOREP_Ipc_Datatype datatype,
                       int                 root )
{
    return PMPI_Bcast( buf, count,
                       get_mpi_datatype( datatype ),
                       root,
                       resolve_comm( group ) ) != MPI_SUCCESS;
}


int
SCOREP_IpcGroup_Gather( SCOREP_Ipc_Group*   group,
                        const void*         sendbuf,
                        void*               recvbuf,
                        int                 count,
                        SCOREP_Ipc_Datatype datatype,
                        int                 root )
{
    return PMPI_Gather( HANDLE_CONST( void, sendbuf ),
                        count,
                        get_mpi_datatype( datatype ),
                        recvbuf,
                        count,
                        get_mpi_datatype( datatype ),
                        root,
                        resolve_comm( group ) ) != MPI_SUCCESS;
}


int
SCOREP_IpcGroup_Gatherv( SCOREP_Ipc_Group*   group,
                         const void*         sendbuf,
                         int                 sendcount,
                         void*               recvbuf,
                         const int*          recvcnts,
                         SCOREP_Ipc_Datatype datatype,
                         int                 root )
{
    int* displs = NULL;
    int  rank   = SCOREP_IpcGroup_GetRank( group );
    if ( root == rank )
    {
        int size = SCOREP_IpcGroup_GetSize( group );
        displs = calloc( size, sizeof( *displs ) );
        UTILS_ASSERT( displs );

        int total = 0;
        for ( int i = 0; i < size; i++ )
        {
            displs[ i ] = total;
            total      += recvcnts[ i ];
        }
    }

    int ret = PMPI_Gatherv( HANDLE_CONST( void, sendbuf ),
                            sendcount,
                            get_mpi_datatype( datatype ),
                            recvbuf,
                            HANDLE_CONST( int, recvcnts ),
                            displs,
                            get_mpi_datatype( datatype ),
                            root,
                            resolve_comm( group ) ) != MPI_SUCCESS;

    free( displs );

    return ret;
}


int
SCOREP_IpcGroup_Allgather( SCOREP_Ipc_Group*   group,
                           const void*         sendbuf,
                           void*               recvbuf,
                           int                 count,
                           SCOREP_Ipc_Datatype datatype )
{
    return PMPI_Allgather( HANDLE_CONST( void, sendbuf ),
                           count,
                           get_mpi_datatype( datatype ),
                           recvbuf,
                           count,
                           get_mpi_datatype( datatype ),
                           resolve_comm( group ) ) != MPI_SUCCESS;
}


int
SCOREP_IpcGroup_Reduce( SCOREP_Ipc_Group*    group,
                        const void*          sendbuf,
                        void*                recvbuf,
                        int                  count,
                        SCOREP_Ipc_Datatype  datatype,
                        SCOREP_Ipc_Operation operation,
                        int                  root )
{
    return PMPI_Reduce( HANDLE_CONST( void, sendbuf ),
                        recvbuf,
                        count,
                        get_mpi_datatype( datatype ),
                        get_mpi_operation( operation ),
                        root,
                        resolve_comm( group ) ) != MPI_SUCCESS;
}


int
SCOREP_IpcGroup_Allreduce( SCOREP_Ipc_Group*    group,
                           const void*          sendbuf,
                           void*                recvbuf,
                           int                  count,
                           SCOREP_Ipc_Datatype  datatype,
                           SCOREP_Ipc_Operation operation )
{
    return PMPI_Allreduce( HANDLE_CONST( void, sendbuf ),
                           recvbuf,
                           count,
                           get_mpi_datatype( datatype ),
                           get_mpi_operation( operation ),
                           resolve_comm( group ) ) != MPI_SUCCESS;
}


int
SCOREP_IpcGroup_Scatter( SCOREP_Ipc_Group*   group,
                         const void*         sendbuf,
                         void*               recvbuf,
                         int                 count,
                         SCOREP_Ipc_Datatype datatype,
                         int                 root )
{
    return PMPI_Scatter( HANDLE_CONST( void, sendbuf ),
                         count,
                         get_mpi_datatype( datatype ),
                         recvbuf,
                         count,
                         get_mpi_datatype( datatype ),
                         root,
                         resolve_comm( group ) ) != MPI_SUCCESS;
}


int
SCOREP_IpcGroup_Scatterv( SCOREP_Ipc_Group*   group,
                          const void*         sendbuf,
                          const int*          sendcounts,
                          void*               recvbuf,
                          int                 recvcount,
                          SCOREP_Ipc_Datatype datatype,
                          int                 root )
{
    int* displs = NULL;
    int  rank   = SCOREP_IpcGroup_GetRank( group );
    if ( root == rank )
    {
        int size = SCOREP_IpcGroup_GetSize( group );
        displs = calloc( size, sizeof( *displs ) );
        UTILS_ASSERT( displs );

        int total = 0;
        for ( int i = 0; i < size; i++ )
        {
            displs[ i ] = total;
            total      += sendcounts[ i ];
        }
    }

    int ret =  PMPI_Scatterv( HANDLE_CONST( void, sendbuf ),
                              HANDLE_CONST( int, sendcounts ),
                              displs,
                              get_mpi_datatype( datatype ),
                              recvbuf,
                              recvcount,
                              get_mpi_datatype( datatype ),
                              root,
                              resolve_comm( group ) ) != MPI_SUCCESS;

    free( displs );

    return ret;
}
