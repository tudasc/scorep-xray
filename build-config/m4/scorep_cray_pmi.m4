## -*- mode: autoconf -*-

##
## This file is part of the Score-P software (http://www.score-p.org)
##
## Copyright (c) 2013, 2015,
## Technische Universitaet Dresden, Germany
##
## Copyright (c) 2015,
## Forschungszentrum Juelich GmbH, Germany
##
## This software may be modified and distributed under the terms of
## a BSD-style license.  See the COPYING file in the package base
## directory for details.
##
##

## file       scorep_cray_pmi.m4
## maintainer Ronny Tschueter <ronny.tschueter@tu-dresden.de>

AC_DEFUN([SCOREP_CRAY_PMI], [

scorep_have_pmi="no"

# On Cray system we want to use PMI to get fine granular information about system topology
# PMI needs librca, the Cray compiler will add necessary flags automatically,
# for other compilers we have to provide path and library name
AS_CASE([${ac_scorep_platform}],
    [crayx*], [AFS_SUMMARY_PUSH
              AC_SCOREP_BACKEND_LIB([libpmi], [pmi.h])
              AC_SCOREP_BACKEND_LIB([librca])],
    [scorep_have_libpmi=no
    AM_CONDITIONAL(HAVE_LIBPMI, [test 1 -eq 0])
    AC_SUBST(LIBPMI_CPPFLAGS, [""])
    AC_SUBST(LIBPMI_LDFLAGS,  [""])
    AC_SUBST(LIBPMI_LIBS,     [""])
    scorep_have_librca=no
    AM_CONDITIONAL(HAVE_LIBRCA, [test 1 -eq 0])
    AC_SUBST(LIBRCA_CPPFLAGS, [""])
    AC_SUBST(LIBRCA_LDFLAGS,  [""])
    AC_SUBST(LIBRCA_LIBS,     [""])])

AC_SCOREP_COND_HAVE([PMI],
                    [test "x${scorep_have_libpmi}" = "xyes" && test "x${scorep_have_librca}" = "xyes"],
                    [Defined if PMI is available.],
                    [scorep_have_pmi="yes"
                     AC_SUBST(PMI_CPPFLAGS, ["${with_libpmi_cppflags}"])
                     AC_SUBST(PMI_LDFLAGS,  ["${with_libpmi_ldflags} ${with_librca_ldflags} ${with_libpmi_rpathflag} ${with_librca_rpathflag}"])
                     AC_SUBST(PMI_LIBS,     ["${with_libpmi_libs} ${with_librca_libs}"])],
                    [AC_SUBST(PMI_CPPFLAGS, [""])
                     AC_SUBST(PMI_LDFLAGS,  [""])
                     AC_SUBST(PMI_LIBS,     [""])])

AS_CASE([${ac_scorep_platform}],
    [crayx*], [AFS_SUMMARY_POP([Cray PMI support], [${scorep_have_pmi}])])
])


dnl ----------------------------------------------------------------------------


AC_DEFUN([_AC_SCOREP_LIBPMI_LIB_CHECK], [
scorep_pmi_error="no"
scorep_pmi_lib_name="pmi"

dnl checking for PMI library
AS_IF([test "x$scorep_pmi_error" = "xno"],
      [scorep_pmi_save_libs="${LIBS}"
       AC_SEARCH_LIBS([PMI_Initialized],
                      [$scorep_pmi_lib_name],
                      [],
                      [AS_IF([test "x${with_libpmi}" != xnot_set || test "x${with_libpmi_lib}" != xnot_set],
                             [AC_MSG_NOTICE([no libpmi found; check path to PMI library ...])])
                       scorep_pmi_error="yes" ])
       LIBS="${scorep_pmi_save_libs}"])

dnl final check for errors
if test "x${scorep_pmi_error}" = "xno"; then
    with_$1_lib_checks_successful="yes"
    with_$1_libs="-l${scorep_pmi_lib_name}"
else
    with_$1_lib_checks_successful="no"
    with_$1_libs=""
fi
])


dnl ----------------------------------------------------------------------------


AC_DEFUN([_AC_SCOREP_LIBRCA_LIB_CHECK], [
scorep_rca_error="no"
scorep_rca_lib_name="rca"

dnl checking for RCA library
AS_IF([test "x$scorep_rca_error" = "xno"],
      [scorep_rca_save_libs="${LIBS}"
       AC_SEARCH_LIBS([rca_nicaddr_to_nid],
                      [$scorep_rca_lib_name],
                      [],
                      [AS_IF([test "x${with_librca}" != xnot_set || test "x${with_librca_lib}" != xnot_set],
                             [AC_MSG_NOTICE([no librca found; check path to RCA library ...])])
                       scorep_rca_error="yes" ])
       LIBS="${scorep_rca_save_libs}"])

dnl final check for errors
if test "x${scorep_rca_error}" = "xno"; then
    with_$1_lib_checks_successful="yes"
    with_$1_libs="-l${scorep_rca_lib_name}"
else
    with_$1_lib_checks_successful="no"
    with_$1_libs=""
fi
])
