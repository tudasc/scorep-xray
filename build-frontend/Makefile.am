## -*- mode: makefile -*-

##
## This file is part of the Score-P software (http://www.score-p.org)
##
## Copyright (c) 2009-2011,
## RWTH Aachen University, Germany
##
## Copyright (c) 2009-2011,
## Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
##
## Copyright (c) 2009-2011, 2015,
## Technische Universitaet Dresden, Germany
##
## Copyright (c) 2009-2011,
## University of Oregon, Eugene, USA
##
## Copyright (c) 2009-2015, 2017,
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

## file       build-frontend/Makefile.am

## Options passed to aclocal to specify local macro directories. Although
## ACLOCAL_AMFLAGS should be replaced by AC_CONFIG_MACRO_DIRS, aclocal-1.13.4
## doesn't honor the paths provided to AC_CONFIG_MACRO_DIRS. See
## https://debbugs.gnu.org/cgi/bugreport.cgi?bug=21352
ACLOCAL_AMFLAGS = -I ../build-config/m4 -I ../vendor/common/build-config/m4

include ../build-includes/common.am
include ../vendor/common/build-config/Makefile.tests-serial.inc.am
#include ../vendor/common/build-config/Makefile.tests-omp.inc.am
check-run-omp:
check-file-omp:

LIB_DIR_SCOREP = ../build-backend/
LIB_DIR_SCOREP_MPI = ../build-mpi/
LIB_DIR_SCOREP_SHMEM = ../build-shmem/
LIB_DIR_SCOREP_SCORE = ../build-score/

# Build external targets - i.e., scorep targets that are built in
# different build-* directories - at once, using just a single rule:
# 'build_external_targets'. We do this to prevent make -j race
# conditons.
# In 'build_external_targets' we trigger the external Makefiles one
# after the other. In these external Makefiles itself we have
# parallelism. Reducing the potential of races comes at the cost of
# triggering the external Makefiles even if the targets are still
# up-to-date.
# There is still the potential for race conditions if we introduce
# circular dependencies between Makefiles, i.e., an external target
# triggered by 'build_external_targets' triggers back the originator
# Makefile (where there is potentially still parallelism). If this is
# the case and the dependency could not be resolved one could add some
# of the 'build_external_targets' to BUILT_SOURCES as there is less
# parallelism in 'all' compared to 'all-am'.
MPI_TARGETS =
SHMEM_TARGETS =
BACKEND_TARGETS =
SCORE_TARGETS =
$(MPI_TARGETS) $(SHMEM_TARGETS) $(BACKEND_TARGETS) $(SCORE_TARGETS): build_external_targets
build_external_targets:
	@targets=`echo $(MPI_TARGETS) | sed 's|^[ ^t]*||;s|[ ^]*$$||;s|$(LIB_DIR_SCOREP_MPI)||g'`; \
	if test "x$$targets" != x; then \
		$(am__cd) $(LIB_DIR_SCOREP_MPI) && $(MAKE) $(AM_MAKEFLAGS) $$targets; \
	fi
	@targets=`echo $(SHMEM_TARGETS) | sed 's|^[ ^t]*||;s|[ ^]*$$||;s|$(LIB_DIR_SCOREP_SHMEM)||g'`; \
	if test "x$$targets" != x; then \
		$(am__cd) $(LIB_DIR_SCOREP_SHMEM) && $(MAKE) $(AM_MAKEFLAGS) $$targets; \
	fi
	@targets=`echo $(BACKEND_TARGETS) | sed 's|^[ ^t]*||;s|[ ^]*$$||;s|$(LIB_DIR_SCOREP)||g'`; \
	if test "x$$targets" != x; then \
		$(am__cd) $(LIB_DIR_SCOREP) && $(MAKE) $(AM_MAKEFLAGS) $$targets; \
	fi
	@targets=`echo $(SCORE_TARGETS) | sed 's|^[ ^t]*||;s|[ ^]*$$||;s|$(LIB_DIR_SCOREP_SCORE)||g'`; \
	    if test "x$$targets" != x; then \
	    $(am__cd) $(LIB_DIR_SCOREP_SCORE) && $(MAKE) $(AM_MAKEFLAGS) $$targets; \
	fi

AM_CPPFLAGS += -DFRONTEND_BUILD -DCROSS_BUILD

include ../build-includes/frontend-only.am
include ../build-includes/front-and-backend.am

check-serial: check-run-serial
check-omp:    check-run-omp
