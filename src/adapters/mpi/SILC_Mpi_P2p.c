/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
**  KOJAK       http://www.fz-juelich.de/jsc/kojak/                        **
*****************************************************************************
**  Copyright (c) 1998-2009                                                **
**  Forschungszentrum Juelich, Juelich Supercomputing Centre               **
**                                                                         **
**  Copyright (c) 2003-2008                                                **
**  University of Tennessee, Innovative Computing Laboratory               **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/

#include "SILC_Mpi_Reg.h"

#include <mpi.h>

/**
 * @file  SILC_Mpi_P2p.c
 *
 * @brief C interface wrappers for point-to-point communication
 */

/**
 * @name Blocking
 * @{
 */
/**
 * Measurement wrapper for MPI_Bsend
 * @note Auto-generated by wrapgen from template: ptp_send.w
 * @note C interface
 * @note Introduced with MPI-1
 * @ingroup p2p
 */
int
MPI_Bsend
(
    void*        buf,
    int          count,
    MPI_Datatype datatype,
    int          dest,
    int          tag,
    MPI_Comm     comm
)
{
    int return_val;

    if ( SILC_MPI_IS_EVENT_GEN_ON_FOR( SILC_MPI_ENABLED_P2P ) )
    {
        int sz;

        SILC_MPI_EVENT_GEN_OFF();
        SILC_EnterRegion( silc_mpi_regid[ SILC__MPI_BSEND ] );

        if ( dest != MPI_PROC_NULL )
        {
            PMPI_Type_size( datatype, &sz );
            SILC_MpiSend( SILC_MPI_RANK_TO_PE( dest, comm ), SILC_MPI_COMM_ID( comm ),
                          tag, count * sz );
        }
        return_val = PMPI_Bsend( buf, count, datatype, dest, tag, comm );

        SILC_ExitRegion( silc_mpi_regid[ SILC__MPI_BSEND ] );
        SILC_MPI_EVENT_GEN_ON();
    }
    else
    {
        return_val = PMPI_Bsend( buf, count, datatype, dest, tag, comm );
    }

    return return_val;
}
/**
 * Measurement wrapper for MPI_Rsend
 * @note Auto-generated by wrapgen from template: ptp_send.w
 * @note C interface
 * @note Introduced with MPI-1
 * @ingroup p2p
 */
int
MPI_Rsend
(
    void*        buf,
    int          count,
    MPI_Datatype datatype,
    int          dest,
    int          tag,
    MPI_Comm     comm
)
{
    int return_val;

    if ( SILC_MPI_IS_EVENT_GEN_ON_FOR( SILC_MPI_ENABLED_P2P ) )
    {
        int sz;

        SILC_MPI_EVENT_GEN_OFF();
        SILC_EnterRegion( silc_mpi_regid[ SILC__MPI_RSEND ] );

        if ( dest != MPI_PROC_NULL )
        {
            PMPI_Type_size( datatype, &sz );
            SILC_MpiSend( SILC_MPI_RANK_TO_PE( dest, comm ), SILC_MPI_COMM_ID( comm ),
                          tag, count * sz );
        }
        return_val = PMPI_Rsend( buf, count, datatype, dest, tag, comm );

        SILC_ExitRegion( silc_mpi_regid[ SILC__MPI_RSEND ] );
        SILC_MPI_EVENT_GEN_ON();
    }
    else
    {
        return_val = PMPI_Rsend( buf, count, datatype, dest, tag, comm );
    }

    return return_val;
}
/**
 * Measurement wrapper for MPI_Send
 * @note Auto-generated by wrapgen from template: ptp_send.w
 * @note C interface
 * @note Introduced with MPI-1
 * @ingroup p2p
 */
int
MPI_Send
(
    void*        buf,
    int          count,
    MPI_Datatype datatype,
    int          dest,
    int          tag,
    MPI_Comm     comm
)
{
    int return_val;

    if ( SILC_MPI_IS_EVENT_GEN_ON_FOR( SILC_MPI_ENABLED_P2P ) )
    {
        int sz;

        SILC_MPI_EVENT_GEN_OFF();
        SILC_EnterRegion( silc_mpi_regid[ SILC__MPI_SEND ] );

        if ( dest != MPI_PROC_NULL )
        {
            PMPI_Type_size( datatype, &sz );
            SILC_MpiSend( SILC_MPI_RANK_TO_PE( dest, comm ), SILC_MPI_COMM_ID( comm ),
                          tag, count * sz );
        }
        return_val = PMPI_Send( buf, count, datatype, dest, tag, comm );

        SILC_ExitRegion( silc_mpi_regid[ SILC__MPI_SEND ] );
        SILC_MPI_EVENT_GEN_ON();
    }
    else
    {
        return_val = PMPI_Send( buf, count, datatype, dest, tag, comm );
    }

    return return_val;
}
/**
 * Measurement wrapper for MPI_Ssend
 * @note Auto-generated by wrapgen from template: ptp_send.w
 * @note C interface
 * @note Introduced with MPI-1
 * @ingroup p2p
 */
