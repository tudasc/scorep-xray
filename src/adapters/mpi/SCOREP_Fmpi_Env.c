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
 * @brief Fortran interface wrappers for environmental management
 */

#include <config.h>
#include "SCOREP_Fmpi.h"

#include <stdlib.h>
#include <UTILS_Error.h>

/* uppercase defines */

/** @def MPI_Finalize_U
    Exchanges MPI_Finalize_U with MPI_FINALIZE.
    It is used for the Fortran wrappers of MPI_Finalize.
 */
#define MPI_Finalize_U MPI_FINALIZE

/** @def MPI_Finalized_U
    Exchanges MPI_Finalized_U with MPI_FINALIZED.
    It is used for the Fortran wrappers of MPI_Finalized.
 */
#define MPI_Finalized_U MPI_FINALIZED

/** @def MPI_Get_library_version_U
    Exchanges MPI_Get_library_version_U with MPI_GET_LIBRARY_VERSION.
    It is used for the Fortran wrappers of MPI_Get_library_version.
 */
#define MPI_Get_library_version_U MPI_GET_LIBRARY_VERSION

/** @def MPI_Init_U
    Exchanges MPI_Init_U with MPI_INIT.
    It is used for the Fortran wrappers of MPI_Init.
 */
#define MPI_Init_U MPI_INIT

/** @def MPI_Init_thread_U
    Exchanges MPI_Init_thread_U with MPI_INIT_THREAD.
    It is used for the Fortran wrappers of MPI_Init_thread.
 */
#define MPI_Init_thread_U MPI_INIT_THREAD

/** @def MPI_Initialized_U
    Exchanges MPI_Initialized_U with MPI_INITIALIZED.
    It is used for the Fortran wrappers of MPI_Initialized.
 */
#define MPI_Initialized_U MPI_INITIALIZED

/** @def MPI_Is_thread_main_U
    Exchanges MPI_Is_thread_main_U with MPI_IS_THREAD_MAIN.
    It is used for the Fortran wrappers of MPI_Is_thread_main.
 */
#define MPI_Is_thread_main_U MPI_IS_THREAD_MAIN

/** @def MPI_Query_thread_U
    Exchanges MPI_Query_thread_U with MPI_QUERY_THREAD.
    It is used for the Fortran wrappers of MPI_Query_thread.
 */
#define MPI_Query_thread_U MPI_QUERY_THREAD

/** @def MPI_Session_call_errhandler_U
    Exchanges MPI_Session_call_errhandler_U with MPI_SESSION_CALL_ERRHANDLER.
    It is used for the Fortran wrappers of MPI_Session_call_errhandler.
 */
#define MPI_Session_call_errhandler_U MPI_SESSION_CALL_ERRHANDLER

/** @def MPI_Session_create_errhandler_U
    Exchanges MPI_Session_create_errhandler_U with MPI_SESSION_CREATE_ERRHANDLER.
    It is used for the Fortran wrappers of MPI_Session_create_errhandler.
 */
#define MPI_Session_create_errhandler_U MPI_SESSION_CREATE_ERRHANDLER

/** @def MPI_Session_finalize_U
    Exchanges MPI_Session_finalize_U with MPI_SESSION_FINALIZE.
    It is used for the Fortran wrappers of MPI_Session_finalize.
 */
#define MPI_Session_finalize_U MPI_SESSION_FINALIZE

/** @def MPI_Session_get_errhandler_U
    Exchanges MPI_Session_get_errhandler_U with MPI_SESSION_GET_ERRHANDLER.
    It is used for the Fortran wrappers of MPI_Session_get_errhandler.
 */
#define MPI_Session_get_errhandler_U MPI_SESSION_GET_ERRHANDLER

/** @def MPI_Session_get_info_U
    Exchanges MPI_Session_get_info_U with MPI_SESSION_GET_INFO.
    It is used for the Fortran wrappers of MPI_Session_get_info.
 */
