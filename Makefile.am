# -*- mode: makefile -*-

##
## This file is part of the Score-P software (http://www.score-p.org)
##
## Copyright (c) 2009-2013,
## RWTH Aachen University, Germany
##
## Copyright (c) 2009-2013,
## Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
##
## Copyright (c) 2009-2013,
## Technische Universitaet Dresden, Germany
##
## Copyright (c) 2009-2013,
## University of Oregon, Eugene, USA
##
## Copyright (c) 2009-2015, 2017,
## Forschungszentrum Juelich GmbH, Germany
##
## Copyright (c) 2009-2013,
## German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
##
## Copyright (c) 2009-2013,
## Technische Universitaet Muenchen, Germany
##
## This software may be modified and distributed under the terms of
## a BSD-style license.  See the COPYING file in the package base
## directory for details.
##

## The SUBDIRS variable holds a list of subdirectories automake (recursivly)
## must build.
SUBDIRS = $(subdirs) $(ax_config_subdirs)

## Options passed to aclocal to specify local macro directories. Although
## ACLOCAL_AMFLAGS should be replaced by AC_CONFIG_MACRO_DIRS, aclocal-1.13.4
## doesn't honor the paths provided to AC_CONFIG_MACRO_DIRS. See
## https://debbugs.gnu.org/cgi/bugreport.cgi?bug=21352
ACLOCAL_AMFLAGS = -I build-config/m4 -I vendor/common/build-config/m4

include vendor/common/build-config/Makefile.inc.am
include doc/Makefile.inc.am
include vendor/common/build-config/platforms.am

## Still, sometimes there are files that must be distributed, but which are not
## covered in the automatic rules. These files should be listed in the
## EXTRA_DIST variable. You can mention files from subdirectories in EXTRA_DIST.
##
## You can also mention a directory in EXTRA_DIST. In this case the entire
## directory will be recursively copied into the distribution. Please note that
## this will also copy everything in the directory, including CVS/RCS version
## control files. We recommend against using this feature.
EXTRA_DIST += $(srcdir)/src/config.h                                      \
              $(srcdir)/src/config-custom.h                               \
              $(srcdir)/doc/doxygen-user/scorep_usage.dox.in              \
              $(srcdir)/doc/doxygen-user/scorep_config_usage.dox.in       \
              $(srcdir)/doc/doxygen-user/scorep_score_usage.dox.in        \
              $(srcdir)/doc/doxygen-user/scorep_backend_info_help.dox.in  \
              $(srcdir)/doc/doxygen-user/scorep_backend_info_usage.dox.in \
              $(srcdir)/doc/doxygen-user/main_1.dox.in                    \
              $(srcdir)/doc/doxygen-user/main_2.dox.in                    \
              $(srcdir)/doc/doxygen-user/main_3.dox.in                    \
              $(srcdir)/doc/doxygen-user/main_4.dox.in                    \
              $(srcdir)/doc/doxygen-user/main_5.dox.in                    \
              $(srcdir)/doc/doxygen-user/main_6.dox.in                    \
              $(srcdir)/doc/doxygen-user/main_7.dox.in                    \
              $(srcdir)/doc/doxygen-user/main_8.dox.in                    \
              $(srcdir)/doc/doxygen-user/main_9.dox.in                    \
              $(srcdir)/doc/doxygen-user/main_10.dox.in                   \
              $(srcdir)/doc/doxygen-user/main_11.dox.in                   \
              $(srcdir)/doc/doxygen-user/scorep_info_confvars_help.md     \
              $(srcdir)/doc/profile/scorep_metrics.html                   \
              $(srcdir)/src/adapters/include/SCOREP_Fortran_Wrapper.h     \
              $(srcdir)/share/scorep.errors                               \
              $(srcdir)/share/bg-gnu/platform-frontend-user-provided      \
              $(srcdir)/share/bg-gnu/platform-backend-user-provided       \
              $(srcdir)/share/bg-gnu/platform-mpi-user-provided           \
              $(srcdir)/share/bg-gnu/platform-shmem-user-provided         \
              $(srcdir)/share/bg-gnu/README                               \
              $(srcdir)/THANKS                                            \
              $(srcdir)/OPEN_ISSUES


