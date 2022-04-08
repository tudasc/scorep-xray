# -*- mode: autoconf -*-

# This file is part of the Score-P software (http://www.score-p.org)
# and is based on AC_RUN_IFELSE as in autoconf 2.69. This program is
# free software; you can redistribute it and/or modify it under the
# terms of the GNU General Public License as published by the Free
# Software Foundation, either version 3 of the License, or (at your
# option) any later version.
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
#
# Written by David MacKenzie, with help from
# Franc,ois Pinard, Karl Berry, Richard Pixley, Ian Lance Taylor,
# Roland McGrath, Noah Friedman, david d zuhn, and many others.
#


# AFS_RUN_IFELSE([ACTION-IF-TRUE], [ACTION-IF-FALSE],
#                [ACTION-IF-CROSS-COMPILING = RUNTIME-ERROR])
# -----------------------------------------------------------
# Run existing program conftest$ac_exeext. In contrast to
# AC_RUN_IFELSE, omit the compile and link step.
#
AC_DEFUN([AFS_RUN_IFELSE],
[AC_LANG_COMPILER_REQUIRE()dnl
m4_ifval([$3], [],
    [m4_warn([cross], [$0 called without default to allow cross compiling])])dnl
AS_IF([test "$cross_compiling" = yes],
    [m4_default([$3],
        [AC_MSG_FAILURE([cannot run test program while cross compiling])])],
    [_AFS_RUN_IFELSE($@)])
])dnl AFS_RUN_IFELSE


# _AFS_RUN_IFELSE([ACTION-IF-TRUE], [ACTION-IF-FALSE])
# ----------------------------------------------------
# Run existing program conftest$ac_exeext. In contrast to
# AC_RUN_IFELSE, omit the compile and link step. Assumes program can
# be executed.
#
m4_define([_AFS_RUN_IFELSE],
[AC_REQUIRE_SHELL_FN([afs_fn_]_AC_LANG_ABBREV[_try_run],
  [AS_FUNCTION_DESCRIBE([afs_fn_]_AC_LANG_ABBREV[_try_run], [LINENO],
    [Try to run existing conftest$ac_exeext.
     Nothing gets compiled or linked.
     Assumes that executables *can* be run.])],
  [$0_BODY])]dnl
[AS_IF([afs_fn_[]_AC_LANG_ABBREV[]_try_run "$LINENO"], [$1], [$2])
rm -f core *.core core.conftest.* conftest$ac_exeext
])dnl _AFS_RUN_IFELSE


# _AFS_RUN_IFELSE_BODY()
# -------------------------
#
m4_define([_AFS_RUN_IFELSE_BODY],
[  AS_LINENO_PUSH([$[]1])
  AS_IF([_AC_DO_TOKENS(./conftest$ac_exeext)],
      [ac_retval=0],
      [AS_ECHO(["$as_me: program exited with status $ac_status"]) >&AS_MESSAGE_LOG_FD
       AS_ECHO(["$as_me: failed program ([]_AC_LANG_ABBREV[]) was: ./conftest$ac_exeext"]) >&AS_MESSAGE_LOG_FD
       ac_retval=$ac_status])
  rm -rf conftest.dSYM conftest_ipa8_conftest.oo
  AS_LINENO_POP
  AS_SET_STATUS([$ac_retval])
])dnl _AFS_RUN_IFELSE_BODY
