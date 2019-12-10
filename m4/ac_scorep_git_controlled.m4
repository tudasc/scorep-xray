## -*- mode: autoconf -*-

##
## This file is part of the Score-P software (http://www.score-p.org)
##
## Copyright (c) 2009-2011,
## RWTH Aachen University, Germany
##
## Copyright (c) 2009-2011,
## Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
##
## Copyright (c) 2009-2011, 2019,
## Technische Universitaet Dresden, Germany
##
## Copyright (c) 2009-2011,
## University of Oregon, Eugene, USA
##
## Copyright (c) 2009-2011, 2015,
## Forschungszentrum Juelich GmbH, Germany
##
## Copyright (c) 2009-2011,
## German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
##
## Copyright (c) 2009-2011,
## Technische Universitaet Muenchen, Germany
##
## This software may be modified and distributed under the terms of
## a BSD-style license.  See the COPYING file in the package base
## directory for details.
##

AC_DEFUN([AC_SCOREP_GIT_CONTROLLED],
[
ac_scorep_git_controlled="no"

# test if ${srcdir} is a git top-level, not any parent directory:
# * if ${srcdir} is a top-level, than the prefix is empty (e.g., we are git controlled)
# * if ${srcdir} is below a top-level, than it wont be empty (e.g., we operate in a
#   tarball, which was extracted below a top-level)
# * if git could not find any top-level, it prints an error to stderr and stop,
#   we catch this error, which makes the test also fail (e.g., we operate in a
#   tarball which is *not* below any top-level)
AS_IF([test -z "$(cd ${srcdir} && git rev-parse --show-prefix 2>&1)"],
      [ac_scorep_git_controlled="yes"
       AC_DEFINE([SCOREP_IN_DEVELOPEMENT], [], [Defined if we are working from git.])],
      [AC_DEFINE([SCOREP_IN_PRODUCTION], [], [Defined if we are working from a make dist generated tarball.])])
AM_CONDITIONAL([GIT_CONTROLLED], [test "x${ac_scorep_git_controlled}" = xyes])
])
