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

## file       build-backend/Makefile.am 
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
include ../vendor/common/build-config/Makefile.tests-serial.inc.am
include ../vendor/common/build-config/Makefile.tests-omp.inc.am

AM_CPPFLAGS += -DBACKEND_BUILD_NOMPI

frontend_bindir = @bindir@
frontend_libdir = @libdir@
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

clean-local: clean-local-scorep-experiment-dirs clean-local-scorep-config-tests clean-local-instrumenter-checks
	rm -f lex.yy.c scanner.h yacc.c yacc.h y.tab.h 

clean-local-scorep-experiment-dirs:
	rm -rf scorep-2*
	rm -rf scorep-measurement-tmp
	rm -f serial_inst_test

clean-local-scorep-config-tests:
	$(RM) test_scorep_config_number.*.out
	$(RM) test_scorep_config_size.*.out

distclean-local:
	rm -rf ../test/jacobi/serial
	rm -rf ../test/jacobi/OpenMP

install-data-local: $(INSTALL_DATA_LOCAL_TARGETS)

install-data-hook: $(INSTALL_DATA_HOOK_TARGETS)

SCOREP_SHARE_INSTALL_DIR = $(DESTDIR)@DATAROOTDIR_SCOREP@

SCOREP_SHARE_LOCAL_DIR=$(PWD)/../share
SCOREP_SHARE_GLOBAL_DIR=$(abs_top_srcdir)/../share
SCOREP_PDT_CONFIG_INSTALL_TARGET = $(SCOREP_SHARE_INSTALL_DIR)/SCOREP_Pdt_Instrumentation.conf
SCOREP_COBI_CONFIG_IN=$(SCOREP_SHARE_GLOBAL_DIR)/SCOREP_Cobi_Adapter.xml.in

uninstall-local: $(UNINSTALL_LOCAL_TARGETS)
	rm -f $(DESTDIR)$(exec_prefix)/bin/scorep_config.dat \
              $(SCOREP_PDT_CONFIG_INSTALL_TARGET) 
	if [ -e $(SCOREP_SHARE_LOCAL_DIR)/SCOREP_Cobi_Adapter.xml.in ]; then \
	  rm -f  $(SCOREP_SHARE_LOCAL_DIR)/SCOREP_Cobi_Adapter.xml; \
	else \
	  rm -rf $(SCOREP_SHARE_LOCAL_DIR); \
	fi

INSTALL_DATA_LOCAL_TARGETS += scorep-config-tool-install 

scorep-config-tool-install:
	$(MKDIR_P) $(SCOREP_SHARE_INSTALL_DIR)
	$(INSTALL_DATA) $(INC_ROOT)share/SCOREP_Pdt_Instrumentation.conf $(SCOREP_PDT_CONFIG_INSTALL_TARGET)
	$(INSTALL_DATA) $(INC_ROOT)share/SCOREP_Cobi_Filter.xml          $(SCOREP_SHARE_INSTALL_DIR)/SCOREP_Cobi_Filter.xml
	$(INSTALL_DATA) $(INC_ROOT)share/SCOREP_Cobi_Adapter.xml.in      $(SCOREP_SHARE_INSTALL_DIR)/SCOREP_Cobi_Adapter.xml

create-cobi-config-local:
	@$(MKDIR_P) $(SCOREP_SHARE_LOCAL_DIR)
	@if [ ! -e $(SCOREP_SHARE_LOCAL_DIR)/SCOREP_Cobi_Adapter.xml ]; then \
         cp $(SCOREP_COBI_CONFIG_IN) $(SCOREP_SHARE_LOCAL_DIR)/SCOREP_Cobi_Adapter.xml; \
	fi
	@if [ ! -e $(SCOREP_SHARE_LOCAL_DIR)/SCOREP_Cobi_Filter.xml ]; then \
	 cp $(SCOREP_SHARE_GLOBAL_DIR)/SCOREP_Cobi_Filter.xml $(SCOREP_SHARE_LOCAL_DIR)/; \
	fi


INSTALLCHECK_SUBDIRS  = ../test/jacobi/serial/C
if HAVE_OPENMP_SUPPORT
INSTALLCHECK_SUBDIRS += ../test/jacobi/OpenMP/C
else
endif

installcheck-local: instrumenter-checks installcheck-public-headers
	@fail= failcom='exit 1'; \
	for f in x $$MAKEFLAGS; do \
	  case $$f in \
	    *=* | --[!k]*);; \
	    *k*) failcom='fail=yes';; \
	  esac; \
	done; \
	list='$(INSTALLCHECK_SUBDIRS)'; for subdir in $$list; do \
	  echo "Making in $$subdir"; \
	  ($(am__cd) $$subdir && $(MAKE) $(AM_MAKEFLAGS)) \
	  || eval $$failcom; \
	done; test -z "$$fail"

PARADIGMS = serial
# component separation will provide the HAVE_OPARI2 conditional
#if HAVE_OPARI2
if HAVE_OPENMP_SUPPORT
PARADIGMS += omp
endif
#endif

instrumenter-checks: $(instrumenter_configuration_files)
	@fail= failcom='exit 1'; \
	for f in x $$MAKEFLAGS; do \
	  case $$f in \
	    *=* | --[!k]*);; \
	    *k*) failcom='fail=yes';; \
	  esac; \
	done; \
	list='$(PARADIGMS)'; for paradigm in $$list; do \
	  echo "Making instrument-configurations in ../installcheck/$$paradigm"; \
	  ($(am__cd) ../installcheck/$$paradigm && $(MAKE) $(AM_MAKEFLAGS) instrument-configurations) \
	  || eval $$failcom; \
	done; test -z "$$fail"

instrumenter_configuration_files = ../installcheck/configurations_serial ../installcheck/configurations_omp

$(instrumenter_configuration_files): ../installcheck/instrumenter-configurations.sh
	@../installcheck/instrumenter-configurations.sh

clean-local-instrumenter-checks:
	@rm -f $(instrumenter_configuration_files)
	@for paradigm in $(PARADIGMS); do \
            rm -f ../installcheck/bin/*-$${paradigm}-*; \
            rm -rf ../installcheck/$$paradigm/build; \
        done
