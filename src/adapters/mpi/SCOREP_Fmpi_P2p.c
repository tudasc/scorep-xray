/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2018,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2019,
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
    Exchanges MPI_Bsend_U with MPI_BSEND.
    It is used for the Fortran wrappers of MPI_Bsend.
 */
#define MPI_Bsend_U MPI_BSEND

/** @def MPI_Bsend_init_U
    Exchanges MPI_Bsend_init_U with MPI_BSEND_INIT.
    It is used for the Fortran wrappers of MPI_Bsend_init.
 */
#define MPI_Bsend_init_U MPI_BSEND_INIT

/** @def MPI_Buffer_attach_U
    Exchanges MPI_Buffer_attach_U with MPI_BUFFER_ATTACH.
    It is used for the Fortran wrappers of MPI_Buffer_attach.
 */
#define MPI_Buffer_attach_U MPI_BUFFER_ATTACH

/** @def MPI_Buffer_detach_U
    Exchanges MPI_Buffer_detach_U with MPI_BUFFER_DETACH.
    It is used for the Fortran wrappers of MPI_Buffer_detach.
 */
#define MPI_Buffer_detach_U MPI_BUFFER_DETACH

/** @def MPI_Ibsend_U
    Exchanges MPI_Ibsend_U with MPI_IBSEND.
    It is used for the Fortran wrappers of MPI_Ibsend.
 */
#define MPI_Ibsend_U MPI_IBSEND

/** @def MPI_Improbe_U
    Exchanges MPI_Improbe_U with MPI_IMPROBE.
    It is used for the Fortran wrappers of MPI_Improbe.
 */
#define MPI_Improbe_U MPI_IMPROBE

/** @def MPI_Imrecv_U
    Exchanges MPI_Imrecv_U with MPI_IMRECV.
    It is used for the Fortran wrappers of MPI_Imrecv.
 */
#define MPI_Imrecv_U MPI_IMRECV

/** @def MPI_Iprobe_U
    Exchanges MPI_Iprobe_U with MPI_IPROBE.
    It is used for the Fortran wrappers of MPI_Iprobe.
 */
#define MPI_Iprobe_U MPI_IPROBE

/** @def MPI_Irecv_U
    Exchanges MPI_Irecv_U with MPI_IRECV.
    It is used for the Fortran wrappers of MPI_Irecv.
 */
#define MPI_Irecv_U MPI_IRECV

/** @def MPI_Irsend_U
    Exchanges MPI_Irsend_U with MPI_IRSEND.
    It is used for the Fortran wrappers of MPI_Irsend.
 */
#define MPI_Irsend_U MPI_IRSEND

/** @def MPI_Isend_U
    Exchanges MPI_Isend_U with MPI_ISEND.
    It is used for the Fortran wrappers of MPI_Isend.
 */
#define MPI_Isend_U MPI_ISEND

/** @def MPI_Isendrecv_U
    Exchanges MPI_Isendrecv_U with MPI_ISENDRECV.
    It is used for the Fortran wrappers of MPI_Isendrecv.
 */
#define MPI_Isendrecv_U MPI_ISENDRECV

/** @def MPI_Isendrecv_replace_U
    Exchanges MPI_Isendrecv_replace_U with MPI_ISENDRECV_REPLACE.
    It is used for the Fortran wrappers of MPI_Isendrecv_replace.
 */
#define MPI_Isendrecv_replace_U MPI_ISENDRECV_REPLACE

/** @def MPI_Issend_U
    Exchanges MPI_Issend_U with MPI_ISSEND.
    It is used for the Fortran wrappers of MPI_Issend.
 */
#define MPI_Issend_U MPI_ISSEND

/** @def MPI_Mprobe_U
    Exchanges MPI_Mprobe_U with MPI_MPROBE.
    It is used for the Fortran wrappers of MPI_Mprobe.
 */
#define MPI_Mprobe_U MPI_MPROBE

/** @def MPI_Mrecv_U
    Exchanges MPI_Mrecv_U with MPI_MRECV.
    It is used for the Fortran wrappers of MPI_Mrecv.
 */
#define MPI_Mrecv_U MPI_MRECV

/** @def MPI_Precv_init_U
    Exchanges MPI_Precv_init_U with MPI_PRECV_INIT.
    It is used for the Fortran wrappers of MPI_Precv_init.
 */
#define MPI_Precv_init_U MPI_PRECV_INIT

/** @def MPI_Probe_U
    Exchanges MPI_Probe_U with MPI_PROBE.
    It is used for the Fortran wrappers of MPI_Probe.
 */
#define MPI_Probe_U MPI_PROBE

/** @def MPI_Psend_init_U
    Exchanges MPI_Psend_init_U with MPI_PSEND_INIT.
    It is used for the Fortran wrappers of MPI_Psend_init.
 */
#define MPI_Psend_init_U MPI_PSEND_INIT

/** @def MPI_Recv_U
    Exchanges MPI_Recv_U with MPI_RECV.
    It is used for the Fortran wrappers of MPI_Recv.
 */
#define MPI_Recv_U MPI_RECV

/** @def MPI_Recv_init_U
    Exchanges MPI_Recv_init_U with MPI_RECV_INIT.
    It is used for the Fortran wrappers of MPI_Recv_init.
 */
#define MPI_Recv_init_U MPI_RECV_INIT

/** @def MPI_Rsend_U
    Exchanges MPI_Rsend_U with MPI_RSEND.
    It is used for the Fortran wrappers of MPI_Rsend.
 */
#define MPI_Rsend_U MPI_RSEND

/** @def MPI_Rsend_init_U
    Exchanges MPI_Rsend_init_U with MPI_RSEND_INIT.
    It is used for the Fortran wrappers of MPI_Rsend_init.
 */
#define MPI_Rsend_init_U MPI_RSEND_INIT

/** @def MPI_Send_U
    Exchanges MPI_Send_U with MPI_SEND.
    It is used for the Fortran wrappers of MPI_Send.
 */
#define MPI_Send_U MPI_SEND

/** @def MPI_Send_init_U
    Exchanges MPI_Send_init_U with MPI_SEND_INIT.
    It is used for the Fortran wrappers of MPI_Send_init.
 */
#define MPI_Send_init_U MPI_SEND_INIT

/** @def MPI_Sendrecv_U
    Exchanges MPI_Sendrecv_U with MPI_SENDRECV.
    It is used for the Fortran wrappers of MPI_Sendrecv.
 */
#define MPI_Sendrecv_U MPI_SENDRECV

/** @def MPI_Sendrecv_replace_U
    Exchanges MPI_Sendrecv_replace_U with MPI_SENDRECV_REPLACE.
    It is used for the Fortran wrappers of MPI_Sendrecv_replace.
 */
#define MPI_Sendrecv_replace_U MPI_SENDRECV_REPLACE

