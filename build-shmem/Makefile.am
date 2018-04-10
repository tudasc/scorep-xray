# -*- mode: makefile -*-

##
## This file is part of the Score-P software (http://www.score-p.org)
##
## Copyright (c) 2013-2015, 2017,
## Technische Universitaet Dresden, Germany
##
## Copyright (c) 2015, 2017-2018,
## Forschungszentrum Juelich GmbH, Germany
##
## This software may be modified and distributed under the terms of
## a BSD-style license.  See the COPYING file in the package base
## directory for details.
##

## file       build-shmem/Makefile.am


## Options passed to aclocal to specify local macro directories. Although
## ACLOCAL_AMFLAGS should be replaced by AC_CONFIG_MACRO_DIRS, aclocal-1.13.4
## doesn't honor the paths provided to AC_CONFIG_MACRO_DIRS. See
## https://debbugs.gnu.org/cgi/bugreport.cgi?bug=21352
ACLOCAL_AMFLAGS = -I ../build-config/m4 -I ../vendor/common/build-config/m4

include ../build-includes/common.am
include ../vendor/common/build-config/Makefile.tests-shmem.inc.am
AM_TESTS_ENVIRONMENT_SHMEM = export SCOREP_EXPERIMENT_DIRECTORY=make_check_experiment_shmem;
CLEAN_LOCAL += clean-local-experiment-directories
clean-local-experiment-directories:
	@rm -rf make_check_experiment_shmem*

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
BACKEND_TARGETS =
$(BACKEND_TARGETS): build_external_targets
build_external_targets:
	@targets=`echo $(BACKEND_TARGETS) | sed 's|^[ ^t]*||;s|[ ^]*$$||;s|$(LIB_DIR_SCOREP)||g'`; \
	if test "x$$targets" != x; then \
		$(am__cd) $(LIB_DIR_SCOREP) && $(MAKE) $(AM_MAKEFLAGS) $$targets; \
	fi

BACKEND_CHECK_TARGETS =
$(BACKEND_CHECK_TARGETS): build_external_check_targets
build_external_check_targets:
	@targets=`echo $(BACKEND_CHECK_TARGETS) | sed 's|^[ ^t]*||;s|[ ^]*$$||;s|$(LIB_DIR_SCOREP)||g'`; \
	if test "x$$targets" != x; then \
		$(am__cd) $(LIB_DIR_SCOREP) && $(MAKE) $(AM_MAKEFLAGS) $$targets; \
	fi

AM_CPPFLAGS += -DBACKEND_BUILD_SHMEM

bindir = @bindir@@backend_suffix@
libdir = @libdir@@backend_suffix@

## Additional libtool linking flags
LINKMODE_FLAGS = @AFS_LINKMODE_LDFLAGS@

include ../build-includes/shmem-backend-only.am

backend_pkglibexecdir = $(pkglibexecdir)@backend_suffix@/shmem
backend_pkglibexec_SCRIPTS = libtool

if CROSS_BUILD
AM_CPPFLAGS += -DCROSS_BUILD
else !CROSS_BUILD
AM_CPPFLAGS += -DNOCROSS_BUILD
endif !CROSS_BUILD

if HAVE_SHMEM_SUPPORT
if !HAVE_SHMEM_PROFILING_INTERFACE
pkgdata_DATA += \
    ../share/shmem.wrap \
    ../share/shmem.nvcc.wrap

DISTCLEANFILES += ../share/shmem.nvcc.wrap

else HAVE_SHMEM_PROFILING_INTERFACE

DISTCLEANFILES += $(builddir)/scorep_have_pshmem_support.txt

endif HAVE_SHMEM_PROFILING_INTERFACE
endif HAVE_SHMEM_SUPPORT

## Determine how to handle tests
if CROSS_BUILD
check-shmem:     check-file-shmem
else !CROSS_BUILD
if BACKEND_TEST_RUNS
check-shmem:     check-run-shmem
else !BACKEND_TEST_RUNS
check-shmem:     check-file-shmem
endif !BACKEND_TEST_RUNS
endif !CROSS_BUILD


## Here all libscorep_*.la are build
LIB_DIR_SCOREP = ../build-backend/

CLEANFILES += lex.yy.c scanner.h yacc.c yacc.h y.tab.h

PARADIGMS =
instrumenter_check_paradigm_makefiles =
instrumenter_configuration_files =
if HAVE_SHMEM_SUPPORT
PARADIGMS += shmem
instrumenter_check_paradigm_makefiles += \
    ../installcheck/instrumenter_checks/shmem/Makefile
instrumenter_configuration_files += \
    ../installcheck/instrumenter_checks/configurations_shmem
if HAVE_OPENMP_SUPPORT
PARADIGMS += shmem_omp
instrumenter_check_paradigm_makefiles += \
    ../installcheck/instrumenter_checks/shmem_omp/Makefile
instrumenter_configuration_files += \
    ../installcheck/instrumenter_checks/configurations_shmem_omp
endif HAVE_OPENMP_SUPPORT
endif HAVE_SHMEM_SUPPORT


# These prerequisites are controlled by build-backend/Makefile
foreign_instrumenter_check_prerequisites = \
    ../installcheck/instrumenter_checks/Makefile.common_rules \
    ../installcheck/instrumenter_checks/Makefile.instrument_configurations \
    ../installcheck/instrumenter_checks/instrumenter-configurations.sh \
    ../installcheck/instrumenter_checks/check-instrumentation.sh

$(foreign_instrumenter_check_prerequisites):
	@$(am__cd) ../build-backend && $(MAKE) $(AM_MAKEFLAGS) $@

$(instrumenter_configuration_files): ../installcheck/instrumenter_checks/instrumenter-configurations.sh
	@../installcheck/instrumenter_checks/instrumenter-configurations.sh

INSTALLCHECK_LOCAL += instrumenter-checks
instrumenter-checks: $(instrumenter_configuration_files) \
                     $(instrumenter_check_paradigm_makefiles) \
                     $(foreign_instrumenter_check_prerequisites)
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

CLEAN_LOCAL += clean-local-instrumenter-checks
clean-local-instrumenter-checks:
	@rm -f $(instrumenter_configuration_files)
	@for paradigm in $(PARADIGMS); do \
            rm -f ../installcheck/instrumenter_checks/bin/*-$${paradigm}-*; \
            rm -rf ../installcheck/instrumenter_checks/$$paradigm/build; \
        done
