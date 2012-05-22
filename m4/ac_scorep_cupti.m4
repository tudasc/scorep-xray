## -*- mode: autoconf -*-

## 
## This file is part of the Score-P software (http://www.score-p.org)
##
## Copyright (c) 2009-2012, 
##    RWTH Aachen, Germany
##    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
##    Technische Universitaet Dresden, Germany
##    University of Oregon, Eugene, USA
##    Forschungszentrum Juelich GmbH, Germany
##    German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
##    Technische Universitaet Muenchen, Germany
##
## See the COPYING file in the package base directory for details.
##

## file       ac_scorep_cupti.m4
## maintainer Robert Dietrich <robert.dietrich@zih.tu-dresden.de>


AC_DEFUN([AC_SCOREP_CUPTI], [

dnl Don't check for CUPTI on the frontend.
AS_IF([test "x$ac_scorep_backend" = xno], 
      [AC_MSG_ERROR([cannot check for CUPTI on frontend.])])

scorep_cupti_error="no"
scorep_have_cupti="no"

AC_REQUIRE([AC_SCOREP_CUDA])

AC_ARG_WITH([cupti],
            [AS_HELP_STRING([--with-cupti=<path-to-cupti-directory>], 
                            [If CUPTI is not installed in the default location (<path-to-cuda-directory>/extras/CUPTI), specify the directory where it can be found.])],
            [scorep_cupti_dir="${withval}"], # action-if-given
            [
                 AS_IF([test x"$with_cuda" != x -a x"$with_cuda" != xyes -a x"$with_cuda" != xno], 
                       [scorep_cupti_dir="$with_cuda"/extras/CUPTI])
            ] # action-if-not-given
)

AC_ARG_WITH([cupti-include],
            [AS_HELP_STRING([--with-cupti-include=<path-to-cupti-include-directory>], 
                            [If cupti.h is not installed in the default location (<path-to-cupti-directory>/include), specify the directory name where it can be found.])],
            [scorep_cupti_inc_dir="${withval}"],  # action-if-given
            [
                AS_IF([test x"$scorep_cupti_dir" != x], 
                      [scorep_cupti_inc_dir="$scorep_cupti_dir"/include])
            ] # action-if-not-given
)

AC_ARG_WITH([cupti-lib],
            [AS_HELP_STRING([--with-cupti-lib=<path-to-libcupti.*>], 
                            [If libcupti.* is not installed in the default location (<path-to-cupti-directory>/lib64), specify the directory name where it can be found.])],
            [scorep_cupti_lib_dir="${withval}"],  # action-if-given
            [
                AS_IF([test x"$scorep_cupti_dir" != x], 
                      [scorep_cupti_lib_dir="$scorep_cupti_dir"/lib64])
            ] # action-if-not-given
)

AS_IF([test x"$SCOREP_CUDA_ENABLE" = x"yes"],
[
dnl use C as configure check language
  AC_LANG_PUSH([C])

dnl save CPPFLAGS and LIBS and set path, if available
  cppflags_save="$CPPFLAGS"
  AS_IF([test x"$scorep_cupti_inc_dir" != x], 
        [CPPFLAGS="-I$scorep_cupti_inc_dir $CPPFLAGS"])
  AS_IF([test x"$SCOREP_CUDA_INCDIR" != x], 
        [CPPFLAGS="-I$SCOREP_CUDA_INCDIR $CPPFLAGS"])
  libs_save="$LIBS"
  AS_IF([test x"$scorep_cupti_lib_dir" != x], 
        [LIBS="-L$scorep_cupti_lib_dir $LIBS"])
  AS_IF([test x"$SCOREP_CUDA_LIBDIR" != x], 
        [LIBS="-L$SCOREP_CUDA_LIBDIR $LIBS"])
  LIBS="$LIBS $SCOREP_CUDA_LIBS"

dnl check the available CUPTI headers
  AC_CHECK_HEADER([cupti.h], [], [
    AC_MSG_NOTICE([error: no cupti.h found; check path to CUPTI headers ...])
    scorep_cupti_error="yes"
  ])

dnl checking for CUPTI library
  AS_IF([test x"$scorep_cupti_error" = "xno"], [
    scorep_cupti_lib_name="cupti"
    AC_CHECK_LIB([$scorep_cupti_lib_name], [cuptiGetVersion], [],
      [
        AC_MSG_NOTICE([error: no libcupti found; check path to CUPTI library ...])
        scorep_cupti_error="yes"
      ]
    )
  ])

dnl check the version of CUPTI
  AS_IF([test x"$scorep_cupti_error" = "xno"], [
    AC_COMPILE_IFELSE([
      AC_LANG_PROGRAM(
        [[#include "cupti.h"]],
        [[
#ifndef CUPTI_API_VERSION
#  error "CUPTI_API_VERSION not defined"
#elif CUPTI_API_VERSION < 2
#  error "CUPTI_API_VERSION < 2"
#endif
        ]])],[],
        [
          AC_MSG_NOTICE([error: CUPTI version could not be determined and/or is 
                        incompatible (< 2)	See 'config.log' for more details.])
          scorep_cupti_error="yes"
    ])
  ])

dnl restore CPPFLAGS and LIBS
  LIBS="$libs_save"
  CPPFLAGS="$cppflags_save"
  AC_LANG_POP([C])

dnl final check for errors
  if test "x${scorep_cupti_error}" = "xno"; then
    scorep_have_cupti="yes"
  fi

dnl generating output
  AC_MSG_CHECKING([for CUPTI support])
  AC_MSG_RESULT([$scorep_have_cupti])

dnl generating results
  if test "x${scorep_have_cupti}" = "xyes"; then
      AC_DEFINE([HAVE_CUPTI], [1],     [Defined if CUPTI is available.])
      AS_IF([test x"$scorep_cuda_lib_dir" != x],
      [
          AC_SUBST([SCOREP_CUPTI_LDFLAGS], [-L${scorep_cupti_lib_dir}])
      ],[
          AC_SUBST([SCOREP_CUPTI_LDFLAGS], [""])
      ])
      AC_SUBST([SCOREP_CUPTI_LIBS],    [-l${scorep_cupti_lib_name}])
  else
      AC_SUBST([SCOREP_CUPTI_LDFLAGS], [""])
      AC_SUBST([SCOREP_CUPTI_LIBS],    [""])
  fi

  AC_SUBST([SCOREP_CUPTI_CPPFLAGS],[$scorep_cupti_cppflags])
  AC_SUBST([SCOREP_CUPTI_LIBDIR],  [$scorep_cupti_lib_dir])
  AC_SUBST([SCOREP_CUPTI_INCDIR],  [$scorep_cupti_inc_dir])

dnl generating summary
  AS_IF([test "x${scorep_have_cupti}" = "xyes"], [
      AC_SCOREP_SUMMARY([CUDA support],   [yes, using CUPTI])
      AC_SCOREP_SUMMARY_VERBOSE([CUPTI root directory], [$scorep_cupti_dir])
      AC_SCOREP_SUMMARY_VERBOSE([CUPTI include directory], [$scorep_cupti_inc_dir])
      AC_SCOREP_SUMMARY_VERBOSE([CUPTI library directory], [$scorep_cupti_lib_dir])
  ], [
      AC_SCOREP_SUMMARY([CUDA support],   [no])
  ])
], [
  AS_IF([test "x${with_cuda}" = "xno"], [
    AC_SCOREP_SUMMARY([CUDA support],[disabled])
  ],[
    AC_SCOREP_SUMMARY([CUDA support],[not found, specify directory])
  ])
])
AM_CONDITIONAL([HAVE_CUPTI],     [test "x${scorep_have_cupti}" = "xyes"])
])