int
MPI_Ssend
(
    void*        buf,
    int          count,
    MPI_Datatype datatype,
    int          dest,
    int          tag,
    MPI_Comm     comm
)
{
    int return_val;

    if ( SILC_MPI_IS_EVENT_GEN_ON_FOR( SILC_MPI_ENABLED_P2P ) )
    {
        int sz;

        SILC_MPI_EVENT_GEN_OFF();
        SILC_EnterRegion( silc_mpi_regid[ SILC__MPI_SSEND ] );

        if ( dest != MPI_PROC_NULL )
        {
            PMPI_Type_size( datatype, &sz );
            SILC_MpiSend( SILC_MPI_RANK_TO_PE( dest, comm ), SILC_MPI_COMM_ID( comm ),
                          tag, count * sz );
        }
        return_val = PMPI_Ssend( buf, count, datatype, dest, tag, comm );

        SILC_ExitRegion( silc_mpi_regid[ SILC__MPI_SSEND ] );
        SILC_MPI_EVENT_GEN_ON();
    }
    else
    {
        return_val = PMPI_Ssend( buf, count, datatype, dest, tag, comm );
    }

    return return_val;
}

/**
 * Measurement wrapper for MPI_Recv
 * @note Manually adapted wrapper
 * @note C interface
 * @note Introduced with MPI-1
 * @ingroup p2p
 */
int
MPI_Recv
(
    void*        buf,
    int          count,
    MPI_Datatype datatype,
    int          source,
    int          tag,
    MPI_Comm     comm,
    MPI_Status*  status
)
{
    int return_val;

    if ( SILC_MPI_IS_EVENT_GEN_ON_FOR( SILC_MPI_ENABLED_P2P ) )
    {
        int        sz;
        MPI_Status mystatus;

        SILC_MPI_EVENT_GEN_OFF();

        SILC_EnterRegion( silc_mpi_regid[ SILC__MPI_RECV ] );

        if ( status == MPI_STATUS_IGNORE )
        {
            status = &mystatus;
        }
        return_val = PMPI_Recv( buf, count, datatype, source, tag, comm, status );

        if ( source != MPI_PROC_NULL && return_val == MPI_SUCCESS )
        {
            PMPI_Type_size( datatype, &sz );
            PMPI_Get_count( status, datatype, &count );
            SILC_MpiRecv( SILC_MPI_RANK_TO_PE( status->MPI_SOURCE, comm ),
                          SILC_MPI_COMM_ID( comm ), status->MPI_TAG, count * sz );
        }

        SILC_ExitRegion( silc_mpi_regid[ SILC__MPI_RECV ] );

        SILC_MPI_EVENT_GEN_ON();
    }
    else
    {
        return_val = PMPI_Recv( buf, count, datatype, source, tag, comm, status );
    }

    return return_val;
}

/**
 * Measurement wrapper for MPI_Probe
 * @note Auto-generated by wrapgen from template: SILC_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-1
 * @ingroup p2p
 * It wraps the me) call with enter and exit events.
 */
int
MPI_Probe
(
    int         source,
    int         tag,
    MPI_Comm    comm,
    MPI_Status* status
)
{
    int return_val;

    if ( SILC_MPI_IS_EVENT_GEN_ON_FOR( SILC_MPI_ENABLED_P2P ) )
    {
        SILC_MPI_EVENT_GEN_OFF();
        SILC_EnterRegion( silc_mpi_regid[ SILC__MPI_PROBE ] );

        return_val = PMPI_Probe( source, tag, comm, status );

        SILC_ExitRegion( silc_mpi_regid[ SILC__MPI_PROBE ] );
        SILC_MPI_EVENT_GEN_ON();
    }
    else
    {
        return_val = PMPI_Probe( source, tag, comm, status );
    }

    return return_val;
}

/**
 * Measurement wrapper for MPI_Sendrecv
 * @note Manually adapted wrapper
 * @note C interface
 * @note Introduced with MPI-1
 * @ingroup p2p
 */
int
MPI_Sendrecv
(
    void*        sendbuf,
    int          sendcount,
    MPI_Datatype sendtype,
    int          dest,
    int          sendtag,
    void*        recvbuf,
    int          recvcount,
    MPI_Datatype recvtype,
    int          source,
    int          recvtag,
    MPI_Comm     comm,
    MPI_Status*  status
)
{
    int return_val;

    if ( SILC_MPI_IS_EVENT_GEN_ON_FOR( SILC_MPI_ENABLED_P2P ) )
    {
        int        sendsz, recvsz;
        MPI_Status mystatus;

        SILC_MPI_EVENT_GEN_OFF();

        SILC_EnterRegion( silc_mpi_regid[ SILC__MPI_SENDRECV ] );

        if ( dest != MPI_PROC_NULL )
        {
            PMPI_Type_size( sendtype, &sendsz );
            SILC_MpiSend( SILC_MPI_RANK_TO_PE( dest, comm ), SILC_MPI_COMM_ID( comm ),
                          sendtag, sendcount * sendsz );
        }

        if ( status == MPI_STATUS_IGNORE )
        {
            status = &mystatus;
        }

        return_val = PMPI_Sendrecv( sendbuf, sendcount, sendtype, dest,   sendtag,
                                    recvbuf, recvcount, recvtype, source, recvtag,
                                    comm, status );
        if ( source != MPI_PROC_NULL && return_val == MPI_SUCCESS )
        {
            PMPI_Type_size( recvtype, &recvsz );
            PMPI_Get_count( status, recvtype, &recvcount );
            SILC_MpiRecv( SILC_MPI_RANK_TO_PE( status->MPI_SOURCE, comm ),
                          SILC_MPI_COMM_ID( comm ), status->MPI_TAG, recvcount * recvsz );
        }

        SILC_ExitRegion( silc_mpi_regid[ SILC__MPI_SENDRECV ] );

        SILC_MPI_EVENT_GEN_ON();
    }
    else
    {
        return_val = PMPI_Sendrecv( sendbuf, sendcount, sendtype, dest,   sendtag,
                                    recvbuf, recvcount, recvtype, source, recvtag,
                                    comm, status );
    }

    return return_val;
}

