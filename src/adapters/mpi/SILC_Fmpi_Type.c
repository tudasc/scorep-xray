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
#include "config.h"

/**
 * @file SILC_Fmpi_Type.c
 *
 * @brief Fortran interface wrappers for datatype functions
 */

/* uppercase defines */
/** @def MPI_Sizeof_U
    Exchange MPI_Sizeof_U by MPI_SIZEOF.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Sizeof_U MPI_SIZEOF


/** @def MPI_Pack_U
    Exchange MPI_Pack_U by MPI_PACK.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Pack_U MPI_PACK

/** @def MPI_Pack_external_U
    Exchange MPI_Pack_external_U by MPI_PACK_EXTERNAL.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Pack_external_U MPI_PACK_EXTERNAL

/** @def MPI_Pack_external_size_U
    Exchange MPI_Pack_external_size_U by MPI_PACK_EXTERNAL_SIZE.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Pack_external_size_U MPI_PACK_EXTERNAL_SIZE

/** @def MPI_Pack_size_U
    Exchange MPI_Pack_size_U by MPI_PACK_SIZE.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Pack_size_U MPI_PACK_SIZE

/** @def MPI_Type_commit_U
    Exchange MPI_Type_commit_U by MPI_TYPE_COMMIT.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Type_commit_U MPI_TYPE_COMMIT

/** @def MPI_Type_contiguous_U
    Exchange MPI_Type_contiguous_U by MPI_TYPE_CONTIGUOUS.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Type_contiguous_U MPI_TYPE_CONTIGUOUS

/** @def MPI_Type_create_darray_U
    Exchange MPI_Type_create_darray_U by MPI_TYPE_CREATE_DARRAY.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Type_create_darray_U MPI_TYPE_CREATE_DARRAY

/** @def MPI_Type_create_f90_complex_U
    Exchange MPI_Type_create_f90_complex_U by MPI_TYPE_CREATE_F90_COMPLEX.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Type_create_f90_complex_U MPI_TYPE_CREATE_F90_COMPLEX

/** @def MPI_Type_create_f90_integer_U
    Exchange MPI_Type_create_f90_integer_U by MPI_TYPE_CREATE_F90_INTEGER.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Type_create_f90_integer_U MPI_TYPE_CREATE_F90_INTEGER

/** @def MPI_Type_create_f90_real_U
    Exchange MPI_Type_create_f90_real_U by MPI_TYPE_CREATE_F90_REAL.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Type_create_f90_real_U MPI_TYPE_CREATE_F90_REAL

/** @def MPI_Type_create_hindexed_U
    Exchange MPI_Type_create_hindexed_U by MPI_TYPE_CREATE_HINDEXED.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Type_create_hindexed_U MPI_TYPE_CREATE_HINDEXED

/** @def MPI_Type_create_hvector_U
    Exchange MPI_Type_create_hvector_U by MPI_TYPE_CREATE_HVECTOR.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Type_create_hvector_U MPI_TYPE_CREATE_HVECTOR

/** @def MPI_Type_create_indexed_block_U
    Exchange MPI_Type_create_indexed_block_U by MPI_TYPE_CREATE_INDEXED_BLOCK.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Type_create_indexed_block_U MPI_TYPE_CREATE_INDEXED_BLOCK

/** @def MPI_Type_create_resized_U
    Exchange MPI_Type_create_resized_U by MPI_TYPE_CREATE_RESIZED.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Type_create_resized_U MPI_TYPE_CREATE_RESIZED

/** @def MPI_Type_create_struct_U
    Exchange MPI_Type_create_struct_U by MPI_TYPE_CREATE_STRUCT.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Type_create_struct_U MPI_TYPE_CREATE_STRUCT

/** @def MPI_Type_create_subarray_U
    Exchange MPI_Type_create_subarray_U by MPI_TYPE_CREATE_SUBARRAY.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Type_create_subarray_U MPI_TYPE_CREATE_SUBARRAY

/** @def MPI_Type_dup_U
    Exchange MPI_Type_dup_U by MPI_TYPE_DUP.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Type_dup_U MPI_TYPE_DUP

/** @def MPI_Type_extent_U
    Exchange MPI_Type_extent_U by MPI_TYPE_EXTENT.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Type_extent_U MPI_TYPE_EXTENT

/** @def MPI_Type_free_U
    Exchange MPI_Type_free_U by MPI_TYPE_FREE.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Type_free_U MPI_TYPE_FREE

/** @def MPI_Type_get_contents_U
    Exchange MPI_Type_get_contents_U by MPI_TYPE_GET_CONTENTS.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Type_get_contents_U MPI_TYPE_GET_CONTENTS

/** @def MPI_Type_get_envelope_U
    Exchange MPI_Type_get_envelope_U by MPI_TYPE_GET_ENVELOPE.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Type_get_envelope_U MPI_TYPE_GET_ENVELOPE

/** @def MPI_Type_get_extent_U
    Exchange MPI_Type_get_extent_U by MPI_TYPE_GET_EXTENT.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Type_get_extent_U MPI_TYPE_GET_EXTENT

/** @def MPI_Type_get_true_extent_U
    Exchange MPI_Type_get_true_extent_U by MPI_TYPE_GET_TRUE_EXTENT.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Type_get_true_extent_U MPI_TYPE_GET_TRUE_EXTENT

/** @def MPI_Type_hindexed_U
    Exchange MPI_Type_hindexed_U by MPI_TYPE_HINDEXED.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Type_hindexed_U MPI_TYPE_HINDEXED

/** @def MPI_Type_hvector_U
    Exchange MPI_Type_hvector_U by MPI_TYPE_HVECTOR.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Type_hvector_U MPI_TYPE_HVECTOR

/** @def MPI_Type_indexed_U
    Exchange MPI_Type_indexed_U by MPI_TYPE_INDEXED.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Type_indexed_U MPI_TYPE_INDEXED

/** @def MPI_Type_lb_U
    Exchange MPI_Type_lb_U by MPI_TYPE_LB.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Type_lb_U MPI_TYPE_LB

/** @def MPI_Type_match_size_U
    Exchange MPI_Type_match_size_U by MPI_TYPE_MATCH_SIZE.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Type_match_size_U MPI_TYPE_MATCH_SIZE

/** @def MPI_Type_size_U
    Exchange MPI_Type_size_U by MPI_TYPE_SIZE.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Type_size_U MPI_TYPE_SIZE

/** @def MPI_Type_struct_U
    Exchange MPI_Type_struct_U by MPI_TYPE_STRUCT.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Type_struct_U MPI_TYPE_STRUCT

/** @def MPI_Type_ub_U
    Exchange MPI_Type_ub_U by MPI_TYPE_UB.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Type_ub_U MPI_TYPE_UB

/** @def MPI_Type_vector_U
    Exchange MPI_Type_vector_U by MPI_TYPE_VECTOR.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Type_vector_U MPI_TYPE_VECTOR

/** @def MPI_Unpack_U
    Exchange MPI_Unpack_U by MPI_UNPACK.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Unpack_U MPI_UNPACK

/** @def MPI_Unpack_external_U
    Exchange MPI_Unpack_external_U by MPI_UNPACK_EXTERNAL.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Unpack_external_U MPI_UNPACK_EXTERNAL


/** @def MPI_Type_create_keyval_U
    Exchange MPI_Type_create_keyval_U by MPI_TYPE_CREATE_KEYVAL.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Type_create_keyval_U MPI_TYPE_CREATE_KEYVAL

/** @def MPI_Type_delete_attr_U
    Exchange MPI_Type_delete_attr_U by MPI_TYPE_DELETE_ATTR.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Type_delete_attr_U MPI_TYPE_DELETE_ATTR

/** @def MPI_Type_free_keyval_U
    Exchange MPI_Type_free_keyval_U by MPI_TYPE_FREE_KEYVAL.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Type_free_keyval_U MPI_TYPE_FREE_KEYVAL

/** @def MPI_Type_get_attr_U
    Exchange MPI_Type_get_attr_U by MPI_TYPE_GET_ATTR.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Type_get_attr_U MPI_TYPE_GET_ATTR

/** @def MPI_Type_get_name_U
    Exchange MPI_Type_get_name_U by MPI_TYPE_GET_NAME.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Type_get_name_U MPI_TYPE_GET_NAME

/** @def MPI_Type_set_attr_U
    Exchange MPI_Type_set_attr_U by MPI_TYPE_SET_ATTR.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Type_set_attr_U MPI_TYPE_SET_ATTR

/** @def MPI_Type_set_name_U
    Exchange MPI_Type_set_name_U by MPI_TYPE_SET_NAME.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Type_set_name_U MPI_TYPE_SET_NAME



/* lowercase defines */
/** @def MPI_Sizeof_L
    Exchanges MPI_Sizeof_L by mpi_sizeof.
    It is used for the Forran wrappers of me).
 */
