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

#include "SILC_Fmpi.h"

/**
 * @file  SILC_Fmpi_Ext.c
 *
 * @brief Fortran interface wrappers for external interface functions
 */

/* uppercase defines */
/** @def MPI_Abort_U
    Exchange MPI_Abort_U by MPI_ABORT.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Abort_U MPI_ABORT

/** @def MPI_Get_count_U
    Exchange MPI_Get_count_U by MPI_GET_COUNT.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Get_count_U MPI_GET_COUNT

/** @def MPI_Get_elements_U
    Exchange MPI_Get_elements_U by MPI_GET_ELEMENTS.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Get_elements_U MPI_GET_ELEMENTS

/** @def MPI_Get_processor_name_U
    Exchange MPI_Get_processor_name_U by MPI_GET_PROCESSOR_NAME.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Get_processor_name_U MPI_GET_PROCESSOR_NAME

/** @def MPI_Grequest_complete_U
    Exchange MPI_Grequest_complete_U by MPI_GREQUEST_COMPLETE.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Grequest_complete_U MPI_GREQUEST_COMPLETE

/** @def MPI_Grequest_start_U
    Exchange MPI_Grequest_start_U by MPI_GREQUEST_START.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Grequest_start_U MPI_GREQUEST_START

/** @def MPI_Status_set_cancelled_U
    Exchange MPI_Status_set_cancelled_U by MPI_STATUS_SET_CANCELLED.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Status_set_cancelled_U MPI_STATUS_SET_CANCELLED

/** @def MPI_Status_set_elements_U
    Exchange MPI_Status_set_elements_U by MPI_STATUS_SET_ELEMENTS.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Status_set_elements_U MPI_STATUS_SET_ELEMENTS

/** @def MPI_Wtick_U
    Exchange MPI_Wtick_U by MPI_WTICK.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Wtick_U MPI_WTICK

/** @def MPI_Wtime_U
    Exchange MPI_Wtime_U by MPI_WTIME.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Wtime_U MPI_WTIME


/* lowercase defines */
/** @def MPI_Abort_L
    Exchanges MPI_Abort_L by mpi_abort.
    It is used for the Forran wrappers of me).
 */
#define MPI_Abort_L mpi_abort

/** @def MPI_Get_count_L
    Exchanges MPI_Get_count_L by mpi_get_count.
    It is used for the Forran wrappers of me).
 */
#define MPI_Get_count_L mpi_get_count

/** @def MPI_Get_elements_L
    Exchanges MPI_Get_elements_L by mpi_get_elements.
    It is used for the Forran wrappers of me).
 */
#define MPI_Get_elements_L mpi_get_elements

/** @def MPI_Get_processor_name_L
    Exchanges MPI_Get_processor_name_L by mpi_get_processor_name.
    It is used for the Forran wrappers of me).
 */
#define MPI_Get_processor_name_L mpi_get_processor_name

/** @def MPI_Grequest_complete_L
    Exchanges MPI_Grequest_complete_L by mpi_grequest_complete.
    It is used for the Forran wrappers of me).
 */
#define MPI_Grequest_complete_L mpi_grequest_complete

/** @def MPI_Grequest_start_L
    Exchanges MPI_Grequest_start_L by mpi_grequest_start.
    It is used for the Forran wrappers of me).
 */
#define MPI_Grequest_start_L mpi_grequest_start

/** @def MPI_Status_set_cancelled_L
    Exchanges MPI_Status_set_cancelled_L by mpi_status_set_cancelled.
    It is used for the Forran wrappers of me).
 */
#define MPI_Status_set_cancelled_L mpi_status_set_cancelled

/** @def MPI_Status_set_elements_L
    Exchanges MPI_Status_set_elements_L by mpi_status_set_elements.
    It is used for the Forran wrappers of me).
 */
#define MPI_Status_set_elements_L mpi_status_set_elements

/** @def MPI_Wtick_L
    Exchanges MPI_Wtick_L by mpi_wtick.
    It is used for the Forran wrappers of me).
 */
