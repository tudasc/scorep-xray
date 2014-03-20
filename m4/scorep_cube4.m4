## -*- mode: autoconf -*-

## 
## This file is part of the Score-P software (http://www.score-p.org)
##
## Copyright (c) 2009-2011, 
##    RWTH Aachen University, Germany
##    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
##    Technische Universitaet Dresden, Germany
##    University of Oregon, Eugene, USA
##    Forschungszentrum Juelich GmbH, Germany
##    German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
##    Technische Universitaet Muenchen, Germany
##
## See the COPYING file in the package base directory for details.
##


AC_DEFUN([AC_SCOREP_CUBE_WRITER], [
# provide a link check here
])

dnl ----------------------------------------------------------------------------

AC_DEFUN([AC_SCOREP_CUBE_READER], [

m4_ifdef([AC_SCOREP_SCORE], 
         [_AC_SCOREP_CUBE_READER_HEADER_AND_LIB
          AM_CONDITIONAL([HAVE_SCORE], [test 1 -eq 1])],
         [AS_IF([test -n "${scorep_cube_bindir}"],
                [CUBE_READER_CXX="`${scorep_cube_bindir}/cube-config --reader-cxx`"
                 AS_IF([test "x${CXX}" = "x${CUBE_READER_CXX}"], 
                       [_AC_SCOREP_CUBE_READER_HEADER_AND_LIB],
                       [# unfortunatly ../build-score is not a valid path for 
                        # AC_CONFIG_SUBDIR_CUSTOM so we need to trigger the score
                        # configure from the toplevel configure
                        AM_CONDITIONAL([HAVE_SCORE], [test 1 -eq 0])])],

                [AM_CONDITIONAL([HAVE_SCORE], [test 1 -eq 0])])])

AC_SUBST([CUBE_READER_CPPFLAGS])
AC_SUBST([CUBE_READER_LIBS])
AC_SUBST([CUBE_READER_LDFLAGS])
AC_SUBST([CUBE_READER_CXX])

])

dnl ----------------------------------------------------------------------------

AC_DEFUN([_AC_SCOREP_CUBE_READER_HEADER_AND_LIB], [

CUBE_READER_CPPFLAGS="`${scorep_cube_bindir}/cube-config --reader-cppflags`"
CUBE_READER_LIBS="`${scorep_cube_bindir}/cube-config --reader-libs`"
CUBE_READER_LDFLAGS="`${scorep_cube_bindir}/cube-config --reader-ldflags`"

AC_LANG_PUSH([C++])

## Check presence of cube reader header
scorep_save_cppflags="${CPPFLAGS}"
CPPFLAGS="${CUBE_READER_CPPFLAGS} ${CPPFLAGS}"
AC_CHECK_HEADER([Cube.h], 
                [has_cube_reader_header="yes"], 
                [has_cube_reader_header="no"])

## Check presence of cube reader library
scorep_save_libs="${LIBS}"
LIBS="${LIBS} ${CUBE_READER_LIBS}"
scorep_save_ldflags="${LDFLAGS}"
LDFLAGS="${CUBE_READER_LDFLAGS} ${LDFLAGS}"
AC_LINK_IFELSE([AC_LANG_PROGRAM([[#include <Cube.h>]],
                                [[cube::Cube mycube;]])],
               [has_cube_reader_lib="yes"],
               [has_cube_reader_lib="no"])

AC_MSG_CHECKING([for cube reader library])
AS_IF([test "x${has_cube_reader_header}" = "xyes" && test "x${has_cube_reader_lib}" = "xyes"],
      [AC_MSG_RESULT([yes])
       AFS_SUMMARY([cube reader support], [yes, using ${CUBE_READER_CPPFLAGS} ${CUBE_READER_LDFLAGS} ${CUBE_READER_LIBS}])
       AM_CONDITIONAL([HAVE_SCORE], [test 1 -eq 1])],
      [AC_MSG_RESULT([no])
       AFS_SUMMARY([cube reader support], [no])
       AM_CONDITIONAL([HAVE_SCORE], [test 1 -eq 0])])

## Clean up
LIBS="${scorep_save_libs}"
LDFLAGS="${scorep_save_ldflags}"
CPPFLAGS="${scorep_save_cppflags}"
AC_LANG_POP([C++])
])