/**
 * Measurement wrapper for MPI_Sendrecv_replace
 * @note Manually adapted wrapper
 * @note C interface
 * @note Introduced with MPI-1
 * @ingroup p2p
 */
int
MPI_Sendrecv_replace
(
    void*        buf,
    int          count,
    MPI_Datatype datatype,
    int          dest,
    int          sendtag,
    int          source,
    int          recvtag,
    MPI_Comm     comm,
    MPI_Status*  status
)
{
    int return_val;


    if ( SILC_MPI_IS_EVENT_GEN_ON_FOR( SILC_MPI_ENABLED_P2P ) )
    {
        int        sz;
        MPI_Status mystatus;

        SILC_MPI_EVENT_GEN_OFF();

        SILC_EnterRegion( silc_mpi_regid[ SILC__MPI_SENDRECV_REPLACE ] );

        PMPI_Type_size( datatype, &sz );
        if ( dest != MPI_PROC_NULL )
        {
            SILC_MpiSend( SILC_MPI_RANK_TO_PE( dest, comm ),
                          SILC_MPI_COMM_ID( comm ),
                          sendtag,
                          count * sz );
        }

        if ( status == MPI_STATUS_IGNORE )
        {
            status = &mystatus;
        }

        return_val = PMPI_Sendrecv_replace( buf, count, datatype, dest,
                                            sendtag, source, recvtag,
                                            comm, status );
        if ( source != MPI_PROC_NULL && return_val == MPI_SUCCESS )
        {
            SILC_MpiRecv( SILC_MPI_RANK_TO_PE( status->MPI_SOURCE, comm ),
                          SILC_MPI_COMM_ID( comm ), status->MPI_TAG, count * sz );
        }

        SILC_ExitRegion( silc_mpi_regid[ SILC__MPI_SENDRECV_REPLACE ] );

        SILC_MPI_EVENT_GEN_ON();
    }
    else
    {
        return_val = PMPI_Sendrecv_replace( buf, count, datatype, dest,
                                            sendtag, source, recvtag,
                                            comm, status );
    }

    return return_val;
}

/**
 * @}
 * @name Non-blocking
 * @{
 */

/**
 * Measurement wrapper for MPI_Ibsend
 * @note Auto-generated by wrapgen from template: ptp_send.w
 * @note C interface
 * @note Introduced with MPI-1
 * @ingroup p2p
 */
int
MPI_Ibsend
(
    void*        buf,
    int          count,
    MPI_Datatype datatype,
    int          dest,
    int          tag,
    MPI_Comm     comm,
    MPI_Request* request
)
{
    int return_val;

    if ( SILC_MPI_IS_EVENT_GEN_ON_FOR( SILC_MPI_ENABLED_P2P ) )
    {
        int sz;

        SILC_MPI_EVENT_GEN_OFF();
        SILC_EnterRegion( silc_mpi_regid[ SILC__MPI_IBSEND ] );

        if ( dest != MPI_PROC_NULL )
        {
            PMPI_Type_size( datatype, &sz );
            SILC_MpiSend( SILC_MPI_RANK_TO_PE( dest, comm ), SILC_MPI_COMM_ID( comm ),
                          tag, count * sz );
        }
        return_val = PMPI_Ibsend( buf, count, datatype, dest, tag, comm, request );

        SILC_ExitRegion( silc_mpi_regid[ SILC__MPI_IBSEND ] );
        SILC_MPI_EVENT_GEN_ON();
    }
    else
    {
        return_val = PMPI_Ibsend( buf, count, datatype, dest, tag, comm, request );
    }

    return return_val;
}
/**
 * Measurement wrapper for MPI_Irsend
 * @note Auto-generated by wrapgen from template: ptp_send.w
 * @note C interface
 * @note Introduced with MPI-1
 * @ingroup p2p
 */
int
MPI_Irsend
(
    void*        buf,
    int          count,
    MPI_Datatype datatype,
    int          dest,
    int          tag,
    MPI_Comm     comm,
    MPI_Request* request
)
{
    int return_val;

    if ( SILC_MPI_IS_EVENT_GEN_ON_FOR( SILC_MPI_ENABLED_P2P ) )
    {
        int sz;

        SILC_MPI_EVENT_GEN_OFF();
        SILC_EnterRegion( silc_mpi_regid[ SILC__MPI_IRSEND ] );

        if ( dest != MPI_PROC_NULL )
        {
            PMPI_Type_size( datatype, &sz );
            SILC_MpiSend( SILC_MPI_RANK_TO_PE( dest, comm ), SILC_MPI_COMM_ID( comm ),
                          tag, count * sz );
        }
        return_val = PMPI_Irsend( buf, count, datatype, dest, tag, comm, request );

        SILC_ExitRegion( silc_mpi_regid[ SILC__MPI_IRSEND ] );
        SILC_MPI_EVENT_GEN_ON();
    }
    else
    {
        return_val = PMPI_Irsend( buf, count, datatype, dest, tag, comm, request );
    }

    return return_val;
}
/**
 * Measurement wrapper for MPI_Isend
 * @note Auto-generated by wrapgen from template: ptp_send.w
 * @note C interface
 * @note Introduced with MPI-1
 * @ingroup p2p
 */
