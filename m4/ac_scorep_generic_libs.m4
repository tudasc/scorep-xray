## -*- mode: autoconf -*-

## 
## This file is part of the Score-P software (http://www.score-p.org)
##
## Copyright (c) 2009-2012, 
##    RWTH Aachen, Germany
##    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
##    Technische Universitaet Dresden, Germany
##    University of Oregon, Eugene, USA
##    Forschungszentrum Juelich GmbH, Germany
##    German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
##    Technische Universitaet Muenchen, Germany
##
## See the COPYING file in the package base directory for details.
##

## file       ac_scorep_generic_libs.m4
## maintainer Christian Roessel <c.roessel@fz-juelich.de>


AC_DEFUN([AC_SCOREP_BAR_LIB_CHECK], [
echo "hello from $1, using $2"
with_$1_lib_checks_successful="yes"
with_$1_libs="-lunwind"
])

dnl ------------------------------------------------------------------

AC_DEFUN([AC_SCOREP_FRONTEND_LIB], [

m4_ifdef([AC_SCOREP_BACKEND], 
         [m4_fatal([calling AC_SCOREP_FRONTEND_LIB([$1]) macro in backend configure])], [])

m4_define([$1_NAME], [m4_toupper($1)])

AC_ARG_VAR($1_NAME[]_INCLUDE, [Path to $1 headers.])
AC_ARG_VAR($1_NAME[]_LIB, [Path to $1 libraries.])

AC_ARG_WITH([$1], 
            [AS_HELP_STRING([--with-$1=<Path to $1 installation>], 
                            [If you want to build scorep with $1 but
                             do not have a frontend $1 in a standard
                             location then you need to explicitly
                             specify the directory where it is
                             installed (--with-$1=<$1-install-dir>)
                             [yes]. This is a shorthand for
                             --with-$1-include=<Path/include> and
                             --with-$1-lib=<Path/lib>. If these
                             shorthand assumptions are not correct,
                             you can use the explicit include and lib
                             options directly.])],
            [],
            [with_$1="not_set"])
echo "with_$1 = $with_$1"

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

       # sanity check
       AS_IF([   (test "x${with_$1_include}" != "xno" && test "x${with_$1_lib}"  = "xno") \
              || (test "x${with_$1_include}"  = "xno" && test "x${with_$1_lib}" != "xno")], 
             [AC_MSG_ERROR([invalid combination of --with-$1-include and --with-$1-lib options.])])],

      # else (--with-$1 set, i.e. (no|yes|path))
      [AS_IF([test "x${with_$1}" = "xyes"], [with_$1_include="yes"; with_$1_lib="yes"],
             [test "x${with_$1}" = "xno"],  [with_$1_include="no";  with_$1_lib="no"],
             [with_$1_include="${with_$1}/include"; with_$1_lib="${with_$1}/lib"])])


echo "with_$1_include = ${with_[$1]_include}"
echo "with_$1_lib     = ${with_$1_lib}"

_AC_SCOREP_GENERIC_HEADER_AND_LIB_CHECK([$1], [$2])

])


dnl ------------------------------------------------------------------