/** @def MPI_Ssend_U
    Exchanges MPI_Ssend_U with MPI_SSEND.
    It is used for the Fortran wrappers of MPI_Ssend.
 */
#define MPI_Ssend_U MPI_SSEND

/** @def MPI_Ssend_init_U
    Exchanges MPI_Ssend_init_U with MPI_SSEND_INIT.
    It is used for the Fortran wrappers of MPI_Ssend_init.
 */
#define MPI_Ssend_init_U MPI_SSEND_INIT

/* lowercase defines */

/** @def MPI_Bsend_L
    Exchanges MPI_Bsend_L with mpi_bsend.
    It is used for the Fortran wrappers of MPI_Bsend.
 */
#define MPI_Bsend_L mpi_bsend

/** @def MPI_Bsend_init_L
    Exchanges MPI_Bsend_init_L with mpi_bsend_init.
    It is used for the Fortran wrappers of MPI_Bsend_init.
 */
#define MPI_Bsend_init_L mpi_bsend_init

/** @def MPI_Buffer_attach_L
    Exchanges MPI_Buffer_attach_L with mpi_buffer_attach.
    It is used for the Fortran wrappers of MPI_Buffer_attach.
 */
#define MPI_Buffer_attach_L mpi_buffer_attach

/** @def MPI_Buffer_detach_L
    Exchanges MPI_Buffer_detach_L with mpi_buffer_detach.
    It is used for the Fortran wrappers of MPI_Buffer_detach.
 */
#define MPI_Buffer_detach_L mpi_buffer_detach

/** @def MPI_Ibsend_L
    Exchanges MPI_Ibsend_L with mpi_ibsend.
    It is used for the Fortran wrappers of MPI_Ibsend.
 */
#define MPI_Ibsend_L mpi_ibsend

/** @def MPI_Improbe_L
    Exchanges MPI_Improbe_L with mpi_improbe.
    It is used for the Fortran wrappers of MPI_Improbe.
 */
#define MPI_Improbe_L mpi_improbe

/** @def MPI_Imrecv_L
    Exchanges MPI_Imrecv_L with mpi_imrecv.
    It is used for the Fortran wrappers of MPI_Imrecv.
 */
#define MPI_Imrecv_L mpi_imrecv

/** @def MPI_Iprobe_L
    Exchanges MPI_Iprobe_L with mpi_iprobe.
    It is used for the Fortran wrappers of MPI_Iprobe.
 */
#define MPI_Iprobe_L mpi_iprobe

/** @def MPI_Irecv_L
    Exchanges MPI_Irecv_L with mpi_irecv.
    It is used for the Fortran wrappers of MPI_Irecv.
 */
#define MPI_Irecv_L mpi_irecv

/** @def MPI_Irsend_L
    Exchanges MPI_Irsend_L with mpi_irsend.
    It is used for the Fortran wrappers of MPI_Irsend.
 */
#define MPI_Irsend_L mpi_irsend

/** @def MPI_Isend_L
    Exchanges MPI_Isend_L with mpi_isend.
    It is used for the Fortran wrappers of MPI_Isend.
 */
#define MPI_Isend_L mpi_isend

/** @def MPI_Isendrecv_L
    Exchanges MPI_Isendrecv_L with mpi_isendrecv.
    It is used for the Fortran wrappers of MPI_Isendrecv.
 */
#define MPI_Isendrecv_L mpi_isendrecv

/** @def MPI_Isendrecv_replace_L
    Exchanges MPI_Isendrecv_replace_L with mpi_isendrecv_replace.
    It is used for the Fortran wrappers of MPI_Isendrecv_replace.
 */
#define MPI_Isendrecv_replace_L mpi_isendrecv_replace

/** @def MPI_Issend_L
    Exchanges MPI_Issend_L with mpi_issend.
    It is used for the Fortran wrappers of MPI_Issend.
 */
#define MPI_Issend_L mpi_issend

/** @def MPI_Mprobe_L
    Exchanges MPI_Mprobe_L with mpi_mprobe.
    It is used for the Fortran wrappers of MPI_Mprobe.
 */
#define MPI_Mprobe_L mpi_mprobe

/** @def MPI_Mrecv_L
    Exchanges MPI_Mrecv_L with mpi_mrecv.
    It is used for the Fortran wrappers of MPI_Mrecv.
 */
#define MPI_Mrecv_L mpi_mrecv

/** @def MPI_Precv_init_L
    Exchanges MPI_Precv_init_L with mpi_precv_init.
    It is used for the Fortran wrappers of MPI_Precv_init.
 */
#define MPI_Precv_init_L mpi_precv_init

/** @def MPI_Probe_L
    Exchanges MPI_Probe_L with mpi_probe.
    It is used for the Fortran wrappers of MPI_Probe.
 */
#define MPI_Probe_L mpi_probe

/** @def MPI_Psend_init_L
    Exchanges MPI_Psend_init_L with mpi_psend_init.
    It is used for the Fortran wrappers of MPI_Psend_init.
 */
#define MPI_Psend_init_L mpi_psend_init

/** @def MPI_Recv_L
    Exchanges MPI_Recv_L with mpi_recv.
    It is used for the Fortran wrappers of MPI_Recv.
 */
#define MPI_Recv_L mpi_recv

/** @def MPI_Recv_init_L
    Exchanges MPI_Recv_init_L with mpi_recv_init.
    It is used for the Fortran wrappers of MPI_Recv_init.
 */
#define MPI_Recv_init_L mpi_recv_init

/** @def MPI_Rsend_L
    Exchanges MPI_Rsend_L with mpi_rsend.
    It is used for the Fortran wrappers of MPI_Rsend.
 */
#define MPI_Rsend_L mpi_rsend

/** @def MPI_Rsend_init_L
    Exchanges MPI_Rsend_init_L with mpi_rsend_init.
    It is used for the Fortran wrappers of MPI_Rsend_init.
 */
#define MPI_Rsend_init_L mpi_rsend_init

/** @def MPI_Send_L
    Exchanges MPI_Send_L with mpi_send.
    It is used for the Fortran wrappers of MPI_Send.
 */
#define MPI_Send_L mpi_send

/** @def MPI_Send_init_L
    Exchanges MPI_Send_init_L with mpi_send_init.
    It is used for the Fortran wrappers of MPI_Send_init.
 */
#define MPI_Send_init_L mpi_send_init

/** @def MPI_Sendrecv_L
    Exchanges MPI_Sendrecv_L with mpi_sendrecv.
    It is used for the Fortran wrappers of MPI_Sendrecv.
 */
#define MPI_Sendrecv_L mpi_sendrecv

/** @def MPI_Sendrecv_replace_L
    Exchanges MPI_Sendrecv_replace_L with mpi_sendrecv_replace.
    It is used for the Fortran wrappers of MPI_Sendrecv_replace.
 */
