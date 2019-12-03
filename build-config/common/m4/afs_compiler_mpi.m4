## -*- mode: autoconf -*-

##
## This file is part of the Score-P software (http://www.score-p.org)
##
## Copyright (c) 2013-2014, 2017,
## Forschungszentrum Juelich GmbH, Germany
##
## This software may be modified and distributed under the terms of
## a BSD-style license.  See the COPYING file in the package base
## directory for details.
##

## file build-config/m4/afs_compiler_mpi.m4


AC_DEFUN([AFS_COMPILER_MPI],
[
AC_CHECK_PROG([NM], [nm], [`which nm`])
AC_REQUIRE([AC_PROG_GREP])
AC_REQUIRE([AC_PROG_AWK])

NMPIS=0
MPIS=""
MPI=""

AC_MSG_CHECKING([for mpiicc])
MPIICC=`which mpiicc 2> /dev/null`
AS_IF([test -n "${MPIICC}"],
    [R_MPIICC=`readlink -f ${MPIICC}`
     AS_IF([test -n "${R_MPIICC}"],
         [MPICC=${R_MPIICC}])
     MBINDIR=`dirname ${MPIICC}`
     MINCDIR=`echo ${MBINDIR} | sed -e 's/bin/include/'`
     AS_IF([test -f ${MINCDIR}/mpi.h],
         [IMPIVER=`${GREP} ^# ${MINCDIR}/mpi.h 2> /dev/null | \
                   ${GREP} MPI_VERSION | head -1 | ${AWK} '{print $NF}'`
          AS_IF([test -z "${IMPIVER}"],
              [IMPIVER=-42])
          AS_IF([test ${IMPIVER} -eq 1],
              [NMPIS=`expr ${NMPIS} + 1`
               MPI=intel
               AC_MSG_RESULT([Intel MPI 1 ${MPIICC}])],
              [test ${IMPIVER} -eq 2],
              [NMPIS=`expr ${NMPIS} + 1`
               MPI=intel2
               AC_MSG_RESULT([Intel MPI 2 ${MPIICC}])],
              [test ${IMPIVER} -eq 3],
              [NMPIS=`expr ${NMPIS} + 1`
               MPI=intel3
               AC_MSG_RESULT([Intel MPI 3 ${MPIICC}])],
              [AC_MSG_RESULT([yes])
               AC_MSG_ERROR([cannot determine Intel MPI version. Please select MPI using --with-mpi=intel|intel2|intel3])
              ])
         ])
     AS_IF([test -z "${MPIS}"],
         [MPIS="${MPI}"],
         [MPIS="${MPIS}|${MPI}"])
    ],
    [AC_MSG_RESULT([no])
    ])

AC_MSG_CHECKING([for mpcc])
MPCC=`which mpcc 2> /dev/null`
AS_IF([test -n "${MPCC}"],
    [R_MPCC=`readlink -f ${MPCC}`
     AS_IF([test -n "${R_MPCC}"],
         [MPICC=${R_MPCC}])
     MBINDIR=`dirname ${MPCC}`
     AS_IF([test `uname -m` = "x86_64"],
         [NMPIS=`expr ${NMPIS} + 1`
          MPI=intelpoe
          AC_MSG_RESULT([Intel POE ${MBINDIR}])],
         [NMPIS=`expr ${NMPIS} + 1`
          MPI=ibmpoe
          AC_MSG_RESULT([IBM POE ${MBINDIR}])])
     AS_IF([test -z "${MPIS}"],
         [MPIS="${MPI}"],
         [MPIS="${MPIS}|${MPI}"])
    ],
    [AC_MSG_RESULT([no])
    ])

AC_MSG_CHECKING([for SGI MPT])
# rail-config - Sets up the MPI multi-rail configuration file
# newer SGI MPT versions provide wrappers (mpicc, mpicxx, mpif77/90/08)
MPIRC=`which rail-config 2> /dev/null`
AS_IF([(test -f /etc/sgi-release && test "x${MPIRC}" != x)],
    [MBINDIR=`dirname ${MPIRC}`
     AS_IF([test -f "${MBINDIR}/mpicc"],
         [MPI=sgimptwrapper
          variant="(wrapper)"],
         [MPI=sgimpt
          variant="(plain)"])
     NMPIS=`expr ${NMPIS} + 1`
     AC_MSG_RESULT([SGI MPT ${variant} ${MBINDIR}])
     AS_IF([test -z "${MPIS}"],
         [MPIS="${MPI}"],
         [MPIS="${MPIS}|${MPI}"])
    ],
    [AC_MSG_RESULT([no])
    ])

AC_MSG_CHECKING([for mpicc])
MPICC=`which mpicc 2> /dev/null`
AS_IF([test -n "${MPICC}"],
    [R_MPICC=`readlink -f ${MPICC}`
     AS_IF([test -n "${R_MPICC}"],
         [MPICC=${R_MPICC}])
     MBINDIR=`dirname ${MPICC}`
     MPIROOTDIR1=`dirname ${MBINDIR}`

     echo "#include <mpi.h>" > conftest.c
     mpicc -E conftest.c | ${GREP} '/mpi.h"' | head -1 > mpiconf.txt
     MINCDIR=`cat mpiconf.txt | sed -e 's#^.* "##' -e 's#/mpi.h".*##'`
     AS_IF([test -n "${MINCDIR}"],
         [MPIROOTDIR2=`dirname ${MINCDIR}`
          R_MPIROOTDIR2=`readlink -f ${MPIROOTDIR2}`
          AS_IF([test -n "${R_MPIROOTDIR2}"],
              [MPIROOTDIR2=${R_MPIROOTDIR2}])
          rm -f conftest.c mpiconf.txt
          AS_IF([test "${MPIROOTDIR1}" = "${MPIROOTDIR2}"],
              [MPIROOTDIR2=""])
         ])

     FMPI=""
     for mr in ${MPIROOTDIR1} ${MPIROOTDIR2}
     do
         MLIBDIR="${mr}/lib"
         MLIB64DIR="${mr}/lib64"
         MBINDIR="${mr}/bin"
         AS_IF([(test -f ${MLIBDIR}/libmpich.a || test -f ${MLIBDIR}/libmpich.so || \
              test -f ${MLIB64DIR}/libmpich.a || test -f ${MLIB64DIR}/libmpich.so)],
             [AS_IF([test -f ${MBINDIR}/mpibull2-version],
                  [NMPIS=`expr ${NMPIS} + 1`
                   FMPI=mpibull2
                   AC_MSG_RESULT([Bull MPICH2 ${MPICC}])],
                  [test -f ${MBINDIR}/mpichversion],
                  [MPICHVER=`${GREP} ^# ${MINCDIR}/mpi.h 2> /dev/null | \
                             ${GREP} MPI_VERSION | ${AWK} '{print $NF}'`
                   AS_IF([test -z "${MPICHVER}"],
                       [MPICHVER=-42])
                   AS_IF([test ${MPICHVER} -eq 3],
                       [NMPIS=`expr ${NMPIS} + 1`
                        FMPI=mpich3
                        AC_MSG_RESULT([MPICH3 ${MPICC}])],
                       [NMPIS=`expr ${NMPIS} + 1`
                        FMPI=mpich
                        AC_MSG_RESULT([MPICH1 ${MPICC}])])
                  ],
                  [test -f ${MBINDIR}/mpich2version],
                  [NMPIS=`expr ${NMPIS} + 1`
                   FMPI=mpich2
                   AC_MSG_RESULT([MPICH2 ${MPICC}])],
                  [test ! -f ${MBINDIR}/mpiicc],
                  [AS_IF([test -f ${MLIBDIR}/libmpich.a],
                       [ML=${MLIBDIR}/libmpich.a],
                       [test -f ${MLIBDIR}/libmpich.so],
                       [ML=${MLIBDIR}/libmpich.so],
                       [test -f ${MLIB64DIR}/libmpich.a],
                       [ML=${MLIB64DIR}/libmpich.a],
                       [test -f ${MLIB64DIR}/libmpich.so],
                       [ML=${MLIB64DIR}/libmpich.so],
                       [AC_MSG_ERROR([cannot determine MPICH version])
                       ])
                   ( ${NM} ${ML} | ${GREP} MPI_Win >/dev/null 2>&1 )
                   AS_IF([test $? -eq 0],
                       [NMPIS=`expr ${NMPIS} + 1`
                        FMPI=mpich2
                        AC_MSG_RESULT([MPICH2 ${MPICC}])],
                       [NMPIS=`expr ${NMPIS} + 1`
                        FMPI=mpich
                        AC_MSG_RESULT([MPICH1 ${MPICC}])])
                  ])
             ],
             [(test -f ${MLIBDIR}/liblam.a || test -f ${MLIBDIR}/liblam.so || \
              test -f ${MLIB64DIR}/liblam.a || test -f ${MLIB64DIR}/liblam.so)],
             [NMPIS=`expr ${NMPIS} + 1`
              FMPI=lam
              AC_MSG_RESULT([LAM ${MPICC}])],
             [( (test -d ${MLIBDIR}/openmpi && ( test -f ${MLIBDIR}/libmpi.a || test -f ${MLIBDIR}/libmpi.so ) ) || \
              (test -d ${MLIB64DIR}/openmpi && ( test -f ${MLIB64DIR}/libmpi.a || test -f ${MLIB64DIR}/libmpi.so ) ) )],
             [NMPIS=`expr ${NMPIS} + 1`
              AS_IF([test ! -f ${MBINDIR}/oshcxx],
                  [FMPI=openmpi
                   AC_MSG_RESULT([Open MPI ${MPICC}])],
                  [FMPI=openmpi3
                   AC_MSG_RESULT([Open MPI 3 ${MPICC}])])
             ],
             [( (test -d ${MLIBDIR}/bullxmpi && ( test -f ${MLIBDIR}/libmpi.a || test -f ${MLIBDIR}/libmpi.so ) ) || \
              (test -d ${MLIB64DIR}/bullxmpi && ( test -f ${MLIB64DIR}/libmpi.a || test -f ${MLIB64DIR}/libmpi.so ) ) )],
             [NMPIS=`expr ${NMPIS} + 1`
              FMPI=bullxmpi
              AC_MSG_RESULT([bullx MPI ${MPICC}])],
             [( (test -d ${MLIBDIR}/spectrum_mpi && ( test -f ${MLIBDIR}/libmpi_ibm.a || test -f ${MLIBDIR}/libmpi_ibm.so ) ) || \
              (test -d ${MLIB64DIR}/spectrum_mpi && ( test -f ${MLIB64DIR}/libmpi_ibm.a || test -f ${MLIB64DIR}/libmpi_ibm.so ) ) )],
             [NMPIS=`expr ${NMPIS} + 1`
              FMPI=spectrum
              AC_MSG_RESULT([IBM Spectrum MPI ${MPICC}])],
             [(test -f ${MLIBDIR}/linux_ia32/libpcmpi.a || test -f ${MLIBDIR}/linux_ia32/libpcmpi.so || \
              test -f ${MLIBDIR}/linux_amd64/libpcmpi.a || test -f ${MLIBDIR}/linux_amd64/libpcmpi.so)],
             [NMPIS=`expr ${NMPIS} + 1`
              FMPI=platform
              AC_MSG_RESULT([Platform Computing MPI ${MPICC}])],
             [(test -f ${MLIBDIR}/linux_ia32/libhpmpi.a || test -f ${MLIBDIR}/linux_ia32/libhpmpi.so || \
              test -f ${MLIBDIR}/linux_amd64/libhpmpi.a || test -f ${MLIBDIR}/linux_amd64/libhpmpi.so || \
              test -f ${MLIBDIR}/linux_ia64/libhpmpi.a || test -f ${MLIBDIR}/linux_ia64/libhpmpi.so)],
             [NMPIS=`expr ${NMPIS} + 1`
              FMPI=hp
              AC_MSG_RESULT([HP MPI ${MPICC}])],
             [(test -f ${MBINDIR}/mpimon && ( test -f ${MLIBDIR}/libmpi.a || test -f ${MLIBDIR}/libmpi.so || \
              test -f ${MLIB64DIR}/libmpi.a || test -f ${MLIB64DIR}/libmpi.so ) )],
             [NMPIS=`expr ${NMPIS} + 1`
              FMPI=scali
              AC_MSG_RESULT([SCALI MPI ${MPICC}])])

         AS_IF([test -n "${FMPI}"],
             [AS_IF([test -z "${MPIS}"],
                  [MPIS="${FMPI}"],
                  [MPIS="${MPIS}|${FMPI}"])
              MPI="${FMPI}"
              break])
     done
     AS_IF([test -z "${FMPI}"],
         [AC_MSG_RESULT([no])])
    ],
    [AC_MSG_RESULT([no])
    ])

dnl echo NMPIS $NMPIS
dnl echo MPIS $MPIS
dnl echo MPI $MPI

AS_IF([test "${NMPIS}" -eq 0],
    [AS_IF([test -n "${MPICC}"],
         [AC_MSG_ERROR([mpicc found but cannot determine MPI library. Select MPI using --with-mpi=bullxmpi|hp|lam|mpibull2|mpich|mpich2|mpich3|openmpi|platform|scali])],
         [AC_MSG_ERROR([cannot detect MPI library. Make sure mpicc, mpcc or mpiicc is in your PATH and rerun configure. Please specify --without-mpi if you intend to build AC_PACKAGE_NAME without MPI support.])])],
    [test "${NMPIS}" -gt 1],
    [AC_MSG_ERROR([found ${NMPIS} MPI installations. Select one using --with-mpi=${MPIS}])],
    [afs_compiler_mpi=${MPI}
     AC_MSG_NOTICE([using '${afs_compiler_mpi}' MPI implementation.])
    ])
])# AFS_COMPILER_MPI