#define MPI_Session_get_info_U MPI_SESSION_GET_INFO

/** @def MPI_Session_get_nth_pset_U
    Exchanges MPI_Session_get_nth_pset_U with MPI_SESSION_GET_NTH_PSET.
    It is used for the Fortran wrappers of MPI_Session_get_nth_pset.
 */
#define MPI_Session_get_nth_pset_U MPI_SESSION_GET_NTH_PSET

/** @def MPI_Session_get_num_psets_U
    Exchanges MPI_Session_get_num_psets_U with MPI_SESSION_GET_NUM_PSETS.
    It is used for the Fortran wrappers of MPI_Session_get_num_psets.
 */
#define MPI_Session_get_num_psets_U MPI_SESSION_GET_NUM_PSETS

/** @def MPI_Session_get_pset_info_U
    Exchanges MPI_Session_get_pset_info_U with MPI_SESSION_GET_PSET_INFO.
    It is used for the Fortran wrappers of MPI_Session_get_pset_info.
 */
#define MPI_Session_get_pset_info_U MPI_SESSION_GET_PSET_INFO

/** @def MPI_Session_init_U
    Exchanges MPI_Session_init_U with MPI_SESSION_INIT.
    It is used for the Fortran wrappers of MPI_Session_init.
 */
#define MPI_Session_init_U MPI_SESSION_INIT

/** @def MPI_Session_set_errhandler_U
    Exchanges MPI_Session_set_errhandler_U with MPI_SESSION_SET_ERRHANDLER.
    It is used for the Fortran wrappers of MPI_Session_set_errhandler.
 */
#define MPI_Session_set_errhandler_U MPI_SESSION_SET_ERRHANDLER

/* lowercase defines */

/** @def MPI_Finalize_L
    Exchanges MPI_Finalize_L with mpi_finalize.
    It is used for the Fortran wrappers of MPI_Finalize.
 */
#define MPI_Finalize_L mpi_finalize

/** @def MPI_Finalized_L
    Exchanges MPI_Finalized_L with mpi_finalized.
    It is used for the Fortran wrappers of MPI_Finalized.
 */
#define MPI_Finalized_L mpi_finalized

/** @def MPI_Get_library_version_L
    Exchanges MPI_Get_library_version_L with mpi_get_library_version.
    It is used for the Fortran wrappers of MPI_Get_library_version.
 */
#define MPI_Get_library_version_L mpi_get_library_version

/** @def MPI_Init_L
    Exchanges MPI_Init_L with mpi_init.
    It is used for the Fortran wrappers of MPI_Init.
 */
#define MPI_Init_L mpi_init

/** @def MPI_Init_thread_L
    Exchanges MPI_Init_thread_L with mpi_init_thread.
    It is used for the Fortran wrappers of MPI_Init_thread.
 */
#define MPI_Init_thread_L mpi_init_thread

/** @def MPI_Initialized_L
    Exchanges MPI_Initialized_L with mpi_initialized.
    It is used for the Fortran wrappers of MPI_Initialized.
 */
#define MPI_Initialized_L mpi_initialized

/** @def MPI_Is_thread_main_L
    Exchanges MPI_Is_thread_main_L with mpi_is_thread_main.
    It is used for the Fortran wrappers of MPI_Is_thread_main.
 */
#define MPI_Is_thread_main_L mpi_is_thread_main

/** @def MPI_Query_thread_L
    Exchanges MPI_Query_thread_L with mpi_query_thread.
    It is used for the Fortran wrappers of MPI_Query_thread.
 */
#define MPI_Query_thread_L mpi_query_thread

/** @def MPI_Session_call_errhandler_L
    Exchanges MPI_Session_call_errhandler_L with mpi_session_call_errhandler.
    It is used for the Fortran wrappers of MPI_Session_call_errhandler.
 */
#define MPI_Session_call_errhandler_L mpi_session_call_errhandler

