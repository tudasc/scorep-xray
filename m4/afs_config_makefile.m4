dnl
dnl This file is part of the Score-P software (http://www.score-p.org)
dnl
dnl Copyright (c) 2014,
dnl Technische Universitaet Dresden, Germany
dnl
dnl This software may be modified and distributed under the terms of
dnl a BSD-style license.  See the COPYING file in the package base
dnl directory for details.
dnl

## AFS_SET_SILENT_RULE_PREFIX( PREFIX )
## ======================================
## Prepends a prefix to all preprocess/compile/link silent rule builds.
AC_DEFUN([AFS_SET_SILENT_RULE_PREFIX], [
m4_pushdef([_afs_srp], m4_toupper($1))dnl
AC_SUBST([AFS_SRP], _afs_srp)
m4_popdef([_afs_srp])dnl
])

## AFS_CONFIG_MAKEFILE
## ======================================
## Calls AC_CONFIG_FILES([Makefile]) and includes modification code to prepend
## silent rule prefix set with AFS_SET_SILENT_RULE_PREFIX.
AC_DEFUN([AFS_CONFIG_MAKEFILE],
AC_CONFIG_FILES([Makefile], [
sed -e '
s/_0 = @echo "  \(YACC\|GEN\|LEX\)/_0 = @AFS_SRP="`echo $(AFS_SRP) | sed "s,., ,g"`"; echo "  \1$$$AFS_SRP/
t
s/_0 = @echo "  \(@<:@^ @:>@\+\)/_0 = @echo "  $(AFS_SRP)\1$/' \
    Makefile >Makefile.afs && mv -f Makefile.afs Makefile || rm -f Makefile.afs
])
)
