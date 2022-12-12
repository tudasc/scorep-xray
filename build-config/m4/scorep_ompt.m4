dnl
dnl This file is part of the Score-P software (http://www.score-p.org)
dnl
dnl Copyright (c) 2022,
dnl Forschungszentrum Juelich GmbH, Germany
dnl
dnl This software may be modified and distributed under the terms of
dnl a BSD-style license.  See the COPYING file in the package base
dnl directory for details.
dnl


# SCOREP_OMPT_SUMMARY()
# ---------------------
#
AC_DEFUN_ONCE([SCOREP_OMPT_SUMMARY], [
AC_REQUIRE([SCOREP_OMPT])
AFS_SUMMARY_PUSH
AS_IF([test "x${scorep_have_addr2line}${afs_have_thread_local_storage}${scorep_have_openmp_c_support}" = xyesyesyes],
    [AFS_SUMMARY([OMPT header], [$have_ompt_header${ompt_reason_header:+, $ompt_reason_header}])
     AS_IF([test "x${have_ompt_header}" = xyes],
         [AFS_SUMMARY([OMPT tool], [$have_ompt_tool])
          AS_IF([test "x${have_ompt_tool}" = xyes],
              [AFS_SUMMARY([OMPT C support], [$have_ompt_c_support${ompt_reason_c:+, $ompt_reason_c}])
               AS_IF([test "x${scorep_have_openmp_cxx_support}" = xyes],
                   [AFS_SUMMARY([OMPT C++ support], [$have_ompt_cxx_support${ompt_reason_cxx:+, $ompt_reason_cxx}])])
               AS_IF([test "x${scorep_have_openmp_fc_support}" = xyes],
                   [AFS_SUMMARY([OMPT Fortran support], [$have_ompt_fc_support${ompt_reason_fc:+, $ompt_reason_fc}])])
               AFS_SUMMARY([OMPT is default], [$scorep_enable_default_ompt])])])])
AFS_SUMMARY_POP([OMPT support], [$have_ompt_support${ompt_reason:+, $ompt_reason}])
])dnl SCOREP_OMPT_SUMMARY


# SCOREP_OMPT()
# -------------
# Check whether OMPT is supported by checking for omp-tools.h,
# building a tool with the C compiler, and using/running the tool with
# the C, C++, and Fortran compiler. Don't call from MPI/SHMEM as they
# need a launcher for running, but rely on build-backend|mpi|shmem
# compatibility.
#
AC_DEFUN_ONCE([SCOREP_OMPT], [
AC_REQUIRE([SCOREP_OPENMP])
AC_REQUIRE([SCOREP_ADDR2LINE])
AC_REQUIRE([AFS_CHECK_THREAD_LOCAL_STORAGE])
AC_REQUIRE([SCOREP_ENABLE_DEFAULT])

have_ompt_header=no
have_ompt_tool=no
have_ompt_support=no
have_ompt_c_support=no
have_ompt_cxx_support=no
have_ompt_fc_support=no

AS_IF([test "x${scorep_have_addr2line}" = xyes],
    [AS_IF([test "x${afs_have_thread_local_storage}" = xyes],
         [AS_IF([test "x${scorep_have_openmp_c_support}" = xyes],
              [_CHECK_OMPT_HEADER
               AS_IF([test "x${have_ompt_header}" = xyes],
                   [_BUILD_OMPT_TOOL
                    AS_IF([test "x${have_ompt_tool}" = xyes],
                        [AC_LANG_PUSH([C])
                         _CHECK_OMPT_SUPPORT
                         AC_LANG_POP([C])
                         AS_IF([test "x${scorep_have_openmp_cxx_support}" = xyes],
                             [AC_LANG_PUSH([C++])
                              _CHECK_OMPT_SUPPORT
                              AC_LANG_POP([C++])])
                         AS_IF([test "x${scorep_have_openmp_fc_support}" = xyes],
                             [AC_LANG_PUSH([Fortran])
                              _CHECK_OMPT_SUPPORT
                              AC_LANG_POP([Fortran])])])
                    rm -f tool.$ac_objext])],
              [ompt_reason="OpenMP C support missing"])],
         [ompt_reason="thread local storage support missing"])],
    [ompt_reason="addr2line support missing"])
dnl
AC_SCOREP_COND_HAVE([SCOREP_OMPT_SUPPORT],
    [test "x$have_ompt_support" = xyes],
    [Defined if OMPT is supported for at least one language])
dnl
AC_SCOREP_COND_HAVE([SCOREP_DEFAULT_OMPT],
    [test "x$have_ompt_support" = xyes && test "x$scorep_enable_default_ompt" = xyes],
    [Defined if OMPT is the default OpenMP instrumentation method.])
])dnl SCOREP_OMPT


# _CHECK_OMPT_HEADER()
# --------------------
#
m4_define([_CHECK_OMPT_HEADER], [
AS_IF([test "x${ac_cv_prog_cc_openmp}" != xunsupported],
    [AC_LANG_PUSH([C])
     CFLAGS_save="${CFLAGS}"
     CFLAGS="${OPENMP_CFLAGS} ${CFLAGS}"
     # ompt-tools.h
     AC_CHECK_HEADER([omp-tools.h],
         [have_ompt_header=yes])
     AS_IF([test "x${have_ompt_header}" = xyes],
         [# check for required enum values
          AC_COMPILE_IFELSE(
              [AC_LANG_PROGRAM([#include <omp-tools.h>], [
/* require version 5.1+ for sync_region */
ompt_sync_region_t foo1 = ompt_sync_region_barrier_implicit_workshare;
ompt_sync_region_t foo2 = ompt_sync_region_barrier_implicit_parallel;
ompt_sync_region_t foo3 = ompt_sync_region_barrier_teams;
          ])],
              [# success
               # check for optional enum values, if any
               #
               # ompt_scope_beginend: used in target-global-data-op,
               # target-data-associate, target-data-disassociate
               # events. Omission produces warnings.
               #
               # ompt_task_taskwait: to detect taskwait-init event
               # (since 5.1)
               #
               AC_CHECK_DECLS([ompt_scope_beginend,
                               ompt_task_taskwait,
                               ompt_sync_region_barrier_implicit_workshare,
                               ompt_sync_region_barrier_implicit_parallel,
                               ompt_sync_region_barrier_teams,
                               ompt_work_loop_static,
                               ompt_work_loop_dynamic,
                               ompt_work_loop_guided,
                               ompt_work_loop_other,
                               ompt_dispatch_ws_loop_chunk,
                               ompt_dispatch_taskloop_chunk,
                               ompt_dispatch_distribute_chunk], [], [], [[#include <omp-tools.h>]])],
              [# failure
               AC_MSG_WARN([OMPT header lacks version 5.1 enum values])
               ompt_reason_header="lacks version 5.1 enum values"
               have_ompt_header=no])])
     CFLAGS="$CFLAGS_save"
     AC_LANG_POP([C])])
])dnl _CHECK_OMPT_HEADER


# _BUILD_OMPT_TOOL()
# ------------------
# Build tool.$ac_objext
#
m4_define([_BUILD_OMPT_TOOL], [
AC_LANG_PUSH([C])
CFLAGS_save="${CFLAGS}"
CFLAGS="${OPENMP_CFLAGS} ${CFLAGS}"
AC_COMPILE_IFELSE([AC_LANG_SOURCE(_INPUT_OMPT_TOOL)],
    [have_ompt_tool=yes
     tool_o="tool.$ac_objext"
     # preserve object file for use at a later time
     cp conftest.$ac_objext $tool_o])
CFLAGS="$CFLAGS_save"
AC_LANG_POP([C])
AC_MSG_CHECKING([whether an OMPT tool can be compiled])
AC_MSG_RESULT([$have_ompt_tool])
])dnl _BUILD_OMPT_TOOL


# _CHECK_TOOL_ACTIVATION()
# ------------------------
# Common part of _CHECK_OMPT_SUPPORT (see below).
#
m4_define([_CHECK_TOOL_ACTIVATION], [
AC_LINK_IFELSE([_INPUT_IGNORED],
    [# Assumption: Cray systems use same architecture for login- and
     # compute-nodes. Thus, running conftest programs is ok. To do
     # so, set cross_compiling temporarily to "no".
     cross_compiling_save="${cross_compiling}"
     AS_IF([test "x${afs_platform_cray}" = xyes],
         [cross_compiling=no])
     AFS_RUN_IFELSE(
         [# do not try to run MPI or SHMEM programs (no launcher)
          have_ompt_support=yes
          have_ompt_[]_AC_LANG_ABBREV[]_support=yes
          # Source omp_version, runtime_version, and
          # initial_device_num as reported from conftest run.
          # Could be used in summary output.
          . ./ompt
          rm -f ./ompt
          AS_UNSET([ompt_reason_]_AC_LANG_ABBREV)],
         [tool_got_activated=yes
          AS_CASE(["x${ac_retval}"],
              [x1], [ompt_reason_[]_AC_LANG_ABBREV="tool not activated"
                     tool_got_activated=no],
              [x2], [ompt_reason_[]_AC_LANG_ABBREV="overdue events not dispatched"],
              [x3], [ompt_reason_[]_AC_LANG_ABBREV="ompt_set_callback lookup failed"],
              [x4], [ompt_reason_[]_AC_LANG_ABBREV="ompt_finalize_tool lookup failed"],
              [x5], [ompt_reason_[]_AC_LANG_ABBREV="thread-begin cb registration failed"],
              [x6], [ompt_reason_[]_AC_LANG_ABBREV="thread-end cb registration failed"],
              [tool_got_activated=no])
         ],
         [AC_MSG_FAILURE([TODO: handle real cross-compile systems])])
     cross_compiling="${cross_compiling_save}"],
    [ompt_reason_[]_AC_LANG_ABBREV="tool cannot be linked"])
])dnl _CHECK_TOOL_ACTIVATION


# _CHECK_OMPT_SUPPORT()
# ---------------------
# Compile OpenMP application and link with tool.$ac_objext (see
# _BUILD_OMPT_TOOL). Run the binary; the return value indicates
# whether OMPT got activated. Call in C context first.
#
m4_define([_CHECK_OMPT_SUPPORT], [
_AC_LANG_PREFIX[]FLAGS_save="$_AC_LANG_PREFIX[]FLAGS"
_AC_LANG_PREFIX[]FLAGS="$OPENMP_[]_AC_LANG_PREFIX[]FLAGS $_AC_LANG_PREFIX[]FLAGS"
AS_UNSET([tool_got_activated])
AC_COMPILE_IFELSE([AC_LANG_SOURCE(_INPUT_OPENMP_[]_AC_LANG_PREFIX)],
    [cp conftest.$ac_objext main.$ac_objext
     # use customized link command
     ac_link_save="$ac_link"
     m4_case(_AC_CC,
         [CC], [ac_link='$CC -o conftest$ac_exeext $CFLAGS $CPPFLAGS $LDFLAGS $tool_o main.$ac_objext $LIBS >&5'],
         [CXX], [ac_link='$CXX -o conftest$ac_exeext $CXXFLAGS $CPPFLAGS $LDFLAGS $tool_o main.$ac_objext $LIBS >&5'],
         [FC], [ac_link='$FC -o conftest$ac_exeext $FCFLAGS $LDFLAGS $tool_o main.$ac_objext $LIBS >&5'],
         [m4_fatal([Unsupported language ]_AC_CC)])
     #
     # Compilers might need extra_ldflags to activate OMPT. Require the CC
     # setup to work with CXX and FC as well (as we don't distinguish
     # language at link time).
     #
     # NVHPC 22.5, 22.7: -mt=ompt
     #
     LDFLAGS_save="$LDFLAGS"
     m4_case(_AC_CC,
         [CC],
             [for extra_ldflags in "" "-mp=ompt"; do
                  LDFLAGS="$extra_ldflags $LDFLAGS_save"
                  _CHECK_TOOL_ACTIVATION
                  AS_IF([test "x${have_ompt_[]_AC_LANG_ABBREV[]_support}" = xyes ||
                         test "x${tool_got_activated}" = xyes],
                      [break])
              done
              ompt_[]_AC_LANG_ABBREV[]_ldflags="${extra_ldflags}"
              AC_SUBST([SCOREP_OMPT_LDFLAGS], [${ompt_[]_AC_LANG_ABBREV[]_ldflags}])],
         [LDFLAGS="${ompt_c_ldflags} ${LDFLAGS}"
          _CHECK_TOOL_ACTIVATION])
     LDFLAGS="$LDFLAGS_save"
     rm -f main.$ac_objext
     ac_link="$ac_link_save"],
    [ompt_reason_[]_AC_LANG_ABBREV="OpenMP program does not compile"])
_AC_LANG_PREFIX[]FLAGS="$_AC_LANG_PREFIX[]FLAGS_save"
AC_MSG_CHECKING([whether OMPT is supported for language ]_AC_LANG)
AC_MSG_RESULT([$have_ompt_[]_AC_LANG_ABBREV[]_support${ompt_reason_[]_AC_LANG_ABBREV:+, $ompt_reason_[]_AC_LANG_ABBREV}])
])dnl _CHECK_OMPT_SUPPORT


# _INPUT_IGNORED()
# ----------------
#
m4_define([_INPUT_IGNORED], [AC_LANG_PROGRAM([], [])])


# _INPUT_OMPT_TOOL()
# ------------------
#
m4_define([_INPUT_OMPT_TOOL], [[
#include <omp-tools.h>
#include <stdlib.h>
#include <stdio.h>
static int initialized;
static int overdue_dispatched;
static ompt_finalize_tool_t finalize_tool;
FILE* ompt;

void cb_thread_begin( ompt_thread_t thread_type,
                      ompt_data_t*  thread_data )
{
    static uint64_t thread_cnt;
    /* no need to lock increment, just a single worker thread, see main() below. */
    thread_data->value = thread_cnt++;
}

void cb_thread_end( ompt_data_t* thread_data )
{
    if ( thread_data->value ==1 )
    {
        /* overdue event on worker thread got dispatched. */
        overdue_dispatched = 1;
    }
}

static int ompt_initialize( ompt_function_lookup_t lookup,
                            int initial_device_num,
                            ompt_data_t *tool_data )
{
    fprintf( ompt, "initial_device_num=\"%d\"\n", initial_device_num );
    fclose( ompt );

    ompt_set_callback_t set_cb = ( ompt_set_callback_t )lookup( "ompt_set_callback" );
    if ( !set_cb )
    {
        _Exit( 3 ); /* Tool got initialized but lookup of runtime-entry-point ompt_set_callback failed. */
    }
    finalize_tool = ( ompt_finalize_tool_t )lookup( "ompt_finalize_tool" );
    if ( !finalize_tool )
    {
        _Exit( 4 ); /* Tool got initialized but lookup of runtime-entry-point ompt_finalize_tool failed. */
    }

    ompt_set_result_t result;
    result = set_cb( ompt_callback_thread_begin, ( ompt_callback_t )&cb_thread_begin );
    if ( result != ompt_set_always )
    {
        _Exit( 5 ); /* Tool got initialized but thread_begin cb couldn't be registered. */
    }
    result = set_cb( ompt_callback_thread_end, ( ompt_callback_t )&cb_thread_end );
    if ( result != ompt_set_always )
    {
        _Exit( 6 ); /* Tool got initialized but thread_end cb couldn't be registered. */
    }

    initialized = 1;
    return 1; /* non-zero indicates success for OMPT runtime. */
}

static void ompt_finalize( ompt_data_t *tool_data )
{

    if ( initialized == 1 )
    {
        if ( overdue_dispatched == 1 )
        {
            _Exit( 0 ); /* Tool got initialized and finalized as well as overdue event on worker got dispatched. */
        }
        _Exit( 2 ); /* Tool got initialized and finalized but overdue event on worker wasn't dispatched. */
    }
}

ompt_start_tool_result_t* ompt_start_tool( unsigned int omp_version, /* == _OPENMP */
                                           const char*  runtime_version )
{
    ompt = fopen( "./ompt", "w");
    fprintf( ompt, "omp_version=\"%d\"\nruntime_version=\"%s\"\n", omp_version, runtime_version );
    static ompt_start_tool_result_t ompt_start_tool_result = { &ompt_initialize,
                                                               &ompt_finalize,
                                                               ompt_data_none };
    return &ompt_start_tool_result;
}

void foo( int tid )
{
    /* called from main.o */
}

void shut_down_tool( int dummy )
{
    if ( initialized == 1 )
    {
        finalize_tool(); /* supposed to trigger overdue events */
    }
}
]])dnl _INPUT_OMPT_TOOL


# _INPUT_OPENMP_C()
# -----------------
#
m4_define([_INPUT_OPENMP_C], [[
#include <omp.h>
#ifdef __cplusplus
extern "C" {
#endif
void foo( int tid ); /* defined in tool.o, thus the parallel region can't be optimized out */
void shut_down_tool( int dummy );
#ifdef __cplusplus
}
#endif

int main()
{
    #pragma omp parallel num_threads( 2 )
    foo( omp_get_thread_num() );

    shut_down_tool(42);
    return 1; /* Indicates that the tool was not initialized and finalized. */
}
]])dnl _INPUT_OPENMP_C


# _INPUT_OPENMP_CXX()
# -------------------
#
m4_copy([_INPUT_OPENMP_C], [_INPUT_OPENMP_CXX])


# _INPUT_OPENMP_FC()
# ------------------
#
m4_define([_INPUT_OPENMP_FC], [[
      PROGRAM main
      USE OMP_LIB
      INTERFACE
      SUBROUTINE foo(tid) BIND(C)
      USE, INTRINSIC :: ISO_C_BINDING, ONLY: C_INT
      IMPLICIT NONE
      INTEGER(C_INT) :: tid
      END SUBROUTINE foo
      SUBROUTINE shut_down_tool(dummy) BIND(C)
      USE, INTRINSIC :: ISO_C_BINDING, ONLY: C_INT
      IMPLICIT NONE
      INTEGER(C_INT) :: dummy
      END SUBROUTINE shut_down_tool
      END INTERFACE
!\$OMP PARALLEL DEFAULT(NONE) NUM_THREADS(2)
      call foo(OMP_GET_THREAD_NUM())
!\$OMP END PARALLEL
      call shut_down_tool(42)
      CALL EXIT(1)
      END
]])dnl _INPUT_OPENMP_FC
