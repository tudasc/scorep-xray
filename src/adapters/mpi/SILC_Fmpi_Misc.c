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

/**
 * @file  SILC_Fmpi_Misc.c
 *
 * @brief Fortran interface wrappers for miscelaneous and handler conversion
 *        functions
 */

/* uppercase defines */
/** @def MPI_Address_U
    Exchange MPI_Address_U by MPI_ADDRESS.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Address_U MPI_ADDRESS

/** @def MPI_Alloc_mem_U
    Exchange MPI_Alloc_mem_U by MPI_ALLOC_MEM.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Alloc_mem_U MPI_ALLOC_MEM

/** @def MPI_Free_mem_U
    Exchange MPI_Free_mem_U by MPI_FREE_MEM.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Free_mem_U MPI_FREE_MEM

/** @def MPI_Get_address_U
    Exchange MPI_Get_address_U by MPI_GET_ADDRESS.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Get_address_U MPI_GET_ADDRESS

/** @def MPI_Get_version_U
    Exchange MPI_Get_version_U by MPI_GET_VERSION.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Get_version_U MPI_GET_VERSION

/** @def MPI_Info_create_U
    Exchange MPI_Info_create_U by MPI_INFO_CREATE.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Info_create_U MPI_INFO_CREATE

/** @def MPI_Info_delete_U
    Exchange MPI_Info_delete_U by MPI_INFO_DELETE.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Info_delete_U MPI_INFO_DELETE

/** @def MPI_Info_dup_U
    Exchange MPI_Info_dup_U by MPI_INFO_DUP.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Info_dup_U MPI_INFO_DUP

/** @def MPI_Info_free_U
    Exchange MPI_Info_free_U by MPI_INFO_FREE.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Info_free_U MPI_INFO_FREE

/** @def MPI_Info_get_U
    Exchange MPI_Info_get_U by MPI_INFO_GET.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Info_get_U MPI_INFO_GET

/** @def MPI_Info_get_nkeys_U
    Exchange MPI_Info_get_nkeys_U by MPI_INFO_GET_NKEYS.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Info_get_nkeys_U MPI_INFO_GET_NKEYS

/** @def MPI_Info_get_nthkey_U
    Exchange MPI_Info_get_nthkey_U by MPI_INFO_GET_NTHKEY.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Info_get_nthkey_U MPI_INFO_GET_NTHKEY

/** @def MPI_Info_get_valuelen_U
    Exchange MPI_Info_get_valuelen_U by MPI_INFO_GET_VALUELEN.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Info_get_valuelen_U MPI_INFO_GET_VALUELEN

/** @def MPI_Info_set_U
    Exchange MPI_Info_set_U by MPI_INFO_SET.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Info_set_U MPI_INFO_SET

/** @def MPI_Op_create_U
    Exchange MPI_Op_create_U by MPI_OP_CREATE.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Op_create_U MPI_OP_CREATE

/** @def MPI_Op_free_U
    Exchange MPI_Op_free_U by MPI_OP_FREE.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Op_free_U MPI_OP_FREE

/** @def MPI_Request_get_status_U
    Exchange MPI_Request_get_status_U by MPI_REQUEST_GET_STATUS.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Request_get_status_U MPI_REQUEST_GET_STATUS


/* lowercase defines */
/** @def MPI_Address_L
    Exchanges MPI_Address_L by mpi_address.
    It is used for the Forran wrappers of me).
 */
#define MPI_Address_L mpi_address

/** @def MPI_Alloc_mem_L
    Exchanges MPI_Alloc_mem_L by mpi_alloc_mem.
    It is used for the Forran wrappers of me).
 */
#define MPI_Alloc_mem_L mpi_alloc_mem

/** @def MPI_Free_mem_L
    Exchanges MPI_Free_mem_L by mpi_free_mem.
    It is used for the Forran wrappers of me).
 */
#define MPI_Free_mem_L mpi_free_mem

