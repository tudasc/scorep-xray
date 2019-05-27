/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2019,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2016, 2018-2019,
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
 * @brief Fortran interface wrappers for miscellaneous and handler conversion
 *        functions
 */

#include <config.h>
#include "SCOREP_Fmpi.h"

#include <UTILS_Debug.h>

/* uppercase defines */
/** @def MPI_Get_version_U
    Exchange MPI_Get_version_U by MPI_GET_VERSION.
    It is used for the Fortran wrappers of MPI_Get_version.
 */
#define MPI_Get_version_U MPI_GET_VERSION

/** @def MPI_Info_create_U
    Exchange MPI_Info_create_U by MPI_INFO_CREATE.
    It is used for the Fortran wrappers of MPI_Info_create.
 */
#define MPI_Info_create_U MPI_INFO_CREATE

/** @def MPI_Info_delete_U
    Exchange MPI_Info_delete_U by MPI_INFO_DELETE.
    It is used for the Fortran wrappers of MPI_Info_delete.
 */
#define MPI_Info_delete_U MPI_INFO_DELETE

/** @def MPI_Info_dup_U
    Exchange MPI_Info_dup_U by MPI_INFO_DUP.
    It is used for the Fortran wrappers of MPI_Info_dup.
 */
#define MPI_Info_dup_U MPI_INFO_DUP

/** @def MPI_Info_free_U
    Exchange MPI_Info_free_U by MPI_INFO_FREE.
    It is used for the Fortran wrappers of MPI_Info_free.
 */
#define MPI_Info_free_U MPI_INFO_FREE

/** @def MPI_Info_get_U
    Exchange MPI_Info_get_U by MPI_INFO_GET.
    It is used for the Fortran wrappers of MPI_Info_get.
 */
#define MPI_Info_get_U MPI_INFO_GET

/** @def MPI_Info_get_nkeys_U
    Exchange MPI_Info_get_nkeys_U by MPI_INFO_GET_NKEYS.
    It is used for the Fortran wrappers of MPI_Info_get_nkeys.
 */
#define MPI_Info_get_nkeys_U MPI_INFO_GET_NKEYS

/** @def MPI_Info_get_nthkey_U
    Exchange MPI_Info_get_nthkey_U by MPI_INFO_GET_NTHKEY.
    It is used for the Fortran wrappers of MPI_Info_get_nthkey.
 */
#define MPI_Info_get_nthkey_U MPI_INFO_GET_NTHKEY

/** @def MPI_Info_get_valuelen_U
    Exchange MPI_Info_get_valuelen_U by MPI_INFO_GET_VALUELEN.
    It is used for the Fortran wrappers of MPI_Info_get_valuelen.
 */
#define MPI_Info_get_valuelen_U MPI_INFO_GET_VALUELEN

/** @def MPI_Info_set_U
    Exchange MPI_Info_set_U by MPI_INFO_SET.
    It is used for the Fortran wrappers of MPI_Info_set.
 */
#define MPI_Info_set_U MPI_INFO_SET

/** @def MPI_Op_commutative_U
    Exchange MPI_Op_commutative_U by MPI_OP_COMMUTATIVE.
    It is used for the Fortran wrappers of MPI_Op_commutative.
 */
#define MPI_Op_commutative_U MPI_OP_COMMUTATIVE

/** @def MPI_Op_create_U
    Exchange MPI_Op_create_U by MPI_OP_CREATE.
    It is used for the Fortran wrappers of MPI_Op_create.
 */
#define MPI_Op_create_U MPI_OP_CREATE

/** @def MPI_Op_free_U
    Exchange MPI_Op_free_U by MPI_OP_FREE.
    It is used for the Fortran wrappers of MPI_Op_free.
 */
#define MPI_Op_free_U MPI_OP_FREE

/** @def MPI_Request_get_status_U
    Exchange MPI_Request_get_status_U by MPI_REQUEST_GET_STATUS.
    It is used for the Fortran wrappers of MPI_Request_get_status.
 */
#define MPI_Request_get_status_U MPI_REQUEST_GET_STATUS


/* lowercase defines */
/** @def MPI_Get_version_L
    Exchanges MPI_Get_version_L by mpi_get_version.
    It is used for the Fortran wrappers of MPI_Get_version.
 */
#define MPI_Get_version_L mpi_get_version

/** @def MPI_Info_create_L
    Exchanges MPI_Info_create_L by mpi_info_create.
    It is used for the Fortran wrappers of MPI_Info_create.
 */
#define MPI_Info_create_L mpi_info_create

