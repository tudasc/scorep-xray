# -*- mode: makefile -*-

## 
## This file is part of the Score-P software (http://www.score-p.org)
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
## Copyright (c) 2009-2013,
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

## file       build-score/Makefile.am 

ACLOCAL_AMFLAGS = -I ../vendor/common/build-config/m4 -I ../build-config/m4

include ../build-includes/common.am

AM_CPPFLAGS += -DSCORE_BUILD

include ../src/tools/score/Makefile.inc.am

include ../src/utils/filtering/Makefile.inc.am
include ../vendor/common/src/utils/cstr/Makefile.inc.am
include ../vendor/common/src/utils/io/Makefile.inc.am
include ../vendor/common/src/utils/exception/Makefile.inc.am
include ../vendor/common/src/utils/Makefile.inc.am