#define MPI_Sendrecv_replace_L mpi_sendrecv_replace

/** @def MPI_Ssend_L
    Exchanges MPI_Ssend_L with mpi_ssend.
    It is used for the Fortran wrappers of MPI_Ssend.
 */
#define MPI_Ssend_L mpi_ssend

/** @def MPI_Ssend_init_L
    Exchanges MPI_Ssend_init_L with mpi_ssend_init.
    It is used for the Fortran wrappers of MPI_Ssend_init.
 */
#define MPI_Ssend_init_L mpi_ssend_init

#ifndef NEED_F2C_CONV

/* If MPI_Comm_f2c is HAVE as a simple macro (which typically does nothing)
 * as it would require a "real" function if it is really needed
 * => we can save the f2c and c2s conversions */

#if HAVE( MPI_1_0_SYMBOL_PMPI_BSEND ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Bsend )
/**
 * Measurement wrapper for MPI_Bsend
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1.0
 * @ingroup p2p
 */
void
FSUB( MPI_Bsend )( void* buf, int* count, MPI_Datatype* datatype, int* dest, int* tag, MPI_Comm* comm, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    if ( buf == scorep_mpi_fortran_bottom )
    {
        buf = MPI_BOTTOM;
    }


    *ierr = MPI_Bsend( buf, *count, *datatype, *dest, *tag, *comm );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_1_0_SYMBOL_PMPI_BSEND_INIT ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Bsend_init )
/**
 * Measurement wrapper for MPI_Bsend_init
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1.0
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
#if HAVE( MPI_1_0_SYMBOL_PMPI_BUFFER_ATTACH ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Buffer_attach )
/**
 * Measurement wrapper for MPI_Buffer_attach
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1.0
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
#if HAVE( MPI_1_0_SYMBOL_PMPI_BUFFER_DETACH ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Buffer_detach )
/**
 * Measurement wrapper for MPI_Buffer_detach
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1.0
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
#if HAVE( MPI_1_0_SYMBOL_PMPI_IBSEND ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Ibsend )
/**
 * Measurement wrapper for MPI_Ibsend
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1.0
 * @ingroup p2p
 */
void
FSUB( MPI_Ibsend )( void* buf, int* count, MPI_Datatype* datatype, int* dest, int* tag, MPI_Comm* comm, MPI_Request* request, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    if ( buf == scorep_mpi_fortran_bottom )
    {
        buf = MPI_BOTTOM;
    }


    *ierr = MPI_Ibsend( buf, *count, *datatype, *dest, *tag, *comm, request );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_3_0_SYMBOL_PMPI_IMPROBE ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Improbe )
/**
 * Measurement wrapper for MPI_Improbe
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-3.0
 * @ingroup p2p
 */
void
FSUB( MPI_Improbe )( int* source, int* tag, MPI_Comm* comm, int* flag, MPI_Message* message, MPI_Status* status, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    if ( status == scorep_mpi_fortran_status_ignore )
    {
        status = MPI_STATUS_IGNORE;
    }


    *ierr = MPI_Improbe( *source, *tag, *comm, flag, message, status );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_3_0_SYMBOL_PMPI_IMRECV ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Imrecv )
/**
 * Measurement wrapper for MPI_Imrecv
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-3.0
 * @ingroup p2p
 */
void
FSUB( MPI_Imrecv )( void* buf, int* count, MPI_Datatype* datatype, MPI_Message* message, MPI_Request* request, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    if ( buf == scorep_mpi_fortran_bottom )
    {
        buf = MPI_BOTTOM;
    }


    *ierr = MPI_Imrecv( buf, *count, *datatype, message, request );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_1_0_SYMBOL_PMPI_IPROBE ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Iprobe )
/**
 * Measurement wrapper for MPI_Iprobe
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1.0
 * @ingroup p2p
 */
void
FSUB( MPI_Iprobe )( int* source, int* tag, MPI_Comm* comm, int* flag, MPI_Status* status, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    if ( status == scorep_mpi_fortran_status_ignore )
    {
        status = MPI_STATUS_IGNORE;
    }


    *ierr = MPI_Iprobe( *source, *tag, *comm, flag, status );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_1_0_SYMBOL_PMPI_IRECV ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Irecv )
/**
 * Measurement wrapper for MPI_Irecv
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1.0
 * @ingroup p2p
 */
void
FSUB( MPI_Irecv )( void* buf, int* count, MPI_Datatype* datatype, int* source, int* tag, MPI_Comm* comm, MPI_Request* request, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    if ( buf == scorep_mpi_fortran_bottom )
    {
        buf = MPI_BOTTOM;
    }


    *ierr = MPI_Irecv( buf, *count, *datatype, *source, *tag, *comm, request );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_1_0_SYMBOL_PMPI_IRSEND ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Irsend )
/**
 * Measurement wrapper for MPI_Irsend
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1.0
 * @ingroup p2p
 */
void
FSUB( MPI_Irsend )( void* buf, int* count, MPI_Datatype* datatype, int* dest, int* tag, MPI_Comm* comm, MPI_Request* request, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    if ( buf == scorep_mpi_fortran_bottom )
    {
        buf = MPI_BOTTOM;
    }


    *ierr = MPI_Irsend( buf, *count, *datatype, *dest, *tag, *comm, request );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_1_0_SYMBOL_PMPI_ISEND ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Isend )
/**
 * Measurement wrapper for MPI_Isend
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1.0
 * @ingroup p2p
 */
void
FSUB( MPI_Isend )( void* buf, int* count, MPI_Datatype* datatype, int* dest, int* tag, MPI_Comm* comm, MPI_Request* request, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    if ( buf == scorep_mpi_fortran_bottom )
    {
        buf = MPI_BOTTOM;
    }


    *ierr = MPI_Isend( buf, *count, *datatype, *dest, *tag, *comm, request );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_4_0_SYMBOL_PMPI_ISENDRECV ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Isendrecv )
/**
 * Measurement wrapper for MPI_Isendrecv
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-4.0
 * @ingroup p2p
 */
void
FSUB( MPI_Isendrecv )( void* sendbuf, int* sendcount, MPI_Datatype* sendtype, int* dest, int* sendtag, void* recvbuf, int* recvcount, MPI_Datatype* recvtype, int* source, int* recvtag, MPI_Comm* comm, MPI_Request* request, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Isendrecv( sendbuf, *sendcount, *sendtype, *dest, *sendtag, recvbuf, *recvcount, *recvtype, *source, *recvtag, *comm, request );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_4_0_SYMBOL_PMPI_ISENDRECV_REPLACE ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Isendrecv_replace )
/**
 * Measurement wrapper for MPI_Isendrecv_replace
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-4.0
 * @ingroup p2p
 */
