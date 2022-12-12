/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013, 2022,
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
#include "scorep_mpi_request_mgmt.h"
/* We do wrap deprecated functions here, but we don't want warnings */
#include "SCOREP_Fmpi.h"

/* uppercase defines */

/** @def MPI_Cancel_U
    Exchanges MPI_Cancel_U with MPI_CANCEL.
    It is used for the Fortran wrappers of MPI_Cancel.
 */
#define MPI_Cancel_U MPI_CANCEL

/** @def MPI_Parrived_U
    Exchanges MPI_Parrived_U with MPI_PARRIVED.
    It is used for the Fortran wrappers of MPI_Parrived.
 */
#define MPI_Parrived_U MPI_PARRIVED

/** @def MPI_Pready_U
    Exchanges MPI_Pready_U with MPI_PREADY.
    It is used for the Fortran wrappers of MPI_Pready.
 */
#define MPI_Pready_U MPI_PREADY

/** @def MPI_Pready_list_U
    Exchanges MPI_Pready_list_U with MPI_PREADY_LIST.
    It is used for the Fortran wrappers of MPI_Pready_list.
 */
#define MPI_Pready_list_U MPI_PREADY_LIST

/** @def MPI_Pready_range_U
    Exchanges MPI_Pready_range_U with MPI_PREADY_RANGE.
    It is used for the Fortran wrappers of MPI_Pready_range.
 */
#define MPI_Pready_range_U MPI_PREADY_RANGE

/** @def MPI_Request_free_U
    Exchanges MPI_Request_free_U with MPI_REQUEST_FREE.
    It is used for the Fortran wrappers of MPI_Request_free.
 */
#define MPI_Request_free_U MPI_REQUEST_FREE

/** @def MPI_Request_get_status_U
    Exchanges MPI_Request_get_status_U with MPI_REQUEST_GET_STATUS.
    It is used for the Fortran wrappers of MPI_Request_get_status.
 */
#define MPI_Request_get_status_U MPI_REQUEST_GET_STATUS

/** @def MPI_Start_U
    Exchanges MPI_Start_U with MPI_START.
    It is used for the Fortran wrappers of MPI_Start.
 */
#define MPI_Start_U MPI_START

/** @def MPI_Startall_U
    Exchanges MPI_Startall_U with MPI_STARTALL.
    It is used for the Fortran wrappers of MPI_Startall.
 */
#define MPI_Startall_U MPI_STARTALL

/** @def MPI_Test_U
    Exchanges MPI_Test_U with MPI_TEST.
    It is used for the Fortran wrappers of MPI_Test.
 */
#define MPI_Test_U MPI_TEST

/** @def MPI_Test_cancelled_U
    Exchanges MPI_Test_cancelled_U with MPI_TEST_CANCELLED.
    It is used for the Fortran wrappers of MPI_Test_cancelled.
 */
#define MPI_Test_cancelled_U MPI_TEST_CANCELLED

/** @def MPI_Testall_U
    Exchanges MPI_Testall_U with MPI_TESTALL.
    It is used for the Fortran wrappers of MPI_Testall.
 */
#define MPI_Testall_U MPI_TESTALL

/** @def MPI_Testany_U
    Exchanges MPI_Testany_U with MPI_TESTANY.
    It is used for the Fortran wrappers of MPI_Testany.
 */
#define MPI_Testany_U MPI_TESTANY

/** @def MPI_Testsome_U
    Exchanges MPI_Testsome_U with MPI_TESTSOME.
    It is used for the Fortran wrappers of MPI_Testsome.
 */
#define MPI_Testsome_U MPI_TESTSOME

/** @def MPI_Wait_U
    Exchanges MPI_Wait_U with MPI_WAIT.
    It is used for the Fortran wrappers of MPI_Wait.
 */
#define MPI_Wait_U MPI_WAIT

/** @def MPI_Waitall_U
    Exchanges MPI_Waitall_U with MPI_WAITALL.
    It is used for the Fortran wrappers of MPI_Waitall.
 */
