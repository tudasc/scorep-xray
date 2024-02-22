## -*- mode: autoconf -*-

##
## This file is part of the Score-P software (http://www.score-p.org)
##
## Copyright (c) 2015-2016, 2019,
## Technische Universitaet Dresden, Germany
##
## Copyright (c) 2021-2022, 2024,
## Forschungszentrum Juelich GmbH, Germany
##
## This software may be modified and distributed under the terms of
## a BSD-style license. See the COPYING file in the package base
## directory for details.
##

## file build-config/m4/scorep_openacc.m4


# SCOREP_OPENACC()
# ----------------
# - Check if the C/CXX/FC compilers support OpenACC.
#   Substitutes OPENACC_C|CXX|FCFLAGS.
# - Check if the OpenACC profiling interface can be used and
#   communicate status via HAVE_[BACKEND_]OPENACC_PROFILING_SUPPORT.
#
AC_DEFUN([SCOREP_OPENACC], [
AC_REQUIRE([SCOREP_COMPUTENODE_CC])dnl
have_openacc_support=no
AC_LANG_PUSH([C])
_CHECK_OPENACC_COMPILER_SUPPORT
AC_LANG_POP([C])
AS_IF([test "x${afs_cv_prog_cxx_works}" = xyes],
    [AC_LANG_PUSH([C++])
     _CHECK_OPENACC_COMPILER_SUPPORT
     AC_LANG_POP([C++])])
AS_IF([test "x${afs_cv_prog_fc_works}" = xyes],
    [AC_LANG_PUSH([Fortran])
     _CHECK_OPENACC_COMPILER_SUPPORT
     AC_LANG_POP([Fortran])])

AS_IF([test "x${have_openacc_support}" = xyes],
    [AC_LANG_PUSH([C])
     _CHECK_OPENACC_PROFILING_INTERFACE
     AC_LANG_POP([C])])

AC_SCOREP_COND_HAVE([OPENACC_PROFILING_SUPPORT],
    [test "x${have_openacc_profiling_support}" = xyes],
    [Defined if an OpenACC profiling tool can be compiled])

AFS_SUMMARY_PUSH
AFS_SUMMARY([C support], [$have_openacc_c_support])
AS_IF([test "x${afs_cv_prog_cxx_works}" = xyes],
    [AFS_SUMMARY([C++ support], [$have_openacc_cxx_support])])
AS_IF([test "x${afs_cv_prog_fc_works}" = xyes],
    [AFS_SUMMARY([Fortran support], [$have_openacc_fc_support])])
AS_IF([test "x${have_openacc_support}" = xyes],
    [AFS_SUMMARY([Profiling support], [$have_openacc_profiling_support${openacc_profiling_reason:+, $openacc_profiling_reason}])])
AFS_SUMMARY_POP([OpenACC support], [$have_openacc_support])
])dnl SCOREP_OPENACC


# _CHECK_OPENACC_COMPILER_SUPPORT()
# ---------------------------------
# Determine compiler flag (if any) to activate OpenACC.
#
m4_define([_CHECK_OPENACC_COMPILER_SUPPORT], [
AC_MSG_CHECKING([whether OpenACC is supported by the ]_AC_LANG_PREFIX[ compiler])
have_openacc_[]_AC_LANG_ABBREV[]_support=no
AC_LINK_IFELSE([AC_LANG_SOURCE(_INPUT_OPENACC_[]_AC_LANG_PREFIX)],
    [openacc_[]_AC_LANG_ABBREV[]_option="none needed"
     have_openacc_support=yes
     have_openacc_[]_AC_LANG_ABBREV[]_support="yes, no flag needed"],
    [openacc_[]_AC_LANG_ABBREV[]_option='unsupported'
     dnl Try these flags:
     dnl GCC      -fopenacc
     dnl NVIDIA   -acc
     dnl Cray CCE -hacc (Fortran only)
     for acc_option in -fopenacc -acc -hacc; do
         save_[]_AC_LANG_PREFIX[]FLAGS=$[]_AC_LANG_PREFIX[]FLAGS
         _AC_LANG_PREFIX[]FLAGS="$[]_AC_LANG_PREFIX[]FLAGS $acc_option"
         AC_LINK_IFELSE([AC_LANG_SOURCE(_INPUT_OPENACC_[]_AC_LANG_PREFIX)],
             [openacc_[]_AC_LANG_ABBREV[]_option=$acc_option
              have_openacc_support=yes
              have_openacc_[]_AC_LANG_ABBREV[]_support="yes, recognizing $acc_option"])
         _AC_LANG_PREFIX[]FLAGS=$save_[]_AC_LANG_PREFIX[]FLAGS
         AS_IF([test "x${openacc_[]_AC_LANG_ABBREV[]_option}" != xunsupported],
             [break])
     done
    ])
AC_MSG_RESULT([${have_openacc_[]_AC_LANG_ABBREV[]_support}])

AS_CASE([${openacc_[]_AC_LANG_ABBREV[]_option}],
    ["none needed" | unsupported], [OPENACC_[]_AC_LANG_PREFIX[]FLAGS=],
    [OPENACC_[]_AC_LANG_PREFIX[]FLAGS=${openacc_[]_AC_LANG_ABBREV[]_option}])
AC_SUBST([OPENACC_]_AC_LANG_PREFIX[FLAGS])
])dnl _CHECK_OPENACC_COMPILER_SUPPORT