#define MPI_Sizeof_L mpi_sizeof


/** @def MPI_Pack_L
    Exchanges MPI_Pack_L by mpi_pack.
    It is used for the Forran wrappers of me).
 */
#define MPI_Pack_L mpi_pack

/** @def MPI_Pack_external_L
    Exchanges MPI_Pack_external_L by mpi_pack_external.
    It is used for the Forran wrappers of me).
 */
#define MPI_Pack_external_L mpi_pack_external

/** @def MPI_Pack_external_size_L
    Exchanges MPI_Pack_external_size_L by mpi_pack_external_size.
    It is used for the Forran wrappers of me).
 */
#define MPI_Pack_external_size_L mpi_pack_external_size

/** @def MPI_Pack_size_L
    Exchanges MPI_Pack_size_L by mpi_pack_size.
    It is used for the Forran wrappers of me).
 */
#define MPI_Pack_size_L mpi_pack_size

/** @def MPI_Type_commit_L
    Exchanges MPI_Type_commit_L by mpi_type_commit.
    It is used for the Forran wrappers of me).
 */
#define MPI_Type_commit_L mpi_type_commit

/** @def MPI_Type_contiguous_L
    Exchanges MPI_Type_contiguous_L by mpi_type_contiguous.
    It is used for the Forran wrappers of me).
 */
#define MPI_Type_contiguous_L mpi_type_contiguous

/** @def MPI_Type_create_darray_L
    Exchanges MPI_Type_create_darray_L by mpi_type_create_darray.
    It is used for the Forran wrappers of me).
 */
#define MPI_Type_create_darray_L mpi_type_create_darray

/** @def MPI_Type_create_f90_complex_L
    Exchanges MPI_Type_create_f90_complex_L by mpi_type_create_f90_complex.
    It is used for the Forran wrappers of me).
 */
#define MPI_Type_create_f90_complex_L mpi_type_create_f90_complex

/** @def MPI_Type_create_f90_integer_L
    Exchanges MPI_Type_create_f90_integer_L by mpi_type_create_f90_integer.
    It is used for the Forran wrappers of me).
 */
#define MPI_Type_create_f90_integer_L mpi_type_create_f90_integer

/** @def MPI_Type_create_f90_real_L
    Exchanges MPI_Type_create_f90_real_L by mpi_type_create_f90_real.
    It is used for the Forran wrappers of me).
 */
#define MPI_Type_create_f90_real_L mpi_type_create_f90_real

/** @def MPI_Type_create_hindexed_L
    Exchanges MPI_Type_create_hindexed_L by mpi_type_create_hindexed.
    It is used for the Forran wrappers of me).
 */
#define MPI_Type_create_hindexed_L mpi_type_create_hindexed

/** @def MPI_Type_create_hvector_L
    Exchanges MPI_Type_create_hvector_L by mpi_type_create_hvector.
    It is used for the Forran wrappers of me).
 */
#define MPI_Type_create_hvector_L mpi_type_create_hvector

/** @def MPI_Type_create_indexed_block_L
    Exchanges MPI_Type_create_indexed_block_L by mpi_type_create_indexed_block.
    It is used for the Forran wrappers of me).
 */
#define MPI_Type_create_indexed_block_L mpi_type_create_indexed_block

/** @def MPI_Type_create_resized_L
    Exchanges MPI_Type_create_resized_L by mpi_type_create_resized.
    It is used for the Forran wrappers of me).
 */
#define MPI_Type_create_resized_L mpi_type_create_resized

/** @def MPI_Type_create_struct_L
    Exchanges MPI_Type_create_struct_L by mpi_type_create_struct.
    It is used for the Forran wrappers of me).
 */
