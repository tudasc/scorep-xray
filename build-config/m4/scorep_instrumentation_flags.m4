dnl -*- mode: autoconf -*-

dnl
dnl This file is part of the Score-P software (http://www.score-p.org)
dnl
dnl Copyright (c) 2013, 2015,
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
    CFLAGS="$CFLAGS $1"

    AC_MSG_CHECKING([whether compiler understands $1])
    AC_COMPILE_IFELSE([AC_LANG_PROGRAM()],
        [AC_MSG_RESULT([yes])
         scorep_compiler_instrumentation_cppflags="$1"],
        [AC_MSG_RESULT([no])])

   CFLAGS="$save_CFLAGS"
   AC_LANG_POP([C])
])
    

AC_DEFUN([SCOREP_COMPILER_INSTRUMENTATION_FLAGS],[
AC_REQUIRE([AX_COMPILER_VENDOR])dnl

dnl Is there a use case for extra-instrumentation-flags?
AC_ARG_WITH([extra-instrumentation-flags],
            [AS_HELP_STRING([--with-extra-instrumentation-flags=flags],
                            [Add additional instrumentation flags.])],
            [scorep_with_extra_instrumentation_cppflags=$withval],
            [scorep_with_extra_instrumentation_cppflags=""])dnl

AC_SCOREP_COND_HAVE([GCC_PLUGIN_SUPPORT],
                    [test -f ../build-gcc-plugin/gcc_plugin_supported],
                    [Defined if GCC plug-in support is available.],
                    [scorep_compiler_gnu_with_plugin=yes],
                    [scorep_compiler_gnu_with_plugin=no])

scorep_compiler_instrumentation_needs_symbol_table="no"
AS_CASE([${ax_cv_c_compiler_vendor}],
    [intel],    [scorep_compiler_instrumentation_cppflags="-tcollect"],
    [sun],      [scorep_compiler_instrumentation_cppflags="-O -Qoption f90comp -phat"],
    [ibm],      [SCOREP_CC_FLAG_TEST([-qdebug=function_trace])
                 SCOREP_CC_FLAG_TEST([-qfunctrace])],
    [portland/llvm], [AFS_AM_CONDITIONAL([SCOREP_COMPILER_PGI_LLVM], [test 1 -eq 1], [false])
                      scorep_compiler_instrumentation_cppflags="-Minstrument=functions"
                      scorep_compiler_instrumentation_needs_symbol_table="yes"],
    [portland], [SCOREP_CC_FLAG_TEST([-Mprof=func])
                 SCOREP_CC_FLAG_TEST([-Minstrument=functions])],
    [gnu],      [AS_IF([test "x${scorep_compiler_gnu_with_plugin}" = "xyes"],
                       [scorep_compiler_instrumentation_cppflags=""],
                       [scorep_compiler_instrumentation_cppflags="-g -finstrument-functions"
                        scorep_compiler_instrumentation_needs_symbol_table="yes"])],
    [cray],     [scorep_compiler_instrumentation_cppflags="-hfunc_trace"
                 scorep_compiler_instrumentation_ldflags="-Wl,-u,__pat_tp_func_entry,-u,__pat_tp_func_return"
                 scorep_compiler_instrumentation_needs_symbol_table=yes],
    [fujitsu],  [scorep_compiler_instrumentation_cppflags="-g -Ntl_vtrc -Ntl_notrt"
                 scorep_compiler_instrumentation_needs_symbol_table="yes"],
    [clang],    [SCOREP_CC_FLAG_TEST([-g -finstrument-functions])
                 SCOREP_CC_FLAG_TEST([-g -finstrument-functions-after-inlining])
                 scorep_compiler_instrumentation_needs_symbol_table="yes"
                 AS_CASE([${ac_scorep_platform}],
                         [mac*], [# Disable position independent executable, which
                                  # also disables address space randomization,
                                  # which avoids matching addresses between NM and
                                  # __cyg_profile_func_*
                                  scorep_compiler_instrumentation_ldflags="-Wl,-no_pie"])],
    [])dnl

AS_IF([test "x${scorep_with_extra_instrumentation_cppflags}" != x || \
       test "x${scorep_compiler_instrumentation_cppflags}" != x],
    [AC_MSG_NOTICE([using compiler instrumentation cppflags: ${scorep_compiler_instrumentation_cppflags} ${scorep_with_extra_instrumentation_cppflags}])
     AFS_SUMMARY_VERBOSE([compiler instrumentation cppflags], [${scorep_compiler_instrumentation_cppflags} ${scorep_with_extra_instrumentation_cppflags}])
     AS_IF([test "x${scorep_compiler_instrumentation_ldflags}" != x],
         [AC_MSG_NOTICE([using compiler instrumentation ldflags: ${scorep_compiler_instrumentation_ldflags}])
          AFS_SUMMARY_VERBOSE([compiler instrumentation ldflags], [${scorep_compiler_instrumentation_ldflags}])
         ])
    ])

AC_SUBST([SCOREP_COMPILER_INSTRUMENTATION_CPPFLAGS],
    ["${scorep_compiler_instrumentation_cppflags} ${scorep_with_extra_instrumentation_cppflags}"])
AC_SUBST([SCOREP_COMPILER_INSTRUMENTATION_LDFLAGS], ["${scorep_compiler_instrumentation_ldflags}"])
])

dnl ------------------------------------------------------------------


# Flags that are needed by all adapters, e.g., you would always want
# to add -Ntl_notrt on K, independent on the instrumentation method
# used. Different from compiler instrumentation flags.
AC_DEFUN([SCOREP_INSTRUMENTATION_FLAGS], [
AC_REQUIRE([AX_COMPILER_VENDOR])dnl

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
                             scorep_instrumentation_ldflags="-dynamic"])],
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
