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
## Copyright (c) 2009-2013
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

## file ac_scorep_sys_detection.m4

# The purpose of platform detection is to provide reasonable default
# compilers, MPI implementations, etc. The user always has the
# possibility to override the defaults by setting environment
# variables, see section "Some influential environment variables" in
# configure --help.  On some systems there may be no reasonable
# defaults for the MPI implementation, so specify them using
# --with-mpi=... Also, on some systems there are different
# compiler-suites available which can be choosen via
# --with-compiler=(gcc|intel|sun|ibm|...)


# intended to be called by toplevel configure
AC_DEFUN_ONCE([AC_SCOREP_DETECT_PLATFORMS], [
AC_REQUIRE([AC_CANONICAL_BUILD])dnl

AC_MSG_CHECKING([for platform])
AS_IF([test "x${ac_scorep_platform}" = "x"],
    [AS_CASE([${build_os}],
         [linux*],
             [AS_IF([test "x${build_cpu}" = "xia64" && test -f /etc/sgi-release],
                      [ac_scorep_platform="altix"],
                  [test "x${build_cpu}" = "xpowerpc64" && test -d /bgl/BlueLight],
                      [ac_scorep_platform="bgl"],
                  [test "x${build_cpu}" = "xpowerpc64" && test -d /bgsys/drivers/ppcfloor/hwi],
                      [ac_scorep_platform="bgq"],
                  [test "x${build_cpu}" = "xpowerpc64" && test -d /bgsys],
                      [ac_scorep_platform="bgp"],
                  [test "x${build_cpu}" = "xx86_64" && test -d /opt/cray/xt-asyncpe],
                      [ac_scorep_platform="crayxt"],
                  [test "x${build_cpu}" = "xarmv7l"],
                      [ac_scorep_platform="arm"],
                  [ac_scorep_platform=linux])],
         [sunos* | solaris*],
              [ac_scorep_platform="solaris"],
         [darwin*],
              [ac_scorep_platform="mac"],
         [aix*],
              [ac_scorep_platform="aix"],
         [superux*],
              [ac_scorep_platform="necsx"],
         [mingw*],
              [ac_scorep_platform="mingw"],
         [ac_scorep_platform="unknown"])
     
     AS_IF([test "x${ac_scorep_platform}" = "xunknown"],
         [AC_MSG_RESULT([$ac_scorep_platform, please contact <AC_PACKAGE_BUGREPORT> if you encounter any problems.])],
         [AC_MSG_RESULT([$ac_scorep_platform (auto detected)])
          AFS_SUMMARY([Platform], [$ac_scorep_platform (auto detected)])])
    ],
    [AC_MSG_RESULT([$ac_scorep_platform (provided)])
     AFS_SUMMARY([Platform], [$ac_scorep_platform (provided)])])
])# AC_SCOREP_DETECT_PLATFORMS


# intended to be called by toplevel configure
AC_DEFUN_ONCE([AFS_CROSSCOMPILING],
[
AC_REQUIRE([AC_SCOREP_DETECT_PLATFORMS])dnl

AS_IF([test "x${ac_scorep_cross_compiling}" = "x"],
    [AS_CASE([${ac_scorep_platform}],
         [altix],   [ac_scorep_cross_compiling="no"],
         [bgl],     [ac_scorep_cross_compiling="yes"],
         [bgp],     [ac_scorep_cross_compiling="yes"],
         [bgq],     [ac_scorep_cross_compiling="yes"],
         [crayxt],  [ac_scorep_cross_compiling="yes"],
         [arm],     [ac_scorep_cross_compiling="no"],
         [linux],   [ac_scorep_cross_compiling="no"],
         [solaris], [ac_scorep_cross_compiling="no"],
         [mac],     [ac_scorep_cross_compiling="no"],
         [mingw],   [ac_scorep_cross_compiling="no"],
         [aix],     [ac_scorep_cross_compiling="no"],
         [necsx],   [ac_scorep_cross_compiling="yes"],
         [unknown], [ac_scorep_cross_compiling="no"],
         [AC_MSG_ERROR([provided platform '${ac_scorep_platform}' unknown.])])
     AFS_SUMMARY([Cross compiling], [$ac_scorep_cross_compiling (auto detected)])
    ],
    [# honor ac_scorep_cross_compiling from the commandline
     AS_IF([test ${ac_scorep_cross_compiling} != "yes" && \
            test ${ac_scorep_cross_compiling} != "no" ],
         [AC_MSG_ERROR([invalid value '${ac_scorep_cross_compiling}' for provided 'ac_scorep_cross_compiling'])])
     AFS_SUMMARY([Cross compiling], [$ac_scorep_cross_compiling (provided)])
    ])

AC_MSG_CHECKING([for cross compilation])
AC_MSG_RESULT([$ac_scorep_cross_compiling])
])# AFS_CROSSCOMPILING


# intended to be called by toplevel configure
AC_DEFUN_ONCE([AFS_SCOREP_PLATFORM_NAME],
[
AC_REQUIRE([AC_SCOREP_DETECT_PLATFORMS])dnl

AS_CASE([${ac_scorep_platform}],
    [altix],   [afs_scorep_platform_name="Altix"],
    [bgl],     [afs_scorep_platform_name="Blue Gene/L"],
    [bgp],     [afs_scorep_platform_name="Blue Gene/P"],
    [bgq],     [afs_scorep_platform_name="Blue Gene/Q"],
    [crayxt],  [afs_scorep_platform_name="Cray XT"],
    [arm],     [afs_scorep_platform_name="ARM"],
    [linux],   [afs_scorep_platform_name="Linux"],
    [solaris], [afs_scorep_platform_name="Solaris"],
    [mac],     [afs_scorep_platform_name="Mac OS X"],
    [mingw],   [afs_scorep_platform_name="MinGW"],
    [aix],     [afs_scorep_platform_name="AIX"],
    [necsx],   [afs_scorep_platform_name="NEC SX"],
    [unknown], [afs_scorep_platform_name="Unknown"],
    [AC_MSG_ERROR([provided platform '${ac_scorep_platform}' unknown.])])])
])# AFS_SCOREP_PLATFORM_NAME


