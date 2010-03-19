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

AC_DEFUN([AC_SILC_SVN_CONTROLLED],
[
    ac_silc_svn_controlled="no"
    if test -d $srcdir/.svn; then
        ac_silc_svn_controlled="yes"
        AC_DEFINE([SILC_IN_DEVELOPEMENT], [], [Defined if we are working from svn.])
    else
        AC_DEFINE([SILC_IN_PRODUCTION], [], [Defined if we are working from a make dist generated tarball.])
    fi
    AM_CONDITIONAL(SVN_CONTROLLED, test "x${ac_silc_svn_controlled}" = xyes)  
])
