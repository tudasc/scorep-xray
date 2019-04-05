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

## file build-config/m4/scorep_chcek_symbols.m4

# _SCOREP_CHECK_SYMBOLS_BISECT_BODY
# ---------------------------------
# Shell function body for _SCOREP_CHECK_SYMBOLS_BISECT.
#
m4_define([_SCOREP_CHECK_SYMBOLS_BISECT_BODY], [
    AS_LINENO_PUSH([$[]1])
    if test $(($[]5 - $[]4 + 1)) -eq 0; then
        return
    fi

    cat <<_ACEOF >confsymbolstest.h
#ifdef __cplusplus
extern "C" {
#endif
_ACEOF

    sed -n -e "$[]4,$[]5 p" confsymbols >confsymbols.this
    while read symbol define; do
        cat <<_ACEOF
#undef $symbol
char $symbol ();
_ACEOF
    done <confsymbols.this >>confsymbolstest.h

    cat <<_ACEOF >>confsymbolstest.h
#ifdef __cplusplus
}
#endif
_ACEOF

    cat <<_ACEOF >confsymbolstest.c
return 0
_ACEOF
    while read symbol define; do
        cat <<_ACEOF
    | $symbol()
_ACEOF
    done <confsymbols.this >>confsymbolstest.c

    AC_LINK_IFELSE([AC_LANG_PROGRAM([[
#include "confsymbolstest.h"
]], [[
#include "confsymbolstest.c"
]])], [
        # we successfully linked all symbols, add them to the confdefs.h
        while read symbol define; do
            AC_MSG_CHECKING([for $symbol in ]$[]2)
            AC_MSG_RESULT([yes])

            AC_DEFINE_UNQUOTED([$define], [1])

            AS_IF([test -n "$[]3"],
                [AS_VAR_APPEND($[]3, [" $symbol"])])

        done <confsymbols.this

    ], [

        # if this was only one symbol, than we can be sure, that this does not exists

        if test $(($[]5 - $[]4 + 1)) -eq 1; then

            while read symbol define; do

                AC_MSG_CHECKING([for $symbol in ]$[]2)
                AC_MSG_RESULT([no])

                AC_DEFINE_UNQUOTED([$define], [0])

            done <confsymbols.this

        else # #symbols != 1

            # we need to bisect into lower and upper

            mid=$(($[]4 + ($[]5 - $[]4 + 1) / 2))

            _SCOREP_CHECK_SYMBOLS_BISECT($[]2, $[]3, $[]4, [$(($mid - 1))])
            _SCOREP_CHECK_SYMBOLS_BISECT($[]2, $[]3, [$mid], $[]5)

        fi # #symbols != 1

    ])
    AS_LINENO_POP
])# _SCOREP_CHECK_SYMBOLS_BISECT_BODY


# _SCOREP_CHECK_SYMBOLS_BISECT(TAG, FOUND-SYMBOL-VAR[, START, END])
# -------------------------------------------------
AC_DEFUN([_SCOREP_CHECK_SYMBOLS_BISECT], [
    AC_REQUIRE_SHELL_FN([_scorep_check_symbols_bisect],
        [AS_FUNCTION_DESCRIBE([_scorep_check_symbols_bisect],
            [LINENO TAG FOUND-SYMBOL-VAR START END],
            [The symbol check.])],
        [$0_BODY])]dnl
    [_scorep_check_symbols_bisect "$LINENO" "$1" "$2" m4_default([$3], [1]) m4_default([$4], [$(cat confsymbols | wc -l)])
])# _SCOREP_CHECK_SYMBOLS_BISECT

# SCOREP_CHECK_SYMBOLS( TAG, [PREFIX], [FOUND-SYMBOL-VARIABLE], SYMBOL-LIST )
# -------------------------------------------------
# Checks all symbols in SYMBOL-LIST (a m4 comma-separated list) for existence.
# For each symbol a AC_DEFINE([HAVE_<TAG>_SYMBOL_<PREFIX><SYMBOL>]) is created
# with the result. FOUND-SYMBOL-VARIABLE is the name for a shell variable, which
# will hold all found symbols separated with space. <PREFIX> can be used to
# automatically prefix all symbols, e.g., 'P' for MPI.
#
AC_DEFUN([SCOREP_CHECK_SYMBOLS], [
m4_pushdef([_scorep_check_symbols_tag],
    m4_bpatsubst(m4_normalize($1), [[^A-Za-z0-9 ]+], []))dnl

(
m4_foreach([func],
           [$4],
           [AH_TEMPLATE(AS_TR_CPP([HAVE_]_scorep_check_symbols_tag[_SYMBOL_]$2[]func),
                [Define to 1 if ]$1[ has the ]$2[]func[ symbol, 0 if not])dnl
printf "%s %s\n" $2[]func AS_TR_CPP([HAVE_]_scorep_check_symbols_tag[_SYMBOL_]$2[]func);
])dnl
) >confsymbols
_SCOREP_CHECK_SYMBOLS_BISECT($1, $3)
rm -f confsymbols confsymbols.this confsymbolstest.h confsymbolstest.c
m4_popdef([_scorep_check_symbols_tag])
])# SCOREP_CHECK_SYMBOLS
