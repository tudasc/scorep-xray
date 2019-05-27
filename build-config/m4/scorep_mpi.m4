## -*- mode: autoconf -*-

##
## This file is part of the Score-P software (http://www.score-p.org)
##
## Copyright (c) 2009-2013,
##    RWTH Aachen University, Germany
##
## Copyright (c) 2009-2013,
##    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
##
## Copyright (c) 2009-2014, 2019,
##    Technische Universitaet Dresden, Germany
##
## Copyright (c) 2009-2013,
##    University of Oregon, Eugene, USA
##
## Copyright (c) 2009-2015,
##    Forschungszentrum Juelich GmbH, Germany
##
## Copyright (c) 2009-2013,
##    German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
##
## Copyright (c) 2009-2013,
##    Technische Universitaet Muenchen, Germany
##
## This software may be modified and distributed under the terms of
## a BSD-style license.  See the COPYING file in the package base
## directory for details.
##


## file build-config/m4/scorep_mpi.m4

dnl ----------------------------------------------------------------------------


AC_DEFUN([AC_SCOREP_MPI_FORTRAN_CONSTANTS], [
AC_LANG_PUSH(Fortran)

AC_MSG_CHECKING([for MPI_BOTTOM])
AC_COMPILE_IFELSE([
      SUBROUTINE foo( i )
      integer :: i
      END SUBROUTINE foo

      PROGRAM test
      IMPLICIT NONE
      INCLUDE  'mpif.h'
      call foo( MPI_BOTTOM )
      END PROGRAM test
], [AC_MSG_RESULT(yes)
    AC_DEFINE(HAVE_MPI_BOTTOM, 1, [Fortran MPI defines MPI_BOTTOM])
], [AC_MSG_RESULT(no)]
) # AC_COMPILE_IF_ELSE

AC_MSG_CHECKING([for MPI_IN_PLACE])
AC_COMPILE_IFELSE([
      SUBROUTINE foo( i )
      integer :: i
      END SUBROUTINE foo

      PROGRAM test
      IMPLICIT NONE
      INCLUDE  'mpif.h'
      call foo( MPI_IN_PLACE )
      END PROGRAM test
], [AC_MSG_RESULT(yes)
    AC_DEFINE(HAVE_MPI_IN_PLACE, 1, [Fortran MPI defines MPI_IN_PLACE])
], [AC_MSG_RESULT(no)]
) # AC_COMPILE_IF_ELSE

AC_MSG_CHECKING([for MPI_STATUS_IGNORE])
AC_COMPILE_IFELSE([
      SUBROUTINE foo( i )
      integer :: i
      END SUBROUTINE foo

      PROGRAM test
      IMPLICIT NONE
      INCLUDE  'mpif.h'
      call foo( MPI_STATUS_IGNORE )
      END PROGRAM test
], [AC_MSG_RESULT(yes)
    AC_DEFINE(HAVE_MPI_STATUS_IGNORE, 1, [Fortran MPI defines MPI_STATUS_IGNORE])
], [AC_MSG_RESULT(no)]
) # AC_COMPILE_IF_ELSE

AC_MSG_CHECKING([for MPI_STATUSES_IGNORE])
scorep_has_statuses_ignore="no"
AC_COMPILE_IFELSE([
      SUBROUTINE foo( i )
      integer :: i
      END SUBROUTINE foo

      PROGRAM test
      IMPLICIT NONE
      INCLUDE  'mpif.h'
      call foo( MPI_STATUSES_IGNORE )
      END PROGRAM test
], [scorep_has_statuses_ignore="yes"], []
) # AC_COMPILE_IF_ELSE

if test "x${scorep_has_statuses_ignore}" = "xyes"; then
   AC_MSG_RESULT(yes);
   AC_DEFINE(HAVE_MPI_STATUSES_IGNORE, 1, [Fortran MPI defines MPI_STATUSES_IGNORE])
else
   AC_MSG_RESULT(no)
fi

AC_MSG_CHECKING([for MPI_UNWEIGHTED])
AC_COMPILE_IFELSE([
      SUBROUTINE foo( i )
      integer :: i
      END SUBROUTINE foo

      PROGRAM test
      IMPLICIT NONE
      INCLUDE  'mpif.h'
      call foo( MPI_UNWEIGHTED )
      END PROGRAM test
], [AC_MSG_RESULT(yes);
    AC_DEFINE(HAVE_MPI_UNWEIGHTED, 1, [Fortran MPI defines MPI_UNWEIGHTED])
], [AC_MSG_RESULT(no)]
) # AC_COMPILE_IF_ELSE

AC_LANG_POP(Fortran)
]) # AC_DEFUN(AC_SCOREP_MPI_FORTRAN_CONSTANTS)


