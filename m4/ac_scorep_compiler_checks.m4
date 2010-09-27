## -*- mode: autoconf -*-

## 
## This file is part of the SCOREP project (http://www.scorep.de)
##
## Copyright (c) 2009-2011, 
##    RWTH Aachen, Germany
##    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
##    Technische Universitaet Dresden, Germany
##    University of Oregon, Eugene USA
##    Forschungszentrum Juelich GmbH, Germany
##    Technische Universitaet Muenchen, Germany
##
## See the COPYING file in the package base directory for details.
##


AC_DEFUN([SCOREP_COMPILER_INTEL],[
AC_MSG_CHECKING([for intel compiler])
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[]],
[[#if defined(__INTEL_COMPILER) || defined(__ICC)
#else
# error "Not an Intel compiler."
#endif
]])],
                  [scorep_compiler_intel="yes"; scorep_compiler_unknown="no"], 
                  [scorep_compiler_intel="no"])
AC_MSG_RESULT([$scorep_compiler_intel])
AS_IF([test "x${scorep_compiler_intel}" = "xyes"], 
      [scorep_compiler_instrumentation_cppflags="-tcollect"]
       AC_DEFINE([FORTRAN_MANGLED(var)], [var ## _], 
                 [Name of var after mangled by the Fortran compiler.])
       AC_DEFINE([FOO_MANGLED],    [foo_],    [[]])
       AC_DEFINE([BAR_MANGLED],    [bar_],    [[]])
       AC_DEFINE([FOOBAR_MANGLED], [foobar_], [[]])
       AC_DEFINE([BAZ_MANGLED],    [baz_],    [[]]))
])

##

AC_DEFUN([SCOREP_COMPILER_SUN],[
AC_MSG_CHECKING([for sun compiler])
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[]],
[[#if defined(__SUNPRO_C)
#else
# error "Not a Sun compiler."
#endif
]])],
                  [scorep_compiler_sun="yes"; scorep_compiler_unknown="no"], 
                  [scorep_compiler_sun="no"])
AC_MSG_RESULT([$scorep_compiler_sun])
AS_IF([test "x${scorep_compiler_sun}" = "xyes"], 
      [scorep_compiler_instrumentation_cppflags="-O -Qoption f90comp -phat"]
       AC_DEFINE([FORTRAN_MANGLED(var)], [var ## _], 
                 [Name of var after mangled by the Fortran compiler.])
       AC_DEFINE([FOO_MANGLED],    [foo_],    [[]])
       AC_DEFINE([BAR_MANGLED],    [bar_],    [[]])
       AC_DEFINE([FOOBAR_MANGLED], [foobar_], [[]])
       AC_DEFINE([BAZ_MANGLED],    [baz_],    [[]]))
])

##

AC_DEFUN([SCOREP_COMPILER_IBM],[
AC_MSG_CHECKING([for ibm compiler])
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[]],
[[#if defined(__IBMC__)
# if __IBMC__ >= 800
# else
#  error "Not an IBM XL compiler."
# endif
#else
# error "Not an IBM compiler."
#endif
]])],
                  [scorep_compiler_ibm="yes"; scorep_compiler_unknown="no"], 
                  [scorep_compiler_ibm="no"])
AC_MSG_RESULT([$scorep_compiler_ibm])
AS_IF([test "x${scorep_compiler_ibm}" = "xyes"], 
      [scorep_compiler_instrumentation_cppflags="-qdebug=function_trace"]
       AC_DEFINE([FORTRAN_MANGLED(var)], [var], 
                 [Name of var after mangled by the Fortran compiler.]))
])

##

AC_DEFUN([SCOREP_COMPILER_PGI],[
AC_MSG_CHECKING([for pgi compiler])
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[]],
[[#if defined(__PGI)
#else
# error "Not a PGI compiler."
#endif
]])],
                  [scorep_compiler_pgi="yes"; scorep_compiler_unknown="no"], 
                  [scorep_compiler_pgi="no"])
AC_MSG_RESULT([$scorep_compiler_pgi])
AS_IF([test "x${scorep_compiler_pgi}" = "xyes"], 
      [scorep_compiler_instrumentation_cppflags="-Mprof=func"]
       AC_DEFINE([FORTRAN_MANGLED(var)], [var ## _], 
                 [Name of var after mangled by the Fortran compiler.]))
])

##

