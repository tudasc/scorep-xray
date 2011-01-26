# scorep--backend

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

if BACKEND_TEST_RUNS
TESTS = $(SCOREP_SERIAL_TESTS) $(SCOREP_OMP_TESTS)
XFAIL_TESTS = $(SCOREP_SERIAL_XFAIL_TESTS) $(SCOREP_OMP_XFAIL_TESTS)
endif

endif

CONFIG_HEADER_BACKEND = ../src/scorep_config_tool_backend.h

if !BACKEND_TEST_RUNS
BUILT_SOURCES += scorep_serial_tests.sh scorep_omp_tests.sh

scorep_serial_tests.sh: 
	rm -f scorep_serial_tests.sh
	echo "#!/bin/bash" > scorep_serial_tests.sh
	for i in $(SCOREP_SERIAL_TESTS); do echo "./$$i" >> scorep_serial_tests.sh; done
	chmod +x scorep_serial_tests.sh

if OPENMP_SUPPORTED
scorep_omp_tests.sh: 
	rm -f scorep_omp_tests.sh
	echo "#!/bin/bash" > scorep_omp_tests.sh
	for i in $(SCOREP_OMP_TESTS); do echo "./$$i" >> scorep_omp_tests.sh; done
	chmod +x scorep_omp_tests.sh
else
scorep_omp_tests.sh:
endif
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

EXTRA_LOCAL_CLEAN_TARGETS = clean-local-scorep-experiment-dirs \
                            clean-local-scorep-config-tests    \
                            clean-local-scorep-config-tool

clean-local: $(EXTRA_LOCAL_CLEAN_TARGETS)
	rm -f scorep_serial_tests.sh  scorep_omp_tests.sh

.PHONY: $(EXTRA_LOCAL_CLEAN_TARGETS) \
        install-scorep-meets-tau

clean-local-scorep-experiment-dirs:
	rm -rf scorep-2*
	rm -rf scorep-measurement-tmp
	rm -f serial_inst_test

clean-local-scorep-config-tests:
	$(RM) test_scorep_config_number.*.out
	$(RM) test_scorep_config_size.*.out

install-data-local: scorep-config-tool-install

uninstall-local:
	rm -f $(DESTDIR)$(exec_prefix)/bin/scorep_config.dat $(CONFIG_HEADER_BACKEND) $(DESTDIR)$(datadir)/SCOREP_Pdt_Instrumentation.conf

scorep-config-tool-install:
	$(MKDIR_P) $(DESTDIR)$(datadir)
	$(INSTALL_DATA) $(INC_ROOT)src/tools/wrapper/SCOREP_Pdt_Instrumentation.conf $(DESTDIR)$(datadir)/SCOREP_Pdt_Instrumentation.conf

clean-local-scorep-config-tool:
	rm -f $(builddir)/scorep_config.dat

scorep-config-tool-local: $(builddir)/scorep_config.dat

