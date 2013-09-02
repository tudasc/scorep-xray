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
 * @file       src/measurement/paradigm/scorep_ipc_mockup.c
 *
 *
 * Default IPC implementation for single program paradigms.
 */


#include <config.h>


#include <UTILS_Error.h>


#include <UTILS_Debug.h>


#include "scorep_ipc.h"

#include <stdlib.h>
#include <assert.h>
#include <string.h>


void
SCOREP_Ipc_Init( void )
{
}


void
SCOREP_Ipc_Finalize( void )
{
}


int
SCOREP_Ipc_GetSize( void )
{
    return 1;
}


int
SCOREP_Ipc_GetRank( void )
{
    return 0;
}


int
SCOREP_Ipc_Barrier( void )
{
    return 0;
}


static size_t ipc_datatype_sizes[] =
{
    1, /* SCOREP_IPC_BYTE */
    1, /* SCOREP_IPC_CHAR */
    1, /* SCOREP_IPC_UNSIGNED_CHAR */
    4, /* SCOREP_IPC_INT */
    4, /* SCOREP_IPC_UNSIGNED */
    4, /* SCOREP_IPC_INT32 */
    4, /* SCOREP_IPC_UINT32 */
    8, /* SCOREP_IPC_INT64 */
    8, /* SCOREP_IPC_UINT64 */
    8  /* SCOREP_IPC_DOUBLE */
};


static inline size_t
get_datatype_size( SCOREP_Ipc_Datatype datatype )
{
    UTILS_BUG_ON( datatype >= SCOREP_IPC_NUMBER_OF_DATATYPES,
                  "Invalid IPC datatype given" );

    return ipc_datatype_sizes[ datatype ];
}


int
SCOREP_Ipc_Send( void*               buf,
                 int                 count,
                 SCOREP_Ipc_Datatype scorep_datatype,
                 int                 dest )
{
    UTILS_BUG_ON( dest != 0, "SCOREP_Ipc_Send() called in non-mpi build." );

    return 0;
}


int
SCOREP_Ipc_Recv( void*               buf,
                 int                 count,
                 SCOREP_Ipc_Datatype scorep_datatype,
                 int                 source )
{
    UTILS_BUG_ON( source != 0, "SCOREP_Mpi_Recv() called in non-mpi build." );

    return 0;
}


int
SCOREP_Ipc_Bcast( void*               buf,
                  int                 count,
                  SCOREP_Ipc_Datatype datatype,
                  int                 root )
{
    UTILS_BUG_ON( root != 0,
                  "Invalid root given for broadcast in single process run." );

    /* In non-mpi case there is no other rank to which we send something.
       Thus, nothing to do */
    return 0;
}


int
SCOREP_Ipc_Gather( void*               sendbuf,
                   void*               recvbuf,
                   int                 count,
                   SCOREP_Ipc_Datatype datatype,
                   int                 root )
{
    UTILS_BUG_ON( root != 0,
                  "Invalid root given for gather in single process run." );

    if ( recvbuf != sendbuf )
    {
        /* In non-mpi case, we have only rank zero. Thus copy sendbuf to recvbuf. */
        size_t num = get_datatype_size( datatype ) * count;
        memcpy( recvbuf, sendbuf, num );
    }
    return 0;
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
    UTILS_BUG_ON( root != 0,
                  "Invalid root given for gather in single process run." );
    UTILS_BUG_ON( sendcount != recvcnts[ 0 ],
                  "Non-matching send and recv count." );

    if ( ( ( char* )recvbuf + displs[ 0 ] ) != sendbuf )
    {
        /* In non-mpi case, we have only rank zero. Thus copy sendbuf to recvbuf. */
        size_t num = get_datatype_size( datatype ) * sendcount;
        memcpy( ( char* )recvbuf + displs[ 0 ], sendbuf, num );
    }
    return 0;
}


int
SCOREP_Ipc_Allgather( void*               sendbuf,
                      void*               recvbuf,
                      int                 count,
                      SCOREP_Ipc_Datatype datatype )
{
    /* In non-mpi case, we have only rank zero. Thus copy sendbuf to recvbuf. */
    if ( recvbuf != sendbuf )
    {
        size_t num = get_datatype_size( datatype ) * count;
        memcpy( recvbuf, sendbuf, num );
    }
    return 0;
}


int
SCOREP_Ipc_Reduce( void*                sendbuf,
                   void*                recvbuf,
                   int                  count,
                   SCOREP_Ipc_Datatype  datatype,
                   SCOREP_Ipc_Operation operation,
                   int                  root )
{
    UTILS_BUG_ON( root != 0,
                  "Invalid root given for reduce in single process run." );

    if ( recvbuf != sendbuf )
    {
        /* In non-mpi case, we have only rank zero. Thus all operations just copy sendbuf to
           recvbuf. */
        size_t num = get_datatype_size( datatype ) * count;
        memcpy( recvbuf, sendbuf, num );
    }
    return 0;
}


int
SCOREP_Ipc_Allreduce( void*                sendbuf,
                      void*                recvbuf,
                      int                  count,
                      SCOREP_Ipc_Datatype  datatype,
                      SCOREP_Ipc_Operation operation )
{
    /* In non-mpi case, we have only rank zero. Thus all operations just copy sendbuf to
       recvbuf. */
    if ( recvbuf != sendbuf )
    {
        size_t num = get_datatype_size( datatype ) * count;
        memcpy( recvbuf, sendbuf, num );
    }
    return 0;
}


int
SCOREP_Ipc_Scan( void*                sendbuf,
                 void*                recvbuf,
                 int                  count,
                 SCOREP_Ipc_Datatype  datatype,
                 SCOREP_Ipc_Operation operation )
{
    /* In non-mpi case, we have only rank zero. Thus all operations just copy sendbuf to
       recvbuf. */
    if ( recvbuf != sendbuf )
    {
        size_t num = get_datatype_size( datatype ) * count;
        memcpy( recvbuf, sendbuf, num );
    }
    return 0;
}

int
SCOREP_Ipc_Scatter( void*               sendbuf,
                    void*               recvbuf,
                    int                 count,
                    SCOREP_Ipc_Datatype datatype,
                    int                 root )
{
    UTILS_BUG_ON( root != 0,
                  "Invalid root given for scatter in single process run." );

    /* In non-mpi case, we have only rank zero. Thus copy sendbuf to recvbuf. */
    size_t num = get_datatype_size( datatype ) * count;
    memcpy( recvbuf, sendbuf, num );

    return 0;
}
