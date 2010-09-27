/*
 * This file is part of the SCOREP project (http://www.scorep.de)
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
 * @file       SCOREP_Fmpi_Cg.c
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 * @status     alpha
 * @ingroup    MPI_Wrapper
 *
 * @brief Fortran interface wrappers for communicator and group functions
 */

#include <config.h>
#include "SCOREP_Fmpi.h"

/**
 * @name Uppercase names
 * @{*/
/** @def MPI_Comm_compare_U
    Exchange MPI_Comm_compare_U by MPI_COMM_COMPARE.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Comm_compare_U MPI_COMM_COMPARE

/** @def MPI_Comm_create_U
    Exchange MPI_Comm_create_U by MPI_COMM_CREATE.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Comm_create_U MPI_COMM_CREATE

/** @def MPI_Comm_dup_U
    Exchange MPI_Comm_dup_U by MPI_COMM_DUP.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Comm_dup_U MPI_COMM_DUP

/** @def MPI_Comm_free_U
    Exchange MPI_Comm_free_U by MPI_COMM_FREE.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Comm_free_U MPI_COMM_FREE

/** @def MPI_Comm_group_U
    Exchange MPI_Comm_group_U by MPI_COMM_GROUP.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Comm_group_U MPI_COMM_GROUP

/** @def MPI_Comm_rank_U
    Exchange MPI_Comm_rank_U by MPI_COMM_RANK.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Comm_rank_U MPI_COMM_RANK

/** @def MPI_Comm_remote_group_U
    Exchange MPI_Comm_remote_group_U by MPI_COMM_REMOTE_GROUP.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Comm_remote_group_U MPI_COMM_REMOTE_GROUP

/** @def MPI_Comm_remote_size_U
    Exchange MPI_Comm_remote_size_U by MPI_COMM_REMOTE_SIZE.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Comm_remote_size_U MPI_COMM_REMOTE_SIZE

/** @def MPI_Comm_size_U
    Exchange MPI_Comm_size_U by MPI_COMM_SIZE.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Comm_size_U MPI_COMM_SIZE

/** @def MPI_Comm_split_U
    Exchange MPI_Comm_split_U by MPI_COMM_SPLIT.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Comm_split_U MPI_COMM_SPLIT

/** @def MPI_Comm_test_inter_U
    Exchange MPI_Comm_test_inter_U by MPI_COMM_TEST_INTER.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Comm_test_inter_U MPI_COMM_TEST_INTER

/** @def MPI_Group_compare_U
    Exchange MPI_Group_compare_U by MPI_GROUP_COMPARE.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Group_compare_U MPI_GROUP_COMPARE

/** @def MPI_Group_difference_U
    Exchange MPI_Group_difference_U by MPI_GROUP_DIFFERENCE.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Group_difference_U MPI_GROUP_DIFFERENCE

/** @def MPI_Group_excl_U
    Exchange MPI_Group_excl_U by MPI_GROUP_EXCL.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Group_excl_U MPI_GROUP_EXCL

/** @def MPI_Group_free_U
    Exchange MPI_Group_free_U by MPI_GROUP_FREE.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Group_free_U MPI_GROUP_FREE

/** @def MPI_Group_incl_U
    Exchange MPI_Group_incl_U by MPI_GROUP_INCL.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Group_incl_U MPI_GROUP_INCL

/** @def MPI_Group_intersection_U
    Exchange MPI_Group_intersection_U by MPI_GROUP_INTERSECTION.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Group_intersection_U MPI_GROUP_INTERSECTION

/** @def MPI_Group_range_excl_U
    Exchange MPI_Group_range_excl_U by MPI_GROUP_RANGE_EXCL.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Group_range_excl_U MPI_GROUP_RANGE_EXCL

/** @def MPI_Group_range_incl_U
    Exchange MPI_Group_range_incl_U by MPI_GROUP_RANGE_INCL.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Group_range_incl_U MPI_GROUP_RANGE_INCL

/** @def MPI_Group_rank_U
    Exchange MPI_Group_rank_U by MPI_GROUP_RANK.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Group_rank_U MPI_GROUP_RANK

/** @def MPI_Group_size_U
    Exchange MPI_Group_size_U by MPI_GROUP_SIZE.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Group_size_U MPI_GROUP_SIZE

/** @def MPI_Group_translate_ranks_U
    Exchange MPI_Group_translate_ranks_U by MPI_GROUP_TRANSLATE_RANKS.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Group_translate_ranks_U MPI_GROUP_TRANSLATE_RANKS

/** @def MPI_Group_union_U
    Exchange MPI_Group_union_U by MPI_GROUP_UNION.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Group_union_U MPI_GROUP_UNION

/** @def MPI_Intercomm_create_U
    Exchange MPI_Intercomm_create_U by MPI_INTERCOMM_CREATE.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Intercomm_create_U MPI_INTERCOMM_CREATE

/** @def MPI_Intercomm_merge_U
    Exchange MPI_Intercomm_merge_U by MPI_INTERCOMM_MERGE.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Intercomm_merge_U MPI_INTERCOMM_MERGE


/** @def MPI_Comm_call_errhandler_U
    Exchange MPI_Comm_call_errhandler_U by MPI_COMM_CALL_ERRHANDLER.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Comm_call_errhandler_U MPI_COMM_CALL_ERRHANDLER

/** @def MPI_Comm_create_errhandler_U
    Exchange MPI_Comm_create_errhandler_U by MPI_COMM_CREATE_ERRHANDLER.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Comm_create_errhandler_U MPI_COMM_CREATE_ERRHANDLER

/** @def MPI_Comm_get_errhandler_U
    Exchange MPI_Comm_get_errhandler_U by MPI_COMM_GET_ERRHANDLER.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Comm_get_errhandler_U MPI_COMM_GET_ERRHANDLER

/** @def MPI_Comm_set_errhandler_U
    Exchange MPI_Comm_set_errhandler_U by MPI_COMM_SET_ERRHANDLER.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Comm_set_errhandler_U MPI_COMM_SET_ERRHANDLER


/** @def MPI_Attr_delete_U
    Exchange MPI_Attr_delete_U by MPI_ATTR_DELETE.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Attr_delete_U MPI_ATTR_DELETE

/** @def MPI_Attr_get_U
    Exchange MPI_Attr_get_U by MPI_ATTR_GET.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Attr_get_U MPI_ATTR_GET

/** @def MPI_Attr_put_U
    Exchange MPI_Attr_put_U by MPI_ATTR_PUT.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Attr_put_U MPI_ATTR_PUT

/** @def MPI_Comm_create_keyval_U
    Exchange MPI_Comm_create_keyval_U by MPI_COMM_CREATE_KEYVAL.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Comm_create_keyval_U MPI_COMM_CREATE_KEYVAL

/** @def MPI_Comm_delete_attr_U
    Exchange MPI_Comm_delete_attr_U by MPI_COMM_DELETE_ATTR.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Comm_delete_attr_U MPI_COMM_DELETE_ATTR

/** @def MPI_Comm_free_keyval_U
    Exchange MPI_Comm_free_keyval_U by MPI_COMM_FREE_KEYVAL.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Comm_free_keyval_U MPI_COMM_FREE_KEYVAL

/** @def MPI_Comm_get_attr_U
    Exchange MPI_Comm_get_attr_U by MPI_COMM_GET_ATTR.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Comm_get_attr_U MPI_COMM_GET_ATTR

/** @def MPI_Comm_get_name_U
    Exchange MPI_Comm_get_name_U by MPI_COMM_GET_NAME.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Comm_get_name_U MPI_COMM_GET_NAME

/** @def MPI_Comm_set_attr_U
    Exchange MPI_Comm_set_attr_U by MPI_COMM_SET_ATTR.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Comm_set_attr_U MPI_COMM_SET_ATTR

/** @def MPI_Comm_set_name_U
    Exchange MPI_Comm_set_name_U by MPI_COMM_SET_NAME.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Comm_set_name_U MPI_COMM_SET_NAME

/** @def MPI_Keyval_create_U
    Exchange MPI_Keyval_create_U by MPI_KEYVAL_CREATE.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Keyval_create_U MPI_KEYVAL_CREATE

/** @def MPI_Keyval_free_U
    Exchange MPI_Keyval_free_U by MPI_KEYVAL_FREE.
    It is used for the Fortran wrappers of me).
 */
#define MPI_Keyval_free_U MPI_KEYVAL_FREE



/**
 * @}
 * @name Lowercase names
 * @{
 */
/** @def MPI_Comm_compare_L
    Exchanges MPI_Comm_compare_L by mpi_comm_compare.
    It is used for the Forran wrappers of me).
 */