AC_DEFUN([SCOREP_COMPILER_GNU],[
AC_MSG_CHECKING([for gnu compiler])
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[]],
[[#if defined(__GNUC__)
#if defined(__INTEL_COMPILER) || defined(__ICC)
# error "Not a GNU compiler."
#endif
#else
# error "Not a GNU compiler."
#endif
]])],
                  [scorep_compiler_gnu="yes"; scorep_compiler_unknown="no"], 
                  [scorep_compiler_gnu="no"])
AC_MSG_RESULT([$scorep_compiler_gnu])
AS_IF([test "x${scorep_compiler_gnu}" = "xyes"], 
      [scorep_compiler_instrumentation_cppflags="-finstrument-functions"
       AC_DEFINE([FORTRAN_MANGLED(var)], [var ## _], 
                 [Name of var after mangled by the Fortran compiler.])])
])

##

AC_DEFUN([SCOREP_COMPILER_HP],[
AC_MSG_CHECKING([for hp compiler])
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[]],
[[#if defined(__HP_cc) || defined(__hpux) || defined(__hpua)
#else
# error "Not a HP compiler."
#endif
]])],
                  [scorep_compiler_hp="yes"; scorep_compiler_unknown="no"], 
                  [scorep_compiler_hp="no"])
AC_MSG_RESULT([$scorep_compiler_hp])
AS_IF([test "x${scorep_compiler_hp}" = "xyes"], 
      [scorep_compiler_instrumentation_cppflags=""]
       AC_DEFINE([FORTRAN_MANGLED(var)], [hp compiler's Fortran mangling not implemented yet, see ac_scorep_compiler_checks.m4], 
                 [Name of var after mangled by the Fortran compiler.]))
])

## 

AC_DEFUN([SCOREP_COMPILER_SX],[
AC_MSG_CHECKING([for sx compiler])
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[]],
[[#if defined(__SX_cc) || defined(__hpux) || defined(__hpua)
#else
# error "Not a SX compiler."
#endif
]])],
                  [scorep_compiler_sx="yes"; scorep_compiler_unknown="no"], 
                  [scorep_compiler_sx="no"])
AC_MSG_RESULT([$scorep_compiler_sx])
AS_IF([test "x${scorep_compiler_sx}" = "xyes"], 
      [scorep_compiler_instrumentation_cppflags=""]
       AC_DEFINE([FORTRAN_MANGLED(var)], [sx compiler's Fortran mangling not implemented yet, see ac_scorep_compiler_checks.m4], 
                 [Name of var after mangled by the Fortran compiler.]))
])

## 

AC_DEFUN([AC_SCOREP_COMPILER_CHECKS],[
scorep_compiler_unknown="yes"

scorep_compiler_intel="no"
scorep_compiler_sun="no"
scorep_compiler_ibm="no"
scorep_compiler_pgi="no"
scorep_compiler_gnu="no"
scorep_compiler_hp="no"
scorep_compiler_sx="no"

scorep_compiler_instrumentation_cppflags=""

# I (croessel) don't think that more than one test can possibly succeed,
# so I skip extra testing here.
AC_LANG_PUSH([C])
SCOREP_COMPILER_INTEL
SCOREP_COMPILER_SUN
SCOREP_COMPILER_IBM
SCOREP_COMPILER_PGI
SCOREP_COMPILER_GNU
SCOREP_COMPILER_HP
SCOREP_COMPILER_SX
AC_LANG_POP([C])

if test "x${scorep_compiler_unknown}" = "xyes"; then
    AC_MSG_WARN([Could not determine compiler vendor. Compiler instrumentation may not work.])
fi

AC_SUBST([COMPILER_INSTRUMENTATION_CPPFLAGS], [${scorep_compiler_instrumentation_cppflags}])

AM_CONDITIONAL([SCOREP_COMPILER_INTEL], [test "x${scorep_compiler_intel}" = "xyes"])
AM_CONDITIONAL([SCOREP_COMPILER_SUN],   [test "x${scorep_compiler_sun}"   = "xyes"])
AM_CONDITIONAL([SCOREP_COMPILER_IBM],   [test "x${scorep_compiler_ibm}"   = "xyes"])
AM_CONDITIONAL([SCOREP_COMPILER_PGI],   [test "x${scorep_compiler_pgi}"   = "xyes"])
AM_CONDITIONAL([SCOREP_COMPILER_GNU],   [test "x${scorep_compiler_gnu}"   = "xyes"])
AM_CONDITIONAL([SCOREP_COMPILER_HP],    [test "x${scorep_compiler_hp}"    = "xyes"])
AM_CONDITIONAL([SCOREP_COMPILER_SX],    [test "x${scorep_compiler_sx}"    = "xyes"])
])
