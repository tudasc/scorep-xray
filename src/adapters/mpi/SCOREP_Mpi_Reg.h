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
 * @brief Registration of MPI functions
 * For all MPI functions a region is registered at initialization time of the MPI adapter.
 * The dynamic region handle obtained from the measurement system is stored in an array
 * with a fixed index for every region.
 */

#ifndef SCOREP_MPI_REG_H
#define SCOREP_MPI_REG_H

#include <SCOREP_Definitions.h>
/*
 * -----------------------------------------------------------------------------
 *
 *  - Registration of MPI functions
 *
 * -----------------------------------------------------------------------------
 */

/** function type is not point-to-point */
#define SCOREP_MPI_TYPE__NONE                          0
/** function type is receive operation */
#define SCOREP_MPI_TYPE__RECV                          1
/** function type is send operation */
#define SCOREP_MPI_TYPE__SEND                          2
/** function type is send and receive operation */
#define SCOREP_MPI_TYPE__SENDRECV                      3
/** function type is collective */
#define SCOREP_MPI_TYPE__COLLECTIVE                    4

/** function has is unknown communication pattern */
#define SCOREP_COLL_TYPE__UNKNOWN                      1
/** function is barrier-like operation */
#define SCOREP_COLL_TYPE__BARRIER                      2
/** function has 1:n communication pattern */
#define SCOREP_COLL_TYPE__ONE2ALL                      3
/** function has n:1 communication pattern */
#define SCOREP_COLL_TYPE__ALL2ONE                      4
/** function has n:n communication pattern */
#define SCOREP_COLL_TYPE__ALL2ALL                      5
/** function may be partially synchronizing */
#define SCOREP_COLL_TYPE__PARTIAL                      6
/** function is implicitly synchronizing */
#define SCOREP_COLL_TYPE__IMPLIED                      7

/** Mappings from MPI collective names to Score-P @{ */
#define SCOREP_MPI_COLLECTIVE__MPI_BARRIER              SCOREP_COLLECTIVE_BARRIER
#define SCOREP_MPI_COLLECTIVE__MPI_BCAST                SCOREP_COLLECTIVE_BROADCAST
#define SCOREP_MPI_COLLECTIVE__MPI_GATHER               SCOREP_COLLECTIVE_GATHER
#define SCOREP_MPI_COLLECTIVE__MPI_GATHERV              SCOREP_COLLECTIVE_GATHERV
#define SCOREP_MPI_COLLECTIVE__MPI_SCATTER              SCOREP_COLLECTIVE_SCATTER
#define SCOREP_MPI_COLLECTIVE__MPI_SCATTERV             SCOREP_COLLECTIVE_SCATTERV
#define SCOREP_MPI_COLLECTIVE__MPI_ALLGATHER            SCOREP_COLLECTIVE_ALLGATHER
#define SCOREP_MPI_COLLECTIVE__MPI_ALLGATHERV           SCOREP_COLLECTIVE_ALLGATHERV
#define SCOREP_MPI_COLLECTIVE__MPI_ALLTOALL             SCOREP_COLLECTIVE_ALLTOALL
#define SCOREP_MPI_COLLECTIVE__MPI_ALLTOALLV            SCOREP_COLLECTIVE_ALLTOALLV
#define SCOREP_MPI_COLLECTIVE__MPI_ALLTOALLW            SCOREP_COLLECTIVE_ALLTOALLW
#define SCOREP_MPI_COLLECTIVE__MPI_ALLREDUCE            SCOREP_COLLECTIVE_ALLREDUCE
#define SCOREP_MPI_COLLECTIVE__MPI_REDUCE               SCOREP_COLLECTIVE_REDUCE
#define SCOREP_MPI_COLLECTIVE__MPI_REDUCE_SCATTER       SCOREP_COLLECTIVE_REDUCE_SCATTER
#define SCOREP_MPI_COLLECTIVE__MPI_REDUCE_SCATTER_BLOCK SCOREP_COLLECTIVE_REDUCE_SCATTER_BLOCK
#define SCOREP_MPI_COLLECTIVE__MPI_SCAN                 SCOREP_COLLECTIVE_SCAN
#define SCOREP_MPI_COLLECTIVE__MPI_EXSCAN               SCOREP_COLLECTIVE_EXSCAN
/** @} */

/**
 * Bit patterns for runtime wrapper enabling.
 * @note Adding a new derived group requires a change in enable_derived_groups().
 */
enum scorep_mpi_groups
{
    /* pure groups, which can be specified in conf */
    SCOREP_MPI_ENABLED_CG        = 1 << 0,
    SCOREP_MPI_ENABLED_COLL      = 1 << 1,
    SCOREP_MPI_ENABLED_ENV       = 1 << 2,
    SCOREP_MPI_ENABLED_ERR       = 1 << 3,
    SCOREP_MPI_ENABLED_EXT       = 1 << 4,
    SCOREP_MPI_ENABLED_IO        = 1 << 5,
    SCOREP_MPI_ENABLED_MISC      = 1 << 6,
    SCOREP_MPI_ENABLED_P2P       = 1 << 7,
    SCOREP_MPI_ENABLED_RMA       = 1 << 8,
    SCOREP_MPI_ENABLED_SPAWN     = 1 << 9,
    SCOREP_MPI_ENABLED_TOPO      = 1 << 10,
    SCOREP_MPI_ENABLED_TYPE      = 1 << 11,
    SCOREP_MPI_ENABLED_PERF      = 1 << 12,
    SCOREP_MPI_ENABLED_XNONBLOCK = 1 << 13,
    SCOREP_MPI_ENABLED_XREQTEST  = 1 << 14,
    /* derived groups, which are a combination of existing groups */
    SCOREP_MPI_ENABLED_CG_ERR    = 1 << 15,
    SCOREP_MPI_ENABLED_CG_EXT    = 1 << 16,
    SCOREP_MPI_ENABLED_CG_MISC   = 1 << 17,
    SCOREP_MPI_ENABLED_IO_ERR    = 1 << 18,
    SCOREP_MPI_ENABLED_IO_MISC   = 1 << 19,
    SCOREP_MPI_ENABLED_REQUEST   = 1 << 20,
    SCOREP_MPI_ENABLED_RMA_ERR   = 1 << 21,
    SCOREP_MPI_ENABLED_RMA_EXT   = 1 << 22,
    SCOREP_MPI_ENABLED_RMA_MISC  = 1 << 23,
    SCOREP_MPI_ENABLED_TYPE_EXT  = 1 << 24,
    SCOREP_MPI_ENABLED_TYPE_MISC = 1 << 25,
    /* NOTE: ALL should comprise all pure groups */
    SCOREP_MPI_ENABLED_ALL       = SCOREP_MPI_ENABLED_CG        |
                                   SCOREP_MPI_ENABLED_COLL      |
                                   SCOREP_MPI_ENABLED_ENV       |
                                   SCOREP_MPI_ENABLED_ERR       |
                                   SCOREP_MPI_ENABLED_EXT       |
                                   SCOREP_MPI_ENABLED_IO        |
                                   SCOREP_MPI_ENABLED_MISC      |
                                   SCOREP_MPI_ENABLED_P2P       |
                                   SCOREP_MPI_ENABLED_RMA       |
                                   SCOREP_MPI_ENABLED_SPAWN     |
                                   SCOREP_MPI_ENABLED_TOPO      |
                                   SCOREP_MPI_ENABLED_TYPE      |
                                   SCOREP_MPI_ENABLED_PERF      |
                                   SCOREP_MPI_ENABLED_XNONBLOCK |
                                   SCOREP_MPI_ENABLED_XREQTEST,
    /* NOTE: DEFAULT should reflect the default set */
    SCOREP_MPI_ENABLED_DEFAULT = SCOREP_MPI_ENABLED_CG    |
                                 SCOREP_MPI_ENABLED_COLL  |
                                 SCOREP_MPI_ENABLED_ENV   |
                                 SCOREP_MPI_ENABLED_IO    |
                                 SCOREP_MPI_ENABLED_P2P   |
                                 SCOREP_MPI_ENABLED_RMA   |
                                 SCOREP_MPI_ENABLED_TOPO  |
                                 SCOREP_MPI_ENABLED_XNONBLOCK
};

