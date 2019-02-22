
## -*- mode: autoconf -*-

##
## This file is part of the Score-P software (http://www.score-p.org)
##
## Copyright (c) 2009-2012,
## RWTH Aachen University, Germany
##
## Copyright (c) 2009-2012,
## Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
##
## Copyright (c) 2009-2012,
## Technische Universitaet Dresden, Germany
##
## Copyright (c) 2009-2012,
## University of Oregon, Eugene, USA
##
## Copyright (c) 2009-2012, 2014-2015,
## Forschungszentrum Juelich GmbH, Germany
##
## Copyright (c) 2009-2012,
## German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
##
## Copyright (c) 2009-2012,
## Technische Universitaet Muenchen, Germany
##
## This software may be modified and distributed under the terms of
## a BSD-style license.  See the COPYING file in the package base
## directory for details.
##

## file       ac_scorep_generic_libs.m4


dnl ------------------------------------------------------------------

dnl $1: name of the library on the filesystem, without the extension,
dnl     used for searching the file if a path is given
dnl $2: whitespace separated list of headers
dnl $3: extra cppflags, e.g. -D_XOPEN_SOURCE=500
AC_DEFUN([AC_SCOREP_FRONTEND_LIB], [

m4_ifdef([AC_SCOREP_BACKEND],
         [m4_fatal([calling AC_SCOREP_FRONTEND_LIB([$1]) macro in backend configure])], [])
m4_ifdef([AC_SCOREP_BACKEND_MPI],
         [m4_fatal([calling AC_SCOREP_FRONTEND_LIB([$1]) macro in mpi configure])], [])

m4_define([$1_NAME], [m4_toupper($1)])

AC_ARG_VAR($1_NAME[]_INCLUDE, [Path to $1 headers.])
AC_ARG_VAR($1_NAME[]_LIB, [Path to $1 libraries.])

AC_ARG_WITH([$1],
            [AS_HELP_STRING([--with-$1=(yes|no|<Path to $1 installation>)],
                            [If you want to build with $1 support but
                             do not have a (frontend) $1 in a standard
                             location, you need to explicitly specify
                             the directory where it is installed
                             (--with-$1=<$1-install-dir>)
                             [yes]. This is a shorthand for
                             --with-$1-include=<Path/include> and
                             --with-$1-lib=<Path/lib>. If these
                             shorthand assumptions are not correct,
                             you can use the explicit include and lib
                             options directly.])],
            [],
            [with_$1="not_set"])
#echo "with_$1 = $with_$1"

AS_IF([test "x${with_$1}" = "xnot_set"],
      #then
      [AC_ARG_WITH([$1-include],
                   [  --with-$1-include=<Path to $1 headers>],
                   [],
                   [with_$1_include="${$1_NAME[]_INCLUDE:-yes}"])

       AC_ARG_WITH([$1-lib],
                   [  --with-$1-lib=<Path to $1 libraries>],
                   [],
                   [with_$1_lib="${$1_NAME[]_LIB:-yes}"])

       _AC_SCOREP_GENERIC_LIB_SANITY_CHECK([$1], [], [])],

      # else (--with-$1 set, i.e. (no|yes|path))
      [_AC_SCOREP_GENERIC_LIB_TOPLEVEL_GIVEN([$1], [])])

#echo "with_$1_include = ${with_$1_include}"
#echo "with_$1_lib     = ${with_$1_lib}"

_AC_SCOREP_GENERIC_HEADER_AND_LIB_CHECK([$1], [$2], [$3])
])


dnl ------------------------------------------------------------------