/** @def MPI_Session_create_errhandler_L
    Exchanges MPI_Session_create_errhandler_L with mpi_session_create_errhandler.
    It is used for the Fortran wrappers of MPI_Session_create_errhandler.
 */
#define MPI_Session_create_errhandler_L mpi_session_create_errhandler

/** @def MPI_Session_finalize_L
    Exchanges MPI_Session_finalize_L with mpi_session_finalize.
    It is used for the Fortran wrappers of MPI_Session_finalize.
 */
#define MPI_Session_finalize_L mpi_session_finalize

/** @def MPI_Session_get_errhandler_L
    Exchanges MPI_Session_get_errhandler_L with mpi_session_get_errhandler.
    It is used for the Fortran wrappers of MPI_Session_get_errhandler.
 */
#define MPI_Session_get_errhandler_L mpi_session_get_errhandler

/** @def MPI_Session_get_info_L
    Exchanges MPI_Session_get_info_L with mpi_session_get_info.
    It is used for the Fortran wrappers of MPI_Session_get_info.
 */
#define MPI_Session_get_info_L mpi_session_get_info

/** @def MPI_Session_get_nth_pset_L
    Exchanges MPI_Session_get_nth_pset_L with mpi_session_get_nth_pset.
    It is used for the Fortran wrappers of MPI_Session_get_nth_pset.
 */
#define MPI_Session_get_nth_pset_L mpi_session_get_nth_pset

/** @def MPI_Session_get_num_psets_L
    Exchanges MPI_Session_get_num_psets_L with mpi_session_get_num_psets.
    It is used for the Fortran wrappers of MPI_Session_get_num_psets.
 */
#define MPI_Session_get_num_psets_L mpi_session_get_num_psets

/** @def MPI_Session_get_pset_info_L
    Exchanges MPI_Session_get_pset_info_L with mpi_session_get_pset_info.
    It is used for the Fortran wrappers of MPI_Session_get_pset_info.
 */
#define MPI_Session_get_pset_info_L mpi_session_get_pset_info

/** @def MPI_Session_init_L
    Exchanges MPI_Session_init_L with mpi_session_init.
    It is used for the Fortran wrappers of MPI_Session_init.
 */
#define MPI_Session_init_L mpi_session_init

/** @def MPI_Session_set_errhandler_L
    Exchanges MPI_Session_set_errhandler_L with mpi_session_set_errhandler.
    It is used for the Fortran wrappers of MPI_Session_set_errhandler.
 */
#define MPI_Session_set_errhandler_L mpi_session_set_errhandler

/**
 * @name Fortran wrappers
 * @{
 */

#ifndef NEED_F2C_CONV

#if HAVE( MPI_1_0_SYMBOL_PMPI_INITIALIZED ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_ENV ) && !defined( MPI_Initialized )
/**
 * Measurement wrapper for MPI_Initialized
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1.0
 * @ingroup env
 */
