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
 * @brief Fortran interface wrappers for datatype functions
 */

#include <config.h>
#include "SCOREP_Fmpi.h"

#include <UTILS_Debug.h>
#include <UTILS_Error.h>

/* uppercase defines */
/** @def MPI_Pack_U
    Exchange MPI_Pack_U by MPI_PACK.
    It is used for the Fortran wrappers of MPI_Pack.
 */
#define MPI_Pack_U MPI_PACK

/** @def MPI_Pack_external_U
    Exchange MPI_Pack_external_U by MPI_PACK_EXTERNAL.
    It is used for the Fortran wrappers of MPI_Pack_external.
 */
#define MPI_Pack_external_U MPI_PACK_EXTERNAL

/** @def MPI_Pack_external_size_U
    Exchange MPI_Pack_external_size_U by MPI_PACK_EXTERNAL_SIZE.
    It is used for the Fortran wrappers of MPI_Pack_external_size.
 */
#define MPI_Pack_external_size_U MPI_PACK_EXTERNAL_SIZE

/** @def MPI_Pack_size_U
    Exchange MPI_Pack_size_U by MPI_PACK_SIZE.
    It is used for the Fortran wrappers of MPI_Pack_size.
 */
#define MPI_Pack_size_U MPI_PACK_SIZE

/** @def MPI_Sizeof_U
    Exchange MPI_Sizeof_U by MPI_SIZEOF.
    It is used for the Fortran wrappers of MPI_Sizeof.
 */
#define MPI_Sizeof_U MPI_SIZEOF

/** @def MPI_Type_commit_U
    Exchange MPI_Type_commit_U by MPI_TYPE_COMMIT.
    It is used for the Fortran wrappers of MPI_Type_commit.
 */
#define MPI_Type_commit_U MPI_TYPE_COMMIT

/** @def MPI_Type_contiguous_U
    Exchange MPI_Type_contiguous_U by MPI_TYPE_CONTIGUOUS.
    It is used for the Fortran wrappers of MPI_Type_contiguous.
 */
#define MPI_Type_contiguous_U MPI_TYPE_CONTIGUOUS

/** @def MPI_Type_create_darray_U
    Exchange MPI_Type_create_darray_U by MPI_TYPE_CREATE_DARRAY.
    It is used for the Fortran wrappers of MPI_Type_create_darray.
 */
#define MPI_Type_create_darray_U MPI_TYPE_CREATE_DARRAY

/** @def MPI_Type_create_f90_complex_U
    Exchange MPI_Type_create_f90_complex_U by MPI_TYPE_CREATE_F90_COMPLEX.
    It is used for the Fortran wrappers of MPI_Type_create_f90_complex.
 */
#define MPI_Type_create_f90_complex_U MPI_TYPE_CREATE_F90_COMPLEX

/** @def MPI_Type_create_f90_integer_U
    Exchange MPI_Type_create_f90_integer_U by MPI_TYPE_CREATE_F90_INTEGER.
    It is used for the Fortran wrappers of MPI_Type_create_f90_integer.
 */
#define MPI_Type_create_f90_integer_U MPI_TYPE_CREATE_F90_INTEGER

/** @def MPI_Type_create_f90_real_U
    Exchange MPI_Type_create_f90_real_U by MPI_TYPE_CREATE_F90_REAL.
    It is used for the Fortran wrappers of MPI_Type_create_f90_real.
 */
#define MPI_Type_create_f90_real_U MPI_TYPE_CREATE_F90_REAL

/** @def MPI_Type_create_hindexed_U
    Exchange MPI_Type_create_hindexed_U by MPI_TYPE_CREATE_HINDEXED.
    It is used for the Fortran wrappers of MPI_Type_create_hindexed.
 */
#define MPI_Type_create_hindexed_U MPI_TYPE_CREATE_HINDEXED

/** @def MPI_Type_create_hindexed_block_U
    Exchange MPI_Type_create_hindexed_block_U by MPI_TYPE_CREATE_HINDEXED_BLOCK.
    It is used for the Fortran wrappers of MPI_Type_create_hindexed_block.
 */
#define MPI_Type_create_hindexed_block_U MPI_TYPE_CREATE_HINDEXED_BLOCK

/** @def MPI_Type_create_hvector_U
    Exchange MPI_Type_create_hvector_U by MPI_TYPE_CREATE_HVECTOR.
    It is used for the Fortran wrappers of MPI_Type_create_hvector.
 */
#define MPI_Type_create_hvector_U MPI_TYPE_CREATE_HVECTOR

/** @def MPI_Type_create_indexed_block_U
    Exchange MPI_Type_create_indexed_block_U by MPI_TYPE_CREATE_INDEXED_BLOCK.
    It is used for the Fortran wrappers of MPI_Type_create_indexed_block.
 */
#define MPI_Type_create_indexed_block_U MPI_TYPE_CREATE_INDEXED_BLOCK

/** @def MPI_Type_create_resized_U
    Exchange MPI_Type_create_resized_U by MPI_TYPE_CREATE_RESIZED.
    It is used for the Fortran wrappers of MPI_Type_create_resized.
 */
#define MPI_Type_create_resized_U MPI_TYPE_CREATE_RESIZED

/** @def MPI_Type_create_struct_U
    Exchange MPI_Type_create_struct_U by MPI_TYPE_CREATE_STRUCT.
    It is used for the Fortran wrappers of MPI_Type_create_struct.
 */
#define MPI_Type_create_struct_U MPI_TYPE_CREATE_STRUCT

/** @def MPI_Type_create_subarray_U
    Exchange MPI_Type_create_subarray_U by MPI_TYPE_CREATE_SUBARRAY.
    It is used for the Fortran wrappers of MPI_Type_create_subarray.
 */
#define MPI_Type_create_subarray_U MPI_TYPE_CREATE_SUBARRAY

/** @def MPI_Type_dup_U
    Exchange MPI_Type_dup_U by MPI_TYPE_DUP.
    It is used for the Fortran wrappers of MPI_Type_dup.
 */
#define MPI_Type_dup_U MPI_TYPE_DUP

/** @def MPI_Type_extent_U
    Exchange MPI_Type_extent_U by MPI_TYPE_EXTENT.
    It is used for the Fortran wrappers of MPI_Type_extent.
 */
#define MPI_Type_extent_U MPI_TYPE_EXTENT

/** @def MPI_Type_free_U
    Exchange MPI_Type_free_U by MPI_TYPE_FREE.
    It is used for the Fortran wrappers of MPI_Type_free.
 */
#define MPI_Type_free_U MPI_TYPE_FREE

/** @def MPI_Type_get_contents_U
    Exchange MPI_Type_get_contents_U by MPI_TYPE_GET_CONTENTS.
    It is used for the Fortran wrappers of MPI_Type_get_contents.
 */
#define MPI_Type_get_contents_U MPI_TYPE_GET_CONTENTS

/** @def MPI_Type_get_envelope_U
    Exchange MPI_Type_get_envelope_U by MPI_TYPE_GET_ENVELOPE.
    It is used for the Fortran wrappers of MPI_Type_get_envelope.
 */
#define MPI_Type_get_envelope_U MPI_TYPE_GET_ENVELOPE

/** @def MPI_Type_get_extent_U
    Exchange MPI_Type_get_extent_U by MPI_TYPE_GET_EXTENT.
    It is used for the Fortran wrappers of MPI_Type_get_extent.
 */
#define MPI_Type_get_extent_U MPI_TYPE_GET_EXTENT

/** @def MPI_Type_get_extent_x_U
    Exchange MPI_Type_get_extent_x_U by MPI_TYPE_GET_EXTENT_X.
    It is used for the Fortran wrappers of MPI_Type_get_extent_x.
 */
#define MPI_Type_get_extent_x_U MPI_TYPE_GET_EXTENT_X

/** @def MPI_Type_get_true_extent_U
    Exchange MPI_Type_get_true_extent_U by MPI_TYPE_GET_TRUE_EXTENT.
    It is used for the Fortran wrappers of MPI_Type_get_true_extent.
 */
#define MPI_Type_get_true_extent_U MPI_TYPE_GET_TRUE_EXTENT

/** @def MPI_Type_get_true_extent_x_U
    Exchange MPI_Type_get_true_extent_x_U by MPI_TYPE_GET_TRUE_EXTENT_X.
    It is used for the Fortran wrappers of MPI_Type_get_true_extent_x.
 */
#define MPI_Type_get_true_extent_x_U MPI_TYPE_GET_TRUE_EXTENT_X

/** @def MPI_Type_hindexed_U
    Exchange MPI_Type_hindexed_U by MPI_TYPE_HINDEXED.
    It is used for the Fortran wrappers of MPI_Type_hindexed.
 */
#define MPI_Type_hindexed_U MPI_TYPE_HINDEXED

/** @def MPI_Type_hvector_U
    Exchange MPI_Type_hvector_U by MPI_TYPE_HVECTOR.
    It is used for the Fortran wrappers of MPI_Type_hvector.
 */
#define MPI_Type_hvector_U MPI_TYPE_HVECTOR

/** @def MPI_Type_indexed_U
    Exchange MPI_Type_indexed_U by MPI_TYPE_INDEXED.
    It is used for the Fortran wrappers of MPI_Type_indexed.
 */
#define MPI_Type_indexed_U MPI_TYPE_INDEXED

/** @def MPI_Type_lb_U
    Exchange MPI_Type_lb_U by MPI_TYPE_LB.
    It is used for the Fortran wrappers of MPI_Type_lb.
 */
#define MPI_Type_lb_U MPI_TYPE_LB

/** @def MPI_Type_match_size_U
    Exchange MPI_Type_match_size_U by MPI_TYPE_MATCH_SIZE.
    It is used for the Fortran wrappers of MPI_Type_match_size.
 */
#define MPI_Type_match_size_U MPI_TYPE_MATCH_SIZE

/** @def MPI_Type_size_U
    Exchange MPI_Type_size_U by MPI_TYPE_SIZE.
    It is used for the Fortran wrappers of MPI_Type_size.
 */