dnl $1: name of the library on the filesystem, without the extension,
dnl     used for searching the file if a path is given
dnl $2: whitespace separated list of headers
dnl $3: extra cppflags, e.g. -D_XOPEN_SOURCE=500
dnl $4: --with-$1 parameter if available, e.g. by a previous check.
dnl     Think about cuda that provides  --with-cupti=<cuda>/extras/CUPTI
dnl $5: don't create configure options if $5 is m4-blank
AC_DEFUN([AC_SCOREP_BACKEND_LIB], [

m4_ifdef([AC_SCOREP_FRONTEND],
         [m4_fatal([calling AC_SCOREP_BACKEND_LIB macro in frontend configure])], [])

m4_define([$1_NAME], [m4_toupper($1)])

m4_ifblank($5, [
AC_ARG_VAR($1_NAME[]_INCLUDE, [Path to $1 headers.])
AC_ARG_VAR($1_NAME[]_LIB, [Path to $1 libraries.])
])

AC_ARG_WITH([$1],
            [m4_ifblank($5, [AS_HELP_STRING([--with-$1=(yes|no|<Path to $1 installation>)],
                                            [If you want to build with $1 support but
                                             do not have a $1 in a standard location,
                                             you need to explicitly specify the
                                             directory where it is installed. On
                                             non-cross-compile systems we search the
                                             system include and lib paths per default
                                             [yes]; on cross-compile systems,
                                             however, you have to specify a path
                                             [no]. --with-$1 is a shorthand for
                                             --with-$1-include=<Path/include> and
                                             --with-$1-lib=<Path/lib>. If these
                                             shorthand assumptions are not correct,
                                             you can use the explicit include and lib
                                             options directly.])])],
            [AS_IF([test "x${ac_scorep_cross_compiling}" = "xyes"],
                   [AS_IF([test "x${withval}" = "xyes"],
                          [AC_MSG_ERROR([in cross-compile mode, invalid value 'yes' to --with-$1, specify a path instead.])])])],
            [m4_ifval([$4],
                           # use value provided in macro invocation (if set) if
                           # user doesn't provide anything via the commandline
                           [AS_IF([test -n "$4"],
                                  [with_$1="$4"],
                                  [with_$1="not_set"])],
                           [with_$1="not_set"])])

#echo "with_$1 = $with_$1"

AS_IF([test "x${with_$1}" = "xnot_set"],
      [AC_ARG_WITH([$1-include],
                   [m4_ifblank($5, [  --with-$1-include=<Path to $1 headers>])],
                   [AS_IF([test "x${ac_scorep_cross_compiling}" = "xno"],
                          [],
                          [AS_IF([test "x${withval}" = "xyes"],
                                 [AC_MSG_ERROR([in cross-compile mode, invalid value 'yes' to --with-$1-include, specify a path instead.])])])],
                   [AS_IF([test "x${ac_scorep_cross_compiling}" = "xno"],
                          [with_$1_include="${$1_NAME[]_INCLUDE:-yes}"],
                          [with_$1_include="${$1_NAME[]_INCLUDE:-no}"])])

       AC_ARG_WITH([$1-lib],
                   [m4_ifblank($5, [  --with-$1-lib=<Path to $1 libraries>])],
                   [AS_IF([test "x${ac_scorep_cross_compiling}" = "xno"],
                          [],
                          [AS_IF([test "x${withval}" = "xyes"],
                                 [AC_MSG_ERROR([in cross-compile mode, invalid value 'yes' to --with-$1-lib, specify a path instead.])])])],
                   [AS_IF([test "x${ac_scorep_cross_compiling}" = "xno"],
                          [with_$1_lib="${$1_NAME[]_LIB:-yes}"],
                          [with_$1_lib="${$1_NAME[]_LIB:-no}"])])

       _AC_SCOREP_GENERIC_LIB_SANITY_CHECK([$1], [], [])],

      # else (--with-$1 set, i.e. (no|yes|path))
      [_AC_SCOREP_GENERIC_LIB_TOPLEVEL_GIVEN([$1], [])])

AS_IF([test "x${ac_scorep_cross_compiling}" = "xyes" && test "x${with_$1_include}" = "xno" && test "x${with_$1_lib}"  = "xno"],
      [AC_MSG_WARN([In cross-compile mode, you need to provide a path to --with-$1 (or --with-$1-include and --with-$1-lib, respectively) in order to activate $1 support.])])

#echo "with_$1_include = ${with_$1_include}"
#echo "with_$1_lib     = ${with_$1_lib}"

_AC_SCOREP_GENERIC_HEADER_AND_LIB_CHECK([$1], [$2], [$3])
])

dnl ------------------------------------------------------------------

dnl AC_DEFUN([AC_SCOREP_FRONT_AND_BACKEND_LIB], [


dnl ])

dnl ------------------------------------------------------------------

AC_DEFUN([_AC_SCOREP_GENERIC_LIB_SANITY_CHECK], [
dnl $1: library name
dnl $2: "backend_" or ""
dnl $3: "backend-" or ""
AS_IF([   (test "x${with_$2[]$1_include}" != "xno" && test "x${with_$2[]$1_lib}"  = "xno") \
       || (test "x${with_$2[]$1_include}"  = "xno" && test "x${with_$2[]$1_lib}" != "xno")],
      [AC_MSG_ERROR([invalid combination of options --with-$3[]$1-include and --with-$3[]$1-lib.])])
])