#define MPI_Type_create_struct_L mpi_type_create_struct

/** @def MPI_Type_create_subarray_L
    Exchanges MPI_Type_create_subarray_L by mpi_type_create_subarray.
    It is used for the Forran wrappers of me).
 */
#define MPI_Type_create_subarray_L mpi_type_create_subarray

/** @def MPI_Type_dup_L
    Exchanges MPI_Type_dup_L by mpi_type_dup.
    It is used for the Forran wrappers of me).
 */
#define MPI_Type_dup_L mpi_type_dup

/** @def MPI_Type_extent_L
    Exchanges MPI_Type_extent_L by mpi_type_extent.
    It is used for the Forran wrappers of me).
 */
#define MPI_Type_extent_L mpi_type_extent

/** @def MPI_Type_free_L
    Exchanges MPI_Type_free_L by mpi_type_free.
    It is used for the Forran wrappers of me).
 */
#define MPI_Type_free_L mpi_type_free

/** @def MPI_Type_get_contents_L
    Exchanges MPI_Type_get_contents_L by mpi_type_get_contents.
    It is used for the Forran wrappers of me).
 */
#define MPI_Type_get_contents_L mpi_type_get_contents

/** @def MPI_Type_get_envelope_L
    Exchanges MPI_Type_get_envelope_L by mpi_type_get_envelope.
    It is used for the Forran wrappers of me).
 */
#define MPI_Type_get_envelope_L mpi_type_get_envelope

/** @def MPI_Type_get_extent_L
    Exchanges MPI_Type_get_extent_L by mpi_type_get_extent.
    It is used for the Forran wrappers of me).
 */
#define MPI_Type_get_extent_L mpi_type_get_extent

/** @def MPI_Type_get_true_extent_L
    Exchanges MPI_Type_get_true_extent_L by mpi_type_get_true_extent.
    It is used for the Forran wrappers of me).
 */
#define MPI_Type_get_true_extent_L mpi_type_get_true_extent

/** @def MPI_Type_hindexed_L
    Exchanges MPI_Type_hindexed_L by mpi_type_hindexed.
    It is used for the Forran wrappers of me).
 */
#define MPI_Type_hindexed_L mpi_type_hindexed

/** @def MPI_Type_hvector_L
    Exchanges MPI_Type_hvector_L by mpi_type_hvector.
    It is used for the Forran wrappers of me).
 */
#define MPI_Type_hvector_L mpi_type_hvector

/** @def MPI_Type_indexed_L
    Exchanges MPI_Type_indexed_L by mpi_type_indexed.
    It is used for the Forran wrappers of me).
 */
#define MPI_Type_indexed_L mpi_type_indexed

/** @def MPI_Type_lb_L
    Exchanges MPI_Type_lb_L by mpi_type_lb.
    It is used for the Forran wrappers of me).
 */
#define MPI_Type_lb_L mpi_type_lb

/** @def MPI_Type_match_size_L
    Exchanges MPI_Type_match_size_L by mpi_type_match_size.
    It is used for the Forran wrappers of me).
 */
#define MPI_Type_match_size_L mpi_type_match_size

/** @def MPI_Type_size_L
    Exchanges MPI_Type_size_L by mpi_type_size.
    It is used for the Forran wrappers of me).
 */
#define MPI_Type_size_L mpi_type_size

/** @def MPI_Type_struct_L
    Exchanges MPI_Type_struct_L by mpi_type_struct.
    It is used for the Forran wrappers of me).
 */
#define MPI_Type_struct_L mpi_type_struct

/** @def MPI_Type_ub_L
    Exchanges MPI_Type_ub_L by mpi_type_ub.
    It is used for the Forran wrappers of me).
 */
#define MPI_Type_ub_L mpi_type_ub

/** @def MPI_Type_vector_L
    Exchanges MPI_Type_vector_L by mpi_type_vector.
    It is used for the Forran wrappers of me).
 */
#define MPI_Type_vector_L mpi_type_vector

/** @def MPI_Unpack_L
    Exchanges MPI_Unpack_L by mpi_unpack.
    It is used for the Forran wrappers of me).
 */
#define MPI_Unpack_L mpi_unpack

/** @def MPI_Unpack_external_L
    Exchanges MPI_Unpack_external_L by mpi_unpack_external.
    It is used for the Forran wrappers of me).
 */
#define MPI_Unpack_external_L mpi_unpack_external


/** @def MPI_Type_create_keyval_L
    Exchanges MPI_Type_create_keyval_L by mpi_type_create_keyval.
    It is used for the Forran wrappers of me).
 */
#define MPI_Type_create_keyval_L mpi_type_create_keyval

/** @def MPI_Type_delete_attr_L
    Exchanges MPI_Type_delete_attr_L by mpi_type_delete_attr.
    It is used for the Forran wrappers of me).
 */
#define MPI_Type_delete_attr_L mpi_type_delete_attr

/** @def MPI_Type_free_keyval_L
    Exchanges MPI_Type_free_keyval_L by mpi_type_free_keyval.
    It is used for the Forran wrappers of me).
 */
#define MPI_Type_free_keyval_L mpi_type_free_keyval

/** @def MPI_Type_get_attr_L
    Exchanges MPI_Type_get_attr_L by mpi_type_get_attr.
    It is used for the Forran wrappers of me).
 */
#define MPI_Type_get_attr_L mpi_type_get_attr

/** @def MPI_Type_get_name_L
    Exchanges MPI_Type_get_name_L by mpi_type_get_name.
    It is used for the Forran wrappers of me).
 */
#define MPI_Type_get_name_L mpi_type_get_name

/** @def MPI_Type_set_attr_L
    Exchanges MPI_Type_set_attr_L by mpi_type_set_attr.
    It is used for the Forran wrappers of me).
 */
#define MPI_Type_set_attr_L mpi_type_set_attr

/** @def MPI_Type_set_name_L
    Exchanges MPI_Type_set_name_L by mpi_type_set_name.
    It is used for the Forran wrappers of me).
 */
#define MPI_Type_set_name_L mpi_type_set_name



/**
 * @name Fortran wrappers for general functions
 * @{
 */

#ifndef NEED_F2C_CONV

/* If MPI_Comm_f2c is HAVE as a simple macro (which typically does nothing)
 * as it would require a "real" function if it is really needed
 * => we can save the f2c and c2s conversions */

