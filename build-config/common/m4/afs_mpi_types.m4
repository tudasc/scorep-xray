## -*- mode: autoconf -*-

##
## This file is part of the Score-P software (http://www.score-p.org)
##
## Copyright (c) 2013,
## Forschungszentrum Juelich GmbH, Germany
##
## Copyright (c) 2014,
## Technische Universitaet Dresden, Germany
##
## This software may be modified and distributed under the terms of
## a BSD-style license.  See the COPYING file in the package base
## directory for details.
##


# AFS_MPI_DATATYPE_<PREFIX><NBITS>_T
# ----------------------------------
# This set of macros determines which MPI C datatype best matches the C
# fixed-width integer type <PREFIX><NBITS>_t. If no suitable MPI datatype
# can be found, the configure script is aborted.
#
# List of configure variables set:
#  `afs_cv_mpi_datatype_<PREFIX><NBITS>_t`::  Matching MPI datatype
#
# List of provided config header defines:
#  `<AFS_PACKAGE>_MPI_<PREFIX><NBITS>_T`::    Matching MPI datatype (unquoted)
#
AC_DEFUN([AFS_MPI_DATATYPE_INT8_T],  [_AFS_MPI_DATATYPE_INTEGER([int],   [8])])
AC_DEFUN([AFS_MPI_DATATYPE_INT16_T], [_AFS_MPI_DATATYPE_INTEGER([int],  [16])])
AC_DEFUN([AFS_MPI_DATATYPE_INT32_T], [_AFS_MPI_DATATYPE_INTEGER([int],  [32])])
AC_DEFUN([AFS_MPI_DATATYPE_INT64_T], [_AFS_MPI_DATATYPE_INTEGER([int],  [64])])
AC_DEFUN([AFS_MPI_DATATYPE_UINT8_T], [_AFS_MPI_DATATYPE_INTEGER([uint],  [8])])
AC_DEFUN([AFS_MPI_DATATYPE_UINT16_T],[_AFS_MPI_DATATYPE_INTEGER([uint], [16])])
AC_DEFUN([AFS_MPI_DATATYPE_UINT32_T],[_AFS_MPI_DATATYPE_INTEGER([uint], [32])])
AC_DEFUN([AFS_MPI_DATATYPE_UINT64_T],[_AFS_MPI_DATATYPE_INTEGER([uint], [64])])


dnl ----------------------------------------------------------------------------


# _AFS_MPI_DATATYPE_INTEGER(PREFIX, NBITS)
# ----------------------------------------
# Generic implementation of the AFS_MPI_DATATYPE_<PREFIX><NBITS>_T macros.
#
# Valid values for PREFIX are 'int' and 'uint'; valid values for NBITS are
# 8, 16, 32, and 64.
#
AC_DEFUN([_AFS_MPI_DATATYPE_INTEGER], [
    m4_ifblank([$1], [m4_fatal([Macro requires exactly two arguments])])
    m4_case([$1], [int], [], [uint], [],
        [m4_fatal([first argument must be either 'int' or 'uint'])])
    m4_ifblank([$2], [m4_fatal([Macro requires exactly two arguments])])
    m4_case([$2], [8], [], [16], [], [32], [], [64], [],
        [m4_fatal([second argument must be one of 8, 16, 32 or 64])])
    m4_ifnblank([$3], [m4_fatal([Macro requires exactly two arguments])])

    AC_CACHE_CHECK([for $1$2_t-compatible MPI datatype],
        [afs_cv_mpi_datatype_$1$2_t],
        [AS_VAR_SET([afs_cv_mpi_datatype_$1$2_t], [none])
         _AFS_MPI_CHECK_DATATYPE(
            [MPI_[]m4_toupper([$1])$2_T],
            [afs_cv_mpi_datatype_$1$2_t])
         AS_IF([test "x$afs_cv_mpi_datatype_$1$2_t" = "xnone"],
            [m4_do(_AFS_MPI_TYPESUBST_[]m4_toupper([$1])([$2]))
             _afs_tmp_var=`AS_ECHO(["$_afs_mpi_typesubst_$1$2_t"]) \
                | LC_ALL=C tr '[a-z ]' '[A-Z_]'`
             _AFS_MPI_CHECK_DATATYPE(
                [MPI_$_afs_tmp_var],
                [afs_cv_mpi_datatype_$1$2_t])
            ])
        ])
    AS_IF([test "x$afs_cv_mpi_datatype_$1$2_t" = "xnone"],
        [AC_MSG_ERROR([No suitable MPI datatype found for $1$2_t.])],
        [AC_DEFINE_UNQUOTED(AFS_PACKAGE_NAME[]_MPI_[]m4_toupper([$1])$2_T,
            [$afs_cv_mpi_datatype_$1$2_t],
            [MPI datatype compatible with $1$2_t])
        ])
])# _AFS_MPI_DATATYPE_INTEGER


dnl ----------------------------------------------------------------------------