dnl ------------------------------------------------------------------

AC_DEFUN([_AC_SCOREP_GENERIC_LIB_TOPLEVEL_GIVEN], [
dnl $1: library name
dnl $2: "backend_" or ""
dnl sets with_[$2_]$1_include and with_[$2_]$1_lib to (yes|no|path/(include|lib)))
AS_IF([test "x${with_$2[]$1}" = "xyes"], [with_$2[]$1_include="yes"; with_$2[]$1_lib="yes"],
      [test "x${with_$2[]$1}" = "xno"],  [with_$2[]$1_include="no";  with_$2[]$1_lib="no"],
      [with_$2[]$1_include="${with_$2[]$1}/include"
       AS_IF([test -d "${with_$2[]$1}/lib64"],
             [with_$2[]$1_lib="${with_$2[]$1}/lib64"],
             [with_$2[]$1_lib="${with_$2[]$1}/lib"])])
])


dnl ------------------------------------------------------------------


dnl $1: list of whitespace separated files
dnl $2: user provided dirname, files get appended
dnl $3: variable indicating the success of the tests. Will be
dnl set to "no" if one list element is not found, left unchanged otherwise.
AC_DEFUN([_AC_SCOREP_ALL_FILES_EXIST], [
AS_FOR([AC_SCOREP_Files],
       [ac_scorep_files],
       [$1],
       [AS_IF([test ! -f $2/AC_SCOREP_Files],
              [$3="no"])])
])

dnl ------------------------------------------------------------------


dnl $1: list of whitespace separated files
dnl $2: user provided dirname, files get appended
dnl $3: variable indicating the success of the tests. Will be
dnl set to "no" if none of the list element is found, left unchanged otherwise
AC_DEFUN([_AC_SCOREP_ONE_OF_FILES_EXIST], [
ac_scorep_one_of_files_exist="no"
AS_FOR([AC_SCOREP_Files],
       [ac_scorep_files],
       [$1],
       [AS_IF([test -f $2/AC_SCOREP_Files],
              [ac_scorep_one_of_files_exist="yes"])])
AS_IF([test "x${ac_scorep_one_of_files_exist}" = "xno"],
      [$3="no"])
])

dnl ------------------------------------------------------------------