void
FSUB( MPI_Initialized )( int* flag, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Initialized( flag );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif

/* If MPI_Comm_f2c is HAVE as a simple macro (which typically does nothing)
 * as it would require a "real" function if it is really needed
 * => we can save the f2c and c2s conversions */

#if HAVE( MPI_2_0_SYMBOL_PMPI_INIT_THREAD )
/**
 * Measurement wrapper for MPI_Init_tread
 * @note Manually adapted wrapper
 * @note Fortran interface
 * @note Introduced with MPI-2.0
 * @ingroup env
 */
void
FSUB( MPI_Init_thread )( int* required,
                         int* provided,
                         int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    *ierr = MPI_Init_thread( 0, ( char*** )0, *required, provided );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif

/**
 * Measurement wrapper for MPI_Init
 * @note Manually adapted wrapper
 * @note Fortran interface
 * @note Introduced with MPI-1.0
 * @ingroup env
 */
void
FSUB( MPI_Init )( int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    *ierr = MPI_Init( 0, ( char*** )0 );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}

#if HAVE( MPI_1_0_SYMBOL_PMPI_FINALIZE )
/**
 * Measurement wrapper for MPI_Finalize
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1.0
 * @ingroup env
 */
void
FSUB( MPI_Finalize )( int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Finalize();

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_2_0_SYMBOL_PMPI_FINALIZED ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_ENV ) && !defined( MPI_Finalized )
/**
 * Measurement wrapper for MPI_Finalized
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2.0
 * @ingroup env
 */
void
FSUB( MPI_Finalized )( int* flag, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Finalized( flag );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_3_0_SYMBOL_PMPI_GET_LIBRARY_VERSION ) && !defined( SCOREP_MPI_NO_ENV ) && !defined( MPI_Get_library_version )
/**
 * Measurement wrapper for MPI_Get_library_version
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-3.0
 * @ingroup env
 */
void
FSUB( MPI_Get_library_version )( char* version, int* resultlen, int* ierr, scorep_fortran_charlen_t version_len )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    char*  c_version     = NULL;
    size_t c_version_len = 0;
    c_version = ( char* )malloc( ( version_len + 1 ) * sizeof( char ) );
    if ( !c_version )
    {
        exit( EXIT_FAILURE );
    }



    *ierr = MPI_Get_library_version( c_version, resultlen );


    c_version_len = strlen( c_version );
    memcpy( version, c_version, c_version_len );
    memset( version + c_version_len, ' ', version_len - c_version_len );
    free( c_version );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_2_0_SYMBOL_PMPI_IS_THREAD_MAIN ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_ENV ) && !defined( MPI_Is_thread_main )
/**
 * Measurement wrapper for MPI_Is_thread_main
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2.0
 * @ingroup env
 */
void
FSUB( MPI_Is_thread_main )( int* flag, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Is_thread_main( flag );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_2_0_SYMBOL_PMPI_QUERY_THREAD ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_ENV ) && !defined( MPI_Query_thread )
/**
 * Measurement wrapper for MPI_Query_thread
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2.0
 * @ingroup env
 */
void
FSUB( MPI_Query_thread )( int* provided, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Query_thread( provided );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_4_0_SYMBOL_PMPI_SESSION_CALL_ERRHANDLER ) && !defined( SCOREP_MPI_NO_ENV ) && !defined( SCOREP_MPI_NO_ERR ) && !defined( MPI_Session_call_errhandler )
/**
 * Measurement wrapper for MPI_Session_call_errhandler
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-4.0
 * @ingroup env
 */
void
FSUB( MPI_Session_call_errhandler )( MPI_Session* session, int* errorcode, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Session_call_errhandler( *session, *errorcode );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_4_0_SYMBOL_PMPI_SESSION_CREATE_ERRHANDLER ) && !defined( SCOREP_MPI_NO_ENV ) && !defined( SCOREP_MPI_NO_ERR ) && !defined( MPI_Session_create_errhandler )
/**
 * Measurement wrapper for MPI_Session_create_errhandler
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-4.0
 * @ingroup env
 */
void
FSUB( MPI_Session_create_errhandler )( MPI_Session_errhandler_function* session_errhandler_fn, MPI_Errhandler* errhandler, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Session_create_errhandler( session_errhandler_fn, errhandler );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_4_0_SYMBOL_PMPI_SESSION_FINALIZE )
/**
 * Measurement wrapper for MPI_Session_finalize
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-4.0
 * @ingroup env
 */
void
FSUB( MPI_Session_finalize )( MPI_Session* session, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Session_finalize( session );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_4_0_SYMBOL_PMPI_SESSION_GET_ERRHANDLER ) && !defined( SCOREP_MPI_NO_ENV ) && !defined( SCOREP_MPI_NO_ERR ) && !defined( MPI_Session_get_errhandler )
/**
 * Measurement wrapper for MPI_Session_get_errhandler
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-4.0
 * @ingroup env
 */
void
FSUB( MPI_Session_get_errhandler )( MPI_Session* session, MPI_Errhandler* errhandler, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Session_get_errhandler( *session, errhandler );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_4_0_SYMBOL_PMPI_SESSION_GET_INFO ) && !defined( SCOREP_MPI_NO_ENV ) && !defined( SCOREP_MPI_NO_EXT ) && !defined( MPI_Session_get_info )
/**
 * Measurement wrapper for MPI_Session_get_info
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-4.0
 * @ingroup env
 */
void
FSUB( MPI_Session_get_info )( MPI_Session* session, MPI_Info* info_used, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Session_get_info( *session, info_used );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_4_0_SYMBOL_PMPI_SESSION_GET_NTH_PSET ) && !defined( SCOREP_MPI_NO_ENV ) && !defined( SCOREP_MPI_NO_EXT ) && !defined( MPI_Session_get_nth_pset )
/**
 * Measurement wrapper for MPI_Session_get_nth_pset
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-4.0
 * @ingroup env
 */
void
FSUB( MPI_Session_get_nth_pset )( MPI_Session* session, MPI_Info* info, int* n, int* pset_len, char* pset_name, int* ierr, scorep_fortran_charlen_t pset_name_len )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    char*  c_pset_name     = NULL;
    size_t c_pset_name_len = 0;
    c_pset_name = ( char* )malloc( ( pset_name_len + 1 ) * sizeof( char ) );
    if ( !c_pset_name )
    {
        exit( EXIT_FAILURE );
    }



    *ierr = MPI_Session_get_nth_pset( *session, *info, *n, pset_len, c_pset_name );


    c_pset_name_len = strlen( c_pset_name );
    memcpy( pset_name, c_pset_name, c_pset_name_len );
    memset( pset_name + c_pset_name_len, ' ', pset_name_len - c_pset_name_len );
    free( c_pset_name );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_4_0_SYMBOL_PMPI_SESSION_GET_NUM_PSETS ) && !defined( SCOREP_MPI_NO_ENV ) && !defined( SCOREP_MPI_NO_EXT ) && !defined( MPI_Session_get_num_psets )