#if HAVE( DECL_MPI_SIZEOF ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_TYPE )
/**
 * Measurement wrapper for MPI_Sizeof
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup type
 */
void
FSUB( MPI_Sizeof ) ( MPI_Datatype * x, int* size, int* ierr )
{
    *ierr = MPI_Sizeof( *x, *size );
}
#endif

#if HAVE( DECL_MPI_PACK ) && !defined( SILC_MPI_NO_EXTRA )
/**
 * Measurement wrapper for MPI_Pack
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup type
 */
void
FSUB( MPI_Pack ) ( void* inbuf, int* incount, MPI_Datatype * datatype, void* outbuf, int* outsize, int* position, MPI_Comm * comm, int* ierr )
{
    *ierr = MPI_Pack( inbuf, *incount, *datatype, outbuf, *outsize, position, *comm );
}
#endif
#if HAVE( DECL_MPI_PACK_EXTERNAL ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_TYPE )
/**
 * Measurement wrapper for MPI_Pack_external
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup type
 */
void
FSUB( MPI_Pack_external ) ( char* datarep, void* inbuf, int* incount, MPI_Datatype * datatype, void* outbuf, MPI_Aint * outsize, MPI_Aint * position, int* ierr, int datarep_len )
{
    char* c_datarep = NULL;
    c_datarep = ( char* )malloc( ( datarep_len + 1 ) * sizeof( char ) );
    if ( !c_datarep )
    {
        exit( EXIT_FAILURE );
    }
    strncpy( c_datarep, datarep, datarep_len );
    c_datarep[ datarep_len ] = '\0';


    *ierr = MPI_Pack_external( c_datarep, inbuf, *incount, *datatype, outbuf, *outsize, position );

    free( c_datarep );
}
#endif
#if HAVE( DECL_MPI_PACK_EXTERNAL_SIZE ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_TYPE )
/**
 * Measurement wrapper for MPI_Pack_external_size
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup type
 */
void
FSUB( MPI_Pack_external_size ) ( char* datarep, int* incount, MPI_Datatype * datatype, MPI_Aint * size, int* ierr, int datarep_len )
{
    char* c_datarep = NULL;
    c_datarep = ( char* )malloc( ( datarep_len + 1 ) * sizeof( char ) );
    if ( !c_datarep )
    {
        exit( EXIT_FAILURE );
    }
    strncpy( c_datarep, datarep, datarep_len );
    c_datarep[ datarep_len ] = '\0';


    *ierr = MPI_Pack_external_size( c_datarep, *incount, *datatype, size );

    free( c_datarep );
}
#endif
#if HAVE( DECL_MPI_PACK_SIZE ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_TYPE )
/**
 * Measurement wrapper for MPI_Pack_size
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup type
 */
void
FSUB( MPI_Pack_size ) ( int* incount, MPI_Datatype * datatype, MPI_Comm * comm, int* size, int* ierr )
{
    *ierr = MPI_Pack_size( *incount, *datatype, *comm, size );
}
#endif
#if HAVE( DECL_MPI_TYPE_COMMIT ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_TYPE )
/**
 * Measurement wrapper for MPI_Type_commit
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup type
 */
void
FSUB( MPI_Type_commit ) ( MPI_Datatype * datatype, int* ierr )
{
    *ierr = MPI_Type_commit( datatype );
}
#endif
#if HAVE( DECL_MPI_TYPE_CONTIGUOUS ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_TYPE )
/**
 * Measurement wrapper for MPI_Type_contiguous
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup type
 */
void
FSUB( MPI_Type_contiguous ) ( int* count, MPI_Datatype * oldtype, MPI_Datatype * newtype, int* ierr )
{
    *ierr = MPI_Type_contiguous( *count, *oldtype, newtype );
}
#endif
#if HAVE( DECL_MPI_TYPE_CREATE_DARRAY ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_TYPE )
/**
 * Measurement wrapper for MPI_Type_create_darray
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup type
 */
void
FSUB( MPI_Type_create_darray ) ( int* size, int* rank, int* ndims, int array_of_gsizes[], int array_of_distribs[], int array_of_dargs[], int array_of_psizes[], int* order, MPI_Datatype * oldtype, MPI_Datatype * newtype, int* ierr )
{
    *ierr = MPI_Type_create_darray( *size, *rank, *ndims, array_of_gsizes, array_of_distribs, array_of_dargs, array_of_psizes, *order, *oldtype, newtype );
}
#endif
#if HAVE( DECL_MPI_TYPE_CREATE_F90_COMPLEX ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_TYPE )
/**
 * Measurement wrapper for MPI_Type_create_f90_complex
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup type
 */
void
FSUB( MPI_Type_create_f90_complex ) ( int* p, int* r, MPI_Datatype * newtype, int* ierr )
{
    *ierr = MPI_Type_create_f90_complex( *p, *r, newtype );
}
#endif
#if HAVE( DECL_MPI_TYPE_CREATE_F90_INTEGER ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_TYPE )
/**
 * Measurement wrapper for MPI_Type_create_f90_integer
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup type
 */
void
FSUB( MPI_Type_create_f90_integer ) ( int* r, MPI_Datatype * newtype, int* ierr )
{
    *ierr = MPI_Type_create_f90_integer( *r, newtype );
}
#endif
#if HAVE( DECL_MPI_TYPE_CREATE_F90_REAL ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_TYPE )
/**
 * Measurement wrapper for MPI_Type_create_f90_real
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup type
 */
void
FSUB( MPI_Type_create_f90_real ) ( int* p, int* r, MPI_Datatype * newtype, int* ierr )
{
    *ierr = MPI_Type_create_f90_real( *p, *r, newtype );
}
#endif
#if HAVE( DECL_MPI_TYPE_CREATE_HINDEXED ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_TYPE )
/**
 * Measurement wrapper for MPI_Type_create_hindexed
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup type
 */
void
FSUB( MPI_Type_create_hindexed ) ( int* count, int array_of_blocklengths[], MPI_Aint array_of_displacements[], MPI_Datatype * oldtype, MPI_Datatype * newtype, int* ierr )
{
    *ierr = MPI_Type_create_hindexed( *count, array_of_blocklengths, array_of_displacements, *oldtype, newtype );
}
#endif
#if HAVE( DECL_MPI_TYPE_CREATE_HVECTOR ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_TYPE )
/**
 * Measurement wrapper for MPI_Type_create_hvector
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup type
 */