#define MPI_Waitall_U MPI_WAITALL

/** @def MPI_Waitany_U
    Exchanges MPI_Waitany_U with MPI_WAITANY.
    It is used for the Fortran wrappers of MPI_Waitany.
 */
#define MPI_Waitany_U MPI_WAITANY

/** @def MPI_Waitsome_U
    Exchanges MPI_Waitsome_U with MPI_WAITSOME.
    It is used for the Fortran wrappers of MPI_Waitsome.
 */
#define MPI_Waitsome_U MPI_WAITSOME

/* lowercase defines */

/** @def MPI_Cancel_L
    Exchanges MPI_Cancel_L with mpi_cancel.
    It is used for the Fortran wrappers of MPI_Cancel.
 */
#define MPI_Cancel_L mpi_cancel

/** @def MPI_Parrived_L
    Exchanges MPI_Parrived_L with mpi_parrived.
    It is used for the Fortran wrappers of MPI_Parrived.
 */
#define MPI_Parrived_L mpi_parrived

/** @def MPI_Pready_L
    Exchanges MPI_Pready_L with mpi_pready.
    It is used for the Fortran wrappers of MPI_Pready.
 */
#define MPI_Pready_L mpi_pready

/** @def MPI_Pready_list_L
    Exchanges MPI_Pready_list_L with mpi_pready_list.
    It is used for the Fortran wrappers of MPI_Pready_list.
 */
#define MPI_Pready_list_L mpi_pready_list

/** @def MPI_Pready_range_L
    Exchanges MPI_Pready_range_L with mpi_pready_range.
    It is used for the Fortran wrappers of MPI_Pready_range.
 */
#define MPI_Pready_range_L mpi_pready_range

/** @def MPI_Request_free_L
    Exchanges MPI_Request_free_L with mpi_request_free.
    It is used for the Fortran wrappers of MPI_Request_free.
 */
#define MPI_Request_free_L mpi_request_free

/** @def MPI_Request_get_status_L
    Exchanges MPI_Request_get_status_L with mpi_request_get_status.
    It is used for the Fortran wrappers of MPI_Request_get_status.
 */
#define MPI_Request_get_status_L mpi_request_get_status

/** @def MPI_Start_L
    Exchanges MPI_Start_L with mpi_start.
    It is used for the Fortran wrappers of MPI_Start.
 */
#define MPI_Start_L mpi_start

/** @def MPI_Startall_L
    Exchanges MPI_Startall_L with mpi_startall.
    It is used for the Fortran wrappers of MPI_Startall.
 */
#define MPI_Startall_L mpi_startall

/** @def MPI_Test_L
    Exchanges MPI_Test_L with mpi_test.
    It is used for the Fortran wrappers of MPI_Test.
 */
#define MPI_Test_L mpi_test

/** @def MPI_Test_cancelled_L
    Exchanges MPI_Test_cancelled_L with mpi_test_cancelled.
    It is used for the Fortran wrappers of MPI_Test_cancelled.
 */
#define MPI_Test_cancelled_L mpi_test_cancelled

/** @def MPI_Testall_L
    Exchanges MPI_Testall_L with mpi_testall.
    It is used for the Fortran wrappers of MPI_Testall.
 */
#define MPI_Testall_L mpi_testall

/** @def MPI_Testany_L
    Exchanges MPI_Testany_L with mpi_testany.
    It is used for the Fortran wrappers of MPI_Testany.
 */
#define MPI_Testany_L mpi_testany

/** @def MPI_Testsome_L
    Exchanges MPI_Testsome_L with mpi_testsome.
    It is used for the Fortran wrappers of MPI_Testsome.
 */
#define MPI_Testsome_L mpi_testsome

/** @def MPI_Wait_L
    Exchanges MPI_Wait_L with mpi_wait.
    It is used for the Fortran wrappers of MPI_Wait.
 */
#define MPI_Wait_L mpi_wait

/** @def MPI_Waitall_L
    Exchanges MPI_Waitall_L with mpi_waitall.
    It is used for the Fortran wrappers of MPI_Waitall.
 */
