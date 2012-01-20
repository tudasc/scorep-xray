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

bindir=${exec_prefix}@backend_bin_dir@
libdir=${exec_prefix}@backend_lib_dir@


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


# I'm not sure if this is the way to go or if we need per 
# target _LDADD = $(MPILIBS)
#LIBS += $(MPILIBS)


## Convenience variable, used in common.am. It is either build-backend
## or build-frontend and used to refrence otf2 and utility variables.
BUILD_DIR=build-backend

## Here all libscorep_*.la are build
LIB_DIR_SCOREP = ../build-backend/

clean-local:
	rm -f scorep_mpi_omp_tests  scorep_mpi_tests lex.yy.c scanner.h yacc.c yacc.h y.tab.h

distclean-local:
	rm -f mpi_supported
	rm -rf ../test/jacobi/MPI
	rm -rf ../test/jacobi/hybrid

## Provide the scorep-config build with information from this mpi Makefile.
$(CONFIG_HEADER_MPI):
	$(AM_V_GEN)( \
	    echo "#define SCOREP_MPI_SION_LIBS     \"$(SCOREP_SION_MPI_LIBS)\""; \
	    echo "#define SCOREP_MPI_SION_LDFLAGS  \"$(SCOREP_SION_MPI_LDFLAGS)\""; \
	    echo "#define SCOREP_MPI_SION_CPPFLAGS \"$(SCOREP_SION_MPI_CPPFLAGS)\""; \
	    echo "#define SCOREP_MPICC             \"$(CC)\"";\
	    echo "#define SCOREP_MPICXX            \"$(CXX)\"";\
	    echo "#define SCOREP_MPIFC             \"$(FC)\"";\
	) >$@


if MPI_SUPPORTED
if OPENMP_SUPPORTED
installcheck-local:
	@$(am__cd) ../test/jacobi/MPI/C    && $(MAKE) $(AM_MAKEFLAGS)
	@$(am__cd) ../test/jacobi/hybrid/C && $(MAKE) $(AM_MAKEFLAGS)
else
installcheck-local:
	@$(am__cd) ../test/jacobi/MPI/C    && $(MAKE) $(AM_MAKEFLAGS)
endif
else
installcheck-local:
endif
