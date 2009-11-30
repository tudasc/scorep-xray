/*
 * This file is part of the SILC project (http://www.silc.de)
 *
 * Copyright (c) 2009-2011,
 *    RWTH Aachen, Germany
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *    Technische Universitaet Dresden, Germany
 *    University of Oregon, Eugene USA
 *    Forschungszentrum Juelich GmbH, Germany
 *    Technische Universitaet Muenchen, Germany
 *
 * See the COPYING file in the package base directory for details.
 *
 */


/**
 * @file  SILC_Mpiwrap_Reg.h
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 * @status     ALPHA
 * @ingroup    MPI_Wrapper
 *
 * @brief Registration of MPI functions
 * For all MPI functions a region is reistered at initialization time of the MPI adapter.
 * The dynamic region handle obtained from the measurement system is stored in an array
 * unter a fixed index for every region.
 */

#ifndef _SILC_MPIWRAP_REG_H
#define _SILC_MPIWRAP_REG_H

#ifdef __cplusplus
#   define EXTERN extern "C" /**< External C definition*/
#else
#   define EXTERN extern     /**< External definition */
#endif

#include "SILC_Definitions.h"

/*
 *-----------------------------------------------------------------------------
 *
 *  - Registration of MPI functions
 *
 *-----------------------------------------------------------------------------
 */

/** function type is not point-to-point */
#define SILC_MPI_TYPE__NONE                          0
/** function type is receive operation */
#define SILC_MPI_TYPE__RECV                          1
/** function type is send operation */
#define SILC_MPI_TYPE__SEND                          2
/** function type is send and receive operation */
#define SILC_MPI_TYPE__SENDRECV                      3
/** function type is collective */
#define SILC_MPI_TYPE__COLLECTIVE                    4

/** function has is unknown communication pattern */
#define SILC_COLL_TYPE__UNKNOWN                      1
/** function is barrier-like operation */
#define SILC_COLL_TYPE__BARRIER                      2
/** function has 1:n communication pattern */
#define SILC_COLL_TYPE__ONE2ALL                      3
/** function has n:1 communication pattern */
#define SILC_COLL_TYPE__ALL2ONE                      4
/** function has n:n communication pattern */
#define SILC_COLL_TYPE__ALL2ALL                      5
/** function may be partially synchronizing */
#define SILC_COLL_TYPE__PARTIAL                      6
/** function is implicitely synchronizing */
#define SILC_COLL_TYPE__IMPLIED                      7

/**
 * Bitpatterns for runtime wrapper enabling
 */
enum silc_mpi_groups
{
    /* pure groups, which can be specified in conf */
    SILC_MPI_ENABLED_CG        =     1,
    SILC_MPI_ENABLED_COLL      =     2,
    SILC_MPI_ENABLED_ENV       =     4,
    SILC_MPI_ENABLED_ERR       =     8,
    SILC_MPI_ENABLED_EXT       =    16,
    SILC_MPI_ENABLED_IO        =    32,
    SILC_MPI_ENABLED_MISC      =    64,
    SILC_MPI_ENABLED_P2P       =   128,
    SILC_MPI_ENABLED_RMA       =   256,
    SILC_MPI_ENABLED_SPAWN     =   512,
    SILC_MPI_ENABLED_TOPO      =  1024,
    SILC_MPI_ENABLED_TYPE      =  2048,
    /* derived groups, which are a combination of existing groups */
    SILC_MPI_ENABLED_CG_ERR    =  SILC_MPI_ENABLED_CG    | SILC_MPI_ENABLED_ERR,
    SILC_MPI_ENABLED_CG_EXT    =  SILC_MPI_ENABLED_CG    | SILC_MPI_ENABLED_EXT,
    SILC_MPI_ENABLED_CG_MISC   =  SILC_MPI_ENABLED_CG    | SILC_MPI_ENABLED_MISC,
    SILC_MPI_ENABLED_IO_ERR    =  SILC_MPI_ENABLED_IO    | SILC_MPI_ENABLED_ERR,
    SILC_MPI_ENABLED_IO_MISC   =  SILC_MPI_ENABLED_IO    | SILC_MPI_ENABLED_MISC,
    SILC_MPI_ENABLED_RMA_ERR   =  SILC_MPI_ENABLED_RMA   | SILC_MPI_ENABLED_ERR,
    SILC_MPI_ENABLED_RMA_EXT   =  SILC_MPI_ENABLED_RMA   | SILC_MPI_ENABLED_EXT,
    SILC_MPI_ENABLED_RMA_MISC  =  SILC_MPI_ENABLED_RMA   | SILC_MPI_ENABLED_MISC,
    SILC_MPI_ENABLED_TYPE_EXT  =  SILC_MPI_ENABLED_TYPE  | SILC_MPI_ENABLED_EXT,
    SILC_MPI_ENABLED_TYPE_MISC =  SILC_MPI_ENABLED_TYPE  | SILC_MPI_ENABLED_MISC,
    /* NOTE: ALL should comprise all pure groups */
    SILC_MPI_ENABLED_ALL       =  SILC_MPI_ENABLED_CG    |
                                 SILC_MPI_ENABLED_COLL  |
                                 SILC_MPI_ENABLED_ENV   |
                                 SILC_MPI_ENABLED_ERR   |
                                 SILC_MPI_ENABLED_EXT   |
                                 SILC_MPI_ENABLED_IO    |
                                 SILC_MPI_ENABLED_MISC  |
                                 SILC_MPI_ENABLED_P2P   |
                                 SILC_MPI_ENABLED_RMA   |
                                 SILC_MPI_ENABLED_SPAWN |
                                 SILC_MPI_ENABLED_TOPO  |
                                 SILC_MPI_ENABLED_TYPE,
    /* NOTE: DEFAULT should reflect the default set in 'epk_conf.c' */
    SILC_MPI_ENABLED_DEFAULT =  SILC_MPI_ENABLED_CG    |
                               SILC_MPI_ENABLED_COLL  |
                               SILC_MPI_ENABLED_ENV   |
                               SILC_MPI_ENABLED_IO    |
                               SILC_MPI_ENABLED_P2P   |
                               SILC_MPI_ENABLED_RMA   |
                               SILC_MPI_ENABLED_SPAWN |
                               SILC_MPI_ENABLED_TOPO
};

