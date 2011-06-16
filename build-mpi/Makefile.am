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

AM_CPPFLAGS += -DBACKEND_BUILD_MPI

bindir=${exec_prefix}@backend_bin_dir@
libdir=${exec_prefix}@backend_lib_dir@


include ../build-includes/mpi-backend-only.am

if CROSS_BUILD

AM_CPPFLAGS += -DCROSS_BUILD

else

AM_CPPFLAGS += -DNOCROSS_BUILD

endif

#if BACKEND_TEST_RUNS
#TESTS = $(SCOREP_MPI_TESTS) $(SCOREP_MPI_OMP_TESTS)
#XFAIL_TESTS = $(SCOREP_MPI_XFAIL_TESTS) $(SCOREP_MPI_OMP_XFAIL_TESTS)
#endif


# I'm not sure if this is the way to go or if we need per 
# target _LDADD = $(MPILIBS)
#LIBS += $(MPILIBS)


#if !BACKEND_TEST_RUNS
BUILT_SOURCES += scorep_mpi_tests scorep_mpi_omp_tests

if MPI_SUPPORTED
scorep_mpi_tests:
	rm -f scorep_mpi_tests
	echo "# Available MPI tests:" > scorep_mpi_tests
	for i in $(SCOREP_MPI_TESTS); do echo "$$i" >> scorep_mpi_tests; done

if OPENMP_SUPPORTED
scorep_mpi_omp_tests:
	rm -f scorep_mpi_omp_tests
	echo "# Available hybrid tests:" > scorep_mpi_omp_tests
	for i in $(SCOREP_MPI_OMP_TESTS); do echo "$$i" >> scorep_mpi_omp_tests; done

else
scorep_mpi_omp_tests:
endif
else
scorep_mpi_tests:
scorep_mpi_omp_tests:
endif
#endif


## Convenience variable, used in common.am. It is either build-backend
## or build-frontend and used to refrence otf2 and utility variables.
BUILD_DIR=build-backend

## Here all libscorep_*.la are build
LIB_DIR_SCOREP = ../build-backend/

clean-local:
	rm -f scorep_mpi_omp_tests  scorep_mpi_tests lex.yy.c scanner.h yacc.c yacc.h

distclean-local:
	rm -f mpi_supported
	rm -rf ../test/jacobi/MPI
	rm -rf ../test/jacobi/hybrid


if MPI_SUPPORTED
if OPENMP_SUPPORTED
installcheck-local:
	(cd ../test/jacobi/MPI/C && $(MAKE) $(AM_MAKEFLAGS)) || exit 1;
	(cd ../test/jacobi/hybrid/C && $(MAKE) $(AM_MAKEFLAGS)) || exit 1;
else
installcheck-local:
	(cd ../test/jacobi/MPI/C && $(MAKE) $(AM_MAKEFLAGS)) || exit 1;
endif
else
installcheck-local:
endif