if ! CROSS_BUILD
## This EXTRA_DIST is deliberately inside this conditional block
EXTRA_DIST += $(srcdir)/build-frontend/configure.ac \
              $(srcdir)/build-frontend/aclocal.m4   \
              $(srcdir)/build-frontend/Makefile.am  \
              $(srcdir)/build-frontend/Makefile.in  \
              $(srcdir)/build-frontend/configure    \
              $(srcdir)/src/config-frontend.h.in
endif

pkgdata_DATA = @PACKAGE@.summary \
               share/scorep.spec \
               $(srcdir)/doc/doxygen-user/scorep_info_confvars_help.md

doc_DATA = $(srcdir)/OPEN_ISSUES \
           $(srcdir)/ChangeLog

install-data-local: doxygen-user-install
	$(MKDIR_P) $(DESTDIR)$(docdir)/profile; \
	$(INSTALL_DATA) $(DOC_SRC_DIR)/profile/scorep_metrics.html \
                        $(DESTDIR)$(docdir)/profile/scorep_metrics-$(PACKAGE_VERSION).html

install-data-hook:
	$(AM_V_at)for target in $(TARGETS)""; do \
	    if test -n "$$target" && test -f $(pkgdatadir)/$$target/@PACKAGE@.summary; then \
	        echo >> $(DESTDIR)$(pkgdatadir)/@PACKAGE@.summary ; \
	        echo >> $(DESTDIR)$(pkgdatadir)/@PACKAGE@.summary ; \
	        echo "=====[ Target: $$target ]====================" >> $(DESTDIR)$(pkgdatadir)/@PACKAGE@.summary ; \
	        echo >> $(DESTDIR)$(pkgdatadir)/@PACKAGE@.summary ; \
	        cat $(pkgdatadir)/$$target/@PACKAGE@.summary >> $(DESTDIR)$(pkgdatadir)/@PACKAGE@.summary ; \
	    fi \
	done

uninstall-hook: doxygen-uninstall common-uninstall

dist-hook: prohibit-svn-files-in-dist doc/doxygen-user/main.dox doxygen-user-generate

clean-local: doxygen-user-clean

CLEANFILES += doc/doxygen-user/main.dox

if SVN_CONTROLLED

if CROSS_BUILD
INFO_TOOL_BUILD=build-frontend
else !CROSS_BUILD
INFO_TOOL_BUILD=build-backend
endif !CROSS_BUILD

scorep-info-command:
	@$(am__cd) $(INFO_TOOL_BUILD) && $(MAKE) $(AM_MAKEFLAGS) scorep-info

CLEANFILES += doc/doxygen-user/scorep_info_usage.dox.in
doc/doxygen-user/scorep_info_usage.dox.in: scorep-info-command
	$(AM_V_GEN)$(MKDIR_P) `dirname $@` && \
	./$(INFO_TOOL_BUILD)/scorep-info --help >$@

CLEANFILES += doc/doxygen-user/confvars.dox.in
doc/doxygen-user/confvars.dox.in: scorep-info-command
	$(AM_V_GEN)$(MKDIR_P) `dirname $@` && \
	./$(INFO_TOOL_BUILD)/scorep-info config-vars --doxygen >$@

scorep-wrapper-command:
	@$(am__cd) build-backend && $(MAKE) $(AM_MAKEFLAGS) scorep-wrapper

CLEANFILES += doc/doxygen-user/scorep_wrapper_usage.md
doc/doxygen-user/scorep_wrapper_usage.md: scorep-wrapper-command
	$(AM_V_GEN)$(MKDIR_P) `dirname $@` && \
	./build-backend/scorep-wrapper --help | \
            sed '$$d' >$@.raw && \
        ( \
            sed -e '/^Wrapper Instances$$/q' $@.raw; \
            echo "================="; \
            echo ""; \
            echo "The installation provides wrapper instances based on the compilers used to"; \
            echo "build Score-P.  Run \`scorep-wrapper --help\` to see a listing of all"; \
            echo "default instances of the used Score-P installation."; \
            echo ""; \
            echo "Additional wrapper instances can be created with \`scorep-wrapper --create\`."; \
            echo ""; \
            sed -n -e '/^Examples$$/,$$p' $@.raw; \
        ) >$@ && \
        rm $@.raw