# intended to be called by toplevel configure
AC_DEFUN_ONCE([AFS_SCOREP_MACHINE_NAME],
[
AC_REQUIRE([AFS_SCOREP_PLATFORM_NAME])dnl

AC_ARG_WITH([machine-name],
    [AS_HELP_STRING([--with-machine-name=<default machine name>],
        [The default machine name used in profile and trace output. We suggest using a unique name, e.g., the fully qualified domain name. If not set, a name based on the detected platform is used. Can be overridden at measurement time by setting the environment variable SCOREP_MACHINE_NAME.])],
     # action-if-given
     [AS_IF([test "x${withval}" != "xno"],
          [afs_scorep_default_machine_name="${withval}"],
          [AC_MSG_ERROR([option --without-machine-name makes no sense.])])],
     # action-if-not-given
     [afs_scorep_default_machine_name="${afs_scorep_platform_name}"])

AFS_SUMMARY([Machine name], [${afs_scorep_default_machine_name}])
])# AFS_SCOREP_MACHINE_NAME


# This macro is called by the build-backend/frontend/mpi configures only.
AC_DEFUN([AC_SCOREP_PLATFORM_SETTINGS],
[
    AC_REQUIRE([AC_CANONICAL_BUILD])

    AM_CONDITIONAL([PLATFORM_ALTIX],   [test "x${ac_scorep_platform}" = "xaltix"])
    AM_CONDITIONAL([PLATFORM_AIX],     [test "x${ac_scorep_platform}" = "xaix" && test "x${build_cpu}" = "xpowerpc"])
    AM_CONDITIONAL([PLATFORM_BGL],     [test "x${ac_scorep_platform}" = "xbgl"])
    AM_CONDITIONAL([PLATFORM_BGP],     [test "x${ac_scorep_platform}" = "xbgp"])
    AM_CONDITIONAL([PLATFORM_BGQ],     [test "x${ac_scorep_platform}" = "xbgq"])
    AM_CONDITIONAL([PLATFORM_CRAYXT],  [test "x${ac_scorep_platform}" = "xcrayxt"])
    AM_CONDITIONAL([PLATFORM_LINUX],   [test "x${ac_scorep_platform}" = "xlinux"])
    AM_CONDITIONAL([PLATFORM_SOLARIS], [test "x${ac_scorep_platform}" = "xsolaris"])
    AM_CONDITIONAL([PLATFORM_MAC],     [test "x${ac_scorep_platform}" = "xmac"])
    AM_CONDITIONAL([PLATFORM_NECSX],   [test "x${ac_scorep_platform}" = "xnecsx"])
    AM_CONDITIONAL([PLATFORM_ARM],     [test "x${ac_scorep_platform}" = "xarm"])

    AM_COND_IF([PLATFORM_ALTIX],
        [AC_DEFINE([HAVE_PLATFORM_ALTIX], [1], [Set if we are building for the ALTIX platform])])
    AM_COND_IF([PLATFORM_AIX],
        [AC_DEFINE([HAVE_PLATFORM_AIX], [1], [Set if we are building for the AIX platform])])
    AM_COND_IF([PLATFORM_BGL],
        [AC_DEFINE([HAVE_PLATFORM_BGL], [1], [Set if we are building for the BG/L platform])])
    AM_COND_IF([PLATFORM_BGP],
        [AC_DEFINE([HAVE_PLATFORM_BGP], [1], [Set if we are building for the BG/P platform])])
    AM_COND_IF([PLATFORM_BGQ],
        [AC_DEFINE([HAVE_PLATFORM_BGQ], [1], [Set if we are building for the BG/Q platform])])
    AM_COND_IF([PLATFORM_CRAYXT],
        [AC_DEFINE([HAVE_PLATFORM_CRAYXT], [1], [Set if we are building for the Cray XT platform])])
    AM_COND_IF([PLATFORM_LINUX],
        [AC_DEFINE([HAVE_PLATFORM_LINUX], [1], [Set if we are building for the Linux platform])])
    AM_COND_IF([PLATFORM_SOLARIS],
        [AC_DEFINE([HAVE_PLATFORM_SOLARIS], [1], [Set if we are building for the Solaris platform])])
    AM_COND_IF([PLATFORM_MAC],
        [AC_DEFINE([HAVE_PLATFORM_MAC], [1], [Set if we are building for the Mac platform])])
    AM_COND_IF([PLATFORM_NECSX],
        [AC_DEFINE([HAVE_PLATFORM_NECSX], [1], [Set if we are building for the NEC SX platform])])
    AM_COND_IF([PLATFORM_ARM],
        [AC_DEFINE([HAVE_PLATFORM_ARM], [1], [Set if we are building for the ARM platform])])
])

# Provides the platform and machine names as defines in the config header
AC_DEFUN([AC_SCOREP_PLATFORM_AND_MACHINE_NAMES],
[
    AC_DEFINE_UNQUOTED([SCOREP_PLATFORM_NAME],
                       ["${afs_scorep_platform_name}"],
                       [Name of the platform Score-P was built on.])
    AC_DEFINE_UNQUOTED([SCOREP_DEFAULT_MACHINE_NAME],
                       ["${afs_scorep_default_machine_name}"],
                       [Default name of the machine Score-P is running on.])
])
