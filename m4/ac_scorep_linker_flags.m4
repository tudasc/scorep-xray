## -*- mode: autoconf -*-

##
## This file is part of the Score-P software (http://www.score-p.org)
##
## Copyright (c) 2009-2013,
## RWTH Aachen University, Germany
##
## Copyright (c) 2009-2013,
## Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
##
## Copyright (c) 2009-2014,
## Technische Universitaet Dresden, Germany
##
## Copyright (c) 2009-2013,
## University of Oregon, Eugene, USA
##
## Copyright (c) 2009-2013,
## Forschungszentrum Juelich GmbH, Germany
##
## Copyright (c) 2009-2013,
## German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
##
## Copyright (c) 2009-2013,
## Technische Universitaet Muenchen, Germany
##
## This software may be modified and distributed under the terms of
## a BSD-style license.  See the COPYING file in the package base
## directory for details.
##

AC_DEFUN([AC_SCOREP_CHECK_LINKER_FLAGS], [
    AC_LANG_PUSH(C)
    has_linker_start_end_group="yes"
    lflags_save_flags=$LDFLAGS
    LDFLAGS="$LDFLAGS -Wl,-start-group -Wl,-end-group"
    AC_MSG_CHECKING([for start/end-group linker flags])
    AC_LINK_IFELSE([
                    AC_LANG_SOURCE([
                        int main()
                        {
                            return 0;
                        }
                    ])],
                    [AC_MSG_RESULT(yes);
                    AC_DEFINE(HAVE_LINKER_START_END_GROUP, 1, [Check for start/end-group linker flags])],
                    [AC_MSG_RESULT(no)
                    has_linker_start_end_group="no"])
    LDFLAGS=$lflags_save_flags
    AC_LANG_POP(C)
    AM_CONDITIONAL([HAVE_LINKER_START_END_GROUP], [test "x${has_linker_start_end_group}" = "xyes"])
])


