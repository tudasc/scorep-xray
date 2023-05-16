## -*- mode: autoconf -*-

##
## The first part of this file is part of the Score-P software (http://www.score-p.org),
## the second part is part of autoconf
##
## Copyright (c) 2009-2011,
## RWTH Aachen University, Germany
##
## Copyright (c) 2009-2011,
## Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
##
## Copyright (c) 2009-2011, 2019, 2022,
## Technische Universitaet Dresden, Germany
##
## Copyright (c) 2009-2011,
## University of Oregon, Eugene, USA
##
## Copyright (c) 2009-2011, 2013, 2022,
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

## file build-config/m4/scorep_openmp.m4


# SCOREP_OPENMP_SUMMARY()
# -----------------------
# Trigger SCOREP_OPENMP and provide summary output
#
AC_DEFUN_ONCE([SCOREP_OPENMP_SUMMARY], [
AC_REQUIRE([SCOREP_OPENMP])
AFS_SUMMARY_PUSH
AFS_SUMMARY([C support], [$scorep_have_openmp_c_support${openmp_c_summary:+, $openmp_c_summary}])
AS_IF([test "x${scorep_have_openmp_c_support}" = xyes],
    [AS_IF([test "x${afs_cv_prog_cxx_works}" = xyes],
         [AFS_SUMMARY([C++ support], [$scorep_have_openmp_cxx_support${openmp_cxx_summary:+, $openmp_cxx_summary}])])
     AS_IF([test "x${afs_cv_prog_fc_works}" = xyes],
         [AFS_SUMMARY([Fortran support], [$scorep_have_openmp_fc_support${openmp_fc_summary:+, $openmp_fc_summary}])])])
AFS_SUMMARY_POP([OpenMP support], [${scorep_have_openmp_c_support}])
])dnl SCOREP_OPENMP_SUMMARY


# SCOREP_OPENMP()
# ---------------
# Check for OpenMP support of CC, CXX, and FC compilers. Can be
# AC_REQUIREd.
# Provides substitutions OPENMP_<LANG>FLAG, as in AC_OPENMP and
# OPENMP_<LANG>FLAGS_ALL, containing a space-separated list of flags
# that activate OpenMP.
# Provides SCOREP_OPENMP_FLAGS_ALL, containing a duplicate-free,
# comma-separated list of "-quoted flags that activate OpenMP.
# Provides the AM conditional HAVE_OPENMP_<LANG>_SUPPORT.
# Sets scorep_have_openmp_(c|cxx|fc)_support=(yes|no).
# Computes _OPENMP and provides it via scorep_openmp_(c|cxx)_version.
# Provides openmp_<lang>_summary.
#
AC_DEFUN_ONCE([SCOREP_OPENMP], [
AC_REQUIRE([SCOREP_COMPUTENODE_CC])dnl
AC_REQUIRE([SCOREP_COMPUTENODE_CXX])dnl
dnl SCOREP_COMPUTENODE_FC can't be required
dnl
scorep_openmp_flags_all=""
AC_LANG_PUSH([C])
_CHECK_OPENMP_SUPPORT
AC_LANG_POP([C])
# C support needed for building the adapter. If that isn't possible,
# no need to check for other languages.
AS_IF([test "x${scorep_have_openmp_c_support}" = xyes],
    [AS_IF([test "x${afs_cv_prog_cxx_works}" = xyes],
         [AC_LANG_PUSH([C++])
          _CHECK_OPENMP_SUPPORT
          AC_LANG_POP([C++])])
     AS_IF([test "x${afs_cv_prog_fc_works}" = xyes],
         [AC_LANG_PUSH([Fortran])
          _CHECK_OPENMP_SUPPORT
          AC_LANG_POP([Fortran])])])
AC_SUBST([SCOREP_OPENMP_FLAGS_ALL], ["${scorep_openmp_flags_all}"])
])dnl SCOREP_OPENMP