int
MPI_Isend
(
    void*        buf,
    int          count,
    MPI_Datatype datatype,
    int          dest,
    int          tag,
    MPI_Comm     comm,
    MPI_Request* request
)
{
    int return_val;

    if ( SILC_MPI_IS_EVENT_GEN_ON_FOR( SILC_MPI_ENABLED_P2P ) )
    {
        int sz;

        SILC_MPI_EVENT_GEN_OFF();
        SILC_EnterRegion( silc_mpi_regid[ SILC__MPI_ISEND ] );

        if ( dest != MPI_PROC_NULL )
        {
            PMPI_Type_size( datatype, &sz );
            SILC_MpiSend( SILC_MPI_RANK_TO_PE( dest, comm ), SILC_MPI_COMM_ID( comm ),
                          tag, count * sz );
        }
        return_val = PMPI_Isend( buf, count, datatype, dest, tag, comm, request );

        SILC_ExitRegion( silc_mpi_regid[ SILC__MPI_ISEND ] );
        SILC_MPI_EVENT_GEN_ON();
    }
    else
    {
        return_val = PMPI_Isend( buf, count, datatype, dest, tag, comm, request );
    }

    return return_val;
}
/**
 * Measurement wrapper for MPI_Issend
 * @note Auto-generated by wrapgen from template: ptp_send.w
 * @note C interface
 * @note Introduced with MPI-1
 * @ingroup p2p
 */
int
MPI_Issend
(
    void*        buf,
    int          count,
    MPI_Datatype datatype,
    int          dest,
    int          tag,
    MPI_Comm     comm,
    MPI_Request* request
)
{
    int return_val;

    if ( SILC_MPI_IS_EVENT_GEN_ON_FOR( SILC_MPI_ENABLED_P2P ) )
    {
        int sz;

        SILC_MPI_EVENT_GEN_OFF();
        SILC_EnterRegion( silc_mpi_regid[ SILC__MPI_ISSEND ] );

        if ( dest != MPI_PROC_NULL )
        {
            PMPI_Type_size( datatype, &sz );
            SILC_MpiSend( SILC_MPI_RANK_TO_PE( dest, comm ), SILC_MPI_COMM_ID( comm ),
                          tag, count * sz );
        }
        return_val = PMPI_Issend( buf, count, datatype, dest, tag, comm, request );

        SILC_ExitRegion( silc_mpi_regid[ SILC__MPI_ISSEND ] );
        SILC_MPI_EVENT_GEN_ON();
    }
    else
    {
        return_val = PMPI_Issend( buf, count, datatype, dest, tag, comm, request );
    }

    return return_val;
}

/**
 * Measurement wrapper for MPI_Irecv
 * @note Manually adapted wrapper
 * @note C interface
 * @note Introduced with MPI-1
 * @ingroup p2p
 */
int
MPI_Irecv
(
    void*        buf,
    int          count,
    MPI_Datatype datatype,
    int          source,
    int          tag,
    MPI_Comm     comm,
    MPI_Request* request
)
{
    const int event_gen_active = SILC_MPI_IS_EVENT_GEN_ON_FOR( SILC_MPI_ENABLED_P2P );
    int       return_val;

    if ( event_gen_active )
    {
        SILC_MPI_EVENT_GEN_OFF();

        SILC_EnterRegion( silc_mpi_regid[ SILC__MPI_IRECV ] );
    }

    return_val = PMPI_Irecv( buf, count, datatype, source, tag, comm, request );

/* no asynchroneous communication handling at first
   if (source != MPI_PROC_NULL && return_val == MPI_SUCCESS)
   {
    int sz;
    PMPI_Type_size(datatype, &sz);
    silc_request_create(*request, ERF_RECV,
                       tag, 0, count * sz, datatype, comm);
   }
 */

    if ( event_gen_active )
    {
        SILC_ExitRegion( silc_mpi_regid[ SILC__MPI_IRECV ] );

        SILC_MPI_EVENT_GEN_ON();
    }

    return return_val;
}

/**
 * Measurement wrapper for MPI_Wait
 * @note Manually adapted wrapper
 * @note C interface
 * @note Introduced with MPI-1
 * @ingroup p2p
 */
int
MPI_Wait
(
    MPI_Request* request,
    MPI_Status*  status
)
{
    const int           event_gen_active = SILC_MPI_IS_EVENT_GEN_ON_FOR( SILC_MPI_ENABLED_P2P );
    int                 return_val;
    MPI_Status          mystatus;
    struct SilcRequest* orig_req;

    if ( event_gen_active )
    {
        SILC_MPI_EVENT_GEN_OFF();

        SILC_EnterRegion( silc_mpi_regid[ SILC__MPI_WAIT ] );
    }

    if ( status == MPI_STATUS_IGNORE )
    {
        status = &mystatus;
    }

/* no asynchroneous communication handling at the beginning included
   orig_req   = silc_request_get(*request);
 */
    return_val = PMPI_Wait( request, status );

/* no asynchroneous communication handling at the beginning included
   silc_check_request(orig_req, status);
 */

    if ( event_gen_active )
    {
        SILC_ExitRegion( silc_mpi_regid[ SILC__MPI_WAIT ] );

        SILC_MPI_EVENT_GEN_ON();
    }

    return return_val;
}

/**
 * Measurement wrapper for MPI_Waitall
 * @note Manually adapted wrapper
 * @note C interface
 * @note Introduced with MPI-1
 * @ingroup p2p
 */
