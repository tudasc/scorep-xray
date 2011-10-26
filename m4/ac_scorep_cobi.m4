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


AC_DEFUN([AC_SCOREP_COBI], [

AC_MSG_CHECKING([for cobi ])

# Evalute command line parameters
AC_ARG_WITH(cobi, [AS_HELP_STRING([--with-cobi=yes|no|path_to_binary], [Specifies whether Score-P ist build with support for binary instrumentation with cobi. You can either disable cobi-support, specify a path to the binary or let configure determine the path itself. In the last case, cobi must be available in the current PATH environment.])],[
    cobi_path=$withval/cobi
    have_cobi=yes
],[have_cobi=no])

# If --with-cobi is not explicitly specified, cobi support is not enabled. 
# If we want to enable automatic configuration of all possible instrumentation methods
# we can check whether we find a cobi in the path if  $withval is yes 
# and $have_cobi is no.

# If cobe is enabled, test if shared libraries are build.
if [ test "x$enable_shared$have_cobi" = "xnoyes" ]; then
    AC_MSG_RESULT([ prerequisites missing])
    AC_MSG_ERROR([Cobi support is only possible when shared libraries are enabled. Please, enable building shared libraries by specifying --enable-shared or disable cobi-support via --with-cobi=no])
    have_cobi=no
fi

# If no specific path to cobi is given, try to find cobi from in the PATH.
if [ test "x$cobi_path" = "xyes/cobi" ]; then
    cobi_path="`which cobi 2>/dev/null`"
    if [ test "x$cobi_path" = "x" ]; then
       AC_MSG_RESULT([ not found])
       have_cobi=no
    else
       AC_MSG_RESULT([$cobi_path])
    fi
else
    if [ test x$have_cobi = xyes]; then 
       AC_MSG_RESULT([ $cobi_path])
    else
       AC_MSG_RESULT([ no cobi support])
    fi
fi

# Output results
AM_CONDITIONAL(HAVE_COBI,[test x$have_cobi = xyes])

if [test "x$have_cobi" = "xyes" ]; then
    AC_DEFINE(HAVE_COBI, 1, "Defined if configured to use Cobi." )
    AC_DEFINE_UNQUOTED(SCOREP_COBI_PATH, "$cobi_path", "Defines path to Cobi." )
else
    AC_DEFINE(SCOREP_COBI_PATH, "cobi", "No Cobi path defined." )
fi
])