/**
 * Measurement wrapper for MPI_Session_get_num_psets
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-4.0
 * @ingroup env
 */
void
FSUB( MPI_Session_get_num_psets )( MPI_Session* session, MPI_Info* info, int* npset_names, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Session_get_num_psets( *session, *info, npset_names );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_4_0_SYMBOL_PMPI_SESSION_GET_PSET_INFO ) && !defined( SCOREP_MPI_NO_ENV ) && !defined( SCOREP_MPI_NO_EXT ) && !defined( MPI_Session_get_pset_info )
/**
 * Measurement wrapper for MPI_Session_get_pset_info
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-4.0
 * @ingroup env
 */
void
FSUB( MPI_Session_get_pset_info )( MPI_Session* session, char* pset_name, MPI_Info* info, int* ierr, scorep_fortran_charlen_t pset_name_len )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    char* c_pset_name = NULL;
    c_pset_name = scorep_mpi_f2c_string( pset_name, pset_name_len );



    *ierr = MPI_Session_get_pset_info( *session, c_pset_name, info );

    free( c_pset_name );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_4_0_SYMBOL_PMPI_SESSION_INIT )
/**
 * Measurement wrapper for MPI_Session_init
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-4.0
 * @ingroup env
 */
void
FSUB( MPI_Session_init )( MPI_Info* info, MPI_Errhandler* errhandler, MPI_Session* session, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Session_init( *info, *errhandler, session );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_4_0_SYMBOL_PMPI_SESSION_SET_ERRHANDLER ) && !defined( SCOREP_MPI_NO_ENV ) && !defined( SCOREP_MPI_NO_ERR ) && !defined( MPI_Session_set_errhandler )
/**
 * Measurement wrapper for MPI_Session_set_errhandler
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-4.0
 * @ingroup env
 */