void
FSUB( MPI_Type_create_hvector ) ( int* count, int* blocklength, MPI_Aint * stride, MPI_Datatype * oldtype, MPI_Datatype * newtype, int* ierr )
{
    *ierr = MPI_Type_create_hvector( *count, *blocklength, *stride, *oldtype, newtype );
}
#endif
#if HAVE( DECL_MPI_TYPE_CREATE_INDEXED_BLOCK ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_TYPE )
/**
 * Measurement wrapper for MPI_Type_create_indexed_block
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup type
 */
void
FSUB( MPI_Type_create_indexed_block ) ( int* count, int* blocklength, int array_of_displacements[], MPI_Datatype * oldtype, MPI_Datatype * newtype, int* ierr )
{
    *ierr = MPI_Type_create_indexed_block( *count, *blocklength, array_of_displacements, *oldtype, newtype );
}
#endif
#if HAVE( DECL_MPI_TYPE_CREATE_RESIZED ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_TYPE )
/**
 * Measurement wrapper for MPI_Type_create_resized
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup type
 */
void
FSUB( MPI_Type_create_resized ) ( MPI_Datatype * oldtype, MPI_Aint * lb, MPI_Aint * extent, MPI_Datatype * newtype, int* ierr )
{
    *ierr = MPI_Type_create_resized( *oldtype, *lb, *extent, newtype );
}
#endif
#if HAVE( DECL_MPI_TYPE_CREATE_STRUCT ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_TYPE )
/**
 * Measurement wrapper for MPI_Type_create_struct
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup type
 */
void
FSUB( MPI_Type_create_struct ) ( int* count, int array_of_blocklengths[], MPI_Aint array_of_displacements[], MPI_Datatype array_of_types[], MPI_Datatype * newtype, int* ierr )
{
    *ierr = MPI_Type_create_struct( *count, array_of_blocklengths, array_of_displacements, array_of_types, newtype );
}
#endif
#if HAVE( DECL_MPI_TYPE_CREATE_SUBARRAY ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_TYPE )
/**
 * Measurement wrapper for MPI_Type_create_subarray
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup type
 */
void
FSUB( MPI_Type_create_subarray ) ( int* ndims, int array_of_sizes[], int array_of_subsizes[], int array_of_starts[], int* order, MPI_Datatype * oldtype, MPI_Datatype * newtype, int* ierr )
{
    *ierr = MPI_Type_create_subarray( *ndims, array_of_sizes, array_of_subsizes, array_of_starts, *order, *oldtype, newtype );
}
#endif
#if HAVE( DECL_MPI_TYPE_DUP ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_TYPE )
/**
 * Measurement wrapper for MPI_Type_dup
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup type
 */
void
FSUB( MPI_Type_dup ) ( MPI_Datatype * type, MPI_Datatype * newtype, int* ierr )
{
    *ierr = MPI_Type_dup( *type, newtype );
}
#endif
#if HAVE( DECL_MPI_TYPE_EXTENT ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_TYPE )
/**
 * Measurement wrapper for MPI_Type_extent
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup type
 */
void
FSUB( MPI_Type_extent ) ( MPI_Datatype * datatype, MPI_Aint * extent, int* ierr )
{
    *ierr = MPI_Type_extent( *datatype, extent );
}
#endif
#if HAVE( DECL_MPI_TYPE_FREE ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_TYPE )
/**
 * Measurement wrapper for MPI_Type_free
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup type
 */
void
FSUB( MPI_Type_free ) ( MPI_Datatype * datatype, int* ierr )
{
    *ierr = MPI_Type_free( datatype );
}
#endif
#if HAVE( DECL_MPI_TYPE_GET_CONTENTS ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_TYPE )
/**
 * Measurement wrapper for MPI_Type_get_contents
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup type
 */
void
FSUB( MPI_Type_get_contents ) ( MPI_Datatype * datatype, int* max_integers, int* max_addresses, int* max_datatypes, int array_of_integers[], MPI_Aint array_of_addresses[], MPI_Datatype array_of_datatypes[], int* ierr )
{
    *ierr = MPI_Type_get_contents( *datatype, *max_integers, *max_addresses, *max_datatypes, array_of_integers, array_of_addresses, array_of_datatypes );
}
#endif
#if HAVE( DECL_MPI_TYPE_GET_ENVELOPE ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_TYPE )
/**
 * Measurement wrapper for MPI_Type_get_envelope
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup type
 */
void
FSUB( MPI_Type_get_envelope ) ( MPI_Datatype * datatype, int* num_integers, int* num_addresses, int* num_datatypes, int* combiner, int* ierr )
{
    *ierr = MPI_Type_get_envelope( *datatype, num_integers, num_addresses, num_datatypes, combiner );
}
#endif
#if HAVE( DECL_MPI_TYPE_GET_EXTENT ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_TYPE )
/**
 * Measurement wrapper for MPI_Type_get_extent
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup type
 */
void
FSUB( MPI_Type_get_extent ) ( MPI_Datatype * datatype, MPI_Aint * lb, MPI_Aint * extent, int* ierr )
{
    *ierr = MPI_Type_get_extent( *datatype, lb, extent );
}
#endif
#if HAVE( DECL_MPI_TYPE_GET_TRUE_EXTENT ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_TYPE )
/**
 * Measurement wrapper for MPI_Type_get_true_extent
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup type
 */
void
FSUB( MPI_Type_get_true_extent ) ( MPI_Datatype * datatype, MPI_Aint * true_lb, MPI_Aint * true_extent, int* ierr )
{
    *ierr = MPI_Type_get_true_extent( *datatype, true_lb, true_extent );
}
#endif
#if HAVE( DECL_MPI_TYPE_HINDEXED ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_TYPE )
/**
 * Measurement wrapper for MPI_Type_hindexed
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup type
 */
void
FSUB( MPI_Type_hindexed ) ( int* count, int* array_of_blocklengths, MPI_Aint * array_of_displacements, MPI_Datatype * oldtype, MPI_Datatype * newtype, int* ierr )
{
    *ierr = MPI_Type_hindexed( *count, array_of_blocklengths, array_of_displacements, *oldtype, newtype );
}
#endif
#if HAVE( DECL_MPI_TYPE_HVECTOR ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_TYPE )
/**
 * Measurement wrapper for MPI_Type_hvector
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup type
 */
