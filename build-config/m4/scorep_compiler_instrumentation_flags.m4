dnl -*- mode: autoconf -*-

dnl
dnl This file is part of the Score-P software (http://www.score-p.org)
dnl
dnl Copyright (c) 2009-2012,
dnl RWTH Aachen University, Germany
dnl
dnl Copyright (c) 2009-2012,
dnl Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
dnl
dnl Copyright (c) 2009-2014, 2019, 2021,
dnl Technische Universitaet Dresden, Germany
dnl
dnl Copyright (c) 2009-2012,
dnl University of Oregon, Eugene, USA
dnl
dnl Copyright (c) 2009-2014, 2021-2022,
dnl Forschungszentrum Juelich GmbH, Germany
dnl
dnl Copyright (c) 2009-2012,
dnl German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
dnl
dnl Copyright (c) 2009-2012,
dnl Technische Universitaet Muenchen, Germany
dnl
dnl This software may be modified and distributed under the terms of
dnl a BSD-style license.  See the COPYING file in the package base
dnl directory for details.
dnl

dnl file scorep_compiler_instrumentation.m4


# SCOREP_COMPILER_INSTRUMENTATION_FLAGS()
# ---------------------------------------
# Determine compiler options to instrument function enter and exits on
# a per-language basis. An instrumention option corresponds to an
# enter and exit API. Determine this API as well (out of GCC_PLUGIN,
# CYG_PROFILE_FUNC, FUNC_TRACE, PAT_TP_FUNC, VT_INTEL). Communicate
# supported APIs via defines:
# HAVE_SCOREP_COMPILER_INSTRUMENTATION_<API>
#
# Provide supported instrumentation flags (as ':'-separated strings if
# more than one flag is supported) via substitutions:
# SCOREP_COMPILER_INSTRUMENTATION_<LANG>FLAGS where LANG is out of C,
# CXX, FC.
#
# Provide required (language independent) linker flags (if any) via
# the substitution SCOREP_COMPILER_INSTRUMENTATION_LDFLAGS.
#
AC_DEFUN_ONCE([SCOREP_COMPILER_INSTRUMENTATION_FLAGS], [
AC_REQUIRE([AC_PROG_GREP])dnl
AC_REQUIRE([AC_PROG_AWK])dnl
dnl NM provided by libtool
AC_REQUIRE([SCOREP_COMPUTENODE_CC])dnl
AC_REQUIRE([SCOREP_COMPUTENODE_CXX])dnl
dnl AC_REQUIRE([SCOREP_COMPUTENODE_FC])dnl
AC_REQUIRE([SCOREP_ADDR2LINE])dnl
dnl
scorep_have_compiler_instrumentation=no
scorep_have_gcc_plugin_instrumentation=no
scorep_compiler_instrumentation_needs_addr2line=no
dnl
AFS_SUMMARY_PUSH
dnl
AC_LANG_PUSH([C])
_CHECK_COMPILER_INSTRUMENTATION_FLAGS
AC_LANG_POP([C])
AS_IF([test x$afs_cv_prog_cxx_works = xyes], [
    AC_LANG_PUSH([C++])
    _CHECK_COMPILER_INSTRUMENTATION_FLAGS
    AC_LANG_POP([C++])])
dnl The instrumenter doesn't differentiate between Fortran
dnl dialects. Thus, use FC instead of F77 for configure checks as we
dnl can query the FC vendor, as in C and CXX. Decisions based on
dnl vendor should be restricted to situations where compile-time
dnl checks won't work, though.
AS_IF([test x$afs_cv_prog_fc_works = xyes], [
    AC_LANG_PUSH([Fortran])
    _CHECK_COMPILER_INSTRUMENTATION_FLAGS
    AC_LANG_POP([Fortran])])
dnl
dnl SCOREP_COMPILER_INSTRUMENTATION_LDFLAGS:
# For compiler vendor cray, We used to set
# scorep_compiler_instrumentation_ldflags to
# "-Wl,-u,__pat_tp_func_entry,-u,__pat_tp_func_return". On daint, this
# doesn't semm to be neccesarry any longer (2022-03).
dnl
# As there is no addr2line lookup on mac, there is no
# __cyg_profile_func_* instrumentation. Thus, we don't need to set
# scorep_compiler_instrumentation_ldflags to "-Wl,-no_pie" (Disable
# position independent executable, which also disables address space
# randomization, which avoids matching addresses between NM and
# __cyg_profile_func_*) any longer.
AC_SUBST([SCOREP_COMPILER_INSTRUMENTATION_LDFLAGS], ["${scorep_compiler_instrumentation_ldflags}"])
dnl
AC_SCOREP_COND_HAVE([SCOREP_COMPILER_INSTRUMENTATION],
    [test "x${scorep_have_compiler_instrumentation}" = xyes],
    [Defined if compiler instrumentation is supported at all])
dnl
AC_SCOREP_COND_HAVE([SCOREP_COMPILER_INSTRUMENTATION_NEEDS_ADDR2LINE],
    [test "x${scorep_compiler_instrumentation_needs_addr2line}" = xyes],
    [Defined if one of the supported instrumentation APIs needs addr2line lookup])
dnl
AC_SCOREP_COND_HAVE([SCOREP_COMPILER_INSTRUMENTATION_GCC_PLUGIN],
    [test "x${scorep_have_gcc_plugin_instrumentation}" = xyes],
    [Defined if GCC_PLUGIN compiler instrumentation API is supported])
dnl
AFS_SUMMARY([C], [${instrumentation_c_api:-no}${instrumentation_cflags:+ via }${instrumentation_cflags}])
AFS_SUMMARY([C++], [${instrumentation_cxx_api:-no}${instrumentation_cxxflags:+ via }${instrumentation_cxxflags}])
AFS_SUMMARY([Fortran], [${instrumentation_fc_api:-no}${instrumentation_fcflags:+ via }${instrumentation_fcflags}])
AFS_SUMMARY_POP([Compiler instrumentation], [$scorep_have_compiler_instrumentation])
])dnl SCOREP_COMPILER_INSTRUMENTATION_FLAGS


# _CHECK_COMPILER_INSTRUMENTATION_FLAGS()
# ---------------------------------------
# Check known flags against known symbols of API. Stops after first succesful check.
#
m4_define([_CHECK_COMPILER_INSTRUMENTATION_FLAGS], [
dnl query gcc plug-in status
AS_IF([test -f ../build-gcc-plugin/gcc_plugin_supported_[]_AC_LANG_ABBREV[]],
    [AC_SCOREP_DEFINE_HAVE([SCOREP_COMPILER_INSTRUMENTATION_]_AC_CC[_GCC_PLUGIN],
         [1], [Defined if GCC plug-in is supported for language])
     instrumentation_[]_AC_LANG_ABBREV[]_api="gcc_plugin"
     scorep_have_compiler_instrumentation=yes
     scorep_have_gcc_plugin_instrumentation=yes])
dnl
# -g -finstrument-functions: GNU, clang-based
# -g -finstrument-functions-after-inlining: clang-based
# -Minstrument=functions: PGI/NVHPC
# -g -Ntl_vtrc: Fujitsu
# -g -Mx,129,0x800: intended for flang (https://github.com/flang-compiler/flang/issues/391), accepted by NVHPC
AS_IF([test "x${scorep_have_addr2line}" = xyes],
    [_CHECK_COMPILER_INSTRUMENTATION_FLAG([-g -Xarch_host -finstrument-functions-after-inlining], ['__cyg_profile_func_enter$'], ['__cyg_profile_func_exit$'], [CYG_PROFILE_FUNC])
     _CHECK_COMPILER_INSTRUMENTATION_FLAG([-g -finstrument-functions-after-inlining],             ['__cyg_profile_func_enter$'], ['__cyg_profile_func_exit$'], [CYG_PROFILE_FUNC])
     _CHECK_COMPILER_INSTRUMENTATION_FLAG([-g -Xarch_host -finstrument-functions],                ['__cyg_profile_func_enter$'], ['__cyg_profile_func_exit$'], [CYG_PROFILE_FUNC])
     _CHECK_COMPILER_INSTRUMENTATION_FLAG([-g -finstrument-functions],                            ['__cyg_profile_func_enter$'], ['__cyg_profile_func_exit$'], [CYG_PROFILE_FUNC])
     _CHECK_COMPILER_INSTRUMENTATION_FLAG([-g -Ntl_vtrc],                                         ['__cyg_profile_func_enter$'], ['__cyg_profile_func_exit$'], [CYG_PROFILE_FUNC])
     _CHECK_COMPILER_INSTRUMENTATION_FLAG([-Minstrument=functions],                               ['__cyg_profile_func_enter$'], ['__cyg_profile_func_exit$'], [CYG_PROFILE_FUNC])
     _CHECK_COMPILER_INSTRUMENTATION_FLAG([-g -Mx,129,0x800],                                     ['__cyg_profile_func_enter$'], ['__cyg_profile_func_exit$'], [CYG_PROFILE_FUNC])])
dnl
# Intel compilers
# API allows storing a handle, thus, no addr lookup needed
_CHECK_COMPILER_INSTRUMENTATION_FLAG([-tcollect], ['__VT_IntelEntry$'], ['__VT_IntelExit$'], [VT_INTEL])
AS_IF([ test "x$have_instrumentation_[]_AC_LANG_ABBREV[]_api_vt_intel" = xyes],
    [AFS_AM_CONDITIONAL([SCOREP_COMPILER_INSTRUMENTATION_NEEDS_LIBVT], [test 1 -eq 1], [false])])
dnl
# XL compilers
# -qfunctrace provides filtering capabilities
# -qdebug=function_trace seems to work as well, but isn't documented
_CHECK_COMPILER_INSTRUMENTATION_FLAG([-qfunctrace],               ['__func_trace_enter$'], ['__func_trace_exit$'], [FUNC_TRACE])
_CHECK_COMPILER_INSTRUMENTATION_FLAG([-g -finstrument-functions], ['__func_trace_enter$'], ['__func_trace_exit$'], [FUNC_TRACE])
dnl
# Cray compilers
AS_IF([test "x${scorep_have_addr2line}" = xyes],
    [_CHECK_COMPILER_INSTRUMENTATION_FLAG([-hfunc_trace], ['__pat_tp_func_entry$'], ['__pat_tp_func_return$'], [PAT_TP_FUNC])])
dnl
dnl (old) PGI compilers:
dnl With pgcc 17.9, -Mprof=func isn't supported but
dnl -Minstrument=functions is recommended instead.
dnl Newer PGI/NVIDIA compilers use the CYP_PROFILE_FUNC API when passed
dnl -Minstrument=functions.
dnl The instrumented API depends on the architecture (___instent64
dnl vs. ___instentavx)
dnl _CHECK_COMPILER_INSTRUMENTATION_FLAG([-Minstrument=functions], ['___instent'], ['___instret'], [PGIINST])
dnl
AS_IF([   test "x${have_instrumentation_[]_AC_LANG_ABBREV[]_api_cyg_profile_func}" = xyes \
       || test "x${have_instrumentation_[]_AC_LANG_ABBREV[]_api_pat_tp_func}" = xyes ],
    [scorep_compiler_instrumentation_needs_addr2line=yes])
dnl
AC_SUBST(SCOREP_COMPILER_INSTRUMENTATION_[]_AC_LANG_PREFIX[]FLAGS,
    ["$instrumentation_[]_AC_LANG_ABBREV[]flags"])
dnl
])dnl _CHECK_COMPILER_INSTRUMENTATION_FLAGS


# _CHECK_COMPILER_INSTRUMENTATION_FLAG()
# --------------------------------------
# Check instrumentation flag $1 for undefined symbols $2 and $3 which
# belong to API $4.
#
m4_define([_CHECK_COMPILER_INSTRUMENTATION_FLAG], [
AS_IF([test "x${instrumentation_[]_AC_LANG_ABBREV[]_api}" = x], [
    flag_accepted=no
    _AC_LANG_PREFIX[]FLAGS_save="$_AC_LANG_PREFIX[]FLAGS"
    _AC_LANG_PREFIX[]FLAGS="$_AC_LANG_PREFIX[]FLAGS $1"
    AC_COMPILE_IFELSE([AC_LANG_PROGRAM()],
        [$NM -u conftest.$OBJEXT > conftest.nm
         AS_IF([GREP_OPTIONS= ${GREP} -q -E $2 conftest.nm && GREP_OPTIONS= ${GREP} -q -E $3 conftest.nm],
             [flag_accepted=yes])
         rm -f conftest.nm])
    _AC_LANG_PREFIX[]FLAGS="$_AC_LANG_PREFIX[]FLAGS_save"
    AC_MSG_CHECKING([whether _AC_LANG compiler accepts $1])
    AC_MSG_RESULT([$flag_accepted])
    AS_IF([test "x${flag_accepted}" = xyes],
        [AC_SCOREP_DEFINE_HAVE([SCOREP_COMPILER_INSTRUMENTATION_]$4, [1], [Defined if $4 compiler instrumentation API is supported])
         AC_SCOREP_DEFINE_HAVE([SCOREP_COMPILER_INSTRUMENTATION_]_AC_CC[_]$4, [1], [Defined if $4 compiler instrumentation API is supported for language ] _AC_LANG)
         instrumentation_[]_AC_LANG_ABBREV[]flags="$1"
         AS_IF([test "x$have_instrumentation_[]_AC_LANG_ABBREV[]_api_[]m4_tolower($4)" != xyes],
             [scorep_have_compiler_instrumentation=yes
              have_instrumentation_[]_AC_LANG_ABBREV[]_api_[]m4_tolower($4)=yes
              instrumentation_[]_AC_LANG_ABBREV[]_api="m4_tolower($4)"])])
    AS_UNSET([flag_accepted])
])
])dnl _CHECK_COMPILER_INSTRUMENTATION_FLAG