void
FSUB( MPI_Session_set_errhandler )( MPI_Session* session, MPI_Errhandler* errhandler, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Session_set_errhandler( *session, *errhandler );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif

#else /* !NEED_F2C_CONV */

#if HAVE( MPI_2_0_SYMBOL_PMPI_INIT_THREAD )
/**
 * Measurement wrapper for MPI_Init_tread
 * @note Manually adapted wrapper
 * @note Fortran interface
 * @note Introduced with MPI-2.0
 * @ingroup env
 */
void
FSUB( MPI_Init_thread )( MPI_Fint* required,
                         MPI_Fint* provided,
                         MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    if ( sizeof( MPI_Fint ) != sizeof( int ) )
    {
        UTILS_ERROR( SCOREP_ERROR_F2C_INT_SIZE_MISMATCH,
                     "Shouldn't this error abort!" );
    }
    *ierr = MPI_Init_thread( 0, ( char*** )0, *required, provided );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif

/**
 * Measurement wrapper for MPI_Init
 * @note Manually adapted wrapper
 * @note Fortran interface
 * @note Introduced with MPI-1.0
 * @ingroup env
 */
void
FSUB( MPI_Init )( MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    if ( sizeof( MPI_Fint ) != sizeof( int ) )
    {
        UTILS_ERROR( SCOREP_ERROR_F2C_INT_SIZE_MISMATCH,
                     "Shouldn't this error abort!" );
    }
    *ierr = MPI_Init( 0, ( char*** )0 );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}

#if HAVE( MPI_1_0_SYMBOL_PMPI_FINALIZE )
/**
 * Measurement wrapper for MPI_Finalize
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1.0
 * @ingroup env
 * For the order of events see @ref MPI_Finalize
 */
void
FSUB( MPI_Finalize )( MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Finalize();

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_2_0_SYMBOL_PMPI_FINALIZED ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_ENV ) && !defined( MPI_Finalized )
/**
 * Measurement wrapper for MPI_Finalized
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2.0
 * @ingroup env
 * For the order of events see @ref MPI_Finalized
 */
void
FSUB( MPI_Finalized )( MPI_Fint* flag, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Finalized( flag );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_3_0_SYMBOL_PMPI_GET_LIBRARY_VERSION ) && !defined( SCOREP_MPI_NO_ENV ) && !defined( MPI_Get_library_version )
/**
 * Measurement wrapper for MPI_Get_library_version
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-3.0
 * @ingroup env
 * For the order of events see @ref MPI_Get_library_version
 */
void
FSUB( MPI_Get_library_version )( char* version, MPI_Fint* resultlen, MPI_Fint* ierr, scorep_fortran_charlen_t version_len )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    char*  c_version     = NULL;
    size_t c_version_len = 0;
    c_version = ( char* )malloc( ( version_len + 1 ) * sizeof( char ) );
    if ( !c_version )
    {
        exit( EXIT_FAILURE );
    }



    *ierr = MPI_Get_library_version( c_version, resultlen );


    c_version_len = strlen( c_version );
    memcpy( version, c_version, c_version_len );
    memset( version + c_version_len, ' ', version_len - c_version_len );
    free( c_version );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_2_0_SYMBOL_PMPI_IS_THREAD_MAIN ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_ENV ) && !defined( MPI_Is_thread_main )
/**
 * Measurement wrapper for MPI_Is_thread_main
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2.0
 * @ingroup env
 * For the order of events see @ref MPI_Is_thread_main
 */
void
FSUB( MPI_Is_thread_main )( MPI_Fint* flag, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Is_thread_main( flag );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_2_0_SYMBOL_PMPI_QUERY_THREAD ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_ENV ) && !defined( MPI_Query_thread )
/**
 * Measurement wrapper for MPI_Query_thread
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2.0
 * @ingroup env
 * For the order of events see @ref MPI_Query_thread
 */
void
FSUB( MPI_Query_thread )( MPI_Fint* provided, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Query_thread( provided );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_4_0_SYMBOL_PMPI_SESSION_CALL_ERRHANDLER ) && !defined( SCOREP_MPI_NO_ENV ) && !defined( SCOREP_MPI_NO_ERR ) && !defined( MPI_Session_call_errhandler )
/**
 * Measurement wrapper for MPI_Session_call_errhandler
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-4.0
 * @ingroup env
 * For the order of events see @ref MPI_Session_call_errhandler
 */
void
FSUB( MPI_Session_call_errhandler )( MPI_Fint* session, MPI_Fint* errorcode, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Session_call_errhandler( PMPI_Session_f2c( *session ), *errorcode );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_4_0_SYMBOL_PMPI_SESSION_CREATE_ERRHANDLER ) && !defined( SCOREP_MPI_NO_ENV ) && !defined( SCOREP_MPI_NO_ERR ) && !defined( MPI_Session_create_errhandler )
/**
 * Measurement wrapper for MPI_Session_create_errhandler
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-4.0
 * @ingroup env
 * For the order of events see @ref MPI_Session_create_errhandler
 */
void
FSUB( MPI_Session_create_errhandler )( void* session_errhandler_fn, void* errhandler, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Session_create_errhandler( ( MPI_Session_errhandler_function* )session_errhandler_fn, ( MPI_Errhandler* )errhandler );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_4_0_SYMBOL_PMPI_SESSION_FINALIZE )
/**
 * Measurement wrapper for MPI_Session_finalize
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-4.0
 * @ingroup env
 * For the order of events see @ref MPI_Session_finalize
 */
void
FSUB( MPI_Session_finalize )( MPI_Fint* session, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Session c_session = PMPI_Session_f2c( *session );


    *ierr = MPI_Session_finalize( &c_session );

    *session = PMPI_Session_c2f( c_session );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_4_0_SYMBOL_PMPI_SESSION_GET_ERRHANDLER ) && !defined( SCOREP_MPI_NO_ENV ) && !defined( SCOREP_MPI_NO_ERR ) && !defined( MPI_Session_get_errhandler )
/**
 * Measurement wrapper for MPI_Session_get_errhandler
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-4.0
 * @ingroup env
 * For the order of events see @ref MPI_Session_get_errhandler
 */
void
FSUB( MPI_Session_get_errhandler )( MPI_Fint* session, void* errhandler, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Session_get_errhandler( PMPI_Session_f2c( *session ), ( MPI_Errhandler* )errhandler );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_4_0_SYMBOL_PMPI_SESSION_GET_INFO ) && !defined( SCOREP_MPI_NO_ENV ) && !defined( SCOREP_MPI_NO_EXT ) && !defined( MPI_Session_get_info )
/**
 * Measurement wrapper for MPI_Session_get_info
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-4.0
 * @ingroup env
 * For the order of events see @ref MPI_Session_get_info
 */
void
FSUB( MPI_Session_get_info )( MPI_Fint* session, MPI_Fint* info_used, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Info c_info_used;


    *ierr = MPI_Session_get_info( PMPI_Session_f2c( *session ), &c_info_used );

    *info_used = PMPI_Info_c2f( c_info_used );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_4_0_SYMBOL_PMPI_SESSION_GET_NTH_PSET ) && !defined( SCOREP_MPI_NO_ENV ) && !defined( SCOREP_MPI_NO_EXT ) && !defined( MPI_Session_get_nth_pset )
/**
 * Measurement wrapper for MPI_Session_get_nth_pset
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-4.0
 * @ingroup env
 * For the order of events see @ref MPI_Session_get_nth_pset
 */
void
FSUB( MPI_Session_get_nth_pset )( MPI_Fint* session, MPI_Fint* info, MPI_Fint* n, MPI_Fint* pset_len, char* pset_name, MPI_Fint* ierr, scorep_fortran_charlen_t pset_name_len )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    char*  c_pset_name     = NULL;
    size_t c_pset_name_len = 0;
    c_pset_name = ( char* )malloc( ( pset_name_len + 1 ) * sizeof( char ) );
    if ( !c_pset_name )
    {
        exit( EXIT_FAILURE );
    }



    *ierr = MPI_Session_get_nth_pset( PMPI_Session_f2c( *session ), PMPI_Info_f2c( *info ), *n, pset_len, c_pset_name );


    c_pset_name_len = strlen( c_pset_name );
    memcpy( pset_name, c_pset_name, c_pset_name_len );
    memset( pset_name + c_pset_name_len, ' ', pset_name_len - c_pset_name_len );
    free( c_pset_name );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_4_0_SYMBOL_PMPI_SESSION_GET_NUM_PSETS ) && !defined( SCOREP_MPI_NO_ENV ) && !defined( SCOREP_MPI_NO_EXT ) && !defined( MPI_Session_get_num_psets )