void
FSUB( MPI_Type_hvector ) ( int* count, int* blocklength, MPI_Aint * stride, MPI_Datatype * oldtype, MPI_Datatype * newtype, int* ierr )
{
    *ierr = MPI_Type_hvector( *count, *blocklength, *stride, *oldtype, newtype );
}
#endif
#if HAVE( DECL_MPI_TYPE_INDEXED ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_TYPE )
/**
 * Measurement wrapper for MPI_Type_indexed
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup type
 */
void
FSUB( MPI_Type_indexed ) ( int* count, int* array_of_blocklengths, int* array_of_displacements, MPI_Datatype * oldtype, MPI_Datatype * newtype, int* ierr )
{
    *ierr = MPI_Type_indexed( *count, array_of_blocklengths, array_of_displacements, *oldtype, newtype );
}
#endif
#if HAVE( DECL_MPI_TYPE_LB ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_TYPE )
/**
 * Measurement wrapper for MPI_Type_lb
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup type
 */
void
FSUB( MPI_Type_lb ) ( MPI_Datatype * datatype, MPI_Aint * displacement, int* ierr )
{
    *ierr = MPI_Type_lb( *datatype, displacement );
}
#endif
#if HAVE( DECL_MPI_TYPE_MATCH_SIZE ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_TYPE )
/**
 * Measurement wrapper for MPI_Type_match_size
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup type
 */
void
FSUB( MPI_Type_match_size ) ( int* typeclass, int* size, MPI_Datatype * type, int* ierr )
{
    *ierr = MPI_Type_match_size( *typeclass, *size, type );
}
#endif
#if HAVE( DECL_MPI_TYPE_SIZE ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_TYPE )
/**
 * Measurement wrapper for MPI_Type_size
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup type
 */
void
FSUB( MPI_Type_size ) ( MPI_Datatype * datatype, int* size, int* ierr )
{
    *ierr = MPI_Type_size( *datatype, size );
}
#endif
#if HAVE( DECL_MPI_TYPE_STRUCT ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_TYPE )
/**
 * Measurement wrapper for MPI_Type_struct
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup type
 */
void
FSUB( MPI_Type_struct ) ( int* count, int* array_of_blocklengths, MPI_Aint * array_of_displacements, MPI_Datatype * array_of_types, MPI_Datatype * newtype, int* ierr )
{
    *ierr = MPI_Type_struct( *count, array_of_blocklengths, array_of_displacements, array_of_types, newtype );
}
#endif
#if HAVE( DECL_MPI_TYPE_UB ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_TYPE )
/**
 * Measurement wrapper for MPI_Type_ub
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup type
 */
void
FSUB( MPI_Type_ub ) ( MPI_Datatype * datatype, MPI_Aint * displacement, int* ierr )
{
    *ierr = MPI_Type_ub( *datatype, displacement );
}
#endif
#if HAVE( DECL_MPI_TYPE_VECTOR ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_TYPE )
/**
 * Measurement wrapper for MPI_Type_vector
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup type
 */
void
FSUB( MPI_Type_vector ) ( int* count, int* blocklength, int* stride, MPI_Datatype * oldtype, MPI_Datatype * newtype, int* ierr )
{
    *ierr = MPI_Type_vector( *count, *blocklength, *stride, *oldtype, newtype );
}
#endif
#if HAVE( DECL_MPI_UNPACK ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_TYPE )
/**
 * Measurement wrapper for MPI_Unpack
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup type
 */
void
FSUB( MPI_Unpack ) ( void* inbuf, int* insize, int* position, void* outbuf, int* outcount, MPI_Datatype * datatype, MPI_Comm * comm, int* ierr )
{
    *ierr = MPI_Unpack( inbuf, *insize, position, outbuf, *outcount, *datatype, *comm );
}
#endif
#if HAVE( DECL_MPI_UNPACK_EXTERNAL ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_TYPE )
/**
 * Measurement wrapper for MPI_Unpack_external
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup type
 */
void
FSUB( MPI_Unpack_external ) ( char* datarep, void* inbuf, MPI_Aint * insize, MPI_Aint * position, void* outbuf, int* outcount, MPI_Datatype * datatype, int* ierr, int datarep_len )
{
    char* c_datarep = NULL;
    c_datarep = ( char* )malloc( ( datarep_len + 1 ) * sizeof( char ) );
    if ( !c_datarep )
    {
        exit( EXIT_FAILURE );
    }
    strncpy( c_datarep, datarep, datarep_len );
    c_datarep[ datarep_len ] = '\0';


    *ierr = MPI_Unpack_external( c_datarep, inbuf, *insize, position, outbuf, *outcount, *datatype );

    free( c_datarep );
}
#endif

/**
 * @}
 * @name Fortran wrappers for external interface functions
 * @{
 */

#if HAVE( DECL_MPI_TYPE_CREATE_KEYVAL ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_TYPE ) && !defined( SILC_MPI_NO_EXT )
/**
 * Measurement wrapper for MPI_Type_create_keyval
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup type_ext
 */
void
FSUB( MPI_Type_create_keyval ) ( MPI_Type_copy_attr_function * type_copy_attr_fn, MPI_Type_delete_attr_function * type_delete_attr_fn, int* type_keyval, void* extra_state, int* ierr )
{
    *ierr = MPI_Type_create_keyval( type_copy_attr_fn, type_delete_attr_fn, type_keyval, extra_state );
}
#endif
#if HAVE( DECL_MPI_TYPE_DELETE_ATTR ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_TYPE ) && !defined( SILC_MPI_NO_EXT )
/**
 * Measurement wrapper for MPI_Type_delete_attr
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup type_ext
 */
void
FSUB( MPI_Type_delete_attr ) ( MPI_Datatype * type, int* type_keyval, int* ierr )
{
    *ierr = MPI_Type_delete_attr( *type, *type_keyval );
}
#endif
#if HAVE( DECL_MPI_TYPE_FREE_KEYVAL ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_TYPE ) && !defined( SILC_MPI_NO_EXT )
/**
 * Measurement wrapper for MPI_Type_free_keyval
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup type_ext
 */