/** @def MPI_Info_delete_L
    Exchanges MPI_Info_delete_L by mpi_info_delete.
    It is used for the Fortran wrappers of MPI_Info_delete.
 */
#define MPI_Info_delete_L mpi_info_delete

/** @def MPI_Info_dup_L
    Exchanges MPI_Info_dup_L by mpi_info_dup.
    It is used for the Fortran wrappers of MPI_Info_dup.
 */
#define MPI_Info_dup_L mpi_info_dup

/** @def MPI_Info_free_L
    Exchanges MPI_Info_free_L by mpi_info_free.
    It is used for the Fortran wrappers of MPI_Info_free.
 */
#define MPI_Info_free_L mpi_info_free

/** @def MPI_Info_get_L
    Exchanges MPI_Info_get_L by mpi_info_get.
    It is used for the Fortran wrappers of MPI_Info_get.
 */
#define MPI_Info_get_L mpi_info_get

/** @def MPI_Info_get_nkeys_L
    Exchanges MPI_Info_get_nkeys_L by mpi_info_get_nkeys.
    It is used for the Fortran wrappers of MPI_Info_get_nkeys.
 */
#define MPI_Info_get_nkeys_L mpi_info_get_nkeys

/** @def MPI_Info_get_nthkey_L
    Exchanges MPI_Info_get_nthkey_L by mpi_info_get_nthkey.
    It is used for the Fortran wrappers of MPI_Info_get_nthkey.
 */
#define MPI_Info_get_nthkey_L mpi_info_get_nthkey

/** @def MPI_Info_get_valuelen_L
    Exchanges MPI_Info_get_valuelen_L by mpi_info_get_valuelen.
    It is used for the Fortran wrappers of MPI_Info_get_valuelen.
 */
#define MPI_Info_get_valuelen_L mpi_info_get_valuelen

/** @def MPI_Info_set_L
    Exchanges MPI_Info_set_L by mpi_info_set.
    It is used for the Fortran wrappers of MPI_Info_set.
 */
#define MPI_Info_set_L mpi_info_set

/** @def MPI_Op_commutative_L
    Exchanges MPI_Op_commutative_L by mpi_op_commutative.
    It is used for the Fortran wrappers of MPI_Op_commutative.
 */
#define MPI_Op_commutative_L mpi_op_commutative

/** @def MPI_Op_create_L
    Exchanges MPI_Op_create_L by mpi_op_create.
    It is used for the Fortran wrappers of MPI_Op_create.
 */
#define MPI_Op_create_L mpi_op_create

/** @def MPI_Op_free_L
    Exchanges MPI_Op_free_L by mpi_op_free.
    It is used for the Fortran wrappers of MPI_Op_free.
 */
#define MPI_Op_free_L mpi_op_free

/** @def MPI_Request_get_status_L
    Exchanges MPI_Request_get_status_L by mpi_request_get_status.
    It is used for the Fortran wrappers of MPI_Request_get_status.
 */
#define MPI_Request_get_status_L mpi_request_get_status


/**
 * @name Fortran wrappers
 * @{
 */

#ifndef NEED_F2C_CONV

/* If MPI_Comm_f2c is HAVE as a simple macro (which typically does nothing)
 * as it would require a "real" function if it is really needed
 * => we can save the f2c and c2s conversions */

#if HAVE( DECL_PMPI_GET_VERSION ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_MISC ) && !defined( MPI_Get_version )
/**
 * Measurement wrapper for MPI_Get_version
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup misc
 */
void
FSUB( MPI_Get_version )( int* version, int* subversion, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Get_version( version, subversion );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_INFO_CREATE ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_MISC ) && !defined( MPI_Info_create )
/**
 * Measurement wrapper for MPI_Info_create
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup misc
 */
void
FSUB( MPI_Info_create )( MPI_Info* info, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Info_create( info );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_INFO_DELETE ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_MISC ) && !defined( MPI_Info_delete )
/**
 * Measurement wrapper for MPI_Info_delete
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup misc
 */
void
FSUB( MPI_Info_delete )( MPI_Info* info, char* key, int* ierr, scorep_fortran_charlen_t key_len )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    char* c_key = NULL;
    c_key = scorep_mpi_f2c_string( key, key_len );



    *ierr = MPI_Info_delete( *info, c_key );

    free( c_key );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_INFO_DUP ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_MISC ) && !defined( MPI_Info_dup )
/**
 * Measurement wrapper for MPI_Info_dup
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup misc
 */