doc/doxygen-user/main.dox: $(srcdir)/doc/doxygen-user/main_1.dox.in \
    $(srcdir)/doc/doxygen-user/scorep_usage.dox.in \
    $(srcdir)/doc/doxygen-user/main_2.dox.in \
    $(srcdir)/doc/doxygen-user/scorep_config_usage.dox.in \
    $(srcdir)/doc/doxygen-user/main_3.dox.in \
    doc/doxygen-user/scorep_info_usage.dox.in \
    $(srcdir)/doc/doxygen-user/main_4.dox.in \
    $(srcdir)/doc/doxygen-user/scorep_score_usage.dox.in \
    $(srcdir)/doc/doxygen-user/main_5.dox.in \
    $(srcdir)/doc/doxygen-user/scorep_backend_info_usage.dox.in \
    $(srcdir)/doc/doxygen-user/scorep_backend_info_help.dox.in \
    $(srcdir)/doc/doxygen-user/main_6.dox.in \
    $(srcdir)/doc/doxygen-user/scorep_info_confvars_help.md \
    $(srcdir)/doc/doxygen-user/main_7.dox.in \
    doc/doxygen-user/confvars.dox.in \
    $(srcdir)/doc/doxygen-user/main_8.dox.in \
    doc/doxygen-user/scorep_wrapper_usage.md \
    $(srcdir)/doc/doxygen-user/main_9.dox.in \
    $(srcdir)/src/tools/libwrap_init/README.md \
    $(srcdir)/doc/doxygen-user/main_10.dox.in \
    $(srcdir)/doc/doxygen-user/main_11.dox.in
	$(AM_V_GEN)$(MKDIR_P) `dirname $@` && \
	cat $^ >$@

else !SVN_CONTROLLED

doc/doxygen-user/main.dox:
	$(AM_V_GEN)$(MKDIR_P) `dirname $@` && \
	touch $@

endif !SVN_CONTROLLED


if CROSS_BUILD
CHECK_TARGETS = \
    build-frontend:check-serial \
    build-frontend:check-omp \
    build-backend:check-serial \
    build-backend:check-omp
if HAVE_SCOREP_MPI
CHECK_TARGETS += \
    build-mpi:check-mpi \
    build-mpi:check-mpi-omp
endif HAVE_SCOREP_MPI
if HAVE_SCOREP_SHMEM
CHECK_TARGETS += \
    build-shmem:check-shmem
endif HAVE_SCOREP_SHMEM
else !CROSS_BUILD
CHECK_TARGETS = \
    build-backend:check-serial \
    build-backend:check-omp
if HAVE_SCOREP_MPI
CHECK_TARGETS += \
    build-mpi:check-mpi \
    build-mpi:check-mpi-omp
endif HAVE_SCOREP_MPI
if HAVE_SCOREP_SHMEM
CHECK_TARGETS += \
    build-shmem:check-shmem
endif HAVE_SCOREP_SHMEM
endif !CROSS_BUILD

