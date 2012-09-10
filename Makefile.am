
## The SUBDIRS variable holds a list of subdirectories automake (recursivly)
## must build.
SUBDIRS = $(subdirs) $(subdirs_custom)

## ACLOCAL_AMFLAGS contains options to pass to aclocal when aclocal.m4 is to be
## rebuilt by make. This line is also used by autoreconf to run aclocal with
## suitable options, or by autopoint and gettextize to locate the place where
## Gettext's macros should be installed. So even if you do not really care about
## the rebuild rules, you should define ACLOCAL_AMFLAGS.
ACLOCAL_AMFLAGS = -I vendor/common/build-config/m4

include vendor/common/build-config/Makefile.inc.am
include vendor/common/build-config/platforms.am

## Still, sometimes there are files that must be distributed, but which are not
## covered in the automatic rules. These files should be listed in the 
## EXTRA_DIST variable. You can mention files from subdirectories in EXTRA_DIST.
## 
## You can also mention a directory in EXTRA_DIST. In this case the entire
## directory will be recursively copied into the distribution. Please note that
## this will also copy everything in the directory, including CVS/RCS version
## control files. We recommend against using this feature.
EXTRA_DIST += $(srcdir)/src/config.h                                     \
              $(srcdir)/src/config-custom.h                              \
              $(srcdir)/doc/doxygen-user/scorep_usage.dox.in             \
              $(srcdir)/doc/doxygen-user/scorep_config_usage.dox.in      \
              $(srcdir)/doc/profile/scorep_metrics.html                  \
              $(srcdir)/src/utils/include/SCOREP_Omp.h                   \
              $(srcdir)/share/scorep.errors


if ! CROSS_BUILD
EXTRA_DIST += $(srcdir)/build-frontend/configure.ac \
              $(srcdir)/build-frontend/aclocal.m4   \
              $(srcdir)/build-frontend/Makefile.am  \
              $(srcdir)/build-frontend/Makefile.in  \
              $(srcdir)/build-frontend/configure    \
              $(srcdir)/src/config-frontend.h.in
endif

# temporary, we may remove this in the final version
EXTRA_DIST += \
    $(srcdir)/vendor/tclap/ArgException.h \
    $(srcdir)/vendor/tclap/Arg.h \
    $(srcdir)/vendor/tclap/ArgTraits.h \
    $(srcdir)/vendor/tclap/CmdLine.h \
    $(srcdir)/vendor/tclap/CmdLineInterface.h \
    $(srcdir)/vendor/tclap/CmdLineOutput.h \
    $(srcdir)/vendor/tclap/Constraint.h \
    $(srcdir)/vendor/tclap/DocBookOutput.h \
    $(srcdir)/vendor/tclap/HelpVisitor.h \
    $(srcdir)/vendor/tclap/IgnoreRestVisitor.h \
    $(srcdir)/vendor/tclap/MultiArg.h \
    $(srcdir)/vendor/tclap/MultiSwitchArg.h \
    $(srcdir)/vendor/tclap/OptionalUnlabeledTracker.h \
    $(srcdir)/vendor/tclap/StandardTraits.h \
    $(srcdir)/vendor/tclap/StdOutput.h \
    $(srcdir)/vendor/tclap/SwitchArg.h \
    $(srcdir)/vendor/tclap/UnlabeledMultiArg.h \
    $(srcdir)/vendor/tclap/UnlabeledValueArg.h \
    $(srcdir)/vendor/tclap/ValueArg.h \
    $(srcdir)/vendor/tclap/ValuesConstraint.h \
    $(srcdir)/vendor/tclap/VersionVisitor.h \
    $(srcdir)/vendor/tclap/Visitor.h \
    $(srcdir)/vendor/tclap/XorHandler.h \
    $(srcdir)/vendor/tclap/ZshCompletionOutput.h