# _INPUT_OPENACC_C()
# ------------------
#
m4_define([_INPUT_OPENACC_C], [[
#ifndef _OPENACC
choke me
#endif
#include <openacc.h>
int main() { return acc_get_num_devices( acc_device_default ); }
]])dnl _INPUT_OPENACC_C


# _INPUT_OPENACC_CXX()
# --------------------
#
m4_copy([_INPUT_OPENACC_C], [_INPUT_OPENACC_CXX])


# _INPUT_OPENACC_FC()
# -------------------
#
m4_define([_INPUT_OPENACC_FC], [[
      PROGRAM main
      USE OPENACC
      INTEGER NDEV
      NDEV = ACC_GET_NUM_DEVICES( ACC_DEVICE_DEFAULT )
      END
]])dnl _INPUT_OPENACC_FC


# _CHECK_OPENACC_PROFILING_INTERFACE()
# ------------------------------------
# As of 2024-02,
# - there is no runtime that needs stdlib.h to include acc_prof.h,
# - there is no runtime that needs OPENACC_CFLAGS to include acc_prof.h.
# - An nvc-compiled OpenACC program calls acc_register_library (JURECA Stages/2024).
# - No success with GCC 11.4.0 though.
# Be pragmatic and check for the profiling header acc_prof.h and try
# to compile a tool. This is good enough to build the OpenACC
# adapter. If a tool gets activated, i.e., if acc_register_library is
# called, will not be tested here. Such a test would require to run a
# program, preferably with OpenACC being executed on a device.
#
m4_define([_CHECK_OPENACC_PROFILING_INTERFACE], [
AS_UNSET([openacc_profiling_reason])
AC_CHECK_HEADER([acc_prof.h],
    [AC_MSG_CHECKING([whether an OpenACC profiling tool can be compiled])
     AC_COMPILE_IFELSE([AC_LANG_SOURCE([[
#include <acc_prof.h>

static void
handle_event_cb( acc_prof_info*  profInfo,
                 acc_event_info* eventInfo,
                 acc_api_info*   apiInfo )
{
    /* Check for decls used in the OpenACC adapter */
    switch ( profInfo->event_type )
    {
        case acc_ev_alloc:
        case acc_ev_compute_construct_end:
        case acc_ev_compute_construct_start:
        case acc_ev_device_init_end:
        case acc_ev_device_init_start:
        case acc_ev_device_shutdown_end:
        case acc_ev_device_shutdown_start:
        case acc_ev_enqueue_download_end:
        case acc_ev_enqueue_download_start:
        case acc_ev_enqueue_launch_end:
        case acc_ev_enqueue_launch_start:
        case acc_ev_enqueue_upload_end:
        case acc_ev_enqueue_upload_start:
        case acc_ev_enter_data_end:
        case acc_ev_enter_data_start:
        case acc_ev_exit_data_end:
        case acc_ev_exit_data_start:
        case acc_ev_free:
        case acc_ev_update_end:
        case acc_ev_update_start:
        case acc_ev_wait_end:
        case acc_ev_wait_start:
            break;
    }

    eventInfo->data_event.bytes;
    eventInfo->data_event.device_ptr;
    eventInfo->data_event.var_name;
    eventInfo->launch_event.kernel_name;
    eventInfo->launch_event.num_gangs;
    eventInfo->launch_event.num_workers;
    eventInfo->launch_event.vector_length;
    eventInfo->other_event.implicit;
    eventInfo->other_event.tool_info;
    profInfo->device_number;
    profInfo->device_type;
    profInfo->line_no;
    profInfo->src_file;

    acc_data_event_info data_info = eventInfo->data_event;
}

void
acc_register_library( acc_prof_reg accRegister,
                      acc_prof_reg accUnregister,
                      acc_prof_lookup_func lookup )
{
    accRegister( acc_ev_device_init_start, handle_event_cb, (acc_register_t)0 );
}
    ]])],
         [have_openacc_profiling_support=yes],
         [have_openacc_profiling_support=no
          openacc_profiling_reason="tool cannot be compiled"
          have_openacc_support=no])
          AC_MSG_RESULT([$have_openacc_profiling_support])],
    [have_openacc_profiling_support=no
     openacc_profiling_reason="acc_prof.h not available"
     have_openacc_support=no]) dnl AC_CHECK_HEADER
])dnl _CHECK_OPENACC_PROFILING_INTERFACE