#define MPI_Comm_compare_L mpi_comm_compare

/** @def MPI_Comm_create_L
    Exchanges MPI_Comm_create_L by mpi_comm_create.
    It is used for the Forran wrappers of me).
 */
#define MPI_Comm_create_L mpi_comm_create

/** @def MPI_Comm_dup_L
    Exchanges MPI_Comm_dup_L by mpi_comm_dup.
    It is used for the Forran wrappers of me).
 */
#define MPI_Comm_dup_L mpi_comm_dup

/** @def MPI_Comm_free_L
    Exchanges MPI_Comm_free_L by mpi_comm_free.
    It is used for the Forran wrappers of me).
 */
#define MPI_Comm_free_L mpi_comm_free

/** @def MPI_Comm_group_L
    Exchanges MPI_Comm_group_L by mpi_comm_group.
    It is used for the Forran wrappers of me).
 */
#define MPI_Comm_group_L mpi_comm_group

/** @def MPI_Comm_rank_L
    Exchanges MPI_Comm_rank_L by mpi_comm_rank.
    It is used for the Forran wrappers of me).
 */
#define MPI_Comm_rank_L mpi_comm_rank

/** @def MPI_Comm_remote_group_L
    Exchanges MPI_Comm_remote_group_L by mpi_comm_remote_group.
    It is used for the Forran wrappers of me).
 */
#define MPI_Comm_remote_group_L mpi_comm_remote_group

/** @def MPI_Comm_remote_size_L
    Exchanges MPI_Comm_remote_size_L by mpi_comm_remote_size.
    It is used for the Forran wrappers of me).
 */
#define MPI_Comm_remote_size_L mpi_comm_remote_size

/** @def MPI_Comm_size_L
    Exchanges MPI_Comm_size_L by mpi_comm_size.
    It is used for the Forran wrappers of me).
 */
#define MPI_Comm_size_L mpi_comm_size

/** @def MPI_Comm_split_L
    Exchanges MPI_Comm_split_L by mpi_comm_split.
    It is used for the Forran wrappers of me).
 */
#define MPI_Comm_split_L mpi_comm_split

/** @def MPI_Comm_test_inter_L
    Exchanges MPI_Comm_test_inter_L by mpi_comm_test_inter.
    It is used for the Forran wrappers of me).
 */
#define MPI_Comm_test_inter_L mpi_comm_test_inter

/** @def MPI_Group_compare_L
    Exchanges MPI_Group_compare_L by mpi_group_compare.
    It is used for the Forran wrappers of me).
 */
#define MPI_Group_compare_L mpi_group_compare

/** @def MPI_Group_difference_L
    Exchanges MPI_Group_difference_L by mpi_group_difference.
    It is used for the Forran wrappers of me).
 */
#define MPI_Group_difference_L mpi_group_difference

/** @def MPI_Group_excl_L
    Exchanges MPI_Group_excl_L by mpi_group_excl.
    It is used for the Forran wrappers of me).
 */
#define MPI_Group_excl_L mpi_group_excl

/** @def MPI_Group_free_L
    Exchanges MPI_Group_free_L by mpi_group_free.
    It is used for the Forran wrappers of me).
 */
#define MPI_Group_free_L mpi_group_free

/** @def MPI_Group_incl_L
    Exchanges MPI_Group_incl_L by mpi_group_incl.
    It is used for the Forran wrappers of me).
 */
#define MPI_Group_incl_L mpi_group_incl

/** @def MPI_Group_intersection_L
    Exchanges MPI_Group_intersection_L by mpi_group_intersection.
    It is used for the Forran wrappers of me).
 */
#define MPI_Group_intersection_L mpi_group_intersection

/** @def MPI_Group_range_excl_L
    Exchanges MPI_Group_range_excl_L by mpi_group_range_excl.
    It is used for the Forran wrappers of me).
 */
#define MPI_Group_range_excl_L mpi_group_range_excl

/** @def MPI_Group_range_incl_L
    Exchanges MPI_Group_range_incl_L by mpi_group_range_incl.
    It is used for the Forran wrappers of me).
 */
#define MPI_Group_range_incl_L mpi_group_range_incl

/** @def MPI_Group_rank_L
    Exchanges MPI_Group_rank_L by mpi_group_rank.
    It is used for the Forran wrappers of me).
 */
#define MPI_Group_rank_L mpi_group_rank

/** @def MPI_Group_size_L
    Exchanges MPI_Group_size_L by mpi_group_size.
    It is used for the Forran wrappers of me).
 */
#define MPI_Group_size_L mpi_group_size

/** @def MPI_Group_translate_ranks_L
    Exchanges MPI_Group_translate_ranks_L by mpi_group_translate_ranks.
    It is used for the Forran wrappers of me).
 */
#define MPI_Group_translate_ranks_L mpi_group_translate_ranks

/** @def MPI_Group_union_L
    Exchanges MPI_Group_union_L by mpi_group_union.
    It is used for the Forran wrappers of me).
 */
#define MPI_Group_union_L mpi_group_union

/** @def MPI_Intercomm_create_L
    Exchanges MPI_Intercomm_create_L by mpi_intercomm_create.
    It is used for the Forran wrappers of me).
 */
#define MPI_Intercomm_create_L mpi_intercomm_create

/** @def MPI_Intercomm_merge_L
    Exchanges MPI_Intercomm_merge_L by mpi_intercomm_merge.
    It is used for the Forran wrappers of me).
 */
#define MPI_Intercomm_merge_L mpi_intercomm_merge


/** @def MPI_Comm_call_errhandler_L
    Exchanges MPI_Comm_call_errhandler_L by mpi_comm_call_errhandler.
    It is used for the Forran wrappers of me).
 */
#define MPI_Comm_call_errhandler_L mpi_comm_call_errhandler

/** @def MPI_Comm_create_errhandler_L
    Exchanges MPI_Comm_create_errhandler_L by mpi_comm_create_errhandler.
    It is used for the Forran wrappers of me).
 */
#define MPI_Comm_create_errhandler_L mpi_comm_create_errhandler

/** @def MPI_Comm_get_errhandler_L
    Exchanges MPI_Comm_get_errhandler_L by mpi_comm_get_errhandler.
    It is used for the Forran wrappers of me).
 */
#define MPI_Comm_get_errhandler_L mpi_comm_get_errhandler

/** @def MPI_Comm_set_errhandler_L
    Exchanges MPI_Comm_set_errhandler_L by mpi_comm_set_errhandler.
    It is used for the Forran wrappers of me).
 */
#define MPI_Comm_set_errhandler_L mpi_comm_set_errhandler


/** @def MPI_Attr_delete_L
    Exchanges MPI_Attr_delete_L by mpi_attr_delete.
    It is used for the Forran wrappers of me).
 */
#define MPI_Attr_delete_L mpi_attr_delete

/** @def MPI_Attr_get_L
    Exchanges MPI_Attr_get_L by mpi_attr_get.
    It is used for the Forran wrappers of me).
 */
#define MPI_Attr_get_L mpi_attr_get

/** @def MPI_Attr_put_L
    Exchanges MPI_Attr_put_L by mpi_attr_put.
    It is used for the Forran wrappers of me).
 */
#define MPI_Attr_put_L mpi_attr_put

/** @def MPI_Comm_create_keyval_L
    Exchanges MPI_Comm_create_keyval_L by mpi_comm_create_keyval.
    It is used for the Forran wrappers of me).
 */
#define MPI_Comm_create_keyval_L mpi_comm_create_keyval

/** @def MPI_Comm_delete_attr_L
    Exchanges MPI_Comm_delete_attr_L by mpi_comm_delete_attr.
    It is used for the Forran wrappers of me).
 */
#define MPI_Comm_delete_attr_L mpi_comm_delete_attr

/** @def MPI_Comm_free_keyval_L
    Exchanges MPI_Comm_free_keyval_L by mpi_comm_free_keyval.
    It is used for the Forran wrappers of me).
 */
#define MPI_Comm_free_keyval_L mpi_comm_free_keyval

/** @def MPI_Comm_get_attr_L
    Exchanges MPI_Comm_get_attr_L by mpi_comm_get_attr.
    It is used for the Forran wrappers of me).
 */
#define MPI_Comm_get_attr_L mpi_comm_get_attr

/** @def MPI_Comm_get_name_L
    Exchanges MPI_Comm_get_name_L by mpi_comm_get_name.
    It is used for the Forran wrappers of me).
 */
#define MPI_Comm_get_name_L mpi_comm_get_name

/** @def MPI_Comm_set_attr_L
    Exchanges MPI_Comm_set_attr_L by mpi_comm_set_attr.
    It is used for the Forran wrappers of me).
 */
