
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
              $(srcdir)/build-config/VERSION                             \
              $(srcdir)/tools/tau/scorep_meets_tau.awk


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

#.PHONY: doxygen-project-number update-revision clean-doxygen-user doxygen-user doxygen-user-pdf beautify beautify-clean

install-data-local: doxygen-user-install install-scorep-meets-tau

uninstall-local: doxygen-user-uninstall

dist-hook: prohibit-svn-files-in-dist doxygen-user-generate

clean-local: doxygen-user-clean

CLEANFILES += $(builddir)/include/scorep/scorep_meets_tau.h


install-scorep-meets-tau:
	cp -p $(builddir)/include/scorep/scorep_meets_tau_tmp.h $(builddir)/include/scorep/scorep_meets_tau.h
	ordered_headers="SCOREP_DefinitionHandles.h SCOREP_Memory.h SCOREP_RuntimeManagement.h SCOREP_Thread_Types.h SCOREP_Types.h SCOREP_Adapter.h SCOREP_Config.h SCOREP_Definitions.h SCOREP_Events.h"; \
	for i in $$ordered_headers; do                                                                \
	    awk -f $(srcdir)/tools/tau/scorep_meets_tau.awk $(srcdir)/src/measurement/include/$$i >>  \
                   $(builddir)/include/scorep/scorep_meets_tau.h;                   \
	done
	echo "#ifdef __cplusplus"              >> ./include/scorep/scorep_meets_tau.h;
	echo "} /* extern C */"                >> ./include/scorep/scorep_meets_tau.h;
	echo "#endif"                          >> ./include/scorep/scorep_meets_tau.h;
	echo ""                                >> ./include/scorep/scorep_meets_tau.h;
	echo "#endif /* SCOREP_MEETS_TAU_H */" >> ./include/scorep/scorep_meets_tau.h;
	$(MKDIR_P) $(DESTDIR)$(datadir)/@PACKAGE_NAME@/devel
	$(INSTALL_DATA) $(builddir)/include/scorep/scorep_meets_tau.h $(DESTDIR)$(datadir)/@PACKAGE_NAME@/devel/scorep_meets_tau.h


uninstall-local:
	rm -rf $(DESTDIR)$(datadir)/@PACKAGE_NAME@


if CROSS_BUILD
check-local:
	@fail= failcom='exit 1'; \
	for f in x $$MAKEFLAGS; do \
	  case $$f in \
	    *=* | --[!k]*);; \
	    *k*) failcom='fail=yes';; \
	  esac; \
	done; \
	($(am__cd) build-frontend && $(MAKE) $(AM_MAKEFLAGS) check-serial)  || eval $$failcom; \
	($(am__cd) build-frontend && $(MAKE) $(AM_MAKEFLAGS) check-omp)     || eval $$failcom; \
	($(am__cd) build-backend  && $(MAKE) $(AM_MAKEFLAGS) check-serial)  || eval $$failcom; \
	($(am__cd) build-backend  && $(MAKE) $(AM_MAKEFLAGS) check-omp)     || eval $$failcom; \
	($(am__cd) build-mpi      && $(MAKE) $(AM_MAKEFLAGS) check-mpi)     || eval $$failcom; \
	($(am__cd) build-mpi      && $(MAKE) $(AM_MAKEFLAGS) check-mpi-omp) || eval $$failcom; \
        test -z "$$fail"
else !CROSS_BUILD
check-local:
	@fail= failcom='exit 1'; \
	for f in x $$MAKEFLAGS; do \
	  case $$f in \
	    *=* | --[!k]*);; \
	    *k*) failcom='fail=yes';; \
	  esac; \
	done; \
	($(am__cd) build-backend  && $(MAKE) $(AM_MAKEFLAGS) check-serial)  || eval $$failcom; \
	($(am__cd) build-backend  && $(MAKE) $(AM_MAKEFLAGS) check-omp)     || eval $$failcom; \
	($(am__cd) build-mpi      && $(MAKE) $(AM_MAKEFLAGS) check-mpi)     || eval $$failcom; \
	($(am__cd) build-mpi      && $(MAKE) $(AM_MAKEFLAGS) check-mpi-omp) || eval $$failcom; \
        test -z "$$fail"
endif !CROSS_BUILD
