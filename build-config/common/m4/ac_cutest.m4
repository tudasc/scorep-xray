## -*- mode: autoconf -*-

##
## This file is part of the Score-P software ecosystem (http://www.score-p.org)
##
## Copyright (c) 2009-2011,
## RWTH Aachen University, Germany
##
## Copyright (c) 2009-2011,
## Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
##
## Copyright (c) 2009-2011,
## Technische Universitaet Dresden, Germany
##
## Copyright (c) 2009-2011,
## University of Oregon, Eugene, USA
##
## Copyright (c) 2009-2011,
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


AC_DEFUN([AC_CUTEST_COLOR_TESTS],
[

AC_CHECK_HEADER(
    [unistd.h],
    [AC_CHECK_DECL(
        [STDOUT_FILENO],
        [AC_CHECK_FUNC(
            [isatty],
            [AC_DEFINE(
                [CUTEST_USE_COLOR],
                [1],
                [Try to use colorful output for tests.])])
        ],
        [],
        [#include <unistd.h>])])

])