void
FSUB( MPI_Type_free_keyval ) ( int* type_keyval, int* ierr )
{
    *ierr = MPI_Type_free_keyval( type_keyval );
}
#endif
#if HAVE( DECL_MPI_TYPE_GET_ATTR ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_TYPE ) && !defined( SILC_MPI_NO_EXT )
/**
 * Measurement wrapper for MPI_Type_get_attr
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup type_ext
 */
void
FSUB( MPI_Type_get_attr ) ( MPI_Datatype * type, int* type_keyval, void* attribute_val, int* flag, int* ierr )
{
    *ierr = MPI_Type_get_attr( *type, *type_keyval, attribute_val, flag );
}
#endif
#if HAVE( DECL_MPI_TYPE_GET_NAME ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_TYPE ) && !defined( SILC_MPI_NO_EXT )
/**
 * Measurement wrapper for MPI_Type_get_name
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup type_ext
 */
void
FSUB( MPI_Type_get_name ) ( MPI_Datatype * type, char* type_name, int* resultlen, int* ierr, int type_name_len )
{
    char* c_type_name = NULL;
    c_type_name = ( char* )malloc( ( type_name_len + 1 ) * sizeof( char ) );
    if ( !c_type_name )
    {
        exit( EXIT_FAILURE );
    }
    strncpy( c_type_name, type_name, type_name_len );
    c_type_name[ type_name_len ] = '\0';


    *ierr = MPI_Type_get_name( *type, c_type_name, resultlen );

    free( c_type_name );
}
#endif
#if HAVE( DECL_MPI_TYPE_SET_ATTR ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_TYPE ) && !defined( SILC_MPI_NO_EXT )
/**
 * Measurement wrapper for MPI_Type_set_attr
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup type_ext
 */
void
FSUB( MPI_Type_set_attr ) ( MPI_Datatype * type, int* type_keyval, void* attribute_val, int* ierr )
{
    *ierr = MPI_Type_set_attr( *type, *type_keyval, attribute_val );
}
#endif
#if HAVE( DECL_MPI_TYPE_SET_NAME ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_TYPE ) && !defined( SILC_MPI_NO_EXT )
/**
 * Measurement wrapper for MPI_Type_set_name
 * @note Auto-generated by wrapgen from template: SILC_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup type_ext
 */
void
FSUB( MPI_Type_set_name ) ( MPI_Datatype * type, char* type_name, int* ierr, int type_name_len )
{
    char* c_type_name = NULL;
    c_type_name = ( char* )malloc( ( type_name_len + 1 ) * sizeof( char ) );
    if ( !c_type_name )
    {
        exit( EXIT_FAILURE );
    }
    strncpy( c_type_name, type_name, type_name_len );
    c_type_name[ type_name_len ] = '\0';


    *ierr = MPI_Type_set_name( *type, c_type_name );

    free( c_type_name );
}
#endif

/**
 * @}
 * @name Fortran wrappers for handle conversion functions
 * @{
 */



#else /* !NEED_F2C_CONV */

#if HAVE( DECL_MPI_SIZEOF ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_TYPE )
/**
 * Measurement wrapper for MPI_Sizeof
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup type
 */
void
FSUB( MPI_Sizeof ) ( MPI_Fint * x, MPI_Fint * size, int* ierr )
{
    *ierr = MPI_Sizeof( PMPI_Type_f2c( *x ), *size );
}
#endif

#if HAVE( DECL_MPI_TYPE_STRUCT ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_TYPE )
/**
 * Manual measurement wrapper for MPI_Type_struct
 * @ingroup manual_wrapper
 * @ingroup interface_fortran
 * @ingroup mpi_version_1
 * @ingroup mpi_enabled_type
 */
void
FSUB( MPI_Type_struct ) ( MPI_Fint * count,
                          MPI_Fint * array_of_blocklengths,
                          MPI_Aint * array_of_displacements,
                          MPI_Fint * array_of_types,
                          MPI_Fint * newtype,
                          MPI_Fint * ierr )
{
    MPI_Fint      i;
    MPI_Datatype  c_newtype;
    MPI_Datatype* c_array_of_types =
        ( MPI_Datatype* )malloc( *count * sizeof( MPI_Datatype ) );
    if ( !c_array_of_types )
    {
        exit( EXIT_FAILURE );
    }

    for ( i = 0; i < *count; ++i )
    {
        c_array_of_types[ i ] = PMPI_Type_f2c( array_of_types[ i ] );
    }
    *ierr = MPI_Type_struct( *count, array_of_blocklengths,
                             array_of_displacements,
                             c_array_of_types, &c_newtype );
    *newtype = PMPI_Type_c2f( c_newtype );
}
#endif

#if HAVE( DECL_MPI_TYPE_CREATE_STRUCT ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_TYPE )
/**
 * Manual measurement wrapper for MPI_Type_create_struct
 * @ingroup manual_wrapper
 * @ingroup interface_fortran
 * @ingroup mpi_version_2
 * @ingroup mpi_enabled_type
 */
void
FSUB( MPI_Type_create_struct ) ( MPI_Fint * count,
                                 MPI_Fint * array_of_blocklengths,
                                 MPI_Aint array_of_displacements[],
                                 MPI_Fint * array_of_types,
                                 MPI_Fint * newtype,
                                 MPI_Fint * ierr )
{
    MPI_Fint      i;
    MPI_Datatype  c_newtype;
    MPI_Datatype* c_array_of_types =
        ( MPI_Datatype* )malloc( *count * sizeof( MPI_Datatype ) );
    if ( !c_array_of_types )
    {
        exit( EXIT_FAILURE );
    }

    for ( i = 0; i < *count; ++i )
    {
        c_array_of_types[ i ] = PMPI_Type_f2c( array_of_types[ i ] );
    }
    *ierr = MPI_Type_struct( *count, array_of_blocklengths,
                             array_of_displacements,
                             c_array_of_types, &c_newtype );
    *newtype = PMPI_Type_c2f( c_newtype );
}
#endif

#if HAVE( DECL_MPI_TYPE_GET_CONTENTS ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_TYPE )
/**
 * Manual measurement wrapper for MPI_Type_get_contents
 * @ingroup manual_wrapper
 * @ingroup interface_fortran
 * @ingroup mpi_version_2
 * @ingroup mpi_enabled_type
 */