void
FSUB( MPI_Isendrecv_replace )( void* buf, int* count, MPI_Datatype* datatype, int* dest, int* sendtag, int* source, int* recvtag, MPI_Comm* comm, MPI_Request* request, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Isendrecv_replace( buf, *count, *datatype, *dest, *sendtag, *source, *recvtag, *comm, request );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_1_0_SYMBOL_PMPI_ISSEND ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Issend )
/**
 * Measurement wrapper for MPI_Issend
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1.0
 * @ingroup p2p
 */
void
FSUB( MPI_Issend )( void* buf, int* count, MPI_Datatype* datatype, int* dest, int* tag, MPI_Comm* comm, MPI_Request* request, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    if ( buf == scorep_mpi_fortran_bottom )
    {
        buf = MPI_BOTTOM;
    }


    *ierr = MPI_Issend( buf, *count, *datatype, *dest, *tag, *comm, request );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_3_0_SYMBOL_PMPI_MPROBE ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Mprobe )
/**
 * Measurement wrapper for MPI_Mprobe
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-3.0
 * @ingroup p2p
 */
void
FSUB( MPI_Mprobe )( int* source, int* tag, MPI_Comm* comm, MPI_Message* message, MPI_Status* status, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    if ( status == scorep_mpi_fortran_status_ignore )
    {
        status = MPI_STATUS_IGNORE;
    }


    *ierr = MPI_Mprobe( *source, *tag, *comm, message, status );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_3_0_SYMBOL_PMPI_MRECV ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Mrecv )
/**
 * Measurement wrapper for MPI_Mrecv
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-3.0
 * @ingroup p2p
 */
void
FSUB( MPI_Mrecv )( void* buf, int* count, MPI_Datatype* datatype, MPI_Message* message, MPI_Status* status, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    if ( status == scorep_mpi_fortran_status_ignore )
    {
        status = MPI_STATUS_IGNORE;
    }
    if ( buf == scorep_mpi_fortran_bottom )
    {
        buf = MPI_BOTTOM;
    }


    *ierr = MPI_Mrecv( buf, *count, *datatype, message, status );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_4_0_SYMBOL_PMPI_PRECV_INIT ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Precv_init )
/**
 * Measurement wrapper for MPI_Precv_init
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-4.0
 * @ingroup p2p
 */
void
FSUB( MPI_Precv_init )( void* buf, int* partitions, MPI_Count* count, MPI_Datatype* datatype, int* source, int* tag, MPI_Comm* comm, MPI_Info* info, MPI_Request* request, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Precv_init( buf, *partitions, *count, *datatype, *source, *tag, *comm, *info, request );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_1_0_SYMBOL_PMPI_PROBE ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Probe )
/**
 * Measurement wrapper for MPI_Probe
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1.0
 * @ingroup p2p
 */
void
FSUB( MPI_Probe )( int* source, int* tag, MPI_Comm* comm, MPI_Status* status, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    if ( status == scorep_mpi_fortran_status_ignore )
    {
        status = MPI_STATUS_IGNORE;
    }


    *ierr = MPI_Probe( *source, *tag, *comm, status );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_1_0_SYMBOL_PMPI_RECV ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Recv )
/**
 * Measurement wrapper for MPI_Recv
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1.0
 * @ingroup p2p
 */
void
FSUB( MPI_Recv )( void* buf, int* count, MPI_Datatype* datatype, int* source, int* tag, MPI_Comm* comm, MPI_Status* status, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    if ( status == scorep_mpi_fortran_status_ignore )
    {
        status = MPI_STATUS_IGNORE;
    }
    if ( buf == scorep_mpi_fortran_bottom )
    {
        buf = MPI_BOTTOM;
    }


    *ierr = MPI_Recv( buf, *count, *datatype, *source, *tag, *comm, status );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_1_0_SYMBOL_PMPI_RECV_INIT ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Recv_init )
/**
 * Measurement wrapper for MPI_Recv_init
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1.0
 * @ingroup p2p
 */
void
FSUB( MPI_Recv_init )( void* buf, int* count, MPI_Datatype* datatype, int* source, int* tag, MPI_Comm* comm, MPI_Request* request, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    if ( buf == scorep_mpi_fortran_bottom )
    {
        buf = MPI_BOTTOM;
    }


    *ierr = MPI_Recv_init( buf, *count, *datatype, *source, *tag, *comm, request );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_1_0_SYMBOL_PMPI_RSEND ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Rsend )
/**
 * Measurement wrapper for MPI_Rsend
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1.0
 * @ingroup p2p
 */
void
FSUB( MPI_Rsend )( void* buf, int* count, MPI_Datatype* datatype, int* dest, int* tag, MPI_Comm* comm, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    if ( buf == scorep_mpi_fortran_bottom )
    {
        buf = MPI_BOTTOM;
    }


    *ierr = MPI_Rsend( buf, *count, *datatype, *dest, *tag, *comm );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_1_0_SYMBOL_PMPI_RSEND_INIT ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Rsend_init )
/**
 * Measurement wrapper for MPI_Rsend_init
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1.0
 * @ingroup p2p
 */
void
FSUB( MPI_Rsend_init )( void* buf, int* count, MPI_Datatype* datatype, int* dest, int* tag, MPI_Comm* comm, MPI_Request* request, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    if ( buf == scorep_mpi_fortran_bottom )
    {
        buf = MPI_BOTTOM;
    }


    *ierr = MPI_Rsend_init( buf, *count, *datatype, *dest, *tag, *comm, request );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_1_0_SYMBOL_PMPI_SEND ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Send )
/**
 * Measurement wrapper for MPI_Send
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1.0
 * @ingroup p2p
 */
void
FSUB( MPI_Send )( void* buf, int* count, MPI_Datatype* datatype, int* dest, int* tag, MPI_Comm* comm, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    if ( buf == scorep_mpi_fortran_bottom )
    {
        buf = MPI_BOTTOM;
    }


    *ierr = MPI_Send( buf, *count, *datatype, *dest, *tag, *comm );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_1_0_SYMBOL_PMPI_SEND_INIT ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Send_init )
/**
 * Measurement wrapper for MPI_Send_init
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1.0
 * @ingroup p2p
 */
void
FSUB( MPI_Send_init )( void* buf, int* count, MPI_Datatype* datatype, int* dest, int* tag, MPI_Comm* comm, MPI_Request* request, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    if ( buf == scorep_mpi_fortran_bottom )
    {
        buf = MPI_BOTTOM;
    }


    *ierr = MPI_Send_init( buf, *count, *datatype, *dest, *tag, *comm, request );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_1_0_SYMBOL_PMPI_SENDRECV ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Sendrecv )
/**
 * Measurement wrapper for MPI_Sendrecv
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1.0
 * @ingroup p2p
 */