#define MPI_Comm_set_attr_L mpi_comm_set_attr

/** @def MPI_Comm_set_name_L
    Exchanges MPI_Comm_set_name_L by mpi_comm_set_name.
    It is used for the Forran wrappers of me).
 */
#define MPI_Comm_set_name_L mpi_comm_set_name

/** @def MPI_Keyval_create_L
    Exchanges MPI_Keyval_create_L by mpi_keyval_create.
    It is used for the Forran wrappers of me).
 */
#define MPI_Keyval_create_L mpi_keyval_create

/** @def MPI_Keyval_free_L
    Exchanges MPI_Keyval_free_L by mpi_keyval_free.
    It is used for the Forran wrappers of me).
 */
#define MPI_Keyval_free_L mpi_keyval_free



#ifndef NEED_F2C_CONV

/* If MPI_Comm_f2c is HAVE as a simple macro (which typically does nothing)
 * as it would require a "real" function if it is really needed
 * => we can save the f2c and c2s conversions */

/**
 * @}
 * @name Fortran wrappers for communicator constructors
 * @{
 */

#if HAVE( DECL_PMPI_COMM_CREATE )
/**
 * Measurement wrapper for MPI_Comm_create
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup cg
 */
void
FSUB( MPI_Comm_create )( MPI_Comm*  comm,
                         MPI_Group* group,
                         MPI_Comm*  newcomm,
                         int*       ierr )
{
    *ierr = MPI_Comm_create( *comm, *group, newcomm );
}
#endif
#if HAVE( DECL_PMPI_COMM_DUP )
/**
 * Measurement wrapper for MPI_Comm_dup
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup cg
 */
void
FSUB( MPI_Comm_dup )( MPI_Comm* comm,
                      MPI_Comm* newcomm,
                      int*      ierr )
{
    *ierr = MPI_Comm_dup( *comm, newcomm );
}
#endif
#if HAVE( DECL_PMPI_COMM_SPLIT )
/**
 * Measurement wrapper for MPI_Comm_split
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup cg
 */
void
FSUB( MPI_Comm_split )( MPI_Comm* comm,
                        int*      color,
                        int*      key,
                        MPI_Comm* newcomm,
                        int*      ierr )
{
    *ierr = MPI_Comm_split( *comm, *color, *key, newcomm );
}
#endif
#if HAVE( DECL_PMPI_INTERCOMM_CREATE )
/**
 * Measurement wrapper for MPI_Intercomm_create
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup cg
 */
void
FSUB( MPI_Intercomm_create )( MPI_Comm* local_comm,
                              int*      local_leader,
                              MPI_Comm* peer_comm,
                              int*      remote_leader,
                              int*      tag,
                              MPI_Comm* newcomm,
                              int*      ierr )
{
    *ierr = MPI_Intercomm_create( *local_comm, *local_leader, *peer_comm, *remote_leader, *tag, newcomm );
}
#endif
#if HAVE( DECL_PMPI_INTERCOMM_MERGE )
/**
 * Measurement wrapper for MPI_Intercomm_merge
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup cg
 */
void
FSUB( MPI_Intercomm_merge )( MPI_Comm* intercomm,
                             int*      high,
                             MPI_Comm* newcomm,
                             int*      ierr )
{
    *ierr = MPI_Intercomm_merge( *intercomm, *high, newcomm );
}
#endif

/**
 * @}
 * @name Fortran wrappers for communicator destructors
 * @{
 */

#if HAVE( DECL_PMPI_COMM_FREE )
/**
 * Measurement wrapper for MPI_Comm_free
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup cg
 */
void
FSUB( MPI_Comm_free )( MPI_Comm* comm,
                       int*      ierr )
{
    *ierr = MPI_Comm_free( comm );
}
#endif

/**
 * @}
 * @name Fortran wrappers for group constructors
 * @{
 */

#if HAVE( DECL_PMPI_GROUP_DIFFERENCE ) && !defined( SCOREP_MPI_NO_CG ) && !defined( MPI_Group_difference )
/**
 * Measurement wrapper for MPI_Group_difference
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup cg
 */
void
FSUB( MPI_Group_difference )( MPI_Group* group1,
                              MPI_Group* group2,
                              MPI_Group* newgroup,
                              int*       ierr )
{
    *ierr = MPI_Group_difference( *group1, *group2, newgroup );
}
#endif
#if HAVE( DECL_PMPI_GROUP_EXCL ) && !defined( SCOREP_MPI_NO_CG ) && !defined( MPI_Group_excl )
/**
 * Measurement wrapper for MPI_Group_excl
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup cg
 */
void
FSUB( MPI_Group_excl )( MPI_Group* group,
                        int*       n,
                        int*       ranks,
                        MPI_Group* newgroup,
                        int*       ierr )
{
    *ierr = MPI_Group_excl( *group, *n, ranks, newgroup );
}
#endif
#if HAVE( DECL_PMPI_GROUP_INCL ) && !defined( SCOREP_MPI_NO_CG ) && !defined( MPI_Group_incl )
/**
 * Measurement wrapper for MPI_Group_incl
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup cg
 */
void
FSUB( MPI_Group_incl )( MPI_Group* group,
                        int*       n,
                        int*       ranks,
                        MPI_Group* newgroup,
                        int*       ierr )
{
    *ierr = MPI_Group_incl( *group, *n, ranks, newgroup );
}
#endif
#if HAVE( DECL_PMPI_GROUP_INTERSECTION ) && !defined( SCOREP_MPI_NO_CG ) && !defined( MPI_Group_intersection )
/**
 * Measurement wrapper for MPI_Group_intersection
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup cg
 */
void
FSUB( MPI_Group_intersection )( MPI_Group* group1,
                                MPI_Group* group2,
                                MPI_Group* newgroup,
                                int*       ierr )
{
    *ierr = MPI_Group_intersection( *group1, *group2, newgroup );
}
#endif
#if HAVE( DECL_PMPI_GROUP_RANGE_EXCL ) && !defined( SCOREP_MPI_NO_CG ) && !defined( MPI_Group_range_excl )
/**
 * Measurement wrapper for MPI_Group_range_excl
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup cg
 */
void
FSUB( MPI_Group_range_excl )( MPI_Group* group,
                              int*       n,
                              int        ranges[][ 3 ],
                              MPI_Group* newgroup,
                              int*       ierr )
{
    *ierr = MPI_Group_range_excl( *group, *n, ranges, newgroup );
}
#endif
#if HAVE( DECL_PMPI_GROUP_RANGE_INCL ) && !defined( SCOREP_MPI_NO_CG ) && !defined( MPI_Group_range_incl )
/**
 * Measurement wrapper for MPI_Group_range_incl
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup cg
 */
void
FSUB( MPI_Group_range_incl )( MPI_Group* group,
                              int*       n,
                              int        ranges[][ 3 ],
                              MPI_Group* newgroup,
                              int*       ierr )
{
    *ierr = MPI_Group_range_incl( *group, *n, ranges, newgroup );
}
#endif
#if HAVE( DECL_PMPI_GROUP_UNION ) && !defined( SCOREP_MPI_NO_CG ) && !defined( MPI_Group_union )
/**
 * Measurement wrapper for MPI_Group_union
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup cg
 */
void
FSUB( MPI_Group_union )( MPI_Group* group1,
                         MPI_Group* group2,
                         MPI_Group* newgroup,
                         int*       ierr )
{
    *ierr = MPI_Group_union( *group1, *group2, newgroup );
}
#endif
#if HAVE( DECL_PMPI_COMM_GROUP )
/**
 * Measurement wrapper for MPI_Comm_group
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup cg
 */
void
FSUB( MPI_Comm_group )( MPI_Comm*  comm,
                        MPI_Group* group,
                        int*       ierr )
{
    *ierr = MPI_Comm_group( *comm, group );
}
#endif
#if HAVE( DECL_PMPI_COMM_REMOTE_GROUP )
/**
 * Measurement wrapper for MPI_Comm_remote_group
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup cg
 */
void
FSUB( MPI_Comm_remote_group )( MPI_Comm*  comm,
                               MPI_Group* group,
                               int*       ierr )
{
    *ierr = MPI_Comm_remote_group( *comm, group );
}
#endif

/**
 * @}
 * @name Fortran wrappers for group destructors
 * @{
 */

#if HAVE( DECL_PMPI_GROUP_FREE ) && !defined( SCOREP_MPI_NO_CG ) && !defined( MPI_Group_free )
/**
 * Measurement wrapper for MPI_Group_free
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup cg
 */
void
FSUB( MPI_Group_free )( MPI_Group* group,
                        int*       ierr )
{
    *ierr = MPI_Group_free( group );
}
#endif

