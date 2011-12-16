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
 * @file       scorep_mpi_disabled.c
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status alpha
 *
 *
 */


#include <config.h>


#include <scorep_utility/SCOREP_Debug.h>


#include "scorep_mpi.h"

#include <scorep_thread.h>
#include <scorep_definitions.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

extern void
scorep_status_initialize_non_mpi();


extern void
scorep_status_initialize_common();


void
SCOREP_Status_Initialize()
{
    scorep_status_initialize_common();
    scorep_status_initialize_non_mpi();
}


bool
scorep_create_experiment_dir( void ( * createDir )( void ) )
{
    createDir();
    return true;
}


bool
SCOREP_Mpi_HasMpi()
{
    return false;
}


void
SCOREP_Mpi_GlobalBarrier()
{
    // nothing to to here
}


void
SCOREP_Mpi_DuplicateCommWorld()
{
    // nothing to do here
}


int
SCOREP_Mpi_CalculateCommWorldSize()
{
    return 1;
}


int
SCOREP_Mpi_Send( void*               buf,
                 int                 count,
                 SCOREP_Mpi_Datatype scorep_datatype,
                 int                 dest )
{
    SCOREP_BUG( "SCOREP_Mpi_Send() called in non-mpi build" );

    return 1;
}


int
SCOREP_Mpi_Recv( void*               buf,
                 int                 count,
                 SCOREP_Mpi_Datatype scorep_datatype,
                 int                 source,
                 SCOREP_Mpi_Status   status )
{
    SCOREP_BUG( "SCOREP_Mpi_Recv() called in non-mpi build" );

    return 1;
}

static int scorep_mpi_datatype_size[] =
{
    1, /* MPI_UNSIGNED_CHAR */
    4, /* MPI_INT */
    4, /* MPI_UNSINGED */
    8, /* MPI_LONG_LONG */
    8  /* MPI_DOUBLE */
};

int
SCOREP_Mpi_Allgather( void*               sendbuf,
                      int                 sendcount,
                      SCOREP_Mpi_Datatype sendtype,
                      void*               recvbuf,
                      int                 recvcount,
                      SCOREP_Mpi_Datatype recvtype )
{
    /* In non-mpi case, we have only rank zero. Thus copy sendbuf to recvbuf. */
    int num = scorep_mpi_datatype_size[ sendtype ] * sendcount;
    memcpy( recvbuf, sendbuf, num );
    return 0;
}

int
SCOREP_Mpi_Allreduce( void*                sendbuf,
                      void*                recvbuf,
                      int                  count,
                      SCOREP_Mpi_Datatype  scorep_datatype,
                      SCOREP_Mpi_Operation scorep_operation )
{
    /* In non-mpi case, we have only rank zero. Thus all operations just copy sendbuf to
       recvbuf. */
    int num = scorep_mpi_datatype_size[ scorep_datatype ] * count;
    memcpy( recvbuf, sendbuf, num );
    return 0;
}

int
SCOREP_Mpi_Barrier()
{
    return 0;
}

int
SCOREP_Mpi_Bcast( void*               buf,
                  int                 count,
                  SCOREP_Mpi_Datatype scorep_datatype,
                  int                 root )
{
    /* In non-mpi case there is no other rank to which we send something.
       Thus, nothing to do */
    return 0;
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
    /* In non-mpi case, we have only rank zero. Thus copy sendbuf to recvbuf. */
    int num = scorep_mpi_datatype_size[ scorep_sendtype ] * sendcount;
    memcpy( recvbuf, sendbuf, num );
    return 0;
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
    /* In non-mpi case, we have only rank zero. Thus copy sendbuf to recvbuf. */
    int num = scorep_mpi_datatype_size[ scorep_sendtype ] * sendcount;
    memcpy( recvbuf, sendbuf, num );
    return 0;
}

int
SCOREP_Mpi_Reduce( void*                sendbuf,
                   void*                recvbuf,
                   int                  count,
                   SCOREP_Mpi_Datatype  scorep_datatype,
                   SCOREP_Mpi_Operation scorep_operation,
                   int                  root )
{
    /* In non-mpi case, we have only rank zero. Thus all operations just copy sendbuf to
       recvbuf. */
    int num = scorep_mpi_datatype_size[ scorep_datatype ] * count;
    memcpy( recvbuf, sendbuf, num );
    return 0;
}

int
SCOREP_Mpi_Scan( void*                sendbuf,
                 void*                recvbuf,
                 int                  count,
                 SCOREP_Mpi_Datatype  scorep_datatype,
                 SCOREP_Mpi_Operation scorep_operation )
{
    /* In non-mpi case, we have only rank zero. Thus all operations just copy sendbuf to
       recvbuf. */
    int num = scorep_mpi_datatype_size[ scorep_datatype ] * count;
    memcpy( recvbuf, sendbuf, num );
    return 0;
}