#define MPI_Type_size_U MPI_TYPE_SIZE

/** @def MPI_Type_size_x_U
    Exchange MPI_Type_size_x_U by MPI_TYPE_SIZE_X.
    It is used for the Fortran wrappers of MPI_Type_size_x.
 */
#define MPI_Type_size_x_U MPI_TYPE_SIZE_X

/** @def MPI_Type_struct_U
    Exchange MPI_Type_struct_U by MPI_TYPE_STRUCT.
    It is used for the Fortran wrappers of MPI_Type_struct.
 */
#define MPI_Type_struct_U MPI_TYPE_STRUCT

/** @def MPI_Type_ub_U
    Exchange MPI_Type_ub_U by MPI_TYPE_UB.
    It is used for the Fortran wrappers of MPI_Type_ub.
 */
#define MPI_Type_ub_U MPI_TYPE_UB

/** @def MPI_Type_vector_U
    Exchange MPI_Type_vector_U by MPI_TYPE_VECTOR.
    It is used for the Fortran wrappers of MPI_Type_vector.
 */
#define MPI_Type_vector_U MPI_TYPE_VECTOR

/** @def MPI_Unpack_U
    Exchange MPI_Unpack_U by MPI_UNPACK.
    It is used for the Fortran wrappers of MPI_Unpack.
 */
#define MPI_Unpack_U MPI_UNPACK

/** @def MPI_Unpack_external_U
    Exchange MPI_Unpack_external_U by MPI_UNPACK_EXTERNAL.
    It is used for the Fortran wrappers of MPI_Unpack_external.
 */
#define MPI_Unpack_external_U MPI_UNPACK_EXTERNAL


/** @def MPI_Type_create_keyval_U
    Exchange MPI_Type_create_keyval_U by MPI_TYPE_CREATE_KEYVAL.
    It is used for the Fortran wrappers of MPI_Type_create_keyval.
 */
#define MPI_Type_create_keyval_U MPI_TYPE_CREATE_KEYVAL

/** @def MPI_Type_delete_attr_U
    Exchange MPI_Type_delete_attr_U by MPI_TYPE_DELETE_ATTR.
    It is used for the Fortran wrappers of MPI_Type_delete_attr.
 */
#define MPI_Type_delete_attr_U MPI_TYPE_DELETE_ATTR

/** @def MPI_Type_free_keyval_U
    Exchange MPI_Type_free_keyval_U by MPI_TYPE_FREE_KEYVAL.
    It is used for the Fortran wrappers of MPI_Type_free_keyval.
 */
#define MPI_Type_free_keyval_U MPI_TYPE_FREE_KEYVAL

/** @def MPI_Type_get_attr_U
    Exchange MPI_Type_get_attr_U by MPI_TYPE_GET_ATTR.
    It is used for the Fortran wrappers of MPI_Type_get_attr.
 */
#define MPI_Type_get_attr_U MPI_TYPE_GET_ATTR

/** @def MPI_Type_get_name_U
    Exchange MPI_Type_get_name_U by MPI_TYPE_GET_NAME.
    It is used for the Fortran wrappers of MPI_Type_get_name.
 */
#define MPI_Type_get_name_U MPI_TYPE_GET_NAME

/** @def MPI_Type_set_attr_U
    Exchange MPI_Type_set_attr_U by MPI_TYPE_SET_ATTR.
    It is used for the Fortran wrappers of MPI_Type_set_attr.
 */
#define MPI_Type_set_attr_U MPI_TYPE_SET_ATTR

/** @def MPI_Type_set_name_U
    Exchange MPI_Type_set_name_U by MPI_TYPE_SET_NAME.
    It is used for the Fortran wrappers of MPI_Type_set_name.
 */
#define MPI_Type_set_name_U MPI_TYPE_SET_NAME



/* lowercase defines */
/** @def MPI_Pack_L
    Exchanges MPI_Pack_L by mpi_pack.
    It is used for the Fortran wrappers of MPI_Pack.
 */
#define MPI_Pack_L mpi_pack

/** @def MPI_Pack_external_L
    Exchanges MPI_Pack_external_L by mpi_pack_external.
    It is used for the Fortran wrappers of MPI_Pack_external.
 */
#define MPI_Pack_external_L mpi_pack_external

/** @def MPI_Pack_external_size_L
    Exchanges MPI_Pack_external_size_L by mpi_pack_external_size.
    It is used for the Fortran wrappers of MPI_Pack_external_size.
 */
#define MPI_Pack_external_size_L mpi_pack_external_size

/** @def MPI_Pack_size_L
    Exchanges MPI_Pack_size_L by mpi_pack_size.
    It is used for the Fortran wrappers of MPI_Pack_size.
 */
#define MPI_Pack_size_L mpi_pack_size

/** @def MPI_Sizeof_L
    Exchanges MPI_Sizeof_L by mpi_sizeof.
    It is used for the Fortran wrappers of MPI_Sizeof.
 */
#define MPI_Sizeof_L mpi_sizeof

/** @def MPI_Type_commit_L
    Exchanges MPI_Type_commit_L by mpi_type_commit.
    It is used for the Fortran wrappers of MPI_Type_commit.
 */
#define MPI_Type_commit_L mpi_type_commit

/** @def MPI_Type_contiguous_L
    Exchanges MPI_Type_contiguous_L by mpi_type_contiguous.
    It is used for the Fortran wrappers of MPI_Type_contiguous.
 */
#define MPI_Type_contiguous_L mpi_type_contiguous

/** @def MPI_Type_create_darray_L
    Exchanges MPI_Type_create_darray_L by mpi_type_create_darray.
    It is used for the Fortran wrappers of MPI_Type_create_darray.
 */
#define MPI_Type_create_darray_L mpi_type_create_darray

/** @def MPI_Type_create_f90_complex_L
    Exchanges MPI_Type_create_f90_complex_L by mpi_type_create_f90_complex.
    It is used for the Fortran wrappers of MPI_Type_create_f90_complex.
 */
#define MPI_Type_create_f90_complex_L mpi_type_create_f90_complex

/** @def MPI_Type_create_f90_integer_L
    Exchanges MPI_Type_create_f90_integer_L by mpi_type_create_f90_integer.
    It is used for the Fortran wrappers of MPI_Type_create_f90_integer.
 */
#define MPI_Type_create_f90_integer_L mpi_type_create_f90_integer

/** @def MPI_Type_create_f90_real_L
    Exchanges MPI_Type_create_f90_real_L by mpi_type_create_f90_real.
    It is used for the Fortran wrappers of MPI_Type_create_f90_real.
 */
#define MPI_Type_create_f90_real_L mpi_type_create_f90_real

/** @def MPI_Type_create_hindexed_L
    Exchanges MPI_Type_create_hindexed_L by mpi_type_create_hindexed.
    It is used for the Fortran wrappers of MPI_Type_create_hindexed.
 */
#define MPI_Type_create_hindexed_L mpi_type_create_hindexed

/** @def MPI_Type_create_hindexed_block_L
    Exchanges MPI_Type_create_hindexed_block_L by mpi_type_create_hindexed_block.
    It is used for the Fortran wrappers of MPI_Type_create_hindexed_block.
 */
#define MPI_Type_create_hindexed_block_L mpi_type_create_hindexed_block

/** @def MPI_Type_create_hvector_L
    Exchanges MPI_Type_create_hvector_L by mpi_type_create_hvector.
    It is used for the Fortran wrappers of MPI_Type_create_hvector.
 */
#define MPI_Type_create_hvector_L mpi_type_create_hvector

/** @def MPI_Type_create_indexed_block_L
    Exchanges MPI_Type_create_indexed_block_L by mpi_type_create_indexed_block.
    It is used for the Fortran wrappers of MPI_Type_create_indexed_block.
 */
#define MPI_Type_create_indexed_block_L mpi_type_create_indexed_block

/** @def MPI_Type_create_resized_L
    Exchanges MPI_Type_create_resized_L by mpi_type_create_resized.
    It is used for the Fortran wrappers of MPI_Type_create_resized.
 */
#define MPI_Type_create_resized_L mpi_type_create_resized

/** @def MPI_Type_create_struct_L
    Exchanges MPI_Type_create_struct_L by mpi_type_create_struct.
    It is used for the Fortran wrappers of MPI_Type_create_struct.
 */
#define MPI_Type_create_struct_L mpi_type_create_struct

/** @def MPI_Type_create_subarray_L
    Exchanges MPI_Type_create_subarray_L by mpi_type_create_subarray.
    It is used for the Fortran wrappers of MPI_Type_create_subarray.
 */
#define MPI_Type_create_subarray_L mpi_type_create_subarray

/** @def MPI_Type_dup_L
    Exchanges MPI_Type_dup_L by mpi_type_dup.
    It is used for the Fortran wrappers of MPI_Type_dup.
 */
#define MPI_Type_dup_L mpi_type_dup

/** @def MPI_Type_extent_L
    Exchanges MPI_Type_extent_L by mpi_type_extent.
    It is used for the Fortran wrappers of MPI_Type_extent.
 */
#define MPI_Type_extent_L mpi_type_extent

/** @def MPI_Type_free_L
    Exchanges MPI_Type_free_L by mpi_type_free.
    It is used for the Fortran wrappers of MPI_Type_free.
 */
#define MPI_Type_free_L mpi_type_free

/** @def MPI_Type_get_contents_L
    Exchanges MPI_Type_get_contents_L by mpi_type_get_contents.
    It is used for the Fortran wrappers of MPI_Type_get_contents.
 */
#define MPI_Type_get_contents_L mpi_type_get_contents

/** @def MPI_Type_get_envelope_L
    Exchanges MPI_Type_get_envelope_L by mpi_type_get_envelope.
    It is used for the Fortran wrappers of MPI_Type_get_envelope.
 */
#define MPI_Type_get_envelope_L mpi_type_get_envelope

/** @def MPI_Type_get_extent_L
    Exchanges MPI_Type_get_extent_L by mpi_type_get_extent.
    It is used for the Fortran wrappers of MPI_Type_get_extent.
 */