/** @def MPI_Get_address_L
    Exchanges MPI_Get_address_L by mpi_get_address.
    It is used for the Forran wrappers of me).
 */
#define MPI_Get_address_L mpi_get_address

/** @def MPI_Get_version_L
    Exchanges MPI_Get_version_L by mpi_get_version.
    It is used for the Forran wrappers of me).
 */
#define MPI_Get_version_L mpi_get_version

/** @def MPI_Info_create_L
    Exchanges MPI_Info_create_L by mpi_info_create.
    It is used for the Forran wrappers of me).
 */
#define MPI_Info_create_L mpi_info_create

/** @def MPI_Info_delete_L
    Exchanges MPI_Info_delete_L by mpi_info_delete.
    It is used for the Forran wrappers of me).
 */
#define MPI_Info_delete_L mpi_info_delete

/** @def MPI_Info_dup_L
    Exchanges MPI_Info_dup_L by mpi_info_dup.
    It is used for the Forran wrappers of me).
 */
#define MPI_Info_dup_L mpi_info_dup

/** @def MPI_Info_free_L
    Exchanges MPI_Info_free_L by mpi_info_free.
    It is used for the Forran wrappers of me).
 */
#define MPI_Info_free_L mpi_info_free

/** @def MPI_Info_get_L
    Exchanges MPI_Info_get_L by mpi_info_get.
    It is used for the Forran wrappers of me).
 */
#define MPI_Info_get_L mpi_info_get

/** @def MPI_Info_get_nkeys_L
    Exchanges MPI_Info_get_nkeys_L by mpi_info_get_nkeys.
    It is used for the Forran wrappers of me).
 */
#define MPI_Info_get_nkeys_L mpi_info_get_nkeys

/** @def MPI_Info_get_nthkey_L
    Exchanges MPI_Info_get_nthkey_L by mpi_info_get_nthkey.
    It is used for the Forran wrappers of me).
 */
#define MPI_Info_get_nthkey_L mpi_info_get_nthkey

/** @def MPI_Info_get_valuelen_L
    Exchanges MPI_Info_get_valuelen_L by mpi_info_get_valuelen.
    It is used for the Forran wrappers of me).
 */
#define MPI_Info_get_valuelen_L mpi_info_get_valuelen

/** @def MPI_Info_set_L
    Exchanges MPI_Info_set_L by mpi_info_set.
    It is used for the Forran wrappers of me).
 */
#define MPI_Info_set_L mpi_info_set

/** @def MPI_Op_create_L
    Exchanges MPI_Op_create_L by mpi_op_create.
    It is used for the Forran wrappers of me).
 */
#define MPI_Op_create_L mpi_op_create

/** @def MPI_Op_free_L
    Exchanges MPI_Op_free_L by mpi_op_free.
    It is used for the Forran wrappers of me).
 */
#define MPI_Op_free_L mpi_op_free

/** @def MPI_Request_get_status_L
    Exchanges MPI_Request_get_status_L by mpi_request_get_status.
    It is used for the Forran wrappers of me).
 */
#define MPI_Request_get_status_L mpi_request_get_status


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
 * Measurement wrapper for MPI_Address
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup misc
 */
void
FSUB( MPI_Address ) ( void* location, MPI_Aint * address, int* ierr )
{
    *ierr = MPI_Address( location, address );
}
#endif
#if defined( HAS_MPI_EXTRA )
/**
 * Measurement wrapper for MPI_Alloc_mem
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup misc
 */
void
FSUB( MPI_Alloc_mem ) ( MPI_Aint * size, MPI_Info * info, void* baseptr, int* ierr )
{
    *ierr = MPI_Alloc_mem( *size, *info, baseptr );
}
#endif
#if defined( HAS_MPI_EXTRA )
/**
 * Measurement wrapper for MPI_Free_mem
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup misc
 */
void
FSUB( MPI_Free_mem ) ( void* base, int* ierr )
{
    *ierr = MPI_Free_mem( base );
}
#endif
#if defined( HAS_MPI_EXTRA )
/**
 * Measurement wrapper for MPI_Get_address
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup misc
 */
