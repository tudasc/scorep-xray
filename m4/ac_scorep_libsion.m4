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
## Copyright (c) 2009-2013,
## Technische Universitaet Dresden, Germany
##
## Copyright (c) 2009-2011,
## University of Oregon, Eugene, USA
##
## Copyright (c) 2009-2013,
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

## file build-config/m4/ac_scorep_libsion.m4

# output of sionconfig on jugene:
# sionconfig --be --ser --cflags
# -I/usr/local/sionlib/v1.2p2/include -DBGP -D_SION_BGP
# sionconfig --be --ser --libs
# -L/usr/local/sionlib/v1.2p2/lib -lsionser_32
# sionconfig --be --ser --path
# /usr/local/sionlib/v1.2p2
# sionconfig --be --mpi --cflags
# -I/usr/local/sionlib/v1.2p2/include -DBGP -DSION_MPI -D_SION_BGP
# sionconfig --be --mpi --libs
# -L/usr/local/sionlib/v1.2p2/lib -lsion_32 -lsionser_32
# sionconfig --be --mpi --path
# /usr/local/sionlib/v1.2p2
# sionconfig --fe --ser --cflags
# -I/usr/local/sionlib/v1.2p2/include -DBGP -D_SION_BGP
# sionconfig --fe --ser --libs
# -L/usr/local/sionlib/v1.2p2/lib -lsionserfe_32
# sionconfig --fe --ser --path
# /usr/local/sionlib/v1.2p2
# sionconfig --fe --mpi --cflags
# -I/usr/local/sionlib/v1.2p2/include -DBGP -DSION_MPI -D_SION_BGP
# sionconfig --fe --mpi --libs
# -L/usr/local/sionlib/v1.2p2/lib -lsionfe_32 -lsionserfe_32
# sionconfig --fe --mpi --path
# /usr/local/sionlib/v1.2p2


