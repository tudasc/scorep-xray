/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2017,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2015,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2014,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2013,
 * Technische Universitaet Muenchen, Germany
 *
 * Copyright (c) 2016,
 * Technische Universitaet Darmstadt, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2011                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  Copyright (c) 2010-2011                                                **
**  German Research School for Simulation Sciences GmbH,                   **
**  Laboratory for Parallel Programming                                    **
**                                                                         **
**  Copyright (c) 2003-2008                                                **
**  University of Tennessee, Innovative Computing Laboratory               **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/



/**
 * @file
 * @ingroup    MPI_Wrapper
 *
 * @brief Fortran interface wrappers for point-to-point communication
 */

#include <config.h>
#include "SCOREP_Fmpi.h"

/* uppercase defines */
/** @def MPI_Bsend_U
    Exchange MPI_Bsend_U by MPI_BSEND.
    It is used for the Fortran wrappers of MPI_Bsend.
 */
#define MPI_Bsend_U MPI_BSEND

/** @def MPI_Bsend_init_U
    Exchange MPI_Bsend_init_U by MPI_BSEND_INIT.
    It is used for the Fortran wrappers of MPI_Bsend_init.
 */
#define MPI_Bsend_init_U MPI_BSEND_INIT

/** @def MPI_Buffer_attach_U
    Exchange MPI_Buffer_attach_U by MPI_BUFFER_ATTACH.
    It is used for the Fortran wrappers of MPI_Buffer_attach.
 */
#define MPI_Buffer_attach_U MPI_BUFFER_ATTACH

/** @def MPI_Buffer_detach_U
    Exchange MPI_Buffer_detach_U by MPI_BUFFER_DETACH.
    It is used for the Fortran wrappers of MPI_Buffer_detach.
 */
#define MPI_Buffer_detach_U MPI_BUFFER_DETACH

/** @def MPI_Cancel_U
    Exchange MPI_Cancel_U by MPI_CANCEL.
    It is used for the Fortran wrappers of MPI_Cancel.
 */
#define MPI_Cancel_U MPI_CANCEL

/** @def MPI_Ibsend_U
    Exchange MPI_Ibsend_U by MPI_IBSEND.
    It is used for the Fortran wrappers of MPI_Ibsend.
 */
#define MPI_Ibsend_U MPI_IBSEND

/** @def MPI_Improbe_U
    Exchange MPI_Improbe_U by MPI_IMPROBE.
    It is used for the Fortran wrappers of MPI_Improbe.
 */
#define MPI_Improbe_U MPI_IMPROBE

/** @def MPI_Imrecv_U
    Exchange MPI_Imrecv_U by MPI_IMRECV.
    It is used for the Fortran wrappers of MPI_Imrecv.
 */
#define MPI_Imrecv_U MPI_IMRECV

/** @def MPI_Iprobe_U
    Exchange MPI_Iprobe_U by MPI_IPROBE.
    It is used for the Fortran wrappers of MPI_Iprobe.
 */
#define MPI_Iprobe_U MPI_IPROBE

/** @def MPI_Irecv_U
    Exchange MPI_Irecv_U by MPI_IRECV.
    It is used for the Fortran wrappers of MPI_Irecv.
 */
#define MPI_Irecv_U MPI_IRECV

/** @def MPI_Irsend_U
    Exchange MPI_Irsend_U by MPI_IRSEND.
    It is used for the Fortran wrappers of MPI_Irsend.
 */
#define MPI_Irsend_U MPI_IRSEND

/** @def MPI_Isend_U
    Exchange MPI_Isend_U by MPI_ISEND.
    It is used for the Fortran wrappers of MPI_Isend.
 */
#define MPI_Isend_U MPI_ISEND

/** @def MPI_Issend_U
    Exchange MPI_Issend_U by MPI_ISSEND.
    It is used for the Fortran wrappers of MPI_Issend.
 */
#define MPI_Issend_U MPI_ISSEND

/** @def MPI_Mprobe_U
    Exchange MPI_Mprobe_U by MPI_MPROBE.
    It is used for the Fortran wrappers of MPI_Mprobe.
 */
#define MPI_Mprobe_U MPI_MPROBE

/** @def MPI_Mrecv_U
    Exchange MPI_Mrecv_U by MPI_MRECV.
    It is used for the Fortran wrappers of MPI_Mrecv.
 */
#define MPI_Mrecv_U MPI_MRECV

/** @def MPI_Probe_U
    Exchange MPI_Probe_U by MPI_PROBE.
    It is used for the Fortran wrappers of MPI_Probe.
 */
#define MPI_Probe_U MPI_PROBE

/** @def MPI_Recv_U
    Exchange MPI_Recv_U by MPI_RECV.
    It is used for the Fortran wrappers of MPI_Recv.
 */
#define MPI_Recv_U MPI_RECV

/** @def MPI_Recv_init_U
    Exchange MPI_Recv_init_U by MPI_RECV_INIT.
    It is used for the Fortran wrappers of MPI_Recv_init.
 */
#define MPI_Recv_init_U MPI_RECV_INIT

/** @def MPI_Request_free_U
    Exchange MPI_Request_free_U by MPI_REQUEST_FREE.
    It is used for the Fortran wrappers of MPI_Request_free.
 */
#define MPI_Request_free_U MPI_REQUEST_FREE

/** @def MPI_Rsend_U
    Exchange MPI_Rsend_U by MPI_RSEND.
    It is used for the Fortran wrappers of MPI_Rsend.
 */
#define MPI_Rsend_U MPI_RSEND

/** @def MPI_Rsend_init_U
    Exchange MPI_Rsend_init_U by MPI_RSEND_INIT.
    It is used for the Fortran wrappers of MPI_Rsend_init.
 */
#define MPI_Rsend_init_U MPI_RSEND_INIT

/** @def MPI_Send_U
    Exchange MPI_Send_U by MPI_SEND.
    It is used for the Fortran wrappers of MPI_Send.
 */
#define MPI_Send_U MPI_SEND

/** @def MPI_Send_init_U
    Exchange MPI_Send_init_U by MPI_SEND_INIT.
    It is used for the Fortran wrappers of MPI_Send_init.
 */
#define MPI_Send_init_U MPI_SEND_INIT

/** @def MPI_Sendrecv_U
    Exchange MPI_Sendrecv_U by MPI_SENDRECV.
    It is used for the Fortran wrappers of MPI_Sendrecv.
 */
#define MPI_Sendrecv_U MPI_SENDRECV

/** @def MPI_Sendrecv_replace_U
    Exchange MPI_Sendrecv_replace_U by MPI_SENDRECV_REPLACE.
    It is used for the Fortran wrappers of MPI_Sendrecv_replace.
 */
#define MPI_Sendrecv_replace_U MPI_SENDRECV_REPLACE

/** @def MPI_Ssend_U
    Exchange MPI_Ssend_U by MPI_SSEND.
    It is used for the Fortran wrappers of MPI_Ssend.
 */
#define MPI_Ssend_U MPI_SSEND

/** @def MPI_Ssend_init_U
    Exchange MPI_Ssend_init_U by MPI_SSEND_INIT.
    It is used for the Fortran wrappers of MPI_Ssend_init.
 */
#define MPI_Ssend_init_U MPI_SSEND_INIT

/** @def MPI_Start_U
    Exchange MPI_Start_U by MPI_START.
    It is used for the Fortran wrappers of MPI_Start.
 */
#define MPI_Start_U MPI_START

/** @def MPI_Startall_U
    Exchange MPI_Startall_U by MPI_STARTALL.
    It is used for the Fortran wrappers of MPI_Startall.
 */
#define MPI_Startall_U MPI_STARTALL

/** @def MPI_Test_U
    Exchange MPI_Test_U by MPI_TEST.
    It is used for the Fortran wrappers of MPI_Test.
 */
#define MPI_Test_U MPI_TEST

/** @def MPI_Test_cancelled_U
    Exchange MPI_Test_cancelled_U by MPI_TEST_CANCELLED.
    It is used for the Fortran wrappers of MPI_Test_cancelled.
 */
#define MPI_Test_cancelled_U MPI_TEST_CANCELLED

/** @def MPI_Testall_U
    Exchange MPI_Testall_U by MPI_TESTALL.
    It is used for the Fortran wrappers of MPI_Testall.
 */
#define MPI_Testall_U MPI_TESTALL

/** @def MPI_Testany_U
    Exchange MPI_Testany_U by MPI_TESTANY.
    It is used for the Fortran wrappers of MPI_Testany.
 */
#define MPI_Testany_U MPI_TESTANY

/** @def MPI_Testsome_U
    Exchange MPI_Testsome_U by MPI_TESTSOME.
    It is used for the Fortran wrappers of MPI_Testsome.
 */
#define MPI_Testsome_U MPI_TESTSOME

/** @def MPI_Wait_U
    Exchange MPI_Wait_U by MPI_WAIT.
    It is used for the Fortran wrappers of MPI_Wait.
 */
#define MPI_Wait_U MPI_WAIT

/** @def MPI_Waitall_U
    Exchange MPI_Waitall_U by MPI_WAITALL.
    It is used for the Fortran wrappers of MPI_Waitall.
 */
#define MPI_Waitall_U MPI_WAITALL

/** @def MPI_Waitany_U
    Exchange MPI_Waitany_U by MPI_WAITANY.
    It is used for the Fortran wrappers of MPI_Waitany.
 */
#define MPI_Waitany_U MPI_WAITANY

/** @def MPI_Waitsome_U
    Exchange MPI_Waitsome_U by MPI_WAITSOME.
    It is used for the Fortran wrappers of MPI_Waitsome.
 */
#define MPI_Waitsome_U MPI_WAITSOME


/* lowercase defines */
/** @def MPI_Bsend_L
    Exchanges MPI_Bsend_L by mpi_bsend.
    It is used for the Fortran wrappers of MPI_Bsend.
 */
#define MPI_Bsend_L mpi_bsend

/** @def MPI_Bsend_init_L
    Exchanges MPI_Bsend_init_L by mpi_bsend_init.
    It is used for the Fortran wrappers of MPI_Bsend_init.
 */
#define MPI_Bsend_init_L mpi_bsend_init

/** @def MPI_Buffer_attach_L
    Exchanges MPI_Buffer_attach_L by mpi_buffer_attach.
    It is used for the Fortran wrappers of MPI_Buffer_attach.
 */
#define MPI_Buffer_attach_L mpi_buffer_attach