# _AFS_MPI_CHECK_DATATYPE(TYPE, VAR)
# ----------------------------------
# Sets VAR to TYPE if the MPI C datatype TYPE is available.
#
AC_DEFUN([_AFS_MPI_CHECK_DATATYPE], [
    m4_ifblank([$1], [m4_fatal([Macro requires exactly two arguments])])
    m4_ifblank([$2], [m4_fatal([Macro requires exactly two arguments])])
    m4_ifnblank([$3], [m4_fatal([Macro requires exactly two arguments])])

    AC_COMPILE_IFELSE(
        [AC_LANG_PROGRAM(
            [#include <mpi.h>],
            [MPI_Datatype dt = $1;])],
        [AS_VAR_SET([$2], [$1])])
])# _AFS_MPI_CHECK_DATATYPE


dnl ----------------------------------------------------------------------------


# The following helper macros are based on the type check macros provided
# by autoconf (see types.m4), which comes with the following license:
#
# Copyright (C) 2000, 2001, 2002, 2004, 2005, 2006, 2007, 2008, 2009,
# 2010 Free Software Foundation, Inc.
#
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
# respectively.  If not, see <http://www.gnu.org/licenses/>.


# _AFS_MPI_TYPESUBST_INT_BODY
# ---------------------------
# Shell function body for _AFS_MPI_TYPESUBST_INT.
#
m4_define([_AFS_MPI_TYPESUBST_INT_BODY], [
    AS_LINENO_PUSH([$[]1])
    AS_VAR_SET([$[]3], [no])
    # Order is important - never check a type that is potentially smaller
    # than half of the expected target width.
    for afs_type in 'int' 'long' 'long long' 'short' 'signed char'; do
        AC_COMPILE_IFELSE(
            [AC_LANG_BOOL_COMPILE_TRY(
                [AC_INCLUDES_DEFAULT
                 enum { N = $[]2 / 2 - 1 };],
                [0 < ($afs_type) ((((($afs_type) 1 << N) << N) - 1) * 2 + 1)])],
            [AC_COMPILE_IFELSE(
                [AC_LANG_BOOL_COMPILE_TRY(
                    [AC_INCLUDES_DEFAULT
                     enum { N = $[]2 / 2 - 1 };],
                    [($afs_type) ((((($afs_type) 1 << N) << N) - 1) * 2 + 1)
                     < ($afs_type) ((((($afs_type) 1 << N) << N) - 1) * 2 + 2)])],
                [],
                [AS_VAR_SET([$[]3], [$afs_type])])])
        AS_VAR_IF([$[]3], [no], [], [break])
    done
    AS_LINENO_POP
])# _AFS_MPI_TYPESUBST_INT_BODY


# _AFS_MPI_TYPESUBST_INT(NBITS)
# -----------------------------
# Sets the variable _afs_mpi_typesubst_int<NBITS>_t to a replacement signed
# integer type of width exactly NBITS bits, or to 'no' if no suitable type
# could be found.
#
AC_DEFUN([_AFS_MPI_TYPESUBST_INT], [
    AC_REQUIRE_SHELL_FN([afs_fn_mpi_find_intX_t],
        [AS_FUNCTION_DESCRIBE([afs_fn_mpi_find_intX_t],
            [LINENO BITS VAR],
            [Finds a signed integer type with width BITS, setting cache
             variable VAR accordingly.])],
        [$0_BODY])]dnl
    [afs_fn_mpi_find_intX_t "$LINENO" "$1" "_afs_mpi_typesubst_int$1_t"
])# _AFS_MPI_TYPESUBST_INT


# _AFS_MPI_TYPESUBST_UINT_BODY
# ----------------------------
# Shell function body for _AFS_MPI_TYPESUBST_UINT.
#
m4_define([_AFS_MPI_TYPESUBST_UINT_BODY], [
    AS_LINENO_PUSH([$[]1])
    AS_VAR_SET([$[]3], [no])
    # Order is important - never check a type that is potentially smaller
    # than half of the expected target width.
    for afs_type in 'unsigned' 'unsigned long' 'unsigned long long' \
        'unsigned short' 'unsigned char'; do
        AC_COMPILE_IFELSE(
            [AC_LANG_BOOL_COMPILE_TRY(
	        [AC_INCLUDES_DEFAULT],
                [(($afs_type) -1 >> ($[]2 / 2 - 1)) >> ($[]2 / 2 - 1) == 3])],
            [AS_VAR_SET([$[]3], [$afs_type])])
        AS_VAR_IF([$[]3], [no], [], [break])
    done
    AS_LINENO_POP
])# _AFS_MPI_TYPESUBST_UINT_BODY


# _AFS_MPI_TYPESUBST_UINT(NBITS)
# ----------------------------
# Sets the variable _afs_mpi_typesubst_uint<NBITS>_t to a replacement unsigned
# integer type of width exactly NBITS bits, or to 'no' if no suitable type
# could be found.
#
AC_DEFUN([_AFS_MPI_TYPESUBST_UINT], [
    AC_REQUIRE_SHELL_FN([afs_fn_mpi_find_uintX_t],
        [AS_FUNCTION_DESCRIBE([afs_fn_mpi_find_uintX_t],
            [LINENO BITS VAR],
            [Finds an unsigned integer type with width BITS, setting cache
             variable VAR accordingly.])],
        [$0_BODY])]dnl
    [afs_fn_mpi_find_uintX_t "$LINENO" "$1" "_afs_mpi_typesubst_uint$1_t"
])# _AFS_MPI_TYPESUBST_UINT