#define MPI_Wtick_L mpi_wtick

/** @def MPI_Wtime_L
    Exchanges MPI_Wtime_L by mpi_wtime.
    It is used for the Forran wrappers of me).
 */
#define MPI_Wtime_L mpi_wtime


/**
 * @name Fortran wrappers
 * @{
 */

#ifndef NEED_F2C_CONV

/* If MPI_Comm_f2c is defined as a simple macro (which typically does nothing)
 * as it would require a "real" function if it is really needed
 * => we can save the f2c and c2s conversions */

#if defined( HAS_MPI_EXTRA )
/**
 * Measurement wrapper for MPI_Abort
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup ext
 */
void
FSUB( MPI_Abort ) ( MPI_Comm * comm, int* errorcode, int* ierr )
{
    *ierr = MPI_Abort( *comm, *errorcode );
}
#endif
#if defined( HAS_MPI_EXTRA )
/**
 * Measurement wrapper for MPI_Get_count
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup ext
 */
void
FSUB( MPI_Get_count ) ( MPI_Status * status, MPI_Datatype * datatype, int* count, int* ierr )
{
    *ierr = MPI_Get_count( status, *datatype, count );
}
#endif
#if defined( HAS_MPI_EXTRA )
/**
 * Measurement wrapper for MPI_Get_elements
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup ext
 */
void
FSUB( MPI_Get_elements ) ( MPI_Status * status, MPI_Datatype * datatype, int* count, int* ierr )
{
    *ierr = MPI_Get_elements( status, *datatype, count );
}
#endif
#if defined( HAS_MPI_EXTRA )
/**
 * Measurement wrapper for MPI_Get_processor_name
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup ext
 */
void
FSUB( MPI_Get_processor_name ) ( char* name, int* resultlen, int* ierr, int name_len )
{
    char* c_name = NULL;
    c_name = ( char* )malloc( ( name_len + 1 ) * sizeof( char ) );
    if ( !c_name )
    {
        exit( EXIT_FAILURE );
    }
    strncpy( c_name, name, name_len );
    c_name[ name_len ] = '\0';


    *ierr = MPI_Get_processor_name( c_name, resultlen );

    free( c_name );
}
#endif
#if defined( HAS_MPI_EXTRA )
/**
 * Measurement wrapper for MPI_Grequest_complete
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup ext
 */
void
FSUB( MPI_Grequest_complete ) ( MPI_Request * request, int* ierr )
{
    *ierr = MPI_Grequest_complete( *request );
}
#endif
#if defined( HAS_MPI_EXTRA )
/**
 * Measurement wrapper for MPI_Grequest_start
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup ext
 */
void
FSUB( MPI_Grequest_start ) ( MPI_Grequest_query_function * query_fn, MPI_Grequest_free_function * free_fn, MPI_Grequest_cancel_function * cancel_fn, void* extra_state, MPI_Request * request, int* ierr )
{
    *ierr = MPI_Grequest_start( query_fn, free_fn, cancel_fn, extra_state, request );
}
#endif
#if defined( HAS_MPI_EXTRA )
/**
 * Measurement wrapper for MPI_Status_set_cancelled
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup ext
 */
void
FSUB( MPI_Status_set_cancelled ) ( MPI_Status * status, int* flag, int* ierr )
{
    *ierr = MPI_Status_set_cancelled( status, *flag );
}
#endif
#if defined( HAS_MPI_EXTRA )
/**
 * Measurement wrapper for MPI_Status_set_elements
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup ext
 */
void
FSUB( MPI_Status_set_elements ) ( MPI_Status * status, MPI_Datatype * datatype, int* count, int* ierr )
{
    *ierr = MPI_Status_set_elements( status, *datatype, *count );
}
#endif

#else /* !NEED_F2C_CONV */

#if defined( HAS_MPI_EXTRA )
/**
 * Measurement wrapper for MPI_Abort
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup ext
 */