/** @def MPI_Buffer_detach_L
    Exchanges MPI_Buffer_detach_L by mpi_buffer_detach.
    It is used for the Fortran wrappers of MPI_Buffer_detach.
 */
#define MPI_Buffer_detach_L mpi_buffer_detach

/** @def MPI_Cancel_L
    Exchanges MPI_Cancel_L by mpi_cancel.
    It is used for the Fortran wrappers of MPI_Cancel.
 */
#define MPI_Cancel_L mpi_cancel

/** @def MPI_Ibsend_L
    Exchanges MPI_Ibsend_L by mpi_ibsend.
    It is used for the Fortran wrappers of MPI_Ibsend.
 */
#define MPI_Ibsend_L mpi_ibsend

/** @def MPI_Improbe_L
    Exchanges MPI_Improbe_L by mpi_improbe.
    It is used for the Fortran wrappers of MPI_Improbe.
 */
#define MPI_Improbe_L mpi_improbe

/** @def MPI_Imrecv_L
    Exchanges MPI_Imrecv_L by mpi_imrecv.
    It is used for the Fortran wrappers of MPI_Imrecv.
 */
#define MPI_Imrecv_L mpi_imrecv

/** @def MPI_Iprobe_L
    Exchanges MPI_Iprobe_L by mpi_iprobe.
    It is used for the Fortran wrappers of MPI_Iprobe.
 */
#define MPI_Iprobe_L mpi_iprobe

/** @def MPI_Irecv_L
    Exchanges MPI_Irecv_L by mpi_irecv.
    It is used for the Fortran wrappers of MPI_Irecv.
 */
#define MPI_Irecv_L mpi_irecv

/** @def MPI_Irsend_L
    Exchanges MPI_Irsend_L by mpi_irsend.
    It is used for the Fortran wrappers of MPI_Irsend.
 */
#define MPI_Irsend_L mpi_irsend

/** @def MPI_Isend_L
    Exchanges MPI_Isend_L by mpi_isend.
    It is used for the Fortran wrappers of MPI_Isend.
 */
#define MPI_Isend_L mpi_isend

/** @def MPI_Issend_L
    Exchanges MPI_Issend_L by mpi_issend.
    It is used for the Fortran wrappers of MPI_Issend.
 */
#define MPI_Issend_L mpi_issend

/** @def MPI_Mprobe_L
    Exchanges MPI_Mprobe_L by mpi_mprobe.
    It is used for the Fortran wrappers of MPI_Mprobe.
 */
#define MPI_Mprobe_L mpi_mprobe

/** @def MPI_Mrecv_L
    Exchanges MPI_Mrecv_L by mpi_mrecv.
    It is used for the Fortran wrappers of MPI_Mrecv.
 */
#define MPI_Mrecv_L mpi_mrecv

/** @def MPI_Probe_L
    Exchanges MPI_Probe_L by mpi_probe.
    It is used for the Fortran wrappers of MPI_Probe.
 */
#define MPI_Probe_L mpi_probe

/** @def MPI_Recv_L
    Exchanges MPI_Recv_L by mpi_recv.
    It is used for the Fortran wrappers of MPI_Recv.
 */
#define MPI_Recv_L mpi_recv

/** @def MPI_Recv_init_L
    Exchanges MPI_Recv_init_L by mpi_recv_init.
    It is used for the Fortran wrappers of MPI_Recv_init.
 */
#define MPI_Recv_init_L mpi_recv_init

/** @def MPI_Request_free_L
    Exchanges MPI_Request_free_L by mpi_request_free.
    It is used for the Fortran wrappers of MPI_Request_free.
 */
#define MPI_Request_free_L mpi_request_free

/** @def MPI_Rsend_L
    Exchanges MPI_Rsend_L by mpi_rsend.
    It is used for the Fortran wrappers of MPI_Rsend.
 */
#define MPI_Rsend_L mpi_rsend

/** @def MPI_Rsend_init_L
    Exchanges MPI_Rsend_init_L by mpi_rsend_init.
    It is used for the Fortran wrappers of MPI_Rsend_init.
 */
#define MPI_Rsend_init_L mpi_rsend_init

/** @def MPI_Send_L
    Exchanges MPI_Send_L by mpi_send.
    It is used for the Fortran wrappers of MPI_Send.
 */
#define MPI_Send_L mpi_send

/** @def MPI_Send_init_L
    Exchanges MPI_Send_init_L by mpi_send_init.
    It is used for the Fortran wrappers of MPI_Send_init.
 */
#define MPI_Send_init_L mpi_send_init

/** @def MPI_Sendrecv_L
    Exchanges MPI_Sendrecv_L by mpi_sendrecv.
    It is used for the Fortran wrappers of MPI_Sendrecv.
 */
#define MPI_Sendrecv_L mpi_sendrecv

/** @def MPI_Sendrecv_replace_L
    Exchanges MPI_Sendrecv_replace_L by mpi_sendrecv_replace.
    It is used for the Fortran wrappers of MPI_Sendrecv_replace.
 */
#define MPI_Sendrecv_replace_L mpi_sendrecv_replace

/** @def MPI_Ssend_L
    Exchanges MPI_Ssend_L by mpi_ssend.
    It is used for the Fortran wrappers of MPI_Ssend.
 */
#define MPI_Ssend_L mpi_ssend

/** @def MPI_Ssend_init_L
    Exchanges MPI_Ssend_init_L by mpi_ssend_init.
    It is used for the Fortran wrappers of MPI_Ssend_init.
 */
#define MPI_Ssend_init_L mpi_ssend_init

/** @def MPI_Start_L
    Exchanges MPI_Start_L by mpi_start.
    It is used for the Fortran wrappers of MPI_Start.
 */
#define MPI_Start_L mpi_start

/** @def MPI_Startall_L
    Exchanges MPI_Startall_L by mpi_startall.
    It is used for the Fortran wrappers of MPI_Startall.
 */
#define MPI_Startall_L mpi_startall

/** @def MPI_Test_L
    Exchanges MPI_Test_L by mpi_test.
    It is used for the Fortran wrappers of MPI_Test.
 */
#define MPI_Test_L mpi_test

/** @def MPI_Test_cancelled_L
    Exchanges MPI_Test_cancelled_L by mpi_test_cancelled.
    It is used for the Fortran wrappers of MPI_Test_cancelled.
 */
#define MPI_Test_cancelled_L mpi_test_cancelled

/** @def MPI_Testall_L
    Exchanges MPI_Testall_L by mpi_testall.
    It is used for the Fortran wrappers of MPI_Testall.
 */
#define MPI_Testall_L mpi_testall

/** @def MPI_Testany_L
    Exchanges MPI_Testany_L by mpi_testany.
    It is used for the Fortran wrappers of MPI_Testany.
 */
#define MPI_Testany_L mpi_testany

/** @def MPI_Testsome_L
    Exchanges MPI_Testsome_L by mpi_testsome.
    It is used for the Fortran wrappers of MPI_Testsome.
 */
#define MPI_Testsome_L mpi_testsome

/** @def MPI_Wait_L
    Exchanges MPI_Wait_L by mpi_wait.
    It is used for the Fortran wrappers of MPI_Wait.
 */
#define MPI_Wait_L mpi_wait

/** @def MPI_Waitall_L
    Exchanges MPI_Waitall_L by mpi_waitall.
    It is used for the Fortran wrappers of MPI_Waitall.
 */
#define MPI_Waitall_L mpi_waitall

/** @def MPI_Waitany_L
    Exchanges MPI_Waitany_L by mpi_waitany.
    It is used for the Fortran wrappers of MPI_Waitany.
 */
#define MPI_Waitany_L mpi_waitany

/** @def MPI_Waitsome_L
    Exchanges MPI_Waitsome_L by mpi_waitsome.
    It is used for the Fortran wrappers of MPI_Waitsome.
 */
#define MPI_Waitsome_L mpi_waitsome


#ifndef NEED_F2C_CONV

/* If MPI_Comm_f2c is HAVE as a simple macro (which typically does nothing)
 * as it would require a "real" function if it is really needed
 * => we can save the f2c and c2s conversions */

#if HAVE( DECL_PMPI_WAITANY ) && !defined( SCOREP_MPI_NO_P2P )
/**
 * Manual measurement wrapper for the Fortran interface of MPI_Waitany
 * @note C indices have to be converted to Fortran indices, when the
 *       effective index is not MPI_UNDEFINED
 * @ingroup manual_fortran_mpi_1_p2p
 */