void
FSUB( MPI_Info_dup )( MPI_Info* info, MPI_Info* newinfo, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Info_dup( *info, newinfo );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_INFO_FREE ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_MISC ) && !defined( MPI_Info_free )
/**
 * Measurement wrapper for MPI_Info_free
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup misc
 */
void
FSUB( MPI_Info_free )( MPI_Info* info, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Info_free( info );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_INFO_GET ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_MISC ) && !defined( MPI_Info_get )
/**
 * Measurement wrapper for MPI_Info_get
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup misc
 */
void
FSUB( MPI_Info_get )( MPI_Info* info, char* key, int* valuelen, char* value, int* flag, int* ierr, scorep_fortran_charlen_t key_len, scorep_fortran_charlen_t value_len )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    char*  c_key       = NULL;
    char*  c_value     = NULL;
    size_t c_value_len = 0;
    c_key = scorep_mpi_f2c_string( key, key_len );

    c_value = ( char* )malloc( ( value_len + 1 ) * sizeof( char ) );
    if ( !c_value )
    {
        exit( EXIT_FAILURE );
    }



    *ierr = MPI_Info_get( *info, c_key, *valuelen, c_value, flag );

    free( c_key );

    c_value_len = strlen( c_value );
    strncpy( value, c_value, c_value_len );
    memset( value + c_value_len, ' ', value_len - c_value_len );
    free( c_value );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_INFO_GET_NKEYS ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_MISC ) && !defined( MPI_Info_get_nkeys )
/**
 * Measurement wrapper for MPI_Info_get_nkeys
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup misc
 */
void
FSUB( MPI_Info_get_nkeys )( MPI_Info* info, int* nkeys, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Info_get_nkeys( *info, nkeys );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_INFO_GET_NTHKEY ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_MISC ) && !defined( MPI_Info_get_nthkey )
/**
 * Measurement wrapper for MPI_Info_get_nthkey
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup misc
 */
void
FSUB( MPI_Info_get_nthkey )( MPI_Info* info, int* n, char* key, int* ierr, scorep_fortran_charlen_t key_len )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    char*  c_key     = NULL;
    size_t c_key_len = 0;
    c_key = ( char* )malloc( ( key_len + 1 ) * sizeof( char ) );
    if ( !c_key )
    {
        exit( EXIT_FAILURE );
    }



    *ierr = MPI_Info_get_nthkey( *info, *n, c_key );


    c_key_len = strlen( c_key );
    strncpy( key, c_key, c_key_len );
    memset( key + c_key_len, ' ', key_len - c_key_len );
    free( c_key );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_INFO_GET_VALUELEN ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_MISC ) && !defined( MPI_Info_get_valuelen )
/**
 * Measurement wrapper for MPI_Info_get_valuelen
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup misc
 */
void
FSUB( MPI_Info_get_valuelen )( MPI_Info* info, char* key, int* valuelen, int* flag, int* ierr, scorep_fortran_charlen_t key_len )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    char* c_key = NULL;
    c_key = scorep_mpi_f2c_string( key, key_len );



    *ierr = MPI_Info_get_valuelen( *info, c_key, valuelen, flag );

    free( c_key );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_INFO_SET ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_MISC ) && !defined( MPI_Info_set )
/**
 * Measurement wrapper for MPI_Info_set
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup misc
 */
void
FSUB( MPI_Info_set )( MPI_Info* info, char* key, char* value, int* ierr, scorep_fortran_charlen_t key_len, scorep_fortran_charlen_t value_len )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    char* c_key   = NULL;
    char* c_value = NULL;
    c_key = scorep_mpi_f2c_string( key, key_len );

    c_value = scorep_mpi_f2c_string( value, value_len );



    *ierr = MPI_Info_set( *info, c_key, c_value );

    free( c_key );
    free( c_value );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_OP_COMMUTATIVE ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_MISC ) && !defined( MPI_Op_commutative )
/**
 * Measurement wrapper for MPI_Op_commutative
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup misc
 */
void
FSUB( MPI_Op_commutative )( MPI_Op* op, int* commute, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Op_commutative( *op, commute );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_OP_CREATE ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_MISC ) && !defined( MPI_Op_create )
/**
 * Measurement wrapper for MPI_Op_create
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup misc
 */
void
FSUB( MPI_Op_create )( MPI_User_function* function, int* commute, MPI_Op* op, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Op_create( function, *commute, op );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_OP_FREE ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_MISC ) && !defined( MPI_Op_free )
/**
 * Measurement wrapper for MPI_Op_free
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup misc
 */
void
FSUB( MPI_Op_free )( MPI_Op* op, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Op_free( op );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_REQUEST_GET_STATUS ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_MISC ) && !defined( MPI_Request_get_status )
/**
 * Measurement wrapper for MPI_Request_get_status
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup misc
 */
