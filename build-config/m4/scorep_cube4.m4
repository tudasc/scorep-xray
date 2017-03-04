## -*- mode: autoconf -*-

##
## This file is part of the Score-P software (http://www.score-p.org)
##
## Copyright (c) 2009-2011,
## RWTH Aachen University, Germany
##
## Copyright (c) 2009-2011,
## Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
##
## Copyright (c) 2009-2011,
## Technische Universitaet Dresden, Germany
##
## Copyright (c) 2009-2011,
## University of Oregon, Eugene, USA
##
## Copyright (c) 2009-2011, 2015-2017,
## Forschungszentrum Juelich GmbH, Germany
##
## Copyright (c) 2009-2011,
## German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
##
## Copyright (c) 2009-2011,
## Technische Universitaet Muenchen, Germany
##
## This software may be modified and distributed under the terms of
## a BSD-style license.  See the COPYING file in the package base
## directory for details.
##

## file build-config/m4/scorep_cube4.m4


AC_DEFUN([AC_SCOREP_CUBEW], [
# provide a link check here
])

dnl ----------------------------------------------------------------------------

AC_DEFUN([SCOREP_CUBELIB], [
m4_ifndef([SCOREP_SCORE],
    [m4_fatal([Invoke SCOREP_CUBELIB from build-score only.])])

AS_IF([test -n "${scorep_cubelib_bindir}"],
    [AC_SUBST([CUBELIB_CPPFLAGS], ["`${scorep_cubelib_bindir}/cubelib-config --cppflags`"])
     AC_SUBST([CUBELIB_LIBS],     ["`${scorep_cubelib_bindir}/cubelib-config --libs`"])
     AC_SUBST([CUBELIB_LDFLAGS],  ["`${scorep_cubelib_bindir}/cubelib-config --ldflags`"])
     AC_SUBST([CUBELIB_BINDIR],          ["${scorep_cubelib_bindir}"])
    ],
    [AC_SUBST([CUBELIB_CPPFLAGS], ['-I$(srcdir)/../vendor/cubelib/src/cube/include -I../vendor/cubelib/src -I$(srcdir)/../vendor/cubelib/src/cube/include/service -I$(srcdir)/../vendor/cubelib/src/cube/include/dimensions/metric -I$(srcdir)/../vendor/cubelib/src/cube/include/network  -I$(srcdir)/../vendor/cubelib/src/cube/include/dimensions -I$(srcdir)/../vendor/cubelib/src/cube/include/dimensions/system -I$(srcdir)/../vendor/cubelib/src/cube/include/dimensions/calltree -I$(srcdir)/../vendor/cubelib/src/cube/include/dimensions/metric/matrix -I$(srcdir)/../vendor/cubelib/src/cube/include/dimensions/metric/value -I$(srcdir)/../vendor/cubelib/src/cube/include/dimensions/metric/value/trafo/single_value -I$(srcdir)/../vendor/cubelib/src/cube/include/dimensions/metric/index -I$(srcdir)/../vendor/cubelib/src/cube/include/dimensions/metric/data/rows -I$(srcdir)/../vendor/cubelib/src/cube/include/dimensions/metric/strategies -I$(srcdir)/../vendor/cubelib/src/cube/include/service/cubelayout -I$(srcdir)/../vendor/cubelib/src/cube/include/service/cubelayout/readers -I$(srcdir)/../vendor/cubelib/src/cube/include/service/cubelayout/layout -I$(srcdir)/../vendor/cubelib/src/cube/include/dimensions/metric/data -I$(srcdir)/../vendor/cubelib/src/cube/include/dimensions/metric/cache -I$(srcdir)/../vendor/cubelib/src/cube/include/syntax/cubepl -I$(srcdir)/../vendor/cubelib/src/cube/include/syntax/cubepl/evaluators -I$(srcdir)/../vendor/cubelib/src/cube/include/topologies'])
    AC_SUBST([CUBELIB_LIBS], [../vendor/cubelib/build-frontend/libcube4.la])
    AC_SUBST([CUBELIB_BINDIR], ["../vendor/cubelib/build-frontend"])
    AC_SUBST([CUBELIB_LDFLAGS],  [])
    ])

## Check for cube reader header and library only if we are using an
## external cube.
AS_IF([test -n "${scorep_cubelib_bindir}"],
    [AC_LANG_PUSH([C++])
     scorep_save_cppflags="${CPPFLAGS}"
     CPPFLAGS="${CUBELIB_CPPFLAGS} ${CPPFLAGS}"
     AC_CHECK_HEADER([Cube.h],
         [has_cubelib_header="yes"],
         [has_cubelib_header="no"])

     scorep_save_libs="${LIBS}"
     LIBS="${LIBS} ${CUBELIB_LIBS}"
     scorep_save_ldflags="${LDFLAGS}"
     LDFLAGS="${CUBELIB_LDFLAGS} ${LDFLAGS}"
     AC_LINK_IFELSE([AC_LANG_PROGRAM(
                         [[#include <Cube.h>]],
                         [[cube::Cube mycube;]])],
                         [has_cubelib_reader_lib="yes"],
                         [has_cubelib_reader_lib="no"])

     AC_MSG_CHECKING([for cube reader library])
     AS_IF([test "x${has_cubelib_header}" = "xyes" && test "x${has_cubelib_reader_lib}" = "xyes"],
         [AC_MSG_RESULT([yes])
          AFS_SUMMARY([cube c++ library support], [yes, using ${CUBELIB_CPPFLAGS} ${CUBELIB_LDFLAGS} ${CUBELIB_LIBS}])
          AM_CONDITIONAL([HAVE_SCOREP_SCORE], [test 1 -eq 1])],
         [AC_MSG_RESULT([no])
          AFS_SUMMARY([cube c++ library support], [no])
          AM_CONDITIONAL([HAVE_SCOREP_SCORE], [test 1 -eq 0])])

     ## Clean up
     LIBS="${scorep_save_libs}"
     LDFLAGS="${scorep_save_ldflags}"
     CPPFLAGS="${scorep_save_cppflags}"
     AC_LANG_POP([C++])
    ],
    [# using internal cube c++ library, header and lib assumend to be in place
     AFS_SUMMARY([cube c++ library support], [yes, using internal])
     AM_CONDITIONAL([HAVE_SCOREP_SCORE], [test 1 -eq 1])
    ])
])
