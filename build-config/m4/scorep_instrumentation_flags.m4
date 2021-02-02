dnl -*- mode: autoconf -*-

dnl
dnl This file is part of the Score-P software (http://www.score-p.org)
dnl
dnl Copyright (c) 2013, 2015, 2020-2021,
dnl Forschungszentrum Juelich GmbH, Germany
dnl
dnl Copyright (c) 2013-2015, 2019,
dnl Technische Universitaet Dresden, Germany
dnl
dnl Copyright (c) 2016,
dnl Technische Universitaet Darmstadt, Germany
dnl
dnl This software may be modified and distributed under the terms of
dnl a BSD-style license.  See the COPYING file in the package base
dnl directory for details.
dnl

dnl file build-config/m4/scorep_instrumentation_flags.m4

AC_DEFUN([SCOREP_CC_FLAG_TEST],[
    AC_LANG_PUSH([C])
    save_CFLAGS=$CFLAGS
    CFLAGS="$CFLAGS $2"

    AC_MSG_CHECKING([whether compiler understands $2])
    AC_COMPILE_IFELSE([AC_LANG_PROGRAM()],
        [AC_MSG_RESULT([yes])
         $1="$2"],
        [AC_MSG_RESULT([no])])

   CFLAGS="$save_CFLAGS"
   AC_LANG_POP([C])
])


AC_DEFUN([SCOREP_COMPILER_INSTRUMENTATION_FLAGS],[
AC_REQUIRE([SCOREP_COMPUTENODE_CC])dnl
dnl SCOREP_COMPUTENODE_FC cannot be AC_REQUIREd due to Fortan macro
dnl dependencies that lead to configure errors if FC is defunct.
dnl AC_REQUIRE([SCOREP_COMPUTENODE_FC])dnl
AC_ARG_WITH([extra-instrumentation-flags],
    [],
    [AC_MSG_WARN([ignoring --with-extra-instrumentation-flags, functionality got removed])],
    [])dnl

AC_SCOREP_COND_HAVE([GCC_PLUGIN_SUPPORT],
                    [test -f ../build-gcc-plugin/gcc_plugin_supported],
                    [Defined if GCC plug-in support is available.],
                    [scorep_compiler_gnu_with_plugin=yes],
                    [scorep_compiler_gnu_with_plugin=no])

scorep_compiler_instrumentation_needs_symbol_table="no"
scorep_compiler_instrumentation_cflags=
scorep_compiler_instrumentation_cxxflags=
scorep_compiler_instrumentation_fflags=
AS_CASE([${ax_cv_c_compiler_vendor}],
    [intel],    [scorep_compiler_instrumentation_cflags="-tcollect"],
    [sun],      [scorep_compiler_instrumentation_cflags="-O -Qoption f90comp -phat"],
    [ibm],      [SCOREP_CC_FLAG_TEST([scorep_compiler_instrumentation_cflags], [-qdebug=function_trace])
                 SCOREP_CC_FLAG_TEST([scorep_compiler_instrumentation_cflags], [-qfunctrace])],
    [portland/llvm], [AFS_AM_CONDITIONAL([SCOREP_COMPILER_PGI_LLVM], [test 1 -eq 1], [false])
                      scorep_compiler_instrumentation_cflags="-Minstrument=functions"
                      scorep_compiler_instrumentation_needs_symbol_table="yes"],
    [portland], [SCOREP_CC_FLAG_TEST([scorep_compiler_instrumentation_cflags], [-Mprof=func])
                 SCOREP_CC_FLAG_TEST([scorep_compiler_instrumentation_cflags], [-Minstrument=functions])],
    [gnu],      [AS_IF([test "x${scorep_compiler_gnu_with_plugin}" = "xno"],
                       [scorep_compiler_instrumentation_cflags="-g -finstrument-functions"
                        scorep_compiler_instrumentation_needs_symbol_table="yes"])],
    [cray],     [scorep_compiler_instrumentation_cflags="-hfunc_trace"
                 scorep_compiler_instrumentation_ldflags="-Wl,-u,__pat_tp_func_entry,-u,__pat_tp_func_return"
                 scorep_compiler_instrumentation_needs_symbol_table=yes],
    [fujitsu],  [scorep_compiler_instrumentation_cflags="-g -Ntl_vtrc -Ntl_notrt"
                 scorep_compiler_instrumentation_needs_symbol_table="yes"],
    [clang],    [SCOREP_CC_FLAG_TEST([scorep_compiler_instrumentation_cflags], [-g -finstrument-functions])
                 SCOREP_CC_FLAG_TEST([scorep_compiler_instrumentation_cflags], [-g -finstrument-functions-after-inlining])
                 scorep_compiler_instrumentation_needs_symbol_table="yes"
                 AS_CASE([${ac_scorep_platform}],
                         [mac*], [# Disable position independent executable, which
                                  # also disables address space randomization,
                                  # which avoids matching addresses between NM and
                                  # __cyg_profile_func_*
                                  scorep_compiler_instrumentation_ldflags="-Wl,-no_pie"])],
    [])dnl

AS_IF([test "x${ax_cv_c_compiler_vendor}" != "x${ax_cv_fc_compiler_vendor}"],
    [AS_CASE([${ax_cv_fc_compiler_vendor}],
         [cray], [scorep_compiler_instrumentation_fflags="-hfunc_trace"
                  scorep_compiler_instrumentation_ldflags="$scorep_compiler_instrumentation_ldflags -Wl,-u,__pat_tp_func_entry,-u,__pat_tp_func_return"],
         [scorep_compiler_instrumentation_fflags=unsupported])dnl
    ])

AS_IF([test "x${scorep_compiler_instrumentation_cflags}" != x],
    [# Use cflags also for C++ and Fortran if not already set
     AS_IF([test "x${scorep_compiler_instrumentation_cxxflags}" = x],
         [scorep_compiler_instrumentation_cxxflags="${scorep_compiler_instrumentation_cflags}"])
     AS_IF([test "x${scorep_compiler_instrumentation_fflags}" = x],
         [scorep_compiler_instrumentation_fflags="${scorep_compiler_instrumentation_cflags}"])

     AC_MSG_NOTICE([using C compiler instrumentation flags: ${scorep_compiler_instrumentation_cflags}])
     AFS_SUMMARY_VERBOSE([C compiler instrumentation flags], [${scorep_compiler_instrumentation_cflags}])
])

AS_IF([test "x${scorep_compiler_instrumentation_cxxflags}" != x],
    [AC_MSG_NOTICE([using C++ compiler instrumentation flags: ${scorep_compiler_instrumentation_cxxflags}])
     AFS_SUMMARY_VERBOSE([C++ compiler instrumentation flags], [${scorep_compiler_instrumentation_cxxflags}])
])

AS_IF([test "x${scorep_compiler_instrumentation_fflags}" = xunsupported],
    [AC_MSG_WARN([Fortran compiler instrumentation not supported yet. Please contact <AC_PACKAGE_BUGREPORT>.])
     AFS_SUMMARY_VERBOSE([Fortran compiler instrumentation not supported yet])],
    [test "x${scorep_compiler_instrumentation_fflags}" != x],
    [AC_MSG_NOTICE([using Fortran compiler instrumentation flags: ${scorep_compiler_instrumentation_fflags}])
     AFS_SUMMARY_VERBOSE([Fortran compiler instrumentation flags], [${scorep_compiler_instrumentation_fflags}])
])

AS_IF([test "x${scorep_compiler_instrumentation_ldflags}" != x],
    [AC_MSG_NOTICE([using compiler instrumentation ldflags: ${scorep_compiler_instrumentation_ldflags}])
     AFS_SUMMARY_VERBOSE([compiler instrumentation ldflags], [${scorep_compiler_instrumentation_ldflags}])
])

AC_SUBST([SCOREP_COMPILER_INSTRUMENTATION_CFLAGS], ["${scorep_compiler_instrumentation_cflags}"])
AC_SUBST([SCOREP_COMPILER_INSTRUMENTATION_CXXFLAGS], ["${scorep_compiler_instrumentation_cxxflags}"])
AC_SUBST([SCOREP_COMPILER_INSTRUMENTATION_FFLAGS], ["${scorep_compiler_instrumentation_fflags}"])

AC_SUBST([SCOREP_COMPILER_INSTRUMENTATION_LDFLAGS], ["${scorep_compiler_instrumentation_ldflags}"])
])

dnl ------------------------------------------------------------------


# Flags that are needed by all adapters, e.g., you would always want
# to add -Ntl_notrt on K, independent on the instrumentation method
# used. Different from compiler instrumentation flags.
AC_DEFUN([SCOREP_INSTRUMENTATION_FLAGS], [
#AC_REQUIRE([AX_COMPILER_VENDOR])dnl

AS_UNSET([scorep_instrumentation_cppflags])
AS_UNSET([scorep_instrumentation_ldflags])
AS_CASE([${ax_cv_c_compiler_vendor%/*}],
    [intel],    [],
    [sun],      [],
    [ibm],      [],
    [portland], [],
    [gnu],      [AS_CASE([${ac_scorep_platform}],
                     [bg*], [# Link shared variant of (system) libs, if available.
                             # This reduces the nm time and brings the number of
                             # symbols into a reasonable range.
                             scorep_instrumentation_ldflags="-dynamic"])
                 SCOREP_CC_FLAG_TEST([scorep_instrumentation_cppflags], [-fno-ipa-icf])],
    [cray],     [],
    [fujitsu],  [scorep_instrumentation_ldflags="-Ntl_notrt"],
    [clang],    [],
    [])dnl

AS_IF([test "x${scorep_instrumentation_cppflags}" != x],
    [AC_MSG_NOTICE([using instrumentation cppflags: ${scorep_instrumentation_cppflags}])
     AFS_SUMMARY_VERBOSE([instrumentation cppflags], [${scorep_instrumentation_cppflags}])])

AS_IF([test "x${scorep_instrumentation_ldflags}" != x],
    [AC_MSG_NOTICE([using instrumentation ldflags: ${scorep_instrumentation_ldflags}])
     AFS_SUMMARY_VERBOSE([instrumentation ldflags], [${scorep_instrumentation_ldflags}])])

AC_SUBST([SCOREP_INSTRUMENTATION_CPPFLAGS], ["${scorep_instrumentation_cppflags}"])
AC_SUBST([SCOREP_INSTRUMENTATION_LDFLAGS], ["${scorep_instrumentation_ldflags}"])
])