#define MPI_Type_get_extent_L mpi_type_get_extent

/** @def MPI_Type_get_extent_x_L
    Exchanges MPI_Type_get_extent_x_L by mpi_type_get_extent_x.
    It is used for the Fortran wrappers of MPI_Type_get_extent_x.
 */
#define MPI_Type_get_extent_x_L mpi_type_get_extent_x

/** @def MPI_Type_get_true_extent_L
    Exchanges MPI_Type_get_true_extent_L by mpi_type_get_true_extent.
    It is used for the Fortran wrappers of MPI_Type_get_true_extent.
 */
#define MPI_Type_get_true_extent_L mpi_type_get_true_extent

/** @def MPI_Type_get_true_extent_x_L
    Exchanges MPI_Type_get_true_extent_x_L by mpi_type_get_true_extent_x.
    It is used for the Fortran wrappers of MPI_Type_get_true_extent_x.
 */
#define MPI_Type_get_true_extent_x_L mpi_type_get_true_extent_x

/** @def MPI_Type_hindexed_L
    Exchanges MPI_Type_hindexed_L by mpi_type_hindexed.
    It is used for the Fortran wrappers of MPI_Type_hindexed.
 */
#define MPI_Type_hindexed_L mpi_type_hindexed

/** @def MPI_Type_hvector_L
    Exchanges MPI_Type_hvector_L by mpi_type_hvector.
    It is used for the Fortran wrappers of MPI_Type_hvector.
 */
#define MPI_Type_hvector_L mpi_type_hvector

/** @def MPI_Type_indexed_L
    Exchanges MPI_Type_indexed_L by mpi_type_indexed.
    It is used for the Fortran wrappers of MPI_Type_indexed.
 */
#define MPI_Type_indexed_L mpi_type_indexed

/** @def MPI_Type_lb_L
    Exchanges MPI_Type_lb_L by mpi_type_lb.
    It is used for the Fortran wrappers of MPI_Type_lb.
 */
#define MPI_Type_lb_L mpi_type_lb

/** @def MPI_Type_match_size_L
    Exchanges MPI_Type_match_size_L by mpi_type_match_size.
    It is used for the Fortran wrappers of MPI_Type_match_size.
 */
#define MPI_Type_match_size_L mpi_type_match_size

/** @def MPI_Type_size_L
    Exchanges MPI_Type_size_L by mpi_type_size.
    It is used for the Fortran wrappers of MPI_Type_size.
 */
#define MPI_Type_size_L mpi_type_size

/** @def MPI_Type_size_x_L
    Exchanges MPI_Type_size_x_L by mpi_type_size_x.
    It is used for the Fortran wrappers of MPI_Type_size_x.
 */
#define MPI_Type_size_x_L mpi_type_size_x

/** @def MPI_Type_struct_L
    Exchanges MPI_Type_struct_L by mpi_type_struct.
    It is used for the Fortran wrappers of MPI_Type_struct.
 */
#define MPI_Type_struct_L mpi_type_struct

/** @def MPI_Type_ub_L
    Exchanges MPI_Type_ub_L by mpi_type_ub.
    It is used for the Fortran wrappers of MPI_Type_ub.
 */
#define MPI_Type_ub_L mpi_type_ub

/** @def MPI_Type_vector_L
    Exchanges MPI_Type_vector_L by mpi_type_vector.
    It is used for the Fortran wrappers of MPI_Type_vector.
 */
#define MPI_Type_vector_L mpi_type_vector

/** @def MPI_Unpack_L
    Exchanges MPI_Unpack_L by mpi_unpack.
    It is used for the Fortran wrappers of MPI_Unpack.
 */
#define MPI_Unpack_L mpi_unpack

/** @def MPI_Unpack_external_L
    Exchanges MPI_Unpack_external_L by mpi_unpack_external.
    It is used for the Fortran wrappers of MPI_Unpack_external.
 */
#define MPI_Unpack_external_L mpi_unpack_external


/** @def MPI_Type_create_keyval_L
    Exchanges MPI_Type_create_keyval_L by mpi_type_create_keyval.
    It is used for the Fortran wrappers of MPI_Type_create_keyval.
 */
#define MPI_Type_create_keyval_L mpi_type_create_keyval

/** @def MPI_Type_delete_attr_L
    Exchanges MPI_Type_delete_attr_L by mpi_type_delete_attr.
    It is used for the Fortran wrappers of MPI_Type_delete_attr.
 */
#define MPI_Type_delete_attr_L mpi_type_delete_attr

/** @def MPI_Type_free_keyval_L
    Exchanges MPI_Type_free_keyval_L by mpi_type_free_keyval.
    It is used for the Fortran wrappers of MPI_Type_free_keyval.
 */
#define MPI_Type_free_keyval_L mpi_type_free_keyval

/** @def MPI_Type_get_attr_L
    Exchanges MPI_Type_get_attr_L by mpi_type_get_attr.
    It is used for the Fortran wrappers of MPI_Type_get_attr.
 */
#define MPI_Type_get_attr_L mpi_type_get_attr

/** @def MPI_Type_get_name_L
    Exchanges MPI_Type_get_name_L by mpi_type_get_name.
    It is used for the Fortran wrappers of MPI_Type_get_name.
 */
#define MPI_Type_get_name_L mpi_type_get_name

/** @def MPI_Type_set_attr_L
    Exchanges MPI_Type_set_attr_L by mpi_type_set_attr.
    It is used for the Fortran wrappers of MPI_Type_set_attr.
 */
#define MPI_Type_set_attr_L mpi_type_set_attr

/** @def MPI_Type_set_name_L
    Exchanges MPI_Type_set_name_L by mpi_type_set_name.
    It is used for the Fortran wrappers of MPI_Type_set_name.
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

#if HAVE( DECL_PMPI_PACK ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( MPI_Pack )
/**
 * Measurement wrapper for MPI_Pack
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup type
 */
