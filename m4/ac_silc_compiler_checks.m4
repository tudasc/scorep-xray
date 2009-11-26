## -*- mode: autoconf -*-

## 
## This file is part of the SILC project (http://www.silc.de)
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


AC_DEFUN([AC_SILC_COMPILER_INTEL],[
AC_MSG_CHECKING([for intel compiler])
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[]],
[[#if defined(__INTEL_COMPILER) || defined(__ICC)
#else
# error "Not an Intel compiler."
#endif
]])],
                  [ac_silc_compiler_intel="yes"; ac_silc_compiler_unknown="no"], 
                  [ac_silc_compiler_intel="no"])
AC_MSG_RESULT([$ac_silc_compiler_intel])
])

##

AC_DEFUN([AC_SILC_COMPILER_SUN],[
AC_MSG_CHECKING([for sun compiler])
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[]],
[[#if defined(__SUNPRO_C)
#else
# error "Not a Sun compiler."
#endif
]])],
                  [ac_silc_compiler_sun="yes"; ac_silc_compiler_unknown="no"], 
                  [ac_silc_compiler_sun="no"])
AC_MSG_RESULT([$ac_silc_compiler_sun])
])

##

AC_DEFUN([AC_SILC_COMPILER_IBM],[
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
                  [ac_silc_compiler_ibm="yes"; ac_silc_compiler_unknown="no"], 
                  [ac_silc_compiler_ibm="no"])
AC_MSG_RESULT([$ac_silc_compiler_ibm])
])

##

AC_DEFUN([AC_SILC_COMPILER_PGI],[
AC_MSG_CHECKING([for pgi compiler])
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[]],
[[#if defined(__PGI)
#else
# error "Not a PGI compiler."
#endif
]])],
                  [ac_silc_compiler_pgi="yes"; ac_silc_compiler_unknown="no"], 
                  [ac_silc_compiler_pgi="no"])
AC_MSG_RESULT([$ac_silc_compiler_pgi])
])

##

AC_DEFUN([AC_SILC_COMPILER_GNU],[
AC_MSG_CHECKING([for gnu compiler])
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[]],
[[#if defined(__GNUC__)
#else
# error "Not a GNU compiler."
#endif
]])],
                  [ac_silc_compiler_gnu="yes"; ac_silc_compiler_unknown="no"], 
                  [ac_silc_compiler_gnu="no"])
AC_MSG_RESULT([$ac_silc_compiler_gnu])
])

##

AC_DEFUN([AC_SILC_COMPILER_HP],[
AC_MSG_CHECKING([for hp compiler])
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[]],
[[#if defined(__HP_cc) || defined(__hpux) || defined(__hpua)
#else
# error "Not a HP compiler."
#endif
]])],
                  [ac_silc_compiler_hp="yes"; ac_silc_compiler_unknown="no"], 
                  [ac_silc_compiler_hp="no"])
AC_MSG_RESULT([$ac_silc_compiler_hp])
])

## 

AC_DEFUN([AC_SILC_COMPILER_CHECKS],[
ac_silc_compiler_unknown="yes"

ac_silc_compiler_intel="no"
ac_silc_compiler_sun="no"
ac_silc_compiler_ibm="no"
ac_silc_compiler_pgi="no"
ac_silc_compiler_gnu="no"
ac_silc_compiler_hp="no"

# I (croessel) don't think that more than one test can possibly succeed,
# so I skip extra testing here.
AC_SILC_COMPILER_INTEL
AC_SILC_COMPILER_SUN
AC_SILC_COMPILER_IBM
AC_SILC_COMPILER_PGI
AC_SILC_COMPILER_GNU
AC_SILC_COMPILER_HP

if test "x${ac_silc_compiler_unknown}" = "xyes"; then
    AC_MSG_WARN([Could not determine compiler vendor. Compiler instrumentation may not work.])
fi

AM_CONDITIONAL([SILC_COMPILER_INTEL], [test "x${ac_silc_compiler_intel}" = "xyes"])
AM_CONDITIONAL([SILC_COMPILER_SUN],   [test "x${ac_silc_compiler_sun}"   = "xyes"])
AM_CONDITIONAL([SILC_COMPILER_IBM],   [test "x${ac_silc_compiler_ibm}"   = "xyes"])
AM_CONDITIONAL([SILC_COMPILER_PGI],   [test "x${ac_silc_compiler_pgi}"   = "xyes"])
AM_CONDITIONAL([SILC_COMPILER_GNU],   [test "x${ac_silc_compiler_gnu}"   = "xyes"])
AM_CONDITIONAL([SILC_COMPILER_HP],    [test "x${ac_silc_compiler_hp}"    = "xyes"])
])