void
FSUB( MPI_Get_address ) ( void* location, MPI_Aint * address, int* ierr )
{
    *ierr = MPI_Get_address( location, address );
}
#endif
#if defined( HAS_MPI_EXTRA )
/**
 * Measurement wrapper for MPI_Get_version
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup misc
 */
void
FSUB( MPI_Get_version ) ( int* version, int* subversion, int* ierr )
{
    *ierr = MPI_Get_version( version, subversion );
}
#endif
#if defined( HAS_MPI_EXTRA )
/**
 * Measurement wrapper for MPI_Info_create
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup misc
 */
void
FSUB( MPI_Info_create ) ( MPI_Info * info, int* ierr )
{
    *ierr = MPI_Info_create( info );
}
#endif
#if defined( HAS_MPI_EXTRA )
/**
 * Measurement wrapper for MPI_Info_delete
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup misc
 */
void
FSUB( MPI_Info_delete ) ( MPI_Info * info, char* key, int* ierr, int key_len )
{
    char* c_key = NULL;
    c_key = ( char* )malloc( ( key_len + 1 ) * sizeof( char ) );
    if ( !c_key )
    {
        exit( EXIT_FAILURE );
    }
    strncpy( c_key, key, key_len );
    c_key[ key_len ] = '\0';


    *ierr = MPI_Info_delete( *info, c_key );

    free( c_key );
}
#endif
#if defined( HAS_MPI_EXTRA )
/**
 * Measurement wrapper for MPI_Info_dup
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup misc
 */
void
FSUB( MPI_Info_dup ) ( MPI_Info * info, MPI_Info * newinfo, int* ierr )
{
    *ierr = MPI_Info_dup( *info, newinfo );
}
#endif
#if defined( HAS_MPI_EXTRA )
/**
 * Measurement wrapper for MPI_Info_free
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup misc
 */
void
FSUB( MPI_Info_free ) ( MPI_Info * info, int* ierr )
{
    *ierr = MPI_Info_free( info );
}
#endif
#if defined( HAS_MPI_EXTRA )
/**
 * Measurement wrapper for MPI_Info_get
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup misc
 */
void
FSUB( MPI_Info_get ) ( MPI_Info * info, char* key, int* valuelen, char* value, int* flag, int* ierr, int key_len, int value_len )
{
    char* c_key   = NULL;
    char* c_value = NULL;
    c_key = ( char* )malloc( ( key_len + 1 ) * sizeof( char ) );
    if ( !c_key )
    {
        exit( EXIT_FAILURE );
    }
    strncpy( c_key, key, key_len );
    c_key[ key_len ] = '\0';

    c_value = ( char* )malloc( ( value_len + 1 ) * sizeof( char ) );
    if ( !c_value )
    {
        exit( EXIT_FAILURE );
    }
    strncpy( c_value, value, value_len );
    c_value[ value_len ] = '\0';


    *ierr = MPI_Info_get( *info, c_key, *valuelen, c_value, flag );

    free( c_key );
    free( c_value );
}
#endif
#if defined( HAS_MPI_EXTRA )
/**
 * Measurement wrapper for MPI_Info_get_nkeys
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup misc
 */
void
FSUB( MPI_Info_get_nkeys ) ( MPI_Info * info, int* nkeys, int* ierr )
{
    *ierr = MPI_Info_get_nkeys( *info, nkeys );
}
#endif
#if defined( HAS_MPI_EXTRA )
/**
 * Measurement wrapper for MPI_Info_get_nthkey
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup misc
 */
void
FSUB( MPI_Info_get_nthkey ) ( MPI_Info * info, int* n, char* key, int* ierr, int key_len )
{
    char* c_key = NULL;
    c_key = ( char* )malloc( ( key_len + 1 ) * sizeof( char ) );
    if ( !c_key )
    {
        exit( EXIT_FAILURE );
    }
    strncpy( c_key, key, key_len );
    c_key[ key_len ] = '\0';


    *ierr = MPI_Info_get_nthkey( *info, *n, c_key );

    free( c_key );
}
#endif
#if defined( HAS_MPI_EXTRA )
/**
 * Measurement wrapper for MPI_Info_get_valuelen
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup misc
 */