void
FSUB( MPI_Sendrecv )( void* sendbuf, int* sendcount, MPI_Datatype* sendtype, int* dest, int* sendtag, void* recvbuf, int* recvcount, MPI_Datatype* recvtype, int* source, int* recvtag, MPI_Comm* comm, MPI_Status* status, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    if ( sendbuf == scorep_mpi_fortran_bottom )
    {
        sendbuf = MPI_BOTTOM;
    }
    if ( recvbuf == scorep_mpi_fortran_bottom )
    {
        recvbuf = MPI_BOTTOM;
    }
    if ( status == scorep_mpi_fortran_status_ignore )
    {
        status = MPI_STATUS_IGNORE;
    }


    *ierr = MPI_Sendrecv( sendbuf, *sendcount, *sendtype, *dest, *sendtag, recvbuf, *recvcount, *recvtype, *source, *recvtag, *comm, status );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_1_0_SYMBOL_PMPI_SENDRECV_REPLACE ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Sendrecv_replace )
/**
 * Measurement wrapper for MPI_Sendrecv_replace
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1.0
 * @ingroup p2p
 */
void
FSUB( MPI_Sendrecv_replace )( void* buf, int* count, MPI_Datatype* datatype, int* dest, int* sendtag, int* source, int* recvtag, MPI_Comm* comm, MPI_Status* status, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    if ( buf == scorep_mpi_fortran_bottom )
    {
        buf = MPI_BOTTOM;
    }
    if ( status == scorep_mpi_fortran_status_ignore )
    {
        status = MPI_STATUS_IGNORE;
    }


    *ierr = MPI_Sendrecv_replace( buf, *count, *datatype, *dest, *sendtag, *source, *recvtag, *comm, status );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_1_0_SYMBOL_PMPI_SSEND ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Ssend )
/**
 * Measurement wrapper for MPI_Ssend
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1.0
 * @ingroup p2p
 */
void
FSUB( MPI_Ssend )( void* buf, int* count, MPI_Datatype* datatype, int* dest, int* tag, MPI_Comm* comm, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    if ( buf == scorep_mpi_fortran_bottom )
    {
        buf = MPI_BOTTOM;
    }


    *ierr = MPI_Ssend( buf, *count, *datatype, *dest, *tag, *comm );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_1_0_SYMBOL_PMPI_SSEND_INIT ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Ssend_init )
/**
 * Measurement wrapper for MPI_Ssend_init
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1.0
 * @ingroup p2p
 */