/**
 * Measurement wrapper for MPI_Session_get_num_psets
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-4.0
 * @ingroup env
 * For the order of events see @ref MPI_Session_get_num_psets
 */
void
FSUB( MPI_Session_get_num_psets )( MPI_Fint* session, MPI_Fint* info, MPI_Fint* npset_names, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Session_get_num_psets( PMPI_Session_f2c( *session ), PMPI_Info_f2c( *info ), npset_names );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_4_0_SYMBOL_PMPI_SESSION_GET_PSET_INFO ) && !defined( SCOREP_MPI_NO_ENV ) && !defined( SCOREP_MPI_NO_EXT ) && !defined( MPI_Session_get_pset_info )
/**
 * Measurement wrapper for MPI_Session_get_pset_info
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-4.0
 * @ingroup env
 * For the order of events see @ref MPI_Session_get_pset_info
 */
void
FSUB( MPI_Session_get_pset_info )( MPI_Fint* session, char* pset_name, MPI_Fint* info, MPI_Fint* ierr, scorep_fortran_charlen_t pset_name_len )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    char*    c_pset_name = NULL;
    MPI_Info c_info;
    c_pset_name = scorep_mpi_f2c_string( pset_name, pset_name_len );



    *ierr = MPI_Session_get_pset_info( PMPI_Session_f2c( *session ), c_pset_name, &c_info );

    free( c_pset_name );
    *info = PMPI_Info_c2f( c_info );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_4_0_SYMBOL_PMPI_SESSION_INIT )