dnl ----------------------------------------------------------------------------


AC_DEFUN([AC_SCOREP_MPI_C_DATATYPES], [
AC_LANG_PUSH([C])

AFS_MPI_DATATYPE_INT32_T
AFS_MPI_DATATYPE_UINT32_T
AFS_MPI_DATATYPE_INT64_T
AFS_MPI_DATATYPE_UINT64_T

AC_LANG_POP([C])
]) # AC_DEFUN([AC_SCOREP_MPI_C_CONSTANTS])


dnl ----------------------------------------------------------------------------

AC_DEFUN([_SCOREP_MPI_CHECK_COMPLIANCE],
         [AFS_CHECK_COMPLIANCE([@%:@include <mpi.h>], $@)])

dnl ----------------------------------------------------------------------------

AC_DEFUN([AC_SCOREP_MPI_COMPLIANCE], [
    AC_LANG_PUSH(C)

    AC_MSG_CHECKING([whether MPI_Info_delete is standard compliant])
    AC_COMPILE_IFELSE([
        AC_LANG_SOURCE([
            #include<mpi.h>
            #if MPI_VERSION >= 3
            #define SCOREP_MPI_CONST_DECL const
            #else
            #define SCOREP_MPI_CONST_DECL
            #endif
            int MPI_Info_delete(MPI_Info info, SCOREP_MPI_CONST_DECL char *c)
            {
                return 0;
            }
            ])],
        [AC_MSG_RESULT(yes);
         AC_DEFINE(HAVE_MPI_INFO_DELETE_COMPLIANT, 1, [MPI_Info_delete is standard compliant])],
        [AC_MSG_RESULT(no)]
    ) # AC_COMPILE_IF_ELSE

    AC_MSG_CHECKING([whether MPI_Info_get is standard compliant])
    AC_COMPILE_IFELSE([
        AC_LANG_SOURCE([
            #include<mpi.h>
            #if MPI_VERSION >= 3
            #define SCOREP_MPI_CONST_DECL const
            #else
            #define SCOREP_MPI_CONST_DECL
            #endif
            int MPI_Info_get(MPI_Info info, SCOREP_MPI_CONST_DECL char *c1, int i1, char *c2, int *i2)
            {
                return 0;
            }
            ])],
        [AC_MSG_RESULT(yes);
         AC_DEFINE(HAVE_MPI_INFO_GET_COMPLIANT, 1, [MPI_Info_get is standard compliant])],
        [AC_MSG_RESULT(no)]
    ) # AC_COMPILE_IF_ELSE

    AC_MSG_CHECKING([whether MPI_Info_get_valuelen is standard compliant])
    AC_COMPILE_IFELSE([
        AC_LANG_SOURCE([
            #include<mpi.h>
            #if MPI_VERSION >= 3
            #define SCOREP_MPI_CONST_DECL const
            #else
            #define SCOREP_MPI_CONST_DECL
            #endif
            int MPI_Info_get_valuelen(MPI_Info info, SCOREP_MPI_CONST_DECL char *c, int *i1, int *i2)
            {
                return 0;
            }
            ])],
        [AC_MSG_RESULT(yes);
         AC_DEFINE(HAVE_MPI_INFO_GET_VALUELEN_COMPLIANT, 1, [MPI_Info_get_valuelen is standard compliant])],
        [AC_MSG_RESULT(no)]
    ) # AC_COMPILE_IF_ELSE

    AC_MSG_CHECKING([whether MPI_Info_set is standard compliant])
    AC_COMPILE_IFELSE([
        AC_LANG_SOURCE([
            #include<mpi.h>
            #if MPI_VERSION >= 3
            #define SCOREP_MPI_CONST_DECL const
            #else
            #define SCOREP_MPI_CONST_DECL
            #endif
            int MPI_Info_set(MPI_Info info, SCOREP_MPI_CONST_DECL char *c1, SCOREP_MPI_CONST_DECL char *c2)
            {
                return 0;
            }
            ])],
        [AC_MSG_RESULT(yes);
         AC_DEFINE(HAVE_MPI_INFO_SET_COMPLIANT, 1, [MPI_Info_set is standard compliant])],
        [AC_MSG_RESULT(no)]
    ) # AC_COMPILE_IF_ELSE

    AC_MSG_CHECKING([whether MPI_Grequest_complete is standard compliant])
    AC_LINK_IFELSE([
        AC_LANG_SOURCE([
            #include<mpi.h>
            int MPI_Grequest_complete(MPI_Request request)
            {
                return 0;
            }

            int main()
            {
                MPI_Request r;
                return  MPI_Grequest_complete(r);
            }
            ])],
        [AC_MSG_RESULT(yes);
         AC_DEFINE(HAVE_MPI_GREQUEST_COMPLETE_COMPLIANT, 1, [MPI_Grequest_complete is standard compliant])],
        [AC_MSG_RESULT(no)]
    ) # AC_LINK_IF_ELSE

    AC_MSG_CHECKING([whether PMPI_Type_create_f90_complex is standard compliant])
    AC_LINK_IFELSE([
        AC_LANG_SOURCE([
            #include<mpi.h>
            int MPI_Type_create_f90_complex(int p, int r, MPI_Datatype *newtype)
            {
                return PMPI_Type_create_f90_complex(p, r, newtype);
            }

            int main()
            {
                return  MPI_Type_create_f90_complex(3,3,0);
            }
            ])],
        [AC_MSG_RESULT(yes);
         AC_DEFINE(HAVE_MPI_TYPE_CREATE_F90_COMPLEX_COMPLIANT, 1, [MPI_Type_create_f90_complex is standard compliant])],
        [AC_MSG_RESULT(no)]
    ) # AC_LINK_IF_ELSE

    AC_MSG_CHECKING([whether PMPI_Type_create_f90_integer is standard compliant])
    AC_LINK_IFELSE([
        AC_LANG_SOURCE([
            #include<mpi.h>
            int MPI_Type_create_f90_integer(int r, MPI_Datatype *newtype)
            {
                return PMPI_Type_create_f90_integer(r, newtype);
            }

            int main()
            {
                return  MPI_Type_create_f90_integer(3,0);
            }
            ])],
        [AC_MSG_RESULT(yes);
         AC_DEFINE(HAVE_MPI_TYPE_CREATE_F90_INTEGER_COMPLIANT, 1, [MPI_Type_create_f90_integer is standard compliant])],
        [AC_MSG_RESULT(no)]
    ) # AC_LINK_IF_ELSE

    AC_MSG_CHECKING([whether PMPI_Type_create_f90_real is standard compliant])
    AC_LINK_IFELSE([
        AC_LANG_SOURCE([
            #include<mpi.h>
            int MPI_Type_create_f90_real(int p, int r, MPI_Datatype *newtype)
            {
                return PMPI_Type_create_f90_real(p, r, newtype);
            }

            int main()
            {
                return  MPI_Type_create_f90_real(3,3,0);
            }
            ])],
        [AC_MSG_RESULT(yes);
         AC_DEFINE(HAVE_MPI_TYPE_CREATE_F90_REAL_COMPLIANT, 1, [MPI_Type_create_f90_integer is standard compliant])],
        [AC_MSG_RESULT(no)]
    ) # AC_LINK_IF_ELSE

    _SCOREP_MPI_CHECK_COMPLIANCE(
    [int], [return 0],
    [MPI_Address], [( void* location, MPI_Aint* address )],
    [CONST],       [( const void* location, MPI_Aint* address )])

    _SCOREP_MPI_CHECK_COMPLIANCE(
    [int], [return 0],
    [MPI_Type_hindexed], [( int count, int* array_of_blocklengths, MPI_Aint* array_of_displacements, MPI_Datatype oldtype, MPI_Datatype* newtype )],
    [CONST],             [( int count, const int* array_of_blocklengths, const MPI_Aint* array_of_displacements, MPI_Datatype oldtype, MPI_Datatype* newtype )])

    _SCOREP_MPI_CHECK_COMPLIANCE(
    [int], [return 0],
    [MPI_Type_struct], [( int count, int* array_of_blocklengths, MPI_Aint* array_of_displacements, MPI_Datatype* array_of_types, MPI_Datatype* newtype )],
    [CONST],           [( int count, const int* array_of_blocklengths, const MPI_Aint* array_of_displacements, const MPI_Datatype* array_of_types, MPI_Datatype* newtype )])

    _SCOREP_MPI_CHECK_COMPLIANCE(
    [int], [return 0],
    [MPI_Add_error_string], [(int errorcode, char *string)],
    [CONST],                [(int errorcode, const char *string)])

    _SCOREP_MPI_CHECK_COMPLIANCE(
    [int], [return 0],
    [MPI_Compare_and_swap], [( void *origin_addr, void *compare_addr, void *result_addr, MPI_Datatype datatype, int target_rank, MPI_Aint target_disp, MPI_Win win )],
    [CONST],                [( const void *origin_addr, const void *compare_addr, void *result_addr, MPI_Datatype datatype, int target_rank, MPI_Aint target_disp, MPI_Win win )])

    _SCOREP_MPI_CHECK_COMPLIANCE(
    [int], [return 0],
    [MPI_Fetch_and_op], [( void *origin_addr, void *result_addr, MPI_Datatype datatype, int target_rank, MPI_Aint target_disp, MPI_Op op, MPI_Win win )],
    [CONST],                [( const void *origin_addr, void *result_addr, MPI_Datatype datatype, int target_rank, MPI_Aint target_disp, MPI_Op op, MPI_Win win )])

    _SCOREP_MPI_CHECK_COMPLIANCE(
    [int], [return 0],
    [MPI_Raccumulate], [( void *origin_addr, int origin_count, MPI_Datatype origin_datatype, int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype, MPI_Op op, MPI_Win win, MPI_Request *request )],
    [CONST],           [( const void *origin_addr, int origin_count, MPI_Datatype origin_datatype, int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype, MPI_Op op, MPI_Win win, MPI_Request *request )])

    _SCOREP_MPI_CHECK_COMPLIANCE(
    [int], [return 0],
    [MPI_Win_detach], [( MPI_Win win, void *base )],
    [CONST],          [( MPI_Win win, const void *base )])

    AC_LANG_POP(C)
]) # AC_DEFUN(AC_SCOREP_MPI_COMPLIANCE)