void
FSUB( MPI_Info_get_valuelen ) ( MPI_Info * info, char* key, int* valuelen, int* flag, int* ierr, int key_len )
{
    char* c_key = NULL;
    c_key = ( char* )malloc( ( key_len + 1 ) * sizeof( char ) );
    if ( !c_key )
    {
        exit( EXIT_FAILURE );
    }
    strncpy( c_key, key, key_len );
    c_key[ key_len ] = '\0';


    *ierr = MPI_Info_get_valuelen( *info, c_key, valuelen, flag );

    free( c_key );
}
#endif
#if defined( HAS_MPI_EXTRA )
/**
 * Measurement wrapper for MPI_Info_set
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup misc
 */
void
FSUB( MPI_Info_set ) ( MPI_Info * info, char* key, char* value, int* ierr, int key_len, int value_len )
{
    char* c_key   = NULL;
    char* c_value = NULL;
    c_key = ( char* )malloc( ( key_len + 1 ) * sizeof( char ) );
    if ( !c_key )
    {
        exit( EXIT_FAILURE );
    }
    strncpy( c_key, key, key_len );
    c_key[ key_len ] = '\0';

    c_value = ( char* )malloc( ( value_len + 1 ) * sizeof( char ) );
    if ( !c_value )
    {
        exit( EXIT_FAILURE );
    }
    strncpy( c_value, value, value_len );
    c_value[ value_len ] = '\0';


    *ierr = MPI_Info_set( *info, c_key, c_value );

    free( c_key );
    free( c_value );
}
#endif
#if defined( HAS_MPI_EXTRA )
/**
 * Measurement wrapper for MPI_Op_create
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup misc
 */
void
FSUB( MPI_Op_create ) ( MPI_User_function * function, int* commute, MPI_Op * op, int* ierr )
{
    *ierr = MPI_Op_create( function, *commute, op );
}
#endif
#if defined( HAS_MPI_EXTRA )
/**
 * Measurement wrapper for MPI_Op_free
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup misc
 */
void
FSUB( MPI_Op_free ) ( MPI_Op * op, int* ierr )
{
    *ierr = MPI_Op_free( op );
}
#endif
#if defined( HAS_MPI_EXTRA )
/**
 * Measurement wrapper for MPI_Request_get_status
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup misc
 */
void
FSUB( MPI_Request_get_status ) ( MPI_Request * request, int* flag, MPI_Status * status, int* ierr )
{
    *ierr = MPI_Request_get_status( *request, flag, status );
}
#endif

#else /* !NEED_F2C_CONV */

#if defined( HAS_MPI_EXTRA )
/**
 * Measurement wrapper for MPI_Address
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup misc
 */
void
FSUB( MPI_Address ) ( void* location, MPI_Aint * address, int* ierr )
{
    *ierr = MPI_Address( location, address );
}
#endif
#if defined( HAS_MPI_EXTRA )
/**
 * Measurement wrapper for MPI_Alloc_mem
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup misc
 */
void
FSUB( MPI_Alloc_mem ) ( MPI_Aint * size, MPI_Fint * info, void* baseptr, int* ierr )
{
    *ierr = MPI_Alloc_mem( *size, PMPI_Info_f2c( *info ), baseptr );
}
#endif
#if defined( HAS_MPI_EXTRA )
/**
 * Measurement wrapper for MPI_Free_mem
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup misc
 */
void
FSUB( MPI_Free_mem ) ( void* base, int* ierr )
{
    *ierr = MPI_Free_mem( base );
}
#endif
#if defined( HAS_MPI_EXTRA )
/**
 * Measurement wrapper for MPI_Get_address
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup misc
 */
