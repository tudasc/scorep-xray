## -*- mode: autoconf -*-

##
## This file is part of the Score-P software ecosystem (http://www.score-p.org)
##
## Copyright (c) 2021,
## Forschungszentrum Juelich GmbH, Germany
##
## This software may be modified and distributed under the terms of
## a BSD-style license.  See the COPYING file in the package base
## directory for details.
##


# AFS_LTLINK_LA_IFELSE( PROGRAM, LIBRARY, [ACTION-IF-TRUE], [ACTION-IF-FALSE] )
# -----------------------------------------------------------------------------
# Try to libtool-link LIBRARY into libtool archive and PROGRAM against
# the generated libtool archive. Based on _AC_LINK_IFELSE.
# Uses LTLDFLAGS and LTLIBS for generating LIBRARY, LDFLAGS and LIBS
# for generating PROGRAM, as well as CPPFLAGS for both.
#
AC_DEFUN([AFS_LTLINK_LA_IFELSE], [dnl
AC_REQUIRE([LT_OUTPUT])dnl
AC_REQUIRE_SHELL_FN([afs_fn_]_AC_LANG_ABBREV[_try_ltlink_la],
  [AS_FUNCTION_DESCRIBE([afs_fn_]_AC_LANG_ABBREV[_try_ltlink_la], [LINENO],
    [Using libtool, create libconftest.la from libconftest.$ac_ext, then link this into conftest.$ac_ext binary. Return whether this succeeded.])],
  [_$0_BODY])]dnl
[m4_ifvaln([$2], [AC_LANG_CONFTEST([$2])])]dnl
mv conftest.$ac_ext libconftest.$ac_ext
[m4_ifvaln([$1], [AC_LANG_CONFTEST([$1])])]dnl
[AS_IF([afs_fn_[]_AC_LANG_ABBREV[]_try_ltlink_la "$LINENO"], [$3], [$4])
rm -f conftest.err libconftest.lo libconftest.la .libs/libconftest* libconftest.$ac_ext .libs/conftest* conftest$ac_exeext conftest.$ac_objext conftest.$ac_ext[]dnl
])# AFS_LTLINK_LA_IFELSE


# _AFS_LTLINK_LA_IFELSE_BODY()
# ----------------------------
# Shell function body for AFS_LTLINK_LA_IFELSE. Based on
# _AC_LINK_IFELSE_BODY.
#
m4_define([_AFS_LTLINK_LA_IFELSE_BODY],
[  AS_LINENO_PUSH([$[]1])
  afs_[]_AC_LANG_ABBREV[]_ltcompile='./libtool --tag=_AC_CC --mode=compile $_AC_CC $_AC_LANG_PREFIX[]FLAGS $CPPFLAGS -c -o libconftest.lo libconftest.$ac_ext >&AS_MESSAGE_LOG_FD'
  afs_[]_AC_LANG_ABBREV[]_ltlinkla='./libtool --tag=_AC_CC --mode=link $_AC_CC $_AC_LANG_PREFIX[]FLAGS $CPPFLAGS $LTLDFLAGS -o libconftest.la -rpath `pwd` libconftest.lo $LTLIBS >&AS_MESSAGE_LOG_FD'
  afs_[]_AC_LANG_ABBREV[]_compile='$[]_AC_CC $_AC_LANG_PREFIX[]FLAGS $CPPFLAGS -c conftest.$ac_ext -o conftest.$ac_objext'
  afs_[]_AC_LANG_ABBREV[]_ltlink='./libtool --tag=_AC_CC --mode=link $_AC_CC $_AC_LANG_PREFIX[]FLAGS $CPPFLAGS $LDFLAGS -o conftest$ac_exeext conftest.$ac_objext libconftest.la $LIBS >&AS_MESSAGE_LOG_FD'
  rm -f .libs/libconftest.$ac_objext libconftest.lo libconftest.la conftest.$ac_objext .libs/conftest$ac_exeext conftest$ac_exeext
  AS_IF([_AC_DO_STDERR($afs_[]_AC_LANG_ABBREV[]_ltcompile) && {
         test -z "$ac_[]_AC_LANG_ABBREV[]_werror_flag" ||
         test ! -s conftest.err
       } && test -s libconftest.lo && _AC_DO_STDERR($afs_[]_AC_LANG_ABBREV[]_ltlinkla) && {
         test -z "$ac_[]_AC_LANG_ABBREV[]_werror_flag" ||
         test ! -s conftest.err
       } && test -s libconftest.la && _AC_DO_STDERR($afs_[]_AC_LANG_ABBREV[]_compile) && {
         test -z "$ac_[]_AC_LANG_ABBREV[]_werror_flag" ||
         test ! -s conftest.err
       } && test -s conftest.$ac_objext && _AC_DO_STDERR($afs_[]_AC_LANG_ABBREV[]_ltlink) && {
         test -z "$ac_[]_AC_LANG_ABBREV[]_werror_flag" ||
         test ! -s conftest.err
       } && test -s conftest$ac_exeext && {
         test "$cross_compiling" = yes ||
         AS_TEST_X([conftest$ac_exeext])
       }],
      [ac_retval=0],
      [_AC_MSG_LOG_CONFTEST
        ac_retval=1])
  AS_LINENO_POP
  AS_SET_STATUS([$ac_retval])
])# _AFS_LTLINK_LA_IFELSE_BODY