/**
 * @}
 * @name Fortran wrappers for miscelaneous functions
 * @{
 */

#if HAVE( DECL_PMPI_COMM_COMPARE ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_CG ) && !defined( MPI_Comm_compare )
/**
 * Measurement wrapper for MPI_Comm_compare
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup cg
 */
void
FSUB( MPI_Comm_compare )( MPI_Comm* comm1,
                          MPI_Comm* comm2,
                          int*      result,
                          int*      ierr )
{
    *ierr = MPI_Comm_compare( *comm1, *comm2, result );
}
#endif
#if HAVE( DECL_PMPI_COMM_RANK ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_MINI ) && !defined( SCOREP_MPI_NO_CG ) && !defined( MPI_Comm_rank )
/**
 * Measurement wrapper for MPI_Comm_rank
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup cg
 */
void
FSUB( MPI_Comm_rank )( MPI_Comm* comm,
                       int*      rank,
                       int*      ierr )
{
    *ierr = MPI_Comm_rank( *comm, rank );
}
#endif
#if HAVE( DECL_PMPI_COMM_REMOTE_SIZE ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_MINI ) && !defined( SCOREP_MPI_NO_CG ) && !defined( MPI_Comm_remote_size )
/**
 * Measurement wrapper for MPI_Comm_remote_size
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup cg
 */
void
FSUB( MPI_Comm_remote_size )( MPI_Comm* comm,
                              int*      size,
                              int*      ierr )
{
    *ierr = MPI_Comm_remote_size( *comm, size );
}
#endif
#if HAVE( DECL_PMPI_COMM_SIZE ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_MINI ) && !defined( SCOREP_MPI_NO_CG ) && !defined( MPI_Comm_size )
/**
 * Measurement wrapper for MPI_Comm_size
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup cg
 */
void
FSUB( MPI_Comm_size )( MPI_Comm* comm,
                       int*      size,
                       int*      ierr )
{
    *ierr = MPI_Comm_size( *comm, size );
}
#endif
#if HAVE( DECL_PMPI_COMM_TEST_INTER ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_CG ) && !defined( MPI_Comm_test_inter )
/**
 * Measurement wrapper for MPI_Comm_test_inter
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup cg
 */
void
FSUB( MPI_Comm_test_inter )( MPI_Comm* comm,
                             int*      flag,
                             int*      ierr )
{
    *ierr = MPI_Comm_test_inter( *comm, flag );
}
#endif
#if HAVE( DECL_PMPI_GROUP_COMPARE ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_CG ) && !defined( MPI_Group_compare )
/**
 * Measurement wrapper for MPI_Group_compare
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup cg
 */
void
FSUB( MPI_Group_compare )( MPI_Group* group1,
                           MPI_Group* group2,
                           int*       result,
                           int*       ierr )
{
    *ierr = MPI_Group_compare( *group1, *group2, result );
}
#endif
#if HAVE( DECL_PMPI_GROUP_RANK ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_CG ) && !defined( MPI_Group_rank )
/**
 * Measurement wrapper for MPI_Group_rank
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup cg
 */
void
FSUB( MPI_Group_rank )( MPI_Group* group,
                        int*       rank,
                        int*       ierr )
{
    *ierr = MPI_Group_rank( *group, rank );
}
#endif
#if HAVE( DECL_PMPI_GROUP_SIZE ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_CG ) && !defined( MPI_Group_size )
/**
 * Measurement wrapper for MPI_Group_size
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup cg
 */
void
FSUB( MPI_Group_size )( MPI_Group* group,
                        int*       size,
                        int*       ierr )
{
    *ierr = MPI_Group_size( *group, size );
}
#endif
#if HAVE( DECL_PMPI_GROUP_TRANSLATE_RANKS ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_CG ) && !defined( MPI_Group_translate_ranks )
/**
 * Measurement wrapper for MPI_Group_translate_ranks
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup cg
 */
void
FSUB( MPI_Group_translate_ranks )( MPI_Group* group1,
                                   int*       n,
                                   int*       ranks1,
                                   MPI_Group* group2,
                                   int*       ranks2,
                                   int*       ierr )
{
    *ierr = MPI_Group_translate_ranks( *group1, *n, ranks1, *group2, ranks2 );
}
#endif

/**
 * @}
 * @name Fortran wrappers for error handling functions
 * @{
 */

#if HAVE( DECL_PMPI_COMM_CALL_ERRHANDLER ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_CG ) && !defined( SCOREP_MPI_NO_ERR ) && !defined( MPI_Comm_call_errhandler )
/**
 * Measurement wrapper for MPI_Comm_call_errhandler
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup cg_err
 */
void
FSUB( MPI_Comm_call_errhandler )( MPI_Comm* comm,
                                  int*      errorcode,
                                  int*      ierr )
{
    *ierr = MPI_Comm_call_errhandler( *comm, *errorcode );
}
#endif
#if HAVE( DECL_PMPI_COMM_CREATE_ERRHANDLER ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_CG ) && !defined( SCOREP_MPI_NO_ERR ) && !defined( MPI_Comm_create_errhandler )
/**
 * Measurement wrapper for MPI_Comm_create_errhandler
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup cg_err
 */
void
FSUB( MPI_Comm_create_errhandler )( MPI_Comm_errhandler_fn* function,
                                    MPI_Errhandler*         errhandler,
                                    int*                    ierr )
{
    *ierr = MPI_Comm_create_errhandler( function, errhandler );
}
#endif
#if HAVE( DECL_PMPI_COMM_GET_ERRHANDLER ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_CG ) && !defined( SCOREP_MPI_NO_ERR ) && !defined( MPI_Comm_get_errhandler )
/**
 * Measurement wrapper for MPI_Comm_get_errhandler
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup cg_err
 */
void
FSUB( MPI_Comm_get_errhandler )( MPI_Comm*       comm,
                                 MPI_Errhandler* errhandler,
                                 int*            ierr )
{
    *ierr = MPI_Comm_get_errhandler( *comm, errhandler );
}
#endif
#if HAVE( DECL_PMPI_COMM_SET_ERRHANDLER ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_CG ) && !defined( SCOREP_MPI_NO_ERR ) && !defined( MPI_Comm_set_errhandler )
/**
 * Measurement wrapper for MPI_Comm_set_errhandler
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup cg_err
 */
void
FSUB( MPI_Comm_set_errhandler )( MPI_Comm*       comm,
                                 MPI_Errhandler* errhandler,
                                 int*            ierr )
{
    *ierr = MPI_Comm_set_errhandler( *comm, *errhandler );
}
#endif

/**
 * @}
 * @name Fortran wrappers for external interface functions
 * @{
 */

#if HAVE( DECL_PMPI_ATTR_DELETE ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_CG ) && !defined( SCOREP_MPI_NO_EXT ) && !defined( MPI_Attr_delete )
/**
 * Measurement wrapper for MPI_Attr_delete
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup cg_ext
 */
void
FSUB( MPI_Attr_delete )( MPI_Comm* comm,
                         int*      keyval,
                         int*      ierr )
{
    *ierr = MPI_Attr_delete( *comm, *keyval );
}
#endif
#if HAVE( DECL_PMPI_ATTR_GET ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_CG ) && !defined( SCOREP_MPI_NO_EXT ) && !defined( MPI_Attr_get )
/**
 * Measurement wrapper for MPI_Attr_get
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup cg_ext
 */
void
FSUB( MPI_Attr_get )( MPI_Comm* comm,
                      int*      keyval,
                      void*     attribute_val,
                      int*      flag,
                      int*      ierr )
{
    *ierr = MPI_Attr_get( *comm, *keyval, attribute_val, flag );
}
#endif
#if HAVE( DECL_PMPI_ATTR_PUT ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_CG ) && !defined( SCOREP_MPI_NO_EXT ) && !defined( MPI_Attr_put )
/**
 * Measurement wrapper for MPI_Attr_put
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup cg_ext
 */
void
FSUB( MPI_Attr_put )( MPI_Comm* comm,
                      int*      keyval,
                      void*     attribute_val,
                      int*      ierr )
{
    *ierr = MPI_Attr_put( *comm, *keyval, attribute_val );
}
#endif
#if HAVE( DECL_PMPI_COMM_CREATE_KEYVAL ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_CG ) && !defined( SCOREP_MPI_NO_EXT ) && !defined( MPI_Comm_create_keyval )
/**
 * Measurement wrapper for MPI_Comm_create_keyval
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup cg_ext
 */
