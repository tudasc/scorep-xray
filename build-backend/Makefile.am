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
## Copyright (c) 2009-2011, 2013-2017,
## Technische Universitaet Dresden, Germany
##
## Copyright (c) 2009-2011,
## University of Oregon, Eugene, USA
##
## Copyright (c) 2009-2015, 2017-2018,
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

## file       build-backend/Makefile.am


## Options passed to aclocal to specify local macro directories. Although
## ACLOCAL_AMFLAGS should be replaced by AC_CONFIG_MACRO_DIRS, aclocal-1.13.4
## doesn't honor the paths provided to AC_CONFIG_MACRO_DIRS. See
## https://debbugs.gnu.org/cgi/bugreport.cgi?bug=21352
ACLOCAL_AMFLAGS = -I ../build-config/m4 -I ../vendor/common/build-config/m4

include ../build-includes/common.am
include ../vendor/common/build-config/Makefile.tests-serial.inc.am
include ../vendor/common/build-config/Makefile.tests-omp.inc.am

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
SCORE_TARGETS =
MPI_TARGETS =
SHMEM_TARGETS =
$(MPI_TARGETS) $(SHMEM_TARGETS) $(SCORE_TARGETS): build_external_targets
build_external_targets:
	@targets=`echo $(MPI_TARGETS) | sed 's|^[ ^t]*||;s|[ ^]*$$||;s|$(LIB_DIR_SCOREP_MPI)||g'`; \
	if test "x$$targets" != x; then \
		$(am__cd) $(LIB_DIR_SCOREP_MPI) && $(MAKE) $(AM_MAKEFLAGS) $$targets; \
	fi
	@targets=`echo $(SHMEM_TARGETS) | sed 's|^[ ^t]*||;s|[ ^]*$$||;s|$(LIB_DIR_SCOREP_SHMEM)||g'`; \
	if test "x$$targets" != x; then \
		$(am__cd) $(LIB_DIR_SCOREP_SHMEM) && $(MAKE) $(AM_MAKEFLAGS) $$targets; \
	fi
	@targets=`echo $(SCORE_TARGETS) | sed 's|^[ ^t]*||;s|[ ^]*$$||;s|$(LIB_DIR_SCOREP_SCORE)||g'`; \
	if test "x$$targets" != x; then \
		$(am__cd) $(LIB_DIR_SCOREP_SCORE) && $(MAKE) $(AM_MAKEFLAGS) $$targets; \
	fi
MPI_CHECK_TARGETS =
SHMEM_CHECK_TARGETS =
$(MPI_CHECK_TARGETS) $(SHMEM_CHECK_TARGETS): build_external_check_targets
build_external_check_targets:
	@targets=`echo $(MPI_CHECK_TARGETS) | sed 's|^[ ^t]*||;s|[ ^]*$$||;s|$(LIB_DIR_SCOREP_MPI)||g'`; \
	if test "x$$targets" != x; then \
		$(am__cd) $(LIB_DIR_SCOREP_MPI) && $(MAKE) $(AM_MAKEFLAGS) $$targets; \
	fi
	@targets=`echo $(SHMEM_CHECK_TARGETS) | sed 's|^[ ^t]*||;s|[ ^]*$$||;s|$(LIB_DIR_SCOREP_SHMEM)||g'`; \
	if test "x$$targets" != x; then \
		$(am__cd) $(LIB_DIR_SCOREP_SHMEM) && $(MAKE) $(AM_MAKEFLAGS) $$targets; \
	fi

AM_CPPFLAGS += -DBACKEND_BUILD_NOMPI

frontend_bindir = @bindir@
bindir = @bindir@@backend_suffix@
libdir = @libdir@@backend_suffix@

## Additional libtool linking flags
LINKMODE_FLAGS = @AFS_LINKMODE_LDFLAGS@

# for backend tools which are generated in this build-backend, e.g., scorep-wrapper
frontend_bin_SCRIPTS =

backend_pkglibexecdir = $(pkglibexecdir)@backend_suffix@
backend_pkglibexec_SCRIPTS = libtool

if CROSS_BUILD
AM_CPPFLAGS += -DCROSS_BUILD
include ../build-includes/backend-only.am
include ../build-includes/front-and-backend.am
else !CROSS_BUILD
AM_CPPFLAGS += -DNOCROSS_BUILD
include ../build-includes/backend-only.am
include ../build-includes/front-and-backend.am
include ../build-includes/frontend-only.am
endif !CROSS_BUILD

# src/config-backend-for-frontend.h dependencies
if CROSS_BUILD
if SVN_CONTROLLED

$(srcdir)/../src/config-backend-for-frontend.h.in: $(srcdir)/../src/config-backend.h.in
	$(AM_V_GEN)$(srcdir)/../vendor/common/build-config/generate-config-backend-for-frontend.sh $< >$@

endif SVN_CONTROLLED
endif CROSS_BUILD


## Determine how to handle tests
if CROSS_BUILD
check-serial: check-file-serial
check-omp:    check-file-omp
else !CROSS_BUILD
if BACKEND_TEST_RUNS
check-serial: check-run-serial
check-omp:    check-run-omp
else !BACKEND_TEST_RUNS
check-serial: check-file-serial
check-omp:    check-file-omp
endif !BACKEND_TEST_RUNS
endif !CROSS_BUILD


