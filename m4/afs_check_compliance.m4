dnl
dnl This file is part of the Score-P software (http://www.score-p.org)
dnl
dnl Copyright (c) 2014,
dnl Technische Universitaet Dresden, Germany
dnl
dnl This software may be modified and distributed under the terms of
dnl a BSD-style license.  See the COPYING file in the package base
dnl  directory for details.
dnl

## AFS_LANG_FUNCTION( $1: PROLOG,
##                    $2: RETURN-TYPE,
##                    $3: RETURN-STMT,
##                    $4: FUNCTION-NAME,
##                    $5: FUNCTION-ARGS )
## ======================================
## Provides a stub for a function in the current language.
AC_DEFUN([AFS_LANG_FUNCTION],
[AC_LANG_SOURCE([_AC_LANG_DISPATCH([$0], _AC_LANG, $@)])])

## AFS_LANG_FUNCTION(C)( $1: PROLOG,
##                       $2: RETURN-TYPE,
##                       $3: RETURN-STMT,
##                       $4: FUNCTION-NAME,
##                       $5: FUNCTION-ARGS )
## =========================================
## Provides the stub for a C function.
m4_define([AFS_LANG_FUNCTION(C)], [dnl
$1
$2 $4$5
{
    $3;
}dnl
])])

## _AFS_CHECK_VARIANTS_REC( $1: PROLOG,
##                          $2: RETURN-TYPE,
##                          $3: RETURN-STMT,
##                          $4: FUNCTION-NAME,
##                          [$5: VARIANT-NAME, $6: VARIANT-ARGS]... )
## ==================================================================
## Recursive helper macro for AFS_CHECK_COMPLIANCE.
m4_define([_AFS_CHECK_VARIANTS_REC], [
m4_if([$#], 0, [], [$#], 1, [], [$#], 2, [], [$#], 3, [], [$#], 4, [], [$#], 5, [], [
AC_CHECK_DECLS([$4], [
    _afs_check_variants_rec_result=no
    AC_MSG_CHECKING([whether $4 is $5 variant])
    afs_save_[]_AC_LANG_ABBREV[]_werror_flag=$ac_[]_AC_LANG_ABBREV[]_werror_flag
    ac_[]_AC_LANG_ABBREV[]_werror_flag=yes
    AC_COMPILE_IFELSE([AFS_LANG_FUNCTION([$1], [$2], [$3], [$4], [$6])], [
        _afs_check_variants_rec_result=yes
    ], [
        _afs_check_variants_rec_result=no
    ])
    ac_[]_AC_LANG_ABBREV[]_werror_flag=$afs_save_[]_AC_LANG_ABBREV[]_werror_flag
    AC_MSG_RESULT([${_afs_check_variants_rec_result}])
    AS_IF([test "x${_afs_check_variants_rec_result}" = "xyes"], [
        AC_DEFINE(AS_TR_CPP([HAVE_]$4[_]$5[_VARIANT]), [1], [$4 is $5 variant])
        AC_DEFINE(AS_TR_CPP([AFS_PACKAGE_NAME[]_]$4[_PROTO_ARGS]), [$6], [Compliant prototype arguments for $4])
    ], [
        : Iterate over next given variant
        $0([$1], [$2], [$3], [$4], m4_shiftn(6, $@))
    ])
], [], [$1])
])
])

## AFS_CHECK_COMPLIANCE( $1: PROLOG,
##                       $2: RETURN-TYPE,
##                       $3: RETURN-STMT,
##                       $4: FUNCTION-NAME,
##                       $5: COMPLIANT-ARGS,
##                       [$6: VARIANT-NAME, $7: VARIANT-ARGS]... )
## ===============================================================
## Checks if the given function comply to the definition in the system.
## If not, an arbitrary number of variants are checked too. AC_DEFINEs
## HAVE_<FUNCTION-NAME>_COMPLIANT to 1, if this function is compliant,
## else it may define the first HAVE_<FUNCTION-NAME>_<VARIANT-NAME>_VARIANT
## to 1 which matches the prototype.
## If any of the prototypes matches, it also AC_DEFINEs
## AFS_PACKAGE_NAME_<FUNCTION-NAME>_PROTO_ARGS to the matching prototype.
##
## Example:
##
## AFS_CHECK_COMPLIANCE([AC_INCLUDES_DEFAULT],
##                      [int], [return 0],
##                      [mkdir], [(const char *path, mode_t mode)],
##                      [MINGW], [(const char *path)])
##
## This may result in:
##
## #define HAVE_MKDIR_COMPLIANT 1
## #define AFS_PACKAGE_NAME_MKDIR_PROTO_ARGS (const char *path, mode_t mode)
##
## or in:
##
## #define HAVE_MKDIR_MINGW_VARIANT 1
## #define AFS_PACKAGE_NAME_MKDIR_PROTO_ARGS (const char *path)
##
AC_DEFUN([AFS_CHECK_COMPLIANCE], [
AC_CHECK_DECLS([$4], [
    afs_check_compliance_result=no
    AC_MSG_CHECKING([whether $4 is standard compliant])
    afs_save_[]_AC_LANG_ABBREV[]_werror_flag=$ac_[]_AC_LANG_ABBREV[]_werror_flag
    ac_[]_AC_LANG_ABBREV[]_werror_flag=yes
    AC_COMPILE_IFELSE([AFS_LANG_FUNCTION([$1], [$2], [$3], [$4], [$5])], [
        afs_check_compliance_result=yes
    ], [
        afs_check_compliance_result=no
    ])
    ac_[]_AC_LANG_ABBREV[]_werror_flag=$afs_save_[]_AC_LANG_ABBREV[]_werror_flag
    AC_MSG_RESULT([${afs_check_compliance_result}])
    AS_IF([test "x${afs_check_compliance_result}" = "xyes"], [
        AC_DEFINE(AS_TR_CPP([HAVE_]$4[_COMPLIANT]), [1], [$4 is standard compliant])
        AC_DEFINE(AS_TR_CPP([AFS_PACKAGE_NAME[]_]$4[_PROTO_ARGS]), [$5], [Compliant prototype arguments for $4])
    ], [
        : Iterate over next given variant
        _AFS_CHECK_VARIANTS_REC([$1], [$2], [$3], [$4], m4_shiftn(5, $@))
    ])
], [], [$1])
])