void
FSUB( MPI_Comm_create_keyval )( MPI_Comm_copy_attr_function*   comm_copy_attr_fn,
                                MPI_Comm_delete_attr_function* comm_delete_attr_fn,
                                int*                           comm_keyval,
                                void*                          extra_state,
                                int*                           ierr )
{
    *ierr = MPI_Comm_create_keyval( comm_copy_attr_fn, comm_delete_attr_fn, comm_keyval, extra_state );
}
#endif
#if HAVE( DECL_PMPI_COMM_DELETE_ATTR ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_CG ) && !defined( SCOREP_MPI_NO_EXT ) && !defined( MPI_Comm_delete_attr )
/**
 * Measurement wrapper for MPI_Comm_delete_attr
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup cg_ext
 */
void
FSUB( MPI_Comm_delete_attr )( MPI_Comm* comm,
                              int*      comm_keyval,
                              int*      ierr )
{
    *ierr = MPI_Comm_delete_attr( *comm, *comm_keyval );
}
#endif
#if HAVE( DECL_PMPI_COMM_FREE_KEYVAL ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_CG ) && !defined( SCOREP_MPI_NO_EXT ) && !defined( MPI_Comm_free_keyval )
/**
 * Measurement wrapper for MPI_Comm_free_keyval
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup cg_ext
 */
void
FSUB( MPI_Comm_free_keyval )( int* comm_keyval,
                              int* ierr )
{
    *ierr = MPI_Comm_free_keyval( comm_keyval );
}
#endif
#if HAVE( DECL_PMPI_COMM_GET_ATTR ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_CG ) && !defined( SCOREP_MPI_NO_EXT ) && !defined( MPI_Comm_get_attr )
/**
 * Measurement wrapper for MPI_Comm_get_attr
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup cg_ext
 */
void
FSUB( MPI_Comm_get_attr )( MPI_Comm* comm,
                           int*      comm_keyval,
                           void*     attribute_val,
                           int*      flag,
                           int*      ierr )
{
    *ierr = MPI_Comm_get_attr( *comm, *comm_keyval, attribute_val, flag );
}
#endif
#if HAVE( DECL_PMPI_COMM_GET_NAME ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_CG ) && !defined( SCOREP_MPI_NO_EXT ) && !defined( MPI_Comm_get_name )
/**
 * Measurement wrapper for MPI_Comm_get_name
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup cg_ext
 */
void
FSUB( MPI_Comm_get_name )( MPI_Comm* comm,
                           char*     comm_name,
                           int*      resultlen,
                           int*      ierr,
                           int       comm_name_len )
{
    char* c_comm_name     = NULL;
    int   c_comm_name_len = 0;
    c_comm_name = ( char* )malloc( ( comm_name_len + 1 ) * sizeof( char ) );
    if ( !c_comm_name )
    {
        exit( EXIT_FAILURE );
    }


    *ierr = MPI_Comm_get_name( *comm, c_comm_name, resultlen );


    c_comm_name_len = strlen( c_comm_name );
    strncpy( comm_name, c_comm_name, c_comm_name_len );
    memset( comm_name + c_comm_name_len, ' ', comm_name_len - c_comm_name_len );
    free( c_comm_name );
}
#endif
#if HAVE( DECL_PMPI_COMM_SET_ATTR ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_CG ) && !defined( SCOREP_MPI_NO_EXT ) && !defined( MPI_Comm_set_attr )
/**
 * Measurement wrapper for MPI_Comm_set_attr
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup cg_ext
 */
void
FSUB( MPI_Comm_set_attr )( MPI_Comm* comm,
                           int*      comm_keyval,
                           void*     attribute_val,
                           int*      ierr )
{
    *ierr = MPI_Comm_set_attr( *comm, *comm_keyval, attribute_val );
}
#endif
#if HAVE( DECL_PMPI_COMM_SET_NAME ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_CG ) && !defined( SCOREP_MPI_NO_EXT ) && !defined( MPI_Comm_set_name )
/**
 * Measurement wrapper for MPI_Comm_set_name
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup cg_ext
 */
void
FSUB( MPI_Comm_set_name )( MPI_Comm* comm,
                           char*     comm_name,
                           int*      ierr,
                           int       comm_name_len )
{
    char* c_comm_name = NULL;
    c_comm_name = ( char* )malloc( ( comm_name_len + 1 ) * sizeof( char ) );
    if ( !c_comm_name )
    {
        exit( EXIT_FAILURE );
    }
    strncpy( c_comm_name, comm_name, comm_name_len );
    c_comm_name[ comm_name_len ] = '\0';


    *ierr = MPI_Comm_set_name( *comm, c_comm_name );

    free( c_comm_name );
}
#endif
#if HAVE( DECL_PMPI_KEYVAL_CREATE ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_CG ) && !defined( SCOREP_MPI_NO_EXT ) && !defined( MPI_Keyval_create )
/**
 * Measurement wrapper for MPI_Keyval_create
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup cg_ext
 */
void
FSUB( MPI_Keyval_create )( MPI_Copy_function*   copy_fn,
                           MPI_Delete_function* delete_fn,
                           int*                 keyval,
                           void*                extra_state,
                           int*                 ierr )
{
    *ierr = MPI_Keyval_create( copy_fn, delete_fn, keyval, extra_state );
}
#endif
#if HAVE( DECL_PMPI_KEYVAL_FREE ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_CG ) && !defined( SCOREP_MPI_NO_EXT ) && !defined( MPI_Keyval_free )
/**
 * Measurement wrapper for MPI_Keyval_free
 * @note Auto-generated by wrapgen from template: SCOREP_Fmpi_Std.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup cg_ext
 */
void
FSUB( MPI_Keyval_free )( int* keyval,
                         int* ierr )
{
    *ierr = MPI_Keyval_free( keyval );
}
#endif

/**
 * @}
 * @name Fortran wrappers for handle conversion functions
 * @{
 */


#else /* !NEED_F2C_CONV */

/**
 * @}
 * @name Fortran wrappers for communicator constructors
 * @{
 */

#if HAVE( DECL_PMPI_COMM_CREATE )
/**
 * Measurement wrapper for MPI_Comm_create
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup cg
 * For the order of events see @ref MPI_Comm_create
 */
void
FSUB( MPI_Comm_create )( MPI_Fint* comm,
                         MPI_Fint* group,
                         MPI_Fint* newcomm,
                         int*      ierr )
{
    MPI_Comm c_newcomm;
    *ierr    = MPI_Comm_create( PMPI_Comm_f2c( *comm ), PMPI_Group_f2c( *group ), &c_newcomm );
    *newcomm = PMPI_Comm_c2f( c_newcomm );
}
#endif
#if HAVE( DECL_PMPI_COMM_DUP )
/**
 * Measurement wrapper for MPI_Comm_dup
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup cg
 * For the order of events see @ref MPI_Comm_dup
 */
void
FSUB( MPI_Comm_dup )( MPI_Fint* comm,
                      MPI_Fint* newcomm,
                      int*      ierr )
{
    MPI_Comm c_newcomm;
    *ierr    = MPI_Comm_dup( PMPI_Comm_f2c( *comm ), &c_newcomm );
    *newcomm = PMPI_Comm_c2f( c_newcomm );
}
#endif
#if HAVE( DECL_PMPI_COMM_SPLIT )
/**
 * Measurement wrapper for MPI_Comm_split
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup cg
 * For the order of events see @ref MPI_Comm_split
 */
void
FSUB( MPI_Comm_split )( MPI_Fint* comm,
                        MPI_Fint* color,
                        MPI_Fint* key,
                        MPI_Fint* newcomm,
                        int*      ierr )
{
    MPI_Comm c_newcomm;
    *ierr    = MPI_Comm_split( PMPI_Comm_f2c( *comm ), *color, *key, &c_newcomm );
    *newcomm = PMPI_Comm_c2f( c_newcomm );
}
#endif
#if HAVE( DECL_PMPI_INTERCOMM_CREATE )
/**
 * Measurement wrapper for MPI_Intercomm_create
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup cg
 * For the order of events see @ref MPI_Intercomm_create
 */
void
FSUB( MPI_Intercomm_create )( MPI_Fint* local_comm,
                              MPI_Fint* local_leader,
                              MPI_Fint* peer_comm,
                              MPI_Fint* remote_leader,
                              MPI_Fint* tag,
                              MPI_Fint* newcomm,
                              int*      ierr )
{
    MPI_Comm c_newcomm;
    *ierr    = MPI_Intercomm_create( PMPI_Comm_f2c( *local_comm ), *local_leader, PMPI_Comm_f2c( *peer_comm ), *remote_leader, *tag, &c_newcomm );
    *newcomm = PMPI_Comm_c2f( c_newcomm );
}
#endif
#if HAVE( DECL_PMPI_INTERCOMM_MERGE )
/**
 * Measurement wrapper for MPI_Intercomm_merge
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup cg
 * For the order of events see @ref MPI_Intercomm_merge
 */