dnl ----------------------------------------------------------------------------


AC_DEFUN([AC_SCOREP_MPI], [
AC_REQUIRE([_SCOREP_PDT_MPI_INSTRUMENTATION])

AC_DEFINE([OMPI_WANT_MPI_INTERFACE_WARNING], [0], [Disable deprecation warnings in Open MPI])
AC_DEFINE([OMPI_OMIT_MPI1_COMPAT_DECLS],     [0], [Possibly expose deprecated MPI-1 bindings in Open MPI 4.0+])

if test "x${scorep_mpi_c_supported}" = "xyes"; then
  scorep_mpi_supported="yes"
  if test "x${scorep_mpi_f77_supported}" = "xyes" -o "x${scorep_mpi_f90_supported}" = "xyes"; then
    scorep_mpi_fortran_supported="yes"
  else
    scorep_mpi_fortran_supported="no"
  fi
else
  scorep_mpi_supported="no"
  scorep_mpi_fortran_supported="no"
fi

if test "x${scorep_mpi_supported}" = "xno"; then
  AC_MSG_WARN([No suitable MPI compilers found. SCOREP MPI and hybrid libraries will not be build.])
fi
AM_CONDITIONAL([HAVE_MPI_SUPPORT], [test "x${scorep_mpi_supported}" = "xyes"])
AM_CONDITIONAL([HAVE_MPI_FORTRAN_SUPPORT], [test "x${scorep_mpi_fortran_supported}" = "xyes"])
AM_CONDITIONAL([HAVE_MPIFC], [test "x${scorep_mpi_f90_supported}" = "xyes"])

if test "x${scorep_mpi_supported}" = "xyes"; then

  AC_COMPILE_IFELSE(
      [AC_LANG_PROGRAM(
          [[#include <mpi.h>]],
          [[#if (MPI_VERSION == 1) && (MPI_SUBVERSION == 2)
                double version = 1.2;
            #else
                not version 1.2
            #endif
          ]]
      )],
      [AC_DEFINE([HAVE_MPI_VERSION], [1], [[]])
       AC_DEFINE([HAVE_MPI_SUBVERSION], [2], [[]])
       AC_SUBST([HAVE_MPI_VERSION], [1])
       AC_SUBST([HAVE_MPI_SUBVERSION], [2])],
      [
      AC_COMPILE_IFELSE(
          [AC_LANG_PROGRAM(
              [[#include <mpi.h>]],
              [[#if (MPI_VERSION == 2) && (MPI_SUBVERSION == 0)
                    double version = 2.0;
                #else
                    not version 2.0
                #endif
              ]]
          )],
          [AC_DEFINE([HAVE_MPI_VERSION], [2], [[]])
           AC_DEFINE([HAVE_MPI_SUBVERSION], [0], [[]])
           AC_SUBST([HAVE_MPI_VERSION], [2])
           AC_SUBST([HAVE_MPI_SUBVERSION], [0])],
          [
          AC_COMPILE_IFELSE(
              [AC_LANG_PROGRAM(
                  [[#include <mpi.h>]],
                  [[#if (MPI_VERSION == 2) && (MPI_SUBVERSION == 1)
                        double version = 2.1;
                    #else
                        not version 2.1
                    #endif
                  ]]
              )],
              [AC_DEFINE([HAVE_MPI_VERSION], [2], [[]])
               AC_DEFINE([HAVE_MPI_SUBVERSION], [1], [[]])
               AC_SUBST([HAVE_MPI_VERSION], [2])
               AC_SUBST([HAVE_MPI_SUBVERSION], [1])],
              [
              AC_COMPILE_IFELSE(
                  [AC_LANG_PROGRAM(
                      [[#include <mpi.h>]],
                      [[#if (MPI_VERSION == 2) && (MPI_SUBVERSION == 2)
                            double version = 2.2;
                        #else
                            not version 2.2
                        #endif
                      ]]
                   )],
                  [AC_DEFINE([HAVE_MPI_VERSION], [2], [[]])
                   AC_DEFINE([HAVE_MPI_SUBVERSION], [2], [[]])
                   AC_SUBST([HAVE_MPI_VERSION], [2])
                   AC_SUBST([HAVE_MPI_SUBVERSION], [2])],
                  [

                  ]
              )
              ]
          )
          ]
      )
      ]
  )


  AC_CHECK_DECLS([PMPI_Abort, PMPI_Accumulate, PMPI_Add_error_class, PMPI_Add_error_code, PMPI_Add_error_string, PMPI_Address, PMPI_Allgather, PMPI_Allgatherv, PMPI_Alloc_mem, PMPI_Allreduce, PMPI_Alltoall, PMPI_Alltoallv, PMPI_Alltoallw, PMPI_Attr_delete, PMPI_Attr_get, PMPI_Attr_put, PMPI_Barrier, PMPI_Bcast, PMPI_Bsend, PMPI_Bsend_init, PMPI_Buffer_attach, PMPI_Buffer_detach, PMPI_Cancel, PMPI_Cart_coords, PMPI_Cart_create, PMPI_Cart_get, PMPI_Cart_map, PMPI_Cart_rank, PMPI_Cart_shift, PMPI_Cart_sub, PMPI_Cartdim_get, PMPI_Close_port, PMPI_Comm_accept, PMPI_Comm_c2f, PMPI_Comm_call_errhandler, PMPI_Comm_compare, PMPI_Comm_connect, PMPI_Comm_create, PMPI_Comm_create_errhandler, PMPI_Comm_create_group, PMPI_Comm_create_keyval, PMPI_Comm_delete_attr, PMPI_Comm_disconnect, PMPI_Comm_dup, PMPI_Comm_dup_with_info, PMPI_Comm_f2c, PMPI_Comm_free, PMPI_Comm_free_keyval, PMPI_Comm_get_attr, PMPI_Comm_get_errhandler, PMPI_Comm_get_info, PMPI_Comm_get_name, PMPI_Comm_get_parent, PMPI_Comm_group, PMPI_Comm_idup, PMPI_Comm_join, PMPI_Comm_rank, PMPI_Comm_remote_group, PMPI_Comm_remote_size, PMPI_Comm_set_attr, PMPI_Comm_set_errhandler, PMPI_Comm_set_info, PMPI_Comm_set_name, PMPI_Comm_size, PMPI_Comm_spawn, PMPI_Comm_spawn_multiple, PMPI_Comm_split, PMPI_Comm_split_type, PMPI_Comm_test_inter, PMPI_Compare_and_swap, PMPI_Dims_create, PMPI_Dist_graph_create, PMPI_Dist_graph_create_adjacent, PMPI_Dist_graph_neighbors, PMPI_Dist_graph_neighbors_count, PMPI_Errhandler_create, PMPI_Errhandler_free, PMPI_Errhandler_get, PMPI_Errhandler_set, PMPI_Error_class, PMPI_Error_string, PMPI_Exscan, PMPI_Fetch_and_op, PMPI_File_c2f, PMPI_File_call_errhandler, PMPI_File_close, PMPI_File_create_errhandler, PMPI_File_delete, PMPI_File_f2c, PMPI_File_get_amode, PMPI_File_get_atomicity, PMPI_File_get_byte_offset, PMPI_File_get_errhandler, PMPI_File_get_group, PMPI_File_get_info, PMPI_File_get_position, PMPI_File_get_position_shared, PMPI_File_get_size, PMPI_File_get_type_extent, PMPI_File_get_view, PMPI_File_iread, PMPI_File_iread_all, PMPI_File_iread_at, PMPI_File_iread_at_all, PMPI_File_iread_shared, PMPI_File_iwrite, PMPI_File_iwrite_all, PMPI_File_iwrite_at, PMPI_File_iwrite_at_all, PMPI_File_iwrite_shared, PMPI_File_open, PMPI_File_preallocate, PMPI_File_read, PMPI_File_read_all, PMPI_File_read_all_begin, PMPI_File_read_all_end, PMPI_File_read_at, PMPI_File_read_at_all, PMPI_File_read_at_all_begin, PMPI_File_read_at_all_end, PMPI_File_read_ordered, PMPI_File_read_ordered_begin, PMPI_File_read_ordered_end, PMPI_File_read_shared, PMPI_File_seek, PMPI_File_seek_shared, PMPI_File_set_atomicity, PMPI_File_set_errhandler, PMPI_File_set_info, PMPI_File_set_size, PMPI_File_set_view, PMPI_File_sync, PMPI_File_write, PMPI_File_write_all, PMPI_File_write_all_begin, PMPI_File_write_all_end, PMPI_File_write_at, PMPI_File_write_at_all, PMPI_File_write_at_all_begin, PMPI_File_write_at_all_end, PMPI_File_write_ordered, PMPI_File_write_ordered_begin, PMPI_File_write_ordered_end, PMPI_File_write_shared, PMPI_Finalize, PMPI_Finalized, PMPI_Free_mem, PMPI_Gather, PMPI_Gatherv, PMPI_Get, PMPI_Get_accumulate, PMPI_Get_address, PMPI_Get_count, PMPI_Get_elements, PMPI_Get_elements_x, PMPI_Get_library_version, PMPI_Get_processor_name, PMPI_Get_version, PMPI_Graph_create, PMPI_Graph_get, PMPI_Graph_map, PMPI_Graph_neighbors, PMPI_Graph_neighbors_count, PMPI_Graphdims_get, PMPI_Grequest_complete, PMPI_Grequest_start, PMPI_Group_c2f, PMPI_Group_compare, PMPI_Group_difference, PMPI_Group_excl, PMPI_Group_f2c, PMPI_Group_free, PMPI_Group_incl, PMPI_Group_intersection, PMPI_Group_range_excl, PMPI_Group_range_incl, PMPI_Group_rank, PMPI_Group_size, PMPI_Group_translate_ranks, PMPI_Group_union, PMPI_Iallgather, PMPI_Iallgatherv, PMPI_Iallreduce, PMPI_Ialltoall, PMPI_Ialltoallv, PMPI_Ialltoallw, PMPI_Ibarrier, PMPI_Ibcast, PMPI_Ibsend, PMPI_Iexscan, PMPI_Igather, PMPI_Igatherv, PMPI_Improbe, PMPI_Imrecv, PMPI_Ineighbor_allgather, PMPI_Ineighbor_allgatherv, PMPI_Ineighbor_alltoall, PMPI_Ineighbor_alltoallv, PMPI_Ineighbor_alltoallw, PMPI_Info_c2f, PMPI_Info_create, PMPI_Info_delete, PMPI_Info_dup, PMPI_Info_f2c, PMPI_Info_free, PMPI_Info_get, PMPI_Info_get_nkeys, PMPI_Info_get_nthkey, PMPI_Info_get_valuelen, PMPI_Info_set, PMPI_Init, PMPI_Init_thread, PMPI_Initialized, PMPI_Intercomm_create, PMPI_Intercomm_merge, PMPI_Iprobe, PMPI_Irecv, PMPI_Ireduce, PMPI_Ireduce_scatter, PMPI_Ireduce_scatter_block, PMPI_Irsend, PMPI_Is_thread_main, PMPI_Iscan, PMPI_Iscatter, PMPI_Iscatterv, PMPI_Isend, PMPI_Issend, PMPI_Keyval_create, PMPI_Keyval_free, PMPI_Lookup_name, PMPI_Mprobe, PMPI_Mrecv, PMPI_Neighbor_allgather, PMPI_Neighbor_allgatherv, PMPI_Neighbor_alltoall, PMPI_Neighbor_alltoallv, PMPI_Neighbor_alltoallw, PMPI_Op_c2f, PMPI_Op_commutative, PMPI_Op_create, PMPI_Op_f2c, PMPI_Op_free, PMPI_Open_port, PMPI_Pack, PMPI_Pack_external, PMPI_Pack_external_size, PMPI_Pack_size, PMPI_Pcontrol, PMPI_Probe, PMPI_Publish_name, PMPI_Put, PMPI_Query_thread, PMPI_Raccumulate, PMPI_Recv, PMPI_Recv_init, PMPI_Reduce, PMPI_Reduce_local, PMPI_Reduce_scatter, PMPI_Reduce_scatter_block, PMPI_Register_datarep, PMPI_Request_c2f, PMPI_Request_f2c, PMPI_Request_free, PMPI_Request_get_status, PMPI_Rget, PMPI_Rget_accumulate, PMPI_Rput, PMPI_Rsend, PMPI_Rsend_init, PMPI_Scan, PMPI_Scatter, PMPI_Scatterv, PMPI_Send, PMPI_Send_init, PMPI_Sendrecv, PMPI_Sendrecv_replace, PMPI_Sizeof, PMPI_Ssend, PMPI_Ssend_init, PMPI_Start, PMPI_Startall, PMPI_Status_c2f, PMPI_Status_f2c, PMPI_Status_set_cancelled, PMPI_Status_set_elements, PMPI_Status_set_elements_x, PMPI_Test, PMPI_Test_cancelled, PMPI_Testall, PMPI_Testany, PMPI_Testsome, PMPI_Topo_test, PMPI_Type_c2f, PMPI_Type_commit, PMPI_Type_contiguous, PMPI_Type_create_darray, PMPI_Type_create_f90_complex, PMPI_Type_create_f90_integer, PMPI_Type_create_f90_real, PMPI_Type_create_hindexed, PMPI_Type_create_hindexed_block, PMPI_Type_create_hvector, PMPI_Type_create_indexed_block, PMPI_Type_create_keyval, PMPI_Type_create_resized, PMPI_Type_create_struct, PMPI_Type_create_subarray, PMPI_Type_delete_attr, PMPI_Type_dup, PMPI_Type_extent, PMPI_Type_f2c, PMPI_Type_free, PMPI_Type_free_keyval, PMPI_Type_get_attr, PMPI_Type_get_contents, PMPI_Type_get_envelope, PMPI_Type_get_extent, PMPI_Type_get_extent_x, PMPI_Type_get_name, PMPI_Type_get_true_extent, PMPI_Type_get_true_extent_x, PMPI_Type_hindexed, PMPI_Type_hvector, PMPI_Type_indexed, PMPI_Type_lb, PMPI_Type_match_size, PMPI_Type_set_attr, PMPI_Type_set_name, PMPI_Type_size, PMPI_Type_size_x, PMPI_Type_struct, PMPI_Type_ub, PMPI_Type_vector, PMPI_Unpack, PMPI_Unpack_external, PMPI_Unpublish_name, PMPI_Wait, PMPI_Waitall, PMPI_Waitany, PMPI_Waitsome, PMPI_Win_allocate, PMPI_Win_allocate_shared, PMPI_Win_attach, PMPI_Win_c2f, PMPI_Win_call_errhandler, PMPI_Win_complete, PMPI_Win_create, PMPI_Win_create_dynamic, PMPI_Win_create_errhandler, PMPI_Win_create_keyval, PMPI_Win_delete_attr, PMPI_Win_detach, PMPI_Win_f2c, PMPI_Win_fence, PMPI_Win_flush, PMPI_Win_flush_all, PMPI_Win_flush_local, PMPI_Win_flush_local_all, PMPI_Win_free, PMPI_Win_free_keyval, PMPI_Win_get_attr, PMPI_Win_get_errhandler, PMPI_Win_get_group, PMPI_Win_get_info, PMPI_Win_get_name, PMPI_Win_lock, PMPI_Win_lock_all, PMPI_Win_post, PMPI_Win_set_attr, PMPI_Win_set_errhandler, PMPI_Win_set_info, PMPI_Win_set_name, PMPI_Win_shared_query, PMPI_Win_start, PMPI_Win_sync, PMPI_Win_test, PMPI_Win_unlock, PMPI_Win_unlock_all, PMPI_Win_wait, PMPI_Wtick, PMPI_Wtime], [], [], [[#include <mpi.h>]])

  if test "x${scorep_mpi_fortran_supported}" = "xyes"; then
    AC_SCOREP_MPI_FORTRAN_CONSTANTS
  fi
  AC_SCOREP_MPI_C_DATATYPES
  AC_SCOREP_MPI_COMPLIANCE

fi # if test "x${scorep_mpi_supported}" = "xyes"
])


dnl ----------------------------------------------------------------------------


AC_DEFUN([_AC_SCOREP_MPI_INCLUDE], [
AC_REQUIRE([AC_PROG_GREP])
ac_scorep_have_mpi_include="no"
AC_LANG_PUSH([C])
AC_PREPROC_IFELSE([AC_LANG_PROGRAM([[#include <mpi.h>]], [])],
                  [GREP_OPTIONS= ${GREP} '/mpi.h"' conftest.i > conftest_mpi_includes
                   AS_IF([test $? -eq 0],
                         [scorep_mpi_include=`cat conftest_mpi_includes | GREP_OPTIONS= ${GREP} '/mpi.h"' | \
                          head -1 | sed -e 's#^.* "##' -e 's#/mpi.h".*##'`
                          ac_scorep_have_mpi_include="yes"],
                         [])
                   rm -f conftest_mpi_includes],
                  [])
AC_LANG_POP([C])

AS_IF([test "x${ac_scorep_have_mpi_include}" = "xyes"],
      [AC_SUBST([SCOREP_MPI_INCLUDE], [${scorep_mpi_include}])],
      [])
])


dnl ----------------------------------------------------------------------------


AC_DEFUN([_SCOREP_PDT_MPI_INSTRUMENTATION], [
AC_REQUIRE([_AC_SCOREP_MPI_INCLUDE])

AS_IF([test "x${ac_scorep_have_mpi_include}" = "xyes"],
      [AC_SUBST([SCOREP_HAVE_PDT_MPI_INSTRUMENTATION], [1])
       AFS_SUMMARY([PDT MPI instrumentation], [yes, if PDT available])],
      [AC_SUBST([SCOREP_HAVE_PDT_MPI_INSTRUMENTATION], [0])
       AC_MSG_WARN([cannot determine mpi.h include path. PDT MPI instrumentation will be disabled.])
       AFS_SUMMARY([PDT MPI instrumentation], [no, mpi.h include path could not be determined.])])
])