# _CHECK_OPENMP_SUPPORT()
# -----------------------
# Do the real check _SCOREP_OPENMP and compute _OPENMP
#
m4_define([_CHECK_OPENMP_SUPPORT], [
_AC_LANG_PREFIX[]FLAGS_save="$_AC_LANG_PREFIX[]FLAGS"
# Unset FLAGS temporarily because Cray compilers ignore OpenMP flags
# if -g is (automagically) set.
_AC_LANG_PREFIX[]FLAGS=
dnl
# the real OpenMP check
_SCOREP_OPENMP
dnl
# compute _OPENMP, avaialble in scorep_openmp_(c|cxx|)_version
_AC_LANG_PREFIX[]FLAGS="$OPENMP_[]_AC_LANG_PREFIX[]FLAGS"
AS_IF([test "x${openmp_[]_AC_LANG_ABBREV[]flag}" != x],
    [m4_case(_AC_CC,
         [F77], [],
         [FC],  [],
         [dnl AC_COMPUTE_INT not supported for F77 and FC
          AC_COMPUTE_INT([scorep_openmp_[]_AC_LANG_ABBREV[]_version], [_OPENMP])])])
dnl
_AC_LANG_PREFIX[]FLAGS="$_AC_LANG_PREFIX[]FLAGS_save"
AC_MSG_CHECKING([for OpenMP ]_AC_LANG[ flags])
AC_MSG_RESULT([$(echo ${openmp_[]_AC_LANG_ABBREV[]flags_all:-not supported} | sed 's/"//g')])
])dnl _CHECK_OPENMP_SUPPORT


dnl ============================================================================


# The remainder is based on AC_OPENMP from autoconf 2.71:

# This file is part of Autoconf.                        -*- Autoconf -*-
# Programming languages support.
# Copyright (C) 2001-2017, 2020-2021 Free Software Foundation, Inc.

# This file is part of Autoconf.  This program is free
# software; you can redistribute it and/or modify it under the
# terms of the GNU General Public License as published by the
# Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# Under Section 7 of GPL version 3, you are granted additional
# permissions described in the Autoconf Configure Script Exception,
# version 3.0, as published by the Free Software Foundation.
#
# You should have received a copy of the GNU General Public License
# and a copy of the Autoconf Configure Script Exception along with
# this program; see the files COPYINGv3 and COPYING.EXCEPTION
# respectively.  If not, see <https://www.gnu.org/licenses/>.

# Written by David MacKenzie, with help from
# Akim Demaille, Paul Eggert,
# Franc,ois Pinard, Karl Berry, Richard Pixley, Ian Lance Taylor,
# Roland McGrath, Noah Friedman, david d zuhn, and many others.

# AC_OPENMP and _AC_OPENMP_SAFE_WD from autoconf-2.71. AC_OPENMP
# renamed to _SCOREP_OPENMP and modified by Christian Feld
# <c.feld@fz-juelich.de> to fit the needs of Score-P (score-p.org) and
# related projects.

# _SCOREP_OPENMP()
# ----------------
# Based on autoconf 2.71 AC_OPENMP
#
# Check which compiler option need to be passed to the compiler to
# support OpenMP. Set the (OPENMP_C|CXX|F|FC)FLAGS variable to these
# options. In addition, find further options that would activate
# OpenMP as well; make available via
# SCOREP_OPENMP_(C|CXX|F|FC)FLAGS_ALL.
# TODO, document which variables get defined/set

# The options are necessary at compile time (so the #pragmas are
# understood) and at link time (so the appropriate library is linked
# with).  This macro assumes that a explicit option is needed to
# activate OpenMP.

