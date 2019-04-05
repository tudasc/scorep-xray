## -*- mode: autoconf -*-

##
## This file is part of the Score-P software (http://www.score-p.org)
##
## Copyright (c) 2013-2014,
## Technische Universitaet Dresden, Germany
##
## Copyright (c) 2017,
## Forschungszentrum Juelich GmbH, Germany
##
## This software may be modified and distributed under the terms of
## a BSD-style license.  See the COPYING file in the package base
## directory for details.
##

## file build-config/m4/afs_compiler_shmem.m4


AC_DEFUN([AFS_COMPILER_SHMEM],
[
AC_CHECK_PROG([NM], [nm], [`which nm`])
AC_REQUIRE([AC_PROG_GREP])
AC_REQUIRE([AC_PROG_AWK])

NSHMEMS=0
SHMEMS=""
SHMEM=""

AC_MSG_CHECKING([for oshcc])
SHMEMCC=`which oshcc 2> /dev/null`
AS_IF([test -n "${SHMEMCC}"],
    [R_SHMEMCC=`readlink -f ${SHMEMCC}`
     AS_IF([test -n "${R_SHMEMCC}"],
         [SHMEMCC=${R_SHMEMCC}])
     SBINDIR=`dirname ${SHMEMCC}`
     SHMEMROOTDIR1=`dirname ${SBINDIR}`

     echo "#include <shmem.h>" > conftest.c
     oshcc -E conftest.c | ${GREP} '/shmem.h"' | head -1 > shmemconf.txt
     SINCDIR=`cat shmemconf.txt | sed -e 's#^.* "##' -e 's#/shmem.h".*##'`
     AS_IF([test -n "${SINCDIR}"],
         [SHMEMROOTDIR2=`dirname ${SINCDIR}`
          R_SHMEMROOTDIR2=`readlink -f ${SHMEMROOTDIR2}`
          AS_IF([test -n "${R_SHMEMROOTDIR2}"],
              [SHMEMROOTDIR2=${R_SHMEMROOTDIR2}])
          rm -f conftest.c shmemconf.txt
          AS_IF([test "${SHMEMROOTDIR1}" = "${SHMEMROOTDIR2}"],
              [SHMEMROOTDIR2=""])
         ])

     FSHMEM=""
     for sr in ${SHMEMROOTDIR1} ${SHMEMROOTDIR2}
     do
         SLIBDIR="${sr}/lib"
         SLIB64DIR="${sr}/lib64"
         SBINDIR="${sr}/bin"
         AS_IF([(test -f ${SLIBDIR}/libopenshmem.a || test -f ${SLIBDIR}/libopenshmem.so || \
                 test -f ${SLIB64DIR}/libopenshmem.a || test -f ${SLIB64DIR}/libopenshmem.so)],
               [NSHMEMS=`expr ${NSHMEMS} + 1`
                FSHMEM=openshmem
                AC_MSG_RESULT([OpenSHMEM ${SHMEMCC}])],
               [( ( test -d ${SLIBDIR}/spectrum_mpi   && ( test -f ${SLIBDIR}/liboshmem.a   || test -f ${SLIBDIR}/liboshmem.so   ) ) || \
                  ( test -d ${SLIB64DIR}/spectrum_mpi && ( test -f ${SLIB64DIR}/liboshmem.a || test -f ${SLIB64DIR}/liboshmem.so ) ) )],
               [NSHMEMS=`expr ${NSHMEMS} + 1`
                FSHMEM=spectrum
                AC_MSG_RESULT([IBM Spectrum MPI OpenSHMEM ${SHMEMCC}])],
               [( ( test -d ${SLIBDIR}/openmpi   && ( test -f ${SLIBDIR}/liboshmem.a   || test -f ${SLIBDIR}/liboshmem.so   ) ) || \
                  ( test -d ${SLIB64DIR}/openmpi && ( test -f ${SLIB64DIR}/liboshmem.a || test -f ${SLIB64DIR}/liboshmem.so ) ) )],
               [NSHMEMS=`expr ${NSHMEMS} + 1`
                AS_IF([test ! -f ${SBINDIR}/oshcxx],
                    [FSHMEM=openmpi
                     AC_MSG_RESULT([Open MPI OpenSHMEM ${SHMEMCC}])],
                    [FSHMEM=openmpi3
                     AC_MSG_RESULT([Open MPI 3 OpenSHMEM ${SHMEMCC}])])
                ])

         AS_IF([test -n "${FSHMEM}"],
             [AS_IF([test -z "${SHMEMS}"],
                  [SHMEMS="${FSHMEM}"],
                  [SHMEMS="${SHMEMS}|${FSHMEM}"])
              SHMEM="${FSHMEM}"
              break])
     done
     AS_IF([test -z "${FSHMEM}"],
         [AC_MSG_RESULT([no])])
    ],
    [AC_MSG_RESULT([no])
    ])

AC_MSG_CHECKING([for SGI MPT])
# rail-config - Sets up the MPI multi-rail configuration file
# newer SGI MPT versions provide wrappers (oshcc, oshCC, oshfort)
SHMEMRC=`which rail-config 2> /dev/null`
AS_IF([(test -f /etc/sgi-release && test "x${SHMEMRC}" != x)],
    [SBINDIR=`dirname ${SHMEMRC}`
     AS_IF([test -f "${SBINDIR}/oshcc"],
         [SHMEM=sgimptwrapper
          variant="(wrapper)"],
         [SHMEM=sgimpt
          variant="(plain)"])
     NSHMEMS=`expr ${NSHMEMS} + 1`
     AC_MSG_RESULT([SGI MPT ${variant} ${SBINDIR}])
     AS_IF([test -z "${SHMEMS}"],
         [SHMEMS="${SHMEM}"],
         [SHMEMS="${SHMEMS}|${SHMEM}"])
    ],
    [AC_MSG_RESULT([no])
    ])

dnl echo NSHMEMS $NSHMEMS
dnl echo SHMEMS $SHMEMS
dnl echo SHMEM $SHMEM

AS_IF([test "${NSHMEMS}" -eq 0],
    [AS_IF([test -n "${SHMEMCC}"],
         [AC_MSG_ERROR([SHMEM C compiler found but cannot determine SHMEM library. Select SHMEM using --with-shmem])],
         [AC_MSG_NOTICE([cannot detect SHMEM library. Make sure SHMEM compiler is in your PATH and rerun configure. Please specify --with-shmem=no if you intend to build Score-P without SHMEM support.])
          afs_compiler_shmem="without"
          scorep_shmem_user_disabled="yes"])],
    [test "${NSHMEMS}" -gt 1],
    [AC_MSG_ERROR([found ${NSHMEMS} SHMEM installations. Select one using --with-shmem=${SHMEMS}])],
    [afs_compiler_shmem=${SHMEM}
     AC_MSG_NOTICE([using '${afs_compiler_shmem}' SHMEM implementation.])
    ])
])# AFS_COMPILER_SHMEM