## Here all libscorep_*.la are build
LIB_DIR_SCOREP = ./

## Here the MPI convenience libraries are build
LIB_DIR_SCOREP_MPI = ../build-mpi/

## Here the SHMEM convenience libraries are build
LIB_DIR_SCOREP_SHMEM = ../build-shmem/

LIB_DIR_SCOREP_SCORE = ../build-score/

CLEANFILES += lex.yy.c scanner.h yacc.c yacc.h y.tab.h

CLEAN_LOCAL += clean-local-scorep-experiment-dirs
clean-local-scorep-experiment-dirs:
	rm -rf scorep-2*
	rm -rf scorep-measurement-tmp
	rm -f serial_inst_test
	rm -rf rewind-test-dir
	rm -rf scorep-serial-cuda
	rm -rf scorep-serial-opencl
	rm -rf scorep-serial-*-metric-test-dir

CLEAN_LOCAL += clean-local-scorep-config-tests
clean-local-scorep-config-tests:
	$(RM) test_scorep_config_number.*.out
	$(RM) test_scorep_config_size.*.out

install-data-local: $(INSTALL_DATA_LOCAL_TARGETS)

install-data-hook: $(INSTALL_DATA_HOOK_TARGETS)

PARADIGMS = serial
# component separation will provide the HAVE_OPARI2 conditional
#if HAVE_OPARI2
if HAVE_OPENMP_SUPPORT
PARADIGMS += omp
endif
#endif
if HAVE_CUDA_SUPPORT
PARADIGMS += cuda
endif HAVE_CUDA_SUPPORT

INSTALLCHECK_LOCAL += instrumenter-checks
instrumenter-checks: $(instrumenter_configuration_files)
	@fail= failcom='exit 1'; \
	for f in x $$MAKEFLAGS; do \
	  case $$f in \
	    *=* | --[!k]*);; \
	    *k*) failcom='fail=yes';; \
	  esac; \
	done; \
	list='$(PARADIGMS)'; for paradigm in $$list; do \
	  echo "Making instrument-configurations in ../installcheck/instrumenter_checks/$$paradigm"; \
	  ($(am__cd) ../installcheck/instrumenter_checks/$$paradigm && $(MAKE) $(AM_MAKEFLAGS) instrument-configurations) \
	  || eval $$failcom; \
	done; test -z "$$fail"

instrumenter_configuration_files = ../installcheck/instrumenter_checks/configurations_serial ../installcheck/instrumenter_checks/configurations_omp
if HAVE_CUDA_SUPPORT
instrumenter_configuration_files += ../installcheck/instrumenter_checks/configurations_cuda
endif HAVE_CUDA_SUPPORT

$(instrumenter_configuration_files): ../installcheck/instrumenter_checks/instrumenter-configurations.sh
	@../installcheck/instrumenter_checks/instrumenter-configurations.sh

CLEAN_LOCAL += clean-local-instrumenter-checks
clean-local-instrumenter-checks:
	@rm -f $(instrumenter_configuration_files)
	@for paradigm in $(PARADIGMS); do \
            rm -f ../installcheck/instrumenter_checks/bin/*-$${paradigm}-*; \
            rm -rf ../installcheck/instrumenter_checks/$$paradigm/build; \
        done

if HAVE_PTHREAD_SUPPORT

INSTALLCHECK_LOCAL += instrumenter-checks-pthread
instrumenter-checks-pthread: ../installcheck/instrumenter_checks/create_wait/pthread/Makefile \
                             ../installcheck/instrumenter_checks/check-instrumentation.sh
	@$(MAKE) -C ../installcheck/instrumenter_checks/create_wait/pthread

CLEAN_LOCAL += clean-local-instrumenter-checks-pthread
clean-local-instrumenter-checks-pthread: ../installcheck/instrumenter_checks/create_wait/pthread/Makefile
	@$(MAKE) -C ../installcheck/instrumenter_checks/create_wait/pthread clean

endif HAVE_PTHREAD_SUPPORT

if HAVE_MEMORY_SUPPORT

INSTALLCHECK_LOCAL += instrumenter-checks-memory
instrumenter-checks-memory: ../installcheck/instrumenter_checks/memory/Makefile \
                            ../installcheck/instrumenter_checks/check-instrumentation.sh
	@$(MAKE) -C ../installcheck/instrumenter_checks/memory

CLEAN_LOCAL += clean-local-instrumenter-checks-memory
clean-local-instrumenter-checks-memory: ../installcheck/instrumenter_checks/memory/Makefile
	@$(MAKE) -C ../installcheck/instrumenter_checks/memory clean

endif HAVE_MEMORY_SUPPORT

if HAVE_POSIX_IO_SUPPORT

INSTALLCHECK_LOCAL += instrumenter-checks-io
instrumenter-checks-io: ../installcheck/instrumenter_checks/io/Makefile \
                        ../installcheck/instrumenter_checks/check-instrumentation.sh
	@$(MAKE) -C ../installcheck/instrumenter_checks/io

CLEAN_LOCAL += clean-local-instrumenter-checks-io
clean-local-instrumenter-checks-io: ../installcheck/instrumenter_checks/io/Makefile
	@$(MAKE) -C ../installcheck/instrumenter_checks/io clean

endif HAVE_POSIX_IO_SUPPORT