void
FSUB( MPI_Get_address ) ( void* location, MPI_Aint * address, int* ierr )
{
    *ierr = MPI_Get_address( location, address );
}
#endif
#if defined( HAS_MPI_EXTRA )
/**
 * Measurement wrapper for MPI_Get_version
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup misc
 */
void
FSUB( MPI_Get_version ) ( MPI_Fint * version, MPI_Fint * subversion, int* ierr )
{
    *ierr = MPI_Get_version( version, subversion );
}
#endif
#if defined( HAS_MPI_EXTRA )
/**
 * Measurement wrapper for MPI_Info_create
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup misc
 */
void
FSUB( MPI_Info_create ) ( MPI_Fint * info, int* ierr )
{
    MPI_Info c_info;
    *ierr = MPI_Info_create( &c_info );
    *info = PMPI_Info_c2f( c_info );
}
#endif
#if defined( HAS_MPI_EXTRA )
/**
 * Measurement wrapper for MPI_Info_delete
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup misc
 */
void
FSUB( MPI_Info_delete ) ( MPI_Fint * info, char* key, int* ierr, int key_len )
{
    char*    c_key  = NULL;
    MPI_Info c_info = PMPI_Info_f2c( *info );
    c_key = ( char* )malloc( ( key_len + 1 ) * sizeof( char ) );
    if ( !c_key )
    {
        exit( EXIT_FAILURE );
    }
    strncpy( c_key, key, key_len );
    c_key[ key_len ] = '\0';

    *ierr = MPI_Info_delete( c_info, key );
    free( c_key );
    *info = PMPI_Info_c2f( c_info );
}
#endif
#if defined( HAS_MPI_EXTRA )
/**
 * Measurement wrapper for MPI_Info_dup
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup misc
 */
void
FSUB( MPI_Info_dup ) ( MPI_Fint * info, MPI_Fint * newinfo, int* ierr )
{
    MPI_Info c_newinfo;
    *ierr    = MPI_Info_dup( PMPI_Info_f2c( *info ), &c_newinfo );
    *newinfo = PMPI_Info_c2f( c_newinfo );
}
#endif
#if defined( HAS_MPI_EXTRA )
/**
 * Measurement wrapper for MPI_Info_free
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup misc
 */
void
FSUB( MPI_Info_free ) ( MPI_Fint * info, int* ierr )
{
    MPI_Info c_info = PMPI_Info_f2c( *info );
    *ierr = MPI_Info_free( &c_info );
    *info = PMPI_Info_c2f( c_info );
}
#endif
#if defined( HAS_MPI_EXTRA )
/**
 * Measurement wrapper for MPI_Info_get
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup misc
 */
void
FSUB( MPI_Info_get ) ( MPI_Fint * info, char* key, MPI_Fint * valuelen, char* value, MPI_Fint * flag, int* ierr, int key_len, int value_len )
{
    char* c_key   = NULL;
    char* c_value = NULL;
    c_key = ( char* )malloc( ( key_len + 1 ) * sizeof( char ) );
    if ( !c_key )
    {
        exit( EXIT_FAILURE );
    }
    strncpy( c_key, key, key_len );
    c_key[ key_len ] = '\0';

    c_value = ( char* )malloc( ( value_len + 1 ) * sizeof( char ) );
    if ( !c_value )
    {
        exit( EXIT_FAILURE );
    }
    strncpy( c_value, value, value_len );
    c_value[ value_len ] = '\0';

    *ierr = MPI_Info_get( PMPI_Info_f2c( *info ), key, *valuelen, value, flag );
    free( c_key );
    free( c_value );
}
#endif
#if defined( HAS_MPI_EXTRA )
/**
 * Measurement wrapper for MPI_Info_get_nkeys
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup misc
 */
void
FSUB( MPI_Info_get_nkeys ) ( MPI_Fint * info, MPI_Fint * nkeys, int* ierr )
{
    *ierr = MPI_Info_get_nkeys( PMPI_Info_f2c( *info ), nkeys );
}
#endif
#if defined( HAS_MPI_EXTRA )
/**
 * Measurement wrapper for MPI_Info_get_nthkey
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup misc
 */