install-data-local: doxygen-user-install
	$(MKDIR_P) $(DESTDIR)$(docdir)/profile; \
	$(INSTALL_DATA) $(DOC_SRC_DIR)/profile/scorep_metrics.html \
                        $(DESTDIR)$(docdir)/profile/scorep_metrics-$(PACKAGE_VERSION).html
	$(INSTALL_DATA) @PACKAGE@.summary $(DESTDIR)$(datadir)/@PACKAGE@/@PACKAGE@.summary

uninstall-hook: common-uninstall

dist-hook: prohibit-svn-files-in-dist doc/doxygen-user/main.dox doxygen-user-generate

clean-local: doxygen-user-clean

CLEANFILES += doc/doxygen-user/main.dox

if SVN_CONTROLLED

if CROSS_BUILD
INFO_TOOL_BUILD=build-frontend
else
INFO_TOOL_BUILD=build-backend
endif

$(INFO_TOOL_BUILD)/scorep-info:
	$(am__cd) $(INFO_TOOL_BUILD) && $(MAKE) $(AM_MAKEFLAGS) scorep-info

CLEANFILES += doc/doxygen-user/confvars.dox.in

doc/doxygen-user/confvars.dox.in: $(INFO_TOOL_BUILD)/scorep-info
	$(AM_V_GEN)$(MKDIR_P) `dirname $@` && \
	./$(INFO_TOOL_BUILD)/scorep-info config-vars --full=html >$@

doc/doxygen-user/main.dox: $(srcdir)/doc/doxygen-user/main_1.dox.in \
    $(srcdir)/doc/doxygen-user/scorep_usage.dox.in \
    $(srcdir)/doc/doxygen-user/main_2.dox.in \
    $(srcdir)/doc/doxygen-user/scorep_config_usage.dox.in \
    $(srcdir)/doc/doxygen-user/main_3.dox.in \
    doc/doxygen-user/confvars.dox.in \
    $(srcdir)/doc/doxygen-user/main_4.dox.in
	$(AM_V_GEN)$(MKDIR_P) `dirname $@` && \
	cat $^ >$@

else

doc/doxygen-user/main.dox:
	$(AM_V_GEN)$(MKDIR_P) `dirname $@` && \
	touch $@

endif


if CROSS_BUILD
check-local:
	@$(am__cd) build-frontend && $(MAKE) $(AM_MAKEFLAGS) check-serial
	@$(am__cd) build-frontend && $(MAKE) $(AM_MAKEFLAGS) check-omp
	@$(am__cd) build-backend  && $(MAKE) $(AM_MAKEFLAGS) check-serial
	@$(am__cd) build-backend  && $(MAKE) $(AM_MAKEFLAGS) check-omp
	@$(am__cd) build-mpi      && $(MAKE) $(AM_MAKEFLAGS) check-mpi
	@$(am__cd) build-mpi      && $(MAKE) $(AM_MAKEFLAGS) check-mpi-omp
else !CROSS_BUILD
check-local:
	@$(am__cd) build-backend && $(MAKE) $(AM_MAKEFLAGS) check-serial
	@$(am__cd) build-backend && $(MAKE) $(AM_MAKEFLAGS) check-omp
	@$(am__cd) build-mpi     && $(MAKE) $(AM_MAKEFLAGS) check-mpi
	@$(am__cd) build-mpi     && $(MAKE) $(AM_MAKEFLAGS) check-mpi-omp
endif !CROSS_BUILD

DISTCLEANFILES += \
    src/utils/include/SCOREP_ErrorCodes.h \
    src/utils/include/scorep_error_decls.gen.h

BUILT_SOURCES += \
    src/utils/include/SCOREP_ErrorCodes.h \
    src/utils/include/scorep_error_decls.gen.h

src/utils/include/SCOREP_ErrorCodes.h: $(srcdir)/share/scorep.errors
src/utils/include/scorep_error_decls.gen.h: $(srcdir)/share/scorep.errors