AC_DEFUN([AC_SCOREP_BACKEND_LIB], [

m4_ifdef([AC_SCOREP_FRONTEND], 
         [m4_fatal([calling AC_SCOREP_BACKEND_LIB macro in frontend configure])], [])

m4_define([$1_NAME], [m4_toupper($1)])
m4_define([$1_BACKEND_NAME], [m4_toupper($1)_BACKEND])

AC_ARG_VAR($1_NAME[]_INCLUDE, [Path to $1 headers.])
AC_ARG_VAR($1_NAME[]_LIB, [Path to $1 libraries.])
AC_ARG_VAR($1_BACKEND_NAME[]_INCLUDE, [Path to backend $1 headers.])
AC_ARG_VAR($1_BACKEND_NAME[]_LIB, [Path to backend $1 libraries.])

AS_IF([test "x${ac_scorep_cross_compiling}" = "xno"],
[# not cross compiling
AC_ARG_WITH([$1], 
            [AS_HELP_STRING([--with-$1=<Path to $1 installation>], 
                            [On non-cross-compile systems: if you want
                             to build scorep with $1 but do not have a
                             $1 in a standard location then you need
                             to explicitly specify the directory where
                             it is installed
                             (--with-$1=<$1-install-dir>) [yes].  This
                             is a shorthand for
                             --with-$1-include=<Path/include> and
                             --with-$1-lib=<Path/lib>. If these
                             shorthand assumptions are not correct,
                             you can use the explicit include and lib
                             options directly. See also
                             --with-backend-$1 below.])],
            [],
            [with_$1="not_set"])
echo "with_$1 = $with_$1"

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

       # sanity check
       AS_IF([   (test "x${with_$1_include}" != "xno" && test "x${with_$1_lib}"  = "xno") \
              || (test "x${with_$1_include}"  = "xno" && test "x${with_$1_lib}" != "xno")], 
             [AC_MSG_ERROR([invalid combination of --with-$1-include and --with-$1-lib options.])])],

      # else (--with-$1 set, i.e. (no|yes|path))
      [AS_IF([test "x${with_$1}" = "xyes"], [with_$1_include="yes"; with_$1_lib="yes"],
             [test "x${with_$1}" = "xno"],  [with_$1_include="no";  with_$1_lib="no"],
             [with_$1_include="${with_$1}/include"; with_$1_lib="${with_$1}/lib"])])


echo "with_$1_include = ${with_[$1]_include}"
echo "with_$1_lib     = ${with_$1_lib}"
],
# else cross compiling
[

AC_ARG_WITH([backend-$1], 
            [AS_HELP_STRING([--with-backend-$1=<Path to backend $1 installation>], 
                            [On cross-compile systems: if you want to
                             build scorep with $1 then you need to
                             explicitly specify the directory where a
                             backend version of $1 is installed
                             (--with-backend-$1=<backend-$1-install-dir>)
                             [no]. This is a shorthand for
                             --with-backend-$1-include=<Path/include>
                             and --with-backend-$1-lib=<Path/lib>. If
                             these shorthand assumptions are not
                             correct, you can use the explicit include
                             and lib options directly. See also
                             --with-$1 above.])],
            [AS_IF([test "x${withval}" = "xyes"], 
                   [AC_MSG_ERROR([invalid value 'yes' to --with-backend-$1, specify a path instead.])])],
            [with_backend_$1="not_set"])
echo "with_backend_$1 = $with_backend_$1"

AS_IF([test "x${with_backend_$1}" = "xnot_set"], 
      #then
      [AC_ARG_WITH([backend-$1-include],
                   [  --with-backend-$1-include=<Path to backend $1 headers>],
                   [AS_IF([test "x${withval}" = "xyes"],
                          [AC_MSG_ERROR([invalid value 'yes' to --with-backend-$1-include, specify a path instead.])])],
                   [with_backend_$1_include="${$1_BACKEND_NAME[]_INCLUDE:-no}"])

       AC_ARG_WITH([backend-$1-lib], 
                   [  --with-backend-$1-lib=<Path to backend $1 libraries>], 
                   [AS_IF([test "x${withval}" = "xyes"],
                          [AC_MSG_ERROR([invalid value 'yes' to --with-backend-$1-lib, specify a path instead.])])],
                   [with_backend_$1_lib="${$1_BACKEND_NAME[]_LIB:-no}"])

       # sanity check
       AS_IF([   (test "x${with_backend_$1_include}" != "xno" && test "x${with_backend_$1_lib}"  = "xno") \
              || (test "x${with_backend_$1_include}"  = "xno" && test "x${with_backend_$1_lib}" != "xno")], 
             [AC_MSG_ERROR([invalid combination of --with-backend-$1-include and --with-backend-$1-lib options.])])],

      # else (--with-backend-$1 set, i.e. (no|yes|path))
      [AS_IF([test "x${with_backend_$1}" = "xyes"], [with_backend_$1_include="yes"; with_backend_$1_lib="yes"],
             [test "x${with_backend_$1}" = "xno"],  [with_backend_$1_include="no";  with_backend_$1_lib="no"],
             [with_backend_$1_include="${with_backend_$1}/include"; with_backend_$1_lib="${with_backend_$1}/lib"])])


echo "with_backend_$1_include = ${with_[backend_$1]_include}"
echo "with_backend_$1_lib     = ${with_backend_$1_lib}"


])



_AC_SCOREP_GENERIC_HEADER_AND_LIB_CHECK([$1], [$2])

])

