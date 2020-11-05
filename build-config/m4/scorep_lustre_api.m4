## -*- mode: autoconf -*-

##
## This file is part of the Score-P software (http://www.score-p.org)
##
## Copyright (c) 2020,
## Technische Universitaet Dresden, Germany
##
## This software may be modified and distributed under the terms of
## a BSD-style license.  See the COPYING file in the package base
## directory for details.
##
##

AC_DEFUN([SCOREP_LUSTREAPI_CHECK], [

AFS_SUMMARY_PUSH

scorep_lustreapi_support="yes"
scorep_old_lustreapi_support="no"

AS_IF([test x"${scorep_lustreapi_support}" = x"yes"],
      [AC_SCOREP_BACKEND_LIB([liblustreapi], [lustre/lustreapi.h])
      AS_IF([test x"${scorep_have_liblustreapi}" != x"yes"],
                  [scorep_lustreapi_support="no"])
      ])

AC_SCOREP_COND_HAVE([LUSTRE_API_SUPPORT],
                    [test x"${scorep_lustreapi_support}" = x"yes" || test x"${scorep_old_lustreapi_support}" = x"yes"],
                    [Defined if recording calls to Lustre-API is possible.],
                    [AC_SUBST([SCOREP_LUSTRE_CPPFLAGS],["${with_liblustreapi_cppflags}"])
                     AC_SUBST([SCOREP_LUSTRE_LDFLAGS], ["${with_liblustreapi_ldflags} ${with_liblustreapi_rpathflag}"])
                     AC_SUBST([SCOREP_LUSTRE_LIBS],    ["${with_liblustreapi_libs}"])],
                    [AC_SUBST([SCOREP_LUSTRE_CPPFLAGS],[""])
                     AC_SUBST([SCOREP_LUSTRE_LDFLAGS], [""])
                     AC_SUBST([SCOREP_LUSTRE_LIBS],    [""])])

AFS_SUMMARY_POP([Lustre stripe info support], [${scorep_lustreapi_support}])

])

AC_DEFUN([_AC_SCOREP_LIBLUSTREAPI_LIB_CHECK], [
scorep_$1_error="no"
scorep_$1_name="lustreapi"

dnl checking for lustre API
AS_IF([test "x${scorep_$1_error}" = "xno"],
      [scorep_$1_save_libs="${LIBS}"
       AS_UNSET([ac_cv_search_llapi_file_get_stripe])
       AC_SEARCH_LIBS([llapi_file_get_stripe],
                      [$scorep_$1_name],
                      [],
                      [AS_IF([test "x${with_$1}" != xnot_set || test "x${with_$1_lib}" != xnot_set],
                             [AC_MSG_NOTICE([no $1 found; check path to lustre library ...])])
                       scorep_$1_error="yes" ])
       LIBS="${scorep_$1_save_libs}"])

dnl final check for errors
if test "x${scorep_$1_error}" = "xno"; then
    with_$1_lib_checks_successful="yes"
    with_$1_libs="-l${scorep_$1_name}"
else
    with_$1_lib_checks_successful="no"
    with_$1_libs=""
fi
])