# AFS_LTLINK_IFELSE( PROGRAM, [ACTION-IF-TRUE], [ACTION-IF-FALSE] )
# -----------------------------------------------------------------
# Try to libtool-link PROGRAM. Based on _AC_LINK_IFELSE.
# Uses CPPFLAGS, LDFLAGS, and LIBS for generating PROGRAM.
#
AC_DEFUN([AFS_LTLINK_IFELSE], [dnl
AC_REQUIRE([LT_OUTPUT])dnl
AC_REQUIRE_SHELL_FN([afs_fn_]_AC_LANG_ABBREV[_try_ltlink],
  [AS_FUNCTION_DESCRIBE([afs_fn_]_AC_LANG_ABBREV[_try_ltlink], [LINENO],
    [Using libtool, try to link conftest.$ac_ext. Return whether this succeeded.])],
  [_$0_BODY])]dnl
[m4_ifvaln([$1], [AC_LANG_CONFTEST([$1])])]dnl
[AS_IF([afs_fn_[]_AC_LANG_ABBREV[]_try_ltlink "$LINENO"], [$2], [$3])
rm -f conftest.err libconftest.lo libconftest.la .libs/libconftest* libconftest.$ac_ext .libs/conftest* conftest$ac_exeext conftest.$ac_objext conftest.$ac_ext[]])


# _AFS_LTLINK_IFELSE_BODY()
# -------------------------
# Shell function body for AFS_LTLINK_IFELSE. Based on
# _AC_LINK_IFELSE_BODY.
#
m4_define([_AFS_LTLINK_IFELSE_BODY],
[  AS_LINENO_PUSH([$[]1])
  afs_[]_AC_LANG_ABBREV[]_compile='$[]_AC_CC $_AC_LANG_PREFIX[]FLAGS $CPPFLAGS -c conftest.$ac_ext -o conftest.$ac_objext'
  afs_[]_AC_LANG_ABBREV[]_ltlink='./libtool --tag=_AC_CC --mode=link $_AC_CC $_AC_LANG_PREFIX[]FLAGS $CPPFLAGS $LDFLAGS -o conftest$ac_exeext conftest.$ac_objext $LIBS >&AS_MESSAGE_LOG_FD'
  rm -f .libs/libconftest.$ac_objext libconftest.lo libconftest.la conftest.$ac_objext .libs/conftest$ac_exeext conftest$ac_exeext
  AS_IF([_AC_DO_STDERR($afs_[]_AC_LANG_ABBREV[]_compile) && {
         test -z "$ac_[]_AC_LANG_ABBREV[]_werror_flag" ||
         test ! -s conftest.err
       } && test -s conftest.$ac_objext && _AC_DO_STDERR($afs_[]_AC_LANG_ABBREV[]_ltlink) && {
         test -z "$ac_[]_AC_LANG_ABBREV[]_werror_flag" ||
         test ! -s conftest.err
       } && test -s conftest$ac_exeext && {
         test "$cross_compiling" = yes ||
         AS_TEST_X([conftest$ac_exeext])
       }],
      [ac_retval=0],
      [_AC_MSG_LOG_CONFTEST
        ac_retval=1])
  AS_LINENO_POP
  AS_SET_STATUS([$ac_retval])
])# _AFS_LTLINK_IFELSE_BODY