void
FSUB( MPI_Type_get_contents ) ( MPI_Fint * datatype,
                                MPI_Fint * max_integers,
                                MPI_Fint * max_addresses,
                                MPI_Fint * max_datatypes,
                                MPI_Fint * array_of_integers,
                                MPI_Aint * array_of_addresses,
                                MPI_Fint * array_of_datatypes,
                                MPI_Fint * ierr )
{
    MPI_Fint      i;
    MPI_Datatype* c_array_of_datatypes = NULL;
    /* provide storage for C Datatypes */
    c_array_of_datatypes =
        ( MPI_Datatype* )malloc( *max_datatypes * sizeof( MPI_Datatype ) );
    if ( !c_array_of_datatypes )
    {
        exit( EXIT_FAILURE );
    }
    *ierr = MPI_Type_get_contents( PMPI_Type_f2c( *datatype ),
                                   *max_integers,
                                   *max_addresses,
                                   *max_datatypes,
                                   array_of_integers,
                                   array_of_addresses,
                                   c_array_of_datatypes );
    /* convert C handles to Fortran handles */
    for ( i = 0; i < *max_datatypes; ++i )
    {
        array_of_datatypes[ i ] = PMPI_Type_c2f( c_array_of_datatypes[ i ] );
    }
    /* free local buffer */
    free( c_array_of_datatypes );
}
#endif


/**
 * @}
 * @name Fortran wrappers for external interface functions
 * @{
 */

#if HAVE( DECL_MPI_TYPE_CREATE_KEYVAL ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_TYPE ) && !defined( SILC_MPI_NO_EXT )
/**
 * Measurement wrapper for MPI_Type_create_keyval
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup type_ext
 */
void
FSUB( MPI_Type_create_keyval ) ( void* type_copy_attr_fn, void* type_delete_attr_fn, MPI_Fint * type_keyval, void* extra_state, int* ierr )
{
    *ierr = MPI_Type_create_keyval( ( MPI_Type_copy_attr_function* )type_copy_attr_fn, ( MPI_Type_delete_attr_function* )type_delete_attr_fn, type_keyval, extra_state );
}
#endif
#if HAVE( DECL_MPI_TYPE_DELETE_ATTR ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_TYPE ) && !defined( SILC_MPI_NO_EXT )
/**
 * Measurement wrapper for MPI_Type_delete_attr
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup type_ext
 */
void
FSUB( MPI_Type_delete_attr ) ( MPI_Fint * type, MPI_Fint * type_keyval, int* ierr )
{
    MPI_Datatype c_type = PMPI_Type_f2c( *type );
    *ierr = MPI_Type_delete_attr( c_type, *type_keyval );
    *type = PMPI_Type_c2f( c_type );
}
#endif
#if HAVE( DECL_MPI_TYPE_FREE_KEYVAL ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_TYPE ) && !defined( SILC_MPI_NO_EXT )
/**
 * Measurement wrapper for MPI_Type_free_keyval
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup type_ext
 */
void
FSUB( MPI_Type_free_keyval ) ( MPI_Fint * type_keyval, int* ierr )
{
    *ierr = MPI_Type_free_keyval( type_keyval );
}
#endif
#if HAVE( DECL_MPI_TYPE_GET_ATTR ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_TYPE ) && !defined( SILC_MPI_NO_EXT )
/**
 * Measurement wrapper for MPI_Type_get_attr
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup type_ext
 */
void
FSUB( MPI_Type_get_attr ) ( MPI_Fint * type, MPI_Fint * type_keyval, void* attribute_val, MPI_Fint * flag, int* ierr )
{
    *ierr = MPI_Type_get_attr( PMPI_Type_f2c( *type ), *type_keyval, attribute_val, flag );
}
#endif
#if HAVE( DECL_MPI_TYPE_GET_NAME ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_TYPE ) && !defined( SILC_MPI_NO_EXT )
/**
 * Measurement wrapper for MPI_Type_get_name
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup type_ext
 */
void
FSUB( MPI_Type_get_name ) ( MPI_Fint * type, char* type_name, MPI_Fint * resultlen, int* ierr, int type_name_len )
{
    char* c_type_name = NULL;
    c_type_name = ( char* )malloc( ( type_name_len + 1 ) * sizeof( char ) );
    if ( !c_type_name )
    {
        exit( EXIT_FAILURE );
    }
    strncpy( c_type_name, type_name, type_name_len );
    c_type_name[ type_name_len ] = '\0';

    *ierr = MPI_Type_get_name( PMPI_Type_f2c( *type ), type_name, resultlen );
    free( c_type_name );
}
#endif
#if HAVE( DECL_MPI_TYPE_SET_ATTR ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_TYPE ) && !defined( SILC_MPI_NO_EXT )
/**
 * Measurement wrapper for MPI_Type_set_attr
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup type_ext
 */
void
FSUB( MPI_Type_set_attr ) ( MPI_Fint * type, MPI_Fint * type_keyval, void* attribute_val, int* ierr )
{
    MPI_Datatype c_type = PMPI_Type_f2c( *type );
    *ierr = MPI_Type_set_attr( c_type, *type_keyval, attribute_val );
    *type = PMPI_Type_c2f( c_type );
}
#endif
#if HAVE( DECL_MPI_TYPE_SET_NAME ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_TYPE ) && !defined( SILC_MPI_NO_EXT )
/**
 * Measurement wrapper for MPI_Type_set_name
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup type_ext
 */
void
FSUB( MPI_Type_set_name ) ( MPI_Fint * type, char* type_name, int* ierr, int type_name_len )
{
    char*        c_type_name = NULL;
    MPI_Datatype c_type      = PMPI_Type_f2c( *type );
    c_type_name = ( char* )malloc( ( type_name_len + 1 ) * sizeof( char ) );
    if ( !c_type_name )
    {
        exit( EXIT_FAILURE );
    }
    strncpy( c_type_name, type_name, type_name_len );
    c_type_name[ type_name_len ] = '\0';

    *ierr = MPI_Type_set_name( c_type, type_name );
    free( c_type_name );
    *type = PMPI_Type_c2f( c_type );
}
#endif

/**
 * @}
 * @name Fortran wrappers for handle conversion functions
 * @{
 */


#endif

/**
 * @}
 */