int
MPI_Waitall
(
    int          count,
    MPI_Request* requests,
    MPI_Status*  array_of_statuses
)
{
    const int           event_gen_active = SILC_MPI_IS_EVENT_GEN_ON_FOR( SILC_MPI_ENABLED_P2P );
    int                 return_val, i;
    struct SilcRequest* orig_req;

    if ( event_gen_active )
    {
        SILC_MPI_EVENT_GEN_OFF();

        SILC_EnterRegion( silc_mpi_regid[ SILC__MPI_WAITALL ] );
    }

/* no asynchroneous communication handling at the beginning included
   if (array_of_statuses == MPI_STATUSES_IGNORE)
   {
    array_of_statuses = silc_get_status_array(count);
   }
   silc_save_request_array(requests, count);
 */
    return_val = PMPI_Waitall( count, requests, array_of_statuses );
/* no asynchroneous communication handling at the beginning included
   for (i = 0; i < count; i++)
   {
    orig_req = silc_saved_request_get(i);
    silc_check_request(orig_req, &(array_of_statuses[i]));
   }
 */
    if ( event_gen_active )
    {
        SILC_ExitRegion( silc_mpi_regid[ SILC__MPI_WAITALL ] );

        SILC_MPI_EVENT_GEN_ON();
    }

    return return_val;
}

/**
 * Measurement wrapper for MPI_Waitany
 * @note Manually adapted wrapper
 * @note C interface
 * @note Introduced with MPI-1
 * @ingroup p2p
 */
int
MPI_Waitany
(
    int          count,
    MPI_Request* requests,
    int*         index,
    MPI_Status*  status
)
{
    const int           event_gen_active = SILC_MPI_IS_EVENT_GEN_ON_FOR( SILC_MPI_ENABLED_P2P );
    int                 return_val;
    struct SilcRequest* orig_req;
    MPI_Status          mystatus;

    if ( event_gen_active )
    {
        SILC_MPI_EVENT_GEN_OFF();

        SILC_EnterRegion( silc_mpi_regid[ SILC__MPI_WAITANY ] );
    }

/* no asynchroneous communication handling at the beginning included
   if (status == MPI_STATUS_IGNORE)
   {
    status = &mystatus;
   }

   silc_save_request_array(requests, count);
 */
    return_val = PMPI_Waitany( count, requests, index, status );
/* no asynchroneous communication handling at the beginning included
   orig_req   = silc_saved_request_get(*index);
   silc_check_request(orig_req, status);
 */
    if ( event_gen_active )
    {
        SILC_ExitRegion( silc_mpi_regid[ SILC__MPI_WAITANY ] );

        SILC_MPI_EVENT_GEN_ON();
    }

    return return_val;
}

/**
 * Measurement wrapper for MPI_Waitsome
 * @note Manually adapted wrapper
 * @note C interface
 * @note Introduced with MPI-1
 * @ingroup p2p
 */
int
MPI_Waitsome
(
    int          incount,
    MPI_Request* array_of_requests,
    int*         outcount,
    int*         array_of_indices,
    MPI_Status*  array_of_statuses
)
{
    const int           event_gen_active = SILC_MPI_IS_EVENT_GEN_ON_FOR( SILC_MPI_ENABLED_P2P );
    int                 return_val, i;
    struct SilcRequest* orig_req;

    if ( event_gen_active )
    {
        SILC_MPI_EVENT_GEN_OFF();

        SILC_EnterRegion( silc_mpi_regid[ SILC__MPI_WAITSOME ] );
    }

/* no asynchroneous communication handling at the beginning included
   if (array_of_statuses == MPI_STATUSES_IGNORE)
   {
    array_of_statuses = silc_get_status_array(incount);
   }
   silc_save_request_array(array_of_requests, incount);
 */
    return_val = PMPI_Waitsome( incount, array_of_requests, outcount,
                                array_of_indices, array_of_statuses );
/* no asynchroneous communication handling at the beginning included
   for (i=0; i<*outcount; ++i)
   {
    orig_req = silc_saved_request_get(array_of_indices[i]);
    silc_check_request(orig_req, &(array_of_statuses[i]));
   }
 */

    if ( event_gen_active )
    {
        SILC_ExitRegion( silc_mpi_regid[ SILC__MPI_WAITSOME ] );

        SILC_MPI_EVENT_GEN_ON();
    }

    return return_val;
}

/**
 * Measurement wrapper for MPI_Test
 * @note Manually adapted wrapper
 * @note C interface
 * @note Introduced with MPI-1
 * @ingroup p2p
 */
int
MPI_Test
(
    MPI_Request* request,
    int*         flag,
    MPI_Status*  status
)
{
    const int           event_gen_active = SILC_MPI_IS_EVENT_GEN_ON_FOR( SILC_MPI_ENABLED_P2P );
    int                 return_val;
    struct SilcRequest* orig_req;
    MPI_Status          mystatus;

    if ( event_gen_active )
    {
        SILC_MPI_EVENT_GEN_OFF();

        SILC_EnterRegion( silc_mpi_regid[ SILC__MPI_TEST ] );
    }

/* no asynchroneous communication handling at the beginning included
   if (status == MPI_STATUS_IGNORE)
   {
    status = &mystatus;
   }
   orig_req   = silc_request_get(*request);
 */
    return_val = PMPI_Test( request, flag, status );
/* no asynchroneous communication handling at the beginning included
   if (*flag)
   {
    silc_check_request(orig_req, status);
   }
 */

    if ( event_gen_active )
    {
        SILC_ExitRegion( silc_mpi_regid[ SILC__MPI_TEST ] );

        SILC_MPI_EVENT_GEN_ON();
    }

    return return_val;
}

/**
 * Measurement wrapper for MPI_Testany
 * @note Manually adapted wrapper
 * @note C interface
 * @note Introduced with MPI-1
 * @ingroup p2p
 */