void
FSUB( MPI_Pack )( void* inbuf, int* incount, MPI_Datatype* datatype, void* outbuf, int* outsize, int* position, MPI_Comm* comm, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    #if HAVE( MPI_BOTTOM )
    if ( inbuf == scorep_mpi_fortran_bottom )
    {
        inbuf = MPI_BOTTOM;
    }
    #endif


    *ierr = MPI_Pack( inbuf, *incount, *datatype, outbuf, *outsize, position, *comm );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_PACK_EXTERNAL ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( MPI_Pack_external )
/**
 * Measurement wrapper for MPI_Pack_external
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup type
 */
void
FSUB( MPI_Pack_external )( char* datarep, void* inbuf, int* incount, MPI_Datatype* datatype, void* outbuf, MPI_Aint* outsize, MPI_Aint* position, int* ierr, scorep_fortran_charlen_t datarep_len )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    char* c_datarep = NULL;
    c_datarep = scorep_mpi_f2c_string( datarep, datarep_len );



    #if HAVE( MPI_BOTTOM )
    if ( inbuf == scorep_mpi_fortran_bottom )
    {
        inbuf = MPI_BOTTOM;
    }
    #endif


    *ierr = MPI_Pack_external( c_datarep, inbuf, *incount, *datatype, outbuf, *outsize, position );

    free( c_datarep );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_PACK_EXTERNAL_SIZE ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( MPI_Pack_external_size )
/**
 * Measurement wrapper for MPI_Pack_external_size
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup type
 */
void
FSUB( MPI_Pack_external_size )( char* datarep, int* incount, MPI_Datatype* datatype, MPI_Aint* size, int* ierr, scorep_fortran_charlen_t datarep_len )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    char* c_datarep = NULL;
    c_datarep = scorep_mpi_f2c_string( datarep, datarep_len );



    *ierr = MPI_Pack_external_size( c_datarep, *incount, *datatype, size );

    free( c_datarep );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_PACK_SIZE ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( MPI_Pack_size )
/**
 * Measurement wrapper for MPI_Pack_size
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup type
 */
void
FSUB( MPI_Pack_size )( int* incount, MPI_Datatype* datatype, MPI_Comm* comm, int* size, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Pack_size( *incount, *datatype, *comm, size );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_SIZEOF ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( MPI_Sizeof )
/**
 * Measurement wrapper for MPI_Sizeof
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup type
 */
void
FSUB( MPI_Sizeof )( MPI_Datatype* x, int* size, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Sizeof( *x, *size );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_TYPE_COMMIT ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( MPI_Type_commit )
/**
 * Measurement wrapper for MPI_Type_commit
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup type
 */
void
FSUB( MPI_Type_commit )( MPI_Datatype* datatype, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Type_commit( datatype );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_TYPE_CONTIGUOUS ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( MPI_Type_contiguous )
/**
 * Measurement wrapper for MPI_Type_contiguous
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup type
 */
void
FSUB( MPI_Type_contiguous )( int* count, MPI_Datatype* oldtype, MPI_Datatype* newtype, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Type_contiguous( *count, *oldtype, newtype );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_TYPE_CREATE_DARRAY ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( MPI_Type_create_darray )
/**
 * Measurement wrapper for MPI_Type_create_darray
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup type
 */
void
FSUB( MPI_Type_create_darray )( int* size, int* rank, int* ndims, int array_of_gsizes[], int array_of_distribs[], int array_of_dargs[], int array_of_psizes[], int* order, MPI_Datatype* oldtype, MPI_Datatype* newtype, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Type_create_darray( *size, *rank, *ndims, array_of_gsizes, array_of_distribs, array_of_dargs, array_of_psizes, *order, *oldtype, newtype );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_TYPE_CREATE_HINDEXED ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( MPI_Type_create_hindexed )
/**
 * Measurement wrapper for MPI_Type_create_hindexed
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup type
 */
void
FSUB( MPI_Type_create_hindexed )( int* count, int array_of_blocklengths[], MPI_Aint array_of_displacements[], MPI_Datatype* oldtype, MPI_Datatype* newtype, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Type_create_hindexed( *count, array_of_blocklengths, array_of_displacements, *oldtype, newtype );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_TYPE_CREATE_HINDEXED_BLOCK ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( MPI_Type_create_hindexed_block )
/**
 * Measurement wrapper for MPI_Type_create_hindexed_block
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-3
 * @ingroup type
 */
void
FSUB( MPI_Type_create_hindexed_block )( int* count, int* blocklength, MPI_Aint array_of_displacements[], MPI_Datatype* oldtype, MPI_Datatype* newtype, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Type_create_hindexed_block( *count, *blocklength, array_of_displacements, *oldtype, newtype );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_TYPE_CREATE_HVECTOR ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( MPI_Type_create_hvector )
/**
 * Measurement wrapper for MPI_Type_create_hvector
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup type
 */
void
FSUB( MPI_Type_create_hvector )( int* count, int* blocklength, MPI_Aint* stride, MPI_Datatype* oldtype, MPI_Datatype* newtype, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Type_create_hvector( *count, *blocklength, *stride, *oldtype, newtype );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_TYPE_CREATE_INDEXED_BLOCK ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( MPI_Type_create_indexed_block )
/**
 * Measurement wrapper for MPI_Type_create_indexed_block
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup type
 */
void
FSUB( MPI_Type_create_indexed_block )( int* count, int* blocklength, int array_of_displacements[], MPI_Datatype* oldtype, MPI_Datatype* newtype, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Type_create_indexed_block( *count, *blocklength, array_of_displacements, *oldtype, newtype );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_TYPE_CREATE_RESIZED ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( MPI_Type_create_resized )
/**
 * Measurement wrapper for MPI_Type_create_resized
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup type
 */
void
FSUB( MPI_Type_create_resized )( MPI_Datatype* oldtype, MPI_Aint* lb, MPI_Aint* extent, MPI_Datatype* newtype, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Type_create_resized( *oldtype, *lb, *extent, newtype );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_TYPE_CREATE_STRUCT ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( MPI_Type_create_struct )
/**
 * Measurement wrapper for MPI_Type_create_struct
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup type
 */
void
FSUB( MPI_Type_create_struct )( int* count, int array_of_blocklengths[], MPI_Aint array_of_displacements[], MPI_Datatype array_of_types[], MPI_Datatype* newtype, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Type_create_struct( *count, array_of_blocklengths, array_of_displacements, array_of_types, newtype );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_TYPE_CREATE_SUBARRAY ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( MPI_Type_create_subarray )
/**
 * Measurement wrapper for MPI_Type_create_subarray
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup type
 */
void
FSUB( MPI_Type_create_subarray )( int* ndims, int array_of_sizes[], int array_of_subsizes[], int array_of_starts[], int* order, MPI_Datatype* oldtype, MPI_Datatype* newtype, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Type_create_subarray( *ndims, array_of_sizes, array_of_subsizes, array_of_starts, *order, *oldtype, newtype );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_TYPE_DUP ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( MPI_Type_dup )
/**
 * Measurement wrapper for MPI_Type_dup
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup type
 */
void
FSUB( MPI_Type_dup )( MPI_Datatype* type, MPI_Datatype* newtype, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Type_dup( *type, newtype );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_TYPE_EXTENT ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( MPI_Type_extent )
/**
 * Measurement wrapper for MPI_Type_extent
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup type
 */
void
FSUB( MPI_Type_extent )( MPI_Datatype* datatype, int* extent, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Aint c_extent;



    *ierr = MPI_Type_extent( *datatype, &c_extent );

    *extent = ( int )c_extent;
    if ( *extent != c_extent )
    {
        UTILS_DEBUG_PRINTF( SCOREP_DEBUG_MPI, "Value truncated in \"MPI_Type_extent\". Function is deprecated due to mismatching parameter types! Consult the MPI Standard for more details." );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_TYPE_FREE ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( MPI_Type_free )
/**
 * Measurement wrapper for MPI_Type_free
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup type
 */
void
FSUB( MPI_Type_free )( MPI_Datatype* datatype, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Type_free( datatype );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_TYPE_GET_CONTENTS ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( MPI_Type_get_contents )
/**
 * Measurement wrapper for MPI_Type_get_contents
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup type
 */
void
FSUB( MPI_Type_get_contents )( MPI_Datatype* datatype, int* max_integers, int* max_addresses, int* max_datatypes, int array_of_integers[], MPI_Aint array_of_addresses[], MPI_Datatype array_of_datatypes[], int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Type_get_contents( *datatype, *max_integers, *max_addresses, *max_datatypes, array_of_integers, array_of_addresses, array_of_datatypes );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_TYPE_GET_ENVELOPE ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( MPI_Type_get_envelope )
/**
 * Measurement wrapper for MPI_Type_get_envelope
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup type
 */
void
FSUB( MPI_Type_get_envelope )( MPI_Datatype* datatype, int* num_integers, int* num_addresses, int* num_datatypes, int* combiner, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Type_get_envelope( *datatype, num_integers, num_addresses, num_datatypes, combiner );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_TYPE_GET_EXTENT ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( MPI_Type_get_extent )
/**
 * Measurement wrapper for MPI_Type_get_extent
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup type
 */
void
FSUB( MPI_Type_get_extent )( MPI_Datatype* datatype, MPI_Aint* lb, MPI_Aint* extent, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Type_get_extent( *datatype, lb, extent );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_TYPE_GET_EXTENT_X ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( MPI_Type_get_extent_x )
/**
 * Measurement wrapper for MPI_Type_get_extent_x
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-3
 * @ingroup type
 */
void
FSUB( MPI_Type_get_extent_x )( MPI_Datatype* datatype, MPI_Count* lb, MPI_Count* extent, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Type_get_extent_x( *datatype, lb, extent );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_TYPE_GET_TRUE_EXTENT ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( MPI_Type_get_true_extent )
/**
 * Measurement wrapper for MPI_Type_get_true_extent
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup type
 */
void
FSUB( MPI_Type_get_true_extent )( MPI_Datatype* datatype, MPI_Aint* true_lb, MPI_Aint* true_extent, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Type_get_true_extent( *datatype, true_lb, true_extent );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_TYPE_GET_TRUE_EXTENT_X ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( MPI_Type_get_true_extent_x )
/**
 * Measurement wrapper for MPI_Type_get_true_extent_x
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-3
 * @ingroup type
 */
void
FSUB( MPI_Type_get_true_extent_x )( MPI_Datatype* datatype, MPI_Count* true_lb, MPI_Count* true_extent, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Type_get_true_extent_x( *datatype, true_lb, true_extent );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_TYPE_HINDEXED ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( MPI_Type_hindexed )
/**
 * Measurement wrapper for MPI_Type_hindexed
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup type
 */
void
FSUB( MPI_Type_hindexed )( int* count, int* array_of_blocklengths, int* array_of_displacements, MPI_Datatype* oldtype, MPI_Datatype* newtype, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Aint* c_array_of_displacements;

    c_array_of_displacements = ( MPI_Aint* )malloc( *count * sizeof( MPI_Aint ) );
    for ( int i = 0; i < *count; ++i )
    {
        c_array_of_displacements[ i ] = array_of_displacements[ i ];
    }



    *ierr = MPI_Type_hindexed( *count, array_of_blocklengths, c_array_of_displacements, *oldtype, newtype );

    free( c_array_of_displacements );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_TYPE_HVECTOR ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( MPI_Type_hvector )
/**
 * Measurement wrapper for MPI_Type_hvector
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup type
 */
void
FSUB( MPI_Type_hvector )( int* count, int* blocklength, int* stride, MPI_Datatype* oldtype, MPI_Datatype* newtype, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Aint c_stride = *stride;



    *ierr = MPI_Type_hvector( *count, *blocklength, *&c_stride, *oldtype, newtype );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_TYPE_INDEXED ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( MPI_Type_indexed )
/**
 * Measurement wrapper for MPI_Type_indexed
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup type
 */
void
FSUB( MPI_Type_indexed )( int* count, int* array_of_blocklengths, int* array_of_displacements, MPI_Datatype* oldtype, MPI_Datatype* newtype, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Type_indexed( *count, array_of_blocklengths, array_of_displacements, *oldtype, newtype );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_TYPE_LB ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( MPI_Type_lb )
/**
 * Measurement wrapper for MPI_Type_lb
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup type
 */
void
FSUB( MPI_Type_lb )( MPI_Datatype* datatype, int* displacement, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Aint c_displacement;



    *ierr = MPI_Type_lb( *datatype, &c_displacement );

    *displacement = ( int )c_displacement;
    if ( *displacement != c_displacement )
    {
        UTILS_DEBUG_PRINTF( SCOREP_DEBUG_MPI, "Value truncated in \"MPI_Type_lb\". Function is deprecated due to mismatching parameter types! Consult the MPI Standard for more details." );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_TYPE_MATCH_SIZE ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( MPI_Type_match_size )
/**
 * Measurement wrapper for MPI_Type_match_size
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup type
 */
void
FSUB( MPI_Type_match_size )( int* typeclass, int* size, MPI_Datatype* type, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Type_match_size( *typeclass, *size, type );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_TYPE_SIZE ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( MPI_Type_size )
/**
 * Measurement wrapper for MPI_Type_size
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup type
 */
void
FSUB( MPI_Type_size )( MPI_Datatype* datatype, int* size, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Type_size( *datatype, size );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_TYPE_SIZE_X ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( MPI_Type_size_x )
/**
 * Measurement wrapper for MPI_Type_size_x
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-3
 * @ingroup type
 */
void
FSUB( MPI_Type_size_x )( MPI_Datatype* datatype, MPI_Count* size, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Type_size_x( *datatype, size );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_TYPE_STRUCT ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( MPI_Type_struct )
/**
 * Measurement wrapper for MPI_Type_struct
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup type
 */
void
FSUB( MPI_Type_struct )( int* count, int* array_of_blocklengths, int* array_of_displacements, MPI_Datatype* array_of_types, MPI_Datatype* newtype, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Aint* c_array_of_displacements;

    c_array_of_displacements = ( MPI_Aint* )malloc( *count * sizeof( MPI_Aint ) );
    for ( int i = 0; i < *count; ++i )
    {
        c_array_of_displacements[ i ] = array_of_displacements[ i ];
    }



    *ierr = MPI_Type_struct( *count, array_of_blocklengths, c_array_of_displacements, array_of_types, newtype );

    free( c_array_of_displacements );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_TYPE_UB ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( MPI_Type_ub )
/**
 * Measurement wrapper for MPI_Type_ub
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup type
 */
void
FSUB( MPI_Type_ub )( MPI_Datatype* datatype, int* displacement, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Aint c_displacement;



    *ierr = MPI_Type_ub( *datatype, &c_displacement );

    *displacement = ( int )c_displacement;
    if ( *displacement != c_displacement )
    {
        UTILS_DEBUG_PRINTF( SCOREP_DEBUG_MPI, "Value truncated in \"MPI_Type_ub\". Function is deprecated due to mismatching parameter types! Consult the MPI Standard for more details." );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_TYPE_VECTOR ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( MPI_Type_vector )
/**
 * Measurement wrapper for MPI_Type_vector
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup type
 */
void
FSUB( MPI_Type_vector )( int* count, int* blocklength, int* stride, MPI_Datatype* oldtype, MPI_Datatype* newtype, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Type_vector( *count, *blocklength, *stride, *oldtype, newtype );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_UNPACK ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( MPI_Unpack )
/**
 * Measurement wrapper for MPI_Unpack
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup type
 */
void
FSUB( MPI_Unpack )( void* inbuf, int* insize, int* position, void* outbuf, int* outcount, MPI_Datatype* datatype, MPI_Comm* comm, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    #if HAVE( MPI_BOTTOM )
    if ( outbuf == scorep_mpi_fortran_bottom )
    {
        outbuf = MPI_BOTTOM;
    }
    #endif


    *ierr = MPI_Unpack( inbuf, *insize, position, outbuf, *outcount, *datatype, *comm );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_UNPACK_EXTERNAL ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( MPI_Unpack_external )
/**
 * Measurement wrapper for MPI_Unpack_external
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup type
 */
void
FSUB( MPI_Unpack_external )( char* datarep, void* inbuf, MPI_Aint* insize, MPI_Aint* position, void* outbuf, int* outcount, MPI_Datatype* datatype, int* ierr, scorep_fortran_charlen_t datarep_len )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    char* c_datarep = NULL;
    c_datarep = scorep_mpi_f2c_string( datarep, datarep_len );



    #if HAVE( MPI_BOTTOM )
    if ( outbuf == scorep_mpi_fortran_bottom )
    {
        outbuf = MPI_BOTTOM;
    }
    #endif


    *ierr = MPI_Unpack_external( c_datarep, inbuf, *insize, position, outbuf, *outcount, *datatype );

    free( c_datarep );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif

#if HAVE( MPI_TYPE_CREATE_F90_COMPLEX_COMPLIANT )
#if HAVE( DECL_PMPI_TYPE_CREATE_F90_COMPLEX ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( MPI_Type_create_f90_complex )
/**
 * Measurement wrapper for MPI_Type_create_f90_complex
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_StdCompl.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup type
 */
void
FSUB( MPI_Type_create_f90_complex )( int* p, int* r, MPI_Datatype* newtype, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Type_create_f90_complex( *p, *r, newtype );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#endif

#if HAVE( MPI_TYPE_CREATE_F90_INTEGER_COMPLIANT )
#if HAVE( DECL_PMPI_TYPE_CREATE_F90_INTEGER ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( MPI_Type_create_f90_integer )
/**
 * Measurement wrapper for MPI_Type_create_f90_integer
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_StdCompl.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup type
 */
void
FSUB( MPI_Type_create_f90_integer )( int* r, MPI_Datatype* newtype, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Type_create_f90_integer( *r, newtype );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#endif

#if HAVE( MPI_TYPE_CREATE_F90_REAL_COMPLIANT )
#if HAVE( DECL_PMPI_TYPE_CREATE_F90_REAL ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( MPI_Type_create_f90_real )
/**
 * Measurement wrapper for MPI_Type_create_f90_real
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_StdCompl.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup type
 */
void
FSUB( MPI_Type_create_f90_real )( int* p, int* r, MPI_Datatype* newtype, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Type_create_f90_real( *p, *r, newtype );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#endif


/**
 * @}
 * @name Fortran wrappers for external interface functions
 * @{
 */

#if HAVE( DECL_PMPI_TYPE_CREATE_KEYVAL ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( SCOREP_MPI_NO_EXT ) && !defined( MPI_Type_create_keyval )
/**
 * Measurement wrapper for MPI_Type_create_keyval
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup type_ext
 */
void
FSUB( MPI_Type_create_keyval )( MPI_Type_copy_attr_function* type_copy_attr_fn, MPI_Type_delete_attr_function* type_delete_attr_fn, int* type_keyval, void* extra_state, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Type_create_keyval( type_copy_attr_fn, type_delete_attr_fn, type_keyval, extra_state );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_TYPE_DELETE_ATTR ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( SCOREP_MPI_NO_EXT ) && !defined( MPI_Type_delete_attr )
/**
 * Measurement wrapper for MPI_Type_delete_attr
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup type_ext
 */
void
FSUB( MPI_Type_delete_attr )( MPI_Datatype* type, int* type_keyval, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Type_delete_attr( *type, *type_keyval );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_TYPE_FREE_KEYVAL ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( SCOREP_MPI_NO_EXT ) && !defined( MPI_Type_free_keyval )
/**
 * Measurement wrapper for MPI_Type_free_keyval
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup type_ext
 */
void
FSUB( MPI_Type_free_keyval )( int* type_keyval, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Type_free_keyval( type_keyval );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_TYPE_GET_ATTR ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( SCOREP_MPI_NO_EXT ) && !defined( MPI_Type_get_attr )
/**
 * Measurement wrapper for MPI_Type_get_attr
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup type_ext
 */
void
FSUB( MPI_Type_get_attr )( MPI_Datatype* type, int* type_keyval, void* attribute_val, int* flag, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Type_get_attr( *type, *type_keyval, attribute_val, flag );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_TYPE_GET_NAME ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( SCOREP_MPI_NO_EXT ) && !defined( MPI_Type_get_name )
/**
 * Measurement wrapper for MPI_Type_get_name
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup type_ext
 */
void
FSUB( MPI_Type_get_name )( MPI_Datatype* type, char* type_name, int* resultlen, int* ierr, scorep_fortran_charlen_t type_name_len )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    char*  c_type_name     = NULL;
    size_t c_type_name_len = 0;
    c_type_name = ( char* )malloc( ( type_name_len + 1 ) * sizeof( char ) );
    if ( !c_type_name )
    {
        exit( EXIT_FAILURE );
    }



    *ierr = MPI_Type_get_name( *type, c_type_name, resultlen );


    c_type_name_len = strlen( c_type_name );
    strncpy( type_name, c_type_name, c_type_name_len );
    memset( type_name + c_type_name_len, ' ', type_name_len - c_type_name_len );
    free( c_type_name );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_TYPE_SET_ATTR ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( SCOREP_MPI_NO_EXT ) && !defined( MPI_Type_set_attr )
/**
 * Measurement wrapper for MPI_Type_set_attr
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup type_ext
 */
void
FSUB( MPI_Type_set_attr )( MPI_Datatype* type, int* type_keyval, void* attribute_val, int* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Type_set_attr( *type, *type_keyval, attribute_val );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_TYPE_SET_NAME ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( SCOREP_MPI_NO_EXT ) && !defined( MPI_Type_set_name )
/**
 * Measurement wrapper for MPI_Type_set_name
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup type_ext
 */
void
FSUB( MPI_Type_set_name )( MPI_Datatype* type, char* type_name, int* ierr, scorep_fortran_charlen_t type_name_len )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    char* c_type_name = NULL;
    c_type_name = scorep_mpi_f2c_string( type_name, type_name_len );



    *ierr = MPI_Type_set_name( *type, c_type_name );

    free( c_type_name );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif

/**
 * @}
 * @name Fortran wrappers for handle conversion functions
 * @{
 */



#else /* !NEED_F2C_CONV */

#if HAVE( DECL_PMPI_SIZEOF ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( MPI_Sizeof )
/**
 * Measurement wrapper for MPI_Sizeof
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup type
 * For the order of events see @ref MPI_Sizeof
 */
void
FSUB( MPI_Sizeof )( MPI_Fint* x, MPI_Fint* size, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Sizeof( PMPI_Type_f2c( *x ), *size );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif

#if HAVE( DECL_PMPI_TYPE_CREATE_STRUCT ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE )
/**
 * Manual measurement wrapper for MPI_Type_create_struct
 * @ingroup manual_wrapper
 * @ingroup interface_fortran
 * @ingroup mpi_version_2
 * @ingroup mpi_enabled_type
 */
void
FSUB( MPI_Type_create_struct )( MPI_Fint* count,
                                MPI_Fint* array_of_blocklengths,
                                MPI_Aint  array_of_displacements[],
                                MPI_Fint* array_of_types,
                                MPI_Fint* newtype,
                                MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
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
    *ierr = MPI_Type_create_struct( *count, array_of_blocklengths,
                                    array_of_displacements,
                                    c_array_of_types, &c_newtype );
    *newtype = PMPI_Type_c2f( c_newtype );

    free( c_array_of_types );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif

#if HAVE( DECL_PMPI_TYPE_GET_CONTENTS ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE )
/**
 * Manual measurement wrapper for MPI_Type_get_contents
 * @ingroup manual_wrapper
 * @ingroup interface_fortran
 * @ingroup mpi_version_2
 * @ingroup mpi_enabled_type
 */
void
FSUB( MPI_Type_get_contents )( MPI_Fint* datatype,
                               MPI_Fint* max_integers,
                               MPI_Fint* max_addresses,
                               MPI_Fint* max_datatypes,
                               MPI_Fint* array_of_integers,
                               MPI_Aint* array_of_addresses,
                               MPI_Fint* array_of_datatypes,
                               MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
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
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif

#if HAVE( DECL_PMPI_PACK ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( MPI_Pack )
/**
 * Measurement wrapper for MPI_Pack
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup type
 * For the order of events see @ref MPI_Pack
 */
void
FSUB( MPI_Pack )( void* inbuf, MPI_Fint* incount, MPI_Fint* datatype, void* outbuf, MPI_Fint* outsize, MPI_Fint* position, MPI_Fint* comm, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    #if HAVE( MPI_BOTTOM )
    if ( inbuf == scorep_mpi_fortran_bottom )
    {
        inbuf = MPI_BOTTOM;
    }
    #endif


    *ierr = MPI_Pack( inbuf, *incount, PMPI_Type_f2c( *datatype ), outbuf, *outsize, position, PMPI_Comm_f2c( *comm ) );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_PACK_EXTERNAL ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( MPI_Pack_external )
/**
 * Measurement wrapper for MPI_Pack_external
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup type
 * For the order of events see @ref MPI_Pack_external
 */
void
FSUB( MPI_Pack_external )( char* datarep, void* inbuf, MPI_Fint* incount, MPI_Fint* datatype, void* outbuf, MPI_Aint* outsize, MPI_Aint* position, MPI_Fint* ierr, scorep_fortran_charlen_t datarep_len )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    char* c_datarep = NULL;
    c_datarep = scorep_mpi_f2c_string( datarep, datarep_len );



    #if HAVE( MPI_BOTTOM )
    if ( inbuf == scorep_mpi_fortran_bottom )
    {
        inbuf = MPI_BOTTOM;
    }
    #endif


    *ierr = MPI_Pack_external( c_datarep, inbuf, *incount, PMPI_Type_f2c( *datatype ), outbuf, *outsize, position );

    free( c_datarep );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_PACK_EXTERNAL_SIZE ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( MPI_Pack_external_size )
/**
 * Measurement wrapper for MPI_Pack_external_size
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup type
 * For the order of events see @ref MPI_Pack_external_size
 */
void
FSUB( MPI_Pack_external_size )( char* datarep, MPI_Fint* incount, MPI_Fint* datatype, MPI_Aint* size, MPI_Fint* ierr, scorep_fortran_charlen_t datarep_len )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    char* c_datarep = NULL;
    c_datarep = scorep_mpi_f2c_string( datarep, datarep_len );



    *ierr = MPI_Pack_external_size( c_datarep, *incount, PMPI_Type_f2c( *datatype ), size );

    free( c_datarep );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_PACK_SIZE ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( MPI_Pack_size )
/**
 * Measurement wrapper for MPI_Pack_size
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup type
 * For the order of events see @ref MPI_Pack_size
 */
void
FSUB( MPI_Pack_size )( MPI_Fint* incount, MPI_Fint* datatype, MPI_Fint* comm, MPI_Fint* size, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Pack_size( *incount, PMPI_Type_f2c( *datatype ), PMPI_Comm_f2c( *comm ), size );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_SIZEOF ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( MPI_Sizeof )
/**
 * Measurement wrapper for MPI_Sizeof
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup type
 * For the order of events see @ref MPI_Sizeof
 */
void
FSUB( MPI_Sizeof )( MPI_Fint* x, MPI_Fint* size, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Sizeof( PMPI_Type_f2c( *x ), *size );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_TYPE_COMMIT ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( MPI_Type_commit )
/**
 * Measurement wrapper for MPI_Type_commit
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup type
 * For the order of events see @ref MPI_Type_commit
 */
void
FSUB( MPI_Type_commit )( MPI_Fint* datatype, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Datatype c_datatype = PMPI_Type_f2c( *datatype );


    *ierr = MPI_Type_commit( &c_datatype );

    *datatype = PMPI_Type_c2f( c_datatype );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_TYPE_CONTIGUOUS ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( MPI_Type_contiguous )
/**
 * Measurement wrapper for MPI_Type_contiguous
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup type
 * For the order of events see @ref MPI_Type_contiguous
 */
void
FSUB( MPI_Type_contiguous )( MPI_Fint* count, MPI_Fint* oldtype, MPI_Fint* newtype, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Datatype c_newtype;


    *ierr = MPI_Type_contiguous( *count, PMPI_Type_f2c( *oldtype ), &c_newtype );

    *newtype = PMPI_Type_c2f( c_newtype );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_TYPE_CREATE_DARRAY ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( MPI_Type_create_darray )
/**
 * Measurement wrapper for MPI_Type_create_darray
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup type
 * For the order of events see @ref MPI_Type_create_darray
 */
void
FSUB( MPI_Type_create_darray )( MPI_Fint* size, MPI_Fint* rank, MPI_Fint* ndims, MPI_Fint* array_of_gsizes, MPI_Fint* array_of_distribs, MPI_Fint* array_of_dargs, MPI_Fint* array_of_psizes, MPI_Fint* order, MPI_Fint* oldtype, MPI_Fint* newtype, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Datatype c_newtype;


    *ierr = MPI_Type_create_darray( *size, *rank, *ndims, array_of_gsizes, array_of_distribs, array_of_dargs, array_of_psizes, *order, PMPI_Type_f2c( *oldtype ), &c_newtype );

    *newtype = PMPI_Type_c2f( c_newtype );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_TYPE_CREATE_HINDEXED ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( MPI_Type_create_hindexed )
/**
 * Measurement wrapper for MPI_Type_create_hindexed
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup type
 * For the order of events see @ref MPI_Type_create_hindexed
 */
void
FSUB( MPI_Type_create_hindexed )( MPI_Fint* count, MPI_Fint* array_of_blocklengths, MPI_Aint array_of_displacements[], MPI_Fint* oldtype, MPI_Fint* newtype, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Datatype c_newtype;


    *ierr = MPI_Type_create_hindexed( *count, array_of_blocklengths, array_of_displacements, PMPI_Type_f2c( *oldtype ), &c_newtype );

    *newtype = PMPI_Type_c2f( c_newtype );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_TYPE_CREATE_HINDEXED_BLOCK ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( MPI_Type_create_hindexed_block )
/**
 * Measurement wrapper for MPI_Type_create_hindexed_block
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-3
 * @ingroup type
 * For the order of events see @ref MPI_Type_create_hindexed_block
 */
void
FSUB( MPI_Type_create_hindexed_block )( MPI_Fint* count, MPI_Fint* blocklength, MPI_Aint array_of_displacements[], MPI_Fint* oldtype, MPI_Fint* newtype, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Datatype c_newtype;


    *ierr = MPI_Type_create_hindexed_block( *count, *blocklength, array_of_displacements, PMPI_Type_f2c( *oldtype ), &c_newtype );

    *newtype = PMPI_Type_c2f( c_newtype );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_TYPE_CREATE_HVECTOR ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( MPI_Type_create_hvector )
/**
 * Measurement wrapper for MPI_Type_create_hvector
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup type
 * For the order of events see @ref MPI_Type_create_hvector
 */
void
FSUB( MPI_Type_create_hvector )( MPI_Fint* count, MPI_Fint* blocklength, MPI_Aint* stride, MPI_Fint* oldtype, MPI_Fint* newtype, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Datatype c_newtype;


    *ierr = MPI_Type_create_hvector( *count, *blocklength, *stride, PMPI_Type_f2c( *oldtype ), &c_newtype );

    *newtype = PMPI_Type_c2f( c_newtype );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_TYPE_CREATE_INDEXED_BLOCK ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( MPI_Type_create_indexed_block )
/**
 * Measurement wrapper for MPI_Type_create_indexed_block
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup type
 * For the order of events see @ref MPI_Type_create_indexed_block
 */
void
FSUB( MPI_Type_create_indexed_block )( MPI_Fint* count, MPI_Fint* blocklength, MPI_Fint* array_of_displacements, MPI_Fint* oldtype, MPI_Fint* newtype, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Datatype c_newtype;


    *ierr = MPI_Type_create_indexed_block( *count, *blocklength, array_of_displacements, PMPI_Type_f2c( *oldtype ), &c_newtype );

    *newtype = PMPI_Type_c2f( c_newtype );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_TYPE_CREATE_RESIZED ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( MPI_Type_create_resized )
/**
 * Measurement wrapper for MPI_Type_create_resized
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup type
 * For the order of events see @ref MPI_Type_create_resized
 */
void
FSUB( MPI_Type_create_resized )( MPI_Fint* oldtype, MPI_Aint* lb, MPI_Aint* extent, MPI_Fint* newtype, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Datatype c_newtype;


    *ierr = MPI_Type_create_resized( PMPI_Type_f2c( *oldtype ), *lb, *extent, &c_newtype );

    *newtype = PMPI_Type_c2f( c_newtype );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_TYPE_CREATE_SUBARRAY ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( MPI_Type_create_subarray )
/**
 * Measurement wrapper for MPI_Type_create_subarray
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup type
 * For the order of events see @ref MPI_Type_create_subarray
 */
void
FSUB( MPI_Type_create_subarray )( MPI_Fint* ndims, MPI_Fint* array_of_sizes, MPI_Fint* array_of_subsizes, MPI_Fint* array_of_starts, MPI_Fint* order, MPI_Fint* oldtype, MPI_Fint* newtype, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Datatype c_newtype;


    *ierr = MPI_Type_create_subarray( *ndims, array_of_sizes, array_of_subsizes, array_of_starts, *order, PMPI_Type_f2c( *oldtype ), &c_newtype );

    *newtype = PMPI_Type_c2f( c_newtype );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_TYPE_DUP ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( MPI_Type_dup )
/**
 * Measurement wrapper for MPI_Type_dup
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup type
 * For the order of events see @ref MPI_Type_dup
 */
void
FSUB( MPI_Type_dup )( MPI_Fint* type, MPI_Fint* newtype, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Datatype c_newtype;


    *ierr = MPI_Type_dup( PMPI_Type_f2c( *type ), &c_newtype );

    *newtype = PMPI_Type_c2f( c_newtype );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_TYPE_EXTENT ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( MPI_Type_extent )
/**
 * Measurement wrapper for MPI_Type_extent
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup type
 * For the order of events see @ref MPI_Type_extent
 */
void
FSUB( MPI_Type_extent )( MPI_Fint* datatype, MPI_Fint* extent, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Aint c_extent;


    *ierr = MPI_Type_extent( PMPI_Type_f2c( *datatype ), &c_extent );

    *extent = ( MPI_Fint )c_extent;
    if ( *extent != c_extent )
    {
        UTILS_DEBUG_PRINTF( SCOREP_DEBUG_MPI, "Value truncated in \"MPI_Type_extent\". Function is deprecated due to mismatching parameter types! Consult the MPI Standard for more details." );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_TYPE_FREE ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( MPI_Type_free )
/**
 * Measurement wrapper for MPI_Type_free
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup type
 * For the order of events see @ref MPI_Type_free
 */
void
FSUB( MPI_Type_free )( MPI_Fint* datatype, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Datatype c_datatype = PMPI_Type_f2c( *datatype );


    *ierr = MPI_Type_free( &c_datatype );

    *datatype = PMPI_Type_c2f( c_datatype );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_TYPE_GET_ENVELOPE ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( MPI_Type_get_envelope )
/**
 * Measurement wrapper for MPI_Type_get_envelope
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup type
 * For the order of events see @ref MPI_Type_get_envelope
 */
void
FSUB( MPI_Type_get_envelope )( MPI_Fint* datatype, MPI_Fint* num_integers, MPI_Fint* num_addresses, MPI_Fint* num_datatypes, MPI_Fint* combiner, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Type_get_envelope( PMPI_Type_f2c( *datatype ), num_integers, num_addresses, num_datatypes, combiner );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_TYPE_GET_EXTENT ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( MPI_Type_get_extent )
/**
 * Measurement wrapper for MPI_Type_get_extent
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup type
 * For the order of events see @ref MPI_Type_get_extent
 */
void
FSUB( MPI_Type_get_extent )( MPI_Fint* datatype, MPI_Aint* lb, MPI_Aint* extent, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Type_get_extent( PMPI_Type_f2c( *datatype ), lb, extent );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_TYPE_GET_EXTENT_X ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( MPI_Type_get_extent_x )
/**
 * Measurement wrapper for MPI_Type_get_extent_x
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-3
 * @ingroup type
 * For the order of events see @ref MPI_Type_get_extent_x
 */
void
FSUB( MPI_Type_get_extent_x )( MPI_Fint* datatype, MPI_Count* lb, MPI_Count* extent, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Type_get_extent_x( PMPI_Type_f2c( *datatype ), lb, extent );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_TYPE_GET_TRUE_EXTENT ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( MPI_Type_get_true_extent )
/**
 * Measurement wrapper for MPI_Type_get_true_extent
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup type
 * For the order of events see @ref MPI_Type_get_true_extent
 */
void
FSUB( MPI_Type_get_true_extent )( MPI_Fint* datatype, MPI_Aint* true_lb, MPI_Aint* true_extent, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Type_get_true_extent( PMPI_Type_f2c( *datatype ), true_lb, true_extent );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_TYPE_GET_TRUE_EXTENT_X ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( MPI_Type_get_true_extent_x )
/**
 * Measurement wrapper for MPI_Type_get_true_extent_x
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-3
 * @ingroup type
 * For the order of events see @ref MPI_Type_get_true_extent_x
 */
void
FSUB( MPI_Type_get_true_extent_x )( MPI_Fint* datatype, MPI_Count* true_lb, MPI_Count* true_extent, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Type_get_true_extent_x( PMPI_Type_f2c( *datatype ), true_lb, true_extent );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_TYPE_HINDEXED ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( MPI_Type_hindexed )
/**
 * Measurement wrapper for MPI_Type_hindexed
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup type
 * For the order of events see @ref MPI_Type_hindexed
 */
void
FSUB( MPI_Type_hindexed )( MPI_Fint* count, MPI_Fint* array_of_blocklengths, MPI_Fint* array_of_displacements, MPI_Fint* oldtype, MPI_Fint* newtype, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    int          i;
    MPI_Aint*    c_array_of_displacements;
    MPI_Datatype c_newtype;
    c_array_of_displacements = ( MPI_Aint* )malloc( *count * sizeof( MPI_Aint ) );
    for ( i = 0; i < *count; ++i )
    {
        c_array_of_displacements[ i ] = array_of_displacements[ i ];
    }



    *ierr = MPI_Type_hindexed( *count, array_of_blocklengths, c_array_of_displacements, PMPI_Type_f2c( *oldtype ), &c_newtype );

    free( c_array_of_displacements );

    *newtype = PMPI_Type_c2f( c_newtype );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_TYPE_HVECTOR ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( MPI_Type_hvector )
/**
 * Measurement wrapper for MPI_Type_hvector
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup type
 * For the order of events see @ref MPI_Type_hvector
 */
void
FSUB( MPI_Type_hvector )( MPI_Fint* count, MPI_Fint* blocklength, MPI_Fint* stride, MPI_Fint* oldtype, MPI_Fint* newtype, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Aint     c_stride = *stride;
    MPI_Datatype c_newtype;


    *ierr = MPI_Type_hvector( *count, *blocklength, c_stride, PMPI_Type_f2c( *oldtype ), &c_newtype );

    *newtype = PMPI_Type_c2f( c_newtype );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_TYPE_INDEXED ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( MPI_Type_indexed )
/**
 * Measurement wrapper for MPI_Type_indexed
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup type
 * For the order of events see @ref MPI_Type_indexed
 */
void
FSUB( MPI_Type_indexed )( MPI_Fint* count, MPI_Fint* array_of_blocklengths, MPI_Fint* array_of_displacements, MPI_Fint* oldtype, MPI_Fint* newtype, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Datatype c_newtype;


    *ierr = MPI_Type_indexed( *count, array_of_blocklengths, array_of_displacements, PMPI_Type_f2c( *oldtype ), &c_newtype );

    *newtype = PMPI_Type_c2f( c_newtype );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_TYPE_LB ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( MPI_Type_lb )
/**
 * Measurement wrapper for MPI_Type_lb
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup type
 * For the order of events see @ref MPI_Type_lb
 */
void
FSUB( MPI_Type_lb )( MPI_Fint* datatype, MPI_Fint* displacement, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Aint c_displacement;


    *ierr = MPI_Type_lb( PMPI_Type_f2c( *datatype ), &c_displacement );

    *displacement = ( MPI_Fint )c_displacement;
    if ( *displacement != c_displacement )
    {
        UTILS_DEBUG_PRINTF( SCOREP_DEBUG_MPI, "Value truncated in \"MPI_Type_lb\". Function is deprecated due to mismatching parameter types! Consult the MPI Standard for more details." );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_TYPE_MATCH_SIZE ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( MPI_Type_match_size )
/**
 * Measurement wrapper for MPI_Type_match_size
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup type
 * For the order of events see @ref MPI_Type_match_size
 */
void
FSUB( MPI_Type_match_size )( MPI_Fint* typeclass, MPI_Fint* size, MPI_Fint* type, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Datatype c_type;


    *ierr = MPI_Type_match_size( *typeclass, *size, &c_type );

    *type = PMPI_Type_c2f( c_type );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_TYPE_SIZE ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( MPI_Type_size )
/**
 * Measurement wrapper for MPI_Type_size
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup type
 * For the order of events see @ref MPI_Type_size
 */
void
FSUB( MPI_Type_size )( MPI_Fint* datatype, MPI_Fint* size, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Type_size( PMPI_Type_f2c( *datatype ), size );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_TYPE_SIZE_X ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( MPI_Type_size_x )
/**
 * Measurement wrapper for MPI_Type_size_x
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-3
 * @ingroup type
 * For the order of events see @ref MPI_Type_size_x
 */
void
FSUB( MPI_Type_size_x )( MPI_Fint* datatype, MPI_Count* size, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Type_size_x( PMPI_Type_f2c( *datatype ), size );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_TYPE_STRUCT ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( MPI_Type_struct )
/**
 * Measurement wrapper for MPI_Type_struct
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup type
 * For the order of events see @ref MPI_Type_struct
 */
void
FSUB( MPI_Type_struct )( MPI_Fint* count, MPI_Fint* array_of_blocklengths, MPI_Fint* array_of_displacements, MPI_Fint* array_of_types, MPI_Fint* newtype, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    int           i;
    MPI_Aint*     c_array_of_displacements;
    MPI_Datatype  c_newtype;
    MPI_Datatype* c_array_of_types;

    c_array_of_displacements = ( MPI_Aint* )malloc( *count * sizeof( MPI_Aint ) );
    for ( i = 0; i < *count; ++i )
    {
        c_array_of_displacements[ i ] = array_of_displacements[ i ];
    }



    c_array_of_types = ( MPI_Datatype* )malloc( *count * sizeof( MPI_Datatype ) );
    if ( !c_array_of_types )
    {
        UTILS_FATAL( "Allocation of %zu bytes for type conversion failed",
                     *count * sizeof( MPI_Datatype ) );
    }
    /* index variable i, is already defined by the implicit conversion done
     * in the special 'noaint' handler */
    for ( i = 0; i < *count; ++i )
    {
        c_array_of_types[ i ] = PMPI_Type_f2c( array_of_types[ i ] );
    }


    *ierr = MPI_Type_struct( *count, array_of_blocklengths, c_array_of_displacements, c_array_of_types, &c_newtype );


    free( c_array_of_types );
    free( c_array_of_displacements );

    *newtype = PMPI_Type_c2f( c_newtype );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_TYPE_UB ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( MPI_Type_ub )
/**
 * Measurement wrapper for MPI_Type_ub
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup type
 * For the order of events see @ref MPI_Type_ub
 */
void
FSUB( MPI_Type_ub )( MPI_Fint* datatype, MPI_Fint* displacement, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Aint c_displacement;


    *ierr = MPI_Type_ub( PMPI_Type_f2c( *datatype ), &c_displacement );

    *displacement = ( MPI_Fint )c_displacement;
    if ( *displacement != c_displacement )
    {
        UTILS_DEBUG_PRINTF( SCOREP_DEBUG_MPI, "Value truncated in \"MPI_Type_ub\". Function is deprecated due to mismatching parameter types! Consult the MPI Standard for more details." );
    }

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_TYPE_VECTOR ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( MPI_Type_vector )
/**
 * Measurement wrapper for MPI_Type_vector
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup type
 * For the order of events see @ref MPI_Type_vector
 */
void
FSUB( MPI_Type_vector )( MPI_Fint* count, MPI_Fint* blocklength, MPI_Fint* stride, MPI_Fint* oldtype, MPI_Fint* newtype, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Datatype c_newtype;


    *ierr = MPI_Type_vector( *count, *blocklength, *stride, PMPI_Type_f2c( *oldtype ), &c_newtype );

    *newtype = PMPI_Type_c2f( c_newtype );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_UNPACK ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( MPI_Unpack )
/**
 * Measurement wrapper for MPI_Unpack
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup type
 * For the order of events see @ref MPI_Unpack
 */
void
FSUB( MPI_Unpack )( void* inbuf, MPI_Fint* insize, MPI_Fint* position, void* outbuf, MPI_Fint* outcount, MPI_Fint* datatype, MPI_Fint* comm, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    #if HAVE( MPI_BOTTOM )
    if ( outbuf == scorep_mpi_fortran_bottom )
    {
        outbuf = MPI_BOTTOM;
    }
    #endif


    *ierr = MPI_Unpack( inbuf, *insize, position, outbuf, *outcount, PMPI_Type_f2c( *datatype ), PMPI_Comm_f2c( *comm ) );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_UNPACK_EXTERNAL ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( MPI_Unpack_external )
/**
 * Measurement wrapper for MPI_Unpack_external
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup type
 * For the order of events see @ref MPI_Unpack_external
 */
void
FSUB( MPI_Unpack_external )( char* datarep, void* inbuf, MPI_Aint* insize, MPI_Aint* position, void* outbuf, MPI_Fint* outcount, MPI_Fint* datatype, MPI_Fint* ierr, scorep_fortran_charlen_t datarep_len )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    char* c_datarep = NULL;
    c_datarep = scorep_mpi_f2c_string( datarep, datarep_len );



    #if HAVE( MPI_BOTTOM )
    if ( outbuf == scorep_mpi_fortran_bottom )
    {
        outbuf = MPI_BOTTOM;
    }
    #endif


    *ierr = MPI_Unpack_external( c_datarep, inbuf, *insize, position, outbuf, *outcount, PMPI_Type_f2c( *datatype ) );

    free( c_datarep );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif

#if HAVE( MPI_TYPE_CREATE_F90_COMPLEX_COMPLIANT )
#if HAVE( DECL_PMPI_TYPE_CREATE_F90_COMPLEX ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( MPI_Type_create_f90_complex )
/**
 * Measurement wrapper for MPI_Type_create_f90_complex
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f_Compl.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup type
 * For the order of events see @ref MPI_Type_create_f90_complex
 */
void
FSUB( MPI_Type_create_f90_complex )( MPI_Fint* p, MPI_Fint* r, MPI_Fint* newtype, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Datatype c_newtype;


    *ierr = MPI_Type_create_f90_complex( *p, *r, &c_newtype );

    *newtype = PMPI_Type_c2f( c_newtype );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#endif
#if HAVE( MPI_TYPE_CREATE_F90_INTEGER_COMPLIANT )
#if HAVE( DECL_PMPI_TYPE_CREATE_F90_INTEGER ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( MPI_Type_create_f90_integer )
/**
 * Measurement wrapper for MPI_Type_create_f90_integer
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f_Compl.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup type
 * For the order of events see @ref MPI_Type_create_f90_integer
 */
void
FSUB( MPI_Type_create_f90_integer )( MPI_Fint* r, MPI_Fint* newtype, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Datatype c_newtype;


    *ierr = MPI_Type_create_f90_integer( *r, &c_newtype );

    *newtype = PMPI_Type_c2f( c_newtype );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#endif
#if HAVE( MPI_TYPE_CREATE_F90_REAL_COMPLIANT )
#if HAVE( DECL_PMPI_TYPE_CREATE_F90_REAL ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( MPI_Type_create_f90_real )
/**
 * Measurement wrapper for MPI_Type_create_f90_real
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f_Compl.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup type
 * For the order of events see @ref MPI_Type_create_f90_real
 */
void
FSUB( MPI_Type_create_f90_real )( MPI_Fint* p, MPI_Fint* r, MPI_Fint* newtype, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Datatype c_newtype;


    *ierr = MPI_Type_create_f90_real( *p, *r, &c_newtype );

    *newtype = PMPI_Type_c2f( c_newtype );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#endif

/**
 * @}
 * @name Fortran wrappers for external interface functions
 * @{
 */

#if HAVE( DECL_PMPI_TYPE_CREATE_KEYVAL ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( SCOREP_MPI_NO_EXT ) && !defined( MPI_Type_create_keyval )
/**
 * Measurement wrapper for MPI_Type_create_keyval
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup type_ext
 * For the order of events see @ref MPI_Type_create_keyval
 */
void
FSUB( MPI_Type_create_keyval )( void* type_copy_attr_fn, void* type_delete_attr_fn, MPI_Fint* type_keyval, void* extra_state, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Type_create_keyval( ( MPI_Type_copy_attr_function* )type_copy_attr_fn, ( MPI_Type_delete_attr_function* )type_delete_attr_fn, type_keyval, extra_state );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_TYPE_DELETE_ATTR ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( SCOREP_MPI_NO_EXT ) && !defined( MPI_Type_delete_attr )
/**
 * Measurement wrapper for MPI_Type_delete_attr
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup type_ext
 * For the order of events see @ref MPI_Type_delete_attr
 */
void
FSUB( MPI_Type_delete_attr )( MPI_Fint* type, MPI_Fint* type_keyval, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Datatype c_type = PMPI_Type_f2c( *type );


    *ierr = MPI_Type_delete_attr( c_type, *type_keyval );

    *type = PMPI_Type_c2f( c_type );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_TYPE_FREE_KEYVAL ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( SCOREP_MPI_NO_EXT ) && !defined( MPI_Type_free_keyval )
/**
 * Measurement wrapper for MPI_Type_free_keyval
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup type_ext
 * For the order of events see @ref MPI_Type_free_keyval
 */
void
FSUB( MPI_Type_free_keyval )( MPI_Fint* type_keyval, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Type_free_keyval( type_keyval );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_TYPE_GET_ATTR ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( SCOREP_MPI_NO_EXT ) && !defined( MPI_Type_get_attr )
/**
 * Measurement wrapper for MPI_Type_get_attr
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup type_ext
 * For the order of events see @ref MPI_Type_get_attr
 */
void
FSUB( MPI_Type_get_attr )( MPI_Fint* type, MPI_Fint* type_keyval, void* attribute_val, MPI_Fint* flag, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();


    *ierr = MPI_Type_get_attr( PMPI_Type_f2c( *type ), *type_keyval, attribute_val, flag );

    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_TYPE_GET_NAME ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( SCOREP_MPI_NO_EXT ) && !defined( MPI_Type_get_name )
/**
 * Measurement wrapper for MPI_Type_get_name
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup type_ext
 * For the order of events see @ref MPI_Type_get_name
 */
void
FSUB( MPI_Type_get_name )( MPI_Fint* type, char* type_name, MPI_Fint* resultlen, MPI_Fint* ierr, scorep_fortran_charlen_t type_name_len )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    char*  c_type_name     = NULL;
    size_t c_type_name_len = 0;
    c_type_name = ( char* )malloc( ( type_name_len + 1 ) * sizeof( char ) );
    if ( !c_type_name )
    {
        exit( EXIT_FAILURE );
    }



    *ierr = MPI_Type_get_name( PMPI_Type_f2c( *type ), c_type_name, resultlen );


    c_type_name_len = strlen( c_type_name );
    strncpy( type_name, c_type_name, c_type_name_len );
    memset( type_name + c_type_name_len, ' ', type_name_len - c_type_name_len );
    free( c_type_name );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_TYPE_SET_ATTR ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( SCOREP_MPI_NO_EXT ) && !defined( MPI_Type_set_attr )
/**
 * Measurement wrapper for MPI_Type_set_attr
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup type_ext
 * For the order of events see @ref MPI_Type_set_attr
 */
void
FSUB( MPI_Type_set_attr )( MPI_Fint* type, MPI_Fint* type_keyval, void* attribute_val, MPI_Fint* ierr )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Datatype c_type = PMPI_Type_f2c( *type );


    *ierr = MPI_Type_set_attr( c_type, *type_keyval, attribute_val );

    *type = PMPI_Type_c2f( c_type );
    SCOREP_IN_MEASUREMENT_DECREMENT();
}
#endif
#if HAVE( DECL_PMPI_TYPE_SET_NAME ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( SCOREP_MPI_NO_EXT ) && !defined( MPI_Type_set_name )
/**
 * Measurement wrapper for MPI_Type_set_name
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_F2c_C2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup type_ext
 * For the order of events see @ref MPI_Type_set_name
 */
void
FSUB( MPI_Type_set_name )( MPI_Fint* type, char* type_name, MPI_Fint* ierr, scorep_fortran_charlen_t type_name_len )
{
    SCOREP_IN_MEASUREMENT_INCREMENT();
    MPI_Datatype c_type      = PMPI_Type_f2c( *type );
    char*        c_type_name = NULL;
    c_type_name = scorep_mpi_f2c_string( type_name, type_name_len );



    *ierr = MPI_Type_set_name( c_type, c_type_name );

    *type = PMPI_Type_c2f( c_type );
    free( c_type_name );
    SCOREP_IN_MEASUREMENT_DECREMENT();
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