# AC_SCOREP_LIBSION(GENERIC|SERIAL|OMP|MPI|MPI_OMP)
AC_DEFUN([AC_SCOREP_LIBSION],
[
m4_case([$1], [GENERIC], [], [SERIAL], [], [OMP], [], [MPI], [], [MPI_OMP], [], [m4_fatal([parameter must be either SERIAL, OMP, MPI or MPI_OMP])])

dnl <begin> Adaption from AC_SCOREP_HAVE_CONFIG_TOOL
AC_ARG_WITH([sionlib], 
    [AS_HELP_STRING([--with-sionlib[[=<sionlib-bindir>]]], 
        [Use an already installed sionlib. Provide path to sionconfig. Auto-detected if already in $PATH.])], 
    [with_sion="${with_sionlib%/}"], dnl yes, no, or <path>
    [with_sion="not_given"])

AS_UNSET([scorep_have_sion_config])
AS_UNSET([scorep_sion_config_bin])
AS_UNSET([scorep_sion_config_arg])
AS_IF([test "x${with_sion}" != "xno"], 
    [AS_IF([test "x${with_sion}" = "xyes" || test "x${with_sion}" = "xnot_given"], 
        [AC_CHECK_PROG([scorep_have_sion_config], [sionconfig], ["yes"], ["no"])
         AS_IF([test "x${scorep_have_sion_config}" = "xyes"],
             [scorep_sion_config_bin="`which sionconfig`"])],
        [# --with-sionlib=<path>
         AC_CHECK_PROG([scorep_have_sion_config], [sionconfig], ["yes"], ["no"], [${with_sion}])
         AS_IF([test "x${scorep_have_sion_config}" = "xyes"],
             [scorep_sion_config_bin="${with_sion}/sionconfig"],
             [AS_UNSET([ac_cv_prog_scorep_have_sion_config])
              AS_UNSET([scorep_have_sion_config])
              AC_CHECK_PROG([scorep_have_sion_config], [sionconfig], ["yes"], ["no"], ["${with_sion}/bin"])
              AS_IF([test "x${scorep_have_sion_config}" = "xyes"],
                  [scorep_sion_config_bin="${with_sion}/bin/sionconfig"])])
        ])
     AS_IF([test "x${scorep_have_sion_config}" = "xyes"], 
         [:
dnl           Version checking via 'sionconfig --interface-version' not available yet. 
dnl           scorep_sion_config_arg="scorep_sion_bindir=`dirname ${scorep_sion_config_bin}`"
dnl           # version checking, see http://www.gnu.org/software/libtool/manual/libtool.html#Versioning
dnl           interface_version=`${scorep_sion_config_bin} --interface-version 2> /dev/null`
dnl           AS_IF([test $? -eq 0 && test "x${interface_version}" != "x"], 
dnl               [# get 'current'
dnl                sion_max_provided_interface_version=`echo ${interface_version} | awk -F ":" '{print $[]1}'`
dnl                # get 'age'
dnl                sion_provided_age=`echo ${interface_version} | awk -F ":" '{print $[]3}'`
dnl               AS_IF([test ${sion_max_provided_interface_version} -eq 0 && test ${sion_provided_age} -eq 0],
dnl                   [# by convention, trunk is 0:0:0
dnl                    AC_MSG_WARN([external sionlib built from trunk, version checks disabled, might produce compile and link errors.])
dnl                    AFS_SUMMARY([sionlib support], [yes, using external via ${scorep_sion_config_bin} (built from trunk, version checks disabled, might produce compile and link errors.)])],
dnl                   [# calc 'current - age'
dnl                    AS_VAR_ARITH([sion_min_provided_interface_version], [${sion_max_provided_interface_version} - ${sion_provided_age}])
dnl                    # this is the version check:
dnl                    AS_IF([test ${sion_max_provided_interface_version} -ge $2 && \
dnl                           test $2 -ge ${sion_min_provided_interface_version}],
dnl                        [AFS_SUMMARY([sionlib support], [yes, using external via ${scorep_sion_config_bin}])],
dnl                        [AS_IF([test ${sion_provided_age} -eq 0],
dnl                            [AC_MSG_ERROR([provided interface version '${sion_max_provided_interface_version}' of sionlib not sufficient for AC_PACKAGE_NAME, provide '$2' or compatible.])],
dnl                            [AC_MSG_ERROR([provided interface versions [[${sion_min_provided_interface_version},${sion_max_provided_interface_version}]] of sionlib not sufficient for AC_PACKAGE_NAME, provide '$2' or compatible.])
dnl ])])])
dnl               ],
dnl               [AC_MSG_ERROR([required option --interface-version not supported by sionconfig.])])
         ],
         [# scorep_have_sion_config = no
          AS_IF([test "x${with_sion}" = "xnot_given"],
              [:
dnl                There is no internal sionlib yet.
dnl                AFS_SUMMARY([sionlib support], [yes, using internal])
              ],
              [test "x${with_sion}" = "xyes"],
              [AC_MSG_ERROR([cannot detect sionconfig although it was requested via --with-sionlib.])],
              [AC_MSG_ERROR([cannot detect sionconfig in ${with_sion} and ${with_sion}/bin.])
              ])
         ])
    ],
    [# --without-sionlib
     :
     dnl There is no internal sionlib yet.
     dnl AS_IF([test ! -d ${srcdir}/vendor/$1], 
     dnl     [AC_MSG_ERROR([sionlib is required. Opting out an external sionlib via --without-sionlib is only an option if an internal sionlib is available, which isn't the case here. Please provide an external sionlib.])])
     dnl scorep_have_sion_config="no"
     dnl AFS_SUMMARY([sionlib support], [yes, using internal])
    ]
)
dnl <end> Adaption from AC_SCOREP_HAVE_CONFIG_TOOL

# macro-internal variables
scorep_sion_cppflags=""
scorep_sion_ldflags=""
scorep_sion_rpathflags=""
scorep_sion_libs=""
scorep_have_sion="no"


    if test "x${scorep_have_sion_config}" = "xyes"; then
        AC_LANG_PUSH([C])
        cppflags_save=$CPPFLAGS
        ldflags_save=$LDFLAGS
        libs_save=$LIBS

        scorep_have_sion="yes"

        sionconfig_febe_flag=""
        if test "x${ac_scorep_backend}" = "xyes"; then
            sionconfig_febe_flag="--be"
        elif test "x${ac_scorep_frontend}" = "xyes"; then
            sionconfig_febe_flag="--fe"
        fi

        AS_CASE([${build_cpu}],
                [i?86],   [sionconfig_architecture_flags="--32"],
                [x86_64], [sionconfig_architecture_flags="--64"],
                [sionconfig_architecture_flags=""])
        AS_IF([test "x${ac_scorep_platform}" = "xbgq"], [sionconfig_architecture_flags="--64"])

        m4_case([$1], [SERIAL],  [sionconfig_paradigm_flag="--ser"],
                      [OMP],     [sionconfig_paradigm_flag="--ser"],
                      [MPI],     [sionconfig_paradigm_flag="--mpi"],
                      [MPI_OMP], [sionconfig_paradigm_flag="--mpi"],
                      [GENERIC], [sionconfig_paradigm_flag="--gen --threadsafe"])

        scorep_sion_cppflags=`${scorep_sion_config_bin} $sionconfig_febe_flag $sionconfig_paradigm_flag --cflags`
        CPPFLAGS="$scorep_sion_cppflags $CPPFLAGS"
        AC_CHECK_HEADER([sion.h], [], [scorep_have_sion="no"; scorep_sion_cppflags=""])

        if test "x${scorep_have_sion}" = "xyes"; then
            scorep_sion_ldflags=`${scorep_sion_config_bin} ${sionconfig_febe_flag} ${sionconfig_paradigm_flag} ${sionconfig_architecture_flags} --libs | \
                                 awk '{for (i=1; i<=NF; i++) {if ([index]($i, "-L") == 1){ldflags = ldflags " " $i}}}END{print ldflags}'`

            scorep_sion_rpathflags=`${scorep_sion_config_bin} ${sionconfig_febe_flag} ${sionconfig_paradigm_flag} ${sionconfig_architecture_flags} --libs | \
                                 awk '{for (i=1; i<=NF; i++) {if ([index]($i, "-L") == 1){sub(/^-L/, "", $i); rpathflags = rpathflags " -R" $i}}}END{print rpathflags}'`

            scorep_sion_libs=`${scorep_sion_config_bin} ${sionconfig_febe_flag} ${sionconfig_paradigm_flag} ${sionconfig_architecture_flags} --libs | \
                              awk '{for (i=1; i<=NF; i++) {if ([index]($i, "-l") == 1){libs = libs " " $i}}}END{print libs}'`

            AC_MSG_CHECKING([for libsion $1])
            LDFLAGS="$scorep_sion_ldflags $LDFLAGS"
            LIBS="$scorep_sion_libs $LIBS"

            # commom libsion checks. for the paradigm specific ones, see below.
            AC_LINK_IFELSE([AC_LANG_PROGRAM([[
#include <stdio.h>
#include <sion.h>
#include <stddef.h>
]],[[
/* common sion functions */
sion_ensure_free_space(42, 84);
sion_feof(42);
sion_bytes_avail_in_block(42);
sion_seek(42,42,42,42);
sion_seek_fp(42,42,42,42, NULL);
sion_fwrite(NULL,42,42,42);
sion_fwrite(NULL,42,42,42);
]])],
                           [],
                           [scorep_have_sion="no"; scorep_sion_ldflags=""; scorep_sion_rpathflags=""; scorep_sion_libs=""])


            # paradigm specific libsion checks
            m4_case([$1],
[GENERIC],
[AC_LINK_IFELSE([AC_LANG_PROGRAM([[
#include <stdio.h>
#include <sion.h>
#include <stddef.h>
]],[[
/* generic sion functions */
sion_generic_paropen(NULL,NULL,NULL,NULL,NULL,NULL,  NULL,NULL,NULL, NULL,NULL,NULL,NULL,NULL);
sion_generic_parclose(NULL);
]])],
                                  [],
                                  [scorep_have_sion="no"; scorep_sion_ldflags=""; scorep_sion_rpathflags=""; scorep_sion_libs=""])],

[SERIAL],
[AC_LINK_IFELSE([AC_LANG_PROGRAM([[
#include <stdio.h>
#include <sion.h>
#include <stddef.h>
]],[[
/* serial sion functions */
sion_open(NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL);
sion_open_rank(NULL,NULL,NULL,NULL,NULL,NULL);
sion_close(42);
sion_get_locations(42,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL);
]])],
                                  [],
                                  [scorep_have_sion="no"; scorep_sion_ldflags=""; scorep_sion_rpathflags=""; scorep_sion_libs=""])],

[OMP],
[scorep_have_sion="no"; scorep_sion_ldflags=""; scorep_sion_rpathflags=""; scorep_sion_libs=""],

[MPI],
[AC_LINK_IFELSE([AC_LANG_PROGRAM([[
#include <stdio.h>
#include <sion.h>
#include <stddef.h>
#include <mpi.h>
]],[[
/* mpi sion functions */
MPI_Comm foo = MPI_COMM_WORLD;
sion_paropen_mpi(NULL,NULL,NULL,foo,&foo,NULL,NULL,NULL,NULL,NULL);
sion_parclose_mpi(42);
]])],
                                  [],
                                  [scorep_have_sion="no"; scorep_sion_ldflags=""; scorep_sion_rpathflags=""; scorep_sion_libs=""])],

[MPI_OMP],
[scorep_have_sion="no"; scorep_sion_ldflags=""; scorep_sion_rpathflags=""; scorep_sion_libs=""])


            AC_MSG_RESULT([$scorep_have_sion])
        fi

        CPPFLAGS=$cppflags_save
        LDFLAGS=$ldflags_save
        LIBS=$libs_save
        AC_LANG_POP([C])
    fi

#echo "debug: scorep_sion_cppflags=$scorep_sion_cppflags"
#echo "debug: scorep_sion_ldflags=$scorep_sion_ldflags"
#echo "debug: scorep_sion_libs=$scorep_sion_libs"

# The output of this macro
AC_SUBST([SCOREP_SION_$1_CPPFLAGS], [$scorep_sion_cppflags])
AC_SUBST([SCOREP_SION_$1_LDFLAGS],  ["$scorep_sion_ldflags $scorep_sion_rpathflags"])
AC_SUBST([SCOREP_SION_$1_LIBS],     [$scorep_sion_libs])
AC_SCOREP_COND_HAVE([SION_]$1,
                    [test "x${scorep_have_sion}" = "xyes"],
                    [Defined if libsion $1 is available.])
AFS_SUMMARY([SION $1 support], [${scorep_have_sion}])
])
