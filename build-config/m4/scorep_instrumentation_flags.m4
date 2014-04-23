dnl -*- mode: autoconf -*-

dnl
dnl This file is part of the Score-P software (http://www.score-p.org)
dnl
dnl Copyright (c) 2013
dnl Forschungszentrum Juelich GmbH, Germany
dnl
dnl This software may be modified and distributed under the terms of
dnl a BSD-style license.  See the COPYING file in the package base
dnl directory for details.
dnl

dnl file build-config/m4/scorep_instrumentation_flags.m4

AC_DEFUN([SCOREP_COMPILER_INSTRUMENTATION_FLAGS],[
AC_REQUIRE([AX_COMPILER_VENDOR])dnl

dnl Is there a use case for extra-instrumentation-flags?
AC_ARG_WITH([extra-instrumentation-flags],
            [AS_HELP_STRING([--with-extra-instrumentation-flags=flags],
                            [Add additional instrumentation flags.])],
            [scorep_with_extra_instrumentation_cppflags=$withval],
            [scorep_with_extra_instrumentation_cppflags=""])dnl

AS_CASE([${ax_cv_c_compiler_vendor}],
    [intel],    [scorep_compiler_instrumentation_cppflags="-tcollect"],
    [sun],      [scorep_compiler_instrumentation_cppflags="-O -Qoption f90comp -phat"],
    [ibm],      [scorep_compiler_instrumentation_cppflags="-qdebug=function_trace"],
    [portland], [scorep_compiler_instrumentation_cppflags="-Mprof=func"],
    [gnu],      [scorep_compiler_instrumentation_cppflags="-g -finstrument-functions"],
    [cray],     [scorep_compiler_instrumentation_cppflags="-g -hfunc_trace"
                 scorep_compiler_instrumentation_ldflags="-Wl,-u,__pat_tp_func_entry,-u,__pat_tp_func_return"],
    [fujitsu],  [scorep_compiler_instrumentation_cppflags="-g -Ntl_vtrc -Ntl_notrt"],
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
AS_CASE([${ax_cv_c_compiler_vendor}],
    [intel],    [],
    [sun],      [],
    [ibm],      [],
    [portland], [],
    [gnu],      [],
    [cray],     [],
    [fujitsu],  [scorep_instrumentation_ldflags="-Ntl_notrt"],
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
