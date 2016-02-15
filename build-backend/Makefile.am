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
## Copyright (c) 2009-2011, 2013-2016,
## Technische Universitaet Dresden, Germany
##
## Copyright (c) 2009-2011,
## University of Oregon, Eugene, USA
##
## Copyright (c) 2009-2015,
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

AM_CPPFLAGS += -DBACKEND_BUILD_NOMPI

pkgdata_DATA += ../share/opencl.wrap \
                ../share/opencl.nvcc.wrap

frontend_bindir = @bindir@
bindir = @bindir@@backend_suffix@
libdir = @libdir@@backend_suffix@

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

SCOREP_SHARE_INSTALL_DIR = $(DESTDIR)@PKGDATADIR@

SCOREP_SHARE_GLOBAL_DIR=$(abs_top_srcdir)/../share
SCOREP_PDT_CONFIG_INSTALL_TARGET = $(SCOREP_SHARE_INSTALL_DIR)/SCOREP_Pdt_Instrumentation.conf

uninstall-local: $(UNINSTALL_LOCAL_TARGETS)
	rm -f $(DESTDIR)$(exec_prefix)/bin/scorep_config.dat \
              $(SCOREP_PDT_CONFIG_INSTALL_TARGET)

INSTALL_DATA_LOCAL_TARGETS += scorep-config-tool-install

scorep-config-tool-install:
	$(MKDIR_P) $(SCOREP_SHARE_INSTALL_DIR)
	$(INSTALL_DATA) $(INC_ROOT)share/SCOREP_Pdt_Instrumentation.conf $(SCOREP_PDT_CONFIG_INSTALL_TARGET)

PARADIGMS = serial
# component separation will provide the HAVE_OPARI2 conditional
#if HAVE_OPARI2
if HAVE_OPENMP_SUPPORT
PARADIGMS += omp
endif
#endif

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

$(instrumenter_configuration_files): ../installcheck/instrumenter_checks/instrumenter-configurations.sh
	@../installcheck/instrumenter_checks/instrumenter-configurations.sh

CLEAN_LOCAL += clean-local-instrumenter-checks
clean-local-instrumenter-checks:
	@rm -f $(instrumenter_configuration_files)
	@for paradigm in $(PARADIGMS); do \
            rm -f ../installcheck/instrumenter_checks/bin/*-$${paradigm}-*; \
            rm -rf ../installcheck/instrumenter_checks/$$paradigm/build; \
        done

if HAVE_PTHREAD
if HAVE_GNU_LINKER

INSTALLCHECK_LOCAL += instrumenter-checks-pthread
instrumenter-checks-pthread: ../installcheck/instrumenter_checks/create_wait/pthread/Makefile \
                             ../installcheck/instrumenter_checks/check-instrumentation.sh
	@$(MAKE) -C ../installcheck/instrumenter_checks/create_wait/pthread

CLEAN_LOCAL += clean-local-instrumenter-checks-pthread
clean-local-instrumenter-checks-pthread: ../installcheck/instrumenter_checks/create_wait/pthread/Makefile
	@$(MAKE) -C ../installcheck/instrumenter_checks/create_wait/pthread clean

endif HAVE_GNU_LINKER
endif HAVE_PTHREAD

if HAVE_MEMORY_SUPPORT

INSTALLCHECK_LOCAL += instrumenter-checks-memory
instrumenter-checks-memory: ../installcheck/instrumenter_checks/memory/Makefile \
                            ../installcheck/instrumenter_checks/check-instrumentation.sh
	@$(MAKE) -C ../installcheck/instrumenter_checks/memory

CLEAN_LOCAL += clean-local-instrumenter-checks-memory
clean-local-instrumenter-checks-memory: ../installcheck/instrumenter_checks/memory/Makefile
	@$(MAKE) -C ../installcheck/instrumenter_checks/memory clean

endif HAVE_MEMORY_SUPPORT