#
# For each candidate option, we do a compile test first, then a link test;
# if the compile test succeeds but the link test fails, that means we have
# found the correct option but it doesn't work because the libraries are
# broken.  (This can happen, for instance, with SunPRO C and a bad combination
# of operating system patches.)
#
# Several of the options in our candidate list can be misinterpreted by
# compilers that don't use them to activate OpenMP support; for example,
# many compilers understand "-openmp" to mean "write output to a file
# named 'penmp'" rather than "enable OpenMP".  We can't completely avoid
# the possibility of clobbering files named 'penmp' or 'mp' in configure's
# working directory; therefore, this macro will bomb out if any such file
# already exists when it's invoked.
m4_define([_SCOREP_OPENMP],
[AC_REQUIRE([_SCOREP_OPENMP_SAFE_WD])]dnl
[
scorep_have_openmp_[]_AC_LANG_ABBREV[]_support=no
AS_UNSET([openmp_[]_AC_LANG_ABBREV[]flag])
AS_UNSET([openmp_[]_AC_LANG_ABBREV[]flags_all])
AS_UNSET([openmp_[]_AC_LANG_ABBREV[]_summary])

dnl Try these flags:
dnl   (on by default)      '' [scorep: for instrumentation we want an explicit flag, thus omit '']
dnl   GCC >= 4.2             -fopenmp
dnl   SunPRO C               -xopenmp
dnl   Intel C                -openmp|-fopenmp|-qopenmp|-fiopenmp
dnl   SGI C, PGI C           -mp
dnl   Tru64 Compaq C         -omp
dnl   IBM XL C (AIX, Linux)  -qsmp=omp|-qsmp=auto|-qsmp=noauto
dnl   Cray CCE               -homp
dnl   NEC SX                 -Popenmp
dnl   Lahey Fortran (Linux)  --openmp
dnl   Fujitsu                -Kopenmp
for ac_option in -fopenmp -xopenmp -fiopenmp -qopenmp -openmp -mp -omp -qsmp=omp -qsmp=auto -qsmp=noauto -homp \
                 -Popenmp --openmp -Kopenmp; do

    ac_save_[]_AC_LANG_PREFIX[]FLAGS=$[]_AC_LANG_PREFIX[]FLAGS
    _AC_LANG_PREFIX[]FLAGS="$[]_AC_LANG_PREFIX[]FLAGS $ac_option"
    AC_COMPILE_IFELSE([_AC_LANG_OPENMP],
        [AC_LINK_IFELSE([_AC_LANG_OPENMP],
            [scorep_have_openmp_[]_AC_LANG_ABBREV[]_support=yes
             openmp_[]_AC_LANG_ABBREV[]flag="${openmp_[]_AC_LANG_ABBREV[]flag:-$ac_option}"
             openmp_[]_AC_LANG_ABBREV[]flags_all="${openmp_[]_AC_LANG_ABBREV[]flags_all}${openmp_[]_AC_LANG_ABBREV[]flags_all:+ }$ac_option"])])
    _AC_LANG_PREFIX[]FLAGS=$ac_save_[]_AC_LANG_PREFIX[]FLAGS

done

dnl _SCOREP_OPENMP_SAFE_WD checked that these files did not exist before we
dnl started probing for OpenMP support, so if they exist now, they were
dnl created by the probe loop and it's safe to delete them.
rm -f penmp mp

# OPENMP_(C|CXX|F|FC)FLAGS as in AC_OPENMP
AC_SUBST([OPENMP_]_AC_LANG_PREFIX[FLAGS], ["$openmp_[]_AC_LANG_ABBREV[]flag"])
AC_SUBST([SCOREP_OPENMP_]_AC_LANG_PREFIX[FLAGS_ALL], ["$openmp_[]_AC_LANG_ABBREV[]flags_all"])
AS_IF([test "x${scorep_have_openmp_[]_AC_LANG_ABBREV[]_support}" = xyes],
    [openmp_[]_AC_LANG_ABBREV[]_summary="recognizing $openmp_[]_AC_LANG_ABBREV[]flags_all"
     # should be HAVE_SCOREP_OPENMP_]_AC_LANG_PREFIX[_SUPPORT
     AFS_AM_CONDITIONAL([HAVE_OPENMP_]_AC_LANG_PREFIX[_SUPPORT], [test 1 -eq 1], [false])
     # add flags to scorep_openmp_flags_all, omit duplicates
     for flag in ${openmp_[]_AC_LANG_ABBREV[]flags_all}; do
         case ', '"$scorep_openmp_flags_all"', ' in
             (*', "'"$flag"'", '*) : # ignore duplicates
                 ;;
             (*)
                 scorep_openmp_flags_all="${scorep_openmp_flags_all}${scorep_openmp_flags_all:+, }"'"'"$flag"'"'
                 ;;
         esac
     done],
    [openmp_[]_AC_LANG_ABBREV[]_summary="cannot be activated by compiler flag"])
])dnl _SCOREP_OPENMP


# _SCOREP_OPENMP_SAFE_WD
# ----------------------
# AC_REQUIREd by _SCOREP_OPENMP.  Checks both at autoconf time and at
# configure time for files that _SCOREP_OPENMP clobbers.
m4_defun([_SCOREP_OPENMP_SAFE_WD],
[m4_syscmd([test ! -e penmp && test ! -e mp])]dnl
[m4_if(sysval, [0], [], [m4_fatal(m4_normalize(
  [_SCOREP_OPENMP clobbers files named 'mp' and 'penmp'.
   To use _SCOREP_OPENMP you must not have either of these files
   at the top level of your source tree.]))])]dnl
[if test -e penmp || test -e mp; then
  AC_MSG_ERROR(m4_normalize(
    [_SCOREP@&t@_OPENMP clobbers files named 'mp' and 'penmp'.
     Aborting configure because one of these files already exists.]))
fi])