int
MPI_Testany
(
    int          count,
    MPI_Request* array_of_requests,
    int*         index,
    int*         flag,
    MPI_Status*  status
)
{
    const int           event_gen_active = SILC_MPI_IS_EVENT_GEN_ON_FOR( SILC_MPI_ENABLED_P2P );
    int                 return_val;
    struct SilcRequest* orig_req;
    MPI_Status          mystatus;

    if ( event_gen_active )
    {
        SILC_MPI_EVENT_GEN_OFF();

        SILC_EnterRegion( silc_mpi_regid[ SILC__MPI_TESTANY ] );
    }

/* no asynchroneous communication handling at the beginning included
   if (status == MPI_STATUS_IGNORE)
   {
    status = &mystatus;
   }
   silc_save_request_array(array_of_requests, count);
 */
    return_val = PMPI_Testany( count, array_of_requests, index, flag, status );
/* no asynchroneous communication handling at the beginning included
   if (*flag && *index != MPI_UNDEFINED)
   {
    orig_req = silc_saved_request_get(*index);
    silc_check_request(orig_req, status);
   }
 */
    if ( event_gen_active )
    {
        SILC_ExitRegion( silc_mpi_regid[ SILC__MPI_TESTANY ] );

        SILC_MPI_EVENT_GEN_ON();
    }

    return return_val;
}

/**
 * Measurement wrapper for MPI_Testall
 * @note Manually adapted wrapper
 * @note C interface
 * @note Introduced with MPI-1
 * @ingroup p2p
 */
int
MPI_Testall
(
    int          count,
    MPI_Request* array_of_requests,
    int*         flag,
    MPI_Status*  array_of_statuses
)
{
    const int           event_gen_active = SILC_MPI_IS_EVENT_GEN_ON_FOR( SILC_MPI_ENABLED_P2P );
    int                 return_val, i;
    struct SilcRequest* orig_req;

    if ( event_gen_active )
    {
        SILC_MPI_EVENT_GEN_OFF();

        SILC_EnterRegion( silc_mpi_regid[ SILC__MPI_TESTALL ] );
    }
/* no asynchroneous communication handling at the beginning included
   if (array_of_statuses == MPI_STATUSES_IGNORE)
   {
    array_of_statuses = silc_get_status_array(count);
   }
   silc_save_request_array(array_of_requests, count);
 */
    return_val = PMPI_Testall( count, array_of_requests, flag, array_of_statuses );
/* no asynchroneous communication handling at the beginning included
   if (*flag)
   {
    for (i = 0; i < count; i++)
    {
      orig_req = silc_saved_request_get(i);
      silc_check_request(orig_req, &(array_of_statuses[i]));
    }
   }
 */
    if ( event_gen_active )
    {
        SILC_ExitRegion( silc_mpi_regid[ SILC__MPI_TESTALL ] );

        SILC_MPI_EVENT_GEN_ON();
    }

    return return_val;
}

/**
 * Measurement wrapper for MPI_Testsome
 * @note Manually adapted wrapper
 * @note C interface
 * @note Introduced with MPI-1
 * @ingroup p2p
 */
int
MPI_Testsome
(
    int          incount,
    MPI_Request* array_of_requests,
    int*         outcount,
    int*         array_of_indices,
    MPI_Status*  array_of_statuses
)
{
    const int           event_gen_active = SILC_MPI_IS_EVENT_GEN_ON_FOR( SILC_MPI_ENABLED_P2P );
    int                 return_val, i;
    struct SilcRequest* orig_req;

    if ( event_gen_active )
    {
        SILC_MPI_EVENT_GEN_OFF();

        SILC_EnterRegion( silc_mpi_regid[ SILC__MPI_TESTSOME ] );
    }

/* no asynchroneous communication handling at the beginning included
   if (array_of_statuses == MPI_STATUSES_IGNORE)
   {
    array_of_statuses = silc_get_status_array(incount);
   }
   silc_save_request_array(array_of_requests, incount);
 */
    return_val = PMPI_Testsome( incount, array_of_requests, outcount,
                                array_of_indices, array_of_statuses );
/* no asynchroneous communication handling at the beginning included
   for (i=0; i<*outcount; ++i)
   {
    orig_req = silc_saved_request_get(array_of_indices[i]);
    silc_check_request(orig_req, &(array_of_statuses[i]));
   }
 */
    if ( event_gen_active )
    {
        SILC_ExitRegion( silc_mpi_regid[ SILC__MPI_TESTSOME ] );

        SILC_MPI_EVENT_GEN_ON();
    }

    return return_val;
}

/**
 * @}
 * @name Persitent requests
 * @{
 */

/* no asynchroneous communication handling at the beginning included
   pragma wrapgen multiple regex(MPI_(S|B|R)[s]?end_init$) skel/SILC_Mpi_PtpSendinit.w
 */
/**
 * Measurement wrapper for MPI_Bsend_init
 * @note Auto-generated by wrapgen from template: SILC_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-1
 * @ingroup p2p
 * It wraps the me) call with enter and exit events.
 */
int
MPI_Bsend_init
(
    void*        buf,
    int          count,
    MPI_Datatype datatype,
    int          dest,
    int          tag,
    MPI_Comm     comm,
    MPI_Request* request
)
{
    int return_val;

    if ( SILC_MPI_IS_EVENT_GEN_ON_FOR( SILC_MPI_ENABLED_P2P ) )
    {
        SILC_MPI_EVENT_GEN_OFF();
        SILC_EnterRegion( silc_mpi_regid[ SILC__MPI_BSEND_INIT ] );

        return_val = PMPI_Bsend_init( buf, count, datatype, dest, tag, comm, request );

        SILC_ExitRegion( silc_mpi_regid[ SILC__MPI_BSEND_INIT ] );
        SILC_MPI_EVENT_GEN_ON();
    }
    else
    {
        return_val = PMPI_Bsend_init( buf, count, datatype, dest, tag, comm, request );
    }

    return return_val;
}
/**
 * Measurement wrapper for MPI_Rsend_init
 * @note Auto-generated by wrapgen from template: SILC_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-1
 * @ingroup p2p
 * It wraps the me) call with enter and exit events.
 */
