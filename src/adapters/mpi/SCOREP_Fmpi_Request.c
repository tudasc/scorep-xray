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
/* We do wrap deprecated functions here, but we don't want warnings */
#include "SCOREP_Fmpi.h"

/* uppercase defines */
/** @def MPI_Cancel_U
    Exchange MPI_Cancel_U by MPI_CANCEL.
    It is used for the Fortran wrappers of MPI_Cancel.
 */
#define MPI_Cancel_U MPI_CANCEL

/** @def MPI_Request_free_U
    Exchange MPI_Request_free_U by MPI_REQUEST_FREE.
    It is used for the Fortran wrappers of MPI_Request_free.
 */
#define MPI_Request_free_U MPI_REQUEST_FREE

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
/** @def MPI_Cancel_L
    Exchanges MPI_Cancel_L by mpi_cancel.
    It is used for the Fortran wrappers of MPI_Cancel.
 */
#define MPI_Cancel_L mpi_cancel

/** @def MPI_Request_free_L
    Exchanges MPI_Request_free_L by mpi_request_free.
    It is used for the Fortran wrappers of MPI_Request_free.
 */
#define MPI_Request_free_L mpi_request_free

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

#if HAVE( DECL_PMPI_WAITANY ) && !defined( SCOREP_MPI_NO_REQUEST )
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

#if HAVE( DECL_PMPI_WAITSOME ) && !defined( SCOREP_MPI_NO_REQUEST )
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

#if HAVE( DECL_PMPI_TESTANY ) && !defined( SCOREP_MPI_NO_REQUEST )
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

#if HAVE( DECL_PMPI_TESTSOME ) && !defined( SCOREP_MPI_NO_REQUEST )
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

#if HAVE( DECL_PMPI_CANCEL ) && !defined( SCOREP_MPI_NO_REQUEST ) && !defined( MPI_Cancel )
/**
 * Measurement wrapper for MPI_Cancel
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
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
#if HAVE( DECL_PMPI_REQUEST_FREE ) && !defined( SCOREP_MPI_NO_REQUEST ) && !defined( MPI_Request_free )
/**
 * Measurement wrapper for MPI_Request_free
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
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
#if HAVE( DECL_PMPI_START ) && !defined( SCOREP_MPI_NO_REQUEST ) && !defined( MPI_Start )
/**
 * Measurement wrapper for MPI_Start
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
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
#if HAVE( DECL_PMPI_STARTALL ) && !defined( SCOREP_MPI_NO_REQUEST ) && !defined( MPI_Startall )
/**
 * Measurement wrapper for MPI_Startall
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
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
#if HAVE( DECL_PMPI_TEST ) && !defined( SCOREP_MPI_NO_REQUEST ) && !defined( MPI_Test )
/**
 * Measurement wrapper for MPI_Test
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup request
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
#if HAVE( DECL_PMPI_TEST_CANCELLED ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_REQUEST ) && !defined( MPI_Test_cancelled )
/**
 * Measurement wrapper for MPI_Test_cancelled
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
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
#if HAVE( DECL_PMPI_TESTALL ) && !defined( SCOREP_MPI_NO_REQUEST ) && !defined( MPI_Testall )
/**
 * Measurement wrapper for MPI_Testall
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup request
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
#if HAVE( DECL_PMPI_WAIT ) && !defined( SCOREP_MPI_NO_REQUEST ) && !defined( MPI_Wait )
/**
 * Measurement wrapper for MPI_Wait
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup request
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
#if HAVE( DECL_PMPI_WAITALL ) && !defined( SCOREP_MPI_NO_REQUEST ) && !defined( MPI_Waitall )
/**
 * Measurement wrapper for MPI_Waitall
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup request
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

#if HAVE( DECL_PMPI_WAIT ) && !defined( SCOREP_MPI_NO_REQUEST )
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

#if HAVE( DECL_PMPI_WAITALL ) && !defined( SCOREP_MPI_NO_REQUEST )
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

#if HAVE( DECL_PMPI_WAITANY ) && !defined( SCOREP_MPI_NO_REQUEST )
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

#if HAVE( DECL_PMPI_WAITSOME ) && !defined( SCOREP_MPI_NO_REQUEST )
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

#if HAVE( DECL_PMPI_TEST ) && !defined( SCOREP_MPI_NO_REQUEST )
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

#if HAVE( DECL_PMPI_TESTANY ) && !defined( SCOREP_MPI_NO_REQUEST )
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

#if HAVE( DECL_PMPI_TESTALL ) && !defined( SCOREP_MPI_NO_REQUEST )
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

#if HAVE( DECL_PMPI_TESTSOME ) && !defined( SCOREP_MPI_NO_REQUEST )
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

#if HAVE( DECL_PMPI_STARTALL ) && !defined( SCOREP_MPI_NO_REQUEST )
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

#if HAVE( DECL_PMPI_REQUEST_FREE ) && !defined( SCOREP_MPI_NO_REQUEST )
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

#if HAVE( DECL_PMPI_CANCEL ) && !defined( SCOREP_MPI_NO_REQUEST )
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

#if HAVE( DECL_PMPI_TEST_CANCELLED ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_REQUEST ) && !defined( MPI_Test_cancelled )
/**
 * Measurement wrapper for MPI_Test_cancelled
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
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

#if HAVE( DECL_PMPI_START ) && !defined( SCOREP_MPI_NO_REQUEST ) && !defined( MPI_Start )
/**
 * Measurement wrapper for MPI_Start
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
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

#endif
