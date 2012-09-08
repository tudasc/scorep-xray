# -*- mode: makefile -*-

## 
## This file is part of the Score-P software (http://www.score-p.org)
##
## Copyright (c) 2009-2011, 
##    RWTH Aachen, Germany
##    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
##    Technische Universitaet Dresden, Germany
##    University of Oregon, Eugene, USA
##    Forschungszentrum Juelich GmbH, Germany
##    German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
##    Technische Universitaet Muenchen, Germany
##
## See the COPYING file in the package base directory for details.
##

## file       build-score/Makefile.am 
## maintainer Christian Roessel <c.roessel@fz-juelich.de>

ACLOCAL_AMFLAGS = -I ../vendor/common/build-config/m4

include ../build-includes/common.am

AM_CPPFLAGS += -DSCORE_BUILD

include ../src/tools/score/Makefile.inc.am

include ../src/utils/filtering/Makefile.inc.am
include ../vendor/common/src/utils/cstr/Makefile.inc.am
include ../vendor/common/src/utils/io/Makefile.inc.am
include ../vendor/common/src/utils/exception/Makefile.inc.am
include ../vendor/common/src/utils/Makefile.inc.am
