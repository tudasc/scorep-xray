/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2015,
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
 * unter a fixed index for every region.
 */

#ifndef _SCOREP_MPIWRAP_REG_H
#define _SCOREP_MPIWRAP_REG_H

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
/** function is implicitely synchronizing */
#define SCOREP_COLL_TYPE__IMPLIED                      7

/**
 * Bitpatterns for runtime wrapper enabling
 */
enum scorep_mpi_groups
{
    /* pure groups, which can be specified in conf */
    SCOREP_MPI_ENABLED_CG        =     1,
    SCOREP_MPI_ENABLED_COLL      =     2,
    SCOREP_MPI_ENABLED_ENV       =     4,
    SCOREP_MPI_ENABLED_ERR       =     8,
    SCOREP_MPI_ENABLED_EXT       =    16,
    SCOREP_MPI_ENABLED_IO        =    32,
    SCOREP_MPI_ENABLED_MISC      =    64,
    SCOREP_MPI_ENABLED_P2P       =   128,
    SCOREP_MPI_ENABLED_RMA       =   256,
    SCOREP_MPI_ENABLED_SPAWN     =   512,
    SCOREP_MPI_ENABLED_TOPO      =  1024,
    SCOREP_MPI_ENABLED_TYPE      =  2048,
    SCOREP_MPI_ENABLED_PERF      =  4096,
    SCOREP_MPI_ENABLED_XNONBLOCK =  8192,
    SCOREP_MPI_ENABLED_XREQTEST  = 16384,
    /* derived groups, which are a combination of existing groups */
    SCOREP_MPI_ENABLED_CG_ERR    = SCOREP_MPI_ENABLED_CG    | SCOREP_MPI_ENABLED_ERR,
    SCOREP_MPI_ENABLED_CG_EXT    = SCOREP_MPI_ENABLED_CG    | SCOREP_MPI_ENABLED_EXT,
    SCOREP_MPI_ENABLED_CG_MISC   = SCOREP_MPI_ENABLED_CG    | SCOREP_MPI_ENABLED_MISC,
    SCOREP_MPI_ENABLED_IO_ERR    = SCOREP_MPI_ENABLED_IO    | SCOREP_MPI_ENABLED_ERR,
    SCOREP_MPI_ENABLED_IO_MISC   = SCOREP_MPI_ENABLED_IO    | SCOREP_MPI_ENABLED_MISC,
    SCOREP_MPI_ENABLED_RMA_ERR   = SCOREP_MPI_ENABLED_RMA   | SCOREP_MPI_ENABLED_ERR,
    SCOREP_MPI_ENABLED_RMA_EXT   = SCOREP_MPI_ENABLED_RMA   | SCOREP_MPI_ENABLED_EXT,
    SCOREP_MPI_ENABLED_RMA_MISC  = SCOREP_MPI_ENABLED_RMA   | SCOREP_MPI_ENABLED_MISC,
    SCOREP_MPI_ENABLED_TYPE_EXT  = SCOREP_MPI_ENABLED_TYPE  | SCOREP_MPI_ENABLED_EXT,
    SCOREP_MPI_ENABLED_TYPE_MISC = SCOREP_MPI_ENABLED_TYPE  | SCOREP_MPI_ENABLED_MISC,
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

/** SCOREP region ID for MPI_Abort */
#define SCOREP__MPI_ABORT                              0
/** SCOREP region ID for MPI_Accumulate */
#define SCOREP__MPI_ACCUMULATE                         1
/** SCOREP region ID for MPI_Add_error_class */
#define SCOREP__MPI_ADD_ERROR_CLASS                    2
/** SCOREP region ID for MPI_Add_error_code */
#define SCOREP__MPI_ADD_ERROR_CODE                     3
/** SCOREP region ID for MPI_Add_error_string */
#define SCOREP__MPI_ADD_ERROR_STRING                   4
/** SCOREP region ID for MPI_Address */
#define SCOREP__MPI_ADDRESS                            5
/** SCOREP region ID for MPI_Allgather */
#define SCOREP__MPI_ALLGATHER                          6
/** SCOREP region ID for MPI_Allgatherv */
#define SCOREP__MPI_ALLGATHERV                         7
/** SCOREP region ID for MPI_Alloc_mem */
#define SCOREP__MPI_ALLOC_MEM                          8
/** SCOREP region ID for MPI_Allreduce */
#define SCOREP__MPI_ALLREDUCE                          9
/** SCOREP region ID for MPI_Alltoall */
#define SCOREP__MPI_ALLTOALL                          10
/** SCOREP region ID for MPI_Alltoallv */
#define SCOREP__MPI_ALLTOALLV                         11
/** SCOREP region ID for MPI_Alltoallw */
#define SCOREP__MPI_ALLTOALLW                         12
/** SCOREP region ID for MPI_Attr_delete */
#define SCOREP__MPI_ATTR_DELETE                       13
/** SCOREP region ID for MPI_Attr_get */
#define SCOREP__MPI_ATTR_GET                          14
/** SCOREP region ID for MPI_Attr_put */
#define SCOREP__MPI_ATTR_PUT                          15
/** SCOREP region ID for MPI_Barrier */
#define SCOREP__MPI_BARRIER                           16
/** SCOREP region ID for MPI_Bcast */
#define SCOREP__MPI_BCAST                             17
/** SCOREP region ID for MPI_Bsend */
#define SCOREP__MPI_BSEND                             18
/** SCOREP region ID for MPI_Bsend_init */
#define SCOREP__MPI_BSEND_INIT                        19
/** SCOREP region ID for MPI_Buffer_attach */
#define SCOREP__MPI_BUFFER_ATTACH                     20
/** SCOREP region ID for MPI_Buffer_detach */
#define SCOREP__MPI_BUFFER_DETACH                     21
/** SCOREP region ID for MPI_Cancel */
#define SCOREP__MPI_CANCEL                            22
/** SCOREP region ID for MPI_Cart_coords */
#define SCOREP__MPI_CART_COORDS                       23
/** SCOREP region ID for MPI_Cart_create */
#define SCOREP__MPI_CART_CREATE                       24
/** SCOREP region ID for MPI_Cart_get */
#define SCOREP__MPI_CART_GET                          25
/** SCOREP region ID for MPI_Cart_map */
#define SCOREP__MPI_CART_MAP                          26
/** SCOREP region ID for MPI_Cart_rank */
#define SCOREP__MPI_CART_RANK                         27
/** SCOREP region ID for MPI_Cart_shift */
#define SCOREP__MPI_CART_SHIFT                        28
/** SCOREP region ID for MPI_Cart_sub */
#define SCOREP__MPI_CART_SUB                          29
/** SCOREP region ID for MPI_Cartdim_get */
#define SCOREP__MPI_CARTDIM_GET                       30
/** SCOREP region ID for MPI_Close_port */
#define SCOREP__MPI_CLOSE_PORT                        31
/** SCOREP region ID for MPI_Comm_accept */
#define SCOREP__MPI_COMM_ACCEPT                       32
/** SCOREP region ID for MPI_Comm_c2f */
#define SCOREP__MPI_COMM_C2F                          33
/** SCOREP region ID for MPI_Comm_call_errhandler */
#define SCOREP__MPI_COMM_CALL_ERRHANDLER              34
/** SCOREP region ID for MPI_Comm_compare */
#define SCOREP__MPI_COMM_COMPARE                      35
/** SCOREP region ID for MPI_Comm_connect */
#define SCOREP__MPI_COMM_CONNECT                      36
/** SCOREP region ID for MPI_Comm_create */
#define SCOREP__MPI_COMM_CREATE                       37
/** SCOREP region ID for MPI_Comm_create_errhandler */
#define SCOREP__MPI_COMM_CREATE_ERRHANDLER            38
/** SCOREP region ID for MPI_Comm_create_group */
#define SCOREP__MPI_COMM_CREATE_GROUP                 39
/** SCOREP region ID for MPI_Comm_create_keyval */
#define SCOREP__MPI_COMM_CREATE_KEYVAL                40
/** SCOREP region ID for MPI_Comm_delete_attr */
#define SCOREP__MPI_COMM_DELETE_ATTR                  41
/** SCOREP region ID for MPI_Comm_disconnect */
#define SCOREP__MPI_COMM_DISCONNECT                   42
/** SCOREP region ID for MPI_Comm_dup */
#define SCOREP__MPI_COMM_DUP                          43
/** SCOREP region ID for MPI_Comm_dup_with_info */
#define SCOREP__MPI_COMM_DUP_WITH_INFO                44
/** SCOREP region ID for MPI_Comm_f2c */
#define SCOREP__MPI_COMM_F2C                          45
/** SCOREP region ID for MPI_Comm_free */
#define SCOREP__MPI_COMM_FREE                         46
/** SCOREP region ID for MPI_Comm_free_keyval */
#define SCOREP__MPI_COMM_FREE_KEYVAL                  47
/** SCOREP region ID for MPI_Comm_get_attr */
#define SCOREP__MPI_COMM_GET_ATTR                     48
/** SCOREP region ID for MPI_Comm_get_errhandler */
#define SCOREP__MPI_COMM_GET_ERRHANDLER               49
/** SCOREP region ID for MPI_Comm_get_info */
#define SCOREP__MPI_COMM_GET_INFO                     50
/** SCOREP region ID for MPI_Comm_get_name */
#define SCOREP__MPI_COMM_GET_NAME                     51
/** SCOREP region ID for MPI_Comm_get_parent */
#define SCOREP__MPI_COMM_GET_PARENT                   52
/** SCOREP region ID for MPI_Comm_group */
#define SCOREP__MPI_COMM_GROUP                        53
/** SCOREP region ID for MPI_Comm_idup */
#define SCOREP__MPI_COMM_IDUP                         54
/** SCOREP region ID for MPI_Comm_join */
#define SCOREP__MPI_COMM_JOIN                         55
/** SCOREP region ID for MPI_Comm_rank */
#define SCOREP__MPI_COMM_RANK                         56
/** SCOREP region ID for MPI_Comm_remote_group */
#define SCOREP__MPI_COMM_REMOTE_GROUP                 57
/** SCOREP region ID for MPI_Comm_remote_size */
#define SCOREP__MPI_COMM_REMOTE_SIZE                  58
/** SCOREP region ID for MPI_Comm_set_attr */
#define SCOREP__MPI_COMM_SET_ATTR                     59
/** SCOREP region ID for MPI_Comm_set_errhandler */
#define SCOREP__MPI_COMM_SET_ERRHANDLER               60
/** SCOREP region ID for MPI_Comm_set_info */
#define SCOREP__MPI_COMM_SET_INFO                     61
/** SCOREP region ID for MPI_Comm_set_name */
#define SCOREP__MPI_COMM_SET_NAME                     62
/** SCOREP region ID for MPI_Comm_size */
#define SCOREP__MPI_COMM_SIZE                         63
/** SCOREP region ID for MPI_Comm_spawn */
#define SCOREP__MPI_COMM_SPAWN                        64
/** SCOREP region ID for MPI_Comm_spawn_multiple */
#define SCOREP__MPI_COMM_SPAWN_MULTIPLE               65
/** SCOREP region ID for MPI_Comm_split */
#define SCOREP__MPI_COMM_SPLIT                        66
/** SCOREP region ID for MPI_Comm_split_type */
#define SCOREP__MPI_COMM_SPLIT_TYPE                   67
/** SCOREP region ID for MPI_Comm_test_inter */
#define SCOREP__MPI_COMM_TEST_INTER                   68
/** SCOREP region ID for MPI_Compare_and_swap */
#define SCOREP__MPI_COMPARE_AND_SWAP                  69
/** SCOREP region ID for MPI_Dims_create */
#define SCOREP__MPI_DIMS_CREATE                       70
/** SCOREP region ID for MPI_Dist_graph_create */
#define SCOREP__MPI_DIST_GRAPH_CREATE                 71
/** SCOREP region ID for MPI_Dist_graph_create_adjacent */
#define SCOREP__MPI_DIST_GRAPH_CREATE_ADJACENT        72
/** SCOREP region ID for MPI_Dist_graph_neighbors */
#define SCOREP__MPI_DIST_GRAPH_NEIGHBORS              73
/** SCOREP region ID for MPI_Dist_graph_neighbors_count */
#define SCOREP__MPI_DIST_GRAPH_NEIGHBORS_COUNT        74
/** SCOREP region ID for MPI_Errhandler_create */
#define SCOREP__MPI_ERRHANDLER_CREATE                 75
/** SCOREP region ID for MPI_Errhandler_free */
#define SCOREP__MPI_ERRHANDLER_FREE                   76
/** SCOREP region ID for MPI_Errhandler_get */
#define SCOREP__MPI_ERRHANDLER_GET                    77
/** SCOREP region ID for MPI_Errhandler_set */
#define SCOREP__MPI_ERRHANDLER_SET                    78
/** SCOREP region ID for MPI_Error_class */
#define SCOREP__MPI_ERROR_CLASS                       79
/** SCOREP region ID for MPI_Error_string */
#define SCOREP__MPI_ERROR_STRING                      80
/** SCOREP region ID for MPI_Exscan */
#define SCOREP__MPI_EXSCAN                            81
/** SCOREP region ID for MPI_Fetch_and_op */
#define SCOREP__MPI_FETCH_AND_OP                      82
/** SCOREP region ID for MPI_File_c2f */
#define SCOREP__MPI_FILE_C2F                          83
/** SCOREP region ID for MPI_File_call_errhandler */
#define SCOREP__MPI_FILE_CALL_ERRHANDLER              84
/** SCOREP region ID for MPI_File_close */
#define SCOREP__MPI_FILE_CLOSE                        85
/** SCOREP region ID for MPI_File_create_errhandler */
#define SCOREP__MPI_FILE_CREATE_ERRHANDLER            86
/** SCOREP region ID for MPI_File_delete */
#define SCOREP__MPI_FILE_DELETE                       87
/** SCOREP region ID for MPI_File_f2c */
#define SCOREP__MPI_FILE_F2C                          88
/** SCOREP region ID for MPI_File_get_amode */
#define SCOREP__MPI_FILE_GET_AMODE                    89
/** SCOREP region ID for MPI_File_get_atomicity */
#define SCOREP__MPI_FILE_GET_ATOMICITY                90
/** SCOREP region ID for MPI_File_get_byte_offset */
#define SCOREP__MPI_FILE_GET_BYTE_OFFSET              91
/** SCOREP region ID for MPI_File_get_errhandler */
#define SCOREP__MPI_FILE_GET_ERRHANDLER               92
/** SCOREP region ID for MPI_File_get_group */
#define SCOREP__MPI_FILE_GET_GROUP                    93
/** SCOREP region ID for MPI_File_get_info */
#define SCOREP__MPI_FILE_GET_INFO                     94
/** SCOREP region ID for MPI_File_get_position */
#define SCOREP__MPI_FILE_GET_POSITION                 95
/** SCOREP region ID for MPI_File_get_position_shared */
#define SCOREP__MPI_FILE_GET_POSITION_SHARED          96
/** SCOREP region ID for MPI_File_get_size */
#define SCOREP__MPI_FILE_GET_SIZE                     97
/** SCOREP region ID for MPI_File_get_type_extent */
#define SCOREP__MPI_FILE_GET_TYPE_EXTENT              98
/** SCOREP region ID for MPI_File_get_view */
#define SCOREP__MPI_FILE_GET_VIEW                     99
/** SCOREP region ID for MPI_File_iread */
#define SCOREP__MPI_FILE_IREAD                       100
/** SCOREP region ID for MPI_File_iread_all */
#define SCOREP__MPI_FILE_IREAD_ALL                   101
/** SCOREP region ID for MPI_File_iread_at */
#define SCOREP__MPI_FILE_IREAD_AT                    102
/** SCOREP region ID for MPI_File_iread_at_all */
#define SCOREP__MPI_FILE_IREAD_AT_ALL                103
/** SCOREP region ID for MPI_File_iread_shared */
#define SCOREP__MPI_FILE_IREAD_SHARED                104
/** SCOREP region ID for MPI_File_iwrite */
#define SCOREP__MPI_FILE_IWRITE                      105
/** SCOREP region ID for MPI_File_iwrite_all */
#define SCOREP__MPI_FILE_IWRITE_ALL                  106
/** SCOREP region ID for MPI_File_iwrite_at */
#define SCOREP__MPI_FILE_IWRITE_AT                   107
/** SCOREP region ID for MPI_File_iwrite_at_all */
#define SCOREP__MPI_FILE_IWRITE_AT_ALL               108
/** SCOREP region ID for MPI_File_iwrite_shared */
#define SCOREP__MPI_FILE_IWRITE_SHARED               109
/** SCOREP region ID for MPI_File_open */
#define SCOREP__MPI_FILE_OPEN                        110
/** SCOREP region ID for MPI_File_preallocate */
#define SCOREP__MPI_FILE_PREALLOCATE                 111
/** SCOREP region ID for MPI_File_read */
#define SCOREP__MPI_FILE_READ                        112
/** SCOREP region ID for MPI_File_read_all */
#define SCOREP__MPI_FILE_READ_ALL                    113
/** SCOREP region ID for MPI_File_read_all_begin */
#define SCOREP__MPI_FILE_READ_ALL_BEGIN              114
/** SCOREP region ID for MPI_File_read_all_end */
#define SCOREP__MPI_FILE_READ_ALL_END                115
/** SCOREP region ID for MPI_File_read_at */
#define SCOREP__MPI_FILE_READ_AT                     116
/** SCOREP region ID for MPI_File_read_at_all */
#define SCOREP__MPI_FILE_READ_AT_ALL                 117
/** SCOREP region ID for MPI_File_read_at_all_begin */
#define SCOREP__MPI_FILE_READ_AT_ALL_BEGIN           118
/** SCOREP region ID for MPI_File_read_at_all_end */
#define SCOREP__MPI_FILE_READ_AT_ALL_END             119
/** SCOREP region ID for MPI_File_read_ordered */
#define SCOREP__MPI_FILE_READ_ORDERED                120
/** SCOREP region ID for MPI_File_read_ordered_begin */
#define SCOREP__MPI_FILE_READ_ORDERED_BEGIN          121
/** SCOREP region ID for MPI_File_read_ordered_end */
#define SCOREP__MPI_FILE_READ_ORDERED_END            122
/** SCOREP region ID for MPI_File_read_shared */
#define SCOREP__MPI_FILE_READ_SHARED                 123
/** SCOREP region ID for MPI_File_seek */
#define SCOREP__MPI_FILE_SEEK                        124
/** SCOREP region ID for MPI_File_seek_shared */
#define SCOREP__MPI_FILE_SEEK_SHARED                 125
/** SCOREP region ID for MPI_File_set_atomicity */
#define SCOREP__MPI_FILE_SET_ATOMICITY               126
/** SCOREP region ID for MPI_File_set_errhandler */
#define SCOREP__MPI_FILE_SET_ERRHANDLER              127
/** SCOREP region ID for MPI_File_set_info */
#define SCOREP__MPI_FILE_SET_INFO                    128
/** SCOREP region ID for MPI_File_set_size */
#define SCOREP__MPI_FILE_SET_SIZE                    129
/** SCOREP region ID for MPI_File_set_view */
#define SCOREP__MPI_FILE_SET_VIEW                    130
/** SCOREP region ID for MPI_File_sync */
#define SCOREP__MPI_FILE_SYNC                        131
/** SCOREP region ID for MPI_File_write */
#define SCOREP__MPI_FILE_WRITE                       132
/** SCOREP region ID for MPI_File_write_all */
#define SCOREP__MPI_FILE_WRITE_ALL                   133
/** SCOREP region ID for MPI_File_write_all_begin */
#define SCOREP__MPI_FILE_WRITE_ALL_BEGIN             134
/** SCOREP region ID for MPI_File_write_all_end */
#define SCOREP__MPI_FILE_WRITE_ALL_END               135
/** SCOREP region ID for MPI_File_write_at */
#define SCOREP__MPI_FILE_WRITE_AT                    136
/** SCOREP region ID for MPI_File_write_at_all */
#define SCOREP__MPI_FILE_WRITE_AT_ALL                137
/** SCOREP region ID for MPI_File_write_at_all_begin */
#define SCOREP__MPI_FILE_WRITE_AT_ALL_BEGIN          138
/** SCOREP region ID for MPI_File_write_at_all_end */
#define SCOREP__MPI_FILE_WRITE_AT_ALL_END            139
/** SCOREP region ID for MPI_File_write_ordered */
#define SCOREP__MPI_FILE_WRITE_ORDERED               140
/** SCOREP region ID for MPI_File_write_ordered_begin */
#define SCOREP__MPI_FILE_WRITE_ORDERED_BEGIN         141
/** SCOREP region ID for MPI_File_write_ordered_end */
#define SCOREP__MPI_FILE_WRITE_ORDERED_END           142
/** SCOREP region ID for MPI_File_write_shared */
#define SCOREP__MPI_FILE_WRITE_SHARED                143
/** SCOREP region ID for MPI_Finalize */
#define SCOREP__MPI_FINALIZE                         144
/** SCOREP region ID for MPI_Finalized */
#define SCOREP__MPI_FINALIZED                        145
/** SCOREP region ID for MPI_Free_mem */
#define SCOREP__MPI_FREE_MEM                         146
/** SCOREP region ID for MPI_Gather */
#define SCOREP__MPI_GATHER                           147
/** SCOREP region ID for MPI_Gatherv */
#define SCOREP__MPI_GATHERV                          148
/** SCOREP region ID for MPI_Get */
#define SCOREP__MPI_GET                              149
/** SCOREP region ID for MPI_Get_accumulate */
#define SCOREP__MPI_GET_ACCUMULATE                   150
/** SCOREP region ID for MPI_Get_address */
#define SCOREP__MPI_GET_ADDRESS                      151
/** SCOREP region ID for MPI_Get_count */
#define SCOREP__MPI_GET_COUNT                        152
/** SCOREP region ID for MPI_Get_elements */
#define SCOREP__MPI_GET_ELEMENTS                     153
/** SCOREP region ID for MPI_Get_elements_x */
#define SCOREP__MPI_GET_ELEMENTS_X                   154
/** SCOREP region ID for MPI_Get_library_version */
#define SCOREP__MPI_GET_LIBRARY_VERSION              155
/** SCOREP region ID for MPI_Get_processor_name */
#define SCOREP__MPI_GET_PROCESSOR_NAME               156
/** SCOREP region ID for MPI_Get_version */
#define SCOREP__MPI_GET_VERSION                      157
/** SCOREP region ID for MPI_Graph_create */
#define SCOREP__MPI_GRAPH_CREATE                     158
/** SCOREP region ID for MPI_Graph_get */
#define SCOREP__MPI_GRAPH_GET                        159
/** SCOREP region ID for MPI_Graph_map */
#define SCOREP__MPI_GRAPH_MAP                        160
/** SCOREP region ID for MPI_Graph_neighbors */
#define SCOREP__MPI_GRAPH_NEIGHBORS                  161
/** SCOREP region ID for MPI_Graph_neighbors_count */
#define SCOREP__MPI_GRAPH_NEIGHBORS_COUNT            162
/** SCOREP region ID for MPI_Graphdims_get */
#define SCOREP__MPI_GRAPHDIMS_GET                    163
/** SCOREP region ID for MPI_Grequest_complete */
#define SCOREP__MPI_GREQUEST_COMPLETE                164
/** SCOREP region ID for MPI_Grequest_start */
#define SCOREP__MPI_GREQUEST_START                   165
/** SCOREP region ID for MPI_Group_c2f */
#define SCOREP__MPI_GROUP_C2F                        166
/** SCOREP region ID for MPI_Group_compare */
#define SCOREP__MPI_GROUP_COMPARE                    167
/** SCOREP region ID for MPI_Group_difference */
#define SCOREP__MPI_GROUP_DIFFERENCE                 168
/** SCOREP region ID for MPI_Group_excl */
#define SCOREP__MPI_GROUP_EXCL                       169
/** SCOREP region ID for MPI_Group_f2c */
#define SCOREP__MPI_GROUP_F2C                        170
/** SCOREP region ID for MPI_Group_free */
#define SCOREP__MPI_GROUP_FREE                       171
/** SCOREP region ID for MPI_Group_incl */
#define SCOREP__MPI_GROUP_INCL                       172
/** SCOREP region ID for MPI_Group_intersection */
#define SCOREP__MPI_GROUP_INTERSECTION               173
/** SCOREP region ID for MPI_Group_range_excl */
#define SCOREP__MPI_GROUP_RANGE_EXCL                 174
/** SCOREP region ID for MPI_Group_range_incl */
#define SCOREP__MPI_GROUP_RANGE_INCL                 175
/** SCOREP region ID for MPI_Group_rank */
#define SCOREP__MPI_GROUP_RANK                       176
/** SCOREP region ID for MPI_Group_size */
#define SCOREP__MPI_GROUP_SIZE                       177
/** SCOREP region ID for MPI_Group_translate_ranks */
#define SCOREP__MPI_GROUP_TRANSLATE_RANKS            178
/** SCOREP region ID for MPI_Group_union */
#define SCOREP__MPI_GROUP_UNION                      179
/** SCOREP region ID for MPI_Iallgather */
#define SCOREP__MPI_IALLGATHER                       180
/** SCOREP region ID for MPI_Iallgatherv */
#define SCOREP__MPI_IALLGATHERV                      181
/** SCOREP region ID for MPI_Iallreduce */
#define SCOREP__MPI_IALLREDUCE                       182
/** SCOREP region ID for MPI_Ialltoall */
#define SCOREP__MPI_IALLTOALL                        183
/** SCOREP region ID for MPI_Ialltoallv */
#define SCOREP__MPI_IALLTOALLV                       184
/** SCOREP region ID for MPI_Ialltoallw */
#define SCOREP__MPI_IALLTOALLW                       185
/** SCOREP region ID for MPI_Ibarrier */
#define SCOREP__MPI_IBARRIER                         186
/** SCOREP region ID for MPI_Ibcast */
#define SCOREP__MPI_IBCAST                           187
/** SCOREP region ID for MPI_Ibsend */
#define SCOREP__MPI_IBSEND                           188
/** SCOREP region ID for MPI_Iexscan */
#define SCOREP__MPI_IEXSCAN                          189
/** SCOREP region ID for MPI_Igather */
#define SCOREP__MPI_IGATHER                          190
/** SCOREP region ID for MPI_Igatherv */
#define SCOREP__MPI_IGATHERV                         191
/** SCOREP region ID for MPI_Improbe */
#define SCOREP__MPI_IMPROBE                          192
/** SCOREP region ID for MPI_Imrecv */
#define SCOREP__MPI_IMRECV                           193
/** SCOREP region ID for MPI_Ineighbor_allgather */
#define SCOREP__MPI_INEIGHBOR_ALLGATHER              194
/** SCOREP region ID for MPI_Ineighbor_allgatherv */
#define SCOREP__MPI_INEIGHBOR_ALLGATHERV             195
/** SCOREP region ID for MPI_Ineighbor_alltoall */
#define SCOREP__MPI_INEIGHBOR_ALLTOALL               196
/** SCOREP region ID for MPI_Ineighbor_alltoallv */
#define SCOREP__MPI_INEIGHBOR_ALLTOALLV              197
/** SCOREP region ID for MPI_Ineighbor_alltoallw */
#define SCOREP__MPI_INEIGHBOR_ALLTOALLW              198
/** SCOREP region ID for MPI_Info_c2f */
#define SCOREP__MPI_INFO_C2F                         199
/** SCOREP region ID for MPI_Info_create */
#define SCOREP__MPI_INFO_CREATE                      200
/** SCOREP region ID for MPI_Info_delete */
#define SCOREP__MPI_INFO_DELETE                      201
/** SCOREP region ID for MPI_Info_dup */
#define SCOREP__MPI_INFO_DUP                         202
/** SCOREP region ID for MPI_Info_f2c */
#define SCOREP__MPI_INFO_F2C                         203
/** SCOREP region ID for MPI_Info_free */
#define SCOREP__MPI_INFO_FREE                        204
/** SCOREP region ID for MPI_Info_get */
#define SCOREP__MPI_INFO_GET                         205
/** SCOREP region ID for MPI_Info_get_nkeys */
#define SCOREP__MPI_INFO_GET_NKEYS                   206
/** SCOREP region ID for MPI_Info_get_nthkey */
#define SCOREP__MPI_INFO_GET_NTHKEY                  207
/** SCOREP region ID for MPI_Info_get_valuelen */
#define SCOREP__MPI_INFO_GET_VALUELEN                208
/** SCOREP region ID for MPI_Info_set */
#define SCOREP__MPI_INFO_SET                         209
/** SCOREP region ID for MPI_Init */
#define SCOREP__MPI_INIT                             210
/** SCOREP region ID for MPI_Init_thread */
#define SCOREP__MPI_INIT_THREAD                      211
/** SCOREP region ID for MPI_Initialized */
#define SCOREP__MPI_INITIALIZED                      212
/** SCOREP region ID for MPI_Intercomm_create */
#define SCOREP__MPI_INTERCOMM_CREATE                 213
/** SCOREP region ID for MPI_Intercomm_merge */
#define SCOREP__MPI_INTERCOMM_MERGE                  214
/** SCOREP region ID for MPI_Iprobe */
#define SCOREP__MPI_IPROBE                           215
/** SCOREP region ID for MPI_Irecv */
#define SCOREP__MPI_IRECV                            216
/** SCOREP region ID for MPI_Ireduce */
#define SCOREP__MPI_IREDUCE                          217
/** SCOREP region ID for MPI_Ireduce_scatter */
#define SCOREP__MPI_IREDUCE_SCATTER                  218
/** SCOREP region ID for MPI_Ireduce_scatter_block */
#define SCOREP__MPI_IREDUCE_SCATTER_BLOCK            219
/** SCOREP region ID for MPI_Irsend */
#define SCOREP__MPI_IRSEND                           220
/** SCOREP region ID for MPI_Is_thread_main */
#define SCOREP__MPI_IS_THREAD_MAIN                   221
/** SCOREP region ID for MPI_Iscan */
#define SCOREP__MPI_ISCAN                            222
/** SCOREP region ID for MPI_Iscatter */
#define SCOREP__MPI_ISCATTER                         223
/** SCOREP region ID for MPI_Iscatterv */
#define SCOREP__MPI_ISCATTERV                        224
/** SCOREP region ID for MPI_Isend */
#define SCOREP__MPI_ISEND                            225
/** SCOREP region ID for MPI_Issend */
#define SCOREP__MPI_ISSEND                           226
/** SCOREP region ID for MPI_Keyval_create */
#define SCOREP__MPI_KEYVAL_CREATE                    227
/** SCOREP region ID for MPI_Keyval_free */
#define SCOREP__MPI_KEYVAL_FREE                      228
/** SCOREP region ID for MPI_Lookup_name */
#define SCOREP__MPI_LOOKUP_NAME                      229
/** SCOREP region ID for MPI_Mprobe */
#define SCOREP__MPI_MPROBE                           230
/** SCOREP region ID for MPI_Mrecv */
#define SCOREP__MPI_MRECV                            231
/** SCOREP region ID for MPI_Neighbor_allgather */
#define SCOREP__MPI_NEIGHBOR_ALLGATHER               232
/** SCOREP region ID for MPI_Neighbor_allgatherv */
#define SCOREP__MPI_NEIGHBOR_ALLGATHERV              233
/** SCOREP region ID for MPI_Neighbor_alltoall */
#define SCOREP__MPI_NEIGHBOR_ALLTOALL                234
/** SCOREP region ID for MPI_Neighbor_alltoallv */
#define SCOREP__MPI_NEIGHBOR_ALLTOALLV               235
/** SCOREP region ID for MPI_Neighbor_alltoallw */
#define SCOREP__MPI_NEIGHBOR_ALLTOALLW               236
/** SCOREP region ID for MPI_Op_c2f */
#define SCOREP__MPI_OP_C2F                           237
/** SCOREP region ID for MPI_Op_commutative */
#define SCOREP__MPI_OP_COMMUTATIVE                   238
/** SCOREP region ID for MPI_Op_create */
#define SCOREP__MPI_OP_CREATE                        239
/** SCOREP region ID for MPI_Op_f2c */
#define SCOREP__MPI_OP_F2C                           240
/** SCOREP region ID for MPI_Op_free */
#define SCOREP__MPI_OP_FREE                          241
/** SCOREP region ID for MPI_Open_port */
#define SCOREP__MPI_OPEN_PORT                        242
/** SCOREP region ID for MPI_Pack */
#define SCOREP__MPI_PACK                             243
/** SCOREP region ID for MPI_Pack_external */
#define SCOREP__MPI_PACK_EXTERNAL                    244
/** SCOREP region ID for MPI_Pack_external_size */
#define SCOREP__MPI_PACK_EXTERNAL_SIZE               245
/** SCOREP region ID for MPI_Pack_size */
#define SCOREP__MPI_PACK_SIZE                        246
/** SCOREP region ID for MPI_Pcontrol */
#define SCOREP__MPI_PCONTROL                         247
/** SCOREP region ID for MPI_Probe */
#define SCOREP__MPI_PROBE                            248
/** SCOREP region ID for MPI_Publish_name */
#define SCOREP__MPI_PUBLISH_NAME                     249
/** SCOREP region ID for MPI_Put */
#define SCOREP__MPI_PUT                              250
/** SCOREP region ID for MPI_Query_thread */
#define SCOREP__MPI_QUERY_THREAD                     251
/** SCOREP region ID for MPI_Raccumulate */
#define SCOREP__MPI_RACCUMULATE                      252
/** SCOREP region ID for MPI_Recv */
#define SCOREP__MPI_RECV                             253
/** SCOREP region ID for MPI_Recv_init */
#define SCOREP__MPI_RECV_INIT                        254
/** SCOREP region ID for MPI_Reduce */
#define SCOREP__MPI_REDUCE                           255
/** SCOREP region ID for MPI_Reduce_local */
#define SCOREP__MPI_REDUCE_LOCAL                     256
/** SCOREP region ID for MPI_Reduce_scatter */
#define SCOREP__MPI_REDUCE_SCATTER                   257
/** SCOREP region ID for MPI_Reduce_scatter_block */
#define SCOREP__MPI_REDUCE_SCATTER_BLOCK             258
/** SCOREP region ID for MPI_Register_datarep */
#define SCOREP__MPI_REGISTER_DATAREP                 259
/** SCOREP region ID for MPI_Request_c2f */
#define SCOREP__MPI_REQUEST_C2F                      260
/** SCOREP region ID for MPI_Request_f2c */
#define SCOREP__MPI_REQUEST_F2C                      261
/** SCOREP region ID for MPI_Request_free */
#define SCOREP__MPI_REQUEST_FREE                     262
/** SCOREP region ID for MPI_Request_get_status */
#define SCOREP__MPI_REQUEST_GET_STATUS               263
/** SCOREP region ID for MPI_Rget */
#define SCOREP__MPI_RGET                             264
/** SCOREP region ID for MPI_Rget_accumulate */
#define SCOREP__MPI_RGET_ACCUMULATE                  265
/** SCOREP region ID for MPI_Rput */
#define SCOREP__MPI_RPUT                             266
/** SCOREP region ID for MPI_Rsend */
#define SCOREP__MPI_RSEND                            267
/** SCOREP region ID for MPI_Rsend_init */
#define SCOREP__MPI_RSEND_INIT                       268
/** SCOREP region ID for MPI_Scan */
#define SCOREP__MPI_SCAN                             269
/** SCOREP region ID for MPI_Scatter */
#define SCOREP__MPI_SCATTER                          270
/** SCOREP region ID for MPI_Scatterv */
#define SCOREP__MPI_SCATTERV                         271
/** SCOREP region ID for MPI_Send */
#define SCOREP__MPI_SEND                             272
/** SCOREP region ID for MPI_Send_init */
#define SCOREP__MPI_SEND_INIT                        273
/** SCOREP region ID for MPI_Sendrecv */
#define SCOREP__MPI_SENDRECV                         274
/** SCOREP region ID for MPI_Sendrecv_replace */
#define SCOREP__MPI_SENDRECV_REPLACE                 275
/** SCOREP region ID for MPI_Sizeof */
#define SCOREP__MPI_SIZEOF                           276
/** SCOREP region ID for MPI_Ssend */
#define SCOREP__MPI_SSEND                            277
/** SCOREP region ID for MPI_Ssend_init */
#define SCOREP__MPI_SSEND_INIT                       278
/** SCOREP region ID for MPI_Start */
#define SCOREP__MPI_START                            279
/** SCOREP region ID for MPI_Startall */
#define SCOREP__MPI_STARTALL                         280
/** SCOREP region ID for MPI_Status_c2f */
#define SCOREP__MPI_STATUS_C2F                       281
/** SCOREP region ID for MPI_Status_f2c */
#define SCOREP__MPI_STATUS_F2C                       282
/** SCOREP region ID for MPI_Status_set_cancelled */
#define SCOREP__MPI_STATUS_SET_CANCELLED             283
/** SCOREP region ID for MPI_Status_set_elements */
#define SCOREP__MPI_STATUS_SET_ELEMENTS              284
/** SCOREP region ID for MPI_Status_set_elements_x */
#define SCOREP__MPI_STATUS_SET_ELEMENTS_X            285
/** SCOREP region ID for MPI_Test */
#define SCOREP__MPI_TEST                             286
/** SCOREP region ID for MPI_Test_cancelled */
#define SCOREP__MPI_TEST_CANCELLED                   287
/** SCOREP region ID for MPI_Testall */
#define SCOREP__MPI_TESTALL                          288
/** SCOREP region ID for MPI_Testany */
#define SCOREP__MPI_TESTANY                          289
/** SCOREP region ID for MPI_Testsome */
#define SCOREP__MPI_TESTSOME                         290
/** SCOREP region ID for MPI_Topo_test */
#define SCOREP__MPI_TOPO_TEST                        291
/** SCOREP region ID for MPI_Type_c2f */
#define SCOREP__MPI_TYPE_C2F                         292
/** SCOREP region ID for MPI_Type_commit */
#define SCOREP__MPI_TYPE_COMMIT                      293
/** SCOREP region ID for MPI_Type_contiguous */
#define SCOREP__MPI_TYPE_CONTIGUOUS                  294
/** SCOREP region ID for MPI_Type_create_darray */
#define SCOREP__MPI_TYPE_CREATE_DARRAY               295
/** SCOREP region ID for MPI_Type_create_f90_complex */
#define SCOREP__MPI_TYPE_CREATE_F90_COMPLEX          296
/** SCOREP region ID for MPI_Type_create_f90_integer */
#define SCOREP__MPI_TYPE_CREATE_F90_INTEGER          297
/** SCOREP region ID for MPI_Type_create_f90_real */
#define SCOREP__MPI_TYPE_CREATE_F90_REAL             298
/** SCOREP region ID for MPI_Type_create_hindexed */
#define SCOREP__MPI_TYPE_CREATE_HINDEXED             299
/** SCOREP region ID for MPI_Type_create_hindexed_block */
#define SCOREP__MPI_TYPE_CREATE_HINDEXED_BLOCK       300
/** SCOREP region ID for MPI_Type_create_hvector */
#define SCOREP__MPI_TYPE_CREATE_HVECTOR              301
/** SCOREP region ID for MPI_Type_create_indexed_block */
#define SCOREP__MPI_TYPE_CREATE_INDEXED_BLOCK        302
/** SCOREP region ID for MPI_Type_create_keyval */
#define SCOREP__MPI_TYPE_CREATE_KEYVAL               303
/** SCOREP region ID for MPI_Type_create_resized */
#define SCOREP__MPI_TYPE_CREATE_RESIZED              304
/** SCOREP region ID for MPI_Type_create_struct */
#define SCOREP__MPI_TYPE_CREATE_STRUCT               305
/** SCOREP region ID for MPI_Type_create_subarray */
#define SCOREP__MPI_TYPE_CREATE_SUBARRAY             306
/** SCOREP region ID for MPI_Type_delete_attr */
#define SCOREP__MPI_TYPE_DELETE_ATTR                 307
/** SCOREP region ID for MPI_Type_dup */
#define SCOREP__MPI_TYPE_DUP                         308
/** SCOREP region ID for MPI_Type_extent */
#define SCOREP__MPI_TYPE_EXTENT                      309
/** SCOREP region ID for MPI_Type_f2c */
#define SCOREP__MPI_TYPE_F2C                         310
/** SCOREP region ID for MPI_Type_free */
#define SCOREP__MPI_TYPE_FREE                        311
/** SCOREP region ID for MPI_Type_free_keyval */
#define SCOREP__MPI_TYPE_FREE_KEYVAL                 312
/** SCOREP region ID for MPI_Type_get_attr */
#define SCOREP__MPI_TYPE_GET_ATTR                    313
/** SCOREP region ID for MPI_Type_get_contents */
#define SCOREP__MPI_TYPE_GET_CONTENTS                314
/** SCOREP region ID for MPI_Type_get_envelope */
#define SCOREP__MPI_TYPE_GET_ENVELOPE                315
/** SCOREP region ID for MPI_Type_get_extent */
#define SCOREP__MPI_TYPE_GET_EXTENT                  316
/** SCOREP region ID for MPI_Type_get_extent_x */
#define SCOREP__MPI_TYPE_GET_EXTENT_X                317
/** SCOREP region ID for MPI_Type_get_name */
#define SCOREP__MPI_TYPE_GET_NAME                    318
/** SCOREP region ID for MPI_Type_get_true_extent */
#define SCOREP__MPI_TYPE_GET_TRUE_EXTENT             319
/** SCOREP region ID for MPI_Type_get_true_extent_x */
#define SCOREP__MPI_TYPE_GET_TRUE_EXTENT_X           320
/** SCOREP region ID for MPI_Type_hindexed */
#define SCOREP__MPI_TYPE_HINDEXED                    321
/** SCOREP region ID for MPI_Type_hvector */
#define SCOREP__MPI_TYPE_HVECTOR                     322
/** SCOREP region ID for MPI_Type_indexed */
#define SCOREP__MPI_TYPE_INDEXED                     323
/** SCOREP region ID for MPI_Type_lb */
#define SCOREP__MPI_TYPE_LB                          324
/** SCOREP region ID for MPI_Type_match_size */
#define SCOREP__MPI_TYPE_MATCH_SIZE                  325
/** SCOREP region ID for MPI_Type_set_attr */
#define SCOREP__MPI_TYPE_SET_ATTR                    326
/** SCOREP region ID for MPI_Type_set_name */
#define SCOREP__MPI_TYPE_SET_NAME                    327
/** SCOREP region ID for MPI_Type_size */
#define SCOREP__MPI_TYPE_SIZE                        328
/** SCOREP region ID for MPI_Type_size_x */
#define SCOREP__MPI_TYPE_SIZE_X                      329
/** SCOREP region ID for MPI_Type_struct */
#define SCOREP__MPI_TYPE_STRUCT                      330
/** SCOREP region ID for MPI_Type_ub */
#define SCOREP__MPI_TYPE_UB                          331
/** SCOREP region ID for MPI_Type_vector */
#define SCOREP__MPI_TYPE_VECTOR                      332
/** SCOREP region ID for MPI_Unpack */
#define SCOREP__MPI_UNPACK                           333
/** SCOREP region ID for MPI_Unpack_external */
#define SCOREP__MPI_UNPACK_EXTERNAL                  334
/** SCOREP region ID for MPI_Unpublish_name */
#define SCOREP__MPI_UNPUBLISH_NAME                   335
/** SCOREP region ID for MPI_Wait */
#define SCOREP__MPI_WAIT                             336
/** SCOREP region ID for MPI_Waitall */
#define SCOREP__MPI_WAITALL                          337
/** SCOREP region ID for MPI_Waitany */
#define SCOREP__MPI_WAITANY                          338
/** SCOREP region ID for MPI_Waitsome */
#define SCOREP__MPI_WAITSOME                         339
/** SCOREP region ID for MPI_Win_allocate */
#define SCOREP__MPI_WIN_ALLOCATE                     340
/** SCOREP region ID for MPI_Win_allocate_shared */
#define SCOREP__MPI_WIN_ALLOCATE_SHARED              341
/** SCOREP region ID for MPI_Win_attach */
#define SCOREP__MPI_WIN_ATTACH                       342
/** SCOREP region ID for MPI_Win_c2f */
#define SCOREP__MPI_WIN_C2F                          343
/** SCOREP region ID for MPI_Win_call_errhandler */
#define SCOREP__MPI_WIN_CALL_ERRHANDLER              344
/** SCOREP region ID for MPI_Win_complete */
#define SCOREP__MPI_WIN_COMPLETE                     345
/** SCOREP region ID for MPI_Win_create */
#define SCOREP__MPI_WIN_CREATE                       346
/** SCOREP region ID for MPI_Win_create_dynamic */
#define SCOREP__MPI_WIN_CREATE_DYNAMIC               348
/** SCOREP region ID for MPI_Win_create_errhandler */
#define SCOREP__MPI_WIN_CREATE_ERRHANDLER            349
/** SCOREP region ID for MPI_Win_create_keyval */
#define SCOREP__MPI_WIN_CREATE_KEYVAL                350
/** SCOREP region ID for MPI_Win_delete_attr */
#define SCOREP__MPI_WIN_DELETE_ATTR                  351
/** SCOREP region ID for MPI_Win_detach */
#define SCOREP__MPI_WIN_DETACH                       347
/** SCOREP region ID for MPI_Win_f2c */
#define SCOREP__MPI_WIN_F2C                          352
/** SCOREP region ID for MPI_Win_fence */
#define SCOREP__MPI_WIN_FENCE                        353
/** SCOREP region ID for MPI_Win_flush */
#define SCOREP__MPI_WIN_FLUSH                        366
/** SCOREP region ID for MPI_Win_flush_all */
#define SCOREP__MPI_WIN_FLUSH_ALL                    367
/** SCOREP region ID for MPI_Win_flush_local */
#define SCOREP__MPI_WIN_FLUSH_LOCAL                  368
/** SCOREP region ID for MPI_Win_flush_local_all */
#define SCOREP__MPI_WIN_FLUSH_LOCAL_ALL              369
/** SCOREP region ID for MPI_Win_free */
#define SCOREP__MPI_WIN_FREE                         354
/** SCOREP region ID for MPI_Win_free_keyval */
#define SCOREP__MPI_WIN_FREE_KEYVAL                  355
/** SCOREP region ID for MPI_Win_get_attr */
#define SCOREP__MPI_WIN_GET_ATTR                     356
/** SCOREP region ID for MPI_Win_get_errhandler */
#define SCOREP__MPI_WIN_GET_ERRHANDLER               357
/** SCOREP region ID for MPI_Win_get_group */
#define SCOREP__MPI_WIN_GET_GROUP                    358
/** SCOREP region ID for MPI_Win_get_info */
#define SCOREP__MPI_WIN_GET_INFO                     359
/** SCOREP region ID for MPI_Win_get_name */
#define SCOREP__MPI_WIN_GET_NAME                     360
/** SCOREP region ID for MPI_Win_lock */
#define SCOREP__MPI_WIN_LOCK                         361
/** SCOREP region ID for MPI_Win_lock_all */
#define SCOREP__MPI_WIN_LOCK_ALL                     362
/** SCOREP region ID for MPI_Win_post */
#define SCOREP__MPI_WIN_POST                         363
/** SCOREP region ID for MPI_Win_set_attr */
#define SCOREP__MPI_WIN_SET_ATTR                     364
/** SCOREP region ID for MPI_Win_set_errhandler */
#define SCOREP__MPI_WIN_SET_ERRHANDLER               365
/** SCOREP region ID for MPI_Win_set_info */
#define SCOREP__MPI_WIN_SET_INFO                     370
/** SCOREP region ID for MPI_Win_set_name */
#define SCOREP__MPI_WIN_SET_NAME                     371
/** SCOREP region ID for MPI_Win_shared_query */
#define SCOREP__MPI_WIN_SHARED_QUERY                 372
/** SCOREP region ID for MPI_Win_start */
#define SCOREP__MPI_WIN_START                        373
/** SCOREP region ID for MPI_Win_sync */
#define SCOREP__MPI_WIN_SYNC                         374
/** SCOREP region ID for MPI_Win_test */
#define SCOREP__MPI_WIN_TEST                         375
/** SCOREP region ID for MPI_Win_unlock */
#define SCOREP__MPI_WIN_UNLOCK                       376
/** SCOREP region ID for MPI_Win_unlock_all */
#define SCOREP__MPI_WIN_UNLOCK_ALL                   377
/** SCOREP region ID for MPI_Win_wait */
#define SCOREP__MPI_WIN_WAIT                         378
/** SCOREP region ID for MPI_Wtick */
#define SCOREP__MPI_WTICK                            379
/** SCOREP region ID for MPI_Wtime */
#define SCOREP__MPI_WTIME                            380

/** Number of MPI functions HAVE */
#define SCOREP__MPI_NUMFUNCS                         381

/** Artificial root for MPI-only experiments when no user-code
 * instrumenation is available */
#define SCOREP_PARALLEL__MPI           SCOREP__MPI_NUMFUNCS

/** Region IDs of MPI functions */
extern SCOREP_RegionHandle scorep_mpi_regid[ SCOREP__MPI_NUMFUNCS + 1 ];

/**
 * Register MPI functions and initialize data structures
 */
void
scorep_mpi_register_regions();

#endif
