## -*- mode: autoconf -*-

## 
## This file is part of the Score-P software (http://www.score-p.org)
##
## Copyright (c) 2013
## Forschungszentrum Juelich GmbH, Germany
##
## This software may be modified and distributed under the terms of
## a BSD-style license.  See the COPYING file in the package base
## directory for details.
##

## file build-config/m4/afs_compiler_mpi.m4


AC_DEFUN([AFS_COMPILER_MPI],
[

# -- Uncomment for stand-alone use
CFGecho () {
  echo "$1"
}
dnl set -x
NMPIS=0
MPIS=""
MPI=""

echo NMPIS $NMPIS 
echo MPIS $MPIS
echo MPI $MPI

MPIICC=`which mpiicc 2> /dev/null`
AS_IF([test -n "${MPIICC}"], 
    [R_MPIICC=`readlink -f ${MPIICC}`
     AS_IF([test -n "${R_MPIICC}"], 
         [MPICC=${R_MPIICC}])
     MBINDIR=`dirname ${MPIICC}`
     MINCDIR=`echo ${MBINDIR} | sed -e 's/bin/include/'`
     AS_IF([test -f ${MINCDIR}/mpi.h],
         [IMPIVER=`grep ^# ${MINCDIR}/mpi.h 2> /dev/null | \
                   grep MPI_VERSION | awk '{print $NF}'`
          AS_IF([test -z "${IMPIVER}"], 
              [IMPIVER=-42])
          AS_IF([test ${IMPIVER} -eq 1], 
              [NMPIS=`expr ${NMPIS} + 1`
               MPI=intel
               CFGecho "INFO: Found Intel MPI 1 ${MPIICC}"],
              [test ${IMPIVER} -eq 2],
              [NMPIS=`expr ${NMPIS} + 1`
               MPI=intel2
               CFGecho "INFO: Found Intel MPI 2 ${MPIICC}"],
              [CFGecho "ERROR: Cannot determine Intel MPI version"
               CFGecho "Select MPI using --mpi=intel|intel2"
               exit 1])
         ])
     AS_IF([test -z "${MPIS}"], 
         [MPIS="${MPI}"], 
         [MPIS="${MPIS}|${MPI}"])
    ])

MPCC=`which mpcc 2> /dev/null`
AS_IF([test -n "${MPCC}"],
    [R_MPCC=`readlink -f ${MPCC}`
     AS_IF([test -n "${R_MPCC}"], 
         [MPICC=${R_MPCC}])
     MBINDIR=`dirname ${MPCC}`
     AS_IF([test `uname -m` = "x86_64"],
         [NMPIS=`expr ${NMPIS} + 1`
          MPI=intelpoe
          CFGecho "INFO: Found Intel POE ${MBINDIR}"],
         [NMPIS=`expr ${NMPIS} + 1`
          MPI=ibmpoe
          CFGecho "INFO: Found IBM POE ${MBINDIR}"])
     AS_IF([test -z "${MPIS}"], 
         [MPIS="${MPI}"], 
         [MPIS="${MPIS}|${MPI}"])
    ])

MPIRC=`which rail-config 2> /dev/null`
AS_IF([test -f /etc/sgi-release && test -n "${MPIRC}"],
    [R_MPIRC=`readlink -f ${MPIRC}`
     AS_IF([test -n "${R_MPIRC}"], 
         [MPICC=${R_MPIRC}])
     MBINDIR=`dirname ${MPIRC}`
     NMPIS=`expr ${NMPIS} + 1`
     MPI=sgimpt
     CFGecho "INFO: Found SGI MPT ${MBINDIR}"
     AS_IF([test -z "${MPIS}"], 
         [MPIS="${MPI}"], 
         [MPIS="${MPIS}|${MPI}"])
    ])
set -x
MPICC=`which mpicc 2> /dev/null`
AS_IF([test -n "${MPICC}"],
    [FMPI=""
     R_MPICC=`readlink -f ${MPICC}`
     AS_IF([test -n "${R_MPICC}"], 
         [MPICC=${R_MPICC}])
     MBINDIR=`dirname ${MPICC}`
     MPIROOTDIR1=`dirname ${MBINDIR}`
   
     echo "#include <mpi.h>" > conftest.c
     mpicc -E conftest.c | grep '/mpi.h"' | head -1 > mpiconf.txt
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
   
     for mr in ${MPIROOTDIR1} ${MPIROOTDIR2}
     do
         MLIBDIR="${mr}/lib"
         MLIB64DIR="${mr}/lib64"
         MBINDIR="${mr}/bin"
         AS_IF([test -f ${MLIBDIR}/libmpich.a || test -f ${MLIBDIR}/libmpich.so || \
              test -f ${MLIB64DIR}/libmpich.a || test ${MLIB64DIR}/libmpich.so],
             [AS_IF([test -f ${MBINDIR}/mpibull2-version],
                  [NMPIS=`expr ${NMPIS} + 1`
                   FMPI=mpibull2
                   CFGecho "INFO: Found Bull MPICH2 ${MPICC}"],
                  [test -f ${MBINDIR}/mpichversion],
                  [MPICHVER=`grep ^# ${MINCDIR}/mpi.h 2> /dev/null | \
                             grep MPI_VERSION | awk '{print $NF}'`
                   AS_IF([test -z "${MPICHVER}"],
                       [MPICHVER=-42])
                   AS_IF([test ${MPICHVER} -eq 3],
                       [NMPIS=`expr ${NMPIS} + 1`
                        FMPI=mpich3
                        CFGecho "INFO: Found MPICH3 ${MPICC}"],
                       [NMPIS=`expr ${NMPIS} + 1`
                        FMPI=mpich
                        CFGecho "INFO: Found MPICH1 ${MPICC}"])
                  ],
                  [test -f ${MBINDIR}/mpich2version],
                  [NMPIS=`expr ${NMPIS} + 1`
                   FMPI=mpich2
                   CFGecho "INFO: Found MPICH2 ${MPICC}"],
                  [test ! -f ${MBINDIR}/mpiicc],
                  [AS_IF([test -f ${MLIBDIR}/libmpich.a], 
                       [ML=${MLIBDIR}/libmpich.a],
                       [test -f ${MLIBDIR}/libmpich.so], 
                       [ML=${MLIBDIR}/libmpich.so],
                       [test -f ${MLIB64DIR}/libmpich.a], 
                       [ML=${MLIB64DIR}/libmpich.a],
                       [test -f ${MLIB64DIR}/libmpich.so], 
                       [ML=${MLIB64DIR}/libmpich.so],
                       [CFGecho "ERROR: Cannot determine MPICH version"
                        exit 1])
                   nm ${ML} | grep -q MPI_Win
                   AS_IF([test $? -eq 0],
                       [NMPIS=`expr ${NMPIS} + 1`
                        FMPI=mpich2
                        CFGecho "INFO: Found MPICH2 ${MPICC}"],
                       [NMPIS=`expr ${NMPIS} + 1`
                        FMPI=mpich
                        CFGecho "INFO: Found MPICH1 ${MPICC}"])
                  ])
             ],
             [test -f ${MLIBDIR}/liblam.a || test -f ${MLIBDIR}/liblam.so || \
              test -f ${MLIB64DIR}/liblam.a || test -f ${MLIB64DIR}/liblam.so],
             [NMPIS=`expr ${NMPIS} + 1`
              FMPI=lam
              CFGecho "INFO: Found LAM ${MPICC}"],
             [test -d ${MLIBDIR}/openmpi && \( test -f ${MLIBDIR}/libmpi.a || test -f ${MLIBDIR}/libmpi.so \) || \
              test -d ${MLIB64DIR}/openmpi && \( test -f ${MLIB64DIR}/libmpi.a || test -f ${MLIB64DIR}/libmpi.so \)],
             [NMPIS=`expr ${NMPIS} + 1`
              FMPI=openmpi
              CFGecho "INFO: Found Open MPI ${MPICC}"],
             [test -d ${MLIBDIR}/bullxmpi && \( test -f ${MLIBDIR}/libmpi.a || test -f ${MLIBDIR}/libmpi.so \) || \
              test -d ${MLIB64DIR}/bullxmpi && \( test -f ${MLIB64DIR}/libmpi.a || test -f ${MLIB64DIR}/libmpi.so \)],
             [NMPIS=`expr ${NMPIS} + 1`
              FMPI=bullxmpi
              CFGecho "INFO: Found bullx MPI ${MPICC}"],
             [test -f ${MLIBDIR}/linux_ia32/libpcmpi.a || test -f ${MLIBDIR}/linux_ia32/libpcmpi.so || \
              test -f ${MLIBDIR}/linux_amd64/libpcmpi.a || test -f ${MLIBDIR}/linux_amd64/libpcmpi.so],
             [NMPIS=`expr ${NMPIS} + 1`
              FMPI=platform
              CFGecho "INFO: Found Platform Computing MPI ${MPICC}"],
             [test -f ${MLIBDIR}/linux_ia32/libhpmpi.a || test -f ${MLIBDIR}/linux_ia32/libhpmpi.so || \
              test -f ${MLIBDIR}/linux_amd64/libhpmpi.a || test -f ${MLIBDIR}/linux_amd64/libhpmpi.so || \
              test -f ${MLIBDIR}/linux_ia64/libhpmpi.a || test -f ${MLIBDIR}/linux_ia64/libhpmpi.so],
             [NMPIS=`expr ${NMPIS} + 1`
              FMPI=hp
              CFGecho "INFO: Found HP MPI ${MPICC}"],
             [test -f ${MBINDIR}/mpimon && \( test -f ${MLIBDIR}/libmpi.a || test -f ${MLIBDIR}/libmpi.so || \
              test -f ${MLIB64DIR}/libmpi.a || test -f ${MLIB64DIR}/libmpi.so \)],
             [NMPIS=`expr ${NMPIS} + 1`
              FMPI=scali
              CFGecho "INFO: Found SCALI MPI ${MPICC}"])

         AS_IF([test -n "${FMPI}"],
             [AS_IF([test -z "${MPIS}"], 
                  [MPIS="${FMPI}"], 
                  [MPIS="${MPIS}|${FMPI}"])
              MPI="${FMPI}"
              break])
     done
    ])
set +x
echo NMPIS $NMPIS 
echo MPIS $MPIS
echo MPI $MPI
set -x
AS_IF([test "${NMPIS}" -eq 0],
    [AS_IF([test -n "${MPICC}"],
         [CFGecho "ERROR: mpicc found but cannot determine MPI library"
          CFGecho "Select MPI using --mpi=mpich|mpich2|mpibull2|lam|openmpi|bullxmpi|intel2|hp|scali"],
         [CFGecho "ERROR: Cannot detect MPI library"
          CFGecho "Make sure mpicc, mpcc or mpiicc is in your PATH and rerun configure"
          CFGecho "or specify --disable-mpi to build without MPI measurement support"])
     exit 1],
    [test "${NMPIS}" -gt 1],
    [CFGecho ""
     CFGecho "INFO: Found ${NMPIS} MPI installations"
     CFGecho "Select MPI using --mpi=${MPIS}"
     exit 1
    ])

echo NMPIS $NMPIS 
echo MPIS $MPIS
echo MPI $MPI

CFGecho ""
set +x
])# AFS_COMPILER_MPI
