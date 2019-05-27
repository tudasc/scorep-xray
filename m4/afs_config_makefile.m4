dnl
dnl This file is part of the Score-P software (http://www.score-p.org)
dnl
dnl Copyright (c) 2014, 2019,
dnl Technische Universitaet Dresden, Germany
dnl
dnl This software may be modified and distributed under the terms of
dnl a BSD-style license.  See the COPYING file in the package base
dnl directory for details.
dnl

## AFS_SET_SILENT_RULE_PREFIX( PREFIX )
## ======================================
## Prepends a prefix to all preprocess/compile/link silent rule builds.
## Prefix can only have alphanumeric, underscore, and space characters.
AC_DEFUN([AFS_SET_SILENT_RULE_PREFIX], [
m4_pushdef([_afs_srp], m4_toupper($1))dnl
AFS_SRP="_afs_srp"
AFS_SRP_PADDING="m4_bpatsubst(_afs_srp, [[A-Z0-9_]], [ ])"
m4_popdef([_afs_srp])dnl
])

## AFS_CONFIG_MAKEFILE
## ======================================
## Calls AC_CONFIG_FILES([Makefile]) and includes modification code to prepend
## silent rule prefix set with AFS_SET_SILENT_RULE_PREFIX.
AC_DEFUN([AFS_CONFIG_MAKEFILE],
AC_CONFIG_FILES([Makefile], [
sed -e '
s/^\(am__v_\(GEN\)_0 = @echo "  \)\2/\1\2'"$AFS_SRP_PADDING"'/
s/^\(am__v_\(YACC\)_0 = @echo "  \)\2/\1\2'"$AFS_SRP_PADDING"'/
s/^\(am__v_\(LEX\)_0 = @echo "  \)\2/\1\2'"$AFS_SRP_PADDING"'/
t
s/^\(am__v_\(@<:@^ @:>@*\)_0 = @echo "  \)\2/\1'"$AFS_SRP"'\2/' \
    Makefile >Makefile.afs && mv -f Makefile.afs Makefile || rm -f Makefile.afs
], [
AFS_SRP='$AFS_SRP'
AFS_SRP_PADDING='$AFS_SRP_PADDING'
])
)