dnl ------------------------------------------------------------------

dnl AC_DEFUN([AC_SCOREP_FRONT_AND_BACKEND_LIB], [


dnl ])

dnl ------------------------------------------------------------------

AC_DEFUN([_AC_SCOREP_GENERIC_HEADER_AND_LIB_CHECK], [
AC_LANG_PUSH([C])

# check header files
with_$1_include_checks_successful="yes"
with_$1_cppflags=""
AS_IF([test "x${with_$1_include}" != "xno"], 
      [cpp_flags_save_$1="${CPPFLAGS}"
       AS_IF([test "x${with_$1_include}" != "xyes"], 
             [with_$1_cppflags="-I${with_$1_include}"
              CPPFLAGS="${with_$1_cppflags} ${CPPFLAGS}"])
       echo "CPPFLAGS $CPPFLAGS"
       m4_foreach([header_file], m4_dquote($2), 
                  [AS_IF([test "x${with_$1_include_checks_successful}" = "xyes"],
                          dnl do not quote header_file
                         [AC_CHECK_HEADER(header_file, [], [with_$1_include_checks_successful="no"])])
                  ])dnl don't remove newline
       CPPFLAGS="${cpp_flags_save_$1}"])

# check libs
with_$1_lib_checks_successful="unknown"
with_$1_ldflags=""
with_$1_libs="unknown"
AS_IF([test "x${with_$1_lib}" != "xno" && test "x${with_$1_include_checks_successful}" = "xyes"],
      [ld_flags_save_$1="${LDFLAGS}"
       libs_save_$1="${LIBS}"
       cpp_flags_save_$1="${CPPFLAGS}"

       AS_IF([test "x${with_$1_lib}" != "xyes"], 
             [with_$1_ldflags="-L${with_$1_lib}"
              LDFLAGS="${with_$1_ldflags} ${LDFLAGS}"
              CPPFLAGS="-I${with_$1_include} ${CPPFLAGS}"])
       echo "CPPFLAGS $CPPFLAGS"
       echo "LDFLAGS $LDFLAGS"
       
       m4_expand(AC_SCOREP_[]$1_NAME[]_LIB_CHECK([$1], [$2])) dnl specific library check, to be impemented elsewhere

       AS_IF([(test "x${with_$1_lib_checks_successful}" != "xyes" &&  \
               test "x${with_$1_lib_checks_successful}" != "xno")  || \
              test "x${with_$1_libs}" = "xunknown"],
             [m4_pattern_allow(AC_SCOREP_[]$1_NAME[]_LIB_CHECK) dnl otherwise following error message fails.
              AC_MSG_ERROR([implementation of AC_SCOREP_[]$1_NAME[]_LIB_CHECK does not provide required output.])])

       CPPFLAGS="${cpp_flags_save_$1}"
       LIBS="${libs_save_$1}"
       LDFLAGS="$ld_flags_save_$1"
      ])

AC_LANG_POP([C])

# generating output
AS_IF([test "x${with_$1_include_checks_successful}" = "xyes" && \
       test "x${with_$1_lib_checks_successful}" = "xyes" ],
      # then
      [# strip whitespace
       result=`echo "yes, using ${with_$1_cppflags} ${with_$1_ldflags} ${with_$1_libs}" | sed 's/  */\ /g'`
       AM_CONDITIONAL(HAVE_[]$1_NAME, [test 1 -eq 1])
       AC_DEFINE(HAVE_[]$1_NAME, [1], [Defined if $1 is available.])
       AC_SUBST($1_NAME[]_CPPFLAGS, ["${with_$1_cppflags}"])
       AC_SUBST($1_NAME[]_LDFLAGS, ["${with_$1_ldflags}"])
       AC_SUBST($1_NAME[]_LIBS, ["${with_$1_libs}"])],
      # else
      [result="no"
       AM_CONDITIONAL(HAVE_[]$1_NAME, [test 1 -eq 0])
       AC_SUBST($1_NAME[]_CPPFLAGS, [""])
       AC_SUBST($1_NAME[]_LDFLAGS,  [""])
       AC_SUBST($1_NAME[]_LIBS,     [""])])

AC_MSG_CHECKING([for $1])
AC_MSG_RESULT([${result}])
AC_SCOREP_SUMMARY([$1 support], [${result}])
])
