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
## Copyright (c) 2009-2013, 2019,
## Technische Universitaet Dresden, Germany
##
## Copyright (c) 2009-2011,
## University of Oregon, Eugene, USA
##
## Copyright (c) 2009-2015,
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
    [crayxe],  [afs_scorep_platform_name="Cray XE"],
    [crayxk],  [afs_scorep_platform_name="Cray XK"],
    [crayxc],  [afs_scorep_platform_name="Cray XC"],
    [arm],     [afs_scorep_platform_name="ARM"],
    [k],       [afs_scorep_platform_name="K"],
    [fx10],    [afs_scorep_platform_name="FX10"],
    [fx100],   [afs_scorep_platform_name="FX100"],
    [linux],   [afs_scorep_platform_name="Linux"],
    [solaris], [afs_scorep_platform_name="Solaris"],
    [mac],     [afs_scorep_platform_name="macOS"],
    [mingw],   [afs_scorep_platform_name="MinGW"],
    [aix],     [afs_scorep_platform_name="AIX"],
    [necsx],   [afs_scorep_platform_name="NEC SX"],
    [mic],     [afs_scorep_platform_name="Xeon Phi"],
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