#define MPI_Waitall_L mpi_waitall

/** @def MPI_Waitany_L
    Exchanges MPI_Waitany_L with mpi_waitany.
    It is used for the Fortran wrappers of MPI_Waitany.
 */
#define MPI_Waitany_L mpi_waitany

/** @def MPI_Waitsome_L
    Exchanges MPI_Waitsome_L with mpi_waitsome.
    It is used for the Fortran wrappers of MPI_Waitsome.
 */
#define MPI_Waitsome_L mpi_waitsome

#ifndef NEED_F2C_CONV

/* If MPI_Comm_f2c is HAVE as a simple macro (which typically does nothing)
 * as it would require a "real" function if it is really needed
 * => we can save the f2c and c2s conversions */

#if HAVE( MPI_1_0_SYMBOL_PMPI_WAITANY ) && !defined( SCOREP_MPI_NO_REQUEST )
/**
 * Manual measurement wrapper for the Fortran interface of MPI_Waitany
 * @note C indices have to be converted to Fortran indices, when the
 *       effective index is not MPI_UNDEFINED
 * @ingroup manual_fortran_mpi_1_request
 */
void
FSUB( MPI_Waitany )( int*         count,
                     MPI_Request* array,
                     int*         index,
                     MPI_Status*  status,
                     int*         ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    if ( status == scorep_mpi_fortran_status_ignore )
    {
        status = MPI_STATUS_IGNORE;
    }

    *ierr = MPI_Waitany( *count, array, index, status );

    if ( ( *index != MPI_UNDEFINED ) && ( *index >= 0 ) )
    {
        ( *index )++;
    }
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif

#if HAVE( MPI_1_0_SYMBOL_PMPI_WAITSOME ) && !defined( SCOREP_MPI_NO_REQUEST )
/**
 * Manual measurement wrapper for the Fortran interface of MPI_Waitsome
 * @note C indices have to be converted to Fortran indices, when the
 *       effective index is not MPI_UNDEFINED
 * @ingroup manual_fortran_mpi_1_request
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

    if ( array_of_statuses == scorep_mpi_fortran_statuses_ignore )
    {
        array_of_statuses = MPI_STATUSES_IGNORE;
    }

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

#if HAVE( MPI_1_0_SYMBOL_PMPI_TESTANY ) && !defined( SCOREP_MPI_NO_REQUEST )
/**
 * Manual measurement wrapper for the Fortran interface of MPI_Testany
 * @note C indices have to be converted to Fortran indices, when the
 *       effective index is not MPI_UNDEFINED
 * @ingroup manual_fortran_mpi_1_request
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
    if ( status == scorep_mpi_fortran_status_ignore )
    {
        status = MPI_STATUS_IGNORE;
    }

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

#if HAVE( MPI_1_0_SYMBOL_PMPI_TESTSOME ) && !defined( SCOREP_MPI_NO_REQUEST )
/**
 * Manual measurement wrapper for the Fortran interface of MPI_Testsome
 * @note C indices have to be converted to Fortran indices, when the
 *       effective index is not MPI_UNDEFINED
 * @ingroup manual_fortran_mpi_1_request
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

    if ( array_of_statuses == scorep_mpi_fortran_statuses_ignore )
    {
        array_of_statuses = MPI_STATUSES_IGNORE;
    }

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

#if HAVE( MPI_1_0_SYMBOL_PMPI_CANCEL ) && !defined( SCOREP_MPI_NO_REQUEST ) && !defined( MPI_Cancel )
/**
 * Measurement wrapper for MPI_Cancel
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1.0
 * @ingroup request
 */
void
FSUB( MPI_Cancel )( MPI_Request* request, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Cancel( request );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_1_0_SYMBOL_PMPI_REQUEST_FREE ) && !defined( SCOREP_MPI_NO_REQUEST ) && !defined( MPI_Request_free )
/**
 * Measurement wrapper for MPI_Request_free
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1.0
 * @ingroup request
 */
void
FSUB( MPI_Request_free )( MPI_Request* request, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Request_free( request );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_2_0_SYMBOL_PMPI_REQUEST_GET_STATUS ) && !defined( SCOREP_MPI_NO_REQUEST ) && !defined( MPI_Request_get_status )
/**
 * Measurement wrapper for MPI_Request_get_status
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2.0
 * @ingroup request
 */
void
FSUB( MPI_Request_get_status )( MPI_Request* request, int* flag, MPI_Status* status, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    if ( status == scorep_mpi_fortran_status_ignore )
    {
        status = MPI_STATUS_IGNORE;
    }


    *ierr = MPI_Request_get_status( *request, flag, status );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_1_0_SYMBOL_PMPI_START ) && !defined( SCOREP_MPI_NO_REQUEST ) && !defined( MPI_Start )
/**
 * Measurement wrapper for MPI_Start
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1.0
 * @ingroup request
 */
void
FSUB( MPI_Start )( MPI_Request* request, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Start( request );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_1_0_SYMBOL_PMPI_STARTALL ) && !defined( SCOREP_MPI_NO_REQUEST ) && !defined( MPI_Startall )
/**
 * Measurement wrapper for MPI_Startall
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1.0
 * @ingroup request
 */
void
FSUB( MPI_Startall )( int* count, MPI_Request* array_of_requests, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Startall( *count, array_of_requests );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_1_0_SYMBOL_PMPI_TEST ) && !defined( SCOREP_MPI_NO_REQUEST ) && !defined( MPI_Test )
/**
 * Measurement wrapper for MPI_Test
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1.0
 * @ingroup request
 */
void
FSUB( MPI_Test )( MPI_Request* request, int* flag, MPI_Status* status, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    if ( status == scorep_mpi_fortran_status_ignore )
    {
        status = MPI_STATUS_IGNORE;
    }


    *ierr = MPI_Test( request, flag, status );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_1_0_SYMBOL_PMPI_TEST_CANCELLED ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_REQUEST ) && !defined( MPI_Test_cancelled )
/**
 * Measurement wrapper for MPI_Test_cancelled
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1.0
 * @ingroup request
 */
void
FSUB( MPI_Test_cancelled )( MPI_Status* status, int* flag, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Test_cancelled( status, flag );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_1_0_SYMBOL_PMPI_TESTALL ) && !defined( SCOREP_MPI_NO_REQUEST ) && !defined( MPI_Testall )
/**
 * Measurement wrapper for MPI_Testall
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1.0
 * @ingroup request
 */
void
FSUB( MPI_Testall )( int* count, MPI_Request* array_of_requests, int* flag, MPI_Status* array_of_statuses, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    if ( array_of_statuses == scorep_mpi_fortran_statuses_ignore )
    {
        array_of_statuses = MPI_STATUSES_IGNORE;
    }


    *ierr = MPI_Testall( *count, array_of_requests, flag, array_of_statuses );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_1_0_SYMBOL_PMPI_WAIT ) && !defined( SCOREP_MPI_NO_REQUEST ) && !defined( MPI_Wait )
/**
 * Measurement wrapper for MPI_Wait
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1.0
 * @ingroup request
 */
void
FSUB( MPI_Wait )( MPI_Request* request, MPI_Status* status, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    if ( status == scorep_mpi_fortran_status_ignore )
    {
        status = MPI_STATUS_IGNORE;
    }


    *ierr = MPI_Wait( request, status );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_1_0_SYMBOL_PMPI_WAITALL ) && !defined( SCOREP_MPI_NO_REQUEST ) && !defined( MPI_Waitall )
/**
 * Measurement wrapper for MPI_Waitall
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1.0
 * @ingroup request
 */
void
FSUB( MPI_Waitall )( int* count, MPI_Request* array_of_requests, MPI_Status* array_of_statuses, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    if ( array_of_statuses == scorep_mpi_fortran_statuses_ignore )
    {
        array_of_statuses = MPI_STATUSES_IGNORE;
    }


    *ierr = MPI_Waitall( *count, array_of_requests, array_of_statuses );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif

#else /* !NEED_F2C_CONV */

#if HAVE( MPI_1_0_SYMBOL_PMPI_WAIT ) && !defined( SCOREP_MPI_NO_REQUEST )
/**
 * Manual measurement wrapper for the Fortran interface of MPI_Wait,
 * when handle conversion is needed.
 * @ingroup manual_f2c_c2f_mpi_1_request
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

    if ( status == scorep_mpi_fortran_status_ignore )
    {
        c_status_ptr = MPI_STATUS_IGNORE;
    }

    *ierr = MPI_Wait( &lrequest, c_status_ptr );

    *request = PMPI_Request_c2f( lrequest );

    if ( *ierr == MPI_SUCCESS )
    {
        if ( status != scorep_mpi_fortran_status_ignore )
        {
            PMPI_Status_c2f( c_status_ptr, status );
        }
    }
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif

#if HAVE( MPI_1_0_SYMBOL_PMPI_WAITALL ) && !defined( SCOREP_MPI_NO_REQUEST )
/**
 * Manual measurement wrapper for the Fortran interface of MPI_Waitall,
 * when handle conversion is needed.
 * @ingroup manual_f2c_c2f_mpi_1_request
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
        lrequest = scorep_mpi_get_request_f2c_array( *count );

        if ( array_of_statuses != scorep_mpi_fortran_statuses_ignore )
        {
            c_status = scorep_mpi_get_status_array( *count );
        }
        else
        {
            c_status = MPI_STATUSES_IGNORE;
        }

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
        if ( array_of_statuses != scorep_mpi_fortran_statuses_ignore )
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

#if HAVE( MPI_1_0_SYMBOL_PMPI_WAITANY ) && !defined( SCOREP_MPI_NO_REQUEST )
/**
 * Manual measurement wrapper for the Fortran interface of MPI_Waitany,
 * when handle conversion is needed.
 * @note C index has to be converted to Fortran index, only if it is not
 *       MPI_UNDEFINED
 * @ingroup manual_f2c_c2f_mpi_1_request
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
        lrequest = scorep_mpi_get_request_f2c_array( *count );
        for ( i = 0; i < *count; i++ )
        {
            lrequest[ i ] = PMPI_Request_f2c( array_of_requests[ i ] );
        }
    }

    if ( status == scorep_mpi_fortran_status_ignore )
    {
        c_status_ptr = MPI_STATUS_IGNORE;
    }

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
        if ( status != scorep_mpi_fortran_status_ignore )
        {
            PMPI_Status_c2f( &c_status, status );
        }
    }
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif

#if HAVE( MPI_1_0_SYMBOL_PMPI_WAITSOME ) && !defined( SCOREP_MPI_NO_REQUEST )
/**
 * Manual measurement wrapper for the Fortran interface of MPI_Waitsome,
 * when handle conversion is needed.
 * @note C indices have to be converted to Fortran indices, only if the
 *       outcount is not MPI_UNDEFINED
 * @ingroup manual_f2c_c2f_mpi_1_request
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
        lrequest = scorep_mpi_get_request_f2c_array( *incount );

        if ( array_of_statuses != scorep_mpi_fortran_statuses_ignore )
        {
            c_status = scorep_mpi_get_status_array( *incount );
        }
        else
        {
            c_status = MPI_STATUSES_IGNORE;
        }

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

        if ( array_of_statuses != scorep_mpi_fortran_statuses_ignore )
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

#if HAVE( MPI_1_0_SYMBOL_PMPI_TEST ) && !defined( SCOREP_MPI_NO_REQUEST )
/**
 * Manual measurement wrapper for the Fortran interface of MPI_Test,
 * when handle conversion is needed.
 * @ingroup manual_f2c_c2f_mpi_1_request
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

    if ( status == scorep_mpi_fortran_status_ignore )
    {
        c_status_ptr = MPI_STATUS_IGNORE;
    }

    *ierr = MPI_Test( &lrequest, flag, c_status_ptr );

    if ( *ierr != MPI_SUCCESS )
    {
        return;
    }
    *request = PMPI_Request_c2f( lrequest );
    if ( flag )
    {
        if ( status != scorep_mpi_fortran_status_ignore )
        {
            PMPI_Status_c2f( &c_status, status );
        }
    }
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif

#if HAVE( MPI_1_0_SYMBOL_PMPI_TESTANY ) && !defined( SCOREP_MPI_NO_REQUEST )
/**
 * Manual measurement wrapper for the Fortran interface of MPI_Testany,
 * when handle conversion is needed.
 * @note C index has to be converted to Fortran index, only if it is not
 *       MPI_UNDEFINED
 * @ingroup manual_f2c_c2f_mpi_1_request
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
        lrequest = scorep_mpi_get_request_f2c_array( *count );
        for ( i = 0; i < *count; i++ )
        {
            lrequest[ i ] = PMPI_Request_f2c( array_of_requests[ i ] );
        }
    }

    if ( status == scorep_mpi_fortran_status_ignore )
    {
        c_status_ptr = MPI_STATUS_IGNORE;
    }

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

        if ( status != scorep_mpi_fortran_status_ignore )
        {
            PMPI_Status_c2f( &c_status, status );
        }
    }
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif

#if HAVE( MPI_1_0_SYMBOL_PMPI_TESTALL ) && !defined( SCOREP_MPI_NO_REQUEST )
/**
 * Manual measurement wrapper for the Fortran interface of MPI_Testall,
 * when handle conversion is needed.
 * @note C indices have to be converted to Fortran indices
 * @ingroup manual_f2c_c2f_mpi_1_request
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
        lrequest = scorep_mpi_get_request_f2c_array( *count );

        if ( array_of_statuses != scorep_mpi_fortran_statuses_ignore )
        {
            c_status = scorep_mpi_get_status_array( *count );
        }
        else
        {
            c_status = MPI_STATUSES_IGNORE;
        }

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
        if ( array_of_statuses != scorep_mpi_fortran_statuses_ignore )
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

#if HAVE( MPI_1_0_SYMBOL_PMPI_TESTSOME ) && !defined( SCOREP_MPI_NO_REQUEST )
/**
 * Manual measurement wrapper for the Fortran interface of MPI_Testsome,
 * when handle conversion is needed.
 * @note C indices have to be converted to Fortran indices, only if the
 *       outcount is not MPI_UNDEFINED
 * @ingroup manual_f2c_c2f_mpi_1_request
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
        lrequest = scorep_mpi_get_request_f2c_array( *incount );

        if ( array_of_statuses != scorep_mpi_fortran_statuses_ignore )
        {
            c_status = scorep_mpi_get_status_array( *incount );
        }
        else
        {
            c_status = MPI_STATUSES_IGNORE;
        }

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

        if ( array_of_statuses != scorep_mpi_fortran_statuses_ignore )
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

#if HAVE( MPI_1_0_SYMBOL_PMPI_STARTALL ) && !defined( SCOREP_MPI_NO_REQUEST )
/**
 * Manual measurement wrapper for the Fortran interface of MPI_Startall,
 * when handle conversion is needed.
 * @ingroup manual_f2c_c2f_mpi_1_request
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
        lrequest = scorep_mpi_get_request_f2c_array( *count );
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

#if HAVE( MPI_1_0_SYMBOL_PMPI_REQUEST_FREE ) && !defined( SCOREP_MPI_NO_REQUEST )
/**
 * Manual measurement wrapper for the Fortran interface of MPI_Request_free,
 * when handle conversion is needed.
 * @ingroup manual_f2c_c2f_mpi_1_request
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

#if HAVE( MPI_1_0_SYMBOL_PMPI_CANCEL ) && !defined( SCOREP_MPI_NO_REQUEST )
/**
 * Manual measurement wrapper for the Fortran interface of MPI_Cancel,
 * when handle conversion is needed.
 * @note C indices have to be converted to Fortran indices, when the
 *       effective index is not MPI_UNDEFINED
 * @ingroup manual_f2c_c2f_mpi_1_request
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

#if HAVE( MPI_1_0_SYMBOL_PMPI_TEST_CANCELLED ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_REQUEST ) && !defined( MPI_Test_cancelled )
/**
 * Measurement wrapper for MPI_Test_cancelled
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1.0
 * @ingroup request
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

#if HAVE( MPI_4_0_SYMBOL_PMPI_PARRIVED ) && !defined( SCOREP_MPI_NO_REQUEST ) && !defined( MPI_Parrived )
/**
 * Measurement wrapper for MPI_Parrived
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-4.0
 * @ingroup request
 * For the order of events see @ref MPI_Parrived
 */
void
FSUB( MPI_Parrived )( MPI_Fint* request, MPI_Fint* partition, MPI_Fint* flag, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Parrived( PMPI_Request_f2c( *request ), *partition, flag );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_4_0_SYMBOL_PMPI_PREADY ) && !defined( SCOREP_MPI_NO_REQUEST ) && !defined( MPI_Pready )
/**
 * Measurement wrapper for MPI_Pready
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-4.0
 * @ingroup request
 * For the order of events see @ref MPI_Pready
 */
void
FSUB( MPI_Pready )( MPI_Fint* partition, MPI_Fint* request, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Pready( *partition, PMPI_Request_f2c( *request ) );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_4_0_SYMBOL_PMPI_PREADY_RANGE ) && !defined( SCOREP_MPI_NO_REQUEST ) && !defined( MPI_Pready_range )
/**
 * Measurement wrapper for MPI_Pready_range
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-4.0
 * @ingroup request
 * For the order of events see @ref MPI_Pready_range
 */
void
FSUB( MPI_Pready_range )( MPI_Fint* partition_low, MPI_Fint* partition_high, MPI_Fint* request, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Pready_range( *partition_low, *partition_high, PMPI_Request_f2c( *request ) );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_2_0_SYMBOL_PMPI_REQUEST_GET_STATUS ) && !defined( SCOREP_MPI_NO_REQUEST ) && !defined( MPI_Request_get_status )
/**
 * Measurement wrapper for MPI_Request_get_status
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2.0
 * @ingroup request
 * For the order of events see @ref MPI_Request_get_status
 */
void
FSUB( MPI_Request_get_status )( MPI_Fint* request, MPI_Fint* flag, MPI_Fint* status, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Status  c_status;
    MPI_Status* c_status_ptr = &c_status;


    if ( status == scorep_mpi_fortran_status_ignore )
    {
        /* hardcoded c_status_ptr needs to be reset */
        c_status_ptr = MPI_STATUS_IGNORE;
    }


    *ierr = MPI_Request_get_status( PMPI_Request_f2c( *request ), flag, c_status_ptr );

    if ( c_status_ptr != MPI_STATUS_IGNORE )
    {
        PMPI_Status_c2f( c_status_ptr, status );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_1_0_SYMBOL_PMPI_START ) && !defined( SCOREP_MPI_NO_REQUEST ) && !defined( MPI_Start )
/**
 * Measurement wrapper for MPI_Start
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1.0
 * @ingroup request
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

#if HAVE( MPI_PREADY_LIST_COMPLIANT )
#if HAVE( MPI_4_0_SYMBOL_PMPI_PREADY_LIST ) && !defined( SCOREP_MPI_NO_REQUEST ) && !defined( MPI_Pready_list )
/**
 * Measurement wrapper for MPI_Pready_list
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f_Compl.w
 * @note Fortran interface
 * @note Introduced with MPI-4.0
 * @ingroup request
 * For the order of events see @ref MPI_Pready_list
 */
void
FSUB( MPI_Pready_list )( MPI_Fint* length, MPI_Fint* array_of_partitions, MPI_Fint* request, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Pready_list( *length, array_of_partitions, PMPI_Request_f2c( *request ) );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#endif

#endif