check-local:
	@fail= failcom='exit 1'; \
	for f in x $$MAKEFLAGS; do \
	  case $$f in \
	    *=* | --[!k]*);; \
	    *k*) failcom='fail=yes';; \
	  esac; \
	done; \
	list='$(CHECK_TARGETS)'; for subcheck in $$list; do \
	  subdir=$${subcheck%:*}; \
	  target=$${subcheck#*:}; \
	  echo "Making $$target in $$subdir"; \
	  ($(am__cd) $$subdir && $(MAKE) $(AM_MAKEFLAGS) $$target) \
	  || eval $$failcom; \
	done; test -z "$$fail"

DISTCLEANFILES += \
    src/utils/include/SCOREP_ErrorCodes.h \
    src/utils/include/scorep_error_decls.gen.h

BUILT_SOURCES += \
    src/utils/include/SCOREP_ErrorCodes.h \
    src/utils/include/scorep_error_decls.gen.h

src/utils/include/SCOREP_ErrorCodes.h: $(srcdir)/share/scorep.errors
src/utils/include/scorep_error_decls.gen.h: $(srcdir)/share/scorep.errors


## bitten targets, intended to be used from bitten only

# Copy installcheck binaries, i.e., all files in a directory named bin
# under <builddir>/installcheck, to a persistent directory
# $(INSTALLCHECKS_PERSIST_DIR) (to be executed later on). On systems
# with low quota, disable copying of installcheck binaries by
# providing $(INSTALLCHECKS_PERSIST_DIR) = no form bitten-slave.
if HAVE_LOCKFILE_PROGS
# Save version, user can lock $(INSTALLCHECKS_PERSIST_DIR) to execute
# installchecks, new installchecks will not overwrite existing ones.
bitten-persist-installchecks:
	installchecks_persist_dir="$(INSTALLCHECKS_PERSIST_DIR)"; \
	if test "$${installchecks_persist_dir:-no}" != no ; then \
	    $(MKDIR_P) `dirname $(INSTALLCHECKS_PERSIST_DIR)`; \
	    lockfile-check $(INSTALLCHECKS_PERSIST_DIR); \
	    if test $$? -eq 0; then exit 0; fi; \
	    lockfile-create $(INSTALLCHECKS_PERSIST_DIR) || exit 1; \
	    lockfile-touch $(INSTALLCHECKS_PERSIST_DIR) &  \
	    lockfile_touch_pid="$$!"; \
	    rm -rf $(INSTALLCHECKS_PERSIST_DIR); \
	    $(MKDIR_P) $(INSTALLCHECKS_PERSIST_DIR) || exit 1; \
            cd installcheck || exit 1; \
            find . -type d -name bin -exec cp --parents -r '{}' $(INSTALLCHECKS_PERSIST_DIR)/ \; || exit 1; \
            cd .. || exit 1; \
	    echo $(REVISION) > $(INSTALLCHECKS_PERSIST_DIR)/REVISION || exit 1; \
	    kill "$${lockfile_touch_pid}"; \
	    lockfile-remove $(INSTALLCHECKS_PERSIST_DIR) || exit 1; \
	fi
else !HAVE_LOCKFILE_PROGS
# Unsafe version, user has to take care that no bitten-slave will
# override $(INSTALLCHECKS_PERSIST_DIR) while processing inside
# $(INSTALLCHECKS_PERSIST_DIR).
bitten-persist-installchecks:
	installchecks_persist_dir="$(INSTALLCHECKS_PERSIST_DIR)"; \
	if test "$${installchecks_persist_dir:-no}" != no ; then \
	    rm -rf $(INSTALLCHECKS_PERSIST_DIR); \
	    $(MKDIR_P) $(INSTALLCHECKS_PERSIST_DIR) || exit 1; \
            cd installcheck || exit 1; \
            find . -type d -name bin -exec cp --parents -r '{}' $(INSTALLCHECKS_PERSIST_DIR)/ \; || exit 1; \
            cd .. || exit 1; \
	    echo $(REVISION) > $(INSTALLCHECKS_PERSIST_DIR)/REVISION || exit 1; \
	fi
endif !HAVE_LOCKFILE_PROGS

include doc/examples/Makefile.inc.am

# Temporary fix for make distcheck failure introduced by r4404, see
# also configure.ac. Obsolete once #741 is completed.
DISTCHECK_CONFIGURE_FLAGS = triggered_by_make_distcheck=yes

# On MIC, install config.summary also in $pkgdatadir/mic
if PLATFORM_MIC

micpkgdatadir = $(pkgdatadir)/mic
micpkgdata_DATA = @PACKAGE@.summary

else !PLATFORM_MIC

TARGETS = mic

endif !PLATFORM_MIC