$(builddir)/scorep_config.dat:
	@echo "Creating $(builddir)/scorep_config.dat"
	@echo COMPILER_INSTRUMENTATION_CPPFLAGS=\"$(COMPILER_INSTRUMENTATION_CPPFLAGS)\" >  $(builddir)/scorep_config.dat
	@echo OPENMP_CFLAGS=\"$(OPENMP_CFLAGS)\"                                    >> $(builddir)/scorep_config.dat
	@echo LIBDIR=\".libs\"                                                      >> $(builddir)/scorep_config.dat
	@echo LIBDIR=\"../vendor/otf2/build-backend/.libs\"                         >> $(builddir)/scorep_config.dat
	@echo LIBDIR=\"../vendor/otf2/vendor/utility/build-backend/.libs\"          >> $(builddir)/scorep_config.dat
	@echo LIBS=\"$(LIBS) -lotf2 -lscorep_utilities\"                            >> $(builddir)/scorep_config.dat
	@echo LIBDIR=\"$(CUBE_LIB_PATH)\"                                           >> $(builddir)/scorep_config.dat
	@echo LIBDIR=\"$(TIMER_LIBDIR)\"                                            >> $(builddir)/scorep_config.dat
	@echo INCDIR=\"$(INC_ROOT)/include\"                                        >> $(builddir)/scorep_config.dat
	@echo INCDIR=\"$(PUBLIC_INC_DIR)\"                                          >> $(builddir)/scorep_config.dat
	@echo INCDIR=\"$(INC_DIR_OTF2)\"                                            >> $(builddir)/scorep_config.dat
	@echo INCDIR=\"$(INC_DIR_UTIL)\"                                            >> $(builddir)/scorep_config.dat
	@echo SRC_ROOT=\"$(PUBLIC_INC_DIR)../../\"                                  >> $(builddir)/scorep_config.dat
	@echo CC=\"$(CC)\"                                                          >> $(builddir)/scorep_config.dat
	@echo LIBS=\"$(LIBBFD)\"                                                    >> $(builddir)/scorep_config.dat
	@echo LIBS=\"$(CUBE_LIBS)\"                                                 >> $(builddir)/scorep_config.dat
	@echo LIBS=\"$(TIMER_LIB)\"                                                 >> $(builddir)/scorep_config.dat
	@echo NM=\"$(NM)\"                                                          >> $(builddir)/scorep_config.dat
	@echo AWK=\"$(AWK)\"                                                        >> $(builddir)/scorep_config.dat
	@echo OPARI=\"../vendor/opari2/build-frontend/opari2\"                      >> $(builddir)/scorep_config.dat
	@echo OPARI_SCRIPT=\"$(INC_ROOT)vendor/opari2/src/pomp-lib-dummy/pomp2_parse_init_regions.awk\" >> $(builddir)/scorep_config.dat
	@echo PDT=\"$(PDT_PATH)\"                                                   >> $(builddir)/scorep_config.dat
	@echo PDT_CONFIG=\"$(INC_ROOT)src/tools/wrapper/SCOREP_Pdt_Instrumentation.conf\" >> $(builddir)/scorep_config.dat


## Here we write only the backend values.
create-config-tool-headers: $(CONFIG_HEADER_BACKEND)

$(CONFIG_HEADER_BACKEND):
	@echo "Creating $(CONFIG_HEADER_BACKEND)"
	@echo "#define SCOREP_CFLAGS \"$(COMPILER_INSTRUMENTATION_CPPFLAGS)\"" >  $(CONFIG_HEADER_BACKEND)
	@echo "#define SCOREP_OPENMP_CFLAGS \"$(OPENMP_CFLAGS)\"" >> $(CONFIG_HEADER_BACKEND)
	@echo "#define SCOREP_PREFIX  \"$(prefix)\""           >> $(CONFIG_HEADER_BACKEND)
	@echo "#define SCOREP_DATADIR \"$(datadir)\""          >> $(CONFIG_HEADER_BACKEND)
	@echo "#define SCOREP_LIBDIR  \"$(libdir)\""           >> $(CONFIG_HEADER_BACKEND)
	@echo "#define SCOREP_LIBS    \"$(LIBS) $(LIBBFD) $(CUBE_LIBS) $(TIMER_LIB)\"" >> $(CONFIG_HEADER_BACKEND)
	@echo "#define CUBE_LIBDIR    \"$(CUBE_LIB_PATH)\""    >> $(CONFIG_HEADER_BACKEND)
	@echo "#define TIMER_LIBDIR   \"$(TIMER_LIBDIR)\""     >> $(CONFIG_HEADER_BACKEND)
	@echo "#define SCOREP_CC      \"$(CC)\""               >> $(CONFIG_HEADER_BACKEND)
	@echo "#define SCOREP_CXX     \"$(CXX)\""              >> $(CONFIG_HEADER_BACKEND)
	@echo "#define SCOREP_FC      \"$(FC)\""               >> $(CONFIG_HEADER_BACKEND)
	@echo "#define NM             \"$(NM)\""               >> $(CONFIG_HEADER_BACKEND)
	@echo "#define AWK            \"$(AWK)\""              >> $(CONFIG_HEADER_BACKEND)
	@echo "#define OPARI          \"$(prefix)/bin/opari2\"">> $(CONFIG_HEADER_BACKEND)
	@echo "#define OPARI_SCRIPT   \"$(prefix)/bin/pomp2_parse_init_regions.awk\"" >> $(CONFIG_HEADER_BACKEND)
	@echo "#define PDT            \"$(PDT_PATH)\""         >> $(CONFIG_HEADER_BACKEND)
	@echo "#define PDT_CONFIG     \"$(datadir)/SCOREP_Pdt_Instrumentation.conf\"" >> $(CONFIG_HEADER_BACKEND)
	@echo "#define RPATH_FLAG     \"$(am_libscorep_serial_la_rpath)\"">> $(CONFIG_HEADER_BACKEND)
