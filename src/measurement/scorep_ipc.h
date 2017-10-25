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
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

#ifndef SCOREP_INTERNAL_IPC_H
#define SCOREP_INTERNAL_IPC_H



/**
 * @file
 *
 *
 */

#include <scorep/SCOREP_PublicTypes.h>
#include <stdbool.h>
#include <stdint.h>


/**
 * IPC implementation specific datatype for different communication groups
 */
typedef struct SCOREP_Ipc_Group SCOREP_Ipc_Group;

/**
 *  IPC Group which represents all processes.
 *
 */
extern SCOREP_Ipc_Group scorep_ipc_group_world;
#define SCOREP_IPC_GROUP_WORLD ( &scorep_ipc_group_world )


/**
 *  Initializes the measurements own multi-process communication layer.
 *  Must be called after the multi-process paradigm was initialized.
 */
void
SCOREP_Ipc_Init( void );


/**
 *  Finalizes the measurements own multi-process communication layer.
 *  Must be called before the multi-process paradigm will be finalized.
 */
void
SCOREP_Ipc_Finalize( void );


/**
 *  Returns a communication group used for the SION substrate in OTF2 which
 *  determines which ranks write into the same physical file.
 *
 *  This function is always called with the same value for @a nProcsPerFile.
 *  And the callee should always return the same object, not creating a new
 *  one for each call. Thus the callee should cache the result of this call
 *  and should destroy the object in SCOREP_Ipc_Finalize().
 *
 *  If the paradigm does not support communication sub groups, it should just
 *  return NULL.
 */
SCOREP_Ipc_Group*
SCOREP_Ipc_GetFileGroup( int nProcsPerFile );


/**
 *  Get the number of processes in this parallel program.
 */
int
SCOREP_IpcGroup_GetSize( SCOREP_Ipc_Group* group );


static inline int
SCOREP_Ipc_GetSize( void )
{
    return SCOREP_IpcGroup_GetSize( SCOREP_IPC_GROUP_WORLD );
}


/**
 * Get the rank of the process.
 *
 * @return In MPI mode we will get the rank of the process in MPI_COMM_WORLD's
 * group, i.e. MPI_Comm_rank(MPI_COMM_WORLD, &rank). In any other mode we will
 * get 0. Calls to this function before MPI_Init() will fail.
 */
int
SCOREP_IpcGroup_GetRank( SCOREP_Ipc_Group* group );


static inline int
SCOREP_Ipc_GetRank( void )
{
    return SCOREP_IpcGroup_GetRank( SCOREP_IPC_GROUP_WORLD );
}


int
SCOREP_IpcGroup_Send( SCOREP_Ipc_Group*   group,
                      const void*         buf,
                      int                 count,
                      SCOREP_Ipc_Datatype datatype,
                      int                 dest );


static inline int
SCOREP_Ipc_Send( const void*         buf,
                 int                 count,
                 SCOREP_Ipc_Datatype datatype,
                 int                 dest )
{
    return SCOREP_IpcGroup_Send( SCOREP_IPC_GROUP_WORLD,
                                 buf,
                                 count,
                                 datatype,
                                 dest );
}


int
SCOREP_IpcGroup_Recv( SCOREP_Ipc_Group*   group,
                      void*               buf,
                      int                 count,
                      SCOREP_Ipc_Datatype datatype,
                      int                 source );


static inline int
SCOREP_Ipc_Recv( void*               buf,
                 int                 count,
                 SCOREP_Ipc_Datatype datatype,
                 int                 source )
{
    return SCOREP_IpcGroup_Recv( SCOREP_IPC_GROUP_WORLD,
                                 buf,
                                 count,
                                 datatype,
                                 source );
}


int
SCOREP_IpcGroup_Barrier( SCOREP_Ipc_Group* group );


static inline int
SCOREP_Ipc_Barrier( void )
{
    return SCOREP_IpcGroup_Barrier( SCOREP_IPC_GROUP_WORLD );
}


int
SCOREP_IpcGroup_Bcast( SCOREP_Ipc_Group*   group,
                       void*               buf,
                       int                 count,
                       SCOREP_Ipc_Datatype datatype,
                       int                 root );


static inline int
SCOREP_Ipc_Bcast( void*               buf,
                  int                 count,
                  SCOREP_Ipc_Datatype datatype,
                  int                 root )
{
    return SCOREP_IpcGroup_Bcast( SCOREP_IPC_GROUP_WORLD,
                                  buf,
                                  count,
                                  datatype,
                                  root );
}


int
SCOREP_IpcGroup_Gather( SCOREP_Ipc_Group*   group,
                        const void*         sendbuf,
                        void*               recvbuf,
                        int                 count,
                        SCOREP_Ipc_Datatype datatype,
                        int                 root );


static inline int
SCOREP_Ipc_Gather( const void*         sendbuf,
                   void*               recvbuf,
                   int                 count,
                   SCOREP_Ipc_Datatype datatype,
                   int                 root )
{
    return SCOREP_IpcGroup_Gather( SCOREP_IPC_GROUP_WORLD,
                                   sendbuf,
                                   recvbuf,
                                   count,
                                   datatype,
                                   root );
}