void
FSUB( MPI_Intercomm_merge )( MPI_Fint* intercomm,
                             MPI_Fint* high,
                             MPI_Fint* newcomm,
                             int*      ierr )
{
    MPI_Comm c_newcomm;
    *ierr    = MPI_Intercomm_merge( PMPI_Comm_f2c( *intercomm ), *high, &c_newcomm );
    *newcomm = PMPI_Comm_c2f( c_newcomm );
}
#endif

/**
 * @}
 * @name Fortran wrappers for communicator destructors
 * @{
 */

#if HAVE( DECL_PMPI_COMM_FREE )
/**
 * Measurement wrapper for MPI_Comm_free
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup cg
 * For the order of events see @ref MPI_Comm_free
 */
void
FSUB( MPI_Comm_free )( MPI_Fint* comm,
                       int*      ierr )
{
    MPI_Comm c_comm = PMPI_Comm_f2c( *comm );
    *ierr = MPI_Comm_free( &c_comm );
    *comm = PMPI_Comm_c2f( c_comm );
}
#endif

/**
 * @}
 * @name Fortran wrappers for group constructors
 * @{
 */

#if HAVE( DECL_PMPI_GROUP_DIFFERENCE ) && !defined( SCOREP_MPI_NO_CG ) && !defined( MPI_Group_difference )
/**
 * Measurement wrapper for MPI_Group_difference
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup cg
 * For the order of events see @ref MPI_Group_difference
 */
void
FSUB( MPI_Group_difference )( MPI_Fint* group1,
                              MPI_Fint* group2,
                              MPI_Fint* newgroup,
                              int*      ierr )
{
    MPI_Group c_newgroup;
    *ierr     = MPI_Group_difference( PMPI_Group_f2c( *group1 ), PMPI_Group_f2c( *group2 ), &c_newgroup );
    *newgroup = PMPI_Group_c2f( c_newgroup );
}
#endif
#if HAVE( DECL_PMPI_GROUP_EXCL ) && !defined( SCOREP_MPI_NO_CG ) && !defined( MPI_Group_excl )
/**
 * Measurement wrapper for MPI_Group_excl
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup cg
 * For the order of events see @ref MPI_Group_excl
 */
void
FSUB( MPI_Group_excl )( MPI_Fint* group,
                        MPI_Fint* n,
                        MPI_Fint* ranks,
                        MPI_Fint* newgroup,
                        int*      ierr )
{
    MPI_Group c_newgroup;
    *ierr     = MPI_Group_excl( PMPI_Group_f2c( *group ), *n, ranks, &c_newgroup );
    *newgroup = PMPI_Group_c2f( c_newgroup );
}
#endif
#if HAVE( DECL_PMPI_GROUP_INCL ) && !defined( SCOREP_MPI_NO_CG ) && !defined( MPI_Group_incl )
/**
 * Measurement wrapper for MPI_Group_incl
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup cg
 * For the order of events see @ref MPI_Group_incl
 */
void
FSUB( MPI_Group_incl )( MPI_Fint* group,
                        MPI_Fint* n,
                        MPI_Fint* ranks,
                        MPI_Fint* newgroup,
                        int*      ierr )
{
    MPI_Group c_newgroup;
    *ierr     = MPI_Group_incl( PMPI_Group_f2c( *group ), *n, ranks, &c_newgroup );
    *newgroup = PMPI_Group_c2f( c_newgroup );
}
#endif
#if HAVE( DECL_PMPI_GROUP_INTERSECTION ) && !defined( SCOREP_MPI_NO_CG ) && !defined( MPI_Group_intersection )
/**
 * Measurement wrapper for MPI_Group_intersection
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup cg
 * For the order of events see @ref MPI_Group_intersection
 */
void
FSUB( MPI_Group_intersection )( MPI_Fint* group1,
                                MPI_Fint* group2,
                                MPI_Fint* newgroup,
                                int*      ierr )
{
    MPI_Group c_newgroup;
    *ierr     = MPI_Group_intersection( PMPI_Group_f2c( *group1 ), PMPI_Group_f2c( *group2 ), &c_newgroup );
    *newgroup = PMPI_Group_c2f( c_newgroup );
}
#endif
#if HAVE( DECL_PMPI_GROUP_RANGE_EXCL ) && !defined( SCOREP_MPI_NO_CG ) && !defined( MPI_Group_range_excl )
/**
 * Measurement wrapper for MPI_Group_range_excl
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup cg
 * For the order of events see @ref MPI_Group_range_excl
 */
void
FSUB( MPI_Group_range_excl )( MPI_Fint* group,
                              MPI_Fint* n,
                              MPI_Fint  ranges[][ 3 ],
                              MPI_Fint* newgroup,
                              int*      ierr )
{
    MPI_Group c_newgroup;
    *ierr     = MPI_Group_range_excl( PMPI_Group_f2c( *group ), *n, ( int( * )[ 3 ] )ranges, &c_newgroup );
    *newgroup = PMPI_Group_c2f( c_newgroup );
}
#endif
#if HAVE( DECL_PMPI_GROUP_RANGE_INCL ) && !defined( SCOREP_MPI_NO_CG ) && !defined( MPI_Group_range_incl )
/**
 * Measurement wrapper for MPI_Group_range_incl
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup cg
 * For the order of events see @ref MPI_Group_range_incl
 */
void
FSUB( MPI_Group_range_incl )( MPI_Fint* group,
                              MPI_Fint* n,
                              MPI_Fint  ranges[][ 3 ],
                              MPI_Fint* newgroup,
                              int*      ierr )
{
    MPI_Group c_newgroup;
    *ierr     = MPI_Group_range_incl( PMPI_Group_f2c( *group ), *n, ( int( * )[ 3 ] )ranges, &c_newgroup );
    *newgroup = PMPI_Group_c2f( c_newgroup );
}
#endif
#if HAVE( DECL_PMPI_GROUP_UNION ) && !defined( SCOREP_MPI_NO_CG ) && !defined( MPI_Group_union )
/**
 * Measurement wrapper for MPI_Group_union
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup cg
 * For the order of events see @ref MPI_Group_union
 */
void
FSUB( MPI_Group_union )( MPI_Fint* group1,
                         MPI_Fint* group2,
                         MPI_Fint* newgroup,
                         int*      ierr )
{
    MPI_Group c_newgroup;
    *ierr     = MPI_Group_union( PMPI_Group_f2c( *group1 ), PMPI_Group_f2c( *group2 ), &c_newgroup );
    *newgroup = PMPI_Group_c2f( c_newgroup );
}
#endif
#if HAVE( DECL_PMPI_COMM_GROUP )
/**
 * Measurement wrapper for MPI_Comm_group
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup cg
 * For the order of events see @ref MPI_Comm_group
 */
void
FSUB( MPI_Comm_group )( MPI_Fint* comm,
                        MPI_Fint* group,
                        int*      ierr )
{
    MPI_Group c_group;
    *ierr  = MPI_Comm_group( PMPI_Comm_f2c( *comm ), &c_group );
    *group = PMPI_Group_c2f( c_group );
}
#endif
#if HAVE( DECL_PMPI_COMM_REMOTE_GROUP )
/**
 * Measurement wrapper for MPI_Comm_remote_group
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup cg
 * For the order of events see @ref MPI_Comm_remote_group
 */
void
FSUB( MPI_Comm_remote_group )( MPI_Fint* comm,
                               MPI_Fint* group,
                               int*      ierr )
{
    MPI_Group c_group;
    *ierr  = MPI_Comm_remote_group( PMPI_Comm_f2c( *comm ), &c_group );
    *group = PMPI_Group_c2f( c_group );
}
#endif

/**
 * @}
 * @name Fortran wrappers for group destructors
 * @{
 */

#if HAVE( DECL_PMPI_GROUP_FREE ) && !defined( SCOREP_MPI_NO_CG ) && !defined( MPI_Group_free )
/**
 * Measurement wrapper for MPI_Group_free
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup cg
 * For the order of events see @ref MPI_Group_free
 */
void
FSUB( MPI_Group_free )( MPI_Fint* group,
                        int*      ierr )
{
    MPI_Group c_group = PMPI_Group_f2c( *group );
    *ierr  = MPI_Group_free( &c_group );
    *group = PMPI_Group_c2f( c_group );
}
#endif

/**
 * @}
 * @name Fortran wrappers for miscelaneous functions
 * @{
 */

#if HAVE( DECL_PMPI_COMM_COMPARE ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_CG ) && !defined( MPI_Comm_compare )
/**
 * Measurement wrapper for MPI_Comm_compare
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup cg
 * For the order of events see @ref MPI_Comm_compare
 */