void
FSUB( MPI_Waitany )( int*         count,
                     MPI_Request* array,
                     int*         index,
                     MPI_Status*  status,
                     int*         ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
  #if HAVE( MPI_STATUS_IGNORE )
    if ( status == scorep_mpi_fortran_status_ignore )
    {
        status = MPI_STATUS_IGNORE;
    }
  #endif

    *ierr = MPI_Waitany( *count, array, index, status );

    if ( ( *index != MPI_UNDEFINED ) && ( *index >= 0 ) )
    {
        ( *index )++;
    }
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif

#if HAVE( DECL_PMPI_WAITSOME ) && !defined( SCOREP_MPI_NO_P2P )
/**
 * Manual measurement wrapper for the Fortran interface of MPI_Waitsome
 * @note C indices have to be converted to Fortran indices, when the
 *       effective index is not MPI_UNDEFINED
 * @ingroup manual_fortran_mpi_1_p2p
 */
void
FSUB( MPI_Waitsome )( int*         incount,
                      MPI_Request* array_of_requests,
                      int*         outcount,
                      int*         array_of_indices,
                      MPI_Status*  array_of_statuses,
                      int*         ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    int i;

  #if HAVE( MPI_STATUSES_IGNORE )
    if ( array_of_statuses == scorep_mpi_fortran_statuses_ignore )
    {
        array_of_statuses = MPI_STATUSES_IGNORE;
    }
  #endif

    *ierr = MPI_Waitsome( *incount, array_of_requests, outcount,
                          array_of_indices, array_of_statuses );
    if ( *outcount != MPI_UNDEFINED )
    {
        for ( i = 0; i < *outcount; i++ )
        {
            if ( array_of_indices[ i ] >= 0 )
            {
                array_of_indices[ i ]++;
            }
        }
    }
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif

#if HAVE( DECL_PMPI_TESTANY ) && !defined( SCOREP_MPI_NO_P2P )
/**
 * Manual measurement wrapper for the Fortran interface of MPI_Testany
 * @note C indices have to be converted to Fortran indices, when the
 *       effective index is not MPI_UNDEFINED
 * @ingroup manual_fortran_mpi_1_p2p
 */
void
FSUB( MPI_Testany )( int*         count,
                     MPI_Request* array_of_requests,
                     int*         index,
                     int*         flag,
                     MPI_Status*  status,
                     int*         ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
  #if HAVE( MPI_STATUS_IGNORE )
    if ( status == scorep_mpi_fortran_status_ignore )
    {
        status = MPI_STATUS_IGNORE;
    }
  #endif

    *ierr = MPI_Testany( *count, array_of_requests, index, flag, status );

    /* convert index to Fortran */
    if ( ( *ierr == MPI_SUCCESS )
         && *flag
         && ( *index != MPI_UNDEFINED )
         && ( *index >= 0 ) )
    {
        ( *index )++;
    }
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif

#if HAVE( DECL_PMPI_TESTSOME ) && !defined( SCOREP_MPI_NO_P2P )
/**
 * Manual measurement wrapper for the Fortran interface of MPI_Testsome
 * @note C indices have to be converted to Fortran indices, when the
 *       effective index is not MPI_UNDEFINED
 * @ingroup manual_fortran_mpi_1_p2p
 */
void
FSUB( MPI_Testsome )( int*         incount,
                      MPI_Request* array_of_requests,
                      int*         outcount,
                      int*         array_of_indices,
                      MPI_Status*  array_of_statuses,
                      int*         ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    int i;

  #if HAVE( MPI_STATUSES_IGNORE )
    if ( array_of_statuses == scorep_mpi_fortran_statuses_ignore )
    {
        array_of_statuses = MPI_STATUSES_IGNORE;
    }
  #endif

    *ierr = MPI_Testsome( *incount, array_of_requests, outcount, array_of_indices,
                          array_of_statuses );

    /* convert indices to Fortran */
    if ( ( *ierr == MPI_SUCCESS ) && ( *outcount != MPI_UNDEFINED ) )
    {
        for ( i = 0; i < *outcount; i++ )
        {
            if ( array_of_indices[ i ] >= 0 )
            {
                array_of_indices[ i ]++;
            }
        }
    }
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif

#if HAVE( DECL_PMPI_BSEND ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Bsend )
/**
 * Measurement wrapper for MPI_Bsend
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup p2p
 */
void
FSUB( MPI_Bsend )( void* buf, int* count, MPI_Datatype* datatype, int* dest, int* tag, MPI_Comm* comm, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    #if HAVE( MPI_BOTTOM )
    if ( buf == scorep_mpi_fortran_bottom )
    {
        buf = MPI_BOTTOM;
    }
    #endif


    *ierr = MPI_Bsend( buf, *count, *datatype, *dest, *tag, *comm );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_BSEND_INIT ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Bsend_init )
/**
 * Measurement wrapper for MPI_Bsend_init
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup p2p
 */
void
FSUB( MPI_Bsend_init )( void* buf, int* count, MPI_Datatype* datatype, int* dest, int* tag, MPI_Comm* comm, MPI_Request* request, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Bsend_init( buf, *count, *datatype, *dest, *tag, *comm, request );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_BUFFER_ATTACH ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Buffer_attach )
/**
 * Measurement wrapper for MPI_Buffer_attach
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup p2p
 */
void
FSUB( MPI_Buffer_attach )( void* buffer, int* size, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Buffer_attach( buffer, *size );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_BUFFER_DETACH ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Buffer_detach )
/**
 * Measurement wrapper for MPI_Buffer_detach
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup p2p
 */
void
FSUB( MPI_Buffer_detach )( void* buffer, int* size, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Buffer_detach( buffer, size );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_CANCEL ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Cancel )
/**
 * Measurement wrapper for MPI_Cancel
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup p2p
 */
void
FSUB( MPI_Cancel )( MPI_Request* request, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Cancel( request );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_IBSEND ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Ibsend )
/**
 * Measurement wrapper for MPI_Ibsend
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup p2p
 */
void
FSUB( MPI_Ibsend )( void* buf, int* count, MPI_Datatype* datatype, int* dest, int* tag, MPI_Comm* comm, MPI_Request* request, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    #if HAVE( MPI_BOTTOM )
    if ( buf == scorep_mpi_fortran_bottom )
    {
        buf = MPI_BOTTOM;
    }
    #endif


    *ierr = MPI_Ibsend( buf, *count, *datatype, *dest, *tag, *comm, request );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_IMPROBE ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Improbe )
/**
 * Measurement wrapper for MPI_Improbe
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-3
 * @ingroup p2p
 */
void
FSUB( MPI_Improbe )( int* source, int* tag, MPI_Comm* comm, int* flag, MPI_Message* message, MPI_Status* status, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    #if HAVE( MPI_STATUS_IGNORE )
    if ( status == scorep_mpi_fortran_status_ignore )
    {
        status = MPI_STATUS_IGNORE;
    }
    #endif


    *ierr = MPI_Improbe( *source, *tag, *comm, flag, message, status );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_IMRECV ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Imrecv )
/**
 * Measurement wrapper for MPI_Imrecv
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-3
 * @ingroup p2p
 */
void
FSUB( MPI_Imrecv )( void* buf, int* count, MPI_Datatype* datatype, MPI_Message* message, MPI_Request* request, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    #if HAVE( MPI_BOTTOM )
    if ( buf == scorep_mpi_fortran_bottom )
    {
        buf = MPI_BOTTOM;
    }
    #endif


    *ierr = MPI_Imrecv( buf, *count, *datatype, message, request );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_IPROBE ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Iprobe )
/**
 * Measurement wrapper for MPI_Iprobe
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup p2p
 */
void
FSUB( MPI_Iprobe )( int* source, int* tag, MPI_Comm* comm, int* flag, MPI_Status* status, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    #if HAVE( MPI_STATUS_IGNORE )
    if ( status == scorep_mpi_fortran_status_ignore )
    {
        status = MPI_STATUS_IGNORE;
    }
    #endif


    *ierr = MPI_Iprobe( *source, *tag, *comm, flag, status );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_IRECV ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Irecv )
/**
 * Measurement wrapper for MPI_Irecv
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup p2p
 */
void
FSUB( MPI_Irecv )( void* buf, int* count, MPI_Datatype* datatype, int* source, int* tag, MPI_Comm* comm, MPI_Request* request, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    #if HAVE( MPI_BOTTOM )
    if ( buf == scorep_mpi_fortran_bottom )
    {
        buf = MPI_BOTTOM;
    }
    #endif


    *ierr = MPI_Irecv( buf, *count, *datatype, *source, *tag, *comm, request );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_IRSEND ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Irsend )
/**
 * Measurement wrapper for MPI_Irsend
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup p2p
 */
void
FSUB( MPI_Irsend )( void* buf, int* count, MPI_Datatype* datatype, int* dest, int* tag, MPI_Comm* comm, MPI_Request* request, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    #if HAVE( MPI_BOTTOM )
    if ( buf == scorep_mpi_fortran_bottom )
    {
        buf = MPI_BOTTOM;
    }
    #endif


    *ierr = MPI_Irsend( buf, *count, *datatype, *dest, *tag, *comm, request );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_ISEND ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Isend )
/**
 * Measurement wrapper for MPI_Isend
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup p2p
 */
void
FSUB( MPI_Isend )( void* buf, int* count, MPI_Datatype* datatype, int* dest, int* tag, MPI_Comm* comm, MPI_Request* request, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    #if HAVE( MPI_BOTTOM )
    if ( buf == scorep_mpi_fortran_bottom )
    {
        buf = MPI_BOTTOM;
    }
    #endif


    *ierr = MPI_Isend( buf, *count, *datatype, *dest, *tag, *comm, request );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_ISSEND ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Issend )
/**
 * Measurement wrapper for MPI_Issend
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup p2p
 */
void
FSUB( MPI_Issend )( void* buf, int* count, MPI_Datatype* datatype, int* dest, int* tag, MPI_Comm* comm, MPI_Request* request, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    #if HAVE( MPI_BOTTOM )
    if ( buf == scorep_mpi_fortran_bottom )
    {
        buf = MPI_BOTTOM;
    }
    #endif


    *ierr = MPI_Issend( buf, *count, *datatype, *dest, *tag, *comm, request );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_MPROBE ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Mprobe )
/**
 * Measurement wrapper for MPI_Mprobe
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-3
 * @ingroup p2p
 */
void
FSUB( MPI_Mprobe )( int* source, int* tag, MPI_Comm* comm, MPI_Message* message, MPI_Status* status, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    #if HAVE( MPI_STATUS_IGNORE )
    if ( status == scorep_mpi_fortran_status_ignore )
    {
        status = MPI_STATUS_IGNORE;
    }
    #endif


    *ierr = MPI_Mprobe( *source, *tag, *comm, message, status );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_MRECV ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Mrecv )
/**
 * Measurement wrapper for MPI_Mrecv
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-3
 * @ingroup p2p
 */
void
FSUB( MPI_Mrecv )( void* buf, int* count, MPI_Datatype* datatype, MPI_Message* message, MPI_Status* status, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    #if HAVE( MPI_STATUS_IGNORE )
    if ( status == scorep_mpi_fortran_status_ignore )
    {
        status = MPI_STATUS_IGNORE;
    }
    #endif
    #if HAVE( MPI_BOTTOM )
    if ( buf == scorep_mpi_fortran_bottom )
    {
        buf = MPI_BOTTOM;
    }
    #endif


    *ierr = MPI_Mrecv( buf, *count, *datatype, message, status );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_PROBE ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Probe )
/**
 * Measurement wrapper for MPI_Probe
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup p2p
 */
void
FSUB( MPI_Probe )( int* source, int* tag, MPI_Comm* comm, MPI_Status* status, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    #if HAVE( MPI_STATUS_IGNORE )
    if ( status == scorep_mpi_fortran_status_ignore )
    {
        status = MPI_STATUS_IGNORE;
    }
    #endif


    *ierr = MPI_Probe( *source, *tag, *comm, status );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_RECV ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Recv )
/**
 * Measurement wrapper for MPI_Recv
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup p2p
 */
void
FSUB( MPI_Recv )( void* buf, int* count, MPI_Datatype* datatype, int* source, int* tag, MPI_Comm* comm, MPI_Status* status, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    #if HAVE( MPI_STATUS_IGNORE )
    if ( status == scorep_mpi_fortran_status_ignore )
    {
        status = MPI_STATUS_IGNORE;
    }
    #endif
    #if HAVE( MPI_BOTTOM )
    if ( buf == scorep_mpi_fortran_bottom )
    {
        buf = MPI_BOTTOM;
    }
    #endif


    *ierr = MPI_Recv( buf, *count, *datatype, *source, *tag, *comm, status );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_RECV_INIT ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Recv_init )
/**
 * Measurement wrapper for MPI_Recv_init
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup p2p
 */
void
FSUB( MPI_Recv_init )( void* buf, int* count, MPI_Datatype* datatype, int* source, int* tag, MPI_Comm* comm, MPI_Request* request, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    #if HAVE( MPI_BOTTOM )
    if ( buf == scorep_mpi_fortran_bottom )
    {
        buf = MPI_BOTTOM;
    }
    #endif


    *ierr = MPI_Recv_init( buf, *count, *datatype, *source, *tag, *comm, request );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_REQUEST_FREE ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Request_free )
/**
 * Measurement wrapper for MPI_Request_free
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup p2p
 */
void
FSUB( MPI_Request_free )( MPI_Request* request, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Request_free( request );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_RSEND ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Rsend )
/**
 * Measurement wrapper for MPI_Rsend
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup p2p
 */
void
FSUB( MPI_Rsend )( void* buf, int* count, MPI_Datatype* datatype, int* dest, int* tag, MPI_Comm* comm, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    #if HAVE( MPI_BOTTOM )
    if ( buf == scorep_mpi_fortran_bottom )
    {
        buf = MPI_BOTTOM;
    }
    #endif


    *ierr = MPI_Rsend( buf, *count, *datatype, *dest, *tag, *comm );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_RSEND_INIT ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Rsend_init )
/**
 * Measurement wrapper for MPI_Rsend_init
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup p2p
 */
void
FSUB( MPI_Rsend_init )( void* buf, int* count, MPI_Datatype* datatype, int* dest, int* tag, MPI_Comm* comm, MPI_Request* request, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    #if HAVE( MPI_BOTTOM )
    if ( buf == scorep_mpi_fortran_bottom )
    {
        buf = MPI_BOTTOM;
    }
    #endif


    *ierr = MPI_Rsend_init( buf, *count, *datatype, *dest, *tag, *comm, request );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_SEND ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Send )
/**
 * Measurement wrapper for MPI_Send
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup p2p
 */
void
FSUB( MPI_Send )( void* buf, int* count, MPI_Datatype* datatype, int* dest, int* tag, MPI_Comm* comm, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    #if HAVE( MPI_BOTTOM )
    if ( buf == scorep_mpi_fortran_bottom )
    {
        buf = MPI_BOTTOM;
    }
    #endif


    *ierr = MPI_Send( buf, *count, *datatype, *dest, *tag, *comm );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_SEND_INIT ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Send_init )
/**
 * Measurement wrapper for MPI_Send_init
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup p2p
 */
void
FSUB( MPI_Send_init )( void* buf, int* count, MPI_Datatype* datatype, int* dest, int* tag, MPI_Comm* comm, MPI_Request* request, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    #if HAVE( MPI_BOTTOM )
    if ( buf == scorep_mpi_fortran_bottom )
    {
        buf = MPI_BOTTOM;
    }
    #endif


    *ierr = MPI_Send_init( buf, *count, *datatype, *dest, *tag, *comm, request );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_SENDRECV ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Sendrecv )
/**
 * Measurement wrapper for MPI_Sendrecv
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup p2p
 */
void
FSUB( MPI_Sendrecv )( void* sendbuf, int* sendcount, MPI_Datatype* sendtype, int* dest, int* sendtag, void* recvbuf, int* recvcount, MPI_Datatype* recvtype, int* source, int* recvtag, MPI_Comm* comm, MPI_Status* status, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    #if HAVE( MPI_BOTTOM )
    if ( sendbuf == scorep_mpi_fortran_bottom )
    {
        sendbuf = MPI_BOTTOM;
    }
    #endif
    #if HAVE( MPI_BOTTOM )
    if ( recvbuf == scorep_mpi_fortran_bottom )
    {
        recvbuf = MPI_BOTTOM;
    }
    #endif
    #if HAVE( MPI_STATUS_IGNORE )
    if ( status == scorep_mpi_fortran_status_ignore )
    {
        status = MPI_STATUS_IGNORE;
    }
    #endif


    *ierr = MPI_Sendrecv( sendbuf, *sendcount, *sendtype, *dest, *sendtag, recvbuf, *recvcount, *recvtype, *source, *recvtag, *comm, status );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_SENDRECV_REPLACE ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Sendrecv_replace )
/**
 * Measurement wrapper for MPI_Sendrecv_replace
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup p2p
 */
void
FSUB( MPI_Sendrecv_replace )( void* buf, int* count, MPI_Datatype* datatype, int* dest, int* sendtag, int* source, int* recvtag, MPI_Comm* comm, MPI_Status* status, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    #if HAVE( MPI_BOTTOM )
    if ( buf == scorep_mpi_fortran_bottom )
    {
        buf = MPI_BOTTOM;
    }
    #endif
    #if HAVE( MPI_STATUS_IGNORE )
    if ( status == scorep_mpi_fortran_status_ignore )
    {
        status = MPI_STATUS_IGNORE;
    }
    #endif


    *ierr = MPI_Sendrecv_replace( buf, *count, *datatype, *dest, *sendtag, *source, *recvtag, *comm, status );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_SSEND ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Ssend )
/**
 * Measurement wrapper for MPI_Ssend
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup p2p
 */
void
FSUB( MPI_Ssend )( void* buf, int* count, MPI_Datatype* datatype, int* dest, int* tag, MPI_Comm* comm, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    #if HAVE( MPI_BOTTOM )
    if ( buf == scorep_mpi_fortran_bottom )
    {
        buf = MPI_BOTTOM;
    }
    #endif


    *ierr = MPI_Ssend( buf, *count, *datatype, *dest, *tag, *comm );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_SSEND_INIT ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Ssend_init )
/**
 * Measurement wrapper for MPI_Ssend_init
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup p2p
 */
void
FSUB( MPI_Ssend_init )( void* buf, int* count, MPI_Datatype* datatype, int* dest, int* tag, MPI_Comm* comm, MPI_Request* request, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    #if HAVE( MPI_BOTTOM )
    if ( buf == scorep_mpi_fortran_bottom )
    {
        buf = MPI_BOTTOM;
    }
    #endif


    *ierr = MPI_Ssend_init( buf, *count, *datatype, *dest, *tag, *comm, request );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_START ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Start )
/**
 * Measurement wrapper for MPI_Start
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup p2p
 */
void
FSUB( MPI_Start )( MPI_Request* request, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Start( request );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_STARTALL ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Startall )
/**
 * Measurement wrapper for MPI_Startall
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup p2p
 */
void
FSUB( MPI_Startall )( int* count, MPI_Request* array_of_requests, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Startall( *count, array_of_requests );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_TEST ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Test )
/**
 * Measurement wrapper for MPI_Test
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup p2p
 */
void
FSUB( MPI_Test )( MPI_Request* request, int* flag, MPI_Status* status, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    #if HAVE( MPI_STATUS_IGNORE )
    if ( status == scorep_mpi_fortran_status_ignore )
    {
        status = MPI_STATUS_IGNORE;
    }
    #endif


    *ierr = MPI_Test( request, flag, status );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_TEST_CANCELLED ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Test_cancelled )
/**
 * Measurement wrapper for MPI_Test_cancelled
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup p2p
 */
void
FSUB( MPI_Test_cancelled )( MPI_Status* status, int* flag, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Test_cancelled( status, flag );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_TESTALL ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Testall )
/**
 * Measurement wrapper for MPI_Testall
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup p2p
 */
void
FSUB( MPI_Testall )( int* count, MPI_Request* array_of_requests, int* flag, MPI_Status* array_of_statuses, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    #if HAVE( MPI_STATUSES_IGNORE )
    if ( array_of_statuses == scorep_mpi_fortran_statuses_ignore )
    {
        array_of_statuses = MPI_STATUSES_IGNORE;
    }
    #endif


    *ierr = MPI_Testall( *count, array_of_requests, flag, array_of_statuses );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_WAIT ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Wait )
/**
 * Measurement wrapper for MPI_Wait
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup p2p
 */
void
FSUB( MPI_Wait )( MPI_Request* request, MPI_Status* status, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    #if HAVE( MPI_STATUS_IGNORE )
    if ( status == scorep_mpi_fortran_status_ignore )
    {
        status = MPI_STATUS_IGNORE;
    }
    #endif


    *ierr = MPI_Wait( request, status );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_WAITALL ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Waitall )
/**
 * Measurement wrapper for MPI_Waitall
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup p2p
 */
void
FSUB( MPI_Waitall )( int* count, MPI_Request* array_of_requests, MPI_Status* array_of_statuses, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    #if HAVE( MPI_STATUSES_IGNORE )
    if ( array_of_statuses == scorep_mpi_fortran_statuses_ignore )
    {
        array_of_statuses = MPI_STATUSES_IGNORE;
    }
    #endif


    *ierr = MPI_Waitall( *count, array_of_requests, array_of_statuses );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif

#else /* !NEED_F2C_CONV */

/**
 * Wrapper for a statically allocated request array that can be reused
 * in different calls. It will be enlarged automatically, when needed.
 * @ingroup utility_functions
 */
static MPI_Request*
alloc_request_array( int count )
{
    static MPI_Request* local_req_arr      = 0;
    static int          local_req_arr_size = 0;

    if ( local_req_arr_size == 0 )
    {
        /* -- never used: initialize -- */
        local_req_arr      = malloc( 2 * count * sizeof( MPI_Request ) );
        local_req_arr_size = 2 * count;
    }
    else if ( count > local_req_arr_size )
    {
        /* -- not enough room: expand -- */
        local_req_arr      = realloc( local_req_arr, count * sizeof( MPI_Request ) );
        local_req_arr_size = count;
    }
    return local_req_arr;
}

/**
 * Wrapper for a statically allocated status array that can be reused
 * in different calls. It will be enlarged automatically, when needed.
 * @ingroup utility_functions
 */
static MPI_Status*
alloc_status_array( int count )
{
    static MPI_Status* local_stat_arr      = 0;
    static int         local_stat_arr_size = 0;

    if ( local_stat_arr_size == 0 )
    {
        /* -- never used: initialize -- */
        local_stat_arr      = malloc( 2 * count * sizeof( MPI_Status ) );
        local_stat_arr_size = 2 * count;
    }
    else if ( count > local_stat_arr_size )
    {
        /* -- not enough room: expand -- */
        local_stat_arr      = realloc( local_stat_arr, count * sizeof( MPI_Status ) );
        local_stat_arr_size = count;
    }
    return local_stat_arr;
}

#if HAVE( DECL_PMPI_WAIT ) && !defined( SCOREP_MPI_NO_P2P )
/**
 * Manual measurement wrapper for the Fortran interface of MPI_Wait,
 * when handle conversion is needed.
 * @ingroup manual_f2c_c2f_mpi_1_p2p
 */
void
FSUB( MPI_Wait )( MPI_Fint* request,
                  MPI_Fint* status,
                  MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Request lrequest;
    MPI_Status  c_status;
    MPI_Status* c_status_ptr = &c_status;

    lrequest = PMPI_Request_f2c( *request );
    *request = PMPI_Request_c2f( lrequest );

  #if HAVE( MPI_STATUS_IGNORE )
    if ( status == scorep_mpi_fortran_status_ignore )
    {
        c_status_ptr = MPI_STATUS_IGNORE;
    }
  #endif

    *ierr = MPI_Wait( &lrequest, c_status_ptr );

    if ( *ierr == MPI_SUCCESS )
    {
    #if HAVE( MPI_STATUS_IGNORE )
        if ( status != scorep_mpi_fortran_status_ignore )
    #endif
        {
            PMPI_Status_c2f( c_status_ptr, status );
        }
    }
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif

#if HAVE( DECL_PMPI_WAITALL ) && !defined( SCOREP_MPI_NO_P2P )
/**
 * Manual measurement wrapper for the Fortran interface of MPI_Waitall,
 * when handle conversion is needed.
 * @ingroup manual_f2c_c2f_mpi_1_p2p
 */
void
FSUB( MPI_Waitall )( MPI_Fint* count,
                     MPI_Fint  array_of_requests[],
                     MPI_Fint  array_of_statuses[],
                     MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    int          i;
    MPI_Request* lrequest = NULL;
    MPI_Status*  c_status = NULL;

    if ( *count > 0 )
    {
        lrequest = alloc_request_array( *count );

    #if HAVE( MPI_STATUSES_IGNORE )
        if ( array_of_statuses != scorep_mpi_fortran_statuses_ignore )
    #endif
        {
            c_status = alloc_status_array( *count );
        }
    #if HAVE( MPI_STATUSES_IGNORE )
        else
        {
            c_status = MPI_STATUSES_IGNORE;
        }
    #endif

        for ( i = 0; i < *count; i++ )
        {
            lrequest[ i ] = PMPI_Request_f2c( array_of_requests[ i ] );
        }
    }

    *ierr = MPI_Waitall( *count, lrequest, c_status );

    for ( i = 0; i < *count; i++ )
    {
        array_of_requests[ i ] = PMPI_Request_c2f( lrequest[ i ] );
    }
    if ( *ierr == MPI_SUCCESS )
    {
    #if HAVE( MPI_STATUSES_IGNORE )
        if ( array_of_statuses != scorep_mpi_fortran_statuses_ignore )
    #endif
        {
            for ( i = 0; i < *count; i++ )
            {
                PMPI_Status_c2f( &( c_status[ i ] ), &( array_of_statuses[ i * scorep_mpi_status_size ] ) );
            }
        }
    }
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif

#if HAVE( DECL_PMPI_WAITANY ) && !defined( SCOREP_MPI_NO_P2P )
/**
 * Manual measurement wrapper for the Fortran interface of MPI_Waitany,
 * when handle conversion is needed.
 * @note C index has to be converted to Fortran index, only if it is not
 *       MPI_UNDEFINED
 * @ingroup manual_f2c_c2f_mpi_1_p2p
 */
void
FSUB( MPI_Waitany )( MPI_Fint* count,
                     MPI_Fint  array_of_requests[],
                     MPI_Fint* index,
                     MPI_Fint* status,
                     MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    int          i;
    MPI_Request* lrequest = NULL;
    MPI_Status   c_status;
    MPI_Status*  c_status_ptr = &c_status;

    if ( *count > 0 )
    {
        lrequest = alloc_request_array( *count );
        for ( i = 0; i < *count; i++ )
        {
            lrequest[ i ] = PMPI_Request_f2c( array_of_requests[ i ] );
        }
    }

  #if HAVE( MPI_STATUS_IGNORE )
    if ( status == scorep_mpi_fortran_status_ignore )
    {
        c_status_ptr = MPI_STATUS_IGNORE;
    }
  #endif

    *ierr = MPI_Waitany( *count, lrequest, index, c_status_ptr );

    if ( ( *ierr == MPI_SUCCESS ) && ( *index != MPI_UNDEFINED ) )
    {
        if ( *index >= 0 )
        {
            array_of_requests[ *index ] = PMPI_Request_c2f( lrequest[ *index ] );

            /* See the description of waitany in the standard;
               the Fortran index ranges are from 1, not zero */
            ( *index )++;
        }
    #if HAVE( MPI_STATUS_IGNORE )
        if ( status != scorep_mpi_fortran_status_ignore )
    #endif
        {
            PMPI_Status_c2f( &c_status, status );
        }
    }
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif

#if HAVE( DECL_PMPI_WAITSOME ) && !defined( SCOREP_MPI_NO_P2P )
/**
 * Manual measurement wrapper for the Fortran interface of MPI_Waitsome,
 * when handle conversion is needed.
 * @note C indices have to be converted to Fortran indices, only if the
 *       outcount is not MPI_UNDEFINED
 * @ingroup manual_f2c_c2f_mpi_1_p2p
 */
void
FSUB( MPI_Waitsome )( MPI_Fint* incount,
                      MPI_Fint  array_of_requests[],
                      MPI_Fint* outcount,
                      MPI_Fint  indices[],
                      MPI_Fint  array_of_statuses[],
                      MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    int          i, j, found;
    MPI_Request* lrequest = NULL;
    MPI_Status*  c_status = NULL;

    /* get addresses of internal buffers, and convert input requests */
    if ( *incount > 0 )
    {
        lrequest = alloc_request_array( *incount );

    #if HAVE( MPI_STATUSES_IGNORE )
        if ( array_of_statuses != scorep_mpi_fortran_statuses_ignore )
    #endif
        {
            c_status = alloc_status_array( *incount );
        }
    #if HAVE( MPI_STATUSES_IGNORE )
        else
        {
            c_status = MPI_STATUSES_IGNORE;
        }
    #endif

        for ( i = 0; i < *incount; i++ )
        {
            lrequest[ i ] = PMPI_Request_f2c( array_of_requests[ i ] );
        }
    }

    /* call C wrapper function */
    *ierr = MPI_Waitsome( *incount, lrequest, outcount, indices, c_status );

    /* convert requests and indices back to fortran */
    if ( ( *ierr == MPI_SUCCESS ) && ( *outcount != MPI_UNDEFINED ) )
    {
        for ( i = 0; i < *incount; i++ )
        {
            if ( i < *outcount )
            {
                if ( indices[ i ] >= 0 )
                {
                    array_of_requests[ indices[ i ] ] = PMPI_Request_c2f( lrequest[ indices[ i ] ] );
                }
            }
            else
            {
                found = j = 0;
                while ( ( !found ) && ( j < *outcount ) )
                {
                    if ( indices[ j++ ] == i )
                    {
                        found = 1;
                    }
                }

                if ( !found )
                {
                    array_of_requests[ i ] = PMPI_Request_c2f( lrequest[ i ] );
                }
            }
        }

    #if HAVE( MPI_STATUSES_IGNORE )
        if ( array_of_statuses != scorep_mpi_fortran_statuses_ignore )
    #endif
        {
            for ( i = 0; i < *outcount; i++ )
            {
                PMPI_Status_c2f( &c_status[ i ], &( array_of_statuses[ i * scorep_mpi_status_size ] ) );
                /* See the description of waitsome in the standard;
                   the Fortran index ranges are from 1, not zero */
                if ( indices[ i ] >= 0 )
                {
                    indices[ i ]++;
                }
            }
        }
    }
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif

#if HAVE( DECL_PMPI_TEST ) && !defined( SCOREP_MPI_NO_P2P )
/**
 * Manual measurement wrapper for the Fortran interface of MPI_Test,
 * when handle conversion is needed.
 * @ingroup manual_f2c_c2f_mpi_1_p2p
 */
void
FSUB( MPI_Test )( MPI_Fint* request,
                  MPI_Fint* flag,
                  MPI_Fint* status,
                  MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Status  c_status;
    MPI_Status* c_status_ptr = &c_status;
    MPI_Request lrequest     = PMPI_Request_f2c( *request );

  #if HAVE( MPI_STATUS_IGNORE )
    if ( status == scorep_mpi_fortran_status_ignore )
    {
        c_status_ptr = MPI_STATUS_IGNORE;
    }
  #endif

    *ierr = MPI_Test( &lrequest, flag, c_status_ptr );

    if ( *ierr != MPI_SUCCESS )
    {
        return;
    }
    *request = PMPI_Request_c2f( lrequest );
    if ( flag )
    {
    #if HAVE( MPI_STATUS_IGNORE )
        if ( status != scorep_mpi_fortran_status_ignore )
    #endif
        {
            PMPI_Status_c2f( &c_status, status );
        }
    }
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif

#if HAVE( DECL_PMPI_TESTANY ) && !defined( SCOREP_MPI_NO_P2P )
/**
 * Manual measurement wrapper for the Fortran interface of MPI_Testany,
 * when handle conversion is needed.
 * @note C index has to be converted to Fortran index, only if it is not
 *       MPI_UNDEFINED
 * @ingroup manual_f2c_c2f_mpi_1_p2p
 */
void
FSUB( MPI_Testany )( MPI_Fint* count,
                     MPI_Fint  array_of_requests[],
                     MPI_Fint* index,
                     MPI_Fint* flag,
                     MPI_Fint* status,
                     MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    int          i;
    MPI_Request* lrequest = NULL;
    MPI_Status   c_status;
    MPI_Status*  c_status_ptr = &c_status;

    if ( *count > 0 )
    {
        lrequest = alloc_request_array( *count );
        for ( i = 0; i < *count; i++ )
        {
            lrequest[ i ] = PMPI_Request_f2c( array_of_requests[ i ] );
        }
    }

  #if HAVE( MPI_STATUS_IGNORE )
    if ( status == scorep_mpi_fortran_status_ignore )
    {
        c_status_ptr = MPI_STATUS_IGNORE;
    }
  #endif

    *ierr = MPI_Testany( *count, lrequest, index, flag, c_status_ptr );

    if ( ( *ierr == MPI_SUCCESS ) && ( *index != MPI_UNDEFINED ) )
    {
        if ( *flag && *index >= 0 )
        {
            /* index may be MPI_UNDEFINED if all are null */
            array_of_requests[ *index ] = PMPI_Request_c2f( lrequest[ *index ] );

            /* See the description of waitany in the standard;
               the Fortran index ranges are from 1, not zero */
            ( *index )++;
        }

    #if HAVE( MPI_STATUS_IGNORE )
        if ( status != scorep_mpi_fortran_status_ignore )
    #endif
        {
            PMPI_Status_c2f( &c_status, status );
        }
    }
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif

#if HAVE( DECL_PMPI_TESTALL ) && !defined( SCOREP_MPI_NO_P2P )
/**
 * Manual measurement wrapper for the Fortran interface of MPI_Testall,
 * when handle conversion is needed.
 * @note C indices have to be converted to Fortran indices
 * @ingroup manual_f2c_c2f_mpi_1_p2p
 */
void
FSUB( MPI_Testall )( MPI_Fint* count,
                     MPI_Fint  array_of_requests[],
                     MPI_Fint* flag,
                     MPI_Fint  array_of_statuses[],
                     MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    int          i;
    MPI_Request* lrequest = NULL;
    MPI_Status*  c_status = NULL;

    if ( *count > 0 )
    {
        lrequest = alloc_request_array( *count );

    #if HAVE( MPI_STATUSES_IGNORE )
        if ( array_of_statuses != scorep_mpi_fortran_statuses_ignore )
    #endif
        {
            c_status = alloc_status_array( *count );
        }
    #if HAVE( MPI_STATUSES_IGNORE )
        else
        {
            c_status = MPI_STATUSES_IGNORE;
        }
    #endif

        for ( i = 0; i < *count; i++ )
        {
            lrequest[ i ] = PMPI_Request_f2c( array_of_requests[ i ] );
        }
    }

    *ierr = MPI_Testall( *count, lrequest, flag, c_status );

    for ( i = 0; i < *count; i++ )
    {
        array_of_requests[ i ] = PMPI_Request_c2f( lrequest[ i ] );
    }
    if ( *ierr == MPI_SUCCESS && *flag )
    {
    #if HAVE( MPI_STATUSES_IGNORE )
        if ( array_of_statuses != scorep_mpi_fortran_statuses_ignore )
    #endif
        {
            for ( i = 0; i < *count; i++ )
            {
                PMPI_Status_c2f( &( c_status[ i ] ), &( array_of_statuses[ i * scorep_mpi_status_size ] ) );
            }
        }
    }
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif

#if HAVE( DECL_PMPI_TESTSOME ) && !defined( SCOREP_MPI_NO_P2P )
/**
 * Manual measurement wrapper for the Fortran interface of MPI_Testsome,
 * when handle conversion is needed.
 * @note C indices have to be converted to Fortran indices, only if the
 *       outcount is not MPI_UNDEFINED
 * @ingroup manual_f2c_c2f_mpi_1_p2p
 */
void
FSUB( MPI_Testsome )( MPI_Fint* incount,
                      MPI_Fint  array_of_requests[],
                      MPI_Fint* outcount,
                      MPI_Fint  indices[],
                      MPI_Fint  array_of_statuses[],
                      MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    int          i, j, found;
    MPI_Request* lrequest = NULL;
    MPI_Status*  c_status = NULL;

    if ( *incount > 0 )
    {
        lrequest = alloc_request_array( *incount );

    #if HAVE( MPI_STATUSES_IGNORE )
        if ( array_of_statuses != scorep_mpi_fortran_statuses_ignore )
    #endif
        {
            c_status = alloc_status_array( *incount );
        }
    #if HAVE( MPI_STATUSES_IGNORE )
        else
        {
            c_status = MPI_STATUSES_IGNORE;
        }
    #endif

        for ( i = 0; i < *incount; i++ )
        {
            lrequest[ i ] = PMPI_Request_f2c( array_of_requests[ i ] );
        }
    }

    *ierr = MPI_Testsome( *incount, lrequest, outcount, indices, c_status );

    if ( ( *ierr == MPI_SUCCESS ) && ( *outcount != MPI_UNDEFINED ) )
    {
        for ( i = 0; i < *incount; i++ )
        {
            if ( i < *outcount )
            {
                array_of_requests[ indices[ i ] ] = PMPI_Request_c2f( lrequest[ indices[ i ] ] );
            }
            else
            {
                found = j = 0;
                while ( ( !found ) && ( j < *outcount ) )
                {
                    if ( indices[ j++ ] == i )
                    {
                        found = 1;
                    }
                }
                if ( !found )
                {
                    array_of_requests[ i ] = PMPI_Request_c2f( lrequest[ i ] );
                }
            }
        }

    #if HAVE( MPI_STATUSES_IGNORE )
        if ( array_of_statuses != scorep_mpi_fortran_statuses_ignore )
    #endif
        {
            for ( i = 0; i < *outcount; i++ )
            {
                PMPI_Status_c2f( &c_status[ i ], &( array_of_statuses[ i * scorep_mpi_status_size ] ) );
                /* See the description of testsome in the standard;
                   the Fortran index ranges are from 1, not zero */
                if ( indices[ i ] >= 0 )
                {
                    indices[ i ]++;
                }
            }
        }
    }
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif

#if HAVE( DECL_PMPI_STARTALL ) && !defined( SCOREP_MPI_NO_P2P )
/**
 * Manual measurement wrapper for the Fortran interface of MPI_Startall,
 * when handle conversion is needed.
 * @ingroup manual_f2c_c2f_mpi_1_p2p
 */
void
FSUB( MPI_Startall )( MPI_Fint* count,
                      MPI_Fint  array_of_requests[],
                      MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    int          i;
    MPI_Request* lrequest = NULL;

    if ( *count > 0 )
    {
        lrequest = alloc_request_array( *count );
        for ( i = 0; i < *count; i++ )
        {
            lrequest[ i ] = PMPI_Request_f2c( array_of_requests[ i ] );
        }
    }

    *ierr = MPI_Startall( *count, lrequest );

    if ( *ierr == MPI_SUCCESS )
    {
        for ( i = 0; i < *count; i++ )
        {
            array_of_requests[ i ] = PMPI_Request_c2f( lrequest[ i ] );
        }
    }
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif

#if HAVE( DECL_PMPI_REQUEST_FREE ) && !defined( SCOREP_MPI_NO_P2P )
/**
 * Manual measurement wrapper for the Fortran interface of MPI_Testall,
 * when handle conversion is needed.
 * @ingroup manual_f2c_c2f_mpi_1_p2p
 */
void
FSUB( MPI_Request_free )( MPI_Fint* request,
                          MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Request lrequest = PMPI_Request_f2c( *request );

    *ierr = MPI_Request_free( &lrequest );

    if ( *ierr == MPI_SUCCESS )
    {
        *request = PMPI_Request_c2f( lrequest );
    }
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif

#if HAVE( DECL_PMPI_CANCEL ) && !defined( SCOREP_MPI_NO_P2P )
/**
 * Manual measurement wrapper for the Fortran interface of MPI_Testall,
 * when handle conversion is needed.
 * @note C indices have to be converted to Fortran indices, when the
 *       effective index is not MPI_UNDEFINED
 * @ingroup manual_f2c_c2f_mpi_1_p2p
 */
void
FSUB( MPI_Cancel )( MPI_Fint* request,
                    MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Request lrequest;

    lrequest = PMPI_Request_f2c( *request );
    *ierr    = MPI_Cancel( &lrequest );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif

#if HAVE( DECL_PMPI_BSEND ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Bsend )
/**
 * Measurement wrapper for MPI_Bsend
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup p2p
 * For the order of events see @ref MPI_Bsend
 */
void
FSUB( MPI_Bsend )( void* buf, MPI_Fint* count, MPI_Fint* datatype, MPI_Fint* dest, MPI_Fint* tag, MPI_Fint* comm, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    #if HAVE( MPI_BOTTOM )
    if ( buf == scorep_mpi_fortran_bottom )
    {
        buf = MPI_BOTTOM;
    }
    #endif


    *ierr = MPI_Bsend( buf, *count, PMPI_Type_f2c( *datatype ), *dest, *tag, PMPI_Comm_f2c( *comm ) );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_BSEND_INIT ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Bsend_init )
/**
 * Measurement wrapper for MPI_Bsend_init
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup p2p
 * For the order of events see @ref MPI_Bsend_init
 */
void
FSUB( MPI_Bsend_init )( void* buf, MPI_Fint* count, MPI_Fint* datatype, MPI_Fint* dest, MPI_Fint* tag, MPI_Fint* comm, MPI_Fint* request, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Request c_request;


    *ierr = MPI_Bsend_init( buf, *count, PMPI_Type_f2c( *datatype ), *dest, *tag, PMPI_Comm_f2c( *comm ), &c_request );

    *request = PMPI_Request_c2f( c_request );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_BUFFER_ATTACH ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Buffer_attach )
/**
 * Measurement wrapper for MPI_Buffer_attach
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup p2p
 * For the order of events see @ref MPI_Buffer_attach
 */
void
FSUB( MPI_Buffer_attach )( void* buffer, MPI_Fint* size, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Buffer_attach( buffer, *size );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_BUFFER_DETACH ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Buffer_detach )
/**
 * Measurement wrapper for MPI_Buffer_detach
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup p2p
 * For the order of events see @ref MPI_Buffer_detach
 */
void
FSUB( MPI_Buffer_detach )( void* buffer, MPI_Fint* size, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Buffer_detach( buffer, size );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_IBSEND ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Ibsend )
/**
 * Measurement wrapper for MPI_Ibsend
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup p2p
 * For the order of events see @ref MPI_Ibsend
 */
void
FSUB( MPI_Ibsend )( void* buf, MPI_Fint* count, MPI_Fint* datatype, MPI_Fint* dest, MPI_Fint* tag, MPI_Fint* comm, MPI_Fint* request, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Request c_request;


    #if HAVE( MPI_BOTTOM )
    if ( buf == scorep_mpi_fortran_bottom )
    {
        buf = MPI_BOTTOM;
    }
    #endif


    *ierr = MPI_Ibsend( buf, *count, PMPI_Type_f2c( *datatype ), *dest, *tag, PMPI_Comm_f2c( *comm ), &c_request );

    *request = PMPI_Request_c2f( c_request );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_IMPROBE ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Improbe )
/**
 * Measurement wrapper for MPI_Improbe
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-3
 * @ingroup p2p
 * For the order of events see @ref MPI_Improbe
 */
void
FSUB( MPI_Improbe )( MPI_Fint* source, MPI_Fint* tag, MPI_Fint* comm, MPI_Fint* flag, MPI_Fint* message, MPI_Fint* status, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Message c_message;
    MPI_Status  c_status;
    MPI_Status* c_status_ptr = &c_status;


    #if HAVE( MPI_STATUS_IGNORE )
    if ( status == scorep_mpi_fortran_status_ignore )
    {
        /* hardcoded c_status_ptr needs to be reset */
        c_status_ptr = MPI_STATUS_IGNORE;
    }
    #endif


    *ierr = MPI_Improbe( *source, *tag, PMPI_Comm_f2c( *comm ), flag, &c_message, c_status_ptr );

    *message = PMPI_Message_c2f( c_message );
    #if HAVE( MPI_STATUS_IGNORE )
    if ( c_status_ptr != MPI_STATUS_IGNORE )
#endif
    {
        PMPI_Status_c2f( c_status_ptr, status );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_IMRECV ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Imrecv )
/**
 * Measurement wrapper for MPI_Imrecv
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-3
 * @ingroup p2p
 * For the order of events see @ref MPI_Imrecv
 */
void
FSUB( MPI_Imrecv )( void* buf, MPI_Fint* count, MPI_Fint* datatype, MPI_Fint* message, MPI_Fint* request, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Message c_message = PMPI_Message_f2c( *message );
    MPI_Request c_request;


    #if HAVE( MPI_BOTTOM )
    if ( buf == scorep_mpi_fortran_bottom )
    {
        buf = MPI_BOTTOM;
    }
    #endif


    *ierr = MPI_Imrecv( buf, *count, PMPI_Type_f2c( *datatype ), &c_message, &c_request );

    *message = PMPI_Message_c2f( c_message );
    *request = PMPI_Request_c2f( c_request );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_IPROBE ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Iprobe )
/**
 * Measurement wrapper for MPI_Iprobe
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup p2p
 * For the order of events see @ref MPI_Iprobe
 */
void
FSUB( MPI_Iprobe )( MPI_Fint* source, MPI_Fint* tag, MPI_Fint* comm, MPI_Fint* flag, MPI_Fint* status, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Status  c_status;
    MPI_Status* c_status_ptr = &c_status;


    #if HAVE( MPI_STATUS_IGNORE )
    if ( status == scorep_mpi_fortran_status_ignore )
    {
        /* hardcoded c_status_ptr needs to be reset */
        c_status_ptr = MPI_STATUS_IGNORE;
    }
    #endif


    *ierr = MPI_Iprobe( *source, *tag, PMPI_Comm_f2c( *comm ), flag, c_status_ptr );

    #if HAVE( MPI_STATUS_IGNORE )
    if ( c_status_ptr != MPI_STATUS_IGNORE )
#endif
    {
        PMPI_Status_c2f( c_status_ptr, status );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_IRECV ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Irecv )
/**
 * Measurement wrapper for MPI_Irecv
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup p2p
 * For the order of events see @ref MPI_Irecv
 */
void
FSUB( MPI_Irecv )( void* buf, MPI_Fint* count, MPI_Fint* datatype, MPI_Fint* source, MPI_Fint* tag, MPI_Fint* comm, MPI_Fint* request, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Request c_request;


    #if HAVE( MPI_BOTTOM )
    if ( buf == scorep_mpi_fortran_bottom )
    {
        buf = MPI_BOTTOM;
    }
    #endif


    *ierr = MPI_Irecv( buf, *count, PMPI_Type_f2c( *datatype ), *source, *tag, PMPI_Comm_f2c( *comm ), &c_request );

    *request = PMPI_Request_c2f( c_request );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_IRSEND ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Irsend )
/**
 * Measurement wrapper for MPI_Irsend
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup p2p
 * For the order of events see @ref MPI_Irsend
 */
void
FSUB( MPI_Irsend )( void* buf, MPI_Fint* count, MPI_Fint* datatype, MPI_Fint* dest, MPI_Fint* tag, MPI_Fint* comm, MPI_Fint* request, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Request c_request;


    #if HAVE( MPI_BOTTOM )
    if ( buf == scorep_mpi_fortran_bottom )
    {
        buf = MPI_BOTTOM;
    }
    #endif


    *ierr = MPI_Irsend( buf, *count, PMPI_Type_f2c( *datatype ), *dest, *tag, PMPI_Comm_f2c( *comm ), &c_request );

    *request = PMPI_Request_c2f( c_request );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_ISEND ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Isend )
/**
 * Measurement wrapper for MPI_Isend
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup p2p
 * For the order of events see @ref MPI_Isend
 */
void
FSUB( MPI_Isend )( void* buf, MPI_Fint* count, MPI_Fint* datatype, MPI_Fint* dest, MPI_Fint* tag, MPI_Fint* comm, MPI_Fint* request, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Request c_request;


    #if HAVE( MPI_BOTTOM )
    if ( buf == scorep_mpi_fortran_bottom )
    {
        buf = MPI_BOTTOM;
    }
    #endif


    *ierr = MPI_Isend( buf, *count, PMPI_Type_f2c( *datatype ), *dest, *tag, PMPI_Comm_f2c( *comm ), &c_request );

    *request = PMPI_Request_c2f( c_request );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_ISSEND ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Issend )
/**
 * Measurement wrapper for MPI_Issend
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup p2p
 * For the order of events see @ref MPI_Issend
 */
void
FSUB( MPI_Issend )( void* buf, MPI_Fint* count, MPI_Fint* datatype, MPI_Fint* dest, MPI_Fint* tag, MPI_Fint* comm, MPI_Fint* request, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Request c_request;


    #if HAVE( MPI_BOTTOM )
    if ( buf == scorep_mpi_fortran_bottom )
    {
        buf = MPI_BOTTOM;
    }
    #endif


    *ierr = MPI_Issend( buf, *count, PMPI_Type_f2c( *datatype ), *dest, *tag, PMPI_Comm_f2c( *comm ), &c_request );

    *request = PMPI_Request_c2f( c_request );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_MPROBE ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Mprobe )
/**
 * Measurement wrapper for MPI_Mprobe
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-3
 * @ingroup p2p
 * For the order of events see @ref MPI_Mprobe
 */
void
FSUB( MPI_Mprobe )( MPI_Fint* source, MPI_Fint* tag, MPI_Fint* comm, MPI_Fint* message, MPI_Fint* status, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Message c_message;
    MPI_Status  c_status;
    MPI_Status* c_status_ptr = &c_status;


    #if HAVE( MPI_STATUS_IGNORE )
    if ( status == scorep_mpi_fortran_status_ignore )
    {
        /* hardcoded c_status_ptr needs to be reset */
        c_status_ptr = MPI_STATUS_IGNORE;
    }
    #endif


    *ierr = MPI_Mprobe( *source, *tag, PMPI_Comm_f2c( *comm ), &c_message, c_status_ptr );

    *message = PMPI_Message_c2f( c_message );
    #if HAVE( MPI_STATUS_IGNORE )
    if ( c_status_ptr != MPI_STATUS_IGNORE )
#endif
    {
        PMPI_Status_c2f( c_status_ptr, status );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_MRECV ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Mrecv )
/**
 * Measurement wrapper for MPI_Mrecv
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-3
 * @ingroup p2p
 * For the order of events see @ref MPI_Mrecv
 */
void
FSUB( MPI_Mrecv )( void* buf, MPI_Fint* count, MPI_Fint* datatype, MPI_Fint* message, MPI_Fint* status, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Message c_message = PMPI_Message_f2c( *message );
    MPI_Status  c_status;
    MPI_Status* c_status_ptr = &c_status;


    #if HAVE( MPI_STATUS_IGNORE )
    if ( status == scorep_mpi_fortran_status_ignore )
    {
        /* hardcoded c_status_ptr needs to be reset */
        c_status_ptr = MPI_STATUS_IGNORE;
    }
    #endif
    #if HAVE( MPI_BOTTOM )
    if ( buf == scorep_mpi_fortran_bottom )
    {
        buf = MPI_BOTTOM;
    }
    #endif


    *ierr = MPI_Mrecv( buf, *count, PMPI_Type_f2c( *datatype ), &c_message, c_status_ptr );

    *message = PMPI_Message_c2f( c_message );
    #if HAVE( MPI_STATUS_IGNORE )
    if ( c_status_ptr != MPI_STATUS_IGNORE )
#endif
    {
        PMPI_Status_c2f( c_status_ptr, status );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_PROBE ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Probe )
/**
 * Measurement wrapper for MPI_Probe
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup p2p
 * For the order of events see @ref MPI_Probe
 */
void
FSUB( MPI_Probe )( MPI_Fint* source, MPI_Fint* tag, MPI_Fint* comm, MPI_Fint* status, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Status  c_status;
    MPI_Status* c_status_ptr = &c_status;


    #if HAVE( MPI_STATUS_IGNORE )
    if ( status == scorep_mpi_fortran_status_ignore )
    {
        /* hardcoded c_status_ptr needs to be reset */
        c_status_ptr = MPI_STATUS_IGNORE;
    }
    #endif


    *ierr = MPI_Probe( *source, *tag, PMPI_Comm_f2c( *comm ), c_status_ptr );

    #if HAVE( MPI_STATUS_IGNORE )
    if ( c_status_ptr != MPI_STATUS_IGNORE )
#endif
    {
        PMPI_Status_c2f( c_status_ptr, status );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_RECV ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Recv )
/**
 * Measurement wrapper for MPI_Recv
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup p2p
 * For the order of events see @ref MPI_Recv
 */
void
FSUB( MPI_Recv )( void* buf, MPI_Fint* count, MPI_Fint* datatype, MPI_Fint* source, MPI_Fint* tag, MPI_Fint* comm, MPI_Fint* status, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Status  c_status;
    MPI_Status* c_status_ptr = &c_status;


    #if HAVE( MPI_STATUS_IGNORE )
    if ( status == scorep_mpi_fortran_status_ignore )
    {
        /* hardcoded c_status_ptr needs to be reset */
        c_status_ptr = MPI_STATUS_IGNORE;
    }
    #endif
    #if HAVE( MPI_BOTTOM )
    if ( buf == scorep_mpi_fortran_bottom )
    {
        buf = MPI_BOTTOM;
    }
    #endif


    *ierr = MPI_Recv( buf, *count, PMPI_Type_f2c( *datatype ), *source, *tag, PMPI_Comm_f2c( *comm ), c_status_ptr );

    #if HAVE( MPI_STATUS_IGNORE )
    if ( c_status_ptr != MPI_STATUS_IGNORE )
#endif
    {
        PMPI_Status_c2f( c_status_ptr, status );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_RECV_INIT ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Recv_init )
/**
 * Measurement wrapper for MPI_Recv_init
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup p2p
 * For the order of events see @ref MPI_Recv_init
 */
void
FSUB( MPI_Recv_init )( void* buf, MPI_Fint* count, MPI_Fint* datatype, MPI_Fint* source, MPI_Fint* tag, MPI_Fint* comm, MPI_Fint* request, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Request c_request;


    #if HAVE( MPI_BOTTOM )
    if ( buf == scorep_mpi_fortran_bottom )
    {
        buf = MPI_BOTTOM;
    }
    #endif


    *ierr = MPI_Recv_init( buf, *count, PMPI_Type_f2c( *datatype ), *source, *tag, PMPI_Comm_f2c( *comm ), &c_request );

    *request = PMPI_Request_c2f( c_request );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_RSEND ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Rsend )
/**
 * Measurement wrapper for MPI_Rsend
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup p2p
 * For the order of events see @ref MPI_Rsend
 */
void
FSUB( MPI_Rsend )( void* buf, MPI_Fint* count, MPI_Fint* datatype, MPI_Fint* dest, MPI_Fint* tag, MPI_Fint* comm, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    #if HAVE( MPI_BOTTOM )
    if ( buf == scorep_mpi_fortran_bottom )
    {
        buf = MPI_BOTTOM;
    }
    #endif


    *ierr = MPI_Rsend( buf, *count, PMPI_Type_f2c( *datatype ), *dest, *tag, PMPI_Comm_f2c( *comm ) );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_RSEND_INIT ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Rsend_init )
/**
 * Measurement wrapper for MPI_Rsend_init
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup p2p
 * For the order of events see @ref MPI_Rsend_init
 */
void
FSUB( MPI_Rsend_init )( void* buf, MPI_Fint* count, MPI_Fint* datatype, MPI_Fint* dest, MPI_Fint* tag, MPI_Fint* comm, MPI_Fint* request, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Request c_request;


    #if HAVE( MPI_BOTTOM )
    if ( buf == scorep_mpi_fortran_bottom )
    {
        buf = MPI_BOTTOM;
    }
    #endif


    *ierr = MPI_Rsend_init( buf, *count, PMPI_Type_f2c( *datatype ), *dest, *tag, PMPI_Comm_f2c( *comm ), &c_request );

    *request = PMPI_Request_c2f( c_request );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_SEND ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Send )
/**
 * Measurement wrapper for MPI_Send
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup p2p
 * For the order of events see @ref MPI_Send
 */
void
FSUB( MPI_Send )( void* buf, MPI_Fint* count, MPI_Fint* datatype, MPI_Fint* dest, MPI_Fint* tag, MPI_Fint* comm, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    #if HAVE( MPI_BOTTOM )
    if ( buf == scorep_mpi_fortran_bottom )
    {
        buf = MPI_BOTTOM;
    }
    #endif


    *ierr = MPI_Send( buf, *count, PMPI_Type_f2c( *datatype ), *dest, *tag, PMPI_Comm_f2c( *comm ) );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_SEND_INIT ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Send_init )
/**
 * Measurement wrapper for MPI_Send_init
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup p2p
 * For the order of events see @ref MPI_Send_init
 */
void
FSUB( MPI_Send_init )( void* buf, MPI_Fint* count, MPI_Fint* datatype, MPI_Fint* dest, MPI_Fint* tag, MPI_Fint* comm, MPI_Fint* request, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Request c_request;


    #if HAVE( MPI_BOTTOM )
    if ( buf == scorep_mpi_fortran_bottom )
    {
        buf = MPI_BOTTOM;
    }
    #endif


    *ierr = MPI_Send_init( buf, *count, PMPI_Type_f2c( *datatype ), *dest, *tag, PMPI_Comm_f2c( *comm ), &c_request );

    *request = PMPI_Request_c2f( c_request );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_SENDRECV ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Sendrecv )
/**
 * Measurement wrapper for MPI_Sendrecv
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup p2p
 * For the order of events see @ref MPI_Sendrecv
 */
void
FSUB( MPI_Sendrecv )( void* sendbuf, MPI_Fint* sendcount, MPI_Fint* sendtype, MPI_Fint* dest, MPI_Fint* sendtag, void* recvbuf, MPI_Fint* recvcount, MPI_Fint* recvtype, MPI_Fint* source, MPI_Fint* recvtag, MPI_Fint* comm, MPI_Fint* status, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Status  c_status;
    MPI_Status* c_status_ptr = &c_status;


    #if HAVE( MPI_BOTTOM )
    if ( sendbuf == scorep_mpi_fortran_bottom )
    {
        sendbuf = MPI_BOTTOM;
    }
    #endif
    #if HAVE( MPI_BOTTOM )
    if ( recvbuf == scorep_mpi_fortran_bottom )
    {
        recvbuf = MPI_BOTTOM;
    }
    #endif
    #if HAVE( MPI_STATUS_IGNORE )
    if ( status == scorep_mpi_fortran_status_ignore )
    {
        /* hardcoded c_status_ptr needs to be reset */
        c_status_ptr = MPI_STATUS_IGNORE;
    }
    #endif


    *ierr = MPI_Sendrecv( sendbuf, *sendcount, PMPI_Type_f2c( *sendtype ), *dest, *sendtag, recvbuf, *recvcount, PMPI_Type_f2c( *recvtype ), *source, *recvtag, PMPI_Comm_f2c( *comm ), c_status_ptr );

    #if HAVE( MPI_STATUS_IGNORE )
    if ( c_status_ptr != MPI_STATUS_IGNORE )
#endif
    {
        PMPI_Status_c2f( c_status_ptr, status );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_SENDRECV_REPLACE ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Sendrecv_replace )
/**
 * Measurement wrapper for MPI_Sendrecv_replace
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup p2p
 * For the order of events see @ref MPI_Sendrecv_replace
 */
void
FSUB( MPI_Sendrecv_replace )( void* buf, MPI_Fint* count, MPI_Fint* datatype, MPI_Fint* dest, MPI_Fint* sendtag, MPI_Fint* source, MPI_Fint* recvtag, MPI_Fint* comm, MPI_Fint* status, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Status  c_status;
    MPI_Status* c_status_ptr = &c_status;


    #if HAVE( MPI_BOTTOM )
    if ( buf == scorep_mpi_fortran_bottom )
    {
        buf = MPI_BOTTOM;
    }
    #endif
    #if HAVE( MPI_STATUS_IGNORE )
    if ( status == scorep_mpi_fortran_status_ignore )
    {
        /* hardcoded c_status_ptr needs to be reset */
        c_status_ptr = MPI_STATUS_IGNORE;
    }
    #endif


    *ierr = MPI_Sendrecv_replace( buf, *count, PMPI_Type_f2c( *datatype ), *dest, *sendtag, *source, *recvtag, PMPI_Comm_f2c( *comm ), c_status_ptr );

    #if HAVE( MPI_STATUS_IGNORE )
    if ( c_status_ptr != MPI_STATUS_IGNORE )
#endif
    {
        PMPI_Status_c2f( c_status_ptr, status );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_SSEND ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Ssend )
/**
 * Measurement wrapper for MPI_Ssend
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup p2p
 * For the order of events see @ref MPI_Ssend
 */
void
FSUB( MPI_Ssend )( void* buf, MPI_Fint* count, MPI_Fint* datatype, MPI_Fint* dest, MPI_Fint* tag, MPI_Fint* comm, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    #if HAVE( MPI_BOTTOM )
    if ( buf == scorep_mpi_fortran_bottom )
    {
        buf = MPI_BOTTOM;
    }
    #endif


    *ierr = MPI_Ssend( buf, *count, PMPI_Type_f2c( *datatype ), *dest, *tag, PMPI_Comm_f2c( *comm ) );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_SSEND_INIT ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Ssend_init )
/**
 * Measurement wrapper for MPI_Ssend_init
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup p2p
 * For the order of events see @ref MPI_Ssend_init
 */
void
FSUB( MPI_Ssend_init )( void* buf, MPI_Fint* count, MPI_Fint* datatype, MPI_Fint* dest, MPI_Fint* tag, MPI_Fint* comm, MPI_Fint* request, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Request c_request;


    #if HAVE( MPI_BOTTOM )
    if ( buf == scorep_mpi_fortran_bottom )
    {
        buf = MPI_BOTTOM;
    }
    #endif


    *ierr = MPI_Ssend_init( buf, *count, PMPI_Type_f2c( *datatype ), *dest, *tag, PMPI_Comm_f2c( *comm ), &c_request );

    *request = PMPI_Request_c2f( c_request );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_START ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Start )
/**
 * Measurement wrapper for MPI_Start
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup p2p
 * For the order of events see @ref MPI_Start
 */
void
FSUB( MPI_Start )( MPI_Fint* request, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Request c_request = PMPI_Request_f2c( *request );


    *ierr = MPI_Start( &c_request );

    *request = PMPI_Request_c2f( c_request );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif

#if HAVE( DECL_PMPI_TEST_CANCELLED ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Test_cancelled )
/**
 * Measurement wrapper for MPI_Test_cancelled
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup p2p
 * For the order of events see @ref MPI_Test_cancelled
 */
void
FSUB( MPI_Test_cancelled )( MPI_Fint* status, MPI_Fint* flag, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Status  c_status;
    MPI_Status* c_status_ptr = &c_status;
    PMPI_Status_f2c( status, c_status_ptr );


    *ierr = MPI_Test_cancelled( c_status_ptr, flag );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif

#endif