int
MPI_Rsend_init
(
    void*        buf,
    int          count,
    MPI_Datatype datatype,
    int          dest,
    int          tag,
    MPI_Comm     comm,
    MPI_Request* request
)
{
    int return_val;

    if ( SILC_MPI_IS_EVENT_GEN_ON_FOR( SILC_MPI_ENABLED_P2P ) )
    {
        SILC_MPI_EVENT_GEN_OFF();
        SILC_EnterRegion( silc_mpi_regid[ SILC__MPI_RSEND_INIT ] );

        return_val = PMPI_Rsend_init( buf, count, datatype, dest, tag, comm, request );

        SILC_ExitRegion( silc_mpi_regid[ SILC__MPI_RSEND_INIT ] );
        SILC_MPI_EVENT_GEN_ON();
    }
    else
    {
        return_val = PMPI_Rsend_init( buf, count, datatype, dest, tag, comm, request );
    }

    return return_val;
}
/**
 * Measurement wrapper for MPI_Send_init
 * @note Auto-generated by wrapgen from template: SILC_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-1
 * @ingroup p2p
 * It wraps the me) call with enter and exit events.
 */
int
MPI_Send_init
(
    void*        buf,
    int          count,
    MPI_Datatype datatype,
    int          dest,
    int          tag,
    MPI_Comm     comm,
    MPI_Request* request
)
{
    int return_val;

    if ( SILC_MPI_IS_EVENT_GEN_ON_FOR( SILC_MPI_ENABLED_P2P ) )
    {
        SILC_MPI_EVENT_GEN_OFF();
        SILC_EnterRegion( silc_mpi_regid[ SILC__MPI_SEND_INIT ] );

        return_val = PMPI_Send_init( buf, count, datatype, dest, tag, comm, request );

        SILC_ExitRegion( silc_mpi_regid[ SILC__MPI_SEND_INIT ] );
        SILC_MPI_EVENT_GEN_ON();
    }
    else
    {
        return_val = PMPI_Send_init( buf, count, datatype, dest, tag, comm, request );
    }

    return return_val;
}
/**
 * Measurement wrapper for MPI_Ssend_init
 * @note Auto-generated by wrapgen from template: SILC_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-1
 * @ingroup p2p
 * It wraps the me) call with enter and exit events.
 */
int
MPI_Ssend_init
(
    void*        buf,
    int          count,
    MPI_Datatype datatype,
    int          dest,
    int          tag,
    MPI_Comm     comm,
    MPI_Request* request
)
{
    int return_val;

    if ( SILC_MPI_IS_EVENT_GEN_ON_FOR( SILC_MPI_ENABLED_P2P ) )
    {
        SILC_MPI_EVENT_GEN_OFF();
        SILC_EnterRegion( silc_mpi_regid[ SILC__MPI_SSEND_INIT ] );

        return_val = PMPI_Ssend_init( buf, count, datatype, dest, tag, comm, request );

        SILC_ExitRegion( silc_mpi_regid[ SILC__MPI_SSEND_INIT ] );
        SILC_MPI_EVENT_GEN_ON();
    }
    else
    {
        return_val = PMPI_Ssend_init( buf, count, datatype, dest, tag, comm, request );
    }

    return return_val;
}

/**
 * Measurement wrapper for MPI_Recv_init
 * @note Manually adapted wrapper
 * @note C interface
 * @note Introduced with MPI-1
 * @ingroup p2p
 */
int
MPI_Recv_init
(
    void*        buf,
    int          count,
    MPI_Datatype datatype,
    int          source,
    int          tag,
    MPI_Comm     comm,
    MPI_Request* request
)
{
    const int event_gen_active = SILC_MPI_IS_EVENT_GEN_ON_FOR( SILC_MPI_ENABLED_P2P );
    int       return_val;

    if ( event_gen_active )
    {
        SILC_MPI_EVENT_GEN_OFF();

        SILC_EnterRegion( silc_mpi_regid[ SILC__MPI_RECV_INIT ] );
    }

    return_val = PMPI_Recv_init( buf, count, datatype, source, tag, comm, request );
    if ( source != MPI_PROC_NULL && return_val == MPI_SUCCESS )
    {
        int sz;
        PMPI_Type_size( datatype, &sz );
/* no asynchroneous communication handling at the beginning included
    silc_request_create(*request, (ERF_RECV | ERF_IS_PERSISTENT),
                       tag, source, count * sz, datatype, comm);
 */
    }

    if ( event_gen_active )
    {
        SILC_ExitRegion( silc_mpi_regid[ SILC__MPI_RECV_INIT ] );

        SILC_MPI_EVENT_GEN_ON();
    }

    return return_val;
}

/**
 * Measurement wrapper for MPI_Start
 * @note Manually adapted wrapper
 * @note C interface
 * @note Introduced with MPI-1
 * @ingroup p2p
 */