void
FSUB( MPI_Comm_compare )( MPI_Fint* comm1,
                          MPI_Fint* comm2,
                          MPI_Fint* result,
                          int*      ierr )
{
    *ierr = MPI_Comm_compare( PMPI_Comm_f2c( *comm1 ), PMPI_Comm_f2c( *comm2 ), result );
}
#endif
#if HAVE( DECL_PMPI_COMM_RANK ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_MINI ) && !defined( SCOREP_MPI_NO_CG ) && !defined( MPI_Comm_rank )
/**
 * Measurement wrapper for MPI_Comm_rank
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup cg
 * For the order of events see @ref MPI_Comm_rank
 */
void
FSUB( MPI_Comm_rank )( MPI_Fint* comm,
                       MPI_Fint* rank,
                       int*      ierr )
{
    *ierr = MPI_Comm_rank( PMPI_Comm_f2c( *comm ), rank );
}
#endif
#if HAVE( DECL_PMPI_COMM_REMOTE_SIZE ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_MINI ) && !defined( SCOREP_MPI_NO_CG ) && !defined( MPI_Comm_remote_size )
/**
 * Measurement wrapper for MPI_Comm_remote_size
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup cg
 * For the order of events see @ref MPI_Comm_remote_size
 */
void
FSUB( MPI_Comm_remote_size )( MPI_Fint* comm,
                              MPI_Fint* size,
                              int*      ierr )
{
    *ierr = MPI_Comm_remote_size( PMPI_Comm_f2c( *comm ), size );
}
#endif
#if HAVE( DECL_PMPI_COMM_SIZE ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_MINI ) && !defined( SCOREP_MPI_NO_CG ) && !defined( MPI_Comm_size )
/**
 * Measurement wrapper for MPI_Comm_size
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup cg
 * For the order of events see @ref MPI_Comm_size
 */
void
FSUB( MPI_Comm_size )( MPI_Fint* comm,
                       MPI_Fint* size,
                       int*      ierr )
{
    *ierr = MPI_Comm_size( PMPI_Comm_f2c( *comm ), size );
}
#endif
#if HAVE( DECL_PMPI_COMM_TEST_INTER ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_CG ) && !defined( MPI_Comm_test_inter )
/**
 * Measurement wrapper for MPI_Comm_test_inter
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup cg
 * For the order of events see @ref MPI_Comm_test_inter
 */
void
FSUB( MPI_Comm_test_inter )( MPI_Fint* comm,
                             MPI_Fint* flag,
                             int*      ierr )
{
    *ierr = MPI_Comm_test_inter( PMPI_Comm_f2c( *comm ), flag );
}
#endif
#if HAVE( DECL_PMPI_GROUP_COMPARE ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_CG ) && !defined( MPI_Group_compare )
/**
 * Measurement wrapper for MPI_Group_compare
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup cg
 * For the order of events see @ref MPI_Group_compare
 */
void
FSUB( MPI_Group_compare )( MPI_Fint* group1,
                           MPI_Fint* group2,
                           MPI_Fint* result,
                           int*      ierr )
{
    *ierr = MPI_Group_compare( PMPI_Group_f2c( *group1 ), PMPI_Group_f2c( *group2 ), result );
}
#endif
#if HAVE( DECL_PMPI_GROUP_RANK ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_CG ) && !defined( MPI_Group_rank )
/**
 * Measurement wrapper for MPI_Group_rank
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup cg
 * For the order of events see @ref MPI_Group_rank
 */
void
FSUB( MPI_Group_rank )( MPI_Fint* group,
                        MPI_Fint* rank,
                        int*      ierr )
{
    *ierr = MPI_Group_rank( PMPI_Group_f2c( *group ), rank );
}
#endif
#if HAVE( DECL_PMPI_GROUP_SIZE ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_CG ) && !defined( MPI_Group_size )
/**
 * Measurement wrapper for MPI_Group_size
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup cg
 * For the order of events see @ref MPI_Group_size
 */
void
FSUB( MPI_Group_size )( MPI_Fint* group,
                        MPI_Fint* size,
                        int*      ierr )
{
    *ierr = MPI_Group_size( PMPI_Group_f2c( *group ), size );
}
#endif
#if HAVE( DECL_PMPI_GROUP_TRANSLATE_RANKS ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_CG ) && !defined( MPI_Group_translate_ranks )
/**
 * Measurement wrapper for MPI_Group_translate_ranks
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup cg
 * For the order of events see @ref MPI_Group_translate_ranks
 */
void
FSUB( MPI_Group_translate_ranks )( MPI_Fint* group1,
                                   MPI_Fint* n,
                                   MPI_Fint* ranks1,
                                   MPI_Fint* group2,
                                   MPI_Fint* ranks2,
                                   int*      ierr )
{
    *ierr = MPI_Group_translate_ranks( PMPI_Group_f2c( *group1 ), *n, ranks1, PMPI_Group_f2c( *group2 ), ranks2 );
}
#endif

/**
 * @}
 * @name Fortran wrappers for error handling functions
 * @{
 */

#if HAVE( DECL_PMPI_COMM_CALL_ERRHANDLER ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_CG ) && !defined( SCOREP_MPI_NO_ERR ) && !defined( MPI_Comm_call_errhandler )
/**
 * Measurement wrapper for MPI_Comm_call_errhandler
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup cg_err
 * For the order of events see @ref MPI_Comm_call_errhandler
 */
void
FSUB( MPI_Comm_call_errhandler )( MPI_Fint* comm,
                                  MPI_Fint* errorcode,
                                  int*      ierr )
{
    *ierr = MPI_Comm_call_errhandler( PMPI_Comm_f2c( *comm ), *errorcode );
}
#endif
#if HAVE( DECL_PMPI_COMM_CREATE_ERRHANDLER ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_CG ) && !defined( SCOREP_MPI_NO_ERR ) && !defined( MPI_Comm_create_errhandler )
/**
 * Measurement wrapper for MPI_Comm_create_errhandler
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup cg_err
 * For the order of events see @ref MPI_Comm_create_errhandler
 */
void
FSUB( MPI_Comm_create_errhandler )( void* function,
                                    void* errhandler,
                                    int*  ierr )
{
    *ierr = MPI_Comm_create_errhandler( ( MPI_Comm_errhandler_fn* )function, ( MPI_Errhandler* )errhandler );
}
#endif
#if HAVE( DECL_PMPI_COMM_GET_ERRHANDLER ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_CG ) && !defined( SCOREP_MPI_NO_ERR ) && !defined( MPI_Comm_get_errhandler )
/**
 * Measurement wrapper for MPI_Comm_get_errhandler
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup cg_err
 * For the order of events see @ref MPI_Comm_get_errhandler
 */
void
FSUB( MPI_Comm_get_errhandler )( MPI_Fint* comm,
                                 void*     errhandler,
                                 int*      ierr )
{
    *ierr = MPI_Comm_get_errhandler( PMPI_Comm_f2c( *comm ), ( MPI_Errhandler* )errhandler );
}
#endif
#if HAVE( DECL_PMPI_COMM_SET_ERRHANDLER ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_CG ) && !defined( SCOREP_MPI_NO_ERR ) && !defined( MPI_Comm_set_errhandler )
/**
 * Measurement wrapper for MPI_Comm_set_errhandler
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup cg_err
 * For the order of events see @ref MPI_Comm_set_errhandler
 */
void
FSUB( MPI_Comm_set_errhandler )( MPI_Fint* comm,
                                 void*     errhandler,
                                 int*      ierr )
{
    MPI_Comm c_comm = PMPI_Comm_f2c( *comm );
    *ierr = MPI_Comm_set_errhandler( c_comm, *( ( MPI_Errhandler* )errhandler ) );
    *comm = PMPI_Comm_c2f( c_comm );
}
#endif

/**
 * @}
 * @name Fortran wrappers for external interface functions
 * @{
 */

#if HAVE( DECL_PMPI_ATTR_DELETE ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_CG ) && !defined( SCOREP_MPI_NO_EXT ) && !defined( MPI_Attr_delete )
/**
 * Measurement wrapper for MPI_Attr_delete
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup cg_ext
 * For the order of events see @ref MPI_Attr_delete
 */
void
FSUB( MPI_Attr_delete )( MPI_Fint* comm,
                         MPI_Fint* keyval,
                         int*      ierr )
{
    *ierr = MPI_Attr_delete( PMPI_Comm_f2c( *comm ), *keyval );
}
#endif
#if HAVE( DECL_PMPI_ATTR_GET ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_CG ) && !defined( SCOREP_MPI_NO_EXT ) && !defined( MPI_Attr_get )
/**
 * Measurement wrapper for MPI_Attr_get
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup cg_ext
 * For the order of events see @ref MPI_Attr_get
 */
