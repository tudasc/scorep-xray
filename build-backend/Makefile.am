# silc--backend

## The SUBDIRS variable holds a list of subdirectories automake (recursivly) 
## must build. 
#SUBDIRS = 

include ../build-includes/common.am

AM_CPPFLAGS += -DBACKEND_BUILD_NOMPI

bindir=${exec_prefix}@backend_bin_dir@
libdir=${exec_prefix}@backend_lib_dir@

if CROSS_BUILD
AM_CPPFLAGS += -DCROSS_BUILD
include ../build-includes/backend-only.am
include ../build-includes/front-and-backend.am
else
AM_CPPFLAGS += -DNOCROSS_BUILD
include ../build-includes/backend-only.am
include ../build-includes/front-and-backend.am
include ../build-includes/frontend-only.am
endif

## Convenience variable, used in common.am
BUILD_DIR=build-backend

## ACLOCAL_AMFLAGS contains options to pass to aclocal when aclocal.m4 is to be
## rebuilt by make. This line is also used by autoreconf to run aclocal with
## suitable options, or by autopoint and gettextize to locate the place where
## Gettext's macros should be installed. So even if you do not really care about
## the rebuild rules, you should define ACLOCAL_AMFLAGS.
## Can't be moved to common.am
ACLOCAL_AMFLAGS = -I ../build-config/m4

EXTRA_LOCAL_CLEAN_TARGETS = clean-local-silc-experiment-dirs \
                            clean-local-silc-config-tests    \
                            clean-local-silc-config-tool

clean-local: $(EXTRA_LOCAL_CLEAN_TARGETS)
.PHONY: $(EXTRA_LOCAL_CLEAN_TARGETS) \
        install-scorep-meets-tau

clean-local-silc-experiment-dirs:
	rm -rf silc-2*
	rm -rf silc-measurement-tmp
	rm -f serial_inst_test

clean-local-silc-config-tests:
	$(RM) test_silc_config_number.*.out
	$(RM) test_silc_config_size.*.out

install-data-local: silc-config-tool-install

uninstall-local:
	rm -f $(DESTDIR)$(exec_prefix)/bin/silc_config.dat

silc-config-tool-install:
	echo COMPILER_INSTRUMENTATION_CPPFLAGS=\"$(COMPILER_INSTRUMENTATION_CPPFLAGS)\" >  $(builddir)/silc_config.dat
	echo OPENMP_CFLAGS=\"$(OPENMP_CFLAGS)\"                                         >> $(builddir)/silc_config.dat
	echo PREFIX=\"$(prefix)\"                                                       >> $(builddir)/silc_config.dat
	echo DATADIR=\"$(datadir)\"                                                     >> $(builddir)/silc_config.dat
	echo LIBDIR=\"$(libdir)\"                                                       >> $(builddir)/silc_config.dat
	echo LIBDIR=\"$(LIBS)\"                                                         >> $(builddir)/silc_config.dat
	echo LIBDIR=\"$(CUBE_LIB_PATH)\"                                                >> $(builddir)/silc_config.dat
	echo LIBDIR=\"$(TIMER_LIBDIR)\"                                                 >> $(builddir)/silc_config.dat
	echo LIBS=\"$(LIBBFD)\"                                                         >> $(builddir)/silc_config.dat
	echo LIBS=\"$(CUBE_LIBS)\"                                                      >> $(builddir)/silc_config.dat
	echo LIBS=\"$(TIMER_LIB)\"                                                      >> $(builddir)/silc_config.dat
	echo CC=\"$(CC)\"                                                               >> $(builddir)/silc_config.dat
	echo NM=\"$(NM)\"                                                               >> $(builddir)/silc_config.dat
	echo AWK=\"$(AWK)\"                                                             >> $(builddir)/silc_config.dat
	echo OPARI=\"$(prefix)bin/opari2\"                                              >> $(builddir)/silc_config.dat
	echo OPARI_SCRIPT=\"$(prefix)bin/pomp2_parse_init_regions.awk\"                  >> $(builddir)/silc_config.dat
	$(INSTALL_DATA) $(builddir)/silc_config.dat $(DESTDIR)$(exec_prefix)/bin/silc_config.dat
# add compiler-suite, name of libs, static/shared, ...

clean-local-silc-config-tool:
	rm -f $(builddir)/silc_config.dat

silc-config-tool-local:
	echo COMPILER_INSTRUMENTATION_CPPFLAGS=\"$(COMPILER_INSTRUMENTATION_CPPFLAGS)\" >  $(builddir)/silc_config.dat
	echo OPENMP_CFLAGS=\"$(OPENMP_CFLAGS)\"                                    >> $(builddir)/silc_config.dat
	echo LIBDIR=\".libs\"                                                      >> $(builddir)/silc_config.dat
	echo LIBDIR=\"../vendor/otf2/build-backend/.libs\"                         >> $(builddir)/silc_config.dat
	echo LIBDIR=\"../vendor/otf2/vendor/utility/build-backend/.libs\"          >> $(builddir)/silc_config.dat
	echo LIBDIR=\"$(LIBS)\"                                                    >> $(builddir)/silc_config.dat
	echo LIBDIR=\"$(CUBE_LIB_PATH)\"                                           >> $(builddir)/silc_config.dat
	echo LIBDIR=\"$(TIMER_LIBDIR)\"                                            >> $(builddir)/silc_config.dat
	echo INCDIR=\"$(PUBLIC_INC_DIR)\"                                          >> $(builddir)/silc_config.dat
	echo INCDIR=\"$(INC_DIR_OTF2)\"                                            >> $(builddir)/silc_config.dat
	echo SRC_ROOT=\"$(PUBLIC_INC_DIR)../../\"                                  >> $(builddir)/silc_config.dat
	echo CC=\"$(CC)\"                                                          >> $(builddir)/silc_config.dat
	echo LIBS=\"$(LIBBFD)\"                                                    >> $(builddir)/silc_config.dat
	echo LIBS=\"$(CUBE_LIBS)\"                                                 >> $(builddir)/silc_config.dat
	echo LIBS=\"$(TIMER_LIB)\"                                                 >> $(builddir)/silc_config.dat
	echo NM=\"$(NM)\"                                                          >> $(builddir)/silc_config.dat
	echo AWK=\"$(AWK)\"                                                        >> $(builddir)/silc_config.dat
	echo OPARI=\"../vendor/opari2/build-frontend/opari2\"                      >> $(builddir)/silc_config.dat
	echo OPARI_SCRIPT=\"$(PUBLIC_INC_DIR)../../vendor/opari2/src/pomp-lib-dummy/pomp2_parse_init_regions.awk\" >> $(builddir)/silc_config.dat