int
MPI_Start
(
    MPI_Request* request
)
{
    const int event_gen_active = SILC_MPI_IS_EVENT_GEN_ON_FOR( SILC_MPI_ENABLED_P2P );
    int       return_val;

    if ( event_gen_active )
    {
        struct SilcRequest* req;

        SILC_MPI_EVENT_GEN_OFF();

        SILC_EnterRegion( silc_mpi_regid[ SILC__MPI_START ] );

/* no asynchroneous communication handling at the beginning included
    req = silc_request_get(*request);
    if (req && (req->flags & ERF_IS_PERSISTENT))
    {
      req->flags |= ERF_IS_ACTIVE;
      if ((req->flags & ERF_SEND) && (req->dest != MPI_PROC_NULL))
      {
        SILC_MpiSend(SILC_MPI_RANK_TO_PE(req->dest, req->comm),
                     SILC_MPI_COMM_ID(req->comm), req->tag,  req->bytes);
      }
    }
 */
    }

    return_val = PMPI_Start( request );

    if ( event_gen_active )
    {
        SILC_ExitRegion( silc_mpi_regid[ SILC__MPI_START ] );

        SILC_MPI_EVENT_GEN_ON();
    }

    return return_val;
}

/**
 * Measurement wrapper for MPI_Startall
 * @note Manually adapted wrapper
 * @note C interface
 * @note Introduced with MPI-1
 * @ingroup p2p
 */
int
MPI_Startall
(
    int          count,
    MPI_Request* array_of_requests
)
{
    const int event_gen_active = SILC_MPI_IS_EVENT_GEN_ON_FOR( SILC_MPI_ENABLED_P2P );
    int       return_val, i;

    if ( event_gen_active )
    {
        MPI_Request* request;

/* no asynchroneous communication handling at the beginning included
    struct SilcRequest* req;
 */

        SILC_MPI_EVENT_GEN_OFF();

        SILC_EnterRegion( silc_mpi_regid[ SILC__MPI_STARTALL ] );

/* no asynchroneous communication handling at the beginning included
    for (i = 0; i < count; i++)
    {
      request = &array_of_requests[i];
      req     = silc_request_get(*request);

      if (req && (req->flags & ERF_IS_PERSISTENT))
      {
        req->flags |= ERF_IS_ACTIVE;
        if ((req->flags & ERF_SEND) && (req->dest != MPI_PROC_NULL))
          SILC_MpiSend(SILC_MPI_RANK_TO_PE(req->dest, req->comm),
                       SILC_MPI_COMM_ID(req->comm), req->tag,  req->bytes);
      }
    }
 */
    }

    return_val = PMPI_Startall( count, array_of_requests );

    if ( event_gen_active )
    {
        SILC_ExitRegion( silc_mpi_regid[ SILC__MPI_STARTALL ] );

        SILC_MPI_EVENT_GEN_ON();
    }

    return return_val;
}

/**
 * Measurement wrapper for MPI_Request_free
 * @note Manually adapted wrapper
 * @note C interface
 * @note Introduced with MPI-1
 * @ingroup p2p
 */
int
MPI_Request_free
(
    MPI_Request* request
)
{
    const int event_gen_active = SILC_MPI_IS_EVENT_GEN_ON_FOR( SILC_MPI_ENABLED_P2P );
    int       return_val;
/* no asynchroneous communication handling at the beginning included
   struct SilcRequest* req;
 */

    if ( event_gen_active )
    {
        SILC_MPI_EVENT_GEN_OFF();

        SILC_EnterRegion( silc_mpi_regid[ SILC__MPI_REQUEST_FREE ] );
    }

/* no asynchroneous communication handling at the beginning included
   req = silc_request_get(*request);
   if (req && (req->flags & ERF_IS_PERSISTENT))
   {
    if (req->flags & ERF_IS_ACTIVE )
      /* mark active requests for deallocation
      req->flags |= ERF_DEALLOCATE;
    else
      /* deallocate inactive requests -
      silc_request_free(req);
   }
 */
/* -- else non-persistent requests:
 *    + we don't track non-persistent sends
 *    + MPI standard strongly suggests to deallocate non-persistent
 *      recv's only by wait or test
 *    ==> nothing to do here
 */
    return_val = PMPI_Request_free( request );

    if ( event_gen_active )
    {
        SILC_ExitRegion( silc_mpi_regid[ SILC__MPI_REQUEST_FREE ] );

        SILC_MPI_EVENT_GEN_ON();
    }

    return return_val;
}

/**
 * Measurement wrapper for MPI_Cancel
 * @note Manually adapted wrapper
 * @note C interface
 * @note Introduced with MPI-1
 * @ingroup p2p
 */
int
MPI_Cancel
(
    MPI_Request* request
)
{
    const int event_gen_active = SILC_MPI_IS_EVENT_GEN_ON_FOR( SILC_MPI_ENABLED_P2P );
    int       return_val;

    if ( event_gen_active )
    {
        SILC_MPI_EVENT_GEN_OFF();

        SILC_EnterRegion( silc_mpi_regid[ SILC__MPI_CANCEL ] );
    }

    /* -- do not really know what to do here ?!?
     *    would need to find out if canceled communcation completed
     *    sucessfully or was canceled sucessfully (probably possible
     *    by using PMPI_Test_cancelled) but whatever we do here,
     *    we end up by an invalid trace as there we cannot remove the
     *    send events already put in the trace buffer, and so the
     *    message matching in the analysis will fail in any case
     */

    return_val = PMPI_Cancel( request );

    if ( event_gen_active )
    {
        SILC_ExitRegion( silc_mpi_regid[ SILC__MPI_CANCEL ] );

        SILC_MPI_EVENT_GEN_ON();
    }

    return return_val;
}
