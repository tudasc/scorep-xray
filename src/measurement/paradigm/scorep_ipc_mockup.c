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
 * Copyright (c) 2009-2013,
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
 *
 */


/**
 * @file
 *
 *
 * Default IPC implementation for single program paradigms.
 */


#include <config.h>


#include <UTILS_Error.h>


#include <UTILS_Debug.h>


#include "scorep_ipc.h"

#include <stdlib.h>
#include <string.h>


struct SCOREP_Ipc_Group
{
    int dummy;
};

SCOREP_Ipc_Group scorep_ipc_group_world;

void
SCOREP_Ipc_Init( void )
{
    scorep_ipc_group_world.dummy = 0;
}


void
SCOREP_Ipc_Finalize( void )
{
}


SCOREP_Ipc_Group*
SCOREP_Ipc_GetFileGroup( int nProcsPerFile )
{
    return NULL;
}


int
SCOREP_IpcGroup_GetSize( SCOREP_Ipc_Group* group )
{
    return 1;
}


int
SCOREP_IpcGroup_GetRank( SCOREP_Ipc_Group* group )
{
    return 0;
}


int
SCOREP_IpcGroup_Barrier( SCOREP_Ipc_Group* group )
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
    4, /* SCOREP_IPC_INT32_T */
    4, /* SCOREP_IPC_UINT32_T */
    8, /* SCOREP_IPC_INT64_T */
    8, /* SCOREP_IPC_UINT64_T */
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
SCOREP_IpcGroup_Send( SCOREP_Ipc_Group*   group,
                      const void*         buf,
                      int                 count,
                      SCOREP_Ipc_Datatype scorep_datatype,
                      int                 dest )
{
    UTILS_BUG_ON( dest != 0, "SCOREP_Ipc_Send() called in non-mpi build." );

    return 0;
}


int
SCOREP_IpcGroup_Recv( SCOREP_Ipc_Group*   group,
                      void*               buf,
                      int                 count,
                      SCOREP_Ipc_Datatype scorep_datatype,
                      int                 source )
{
    UTILS_BUG_ON( source != 0, "SCOREP_Mpi_Recv() called in non-mpi build." );

    return 0;
}


int
SCOREP_IpcGroup_Bcast( SCOREP_Ipc_Group*   group,
                       void*               buf,
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
SCOREP_IpcGroup_Gather( SCOREP_Ipc_Group*   group,
                        const void*         sendbuf,
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
SCOREP_IpcGroup_Gatherv( SCOREP_Ipc_Group*   group,
                         const void*         sendbuf,
                         int                 sendcount,
                         void*               recvbuf,
                         const int*          recvcnts,
                         SCOREP_Ipc_Datatype datatype,
                         int                 root )
{
    UTILS_BUG_ON( root != 0,
                  "Invalid root given for gather in single process run." );
    UTILS_BUG_ON( sendcount != recvcnts[ 0 ],
                  "Non-matching send and recv count." );

    if ( recvbuf != sendbuf )
    {
        /* In non-mpi case, we have only rank zero. Thus copy sendbuf to recvbuf. */
        size_t num = get_datatype_size( datatype ) * sendcount;
        memcpy( recvbuf, sendbuf, num );
    }

    return 0;
}


int
SCOREP_IpcGroup_Allgather( SCOREP_Ipc_Group*   group,
                           const void*         sendbuf,
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
SCOREP_IpcGroup_Reduce( SCOREP_Ipc_Group*    group,
                        const void*          sendbuf,
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
SCOREP_IpcGroup_Allreduce( SCOREP_Ipc_Group*    group,
                           const void*          sendbuf,
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
SCOREP_IpcGroup_Scatter( SCOREP_Ipc_Group*   group,
                         const void*         sendbuf,
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

int
SCOREP_IpcGroup_Scatterv( SCOREP_Ipc_Group*   group,
                          const void*         sendbuf,
                          const int*          sendcounts,
                          void*               recvbuf,
                          int                 recvcount,
                          SCOREP_Ipc_Datatype datatype,
                          int                 root )
{
    UTILS_BUG_ON( root != 0,
                  "Invalid root given for scatter in single process run." );

    /* In non-mpi case, we have only rank zero. Thus copy sendbuf to recvbuf. */
    if ( recvbuf != sendbuf )
    {
        /* In non-mpi case, we have only rank zero. Thus copy sendbuf to recvbuf. */
        size_t num = get_datatype_size( datatype ) * recvcount;
        memcpy( recvbuf, sendbuf, num );
    }

    return 0;
}

SCOREP_Ipc_Group*
SCOREP_IpcGroup_Split( SCOREP_Ipc_Group* parent,
                       int               color,
                       int               key )
{
    return &scorep_ipc_group_world;
}

void
SCOREP_IpcGroup_Free( SCOREP_Ipc_Group* group )
{
}
