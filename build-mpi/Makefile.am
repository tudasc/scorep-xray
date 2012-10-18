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

## file       build-mpi/Makefile.am 
## maintainer Christian Roessel <c.roessel@fz-juelich.de>


## ACLOCAL_AMFLAGS contains options to pass to aclocal when aclocal.m4
## is to be rebuilt by make. This line is also used by autoreconf to
## run aclocal with suitable options, or by autopoint and gettextize
## to locate the place where Gettext's macros should be installed. So
## even if you do not really care about the rebuild rules, you should
## define ACLOCAL_AMFLAGS.
## For some reason this can't be moved to common.am.
ACLOCAL_AMFLAGS = -I ../vendor/common/build-config/m4

include ../build-includes/common.am
include ../vendor/common/build-config/Makefile.tests-mpi.inc.am
include ../vendor/common/build-config/Makefile.tests-mpi-omp.inc.am

AM_CPPFLAGS += -DBACKEND_BUILD_MPI

bindir = @bindir@@backend_suffix@
libdir = @libdir@@backend_suffix@

include ../build-includes/mpi-backend-only.am

if CROSS_BUILD
AM_CPPFLAGS += -DCROSS_BUILD
else !CROSS_BUILD
AM_CPPFLAGS += -DNOCROSS_BUILD
endif !CROSS_BUILD

## Determine how to handle tests
if CROSS_BUILD
check-mpi:     check-file-mpi
check-mpi-omp: check-file-mpi-omp
else !CROSS_BUILD
if BACKEND_TEST_RUNS
check-mpi:     check-run-mpi
check-mpi-omp: check-run-mpi-omp
else !BACKEND_TEST_RUNS
check-mpi:     check-file-mpi
check-mpi-omp: check-file-mpi-omp
endif !BACKEND_TEST_RUNS
endif !CROSS_BUILD


## Here all libscorep_*.la are build
LIB_DIR_SCOREP = ../build-backend/

clean-local: clean-local-instrumenter-checks
	rm -f scorep_mpi_omp_tests  scorep_mpi_tests lex.yy.c scanner.h yacc.c yacc.h y.tab.h

distclean-local:
	rm -f mpi_supported
	rm -rf ../test/jacobi/MPI
	rm -rf ../test/jacobi/hybrid


if HAVE_MPI_SUPPORT
if HAVE_OPENMP_SUPPORT
installcheck-local: instrumenter-checks
	@$(am__cd) ../test/jacobi/MPI/C    && $(MAKE) $(AM_MAKEFLAGS)
	@$(am__cd) ../test/jacobi/hybrid/C && $(MAKE) $(AM_MAKEFLAGS)
else
installcheck-local: instrumenter-checks
	@$(am__cd) ../test/jacobi/MPI/C    && $(MAKE) $(AM_MAKEFLAGS)
endif
else
installcheck-local:
endif


if HAVE_MPI_SUPPORT
PARADIGMS = mpi
if HAVE_OPENMP_SUPPORT
PARADIGMS += mpi_omp
endif
endif

instrumenter-checks: $(instrumenter_configuration_files)
	@for paradigm in $(PARADIGMS); do \
            $(MAKE) -C ../installcheck/$$paradigm instrument-configurations || exit 1; \
        done

instrumenter_configuration_files = ../installcheck/configurations_mpi ../installcheck/configurations_mpi_omp

$(instrumenter_configuration_files):
	@../installcheck/instrumenter-configurations.sh

clean-local-instrumenter-checks:
	@rm -f $(instrumenter_configuration_files)
	@for paradigm in $(PARADIGMS); do \
            rm -f ../installcheck/bin/*-$${paradigm}-*; \
            rm -rf ../installcheck/$$paradigm/build; \
        done