/**
 * Measurement wrapper for MPI_Session_init
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-4.0
 * @ingroup env
 * For the order of events see @ref MPI_Session_init
 */
void
FSUB( MPI_Session_init )( MPI_Fint* info, void* errhandler, MPI_Fint* session, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Session c_session;


    *ierr = MPI_Session_init( PMPI_Info_f2c( *info ), *( ( MPI_Errhandler* )errhandler ), &c_session );

    *session = PMPI_Session_c2f( c_session );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( MPI_4_0_SYMBOL_PMPI_SESSION_SET_ERRHANDLER ) && !defined( SCOREP_MPI_NO_ENV ) && !defined( SCOREP_MPI_NO_ERR ) && !defined( MPI_Session_set_errhandler )
/**
 * Measurement wrapper for MPI_Session_set_errhandler
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-4.0
 * @ingroup env
 * For the order of events see @ref MPI_Session_set_errhandler
 */
void
FSUB( MPI_Session_set_errhandler )( MPI_Fint* session, void* errhandler, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Session_set_errhandler( PMPI_Session_f2c( *session ), *( ( MPI_Errhandler* )errhandler ) );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif

#if HAVE( MPI_1_0_SYMBOL_PMPI_INITIALIZED ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_ENV ) && !defined( MPI_Initialized )
/**
 * Measurement wrapper for MPI_Initialized
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1.0
 * @ingroup env
 */
void
FSUB( MPI_Initialized )( int* flag, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Initialized( flag );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif

#endif

/**
 * @}
 */
