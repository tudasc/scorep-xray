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


AC_DEFUN([SILC_LIBBFD_LINK_TEST], [
AC_LINK_IFELSE([AC_LANG_PROGRAM([[char bfd_init ();
char bfd_openr ();
char bfd_check_format ();
char bfd_close ();]],
                                [[bfd_init ();
bfd_openr ();
bfd_check_format ();
bfd_close ();]])],
               [silc_have_libbfd="yes"], 
               [silc_have_libbfd="no"])
])


AC_DEFUN([AC_SILC_LIBBFD], [
AC_REQUIRE([AC_SILC_COMPILER_CHECKS])
# In scalasca we use bfd only for the gnu compiler instrumentation.

# What shall we do if there is a frontend and a backend libbfd? We need the
# backend version, but usually the frontend version will be found. One
# approach is to require user input on dual-architecture machines. But for now
# we will just abort (until this problem shows up in reality).

if test "x${silc_compiler_gnu}" = "xyes"; then

    if test "x${ac_silc_cross_compiling}" = "xyes"; then
        AC_MSG_ERROR([Can't reliably determine backend libbfd in cross compiling mode.])
    fi

    AC_LANG_PUSH([C])
    AC_CHECK_HEADER([bfd.h])
    
    AC_MSG_CHECKING([for libbfd])    
    silc_libbfd_save_LIBS="$LIBS"

    LIBS="-lbfd"
    SILC_LIBBFD_LINK_TEST
    AS_IF([test "x${silc_have_libbfd}" = "xno"],
          [LIBS="-lbfd -liberty"; 
           SILC_LIBBFD_LINK_TEST
           AS_IF([test "x${silc_have_libbfd}" = "xno"],
                 [LIBS="-lbfd -liberty -lz";
                  SILC_LIBBFD_LINK_TEST
                  AS_IF([test "x${silc_have_libbfd}" = "xno"],
                        [AC_MSG_RESULT([no])],
                        [AC_MSG_RESULT([$LIBS])])],
                 [AC_MSG_RESULT([$LIBS])])],
          [AC_MSG_RESULT([$LIBS])])
    
    silc_bfd_libs="$LIBS"
    LIBS="$silc_libbfd_save_LIBS"

    AM_CONDITIONAL([HAVE_LIBBFD], [test "x${ac_cv_header_bfd_h}" = "xyes" && test "x${silc_have_libbfd}" = "xyes"])
    if test "x${ac_cv_header_bfd_h}" = "xno" || test "x${silc_have_libbfd}" = "xno"; then
        AC_MSG_WARN([libbfd not available. Compiler instrumentation will not work.])
        AC_SUBST([LIBBFD], [""])
    else
        AC_SUBST([LIBBFD], ["$silc_bfd_libs"])
    fi
    AC_LANG_POP([C])
else
    AM_CONDITIONAL([HAVE_LIBBFD], [test 1 -ne 1])
    AC_SUBST([LIBBFD], [""])
fi
])
