## -*- mode: autoconf -*-

##
## This file is part of the Score-P software (http://www.score-p.org)
##
## Copyright (c) 2017,
## Technische Universitaet Dresden, Germany
##
## This software may be modified and distributed under the terms of
## a BSD-style license.  See the COPYING file in the package base
## directory for details.
##

## SCOREP_LD_WRAP_FILE( $1: NAME,
##                      [$2: SYMBOL-VARIABLE-NAME] )
## ===============================================================
## Creates, out of a symbol list in the shell variable SYMBOL-VARIABLE-NAME
## (defaults to 'scorep_NAME_wrap_symbols'), a file with linker flags to wrap
## these symbols.  And a file suitable when using nvcc as the linker driver.
## The file '../share/NAME.nvcc.wrap' must be added to DISTCLEANFILES.
AC_DEFUN([SCOREP_LD_WRAP_FILE], [
    m4_pushdef([_scorep_ld_wrap_file_symvar], m4_default([$2], [scorep_]$1[_wrap_symbols]))
    AC_CONFIG_FILES([../share/$1.wrap:../share/wrap.in],
        [for first in $]_scorep_ld_wrap_file_symvar[
         do
             printf '\x2d\x2dundefined __wrap_%s\n' $first
             break
         done >../share/$1.wrap
         printf '\x2dwrap %s\n' $]_scorep_ld_wrap_file_symvar[ >>../share/$1.wrap
         $SED -e 's/^-@<:@^ @:>@* /-Xlinker &-Xlinker /' ../share/$1.wrap >../share/$1.nvcc.wrap],
        _scorep_ld_wrap_file_symvar[='$]_scorep_ld_wrap_file_symvar['])
    m4_popdef([_scorep_ld_wrap_file_symvar])
])