/** Bit vector for runtime measurement wrapper enabling/disabling */
EXTERN uint64_t silc_mpi_enabled;

/** SILC region ID for MPI_Abort */
#define SILC__MPI_ABORT                              0
/** SILC region ID for MPI_Accumulate */
#define SILC__MPI_ACCUMULATE                         1
/** SILC region ID for MPI_Add_error_class */
#define SILC__MPI_ADD_ERROR_CLASS                    2
/** SILC region ID for MPI_Add_error_code */
#define SILC__MPI_ADD_ERROR_CODE                     3
/** SILC region ID for MPI_Add_error_string */
#define SILC__MPI_ADD_ERROR_STRING                   4
/** SILC region ID for MPI_Address */
#define SILC__MPI_ADDRESS                            5
/** SILC region ID for MPI_Allgather */
#define SILC__MPI_ALLGATHER                          6
/** SILC region ID for MPI_Allgatherv */
#define SILC__MPI_ALLGATHERV                         7
/** SILC region ID for MPI_Alloc_mem */
#define SILC__MPI_ALLOC_MEM                          8
/** SILC region ID for MPI_Allreduce */
#define SILC__MPI_ALLREDUCE                          9
/** SILC region ID for MPI_Alltoall */
#define SILC__MPI_ALLTOALL                          10
/** SILC region ID for MPI_Alltoallv */
#define SILC__MPI_ALLTOALLV                         11
/** SILC region ID for MPI_Alltoallw */
#define SILC__MPI_ALLTOALLW                         12
/** SILC region ID for MPI_Attr_delete */
#define SILC__MPI_ATTR_DELETE                       13
/** SILC region ID for MPI_Attr_get */
#define SILC__MPI_ATTR_GET                          14
/** SILC region ID for MPI_Attr_put */
#define SILC__MPI_ATTR_PUT                          15
/** SILC region ID for MPI_Barrier */
#define SILC__MPI_BARRIER                           16
/** SILC region ID for MPI_Bcast */
#define SILC__MPI_BCAST                             17
/** SILC region ID for MPI_Bsend */
#define SILC__MPI_BSEND                             18
/** SILC region ID for MPI_Bsend_init */
#define SILC__MPI_BSEND_INIT                        19
/** SILC region ID for MPI_Buffer_attach */
#define SILC__MPI_BUFFER_ATTACH                     20
/** SILC region ID for MPI_Buffer_detach */
#define SILC__MPI_BUFFER_DETACH                     21
/** SILC region ID for MPI_Cancel */
#define SILC__MPI_CANCEL                            22
/** SILC region ID for MPI_Cart_coords */
#define SILC__MPI_CART_COORDS                       23
/** SILC region ID for MPI_Cart_create */
#define SILC__MPI_CART_CREATE                       24
/** SILC region ID for MPI_Cart_get */
#define SILC__MPI_CART_GET                          25
/** SILC region ID for MPI_Cart_map */
#define SILC__MPI_CART_MAP                          26
/** SILC region ID for MPI_Cart_rank */
#define SILC__MPI_CART_RANK                         27
/** SILC region ID for MPI_Cart_shift */
#define SILC__MPI_CART_SHIFT                        28
/** SILC region ID for MPI_Cart_sub */
#define SILC__MPI_CART_SUB                          29
/** SILC region ID for MPI_Cartdim_get */
#define SILC__MPI_CARTDIM_GET                       30
/** SILC region ID for MPI_Close_port */
#define SILC__MPI_CLOSE_PORT                        31
/** SILC region ID for MPI_Comm_accept */
#define SILC__MPI_COMM_ACCEPT                       32
/** SILC region ID for MPI_Comm_c2f */
#define SILC__MPI_COMM_C2F                          33
/** SILC region ID for MPI_Comm_call_errhandler */
#define SILC__MPI_COMM_CALL_ERRHANDLER              34
/** SILC region ID for MPI_Comm_compare */
#define SILC__MPI_COMM_COMPARE                      35
/** SILC region ID for MPI_Comm_connect */
#define SILC__MPI_COMM_CONNECT                      36
/** SILC region ID for MPI_Comm_create */
#define SILC__MPI_COMM_CREATE                       37
/** SILC region ID for MPI_Comm_create_errhandler */
#define SILC__MPI_COMM_CREATE_ERRHANDLER            38
/** SILC region ID for MPI_Comm_create_keyval */
#define SILC__MPI_COMM_CREATE_KEYVAL                39
/** SILC region ID for MPI_Comm_delete_attr */
#define SILC__MPI_COMM_DELETE_ATTR                  40
/** SILC region ID for MPI_Comm_disconnect */
#define SILC__MPI_COMM_DISCONNECT                   41
/** SILC region ID for MPI_Comm_dup */
#define SILC__MPI_COMM_DUP                          42
/** SILC region ID for MPI_Comm_f2c */
#define SILC__MPI_COMM_F2C                          43
/** SILC region ID for MPI_Comm_free */
#define SILC__MPI_COMM_FREE                         44
/** SILC region ID for MPI_Comm_free_keyval */
#define SILC__MPI_COMM_FREE_KEYVAL                  45
/** SILC region ID for MPI_Comm_get_attr */
#define SILC__MPI_COMM_GET_ATTR                     46
/** SILC region ID for MPI_Comm_get_errhandler */
#define SILC__MPI_COMM_GET_ERRHANDLER               47
/** SILC region ID for MPI_Comm_get_name */
#define SILC__MPI_COMM_GET_NAME                     48
/** SILC region ID for MPI_Comm_get_parent */
#define SILC__MPI_COMM_GET_PARENT                   49
/** SILC region ID for MPI_Comm_group */
#define SILC__MPI_COMM_GROUP                        50
/** SILC region ID for MPI_Comm_join */
#define SILC__MPI_COMM_JOIN                         51
/** SILC region ID for MPI_Comm_rank */
#define SILC__MPI_COMM_RANK                         52
/** SILC region ID for MPI_Comm_remote_group */
#define SILC__MPI_COMM_REMOTE_GROUP                 53
/** SILC region ID for MPI_Comm_remote_size */
#define SILC__MPI_COMM_REMOTE_SIZE                  54
/** SILC region ID for MPI_Comm_set_attr */
#define SILC__MPI_COMM_SET_ATTR                     55
/** SILC region ID for MPI_Comm_set_errhandler */
#define SILC__MPI_COMM_SET_ERRHANDLER               56
/** SILC region ID for MPI_Comm_set_name */
#define SILC__MPI_COMM_SET_NAME                     57
/** SILC region ID for MPI_Comm_size */
#define SILC__MPI_COMM_SIZE                         58
/** SILC region ID for MPI_Comm_spawn */
#define SILC__MPI_COMM_SPAWN                        59
/** SILC region ID for MPI_Comm_spawn_multiple */
#define SILC__MPI_COMM_SPAWN_MULTIPLE               60
/** SILC region ID for MPI_Comm_split */
#define SILC__MPI_COMM_SPLIT                        61
/** SILC region ID for MPI_Comm_test_inter */
#define SILC__MPI_COMM_TEST_INTER                   62
/** SILC region ID for MPI_Dims_create */
#define SILC__MPI_DIMS_CREATE                       63
/** SILC region ID for MPI_Errhandler_create */
#define SILC__MPI_ERRHANDLER_CREATE                 64
/** SILC region ID for MPI_Errhandler_free */
#define SILC__MPI_ERRHANDLER_FREE                   65
/** SILC region ID for MPI_Errhandler_get */
#define SILC__MPI_ERRHANDLER_GET                    66
/** SILC region ID for MPI_Errhandler_set */
#define SILC__MPI_ERRHANDLER_SET                    67
/** SILC region ID for MPI_Error_class */
#define SILC__MPI_ERROR_CLASS                       68
/** SILC region ID for MPI_Error_string */
#define SILC__MPI_ERROR_STRING                      69
/** SILC region ID for MPI_Exscan */
#define SILC__MPI_EXSCAN                            70
/** SILC region ID for MPI_File_c2f */
#define SILC__MPI_FILE_C2F                          71
/** SILC region ID for MPI_File_call_errhandler */
#define SILC__MPI_FILE_CALL_ERRHANDLER              72
/** SILC region ID for MPI_File_close */
#define SILC__MPI_FILE_CLOSE                        73
/** SILC region ID for MPI_File_create_errhandler */
#define SILC__MPI_FILE_CREATE_ERRHANDLER            74
/** SILC region ID for MPI_File_delete */
#define SILC__MPI_FILE_DELETE                       75
/** SILC region ID for MPI_File_f2c */
#define SILC__MPI_FILE_F2C                          76
/** SILC region ID for MPI_File_get_amode */
#define SILC__MPI_FILE_GET_AMODE                    77
/** SILC region ID for MPI_File_get_atomicity */
#define SILC__MPI_FILE_GET_ATOMICITY                78
/** SILC region ID for MPI_File_get_byte_offset */
#define SILC__MPI_FILE_GET_BYTE_OFFSET              79
/** SILC region ID for MPI_File_get_errhandler */
#define SILC__MPI_FILE_GET_ERRHANDLER               80
/** SILC region ID for MPI_File_get_group */
#define SILC__MPI_FILE_GET_GROUP                    81
/** SILC region ID for MPI_File_get_info */
#define SILC__MPI_FILE_GET_INFO                     82
/** SILC region ID for MPI_File_get_position */
#define SILC__MPI_FILE_GET_POSITION                 83
/** SILC region ID for MPI_File_get_position_shared */
#define SILC__MPI_FILE_GET_POSITION_SHARED          84
/** SILC region ID for MPI_File_get_size */
#define SILC__MPI_FILE_GET_SIZE                     85
/** SILC region ID for MPI_File_get_type_extent */
#define SILC__MPI_FILE_GET_TYPE_EXTENT              86
/** SILC region ID for MPI_File_get_view */
#define SILC__MPI_FILE_GET_VIEW                     87
/** SILC region ID for MPI_File_iread */
#define SILC__MPI_FILE_IREAD                        88
/** SILC region ID for MPI_File_iread_at */
#define SILC__MPI_FILE_IREAD_AT                     89
/** SILC region ID for MPI_File_iread_shared */
#define SILC__MPI_FILE_IREAD_SHARED                 90
/** SILC region ID for MPI_File_iwrite */
#define SILC__MPI_FILE_IWRITE                       91
/** SILC region ID for MPI_File_iwrite_at */
#define SILC__MPI_FILE_IWRITE_AT                    92
/** SILC region ID for MPI_File_iwrite_shared */
#define SILC__MPI_FILE_IWRITE_SHARED                93
/** SILC region ID for MPI_File_open */
#define SILC__MPI_FILE_OPEN                         94
/** SILC region ID for MPI_File_preallocate */
#define SILC__MPI_FILE_PREALLOCATE                  95
/** SILC region ID for MPI_File_read */
#define SILC__MPI_FILE_READ                         96
/** SILC region ID for MPI_File_read_all */
#define SILC__MPI_FILE_READ_ALL                     97
/** SILC region ID for MPI_File_read_all_begin */
#define SILC__MPI_FILE_READ_ALL_BEGIN               98
/** SILC region ID for MPI_File_read_all_end */
#define SILC__MPI_FILE_READ_ALL_END                 99
/** SILC region ID for MPI_File_read_at */
#define SILC__MPI_FILE_READ_AT                     100
/** SILC region ID for MPI_File_read_at_all */
#define SILC__MPI_FILE_READ_AT_ALL                 101
/** SILC region ID for MPI_File_read_at_all_begin */
#define SILC__MPI_FILE_READ_AT_ALL_BEGIN           102
/** SILC region ID for MPI_File_read_at_all_end */
#define SILC__MPI_FILE_READ_AT_ALL_END             103
/** SILC region ID for MPI_File_read_ordered */
#define SILC__MPI_FILE_READ_ORDERED                104
/** SILC region ID for MPI_File_read_ordered_begin */
#define SILC__MPI_FILE_READ_ORDERED_BEGIN          105
/** SILC region ID for MPI_File_read_ordered_end */
#define SILC__MPI_FILE_READ_ORDERED_END            106
/** SILC region ID for MPI_File_read_shared */
#define SILC__MPI_FILE_READ_SHARED                 107
/** SILC region ID for MPI_File_seek */
#define SILC__MPI_FILE_SEEK                        108
/** SILC region ID for MPI_File_seek_shared */
#define SILC__MPI_FILE_SEEK_SHARED                 109
/** SILC region ID for MPI_File_set_atomicity */
#define SILC__MPI_FILE_SET_ATOMICITY               110
/** SILC region ID for MPI_File_set_errhandler */
#define SILC__MPI_FILE_SET_ERRHANDLER              111
/** SILC region ID for MPI_File_set_info */
#define SILC__MPI_FILE_SET_INFO                    112
/** SILC region ID for MPI_File_set_size */
#define SILC__MPI_FILE_SET_SIZE                    113
/** SILC region ID for MPI_File_set_view */
#define SILC__MPI_FILE_SET_VIEW                    114
/** SILC region ID for MPI_File_sync */
#define SILC__MPI_FILE_SYNC                        115
/** SILC region ID for MPI_File_write */
#define SILC__MPI_FILE_WRITE                       116
/** SILC region ID for MPI_File_write_all */
#define SILC__MPI_FILE_WRITE_ALL                   117
/** SILC region ID for MPI_File_write_all_begin */
#define SILC__MPI_FILE_WRITE_ALL_BEGIN             118
/** SILC region ID for MPI_File_write_all_end */
#define SILC__MPI_FILE_WRITE_ALL_END               119
/** SILC region ID for MPI_File_write_at */
#define SILC__MPI_FILE_WRITE_AT                    120
/** SILC region ID for MPI_File_write_at_all */
#define SILC__MPI_FILE_WRITE_AT_ALL                121
/** SILC region ID for MPI_File_write_at_all_begin */
#define SILC__MPI_FILE_WRITE_AT_ALL_BEGIN          122
/** SILC region ID for MPI_File_write_at_all_end */
#define SILC__MPI_FILE_WRITE_AT_ALL_END            123
/** SILC region ID for MPI_File_write_ordered */
#define SILC__MPI_FILE_WRITE_ORDERED               124
/** SILC region ID for MPI_File_write_ordered_begin */
#define SILC__MPI_FILE_WRITE_ORDERED_BEGIN         125
/** SILC region ID for MPI_File_write_ordered_end */
#define SILC__MPI_FILE_WRITE_ORDERED_END           126
/** SILC region ID for MPI_File_write_shared */
#define SILC__MPI_FILE_WRITE_SHARED                127
/** SILC region ID for MPI_Finalize */
#define SILC__MPI_FINALIZE                         128
/** SILC region ID for MPI_Finalized */
#define SILC__MPI_FINALIZED                        129
/** SILC region ID for MPI_Free_mem */
#define SILC__MPI_FREE_MEM                         130
/** SILC region ID for MPI_Gather */
#define SILC__MPI_GATHER                           131
/** SILC region ID for MPI_Gatherv */
#define SILC__MPI_GATHERV                          132
/** SILC region ID for MPI_Get */
#define SILC__MPI_GET                              133
/** SILC region ID for MPI_Get_address */
#define SILC__MPI_GET_ADDRESS                      134
/** SILC region ID for MPI_Get_count */
#define SILC__MPI_GET_COUNT                        135
/** SILC region ID for MPI_Get_elements */
#define SILC__MPI_GET_ELEMENTS                     136
/** SILC region ID for MPI_Get_processor_name */
#define SILC__MPI_GET_PROCESSOR_NAME               137
/** SILC region ID for MPI_Get_version */
#define SILC__MPI_GET_VERSION                      138
/** SILC region ID for MPI_Graph_create */
#define SILC__MPI_GRAPH_CREATE                     139
/** SILC region ID for MPI_Graph_get */
#define SILC__MPI_GRAPH_GET                        140
/** SILC region ID for MPI_Graph_map */
#define SILC__MPI_GRAPH_MAP                        141
/** SILC region ID for MPI_Graph_neighbors */
#define SILC__MPI_GRAPH_NEIGHBORS                  142
/** SILC region ID for MPI_Graph_neighbors_count */
#define SILC__MPI_GRAPH_NEIGHBORS_COUNT            143
/** SILC region ID for MPI_Graphdims_get */
#define SILC__MPI_GRAPHDIMS_GET                    144
/** SILC region ID for MPI_Grequest_complete */
#define SILC__MPI_GREQUEST_COMPLETE                145
/** SILC region ID for MPI_Grequest_start */
#define SILC__MPI_GREQUEST_START                   146
/** SILC region ID for MPI_Group_c2f */
#define SILC__MPI_GROUP_C2F                        147
/** SILC region ID for MPI_Group_compare */
#define SILC__MPI_GROUP_COMPARE                    148
/** SILC region ID for MPI_Group_difference */
#define SILC__MPI_GROUP_DIFFERENCE                 149
/** SILC region ID for MPI_Group_excl */
#define SILC__MPI_GROUP_EXCL                       150
/** SILC region ID for MPI_Group_f2c */
#define SILC__MPI_GROUP_F2C                        151
/** SILC region ID for MPI_Group_free */
#define SILC__MPI_GROUP_FREE                       152
/** SILC region ID for MPI_Group_incl */
#define SILC__MPI_GROUP_INCL                       153
/** SILC region ID for MPI_Group_intersection */
#define SILC__MPI_GROUP_INTERSECTION               154
/** SILC region ID for MPI_Group_range_excl */
#define SILC__MPI_GROUP_RANGE_EXCL                 155
/** SILC region ID for MPI_Group_range_incl */
#define SILC__MPI_GROUP_RANGE_INCL                 156
/** SILC region ID for MPI_Group_rank */
#define SILC__MPI_GROUP_RANK                       157
/** SILC region ID for MPI_Group_size */
#define SILC__MPI_GROUP_SIZE                       158
/** SILC region ID for MPI_Group_translate_ranks */
#define SILC__MPI_GROUP_TRANSLATE_RANKS            159
/** SILC region ID for MPI_Group_union */
#define SILC__MPI_GROUP_UNION                      160
/** SILC region ID for MPI_Ibsend */
#define SILC__MPI_IBSEND                           161
/** SILC region ID for MPI_Info_c2f */
#define SILC__MPI_INFO_C2F                         162
/** SILC region ID for MPI_Info_create */
#define SILC__MPI_INFO_CREATE                      163
/** SILC region ID for MPI_Info_delete */
#define SILC__MPI_INFO_DELETE                      164
/** SILC region ID for MPI_Info_dup */
#define SILC__MPI_INFO_DUP                         165
/** SILC region ID for MPI_Info_f2c */
#define SILC__MPI_INFO_F2C                         166
/** SILC region ID for MPI_Info_free */
#define SILC__MPI_INFO_FREE                        167
/** SILC region ID for MPI_Info_get */
#define SILC__MPI_INFO_GET                         168
/** SILC region ID for MPI_Info_get_nkeys */
#define SILC__MPI_INFO_GET_NKEYS                   169
/** SILC region ID for MPI_Info_get_nthkey */
#define SILC__MPI_INFO_GET_NTHKEY                  170
/** SILC region ID for MPI_Info_get_valuelen */
#define SILC__MPI_INFO_GET_VALUELEN                171
/** SILC region ID for MPI_Info_set */
#define SILC__MPI_INFO_SET                         172
/** SILC region ID for MPI_Init */
#define SILC__MPI_INIT                             173
/** SILC region ID for MPI_Init_thread */
#define SILC__MPI_INIT_THREAD                      174
/** SILC region ID for MPI_Initialized */
#define SILC__MPI_INITIALIZED                      175
/** SILC region ID for MPI_Intercomm_create */
#define SILC__MPI_INTERCOMM_CREATE                 176
/** SILC region ID for MPI_Intercomm_merge */
#define SILC__MPI_INTERCOMM_MERGE                  177
/** SILC region ID for MPI_Iprobe */
#define SILC__MPI_IPROBE                           178
/** SILC region ID for MPI_Irecv */
#define SILC__MPI_IRECV                            179
/** SILC region ID for MPI_Irsend */
#define SILC__MPI_IRSEND                           180
/** SILC region ID for MPI_Is_thread_main */
#define SILC__MPI_IS_THREAD_MAIN                   181
/** SILC region ID for MPI_Isend */
#define SILC__MPI_ISEND                            182
/** SILC region ID for MPI_Issend */
#define SILC__MPI_ISSEND                           183
/** SILC region ID for MPI_Keyval_create */
#define SILC__MPI_KEYVAL_CREATE                    184
/** SILC region ID for MPI_Keyval_free */
#define SILC__MPI_KEYVAL_FREE                      185
/** SILC region ID for MPI_Lookup_name */
#define SILC__MPI_LOOKUP_NAME                      186
/** SILC region ID for MPI_Op_c2f */
#define SILC__MPI_OP_C2F                           187
/** SILC region ID for MPI_Op_create */
#define SILC__MPI_OP_CREATE                        188
/** SILC region ID for MPI_Op_f2c */
#define SILC__MPI_OP_F2C                           189
/** SILC region ID for MPI_Op_free */
#define SILC__MPI_OP_FREE                          190
/** SILC region ID for MPI_Open_port */
#define SILC__MPI_OPEN_PORT                        191
/** SILC region ID for MPI_Pack */
#define SILC__MPI_PACK                             192
/** SILC region ID for MPI_Pack_external */
#define SILC__MPI_PACK_EXTERNAL                    193
/** SILC region ID for MPI_Pack_external_size */
#define SILC__MPI_PACK_EXTERNAL_SIZE               194
/** SILC region ID for MPI_Pack_size */
#define SILC__MPI_PACK_SIZE                        195
/** SILC region ID for MPI_Probe */
#define SILC__MPI_PROBE                            196
/** SILC region ID for MPI_Publish_name */
#define SILC__MPI_PUBLISH_NAME                     197
/** SILC region ID for MPI_Put */
#define SILC__MPI_PUT                              198
/** SILC region ID for MPI_Query_thread */
#define SILC__MPI_QUERY_THREAD                     199
/** SILC region ID for MPI_Recv */
#define SILC__MPI_RECV                             200
/** SILC region ID for MPI_Recv_init */
#define SILC__MPI_RECV_INIT                        201
/** SILC region ID for MPI_Reduce */
#define SILC__MPI_REDUCE                           202
/** SILC region ID for MPI_Reduce_scatter */
#define SILC__MPI_REDUCE_SCATTER                   203
/** SILC region ID for MPI_Register_datarep */
#define SILC__MPI_REGISTER_DATAREP                 204
/** SILC region ID for MPI_Request_c2f */
#define SILC__MPI_REQUEST_C2F                      205
/** SILC region ID for MPI_Request_f2c */
#define SILC__MPI_REQUEST_F2C                      206
/** SILC region ID for MPI_Request_free */
#define SILC__MPI_REQUEST_FREE                     207
/** SILC region ID for MPI_Request_get_status */
#define SILC__MPI_REQUEST_GET_STATUS               208
/** SILC region ID for MPI_Rsend */
#define SILC__MPI_RSEND                            209
/** SILC region ID for MPI_Rsend_init */
#define SILC__MPI_RSEND_INIT                       210
/** SILC region ID for MPI_Scan */
#define SILC__MPI_SCAN                             211
/** SILC region ID for MPI_Scatter */
#define SILC__MPI_SCATTER                          212
/** SILC region ID for MPI_Scatterv */
#define SILC__MPI_SCATTERV                         213
/** SILC region ID for MPI_Send */
#define SILC__MPI_SEND                             214
/** SILC region ID for MPI_Send_init */
#define SILC__MPI_SEND_INIT                        215
/** SILC region ID for MPI_Sendrecv */
#define SILC__MPI_SENDRECV                         216
/** SILC region ID for MPI_Sendrecv_replace */
#define SILC__MPI_SENDRECV_REPLACE                 217
/** SILC region ID for MPI_Sizeof */
#define SILC__MPI_SIZEOF                           218
/** SILC region ID for MPI_Ssend */
#define SILC__MPI_SSEND                            219
/** SILC region ID for MPI_Ssend_init */
#define SILC__MPI_SSEND_INIT                       220
/** SILC region ID for MPI_Start */
#define SILC__MPI_START                            221
/** SILC region ID for MPI_Startall */
#define SILC__MPI_STARTALL                         222
/** SILC region ID for MPI_Status_c2f */
#define SILC__MPI_STATUS_C2F                       223
/** SILC region ID for MPI_Status_f2c */
#define SILC__MPI_STATUS_F2C                       224
/** SILC region ID for MPI_Status_set_cancelled */
#define SILC__MPI_STATUS_SET_CANCELLED             225
/** SILC region ID for MPI_Status_set_elements */
#define SILC__MPI_STATUS_SET_ELEMENTS              226
/** SILC region ID for MPI_Test */
#define SILC__MPI_TEST                             227
/** SILC region ID for MPI_Test_cancelled */
#define SILC__MPI_TEST_CANCELLED                   228
/** SILC region ID for MPI_Testall */
#define SILC__MPI_TESTALL                          229
/** SILC region ID for MPI_Testany */
#define SILC__MPI_TESTANY                          230
/** SILC region ID for MPI_Testsome */
#define SILC__MPI_TESTSOME                         231
/** SILC region ID for MPI_Topo_test */
#define SILC__MPI_TOPO_TEST                        232
/** SILC region ID for MPI_Type_c2f */
#define SILC__MPI_TYPE_C2F                         233
/** SILC region ID for MPI_Type_commit */
#define SILC__MPI_TYPE_COMMIT                      234
/** SILC region ID for MPI_Type_contiguous */
#define SILC__MPI_TYPE_CONTIGUOUS                  235
/** SILC region ID for MPI_Type_create_darray */
#define SILC__MPI_TYPE_CREATE_DARRAY               236
/** SILC region ID for MPI_Type_create_f90_complex */
#define SILC__MPI_TYPE_CREATE_F90_COMPLEX          237
/** SILC region ID for MPI_Type_create_f90_integer */
#define SILC__MPI_TYPE_CREATE_F90_INTEGER          238
/** SILC region ID for MPI_Type_create_f90_real */
#define SILC__MPI_TYPE_CREATE_F90_REAL             239
/** SILC region ID for MPI_Type_create_hindexed */
#define SILC__MPI_TYPE_CREATE_HINDEXED             240
/** SILC region ID for MPI_Type_create_hvector */
#define SILC__MPI_TYPE_CREATE_HVECTOR              241
/** SILC region ID for MPI_Type_create_indexed_block */
#define SILC__MPI_TYPE_CREATE_INDEXED_BLOCK        242
/** SILC region ID for MPI_Type_create_keyval */
#define SILC__MPI_TYPE_CREATE_KEYVAL               243
/** SILC region ID for MPI_Type_create_resized */
#define SILC__MPI_TYPE_CREATE_RESIZED              244
/** SILC region ID for MPI_Type_create_struct */
#define SILC__MPI_TYPE_CREATE_STRUCT               245
/** SILC region ID for MPI_Type_create_subarray */
#define SILC__MPI_TYPE_CREATE_SUBARRAY             246
/** SILC region ID for MPI_Type_delete_attr */
#define SILC__MPI_TYPE_DELETE_ATTR                 247
/** SILC region ID for MPI_Type_dup */
#define SILC__MPI_TYPE_DUP                         248
/** SILC region ID for MPI_Type_extent */
#define SILC__MPI_TYPE_EXTENT                      249
/** SILC region ID for MPI_Type_f2c */
#define SILC__MPI_TYPE_F2C                         250
/** SILC region ID for MPI_Type_free */
#define SILC__MPI_TYPE_FREE                        251
/** SILC region ID for MPI_Type_free_keyval */
#define SILC__MPI_TYPE_FREE_KEYVAL                 252
/** SILC region ID for MPI_Type_get_attr */
#define SILC__MPI_TYPE_GET_ATTR                    253
/** SILC region ID for MPI_Type_get_contents */
#define SILC__MPI_TYPE_GET_CONTENTS                254
/** SILC region ID for MPI_Type_get_envelope */
#define SILC__MPI_TYPE_GET_ENVELOPE                255
/** SILC region ID for MPI_Type_get_extent */
#define SILC__MPI_TYPE_GET_EXTENT                  256
/** SILC region ID for MPI_Type_get_name */
#define SILC__MPI_TYPE_GET_NAME                    257
/** SILC region ID for MPI_Type_get_true_extent */
#define SILC__MPI_TYPE_GET_TRUE_EXTENT             258
/** SILC region ID for MPI_Type_hindexed */
#define SILC__MPI_TYPE_HINDEXED                    259
/** SILC region ID for MPI_Type_hvector */
#define SILC__MPI_TYPE_HVECTOR                     260
/** SILC region ID for MPI_Type_indexed */
#define SILC__MPI_TYPE_INDEXED                     261
/** SILC region ID for MPI_Type_lb */
#define SILC__MPI_TYPE_LB                          262
/** SILC region ID for MPI_Type_match_size */
#define SILC__MPI_TYPE_MATCH_SIZE                  263
/** SILC region ID for MPI_Type_set_attr */
#define SILC__MPI_TYPE_SET_ATTR                    264
/** SILC region ID for MPI_Type_set_name */
#define SILC__MPI_TYPE_SET_NAME                    265
/** SILC region ID for MPI_Type_size */
#define SILC__MPI_TYPE_SIZE                        266
/** SILC region ID for MPI_Type_struct */
#define SILC__MPI_TYPE_STRUCT                      267
/** SILC region ID for MPI_Type_ub */
#define SILC__MPI_TYPE_UB                          268
/** SILC region ID for MPI_Type_vector */
#define SILC__MPI_TYPE_VECTOR                      269
/** SILC region ID for MPI_Unpack */
#define SILC__MPI_UNPACK                           270
/** SILC region ID for MPI_Unpack_external */
#define SILC__MPI_UNPACK_EXTERNAL                  271
/** SILC region ID for MPI_Unpublish_name */
#define SILC__MPI_UNPUBLISH_NAME                   272
/** SILC region ID for MPI_Wait */
#define SILC__MPI_WAIT                             273
/** SILC region ID for MPI_Waitall */
#define SILC__MPI_WAITALL                          274
/** SILC region ID for MPI_Waitany */
#define SILC__MPI_WAITANY                          275
/** SILC region ID for MPI_Waitsome */
#define SILC__MPI_WAITSOME                         276
/** SILC region ID for MPI_Win_c2f */
#define SILC__MPI_WIN_C2F                          277
/** SILC region ID for MPI_Win_call_errhandler */
#define SILC__MPI_WIN_CALL_ERRHANDLER              278
/** SILC region ID for MPI_Win_complete */
#define SILC__MPI_WIN_COMPLETE                     279
/** SILC region ID for MPI_Win_create */
#define SILC__MPI_WIN_CREATE                       280
/** SILC region ID for MPI_Win_create_errhandler */
#define SILC__MPI_WIN_CREATE_ERRHANDLER            281
/** SILC region ID for MPI_Win_create_keyval */
#define SILC__MPI_WIN_CREATE_KEYVAL                282
/** SILC region ID for MPI_Win_delete_attr */
#define SILC__MPI_WIN_DELETE_ATTR                  283
/** SILC region ID for MPI_Win_f2c */
#define SILC__MPI_WIN_F2C                          284
/** SILC region ID for MPI_Win_fence */
#define SILC__MPI_WIN_FENCE                        285
/** SILC region ID for MPI_Win_free */
#define SILC__MPI_WIN_FREE                         286
/** SILC region ID for MPI_Win_free_keyval */
#define SILC__MPI_WIN_FREE_KEYVAL                  287
/** SILC region ID for MPI_Win_get_attr */
#define SILC__MPI_WIN_GET_ATTR                     288
/** SILC region ID for MPI_Win_get_errhandler */
#define SILC__MPI_WIN_GET_ERRHANDLER               289
/** SILC region ID for MPI_Win_get_group */
#define SILC__MPI_WIN_GET_GROUP                    290
/** SILC region ID for MPI_Win_get_name */
#define SILC__MPI_WIN_GET_NAME                     291
/** SILC region ID for MPI_Win_lock */
#define SILC__MPI_WIN_LOCK                         292
/** SILC region ID for MPI_Win_post */
#define SILC__MPI_WIN_POST                         293
/** SILC region ID for MPI_Win_set_attr */
#define SILC__MPI_WIN_SET_ATTR                     294
/** SILC region ID for MPI_Win_set_errhandler */
#define SILC__MPI_WIN_SET_ERRHANDLER               295
/** SILC region ID for MPI_Win_set_name */
#define SILC__MPI_WIN_SET_NAME                     296
/** SILC region ID for MPI_Win_start */
#define SILC__MPI_WIN_START                        297
/** SILC region ID for MPI_Win_test */
#define SILC__MPI_WIN_TEST                         298
/** SILC region ID for MPI_Win_unlock */
#define SILC__MPI_WIN_UNLOCK                       299
/** SILC region ID for MPI_Win_wait */
#define SILC__MPI_WIN_WAIT                         300
/** SILC region ID for MPI_Wtick */
#define SILC__MPI_WTICK                            301
/** SILC region ID for MPI_Wtime */
#define SILC__MPI_WTIME                            302

/** Number of MPI functions HAVE */
#define SILC__MPI_NUMFUNCS                         303

/** Artificial root for MPI-only experiments when no user-code
 * instrumenation is available */
#define SILC_PARALLEL__MPI           SILC__MPI_NUMFUNCS

/** Region IDs of MPI functions */
EXTERN SILC_RegionHandle silc_mpi_regid[ SILC__MPI_NUMFUNCS + 1 ];

/**
 * Register MPI functions and initialize data structures
 */
EXTERN void
silc_mpi_register_regions();

EXTERN uint8_t
silc_is_mpi_collective( const char* str );
EXTERN uint8_t
silc_is_mpi_point2point( const char* str );
EXTERN uint8_t
silc_mpi_eventtype( const char* str );

/**
 * Enable measurement for specific subgroups of MPI functions
 */
EXTERN void
silc_mpi_enable_init();

#endif