void
FSUB( MPI_Request_get_status )( MPI_Request* request, int* flag, MPI_Status* status, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    #if HAVE( MPI_STATUS_IGNORE )
    if ( status == scorep_mpi_fortran_status_ignore )
    {
        status = MPI_STATUS_IGNORE;
    }
    #endif


    *ierr = MPI_Request_get_status( *request, flag, status );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif

#else /* !NEED_F2C_CONV */

#if HAVE( DECL_PMPI_GET_VERSION ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_MISC ) && !defined( MPI_Get_version )
/**
 * Measurement wrapper for MPI_Get_version
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup misc
 * For the order of events see @ref MPI_Get_version
 */
void
FSUB( MPI_Get_version )( MPI_Fint* version, MPI_Fint* subversion, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Get_version( version, subversion );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_INFO_CREATE ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_MISC ) && !defined( MPI_Info_create )
/**
 * Measurement wrapper for MPI_Info_create
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup misc
 * For the order of events see @ref MPI_Info_create
 */
void
FSUB( MPI_Info_create )( MPI_Fint* info, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Info c_info;


    *ierr = MPI_Info_create( &c_info );

    *info = PMPI_Info_c2f( c_info );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_INFO_DELETE ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_MISC ) && !defined( MPI_Info_delete )
/**
 * Measurement wrapper for MPI_Info_delete
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup misc
 * For the order of events see @ref MPI_Info_delete
 */
void
FSUB( MPI_Info_delete )( MPI_Fint* info, char* key, MPI_Fint* ierr, scorep_fortran_charlen_t key_len )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Info c_info = PMPI_Info_f2c( *info );
    char*    c_key  = NULL;
    c_key = scorep_mpi_f2c_string( key, key_len );



    *ierr = MPI_Info_delete( c_info, c_key );

    *info = PMPI_Info_c2f( c_info );
    free( c_key );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_INFO_DUP ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_MISC ) && !defined( MPI_Info_dup )
/**
 * Measurement wrapper for MPI_Info_dup
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup misc
 * For the order of events see @ref MPI_Info_dup
 */
void
FSUB( MPI_Info_dup )( MPI_Fint* info, MPI_Fint* newinfo, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Info c_newinfo;


    *ierr = MPI_Info_dup( PMPI_Info_f2c( *info ), &c_newinfo );

    *newinfo = PMPI_Info_c2f( c_newinfo );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_INFO_FREE ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_MISC ) && !defined( MPI_Info_free )
/**
 * Measurement wrapper for MPI_Info_free
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup misc
 * For the order of events see @ref MPI_Info_free
 */
void
FSUB( MPI_Info_free )( MPI_Fint* info, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Info c_info = PMPI_Info_f2c( *info );


    *ierr = MPI_Info_free( &c_info );

    *info = PMPI_Info_c2f( c_info );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_INFO_GET ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_MISC ) && !defined( MPI_Info_get )
/**
 * Measurement wrapper for MPI_Info_get
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup misc
 * For the order of events see @ref MPI_Info_get
 */
void
FSUB( MPI_Info_get )( MPI_Fint* info, char* key, MPI_Fint* valuelen, char* value, MPI_Fint* flag, MPI_Fint* ierr, scorep_fortran_charlen_t key_len, scorep_fortran_charlen_t value_len )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    char*  c_key       = NULL;
    char*  c_value     = NULL;
    size_t c_value_len = 0;
    c_key = scorep_mpi_f2c_string( key, key_len );

    c_value = ( char* )malloc( ( value_len + 1 ) * sizeof( char ) );
    if ( !c_value )
    {
        exit( EXIT_FAILURE );
    }



    *ierr = MPI_Info_get( PMPI_Info_f2c( *info ), c_key, *valuelen, c_value, flag );

    free( c_key );

    c_value_len = strlen( c_value );
    strncpy( value, c_value, c_value_len );
    memset( value + c_value_len, ' ', value_len - c_value_len );
    free( c_value );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_INFO_GET_NKEYS ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_MISC ) && !defined( MPI_Info_get_nkeys )
/**
 * Measurement wrapper for MPI_Info_get_nkeys
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup misc
 * For the order of events see @ref MPI_Info_get_nkeys
 */
void
FSUB( MPI_Info_get_nkeys )( MPI_Fint* info, MPI_Fint* nkeys, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Info_get_nkeys( PMPI_Info_f2c( *info ), nkeys );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_INFO_GET_NTHKEY ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_MISC ) && !defined( MPI_Info_get_nthkey )
/**
 * Measurement wrapper for MPI_Info_get_nthkey
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup misc
 * For the order of events see @ref MPI_Info_get_nthkey
 */