void
FSUB( MPI_Attr_get )( MPI_Fint* comm,
                      MPI_Fint* keyval,
                      void*     attribute_val,
                      MPI_Fint* flag,
                      int*      ierr )
{
    *ierr = MPI_Attr_get( PMPI_Comm_f2c( *comm ), *keyval, attribute_val, flag );
}
#endif
#if HAVE( DECL_PMPI_ATTR_PUT ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_CG ) && !defined( SCOREP_MPI_NO_EXT ) && !defined( MPI_Attr_put )
/**
 * Measurement wrapper for MPI_Attr_put
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup cg_ext
 * For the order of events see @ref MPI_Attr_put
 */
void
FSUB( MPI_Attr_put )( MPI_Fint* comm,
                      MPI_Fint* keyval,
                      void*     attribute_val,
                      int*      ierr )
{
    *ierr = MPI_Attr_put( PMPI_Comm_f2c( *comm ), *keyval, attribute_val );
}
#endif
#if HAVE( DECL_PMPI_COMM_CREATE_KEYVAL ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_CG ) && !defined( SCOREP_MPI_NO_EXT ) && !defined( MPI_Comm_create_keyval )
/**
 * Measurement wrapper for MPI_Comm_create_keyval
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup cg_ext
 * For the order of events see @ref MPI_Comm_create_keyval
 */
void
FSUB( MPI_Comm_create_keyval )( void*     comm_copy_attr_fn,
                                void*     comm_delete_attr_fn,
                                MPI_Fint* comm_keyval,
                                void*     extra_state,
                                int*      ierr )
{
    *ierr = MPI_Comm_create_keyval( ( MPI_Comm_copy_attr_function* )comm_copy_attr_fn, ( MPI_Comm_delete_attr_function* )comm_delete_attr_fn, comm_keyval, extra_state );
}
#endif
#if HAVE( DECL_PMPI_COMM_DELETE_ATTR ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_CG ) && !defined( SCOREP_MPI_NO_EXT ) && !defined( MPI_Comm_delete_attr )
/**
 * Measurement wrapper for MPI_Comm_delete_attr
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup cg_ext
 * For the order of events see @ref MPI_Comm_delete_attr
 */
void
FSUB( MPI_Comm_delete_attr )( MPI_Fint* comm,
                              MPI_Fint* comm_keyval,
                              int*      ierr )
{
    MPI_Comm c_comm = PMPI_Comm_f2c( *comm );
    *ierr = MPI_Comm_delete_attr( c_comm, *comm_keyval );
    *comm = PMPI_Comm_c2f( c_comm );
}
#endif
#if HAVE( DECL_PMPI_COMM_FREE_KEYVAL ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_CG ) && !defined( SCOREP_MPI_NO_EXT ) && !defined( MPI_Comm_free_keyval )
/**
 * Measurement wrapper for MPI_Comm_free_keyval
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup cg_ext
 * For the order of events see @ref MPI_Comm_free_keyval
 */
void
FSUB( MPI_Comm_free_keyval )( MPI_Fint* comm_keyval,
                              int*      ierr )
{
    *ierr = MPI_Comm_free_keyval( comm_keyval );
}
#endif
#if HAVE( DECL_PMPI_COMM_GET_ATTR ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_CG ) && !defined( SCOREP_MPI_NO_EXT ) && !defined( MPI_Comm_get_attr )
/**
 * Measurement wrapper for MPI_Comm_get_attr
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup cg_ext
 * For the order of events see @ref MPI_Comm_get_attr
 */
void
FSUB( MPI_Comm_get_attr )( MPI_Fint* comm,
                           MPI_Fint* comm_keyval,
                           void*     attribute_val,
                           MPI_Fint* flag,
                           int*      ierr )
{
    *ierr = MPI_Comm_get_attr( PMPI_Comm_f2c( *comm ), *comm_keyval, attribute_val, flag );
}
#endif
#if HAVE( DECL_PMPI_COMM_GET_NAME ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_CG ) && !defined( SCOREP_MPI_NO_EXT ) && !defined( MPI_Comm_get_name )
/**
 * Measurement wrapper for MPI_Comm_get_name
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup cg_ext
 * For the order of events see @ref MPI_Comm_get_name
 */
void
FSUB( MPI_Comm_get_name )( MPI_Fint* comm,
                           char*     comm_name,
                           MPI_Fint* resultlen,
                           int*      ierr,
                           int       comm_name_len )
{
    char* c_comm_name     = NULL;
    int   c_comm_name_len = 0;
    c_comm_name = ( char* )malloc( ( comm_name_len + 1 ) * sizeof( char ) );
    if ( !c_comm_name )
    {
        exit( EXIT_FAILURE );
    }

    *ierr = MPI_Comm_get_name( PMPI_Comm_f2c( *comm ), c_comm_name, resultlen );

    c_comm_name_len = strlen( c_comm_name );
    strncpy( comm_name, c_comm_name, c_comm_name_len );
    memset( comm_name + c_comm_name_len, ' ', comm_name_len - c_comm_name_len );
    free( c_comm_name );
}
#endif
#if HAVE( DECL_PMPI_COMM_SET_ATTR ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_CG ) && !defined( SCOREP_MPI_NO_EXT ) && !defined( MPI_Comm_set_attr )
/**
 * Measurement wrapper for MPI_Comm_set_attr
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup cg_ext
 * For the order of events see @ref MPI_Comm_set_attr
 */
void
FSUB( MPI_Comm_set_attr )( MPI_Fint* comm,
                           MPI_Fint* comm_keyval,
                           void*     attribute_val,
                           int*      ierr )
{
    MPI_Comm c_comm = PMPI_Comm_f2c( *comm );
    *ierr = MPI_Comm_set_attr( c_comm, *comm_keyval, attribute_val );
    *comm = PMPI_Comm_c2f( c_comm );
}
#endif
#if HAVE( DECL_PMPI_COMM_SET_NAME ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_CG ) && !defined( SCOREP_MPI_NO_EXT ) && !defined( MPI_Comm_set_name )
/**
 * Measurement wrapper for MPI_Comm_set_name
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-2
 * @ingroup cg_ext
 * For the order of events see @ref MPI_Comm_set_name
 */
void
FSUB( MPI_Comm_set_name )( MPI_Fint* comm,
                           char*     comm_name,
                           int*      ierr,
                           int       comm_name_len )
{
    char*    c_comm_name = NULL;
    MPI_Comm c_comm      = PMPI_Comm_f2c( *comm );
    c_comm_name = ( char* )malloc( ( comm_name_len + 1 ) * sizeof( char ) );
    if ( !c_comm_name )
    {
        exit( EXIT_FAILURE );
    }
    strncpy( c_comm_name, comm_name, comm_name_len );
    c_comm_name[ comm_name_len ] = '\0';

    *ierr = MPI_Comm_set_name( c_comm, c_comm_name );
    free( c_comm_name );
    *comm = PMPI_Comm_c2f( c_comm );
}
#endif
#if HAVE( DECL_PMPI_KEYVAL_CREATE ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_CG ) && !defined( SCOREP_MPI_NO_EXT ) && !defined( MPI_Keyval_create )
/**
 * Measurement wrapper for MPI_Keyval_create
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup cg_ext
 * For the order of events see @ref MPI_Keyval_create
 */
void
FSUB( MPI_Keyval_create )( void*     copy_fn,
                           void*     delete_fn,
                           MPI_Fint* keyval,
                           void*     extra_state,
                           int*      ierr )
{
    *ierr = MPI_Keyval_create( ( MPI_Copy_function* )copy_fn, ( MPI_Delete_function* )delete_fn, keyval, extra_state );
}
#endif
#if HAVE( DECL_PMPI_KEYVAL_FREE ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_CG ) && !defined( SCOREP_MPI_NO_EXT ) && !defined( MPI_Keyval_free )
/**
 * Measurement wrapper for MPI_Keyval_free
 * @note Auto-generated by wrapgen from template: f2c_c2f.w
 * @note Fortran interface
 * @note Introduced with MPI-1
 * @ingroup cg_ext
 * For the order of events see @ref MPI_Keyval_free
 */
void
FSUB( MPI_Keyval_free )( MPI_Fint* keyval,
                         int*      ierr )
{
    *ierr = MPI_Keyval_free( keyval );
}
#endif

/**
 * @}
 * @name Fortran wrappers for handle conversion functions
 * @{
 */


#endif /* !NEED_F2C_CONV */

/**
 * @}
 */