int
SCOREP_IpcGroup_Gatherv( SCOREP_Ipc_Group*   group,
                         const void*         sendbuf,
                         int                 sendcount,
                         void*               recvbuf,
                         const int*          recvcnts,
                         SCOREP_Ipc_Datatype datatype,
                         int                 root );


static inline int
SCOREP_Ipc_Gatherv( const void*         sendbuf,
                    int                 sendcount,
                    void*               recvbuf,
                    const int*          recvcnts,
                    SCOREP_Ipc_Datatype datatype,
                    int                 root )
{
    return SCOREP_IpcGroup_Gatherv( SCOREP_IPC_GROUP_WORLD,
                                    sendbuf,
                                    sendcount,
                                    recvbuf,
                                    recvcnts,
                                    datatype,
                                    root );
}


int
SCOREP_IpcGroup_Allgather( SCOREP_Ipc_Group*   group,
                           const void*         sendbuf,
                           void*               recvbuf,
                           int                 count,
                           SCOREP_Ipc_Datatype datatype );


static inline int
SCOREP_Ipc_Allgather( const void*         sendbuf,
                      void*               recvbuf,
                      int                 count,
                      SCOREP_Ipc_Datatype datatype )
{
    return SCOREP_IpcGroup_Allgather( SCOREP_IPC_GROUP_WORLD,
                                      sendbuf,
                                      recvbuf,
                                      count,
                                      datatype );
}


int
SCOREP_IpcGroup_Reduce( SCOREP_Ipc_Group*    group,
                        const void*          sendbuf,
                        void*                recvbuf,
                        int                  count,
                        SCOREP_Ipc_Datatype  datatype,
                        SCOREP_Ipc_Operation operation,
                        int                  root );


static inline int
SCOREP_Ipc_Reduce( const void*          sendbuf,
                   void*                recvbuf,
                   int                  count,
                   SCOREP_Ipc_Datatype  datatype,
                   SCOREP_Ipc_Operation operation,
                   int                  root )
{
    return SCOREP_IpcGroup_Reduce( SCOREP_IPC_GROUP_WORLD,
                                   sendbuf,
                                   recvbuf,
                                   count,
                                   datatype,
                                   operation,
                                   root );
}


int
SCOREP_IpcGroup_Allreduce( SCOREP_Ipc_Group*    group,
                           const void*          sendbuf,
                           void*                recvbuf,
                           int                  count,
                           SCOREP_Ipc_Datatype  datatype,
                           SCOREP_Ipc_Operation operation );


static inline int
SCOREP_Ipc_Allreduce( const void*          sendbuf,
                      void*                recvbuf,
                      int                  count,
                      SCOREP_Ipc_Datatype  datatype,
                      SCOREP_Ipc_Operation operation )
{
    return SCOREP_IpcGroup_Allreduce( SCOREP_IPC_GROUP_WORLD,
                                      sendbuf,
                                      recvbuf,
                                      count,
                                      datatype,
                                      operation );
}


int
SCOREP_IpcGroup_Scatter( SCOREP_Ipc_Group*   group,
                         const void*         sendbuf,
                         void*               recvbuf,
                         int                 count,
                         SCOREP_Ipc_Datatype datatype,
                         int                 root );


static inline int
SCOREP_Ipc_Scatter( const void*         sendbuf,
                    void*               recvbuf,
                    int                 count,
                    SCOREP_Ipc_Datatype datatype,
                    int                 root )
{
    return SCOREP_IpcGroup_Scatter( SCOREP_IPC_GROUP_WORLD,
                                    sendbuf,
                                    recvbuf,
                                    count,
                                    datatype,
                                    root );
}


int
SCOREP_IpcGroup_Scatterv( SCOREP_Ipc_Group*   group,
                          const void*         sendbuf,
                          const int*          sendcounts,
                          void*               recvbuf,
                          int                 recvcount,
                          SCOREP_Ipc_Datatype datatype,
                          int                 root );


static inline int
SCOREP_Ipc_Scatterv( const void*         sendbuf,
                     const int*          sendcounts,
                     void*               recvbuf,
                     int                 recvcount,
                     SCOREP_Ipc_Datatype datatype,
                     int                 root )
{
    return SCOREP_IpcGroup_Scatterv( SCOREP_IPC_GROUP_WORLD,
                                     sendbuf,
                                     sendcounts,
                                     recvbuf,
                                     recvcount,
                                     datatype,
                                     root );
}

/**
 * Not implemented in all IPC implementations.
 */
SCOREP_Ipc_Group*
SCOREP_IpcGroup_Split( SCOREP_Ipc_Group* parent,
                       int               color,
                       int               key );

/**
 * Not implemented in all IPC implementations.
 */
void
SCOREP_IpcGroup_Free( SCOREP_Ipc_Group* group );


#endif /* SCOREP_INTERNAL_IPC_H */
