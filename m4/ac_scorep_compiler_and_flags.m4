## -*- mode: autoconf -*-

## 
## This file is part of the SCOREP project (http://www.scorep.de)
##
## Copyright (c) 2009-2011, 
##    RWTH Aachen, Germany
##    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
##    Technische Universitaet Dresden, Germany
##    University of Oregon, Eugene USA
##    Forschungszentrum Juelich GmbH, Germany
##    Technische Universitaet Muenchen, Germany
##
## See the COPYING file in the package base directory for details.
##

## file       ac_scorep_compiler_and_flags.m4
## maintainer Christian Roessel <c.roessel@fz-juelich.de>

AC_DEFUN([AC_SCOREP_CONVERT_FLAGS],
[
if test "x${ac_cv_env_[$1]_FOR_BUILD_set}" != "xset"; then
   # don't use the backend flags if nothing is specified for the frontend
   unset [$1]
else
   # use the frontend flags 
   [$1]="$ac_cv_env_[$1]_FOR_BUILD_value"
fi
## ac_substituting done automagically 
##AC_SUBST([$1])
])


AC_DEFUN([AC_SCOREP_CHECK_COMPILER_VAR_SET],
[
if test "x${ac_cv_env_[$1]_set}" != "xset"; then
    AC_MSG_ERROR([argument $1 not provided in configure call.], [1])
fi
])


AC_DEFUN([AC_SCOREP_CONVERT_COMPILER],
[
if test "x${ac_cv_env_[$1]_FOR_BUILD_set}" != "xset"; then
    AC_MSG_ERROR([argument $1_FOR_BUILD not provided in configure call.], [1])    
fi
[$1]="$ac_cv_env_[$1]_FOR_BUILD_value"
])


AC_DEFUN([AC_SCOREP_WITH_COMPILER_SUITE],
[


ac_scorep_with_nocross_compiler_suite=""
AC_ARG_WITH([nocross-compiler-suite],
            [AS_HELP_STRING([--with-nocross-compiler-suite=(gcc|ibm|intel|pathscale|pgi|sun)], 
                            [The compiler suite to build this package in non cross-compiling environments with. Needs to be in $PATH [gcc].])],
            [AS_IF([test "x${ac_scorep_cross_compiling}" = "xno"], [ac_scorep_with_nocross_compiler_suite=$withval], 
                   [AC_MSG_ERROR([Option --with-nocross-compiler-suite not supported in cross-compiling mode. Please use --with-backend-compiler-suite and --with-frontend-compiler-suite instead.])])],
            [])


ac_scorep_with_backend_compiler_suite=""
AC_ARG_WITH([backend-compiler-suite],
            [AS_HELP_STRING([--with-backend-compiler-suite=(ibm|sx)], 
                            [The compiler suite to build the backend parts of this package in cross-compiling environments with. Needs to be in $PATH [gcc].])],
            [AS_IF([test "x${ac_scorep_cross_compiling}" = "xyes"], [ac_scorep_with_backend_compiler_suite=$withval], 
                   [AC_MSG_ERROR([Option --with-backend-compiler-suite not supported in non cross-compiling mode. Please use --with-nocross-compiler-suite instead.])])],
            [])


ac_scorep_with_frontend_compiler_suite=""
AC_ARG_WITH([frontend-compiler-suite],
            [AS_HELP_STRING([--with-frontend-compiler-suite=(gcc|ibm|intel|pathscale|pgi|sun)], 
                            [The compiler suite to build the frontend parts of this package in cross-compiling environments with. Needs to be in $PATH [gcc].])],
            [AS_IF([test "x${ac_scorep_cross_compiling}" = "xyes"], [ac_scorep_with_frontend_compiler_suite=$withval], 
                   [AC_MSG_ERROR([Option --with-frontend-compiler-suite not supported in non cross-compiling mode. Please use --with-nocross-compiler-suite instead.])])],
            [])


])