void
FSUB( MPI_Abort ) ( MPI_Fint * comm, MPI_Fint * errorcode, int* ierr )
{
    *ierr = MPI_Abort( PMPI_Comm_f2c( *comm ), *errorcode );
}
#endif
#if defined( HAS_MPI_EXTRA )
/**
 * Measurement wrapper for MPI_Get_count
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup ext
 */
void
FSUB( MPI_Get_count ) ( MPI_Fint * status, MPI_Fint * datatype, MPI_Fint * count, int* ierr )
{
    MPI_Status c_status;
    PMPI_Status_f2c( status, &c_status );
    *ierr = MPI_Get_count( &c_status, PMPI_Type_f2c( *datatype ), count );
}
#endif
#if defined( HAS_MPI_EXTRA )
/**
 * Measurement wrapper for MPI_Get_elements
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup ext
 */
void
FSUB( MPI_Get_elements ) ( MPI_Fint * status, MPI_Fint * datatype, MPI_Fint * count, int* ierr )
{
    MPI_Status c_status;
    PMPI_Status_f2c( status, &c_status );
    *ierr = MPI_Get_elements( &c_status, PMPI_Type_f2c( *datatype ), count );
}
#endif
#if defined( HAS_MPI_EXTRA )
/**
 * Measurement wrapper for MPI_Get_processor_name
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup ext
 */
void
FSUB( MPI_Get_processor_name ) ( char* name, MPI_Fint * resultlen, int* ierr, int name_len )
{
    char* c_name = NULL;
    c_name = ( char* )malloc( ( name_len + 1 ) * sizeof( char ) );
    if ( !c_name )
    {
        exit( EXIT_FAILURE );
    }
    strncpy( c_name, name, name_len );
    c_name[ name_len ] = '\0';

    *ierr = MPI_Get_processor_name( name, resultlen );
    free( c_name );
}
#endif
#if defined( HAS_MPI_EXTRA )
/**
 * Measurement wrapper for MPI_Grequest_complete
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup ext
 */
void
FSUB( MPI_Grequest_complete ) ( MPI_Fint * request, int* ierr )
{
    MPI_Request c_request = PMPI_Request_f2c( *request );
    *ierr    = MPI_Grequest_complete( c_request );
    *request = PMPI_Request_c2f( c_request );
}
#endif
#if defined( HAS_MPI_EXTRA )
/**
 * Measurement wrapper for MPI_Grequest_start
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup ext
 */
void
FSUB( MPI_Grequest_start ) ( void* query_fn, void* free_fn, void* cancel_fn, void* extra_state, MPI_Fint * request, int* ierr )
{
    MPI_Request c_request;
    *ierr    = MPI_Grequest_start( ( MPI_Grequest_query_function* )query_fn, ( MPI_Grequest_free_function* )free_fn, ( MPI_Grequest_cancel_function* )cancel_fn, extra_state, &c_request );
    *request = PMPI_Request_c2f( c_request );
}
#endif
#if defined( HAS_MPI_EXTRA )
/**
 * Measurement wrapper for MPI_Status_set_cancelled
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup ext
 */
void
FSUB( MPI_Status_set_cancelled ) ( MPI_Fint * status, MPI_Fint * flag, int* ierr )
{
    MPI_Status c_status;
    PMPI_Status_f2c( status, &c_status );
    *ierr = MPI_Status_set_cancelled( &c_status, *flag );
    MPI_Status_c2f( &c_status, status );
}
#endif
#if defined( HAS_MPI_EXTRA )
/**
 * Measurement wrapper for MPI_Status_set_elements
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup ext
 */
void
FSUB( MPI_Status_set_elements ) ( MPI_Fint * status, MPI_Fint * datatype, MPI_Fint * count, int* ierr )
{
    MPI_Status c_status;
    PMPI_Status_f2c( status, &c_status );
    *ierr = MPI_Status_set_elements( &c_status, PMPI_Type_f2c( *datatype ), *count );
    MPI_Status_c2f( &c_status, status );
}
#endif

#endif

/**
 * @}
 */