void
FSUB( MPI_Info_get_nthkey )( MPI_Fint* info, MPI_Fint* n, char* key, MPI_Fint* ierr, scorep_fortran_charlen_t key_len )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    char*  c_key     = NULL;
    size_t c_key_len = 0;
    c_key = ( char* )malloc( ( key_len + 1 ) * sizeof( char ) );
    if ( !c_key )
    {
        exit( EXIT_FAILURE );
    }



    *ierr = MPI_Info_get_nthkey( PMPI_Info_f2c( *info ), *n, c_key );


    c_key_len = strlen( c_key );
    strncpy( key, c_key, c_key_len );
    memset( key + c_key_len, ' ', key_len - c_key_len );
    free( c_key );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_INFO_GET_VALUELEN ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_MISC ) && !defined( MPI_Info_get_valuelen )
/**
 * Measurement wrapper for MPI_Info_get_valuelen
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup misc
 * For the order of events see @ref MPI_Info_get_valuelen
 */
void
FSUB( MPI_Info_get_valuelen )( MPI_Fint* info, char* key, MPI_Fint* valuelen, MPI_Fint* flag, MPI_Fint* ierr, scorep_fortran_charlen_t key_len )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    char* c_key = NULL;
    c_key = scorep_mpi_f2c_string( key, key_len );



    *ierr = MPI_Info_get_valuelen( PMPI_Info_f2c( *info ), c_key, valuelen, flag );

    free( c_key );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_INFO_SET ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_MISC ) && !defined( MPI_Info_set )
/**
 * Measurement wrapper for MPI_Info_set
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup misc
 * For the order of events see @ref MPI_Info_set
 */
void
FSUB( MPI_Info_set )( MPI_Fint* info, char* key, char* value, MPI_Fint* ierr, scorep_fortran_charlen_t key_len, scorep_fortran_charlen_t value_len )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Info c_info  = PMPI_Info_f2c( *info );
    char*    c_key   = NULL;
    char*    c_value = NULL;
    c_key = scorep_mpi_f2c_string( key, key_len );

    c_value = scorep_mpi_f2c_string( value, value_len );



    *ierr = MPI_Info_set( c_info, c_key, c_value );

    *info = PMPI_Info_c2f( c_info );
    free( c_key );
    free( c_value );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_OP_COMMUTATIVE ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_MISC ) && !defined( MPI_Op_commutative )
/**
 * Measurement wrapper for MPI_Op_commutative
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup misc
 * For the order of events see @ref MPI_Op_commutative
 */
void
FSUB( MPI_Op_commutative )( MPI_Fint* op, MPI_Fint* commute, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Op_commutative( PMPI_Op_f2c( *op ), commute );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_OP_CREATE ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_MISC ) && !defined( MPI_Op_create )
/**
 * Measurement wrapper for MPI_Op_create
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup misc
 * For the order of events see @ref MPI_Op_create
 */
void
FSUB( MPI_Op_create )( void* function, MPI_Fint* commute, MPI_Fint* op, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Op c_op;


    *ierr = MPI_Op_create( ( MPI_User_function* )function, *commute, &c_op );

    *op = PMPI_Op_c2f( c_op );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_OP_FREE ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_MISC ) && !defined( MPI_Op_free )
/**
 * Measurement wrapper for MPI_Op_free
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup misc
 * For the order of events see @ref MPI_Op_free
 */
void
FSUB( MPI_Op_free )( MPI_Fint* op, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Op c_op = PMPI_Op_f2c( *op );


    *ierr = MPI_Op_free( &c_op );

    *op = PMPI_Op_c2f( c_op );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_REQUEST_GET_STATUS ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_MISC ) && !defined( MPI_Request_get_status )
/**
 * Measurement wrapper for MPI_Request_get_status
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup misc
 * For the order of events see @ref MPI_Request_get_status
 */
void
FSUB( MPI_Request_get_status )( MPI_Fint* request, MPI_Fint* flag, MPI_Fint* status, MPI_Fint* ierr )
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


    *ierr = MPI_Request_get_status( PMPI_Request_f2c( *request ), flag, c_status_ptr );

    #if HAVE( MPI_STATUS_IGNORE )
    if ( c_status_ptr != MPI_STATUS_IGNORE )
#endif
    {
        PMPI_Status_c2f( c_status_ptr, status );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif

#endif

/**
 * @}
 */