AC_DEFUN([_AC_SCOREP_GENERIC_HEADER_AND_LIB_CHECK], [
AC_LANG_PUSH([C])
dnl $1: libname
dnl $2: white space separated list of headers
dnl $3: extra cppflags, e.g. -D_XOPEN_SOURCE=500
dnl uses from caller:
dnl - $1
dnl - $1_NAME
dnl - with_$1_include
dnl - with_$1_lib
dnl sets:
dnl - with_$1_include_checks_successful
dnl - with_$1_cppflags
dnl - with_$1_lib_checks_successful
dnl - with_$1_ldflags=""
dnl - with_$1_libs
dnl - HAVE_[]$1_NAME automake conditional
dnl - HAVE_[]$1_NAME preprocessor define
dnl - $1_NAME[]_CPPFLAGS ac_subst
dnl - $1_NAME[]_LDFLAGS ac_subst
dnl - $1_NAME[]_LIBS ac_subst
dnl does save and reset CPPFLAGS, LDFLAGS, and LIBS automatically

#echo "with_$1_include = ${with_$1_include}"
#echo "with_$1_lib     = ${with_$1_lib}"

$1_dirs_exist="yes"
AS_IF([test "x${with_$1_include}" != "xno" && test "x${with_$1_include}" != "xyes" && test ! -d ${with_$1_include}],
      [$1_dirs_exist="no"
       AC_MSG_WARN([Provided $1 include directory ${with_$1_include} does not exist.])])
AS_IF([test "x${with_$1_lib}" != "xno" && test "x${with_$1_lib}" != "xyes" && test ! -d ${with_$1_lib}],
      [$1_dirs_exist="no"
       AC_MSG_WARN([Provided $1 lib directory ${with_$1_lib} does not exist.])])
#echo "$1_dirs_exist ${$1_dirs_exist}"


AS_IF([test "x${$1_dirs_exist}" = "xno"],
      [with_$1_include_checks_successful="no"
       with_$1_lib_checks_successful="no"],
[
# check header files
with_$1_include_checks_successful="yes"
m4_ifnblank($2, [
    with_$1_cppflags=""
    AS_IF([test "x${with_$1_include}" != "xno"],
          [cpp_flags_save_$1="${CPPFLAGS}"
           with_$1_cppflags="$3"
           AS_IF([test "x${with_$1_include}" != "xyes"],
                 [with_$1_cppflags="${with_$1_cppflags} -I${with_$1_include}"
                  _AC_SCOREP_ALL_FILES_EXIST([$2], [${with_$1_include}], [with_$1_include_checks_successful])
                 ])
           #echo "CPPFLAGS $CPPFLAGS"
           AS_IF([test "x${with_$1_include_checks_successful}" = "xyes"],
                 [CPPFLAGS="${with_$1_cppflags} ${CPPFLAGS}"
                  AC_CHECK_HEADERS([$2], [], [with_$1_include_checks_successful="no"])])
           CPPFLAGS="${cpp_flags_save_$1}"],
          [with_$1_include_checks_successful="no"])])

#echo "with_$1_include_checks_successful $with_$1_include_checks_successful"

# check libs
with_$1_lib_checks_successful="unknown"
with_$1_ldflags=""
with_$1_rpathflag=""
with_$1_libs="unknown"
AS_IF([test "x${with_$1_lib}" != "xno" && test "x${with_$1_include_checks_successful}" = "xyes"],
      [ld_flags_save_$1="${LDFLAGS}"
       libs_save_$1="${LIBS}"
       cpp_flags_save_$1="${CPPFLAGS}"
       AS_IF([test "x${with_$1_lib}" != "xyes"],
             [_AC_SCOREP_ONE_OF_FILES_EXIST([$1.a $1.so $1.dylib], [${with_$1_lib}], [with_$1_lib_checks_successful])
              AS_IF([test "x${with_$1_lib_checks_successful}" = "xno"],
                    [tmp_basename=`basename ${with_$1_lib}`
                     tmp_dirname=`dirname ${with_$1_lib}`
                     with_$1_lib_checks_successful="unknown"
                     AS_IF([test "x${tmp_basename}" = "xlib"],
                           [_AC_SCOREP_ONE_OF_FILES_EXIST([$1.a $1.so $1.dylib], [${tmp_dirname}/lib64], [with_$1_lib_checks_successful])
                            AS_IF([test "x${with_$1_lib_checks_successful}" != "xno"],
                                  [with_$1_lib="${tmp_dirname}/lib64"])
                           ],
                           [test "x${tmp_basename}" = "xlib64"],
                           [_AC_SCOREP_ONE_OF_FILES_EXIST([$1.a $1.so $1.dylib], [${tmp_dirname}/lib], [with_$1_lib_checks_successful])
                            AS_IF([test "x${with_$1_lib_checks_successful}" != "xno"],
                                  [with_$1_lib="${tmp_dirname}/lib"])
                           ])
                    ])
              with_$1_ldflags="-L${with_$1_lib}"
              with_$1_rpathflag="-R${with_$1_lib}"
              AS_IF([test "x${with_$1_lib_checks_successful}" = "xno"],
                    [AC_MSG_ERROR([cannot find $1.a, $1.so, or $1.dylib])])])
       AS_IF([test "x${with_$1_lib_checks_successful}" = "xunknown"],
             [CPPFLAGS="${with_$1_cppflags} ${CPPFLAGS}"
              LDFLAGS="${with_$1_ldflags} ${LDFLAGS}"
              #echo "CPPFLAGS $CPPFLAGS"
              #echo "LDFLAGS $LDFLAGS"
              m4_expand(_AC_SCOREP_[]$1_NAME[]_LIB_CHECK([$1], [$2]))]) dnl specific library check, to be impemented elsewhere

       AS_IF([(test "x${with_$1_lib_checks_successful}" != "xyes" && \
               test "x${with_$1_lib_checks_successful}" != "xno") || \
              test "x${with_$1_libs}" = "xunknown"],
             [m4_pattern_allow(AC_SCOREP_[]$1_NAME[]_LIB_CHECK) dnl otherwise following error message fails.
              AC_MSG_ERROR([implementation of _AC_SCOREP_[]$1_NAME[]_LIB_CHECK does not provide required output.])])

       CPPFLAGS="${cpp_flags_save_$1}"
       LIBS="${libs_save_$1}"
       LDFLAGS="$ld_flags_save_$1"],
      [with_$1_lib_checks_successful="no"])

AC_LANG_POP([C])
])

#echo "with_$1_lib_checks_successful $with_$1_lib_checks_successful"

# generating output
AS_IF([test "x${with_$1_include_checks_successful}" = "xyes" && \
       test "x${with_$1_lib_checks_successful}" = "xyes" ],
      # then
      [# strip whitespace
       $1_result=`echo "yes, using ${with_$1_cppflags} ${with_$1_ldflags} ${with_$1_rpathflag} ${with_$1_libs}" | sed 's/  */\ /g'`
       $1_result="${$1_result}${$1_success_post_result}"
       scorep_have_$1="yes"
       AM_CONDITIONAL(HAVE_[]$1_NAME, [test 1 -eq 1])
       AC_DEFINE(HAVE_[]$1_NAME, [1], [Defined if $1 is available.])
       AC_SUBST($1_NAME[]_CPPFLAGS, ["${with_$1_cppflags}"])
       AC_SUBST($1_NAME[]_LDFLAGS, ["${with_$1_ldflags} ${with_$1_rpathflag}"])
       AC_SUBST($1_NAME[]_LIBS, ["${with_$1_libs}"])],
      # else
      [$1_result="no"
       scorep_have_$1="no"
       AM_CONDITIONAL(HAVE_[]$1_NAME, [test 1 -eq 0])
       AC_SUBST($1_NAME[]_CPPFLAGS, [""])
       AC_SUBST($1_NAME[]_LDFLAGS,  [""])
       AC_SUBST($1_NAME[]_LIBS,     [""])])

AC_MSG_CHECKING([for $1])
AC_MSG_RESULT([${$1_result}])
AFS_SUMMARY([$1 support], [${$1_result}])
])