void
FSUB( MPI_Ssend_init )( void* buf, int* count, MPI_Datatype* datatype, int* dest, int* tag, MPI_Comm* comm, MPI_Request* request, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    if ( buf == scorep_mpi_fortran_bottom )
    {
        buf = MPI_BOTTOM;
    }


    *ierr = MPI_Ssend_init( buf, *count, *datatype, *dest, *tag, *comm, request );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif

#if HAVE( MPI_PSEND_INIT_COMPLIANT )
#if HAVE( MPI_4_0_SYMBOL_PMPI_PSEND_INIT ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Psend_init )
/**
 * Measurement wrapper for MPI_Psend_init
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_StdCompl.w
 * @note Fortran interface
 * @note Introduced with MPI-4.0
 * @ingroup p2p
 */
void
FSUB( MPI_Psend_init )( void* buf, int* partitions, MPI_Count* count, MPI_Datatype* datatype, int* dest, int* tag, MPI_Comm* comm, MPI_Info* info, MPI_Request* request, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Psend_init( buf, *partitions, *count, *datatype, *dest, *tag, *comm, *info, request );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#endif

#else /* !NEED_F2C_CONV */

#if HAVE( MPI_1_0_SYMBOL_PMPI_BSEND ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Bsend )
/**
 * Measurement wrapper for MPI_Bsend
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1.0
 * @ingroup p2p
 * For the order of events see @ref MPI_Bsend
 */
void
FSUB( MPI_Bsend )( void* buf, MPI_Fint* count, MPI_Fint* datatype, MPI_Fint* dest, MPI_Fint* tag, MPI_Fint* comm, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    if ( buf == scorep_mpi_fortran_bottom )
    {
        buf = MPI_BOTTOM;
    }


    *ierr = MPI_Bsend( buf, *count, PMPI_Type_f2c( *datatype ), *dest, *tag, PMPI_Comm_f2c( *comm ) );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_1_0_SYMBOL_PMPI_BSEND_INIT ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Bsend_init )
/**
 * Measurement wrapper for MPI_Bsend_init
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1.0
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
#if HAVE( MPI_1_0_SYMBOL_PMPI_BUFFER_ATTACH ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Buffer_attach )
/**
 * Measurement wrapper for MPI_Buffer_attach
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1.0
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
#if HAVE( MPI_1_0_SYMBOL_PMPI_BUFFER_DETACH ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Buffer_detach )
/**
 * Measurement wrapper for MPI_Buffer_detach
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1.0
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
#if HAVE( MPI_1_0_SYMBOL_PMPI_IBSEND ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Ibsend )
/**
 * Measurement wrapper for MPI_Ibsend
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1.0
 * @ingroup p2p
 * For the order of events see @ref MPI_Ibsend
 */
void
FSUB( MPI_Ibsend )( void* buf, MPI_Fint* count, MPI_Fint* datatype, MPI_Fint* dest, MPI_Fint* tag, MPI_Fint* comm, MPI_Fint* request, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Request c_request;


    if ( buf == scorep_mpi_fortran_bottom )
    {
        buf = MPI_BOTTOM;
    }


    *ierr = MPI_Ibsend( buf, *count, PMPI_Type_f2c( *datatype ), *dest, *tag, PMPI_Comm_f2c( *comm ), &c_request );

    *request = PMPI_Request_c2f( c_request );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_3_0_SYMBOL_PMPI_IMPROBE ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Improbe )
/**
 * Measurement wrapper for MPI_Improbe
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-3.0
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


    if ( status == scorep_mpi_fortran_status_ignore )
    {
        /* hardcoded c_status_ptr needs to be reset */
        c_status_ptr = MPI_STATUS_IGNORE;
    }


    *ierr = MPI_Improbe( *source, *tag, PMPI_Comm_f2c( *comm ), flag, &c_message, c_status_ptr );

    *message = PMPI_Message_c2f( c_message );
    if ( c_status_ptr != MPI_STATUS_IGNORE )
    {
        PMPI_Status_c2f( c_status_ptr, status );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_3_0_SYMBOL_PMPI_IMRECV ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Imrecv )
/**
 * Measurement wrapper for MPI_Imrecv
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-3.0
 * @ingroup p2p
 * For the order of events see @ref MPI_Imrecv
 */
void
FSUB( MPI_Imrecv )( void* buf, MPI_Fint* count, MPI_Fint* datatype, MPI_Fint* message, MPI_Fint* request, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Message c_message = PMPI_Message_f2c( *message );
    MPI_Request c_request;


    if ( buf == scorep_mpi_fortran_bottom )
    {
        buf = MPI_BOTTOM;
    }


    *ierr = MPI_Imrecv( buf, *count, PMPI_Type_f2c( *datatype ), &c_message, &c_request );

    *message = PMPI_Message_c2f( c_message );
    *request = PMPI_Request_c2f( c_request );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_1_0_SYMBOL_PMPI_IPROBE ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Iprobe )
/**
 * Measurement wrapper for MPI_Iprobe
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1.0
 * @ingroup p2p
 * For the order of events see @ref MPI_Iprobe
 */
void
FSUB( MPI_Iprobe )( MPI_Fint* source, MPI_Fint* tag, MPI_Fint* comm, MPI_Fint* flag, MPI_Fint* status, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Status  c_status;
    MPI_Status* c_status_ptr = &c_status;


    if ( status == scorep_mpi_fortran_status_ignore )
    {
        /* hardcoded c_status_ptr needs to be reset */
        c_status_ptr = MPI_STATUS_IGNORE;
    }


    *ierr = MPI_Iprobe( *source, *tag, PMPI_Comm_f2c( *comm ), flag, c_status_ptr );

    if ( c_status_ptr != MPI_STATUS_IGNORE )
    {
        PMPI_Status_c2f( c_status_ptr, status );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_1_0_SYMBOL_PMPI_IRECV ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Irecv )
/**
 * Measurement wrapper for MPI_Irecv
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1.0
 * @ingroup p2p
 * For the order of events see @ref MPI_Irecv
 */
void
FSUB( MPI_Irecv )( void* buf, MPI_Fint* count, MPI_Fint* datatype, MPI_Fint* source, MPI_Fint* tag, MPI_Fint* comm, MPI_Fint* request, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Request c_request;


    if ( buf == scorep_mpi_fortran_bottom )
    {
        buf = MPI_BOTTOM;
    }


    *ierr = MPI_Irecv( buf, *count, PMPI_Type_f2c( *datatype ), *source, *tag, PMPI_Comm_f2c( *comm ), &c_request );

    *request = PMPI_Request_c2f( c_request );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_1_0_SYMBOL_PMPI_IRSEND ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Irsend )
/**
 * Measurement wrapper for MPI_Irsend
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1.0
 * @ingroup p2p
 * For the order of events see @ref MPI_Irsend
 */
void
FSUB( MPI_Irsend )( void* buf, MPI_Fint* count, MPI_Fint* datatype, MPI_Fint* dest, MPI_Fint* tag, MPI_Fint* comm, MPI_Fint* request, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Request c_request;


    if ( buf == scorep_mpi_fortran_bottom )
    {
        buf = MPI_BOTTOM;
    }


    *ierr = MPI_Irsend( buf, *count, PMPI_Type_f2c( *datatype ), *dest, *tag, PMPI_Comm_f2c( *comm ), &c_request );

    *request = PMPI_Request_c2f( c_request );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_1_0_SYMBOL_PMPI_ISEND ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Isend )
/**
 * Measurement wrapper for MPI_Isend
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1.0
 * @ingroup p2p
 * For the order of events see @ref MPI_Isend
 */
void
FSUB( MPI_Isend )( void* buf, MPI_Fint* count, MPI_Fint* datatype, MPI_Fint* dest, MPI_Fint* tag, MPI_Fint* comm, MPI_Fint* request, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Request c_request;


    if ( buf == scorep_mpi_fortran_bottom )
    {
        buf = MPI_BOTTOM;
    }


    *ierr = MPI_Isend( buf, *count, PMPI_Type_f2c( *datatype ), *dest, *tag, PMPI_Comm_f2c( *comm ), &c_request );

    *request = PMPI_Request_c2f( c_request );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_4_0_SYMBOL_PMPI_ISENDRECV ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Isendrecv )
/**
 * Measurement wrapper for MPI_Isendrecv
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-4.0
 * @ingroup p2p
 * For the order of events see @ref MPI_Isendrecv
 */
void
FSUB( MPI_Isendrecv )( void* sendbuf, MPI_Fint* sendcount, MPI_Fint* sendtype, MPI_Fint* dest, MPI_Fint* sendtag, void* recvbuf, MPI_Fint* recvcount, MPI_Fint* recvtype, MPI_Fint* source, MPI_Fint* recvtag, MPI_Fint* comm, MPI_Fint* request, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Request c_request;


    *ierr = MPI_Isendrecv( sendbuf, *sendcount, PMPI_Type_f2c( *sendtype ), *dest, *sendtag, recvbuf, *recvcount, PMPI_Type_f2c( *recvtype ), *source, *recvtag, PMPI_Comm_f2c( *comm ), &c_request );

    *request = PMPI_Request_c2f( c_request );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_4_0_SYMBOL_PMPI_ISENDRECV_REPLACE ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Isendrecv_replace )
/**
 * Measurement wrapper for MPI_Isendrecv_replace
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-4.0
 * @ingroup p2p
 * For the order of events see @ref MPI_Isendrecv_replace
 */
void
FSUB( MPI_Isendrecv_replace )( void* buf, MPI_Fint* count, MPI_Fint* datatype, MPI_Fint* dest, MPI_Fint* sendtag, MPI_Fint* source, MPI_Fint* recvtag, MPI_Fint* comm, MPI_Fint* request, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Request c_request;


    *ierr = MPI_Isendrecv_replace( buf, *count, PMPI_Type_f2c( *datatype ), *dest, *sendtag, *source, *recvtag, PMPI_Comm_f2c( *comm ), &c_request );

    *request = PMPI_Request_c2f( c_request );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_1_0_SYMBOL_PMPI_ISSEND ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Issend )
/**
 * Measurement wrapper for MPI_Issend
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1.0
 * @ingroup p2p
 * For the order of events see @ref MPI_Issend
 */
void
FSUB( MPI_Issend )( void* buf, MPI_Fint* count, MPI_Fint* datatype, MPI_Fint* dest, MPI_Fint* tag, MPI_Fint* comm, MPI_Fint* request, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Request c_request;


    if ( buf == scorep_mpi_fortran_bottom )
    {
        buf = MPI_BOTTOM;
    }


    *ierr = MPI_Issend( buf, *count, PMPI_Type_f2c( *datatype ), *dest, *tag, PMPI_Comm_f2c( *comm ), &c_request );

    *request = PMPI_Request_c2f( c_request );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_3_0_SYMBOL_PMPI_MPROBE ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Mprobe )
/**
 * Measurement wrapper for MPI_Mprobe
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-3.0
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


    if ( status == scorep_mpi_fortran_status_ignore )
    {
        /* hardcoded c_status_ptr needs to be reset */
        c_status_ptr = MPI_STATUS_IGNORE;
    }


    *ierr = MPI_Mprobe( *source, *tag, PMPI_Comm_f2c( *comm ), &c_message, c_status_ptr );

    *message = PMPI_Message_c2f( c_message );
    if ( c_status_ptr != MPI_STATUS_IGNORE )
    {
        PMPI_Status_c2f( c_status_ptr, status );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_3_0_SYMBOL_PMPI_MRECV ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Mrecv )
/**
 * Measurement wrapper for MPI_Mrecv
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-3.0
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


    if ( status == scorep_mpi_fortran_status_ignore )
    {
        /* hardcoded c_status_ptr needs to be reset */
        c_status_ptr = MPI_STATUS_IGNORE;
    }
    if ( buf == scorep_mpi_fortran_bottom )
    {
        buf = MPI_BOTTOM;
    }


    *ierr = MPI_Mrecv( buf, *count, PMPI_Type_f2c( *datatype ), &c_message, c_status_ptr );

    *message = PMPI_Message_c2f( c_message );
    if ( c_status_ptr != MPI_STATUS_IGNORE )
    {
        PMPI_Status_c2f( c_status_ptr, status );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_4_0_SYMBOL_PMPI_PRECV_INIT ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Precv_init )