/** Bit vector for runtime measurement wrapper enabling/disabling */
extern uint64_t scorep_mpi_enabled;

/**
 * Detect erroneous compile-time disablement of request group
 */
#if defined( SCOREP_MPI_NO_REQUEST )
    #error SCOREP_MPI_NO_REQUEST must not be set manually. Please remove define from the compile line.
#elif defined( SCOREP_MPI_NO_CG ) \
    && defined( SCOREP_MPI_NO_COLL ) \
    && defined( SCOREP_MPI_NO_EXT ) \
    && defined( SCOREP_MPI_NO_IO ) \
    && defined( SCOREP_MPI_NO_P2P ) \
    && defined( SCOREP_MPI_NO_RMA )
    #define SCOREP_MPI_NO_REQUEST
#endif

enum scorep_mpi_regions
{
    /** Score-P region ID for MPI_Abort */
    SCOREP_MPI_REGION__MPI_ABORT,
    /** Score-P region ID for MPI_Accumulate */
    SCOREP_MPI_REGION__MPI_ACCUMULATE,
    /** Score-P region ID for MPI_Add_error_class */
    SCOREP_MPI_REGION__MPI_ADD_ERROR_CLASS,
    /** Score-P region ID for MPI_Add_error_code */
    SCOREP_MPI_REGION__MPI_ADD_ERROR_CODE,
    /** Score-P region ID for MPI_Add_error_string */
    SCOREP_MPI_REGION__MPI_ADD_ERROR_STRING,
    /** Score-P region ID for MPI_Address */
    SCOREP_MPI_REGION__MPI_ADDRESS,
    /** Score-P region ID for MPI_Allgather */
    SCOREP_MPI_REGION__MPI_ALLGATHER,
    /** Score-P region ID for MPI_Allgatherv */
    SCOREP_MPI_REGION__MPI_ALLGATHERV,
    /** Score-P region ID for MPI_Alloc_mem */
    SCOREP_MPI_REGION__MPI_ALLOC_MEM,
    /** Score-P region ID for MPI_Allreduce */
    SCOREP_MPI_REGION__MPI_ALLREDUCE,
    /** Score-P region ID for MPI_Alltoall */
    SCOREP_MPI_REGION__MPI_ALLTOALL,
    /** Score-P region ID for MPI_Alltoallv */
    SCOREP_MPI_REGION__MPI_ALLTOALLV,
    /** Score-P region ID for MPI_Alltoallw */
    SCOREP_MPI_REGION__MPI_ALLTOALLW,
    /** Score-P region ID for MPI_Attr_delete */
    SCOREP_MPI_REGION__MPI_ATTR_DELETE,
    /** Score-P region ID for MPI_Attr_get */
    SCOREP_MPI_REGION__MPI_ATTR_GET,
    /** Score-P region ID for MPI_Attr_put */
    SCOREP_MPI_REGION__MPI_ATTR_PUT,
    /** Score-P region ID for MPI_Barrier */
    SCOREP_MPI_REGION__MPI_BARRIER,
    /** Score-P region ID for MPI_Bcast */
    SCOREP_MPI_REGION__MPI_BCAST,
    /** Score-P region ID for MPI_Bsend */
    SCOREP_MPI_REGION__MPI_BSEND,
    /** Score-P region ID for MPI_Bsend_init */
    SCOREP_MPI_REGION__MPI_BSEND_INIT,
    /** Score-P region ID for MPI_Buffer_attach */
    SCOREP_MPI_REGION__MPI_BUFFER_ATTACH,
    /** Score-P region ID for MPI_Buffer_detach */
    SCOREP_MPI_REGION__MPI_BUFFER_DETACH,
    /** Score-P region ID for MPI_Cancel */
    SCOREP_MPI_REGION__MPI_CANCEL,
    /** Score-P region ID for MPI_Cart_coords */
    SCOREP_MPI_REGION__MPI_CART_COORDS,
    /** Score-P region ID for MPI_Cart_create */
    SCOREP_MPI_REGION__MPI_CART_CREATE,
    /** Score-P region ID for MPI_Cart_get */
    SCOREP_MPI_REGION__MPI_CART_GET,
    /** Score-P region ID for MPI_Cart_map */
    SCOREP_MPI_REGION__MPI_CART_MAP,
    /** Score-P region ID for MPI_Cart_rank */
    SCOREP_MPI_REGION__MPI_CART_RANK,
    /** Score-P region ID for MPI_Cart_shift */
    SCOREP_MPI_REGION__MPI_CART_SHIFT,
    /** Score-P region ID for MPI_Cart_sub */
    SCOREP_MPI_REGION__MPI_CART_SUB,
    /** Score-P region ID for MPI_Cartdim_get */
    SCOREP_MPI_REGION__MPI_CARTDIM_GET,
    /** Score-P region ID for MPI_Close_port */
    SCOREP_MPI_REGION__MPI_CLOSE_PORT,
    /** Score-P region ID for MPI_Comm_accept */
    SCOREP_MPI_REGION__MPI_COMM_ACCEPT,
    /** Score-P region ID for MPI_Comm_c2f */
    SCOREP_MPI_REGION__MPI_COMM_C2F,
    /** Score-P region ID for MPI_Comm_call_errhandler */
    SCOREP_MPI_REGION__MPI_COMM_CALL_ERRHANDLER,
    /** Score-P region ID for MPI_Comm_compare */
    SCOREP_MPI_REGION__MPI_COMM_COMPARE,
    /** Score-P region ID for MPI_Comm_connect */
    SCOREP_MPI_REGION__MPI_COMM_CONNECT,
    /** Score-P region ID for MPI_Comm_create */
    SCOREP_MPI_REGION__MPI_COMM_CREATE,
    /** Score-P region ID for MPI_Comm_create_errhandler */
    SCOREP_MPI_REGION__MPI_COMM_CREATE_ERRHANDLER,
    /** Score-P region ID for MPI_Comm_create_group */
    SCOREP_MPI_REGION__MPI_COMM_CREATE_GROUP,
    /** Score-P region ID for MPI_Comm_create_keyval */
    SCOREP_MPI_REGION__MPI_COMM_CREATE_KEYVAL,
    /** Score-P region ID for MPI_Comm_delete_attr */
    SCOREP_MPI_REGION__MPI_COMM_DELETE_ATTR,
    /** Score-P region ID for MPI_Comm_disconnect */
    SCOREP_MPI_REGION__MPI_COMM_DISCONNECT,
    /** Score-P region ID for MPI_Comm_dup */
    SCOREP_MPI_REGION__MPI_COMM_DUP,
    /** Score-P region ID for MPI_Comm_dup_with_info */
    SCOREP_MPI_REGION__MPI_COMM_DUP_WITH_INFO,
    /** Score-P region ID for MPI_Comm_f2c */
    SCOREP_MPI_REGION__MPI_COMM_F2C,
    /** Score-P region ID for MPI_Comm_free */
    SCOREP_MPI_REGION__MPI_COMM_FREE,
    /** Score-P region ID for MPI_Comm_free_keyval */
    SCOREP_MPI_REGION__MPI_COMM_FREE_KEYVAL,
    /** Score-P region ID for MPI_Comm_get_attr */
    SCOREP_MPI_REGION__MPI_COMM_GET_ATTR,
    /** Score-P region ID for MPI_Comm_get_errhandler */
    SCOREP_MPI_REGION__MPI_COMM_GET_ERRHANDLER,
    /** Score-P region ID for MPI_Comm_get_info */
    SCOREP_MPI_REGION__MPI_COMM_GET_INFO,
    /** Score-P region ID for MPI_Comm_get_name */
    SCOREP_MPI_REGION__MPI_COMM_GET_NAME,
    /** Score-P region ID for MPI_Comm_get_parent */
    SCOREP_MPI_REGION__MPI_COMM_GET_PARENT,
    /** Score-P region ID for MPI_Comm_group */
    SCOREP_MPI_REGION__MPI_COMM_GROUP,
    /** Score-P region ID for MPI_Comm_idup */
    SCOREP_MPI_REGION__MPI_COMM_IDUP,
    /** Score-P region ID for MPI_Comm_join */
    SCOREP_MPI_REGION__MPI_COMM_JOIN,
    /** Score-P region ID for MPI_Comm_rank */
    SCOREP_MPI_REGION__MPI_COMM_RANK,
    /** Score-P region ID for MPI_Comm_remote_group */
    SCOREP_MPI_REGION__MPI_COMM_REMOTE_GROUP,
    /** Score-P region ID for MPI_Comm_remote_size */
    SCOREP_MPI_REGION__MPI_COMM_REMOTE_SIZE,
    /** Score-P region ID for MPI_Comm_set_attr */
    SCOREP_MPI_REGION__MPI_COMM_SET_ATTR,
    /** Score-P region ID for MPI_Comm_set_errhandler */
    SCOREP_MPI_REGION__MPI_COMM_SET_ERRHANDLER,
    /** Score-P region ID for MPI_Comm_set_info */
    SCOREP_MPI_REGION__MPI_COMM_SET_INFO,
    /** Score-P region ID for MPI_Comm_set_name */
    SCOREP_MPI_REGION__MPI_COMM_SET_NAME,
    /** Score-P region ID for MPI_Comm_size */
    SCOREP_MPI_REGION__MPI_COMM_SIZE,
    /** Score-P region ID for MPI_Comm_spawn */
    SCOREP_MPI_REGION__MPI_COMM_SPAWN,
    /** Score-P region ID for MPI_Comm_spawn_multiple */
    SCOREP_MPI_REGION__MPI_COMM_SPAWN_MULTIPLE,
    /** Score-P region ID for MPI_Comm_split */
    SCOREP_MPI_REGION__MPI_COMM_SPLIT,
    /** Score-P region ID for MPI_Comm_split_type */
    SCOREP_MPI_REGION__MPI_COMM_SPLIT_TYPE,
    /** Score-P region ID for MPI_Comm_test_inter */
    SCOREP_MPI_REGION__MPI_COMM_TEST_INTER,
    /** Score-P region ID for MPI_Compare_and_swap */
    SCOREP_MPI_REGION__MPI_COMPARE_AND_SWAP,
    /** Score-P region ID for MPI_Dims_create */
    SCOREP_MPI_REGION__MPI_DIMS_CREATE,
    /** Score-P region ID for MPI_Dist_graph_create */
    SCOREP_MPI_REGION__MPI_DIST_GRAPH_CREATE,
    /** Score-P region ID for MPI_Dist_graph_create_adjacent */
    SCOREP_MPI_REGION__MPI_DIST_GRAPH_CREATE_ADJACENT,
    /** Score-P region ID for MPI_Dist_graph_neighbors */
    SCOREP_MPI_REGION__MPI_DIST_GRAPH_NEIGHBORS,
    /** Score-P region ID for MPI_Dist_graph_neighbors_count */
    SCOREP_MPI_REGION__MPI_DIST_GRAPH_NEIGHBORS_COUNT,
    /** Score-P region ID for MPI_Errhandler_create */
    SCOREP_MPI_REGION__MPI_ERRHANDLER_CREATE,
    /** Score-P region ID for MPI_Errhandler_free */
    SCOREP_MPI_REGION__MPI_ERRHANDLER_FREE,
    /** Score-P region ID for MPI_Errhandler_get */
    SCOREP_MPI_REGION__MPI_ERRHANDLER_GET,
    /** Score-P region ID for MPI_Errhandler_set */
    SCOREP_MPI_REGION__MPI_ERRHANDLER_SET,
    /** Score-P region ID for MPI_Error_class */
    SCOREP_MPI_REGION__MPI_ERROR_CLASS,
    /** Score-P region ID for MPI_Error_string */
    SCOREP_MPI_REGION__MPI_ERROR_STRING,
    /** Score-P region ID for MPI_Exscan */
    SCOREP_MPI_REGION__MPI_EXSCAN,
    /** Score-P region ID for MPI_Fetch_and_op */
    SCOREP_MPI_REGION__MPI_FETCH_AND_OP,
    /** Score-P region ID for MPI_File_c2f */
    SCOREP_MPI_REGION__MPI_FILE_C2F,
    /** Score-P region ID for MPI_File_call_errhandler */
    SCOREP_MPI_REGION__MPI_FILE_CALL_ERRHANDLER,
    /** Score-P region ID for MPI_File_close */
    SCOREP_MPI_REGION__MPI_FILE_CLOSE,
    /** Score-P region ID for MPI_File_create_errhandler */
    SCOREP_MPI_REGION__MPI_FILE_CREATE_ERRHANDLER,
    /** Score-P region ID for MPI_File_delete */
    SCOREP_MPI_REGION__MPI_FILE_DELETE,
    /** Score-P region ID for MPI_File_f2c */
    SCOREP_MPI_REGION__MPI_FILE_F2C,
    /** Score-P region ID for MPI_File_get_amode */
    SCOREP_MPI_REGION__MPI_FILE_GET_AMODE,
    /** Score-P region ID for MPI_File_get_atomicity */
    SCOREP_MPI_REGION__MPI_FILE_GET_ATOMICITY,
    /** Score-P region ID for MPI_File_get_byte_offset */
    SCOREP_MPI_REGION__MPI_FILE_GET_BYTE_OFFSET,
    /** Score-P region ID for MPI_File_get_errhandler */
    SCOREP_MPI_REGION__MPI_FILE_GET_ERRHANDLER,
    /** Score-P region ID for MPI_File_get_group */
    SCOREP_MPI_REGION__MPI_FILE_GET_GROUP,
    /** Score-P region ID for MPI_File_get_info */
    SCOREP_MPI_REGION__MPI_FILE_GET_INFO,
    /** Score-P region ID for MPI_File_get_position */
    SCOREP_MPI_REGION__MPI_FILE_GET_POSITION,
    /** Score-P region ID for MPI_File_get_position_shared */
    SCOREP_MPI_REGION__MPI_FILE_GET_POSITION_SHARED,
    /** Score-P region ID for MPI_File_get_size */
    SCOREP_MPI_REGION__MPI_FILE_GET_SIZE,
    /** Score-P region ID for MPI_File_get_type_extent */
    SCOREP_MPI_REGION__MPI_FILE_GET_TYPE_EXTENT,
    /** Score-P region ID for MPI_File_get_view */
    SCOREP_MPI_REGION__MPI_FILE_GET_VIEW,
    /** Score-P region ID for MPI_File_iread */
    SCOREP_MPI_REGION__MPI_FILE_IREAD,
    /** Score-P region ID for MPI_File_iread_all */
    SCOREP_MPI_REGION__MPI_FILE_IREAD_ALL,
    /** Score-P region ID for MPI_File_iread_at */
    SCOREP_MPI_REGION__MPI_FILE_IREAD_AT,
    /** Score-P region ID for MPI_File_iread_at_all */
    SCOREP_MPI_REGION__MPI_FILE_IREAD_AT_ALL,
    /** Score-P region ID for MPI_File_iread_shared */
    SCOREP_MPI_REGION__MPI_FILE_IREAD_SHARED,
    /** Score-P region ID for MPI_File_iwrite */
    SCOREP_MPI_REGION__MPI_FILE_IWRITE,
    /** Score-P region ID for MPI_File_iwrite_all */
    SCOREP_MPI_REGION__MPI_FILE_IWRITE_ALL,
    /** Score-P region ID for MPI_File_iwrite_at */
    SCOREP_MPI_REGION__MPI_FILE_IWRITE_AT,
    /** Score-P region ID for MPI_File_iwrite_at_all */
    SCOREP_MPI_REGION__MPI_FILE_IWRITE_AT_ALL,
    /** Score-P region ID for MPI_File_iwrite_shared */
    SCOREP_MPI_REGION__MPI_FILE_IWRITE_SHARED,
    /** Score-P region ID for MPI_File_open */
    SCOREP_MPI_REGION__MPI_FILE_OPEN,
    /** Score-P region ID for MPI_File_preallocate */
    SCOREP_MPI_REGION__MPI_FILE_PREALLOCATE,
    /** Score-P region ID for MPI_File_read */
    SCOREP_MPI_REGION__MPI_FILE_READ,
    /** Score-P region ID for MPI_File_read_all */
    SCOREP_MPI_REGION__MPI_FILE_READ_ALL,
    /** Score-P region ID for MPI_File_read_all_begin */
    SCOREP_MPI_REGION__MPI_FILE_READ_ALL_BEGIN,
    /** Score-P region ID for MPI_File_read_all_end */
    SCOREP_MPI_REGION__MPI_FILE_READ_ALL_END,
    /** Score-P region ID for MPI_File_read_at */
    SCOREP_MPI_REGION__MPI_FILE_READ_AT,
    /** Score-P region ID for MPI_File_read_at_all */
    SCOREP_MPI_REGION__MPI_FILE_READ_AT_ALL,
    /** Score-P region ID for MPI_File_read_at_all_begin */
    SCOREP_MPI_REGION__MPI_FILE_READ_AT_ALL_BEGIN,
    /** Score-P region ID for MPI_File_read_at_all_end */
    SCOREP_MPI_REGION__MPI_FILE_READ_AT_ALL_END,
    /** Score-P region ID for MPI_File_read_ordered */
    SCOREP_MPI_REGION__MPI_FILE_READ_ORDERED,
    /** Score-P region ID for MPI_File_read_ordered_begin */
    SCOREP_MPI_REGION__MPI_FILE_READ_ORDERED_BEGIN,
    /** Score-P region ID for MPI_File_read_ordered_end */
    SCOREP_MPI_REGION__MPI_FILE_READ_ORDERED_END,
    /** Score-P region ID for MPI_File_read_shared */
    SCOREP_MPI_REGION__MPI_FILE_READ_SHARED,
    /** Score-P region ID for MPI_File_seek */
    SCOREP_MPI_REGION__MPI_FILE_SEEK,
    /** Score-P region ID for MPI_File_seek_shared */
    SCOREP_MPI_REGION__MPI_FILE_SEEK_SHARED,
    /** Score-P region ID for MPI_File_set_atomicity */
    SCOREP_MPI_REGION__MPI_FILE_SET_ATOMICITY,
    /** Score-P region ID for MPI_File_set_errhandler */
    SCOREP_MPI_REGION__MPI_FILE_SET_ERRHANDLER,
    /** Score-P region ID for MPI_File_set_info */
    SCOREP_MPI_REGION__MPI_FILE_SET_INFO,
    /** Score-P region ID for MPI_File_set_size */
    SCOREP_MPI_REGION__MPI_FILE_SET_SIZE,
    /** Score-P region ID for MPI_File_set_view */
    SCOREP_MPI_REGION__MPI_FILE_SET_VIEW,
    /** Score-P region ID for MPI_File_sync */
    SCOREP_MPI_REGION__MPI_FILE_SYNC,
    /** Score-P region ID for MPI_File_write */
    SCOREP_MPI_REGION__MPI_FILE_WRITE,
    /** Score-P region ID for MPI_File_write_all */
    SCOREP_MPI_REGION__MPI_FILE_WRITE_ALL,
    /** Score-P region ID for MPI_File_write_all_begin */
    SCOREP_MPI_REGION__MPI_FILE_WRITE_ALL_BEGIN,
    /** Score-P region ID for MPI_File_write_all_end */
    SCOREP_MPI_REGION__MPI_FILE_WRITE_ALL_END,
    /** Score-P region ID for MPI_File_write_at */
    SCOREP_MPI_REGION__MPI_FILE_WRITE_AT,
    /** Score-P region ID for MPI_File_write_at_all */
    SCOREP_MPI_REGION__MPI_FILE_WRITE_AT_ALL,
    /** Score-P region ID for MPI_File_write_at_all_begin */
    SCOREP_MPI_REGION__MPI_FILE_WRITE_AT_ALL_BEGIN,
    /** Score-P region ID for MPI_File_write_at_all_end */
    SCOREP_MPI_REGION__MPI_FILE_WRITE_AT_ALL_END,
    /** Score-P region ID for MPI_File_write_ordered */
    SCOREP_MPI_REGION__MPI_FILE_WRITE_ORDERED,
    /** Score-P region ID for MPI_File_write_ordered_begin */
    SCOREP_MPI_REGION__MPI_FILE_WRITE_ORDERED_BEGIN,
    /** Score-P region ID for MPI_File_write_ordered_end */
    SCOREP_MPI_REGION__MPI_FILE_WRITE_ORDERED_END,
    /** Score-P region ID for MPI_File_write_shared */
    SCOREP_MPI_REGION__MPI_FILE_WRITE_SHARED,
    /** Score-P region ID for MPI_Finalize */
    SCOREP_MPI_REGION__MPI_FINALIZE,
    /** Score-P region ID for MPI_Finalized */
    SCOREP_MPI_REGION__MPI_FINALIZED,
    /** Score-P region ID for MPI_Free_mem */
    SCOREP_MPI_REGION__MPI_FREE_MEM,
    /** Score-P region ID for MPI_Gather */
    SCOREP_MPI_REGION__MPI_GATHER,
    /** Score-P region ID for MPI_Gatherv */
    SCOREP_MPI_REGION__MPI_GATHERV,
    /** Score-P region ID for MPI_Get */
    SCOREP_MPI_REGION__MPI_GET,
    /** Score-P region ID for MPI_Get_accumulate */
    SCOREP_MPI_REGION__MPI_GET_ACCUMULATE,
    /** Score-P region ID for MPI_Get_address */
    SCOREP_MPI_REGION__MPI_GET_ADDRESS,
    /** Score-P region ID for MPI_Get_count */
    SCOREP_MPI_REGION__MPI_GET_COUNT,
    /** Score-P region ID for MPI_Get_elements */
    SCOREP_MPI_REGION__MPI_GET_ELEMENTS,
    /** Score-P region ID for MPI_Get_elements_x */
    SCOREP_MPI_REGION__MPI_GET_ELEMENTS_X,
    /** Score-P region ID for MPI_Get_library_version */
    SCOREP_MPI_REGION__MPI_GET_LIBRARY_VERSION,
    /** Score-P region ID for MPI_Get_processor_name */
    SCOREP_MPI_REGION__MPI_GET_PROCESSOR_NAME,
    /** Score-P region ID for MPI_Get_version */
    SCOREP_MPI_REGION__MPI_GET_VERSION,
    /** Score-P region ID for MPI_Graph_create */
    SCOREP_MPI_REGION__MPI_GRAPH_CREATE,
    /** Score-P region ID for MPI_Graph_get */
    SCOREP_MPI_REGION__MPI_GRAPH_GET,
    /** Score-P region ID for MPI_Graph_map */
    SCOREP_MPI_REGION__MPI_GRAPH_MAP,
    /** Score-P region ID for MPI_Graph_neighbors */
    SCOREP_MPI_REGION__MPI_GRAPH_NEIGHBORS,
    /** Score-P region ID for MPI_Graph_neighbors_count */
    SCOREP_MPI_REGION__MPI_GRAPH_NEIGHBORS_COUNT,
    /** Score-P region ID for MPI_Graphdims_get */
    SCOREP_MPI_REGION__MPI_GRAPHDIMS_GET,
    /** Score-P region ID for MPI_Grequest_complete */
    SCOREP_MPI_REGION__MPI_GREQUEST_COMPLETE,
    /** Score-P region ID for MPI_Grequest_start */
    SCOREP_MPI_REGION__MPI_GREQUEST_START,
    /** Score-P region ID for MPI_Group_c2f */
    SCOREP_MPI_REGION__MPI_GROUP_C2F,
    /** Score-P region ID for MPI_Group_compare */
    SCOREP_MPI_REGION__MPI_GROUP_COMPARE,
    /** Score-P region ID for MPI_Group_difference */
    SCOREP_MPI_REGION__MPI_GROUP_DIFFERENCE,
    /** Score-P region ID for MPI_Group_excl */
    SCOREP_MPI_REGION__MPI_GROUP_EXCL,
    /** Score-P region ID for MPI_Group_f2c */
    SCOREP_MPI_REGION__MPI_GROUP_F2C,
    /** Score-P region ID for MPI_Group_free */
    SCOREP_MPI_REGION__MPI_GROUP_FREE,
    /** Score-P region ID for MPI_Group_incl */
    SCOREP_MPI_REGION__MPI_GROUP_INCL,
    /** Score-P region ID for MPI_Group_intersection */
    SCOREP_MPI_REGION__MPI_GROUP_INTERSECTION,
    /** Score-P region ID for MPI_Group_range_excl */
    SCOREP_MPI_REGION__MPI_GROUP_RANGE_EXCL,
    /** Score-P region ID for MPI_Group_range_incl */
    SCOREP_MPI_REGION__MPI_GROUP_RANGE_INCL,
    /** Score-P region ID for MPI_Group_rank */
    SCOREP_MPI_REGION__MPI_GROUP_RANK,
    /** Score-P region ID for MPI_Group_size */
    SCOREP_MPI_REGION__MPI_GROUP_SIZE,
    /** Score-P region ID for MPI_Group_translate_ranks */
    SCOREP_MPI_REGION__MPI_GROUP_TRANSLATE_RANKS,
    /** Score-P region ID for MPI_Group_union */
    SCOREP_MPI_REGION__MPI_GROUP_UNION,
    /** Score-P region ID for MPI_Iallgather */
    SCOREP_MPI_REGION__MPI_IALLGATHER,
    /** Score-P region ID for MPI_Iallgatherv */
    SCOREP_MPI_REGION__MPI_IALLGATHERV,
    /** Score-P region ID for MPI_Iallreduce */
    SCOREP_MPI_REGION__MPI_IALLREDUCE,
    /** Score-P region ID for MPI_Ialltoall */
    SCOREP_MPI_REGION__MPI_IALLTOALL,
    /** Score-P region ID for MPI_Ialltoallv */
    SCOREP_MPI_REGION__MPI_IALLTOALLV,
    /** Score-P region ID for MPI_Ialltoallw */
    SCOREP_MPI_REGION__MPI_IALLTOALLW,
    /** Score-P region ID for MPI_Ibarrier */
    SCOREP_MPI_REGION__MPI_IBARRIER,
    /** Score-P region ID for MPI_Ibcast */
    SCOREP_MPI_REGION__MPI_IBCAST,
    /** Score-P region ID for MPI_Ibsend */
    SCOREP_MPI_REGION__MPI_IBSEND,
    /** Score-P region ID for MPI_Iexscan */
    SCOREP_MPI_REGION__MPI_IEXSCAN,
    /** Score-P region ID for MPI_Igather */
    SCOREP_MPI_REGION__MPI_IGATHER,
    /** Score-P region ID for MPI_Igatherv */
    SCOREP_MPI_REGION__MPI_IGATHERV,
    /** Score-P region ID for MPI_Improbe */
    SCOREP_MPI_REGION__MPI_IMPROBE,
    /** Score-P region ID for MPI_Imrecv */
    SCOREP_MPI_REGION__MPI_IMRECV,
    /** Score-P region ID for MPI_Ineighbor_allgather */
    SCOREP_MPI_REGION__MPI_INEIGHBOR_ALLGATHER,
    /** Score-P region ID for MPI_Ineighbor_allgatherv */
    SCOREP_MPI_REGION__MPI_INEIGHBOR_ALLGATHERV,
    /** Score-P region ID for MPI_Ineighbor_alltoall */
    SCOREP_MPI_REGION__MPI_INEIGHBOR_ALLTOALL,
    /** Score-P region ID for MPI_Ineighbor_alltoallv */
    SCOREP_MPI_REGION__MPI_INEIGHBOR_ALLTOALLV,
    /** Score-P region ID for MPI_Ineighbor_alltoallw */
    SCOREP_MPI_REGION__MPI_INEIGHBOR_ALLTOALLW,
    /** Score-P region ID for MPI_Info_c2f */
    SCOREP_MPI_REGION__MPI_INFO_C2F,
    /** Score-P region ID for MPI_Info_create */
    SCOREP_MPI_REGION__MPI_INFO_CREATE,
    /** Score-P region ID for MPI_Info_delete */
    SCOREP_MPI_REGION__MPI_INFO_DELETE,
    /** Score-P region ID for MPI_Info_dup */
    SCOREP_MPI_REGION__MPI_INFO_DUP,
    /** Score-P region ID for MPI_Info_f2c */
    SCOREP_MPI_REGION__MPI_INFO_F2C,
    /** Score-P region ID for MPI_Info_free */
    SCOREP_MPI_REGION__MPI_INFO_FREE,
    /** Score-P region ID for MPI_Info_get */
    SCOREP_MPI_REGION__MPI_INFO_GET,
    /** Score-P region ID for MPI_Info_get_nkeys */
    SCOREP_MPI_REGION__MPI_INFO_GET_NKEYS,
    /** Score-P region ID for MPI_Info_get_nthkey */
    SCOREP_MPI_REGION__MPI_INFO_GET_NTHKEY,
    /** Score-P region ID for MPI_Info_get_valuelen */
    SCOREP_MPI_REGION__MPI_INFO_GET_VALUELEN,
    /** Score-P region ID for MPI_Info_set */
    SCOREP_MPI_REGION__MPI_INFO_SET,
    /** Score-P region ID for MPI_Init */
    SCOREP_MPI_REGION__MPI_INIT,
    /** Score-P region ID for MPI_Init_thread */
    SCOREP_MPI_REGION__MPI_INIT_THREAD,
    /** Score-P region ID for MPI_Initialized */
    SCOREP_MPI_REGION__MPI_INITIALIZED,
    /** Score-P region ID for MPI_Intercomm_create */
    SCOREP_MPI_REGION__MPI_INTERCOMM_CREATE,
    /** Score-P region ID for MPI_Intercomm_merge */
    SCOREP_MPI_REGION__MPI_INTERCOMM_MERGE,
    /** Score-P region ID for MPI_Iprobe */
    SCOREP_MPI_REGION__MPI_IPROBE,
    /** Score-P region ID for MPI_Irecv */
    SCOREP_MPI_REGION__MPI_IRECV,
    /** Score-P region ID for MPI_Ireduce */
    SCOREP_MPI_REGION__MPI_IREDUCE,
    /** Score-P region ID for MPI_Ireduce_scatter */
    SCOREP_MPI_REGION__MPI_IREDUCE_SCATTER,
    /** Score-P region ID for MPI_Ireduce_scatter_block */
    SCOREP_MPI_REGION__MPI_IREDUCE_SCATTER_BLOCK,
    /** Score-P region ID for MPI_Irsend */
    SCOREP_MPI_REGION__MPI_IRSEND,
    /** Score-P region ID for MPI_Is_thread_main */
    SCOREP_MPI_REGION__MPI_IS_THREAD_MAIN,
    /** Score-P region ID for MPI_Iscan */
    SCOREP_MPI_REGION__MPI_ISCAN,
    /** Score-P region ID for MPI_Iscatter */
    SCOREP_MPI_REGION__MPI_ISCATTER,
    /** Score-P region ID for MPI_Iscatterv */
    SCOREP_MPI_REGION__MPI_ISCATTERV,
    /** Score-P region ID for MPI_Isend */
    SCOREP_MPI_REGION__MPI_ISEND,
    /** Score-P region ID for MPI_Issend */
    SCOREP_MPI_REGION__MPI_ISSEND,
    /** Score-P region ID for MPI_Keyval_create */
    SCOREP_MPI_REGION__MPI_KEYVAL_CREATE,
    /** Score-P region ID for MPI_Keyval_free */
    SCOREP_MPI_REGION__MPI_KEYVAL_FREE,
    /** Score-P region ID for MPI_Lookup_name */
    SCOREP_MPI_REGION__MPI_LOOKUP_NAME,
    /** Score-P region ID for MPI_Mprobe */
    SCOREP_MPI_REGION__MPI_MPROBE,
    /** Score-P region ID for MPI_Mrecv */
    SCOREP_MPI_REGION__MPI_MRECV,
    /** Score-P region ID for MPI_Neighbor_allgather */
    SCOREP_MPI_REGION__MPI_NEIGHBOR_ALLGATHER,
    /** Score-P region ID for MPI_Neighbor_allgatherv */
    SCOREP_MPI_REGION__MPI_NEIGHBOR_ALLGATHERV,
    /** Score-P region ID for MPI_Neighbor_alltoall */
    SCOREP_MPI_REGION__MPI_NEIGHBOR_ALLTOALL,
    /** Score-P region ID for MPI_Neighbor_alltoallv */
    SCOREP_MPI_REGION__MPI_NEIGHBOR_ALLTOALLV,
    /** Score-P region ID for MPI_Neighbor_alltoallw */
    SCOREP_MPI_REGION__MPI_NEIGHBOR_ALLTOALLW,
    /** Score-P region ID for MPI_Op_c2f */
    SCOREP_MPI_REGION__MPI_OP_C2F,
    /** Score-P region ID for MPI_Op_commutative */
    SCOREP_MPI_REGION__MPI_OP_COMMUTATIVE,
    /** Score-P region ID for MPI_Op_create */
    SCOREP_MPI_REGION__MPI_OP_CREATE,
    /** Score-P region ID for MPI_Op_f2c */
    SCOREP_MPI_REGION__MPI_OP_F2C,
    /** Score-P region ID for MPI_Op_free */
    SCOREP_MPI_REGION__MPI_OP_FREE,
    /** Score-P region ID for MPI_Open_port */
    SCOREP_MPI_REGION__MPI_OPEN_PORT,
    /** Score-P region ID for MPI_Pack */
    SCOREP_MPI_REGION__MPI_PACK,
    /** Score-P region ID for MPI_Pack_external */
    SCOREP_MPI_REGION__MPI_PACK_EXTERNAL,
    /** Score-P region ID for MPI_Pack_external_size */
    SCOREP_MPI_REGION__MPI_PACK_EXTERNAL_SIZE,
    /** Score-P region ID for MPI_Pack_size */
    SCOREP_MPI_REGION__MPI_PACK_SIZE,
    /** Score-P region ID for MPI_Pcontrol */
    SCOREP_MPI_REGION__MPI_PCONTROL,
    /** Score-P region ID for MPI_Probe */
    SCOREP_MPI_REGION__MPI_PROBE,
    /** Score-P region ID for MPI_Publish_name */
    SCOREP_MPI_REGION__MPI_PUBLISH_NAME,
    /** Score-P region ID for MPI_Put */
    SCOREP_MPI_REGION__MPI_PUT,
    /** Score-P region ID for MPI_Query_thread */
    SCOREP_MPI_REGION__MPI_QUERY_THREAD,
    /** Score-P region ID for MPI_Raccumulate */
    SCOREP_MPI_REGION__MPI_RACCUMULATE,
    /** Score-P region ID for MPI_Recv */
    SCOREP_MPI_REGION__MPI_RECV,
    /** Score-P region ID for MPI_Recv_init */
    SCOREP_MPI_REGION__MPI_RECV_INIT,
    /** Score-P region ID for MPI_Reduce */
    SCOREP_MPI_REGION__MPI_REDUCE,
    /** Score-P region ID for MPI_Reduce_local */
    SCOREP_MPI_REGION__MPI_REDUCE_LOCAL,
    /** Score-P region ID for MPI_Reduce_scatter */
    SCOREP_MPI_REGION__MPI_REDUCE_SCATTER,
    /** Score-P region ID for MPI_Reduce_scatter_block */
    SCOREP_MPI_REGION__MPI_REDUCE_SCATTER_BLOCK,
    /** Score-P region ID for MPI_Register_datarep */
    SCOREP_MPI_REGION__MPI_REGISTER_DATAREP,
    /** Score-P region ID for MPI_Request_c2f */
    SCOREP_MPI_REGION__MPI_REQUEST_C2F,
    /** Score-P region ID for MPI_Request_f2c */
    SCOREP_MPI_REGION__MPI_REQUEST_F2C,
    /** Score-P region ID for MPI_Request_free */
    SCOREP_MPI_REGION__MPI_REQUEST_FREE,
    /** Score-P region ID for MPI_Request_get_status */
    SCOREP_MPI_REGION__MPI_REQUEST_GET_STATUS,
    /** Score-P region ID for MPI_Rget */
    SCOREP_MPI_REGION__MPI_RGET,
    /** Score-P region ID for MPI_Rget_accumulate */
    SCOREP_MPI_REGION__MPI_RGET_ACCUMULATE,
    /** Score-P region ID for MPI_Rput */
    SCOREP_MPI_REGION__MPI_RPUT,
    /** Score-P region ID for MPI_Rsend */
    SCOREP_MPI_REGION__MPI_RSEND,
    /** Score-P region ID for MPI_Rsend_init */
    SCOREP_MPI_REGION__MPI_RSEND_INIT,
    /** Score-P region ID for MPI_Scan */
    SCOREP_MPI_REGION__MPI_SCAN,
    /** Score-P region ID for MPI_Scatter */
    SCOREP_MPI_REGION__MPI_SCATTER,
    /** Score-P region ID for MPI_Scatterv */
    SCOREP_MPI_REGION__MPI_SCATTERV,
    /** Score-P region ID for MPI_Send */
    SCOREP_MPI_REGION__MPI_SEND,
    /** Score-P region ID for MPI_Send_init */
    SCOREP_MPI_REGION__MPI_SEND_INIT,
    /** Score-P region ID for MPI_Sendrecv */
    SCOREP_MPI_REGION__MPI_SENDRECV,
    /** Score-P region ID for MPI_Sendrecv_replace */
    SCOREP_MPI_REGION__MPI_SENDRECV_REPLACE,
    /** Score-P region ID for MPI_Sizeof */
    SCOREP_MPI_REGION__MPI_SIZEOF,
    /** Score-P region ID for MPI_Ssend */
    SCOREP_MPI_REGION__MPI_SSEND,
    /** Score-P region ID for MPI_Ssend_init */
    SCOREP_MPI_REGION__MPI_SSEND_INIT,
    /** Score-P region ID for MPI_Start */
    SCOREP_MPI_REGION__MPI_START,
    /** Score-P region ID for MPI_Startall */
    SCOREP_MPI_REGION__MPI_STARTALL,
    /** Score-P region ID for MPI_Status_c2f */
    SCOREP_MPI_REGION__MPI_STATUS_C2F,
    /** Score-P region ID for MPI_Status_f2c */
    SCOREP_MPI_REGION__MPI_STATUS_F2C,
    /** Score-P region ID for MPI_Status_set_cancelled */
    SCOREP_MPI_REGION__MPI_STATUS_SET_CANCELLED,
    /** Score-P region ID for MPI_Status_set_elements */
    SCOREP_MPI_REGION__MPI_STATUS_SET_ELEMENTS,
    /** Score-P region ID for MPI_Status_set_elements_x */
    SCOREP_MPI_REGION__MPI_STATUS_SET_ELEMENTS_X,
    /** Score-P region ID for MPI_Test */
    SCOREP_MPI_REGION__MPI_TEST,
    /** Score-P region ID for MPI_Test_cancelled */
    SCOREP_MPI_REGION__MPI_TEST_CANCELLED,
    /** Score-P region ID for MPI_Testall */
    SCOREP_MPI_REGION__MPI_TESTALL,
    /** Score-P region ID for MPI_Testany */
    SCOREP_MPI_REGION__MPI_TESTANY,
    /** Score-P region ID for MPI_Testsome */
    SCOREP_MPI_REGION__MPI_TESTSOME,
    /** Score-P region ID for MPI_Topo_test */
    SCOREP_MPI_REGION__MPI_TOPO_TEST,
    /** Score-P region ID for MPI_Type_c2f */
    SCOREP_MPI_REGION__MPI_TYPE_C2F,
    /** Score-P region ID for MPI_Type_commit */
    SCOREP_MPI_REGION__MPI_TYPE_COMMIT,
    /** Score-P region ID for MPI_Type_contiguous */
    SCOREP_MPI_REGION__MPI_TYPE_CONTIGUOUS,
    /** Score-P region ID for MPI_Type_create_darray */
    SCOREP_MPI_REGION__MPI_TYPE_CREATE_DARRAY,
    /** Score-P region ID for MPI_Type_create_f90_complex */
    SCOREP_MPI_REGION__MPI_TYPE_CREATE_F90_COMPLEX,
    /** Score-P region ID for MPI_Type_create_f90_integer */
    SCOREP_MPI_REGION__MPI_TYPE_CREATE_F90_INTEGER,
    /** Score-P region ID for MPI_Type_create_f90_real */
    SCOREP_MPI_REGION__MPI_TYPE_CREATE_F90_REAL,
    /** Score-P region ID for MPI_Type_create_hindexed */
    SCOREP_MPI_REGION__MPI_TYPE_CREATE_HINDEXED,
    /** Score-P region ID for MPI_Type_create_hindexed_block */
    SCOREP_MPI_REGION__MPI_TYPE_CREATE_HINDEXED_BLOCK,
    /** Score-P region ID for MPI_Type_create_hvector */
    SCOREP_MPI_REGION__MPI_TYPE_CREATE_HVECTOR,
    /** Score-P region ID for MPI_Type_create_indexed_block */
    SCOREP_MPI_REGION__MPI_TYPE_CREATE_INDEXED_BLOCK,
    /** Score-P region ID for MPI_Type_create_keyval */
    SCOREP_MPI_REGION__MPI_TYPE_CREATE_KEYVAL,
    /** Score-P region ID for MPI_Type_create_resized */
    SCOREP_MPI_REGION__MPI_TYPE_CREATE_RESIZED,
    /** Score-P region ID for MPI_Type_create_struct */
    SCOREP_MPI_REGION__MPI_TYPE_CREATE_STRUCT,
    /** Score-P region ID for MPI_Type_create_subarray */
    SCOREP_MPI_REGION__MPI_TYPE_CREATE_SUBARRAY,
    /** Score-P region ID for MPI_Type_delete_attr */
    SCOREP_MPI_REGION__MPI_TYPE_DELETE_ATTR,
    /** Score-P region ID for MPI_Type_dup */
    SCOREP_MPI_REGION__MPI_TYPE_DUP,
    /** Score-P region ID for MPI_Type_extent */
    SCOREP_MPI_REGION__MPI_TYPE_EXTENT,
    /** Score-P region ID for MPI_Type_f2c */
    SCOREP_MPI_REGION__MPI_TYPE_F2C,
    /** Score-P region ID for MPI_Type_free */
    SCOREP_MPI_REGION__MPI_TYPE_FREE,
    /** Score-P region ID for MPI_Type_free_keyval */
    SCOREP_MPI_REGION__MPI_TYPE_FREE_KEYVAL,
    /** Score-P region ID for MPI_Type_get_attr */
    SCOREP_MPI_REGION__MPI_TYPE_GET_ATTR,
    /** Score-P region ID for MPI_Type_get_contents */
    SCOREP_MPI_REGION__MPI_TYPE_GET_CONTENTS,
    /** Score-P region ID for MPI_Type_get_envelope */
    SCOREP_MPI_REGION__MPI_TYPE_GET_ENVELOPE,
    /** Score-P region ID for MPI_Type_get_extent */
    SCOREP_MPI_REGION__MPI_TYPE_GET_EXTENT,
    /** Score-P region ID for MPI_Type_get_extent_x */
    SCOREP_MPI_REGION__MPI_TYPE_GET_EXTENT_X,
    /** Score-P region ID for MPI_Type_get_name */
    SCOREP_MPI_REGION__MPI_TYPE_GET_NAME,
    /** Score-P region ID for MPI_Type_get_true_extent */
    SCOREP_MPI_REGION__MPI_TYPE_GET_TRUE_EXTENT,
    /** Score-P region ID for MPI_Type_get_true_extent_x */
    SCOREP_MPI_REGION__MPI_TYPE_GET_TRUE_EXTENT_X,
    /** Score-P region ID for MPI_Type_hindexed */
    SCOREP_MPI_REGION__MPI_TYPE_HINDEXED,
    /** Score-P region ID for MPI_Type_hvector */
    SCOREP_MPI_REGION__MPI_TYPE_HVECTOR,
    /** Score-P region ID for MPI_Type_indexed */
    SCOREP_MPI_REGION__MPI_TYPE_INDEXED,
    /** Score-P region ID for MPI_Type_lb */
    SCOREP_MPI_REGION__MPI_TYPE_LB,
    /** Score-P region ID for MPI_Type_match_size */
    SCOREP_MPI_REGION__MPI_TYPE_MATCH_SIZE,
    /** Score-P region ID for MPI_Type_set_attr */
    SCOREP_MPI_REGION__MPI_TYPE_SET_ATTR,
    /** Score-P region ID for MPI_Type_set_name */
    SCOREP_MPI_REGION__MPI_TYPE_SET_NAME,
    /** Score-P region ID for MPI_Type_size */
    SCOREP_MPI_REGION__MPI_TYPE_SIZE,
    /** Score-P region ID for MPI_Type_size_x */
    SCOREP_MPI_REGION__MPI_TYPE_SIZE_X,
    /** Score-P region ID for MPI_Type_struct */
    SCOREP_MPI_REGION__MPI_TYPE_STRUCT,
    /** Score-P region ID for MPI_Type_ub */
    SCOREP_MPI_REGION__MPI_TYPE_UB,
    /** Score-P region ID for MPI_Type_vector */
    SCOREP_MPI_REGION__MPI_TYPE_VECTOR,
    /** Score-P region ID for MPI_Unpack */
    SCOREP_MPI_REGION__MPI_UNPACK,
    /** Score-P region ID for MPI_Unpack_external */
    SCOREP_MPI_REGION__MPI_UNPACK_EXTERNAL,
    /** Score-P region ID for MPI_Unpublish_name */
    SCOREP_MPI_REGION__MPI_UNPUBLISH_NAME,
    /** Score-P region ID for MPI_Wait */
    SCOREP_MPI_REGION__MPI_WAIT,
    /** Score-P region ID for MPI_Waitall */
    SCOREP_MPI_REGION__MPI_WAITALL,
    /** Score-P region ID for MPI_Waitany */
    SCOREP_MPI_REGION__MPI_WAITANY,
    /** Score-P region ID for MPI_Waitsome */
    SCOREP_MPI_REGION__MPI_WAITSOME,
    /** Score-P region ID for MPI_Win_allocate */
    SCOREP_MPI_REGION__MPI_WIN_ALLOCATE,
    /** Score-P region ID for MPI_Win_allocate_shared */
    SCOREP_MPI_REGION__MPI_WIN_ALLOCATE_SHARED,
    /** Score-P region ID for MPI_Win_attach */
    SCOREP_MPI_REGION__MPI_WIN_ATTACH,
    /** Score-P region ID for MPI_Win_c2f */
    SCOREP_MPI_REGION__MPI_WIN_C2F,
    /** Score-P region ID for MPI_Win_call_errhandler */
    SCOREP_MPI_REGION__MPI_WIN_CALL_ERRHANDLER,
    /** Score-P region ID for MPI_Win_complete */
    SCOREP_MPI_REGION__MPI_WIN_COMPLETE,
    /** Score-P region ID for MPI_Win_create */
    SCOREP_MPI_REGION__MPI_WIN_CREATE,
    /** Score-P region ID for MPI_Win_create_dynamic */
    SCOREP_MPI_REGION__MPI_WIN_CREATE_DYNAMIC,
    /** Score-P region ID for MPI_Win_create_errhandler */
    SCOREP_MPI_REGION__MPI_WIN_CREATE_ERRHANDLER,
    /** Score-P region ID for MPI_Win_create_keyval */
    SCOREP_MPI_REGION__MPI_WIN_CREATE_KEYVAL,
    /** Score-P region ID for MPI_Win_delete_attr */
    SCOREP_MPI_REGION__MPI_WIN_DELETE_ATTR,
    /** Score-P region ID for MPI_Win_detach */
    SCOREP_MPI_REGION__MPI_WIN_DETACH,
    /** Score-P region ID for MPI_Win_f2c */
    SCOREP_MPI_REGION__MPI_WIN_F2C,
    /** Score-P region ID for MPI_Win_fence */
    SCOREP_MPI_REGION__MPI_WIN_FENCE,
    /** Score-P region ID for MPI_Win_flush */
    SCOREP_MPI_REGION__MPI_WIN_FLUSH,
    /** Score-P region ID for MPI_Win_flush_all */
    SCOREP_MPI_REGION__MPI_WIN_FLUSH_ALL,
    /** Score-P region ID for MPI_Win_flush_local */
    SCOREP_MPI_REGION__MPI_WIN_FLUSH_LOCAL,
    /** Score-P region ID for MPI_Win_flush_local_all */
    SCOREP_MPI_REGION__MPI_WIN_FLUSH_LOCAL_ALL,
    /** Score-P region ID for MPI_Win_free */
    SCOREP_MPI_REGION__MPI_WIN_FREE,
    /** Score-P region ID for MPI_Win_free_keyval */
    SCOREP_MPI_REGION__MPI_WIN_FREE_KEYVAL,
    /** Score-P region ID for MPI_Win_get_attr */
    SCOREP_MPI_REGION__MPI_WIN_GET_ATTR,
    /** Score-P region ID for MPI_Win_get_errhandler */
    SCOREP_MPI_REGION__MPI_WIN_GET_ERRHANDLER,
    /** Score-P region ID for MPI_Win_get_group */
    SCOREP_MPI_REGION__MPI_WIN_GET_GROUP,
    /** Score-P region ID for MPI_Win_get_info */
    SCOREP_MPI_REGION__MPI_WIN_GET_INFO,
    /** Score-P region ID for MPI_Win_get_name */
    SCOREP_MPI_REGION__MPI_WIN_GET_NAME,
    /** Score-P region ID for MPI_Win_lock */
    SCOREP_MPI_REGION__MPI_WIN_LOCK,
    /** Score-P region ID for MPI_Win_lock_all */
    SCOREP_MPI_REGION__MPI_WIN_LOCK_ALL,
    /** Score-P region ID for MPI_Win_post */
    SCOREP_MPI_REGION__MPI_WIN_POST,
    /** Score-P region ID for MPI_Win_set_attr */
    SCOREP_MPI_REGION__MPI_WIN_SET_ATTR,
    /** Score-P region ID for MPI_Win_set_errhandler */
    SCOREP_MPI_REGION__MPI_WIN_SET_ERRHANDLER,
    /** Score-P region ID for MPI_Win_set_info */
    SCOREP_MPI_REGION__MPI_WIN_SET_INFO,
    /** Score-P region ID for MPI_Win_set_name */
    SCOREP_MPI_REGION__MPI_WIN_SET_NAME,
    /** Score-P region ID for MPI_Win_shared_query */
    SCOREP_MPI_REGION__MPI_WIN_SHARED_QUERY,
    /** Score-P region ID for MPI_Win_start */
    SCOREP_MPI_REGION__MPI_WIN_START,
    /** Score-P region ID for MPI_Win_sync */
    SCOREP_MPI_REGION__MPI_WIN_SYNC,
    /** Score-P region ID for MPI_Win_test */
    SCOREP_MPI_REGION__MPI_WIN_TEST,
    /** Score-P region ID for MPI_Win_unlock */
    SCOREP_MPI_REGION__MPI_WIN_UNLOCK,
    /** Score-P region ID for MPI_Win_unlock_all */
    SCOREP_MPI_REGION__MPI_WIN_UNLOCK_ALL,
    /** Score-P region ID for MPI_Win_wait */
    SCOREP_MPI_REGION__MPI_WIN_WAIT,
    /** Score-P region ID for MPI_Wtick */
    SCOREP_MPI_REGION__MPI_WTICK,
    /** Score-P region ID for MPI_Wtime */
    SCOREP_MPI_REGION__MPI_WTIME,

    /** Total number of regions */
    SCOREP_MPI_NUM_REGIONS
};

/** Region IDs of MPI functions */
extern SCOREP_RegionHandle scorep_mpi_regions[ SCOREP_MPI_NUM_REGIONS ];

/**
 * Register MPI functions and initialize data structures
 */
void
scorep_mpi_register_regions( void );

#endif /* SCOREP_MPI_REG_H */