void
FSUB( MPI_Info_get_nthkey ) ( MPI_Fint * info, MPI_Fint * n, char* key, int* ierr, int key_len )
{
    char* c_key = NULL;
    c_key = ( char* )malloc( ( key_len + 1 ) * sizeof( char ) );
    if ( !c_key )
    {
        exit( EXIT_FAILURE );
    }
    strncpy( c_key, key, key_len );
    c_key[ key_len ] = '\0';

    *ierr = MPI_Info_get_nthkey( PMPI_Info_f2c( *info ), *n, key );
    free( c_key );
}
#endif
#if defined( HAS_MPI_EXTRA )
/**
 * Measurement wrapper for MPI_Info_get_valuelen
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup misc
 */
void
FSUB( MPI_Info_get_valuelen ) ( MPI_Fint * info, char* key, MPI_Fint * valuelen, MPI_Fint * flag, int* ierr, int key_len )
{
    char* c_key = NULL;
    c_key = ( char* )malloc( ( key_len + 1 ) * sizeof( char ) );
    if ( !c_key )
    {
        exit( EXIT_FAILURE );
    }
    strncpy( c_key, key, key_len );
    c_key[ key_len ] = '\0';

    *ierr = MPI_Info_get_valuelen( PMPI_Info_f2c( *info ), key, valuelen, flag );
    free( c_key );
}
#endif
#if defined( HAS_MPI_EXTRA )
/**
 * Measurement wrapper for MPI_Info_set
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup misc
 */
void
FSUB( MPI_Info_set ) ( MPI_Fint * info, char* key, char* value, int* ierr, int key_len, int value_len )
{
    char*    c_key   = NULL;
    char*    c_value = NULL;
    MPI_Info c_info  = PMPI_Info_f2c( *info );
    c_key = ( char* )malloc( ( key_len + 1 ) * sizeof( char ) );
    if ( !c_key )
    {
        exit( EXIT_FAILURE );
    }
    strncpy( c_key, key, key_len );
    c_key[ key_len ] = '\0';

    c_value = ( char* )malloc( ( value_len + 1 ) * sizeof( char ) );
    if ( !c_value )
    {
        exit( EXIT_FAILURE );
    }
    strncpy( c_value, value, value_len );
    c_value[ value_len ] = '\0';

    *ierr = MPI_Info_set( c_info, key, value );
    free( c_key );
    free( c_value );
    *info = PMPI_Info_c2f( c_info );
}
#endif
#if defined( HAS_MPI_EXTRA )
/**
 * Measurement wrapper for MPI_Op_create
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup misc
 */
void
FSUB( MPI_Op_create ) ( void* function, MPI_Fint * commute, MPI_Fint * op, int* ierr )
{
    MPI_Op c_op;
    *ierr = MPI_Op_create( ( MPI_User_function* )function, *commute, &c_op );
    *op   = PMPI_Op_c2f( c_op );
}
#endif
#if defined( HAS_MPI_EXTRA )
/**
 * Measurement wrapper for MPI_Op_free
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup misc
 */
void
FSUB( MPI_Op_free ) ( MPI_Fint * op, int* ierr )
{
    MPI_Op c_op = PMPI_Op_f2c( *op );
    *ierr = MPI_Op_free( &c_op );
    *op   = PMPI_Op_c2f( c_op );
}
#endif
#if defined( HAS_MPI_EXTRA )
/**
 * Measurement wrapper for MPI_Request_get_status
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup misc
 */
void
FSUB( MPI_Request_get_status ) ( MPI_Fint * request, MPI_Fint * flag, MPI_Fint * status, int* ierr )
{
    MPI_Status c_status;
    *ierr = MPI_Request_get_status( PMPI_Request_f2c( *request ), flag, &c_status );
    MPI_Status_c2f( &c_status, status );
}
#endif

#endif

/**
 * @}
 */