/**
 * Measurement wrapper for MPI_Precv_init
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-4.0
 * @ingroup p2p
 * For the order of events see @ref MPI_Precv_init
 */
void
FSUB( MPI_Precv_init )( void* buf, MPI_Fint* partitions, MPI_Count* count, MPI_Fint* datatype, MPI_Fint* source, MPI_Fint* tag, MPI_Fint* comm, MPI_Fint* info, MPI_Fint* request, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Request c_request;


    *ierr = MPI_Precv_init( buf, *partitions, *count, PMPI_Type_f2c( *datatype ), *source, *tag, PMPI_Comm_f2c( *comm ), PMPI_Info_f2c( *info ), &c_request );

    *request = PMPI_Request_c2f( c_request );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_1_0_SYMBOL_PMPI_PROBE ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Probe )
/**
 * Measurement wrapper for MPI_Probe
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1.0
 * @ingroup p2p
 * For the order of events see @ref MPI_Probe
 */
void
FSUB( MPI_Probe )( MPI_Fint* source, MPI_Fint* tag, MPI_Fint* comm, MPI_Fint* status, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Status  c_status;
    MPI_Status* c_status_ptr = &c_status;


    if ( status == scorep_mpi_fortran_status_ignore )
    {
        /* hardcoded c_status_ptr needs to be reset */
        c_status_ptr = MPI_STATUS_IGNORE;
    }


    *ierr = MPI_Probe( *source, *tag, PMPI_Comm_f2c( *comm ), c_status_ptr );

    if ( c_status_ptr != MPI_STATUS_IGNORE )
    {
        PMPI_Status_c2f( c_status_ptr, status );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_1_0_SYMBOL_PMPI_RECV ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Recv )
/**
 * Measurement wrapper for MPI_Recv
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1.0
 * @ingroup p2p
 * For the order of events see @ref MPI_Recv
 */
void
FSUB( MPI_Recv )( void* buf, MPI_Fint* count, MPI_Fint* datatype, MPI_Fint* source, MPI_Fint* tag, MPI_Fint* comm, MPI_Fint* status, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Status  c_status;
    MPI_Status* c_status_ptr = &c_status;


    if ( status == scorep_mpi_fortran_status_ignore )
    {
        /* hardcoded c_status_ptr needs to be reset */
        c_status_ptr = MPI_STATUS_IGNORE;
    }
    if ( buf == scorep_mpi_fortran_bottom )
    {
        buf = MPI_BOTTOM;
    }


    *ierr = MPI_Recv( buf, *count, PMPI_Type_f2c( *datatype ), *source, *tag, PMPI_Comm_f2c( *comm ), c_status_ptr );

    if ( c_status_ptr != MPI_STATUS_IGNORE )
    {
        PMPI_Status_c2f( c_status_ptr, status );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_1_0_SYMBOL_PMPI_RECV_INIT ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Recv_init )
/**
 * Measurement wrapper for MPI_Recv_init
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1.0
 * @ingroup p2p
 * For the order of events see @ref MPI_Recv_init
 */
void
FSUB( MPI_Recv_init )( void* buf, MPI_Fint* count, MPI_Fint* datatype, MPI_Fint* source, MPI_Fint* tag, MPI_Fint* comm, MPI_Fint* request, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Request c_request;


    if ( buf == scorep_mpi_fortran_bottom )
    {
        buf = MPI_BOTTOM;
    }


    *ierr = MPI_Recv_init( buf, *count, PMPI_Type_f2c( *datatype ), *source, *tag, PMPI_Comm_f2c( *comm ), &c_request );

    *request = PMPI_Request_c2f( c_request );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_1_0_SYMBOL_PMPI_RSEND ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Rsend )
/**
 * Measurement wrapper for MPI_Rsend
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1.0
 * @ingroup p2p
 * For the order of events see @ref MPI_Rsend
 */
void
FSUB( MPI_Rsend )( void* buf, MPI_Fint* count, MPI_Fint* datatype, MPI_Fint* dest, MPI_Fint* tag, MPI_Fint* comm, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    if ( buf == scorep_mpi_fortran_bottom )
    {
        buf = MPI_BOTTOM;
    }


    *ierr = MPI_Rsend( buf, *count, PMPI_Type_f2c( *datatype ), *dest, *tag, PMPI_Comm_f2c( *comm ) );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_1_0_SYMBOL_PMPI_RSEND_INIT ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Rsend_init )
/**
 * Measurement wrapper for MPI_Rsend_init
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1.0
 * @ingroup p2p
 * For the order of events see @ref MPI_Rsend_init
 */
void
FSUB( MPI_Rsend_init )( void* buf, MPI_Fint* count, MPI_Fint* datatype, MPI_Fint* dest, MPI_Fint* tag, MPI_Fint* comm, MPI_Fint* request, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Request c_request;


    if ( buf == scorep_mpi_fortran_bottom )
    {
        buf = MPI_BOTTOM;
    }


    *ierr = MPI_Rsend_init( buf, *count, PMPI_Type_f2c( *datatype ), *dest, *tag, PMPI_Comm_f2c( *comm ), &c_request );

    *request = PMPI_Request_c2f( c_request );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_1_0_SYMBOL_PMPI_SEND ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Send )
/**
 * Measurement wrapper for MPI_Send
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1.0
 * @ingroup p2p
 * For the order of events see @ref MPI_Send
 */
void
FSUB( MPI_Send )( void* buf, MPI_Fint* count, MPI_Fint* datatype, MPI_Fint* dest, MPI_Fint* tag, MPI_Fint* comm, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    if ( buf == scorep_mpi_fortran_bottom )
    {
        buf = MPI_BOTTOM;
    }


    *ierr = MPI_Send( buf, *count, PMPI_Type_f2c( *datatype ), *dest, *tag, PMPI_Comm_f2c( *comm ) );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_1_0_SYMBOL_PMPI_SEND_INIT ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Send_init )
/**
 * Measurement wrapper for MPI_Send_init
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1.0
 * @ingroup p2p
 * For the order of events see @ref MPI_Send_init
 */
void
FSUB( MPI_Send_init )( void* buf, MPI_Fint* count, MPI_Fint* datatype, MPI_Fint* dest, MPI_Fint* tag, MPI_Fint* comm, MPI_Fint* request, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Request c_request;


    if ( buf == scorep_mpi_fortran_bottom )
    {
        buf = MPI_BOTTOM;
    }


    *ierr = MPI_Send_init( buf, *count, PMPI_Type_f2c( *datatype ), *dest, *tag, PMPI_Comm_f2c( *comm ), &c_request );

    *request = PMPI_Request_c2f( c_request );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_1_0_SYMBOL_PMPI_SENDRECV ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Sendrecv )
/**
 * Measurement wrapper for MPI_Sendrecv
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1.0
 * @ingroup p2p
 * For the order of events see @ref MPI_Sendrecv
 */
void
FSUB( MPI_Sendrecv )( void* sendbuf, MPI_Fint* sendcount, MPI_Fint* sendtype, MPI_Fint* dest, MPI_Fint* sendtag, void* recvbuf, MPI_Fint* recvcount, MPI_Fint* recvtype, MPI_Fint* source, MPI_Fint* recvtag, MPI_Fint* comm, MPI_Fint* status, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Status  c_status;
    MPI_Status* c_status_ptr = &c_status;


    if ( sendbuf == scorep_mpi_fortran_bottom )
    {
        sendbuf = MPI_BOTTOM;
    }
    if ( recvbuf == scorep_mpi_fortran_bottom )
    {
        recvbuf = MPI_BOTTOM;
    }
    if ( status == scorep_mpi_fortran_status_ignore )
    {
        /* hardcoded c_status_ptr needs to be reset */
        c_status_ptr = MPI_STATUS_IGNORE;
    }


    *ierr = MPI_Sendrecv( sendbuf, *sendcount, PMPI_Type_f2c( *sendtype ), *dest, *sendtag, recvbuf, *recvcount, PMPI_Type_f2c( *recvtype ), *source, *recvtag, PMPI_Comm_f2c( *comm ), c_status_ptr );

    if ( c_status_ptr != MPI_STATUS_IGNORE )
    {
        PMPI_Status_c2f( c_status_ptr, status );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_1_0_SYMBOL_PMPI_SENDRECV_REPLACE ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Sendrecv_replace )
/**
 * Measurement wrapper for MPI_Sendrecv_replace
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1.0
 * @ingroup p2p
 * For the order of events see @ref MPI_Sendrecv_replace
 */
void
FSUB( MPI_Sendrecv_replace )( void* buf, MPI_Fint* count, MPI_Fint* datatype, MPI_Fint* dest, MPI_Fint* sendtag, MPI_Fint* source, MPI_Fint* recvtag, MPI_Fint* comm, MPI_Fint* status, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Status  c_status;
    MPI_Status* c_status_ptr = &c_status;


    if ( buf == scorep_mpi_fortran_bottom )
    {
        buf = MPI_BOTTOM;
    }
    if ( status == scorep_mpi_fortran_status_ignore )
    {
        /* hardcoded c_status_ptr needs to be reset */
        c_status_ptr = MPI_STATUS_IGNORE;
    }


    *ierr = MPI_Sendrecv_replace( buf, *count, PMPI_Type_f2c( *datatype ), *dest, *sendtag, *source, *recvtag, PMPI_Comm_f2c( *comm ), c_status_ptr );

    if ( c_status_ptr != MPI_STATUS_IGNORE )
    {
        PMPI_Status_c2f( c_status_ptr, status );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_1_0_SYMBOL_PMPI_SSEND ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Ssend )
/**
 * Measurement wrapper for MPI_Ssend
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1.0
 * @ingroup p2p
 * For the order of events see @ref MPI_Ssend
 */
void
FSUB( MPI_Ssend )( void* buf, MPI_Fint* count, MPI_Fint* datatype, MPI_Fint* dest, MPI_Fint* tag, MPI_Fint* comm, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    if ( buf == scorep_mpi_fortran_bottom )
    {
        buf = MPI_BOTTOM;
    }


    *ierr = MPI_Ssend( buf, *count, PMPI_Type_f2c( *datatype ), *dest, *tag, PMPI_Comm_f2c( *comm ) );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_1_0_SYMBOL_PMPI_SSEND_INIT ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Ssend_init )
/**
 * Measurement wrapper for MPI_Ssend_init
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1.0
 * @ingroup p2p
 * For the order of events see @ref MPI_Ssend_init
 */
void
FSUB( MPI_Ssend_init )( void* buf, MPI_Fint* count, MPI_Fint* datatype, MPI_Fint* dest, MPI_Fint* tag, MPI_Fint* comm, MPI_Fint* request, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Request c_request;


    if ( buf == scorep_mpi_fortran_bottom )
    {
        buf = MPI_BOTTOM;
    }


    *ierr = MPI_Ssend_init( buf, *count, PMPI_Type_f2c( *datatype ), *dest, *tag, PMPI_Comm_f2c( *comm ), &c_request );

    *request = PMPI_Request_c2f( c_request );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif

#if HAVE( MPI_PSEND_INIT_COMPLIANT )
#if HAVE( MPI_4_0_SYMBOL_PMPI_PSEND_INIT ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Psend_init )
/**
 * Measurement wrapper for MPI_Psend_init
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f_Compl.w
 * @note Fortran interface
 * @note Introduced with MPI-4.0
 * @ingroup p2p
 * For the order of events see @ref MPI_Psend_init
 */
void
FSUB( MPI_Psend_init )( void* buf, MPI_Fint* partitions, MPI_Count* count, MPI_Fint* datatype, MPI_Fint* dest, MPI_Fint* tag, MPI_Fint* comm, MPI_Fint* info, MPI_Fint* request, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Request c_request;


    *ierr = MPI_Psend_init( buf, *partitions, *count, PMPI_Type_f2c( *datatype ), *dest, *tag, PMPI_Comm_f2c( *comm ), PMPI_Info_f2c( *info ), &c_request );

    *request = PMPI_Request_c2f( c_request );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#endif

#endif