dnl BYPASS_GENERIC_LIB_CHECK_ON_FUJITSU(libname, -llib)
dnl S1: libname as in AC_SCOREP_BACKEND_LIB, e.g. libbfd
dnl $2: lib as in link command, e.g., -lbfd
dnl ------------------------------------------------------
dnl bypassing library check on K/FX10 for TW14 2014-03 in Kobe:
dnl K/FX10 are cross-compile machines, so AC_SCOREP_BACKEND_LIB requires
dnl a path to the installation. This path in used as -I<path>/include in
dnl configure checks. libbfd and libz are located in system directories
dnl and specifying these directories in an -I CPP option let the Fujitsu
dnl compiler fail. This is most likely a compiler bug. As we know that
dnl libbfd and libz are available, lets bypass the check for now using:
dnl AS_IF([test "x${ac_scorep_platform}" = xk ||
dnl        test "x${ac_scorep_platform}" = xfx10],
dnl     [BYPASS_GENERIC_LIB_CHECK_ON_FUJITSU([<lib-to-bypass>])],
dnl     [AC_SCOREP_BACKEND_LIB([<lib-to-bypass>], ...)])
AC_DEFUN([BYPASS_GENERIC_LIB_CHECK_ON_FUJITSU], [

AS_IF([test "x${with_$1}" = xno],
    [scorep_have_$1="no"
     AM_CONDITIONAL(HAVE_[]m4_toupper($1), [test 1 -eq 0])
     AC_SUBST(m4_toupper($1)[]_CPPFLAGS, [""])
     AC_SUBST(m4_toupper($1)[]_LDFLAGS,  [""])
     AC_SUBST(m4_toupper($1)[]_LIBS,     [""])

     AFS_SUMMARY([$1 support], [no, disabled])
    ],
    [scorep_have_$1="yes"
     AM_CONDITIONAL(HAVE_[]m4_toupper($1), [test 1 -eq 1])
     AC_DEFINE(HAVE_[]m4_toupper($1), [1], [Defined if $1 is available.])
     AC_SUBST(m4_toupper($1)[]_CPPFLAGS, [""])
     AC_SUBST(m4_toupper($1)[]_LDFLAGS,  [""])
     AC_SUBST(m4_toupper($1)[]_LIBS,     ["$2"])

     AC_MSG_CHECKING([for $1])
     AC_MSG_RESULT([yes, bypassed on Fujitsu systems])
     AFS_SUMMARY([$1 support], [yes (check bypassed on Fujitsu systems)])
    ])
])
