## -*- mode: autoconf -*-

## 
## This file is part of the Score-P software (http://www.score-p.org)
##
## Copyright (c) 2009-2011, 
##    RWTH Aachen University, Germany
##    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
##    Technische Universitaet Dresden, Germany
##    University of Oregon, Eugene, USA
##    Forschungszentrum Juelich GmbH, Germany
##    German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
##    Technische Universitaet Muenchen, Germany
##
## See the COPYING file in the package base directory for details.
##

AC_DEFUN([AC_SCOREP_CUBE4_FRONTEND], [

## Evalute parameters
AC_ARG_WITH(cube4-frontend-lib, [AS_HELP_STRING([--with-cube4-frontend-lib=path_to_library], [Specifies the path where the Cube 4 frontend library is located])],[
    AC_SUBST(CUBE_FRONTEND_LIB_PATH,"$withval")
    LDFLAGS="$LDFLAGS -L$withval -R$withval"
],[])
AC_ARG_WITH(cube4-frontend-header, [AS_HELP_STRING([--with-cube4-frontend-header=path_to_header], [Specifies the path where the Cube 4 frontend header files are located])],[CPPFLAGS="$CPPFLAGS -I$withval"],[])

## Check presence of cube reader library
AC_LANG_PUSH([C++])
AC_MSG_CHECKING([for libcube4])    
scorep_save_libs=$LIBS
CUBE_FRONTEND_LIBS="-lcube4 -lz -lm"
LIBS="$LIBS $CUBE_FRONTEND_LIBS"
AC_LINK_IFELSE([AC_LANG_PROGRAM([[#include <Cube.h>]],
                                [[cube::Cube mycube;]])],[has_cube4_frontend_lib=yes],[CUBE_FRONTEND_LIBS=""])
AC_MSG_RESULT([$CUBE_FRONTEND_LIBS])

## Check presence of cube reader header
AC_CHECK_HEADER([Cube.h], [has_cube4_frontend_header=yes], [], [])

## Set makefile conditional
AM_CONDITIONAL(HAVE_CUBE4_FRONTEND,[test x$has_cube4_frontend_lib$has_cube4_frontend_header = xyesyes])
AC_SUBST(CUBE_FRONTEND_LIBS, "$CUBE_FRONTEND_LIBS")

## Clean up
LIBS=$scorep_save_libs
AC_LANG_POP([C++])
])


