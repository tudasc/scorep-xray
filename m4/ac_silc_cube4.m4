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

AC_DEFUN([AC_SILC_CUBE4], [

## Evalute parameters
AC_ARG_WITH(cube4-lib, [AS_HELP_STRING([--with-cube4-lib=path_to_library], [Specifies the path where the Cube 4 library is located])],[LDFLAGS="$LDFLAGS -L$withval"],[])
AC_ARG_WITH(cube4-header, [AS_HELP_STRING([--with-cube4-header=path_to_header], [Specifies the path where the Cube 4 header files are located])],[CPPFLAGS="$CPPFLAGS -I$withval"],[])

## Check presence of cube writer library
AC_LANG_PUSH([C])
AC_MSG_CHECKING([for libcubew])    
silc_save_libs=$LIBS
LIBS="$LIBS -lcubew -lz -lm"
AC_LINK_IFELSE([AC_LANG_PROGRAM([void* cubew_create(unsigned myrank, unsigned Nthreads, unsigned Nwriters, const char * cubename, int compression);],
                                    [[cubew_create(1,1,1,"test",0);]])],[has_cube4_lib=yes],[LIBS=$silc_savelibs])
AC_MSG_RESULT([$LIBS])

## Check presence of cube writer header
AC_CHECK_HEADER([cubew.h], [has_cube4_header=yes], [], [])

## Set makefile conditional
AM_CONDITIONAL(HAVE_CUBE4,[test x$has_cube4_lib$has_cube4_header = xyesyes])

])
