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

bindir=${exec_prefix}@backend_bin_dir@
libdir=${exec_prefix}@backend_lib_dir@

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


## Convenience variable, used in common.am
BUILD_DIR=build-backend

## Here all libscorep_*.la are build
LIB_DIR_SCOREP = ./

## Here the MPI convenience libraries are build
LIB_DIR_SCOREP_MPI = ../build-mpi/

clean-local: clean-local-scorep-experiment-dirs clean-local-scorep-config-tests
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

SCOREP_SHARE_INSTALL_DIR = $(DESTDIR)$(datadir)/@PACKAGE_NAME@
SCOREP_SHARE_LOCAL_DIR=$(PWD)/../share
SCOREP_SHARE_GLOBAL_DIR=$(abs_top_srcdir)/../share
SCOREP_PDT_CONFIG_INSTALL_TARGET = $(SCOREP_SHARE_INSTALL_DIR)/SCOREP_Pdt_Instrumentation.conf
SCOREP_COBI_CONFIG_IN=$(SCOREP_SHARE_GLOBAL_DIR)/SCOREP_Cobi_Adapter.xml.in

uninstall-local: $(UNINSTALL_LOCAL_TARGETS)
	rm -f $(DESTDIR)$(exec_prefix)/bin/scorep_config.dat \
	     $(CONFIG_HEADER_BACKEND) \
	     $(SCOREP_PDT_CONFIG_INSTALL_TARGET) 
	if [ -e $(SCOREP_SHARE_LOCAL_DIR)/SCOREP_Cobi_Adapter.xml.in ]; then \
	  rm -f  $(SCOREP_SHARE_LOCAL_DIR)/SCOREP_Cobi_AdapterSerial.xml \
	         $(SCOREP_SHARE_LOCAL_DIR)/SCOREP_Cobi_AdapterOmp.xml \
	         $(SCOREP_SHARE_LOCAL_DIR)/SCOREP_Cobi_AdapterMpi.xml \
	         $(SCOREP_SHARE_LOCAL_DIR)/SCOREP_Cobi_AdapterMpiOmp.xml; \
	else \
	  rm -rf $(SCOREP_SHARE_LOCAL_DIR); \
	fi

INSTALL_DATA_LOCAL_TARGETS += scorep-config-tool-install 

scorep-config-tool-install:
	$(MKDIR_P) $(SCOREP_SHARE_INSTALL_DIR)
	$(INSTALL_DATA) $(INC_ROOT)share/SCOREP_Pdt_Instrumentation.conf $(SCOREP_PDT_CONFIG_INSTALL_TARGET)
	$(INSTALL_DATA) $(INC_ROOT)share/SCOREP_Cobi_Filter.xml          $(SCOREP_SHARE_INSTALL_DIR)/SCOREP_Cobi_Filter.xml
	sed 's!libscorep_mpi.so!$(libdir)/libscorep_serial.so!g'  $(SCOREP_COBI_CONFIG_IN) > $(SCOREP_SHARE_INSTALL_DIR)/SCOREP_Cobi_AdapterSerial.xml
	sed 's!libscorep_mpi.so!$(libdir)/libscorep_omp.so!g'     $(SCOREP_COBI_CONFIG_IN) > $(SCOREP_SHARE_INSTALL_DIR)/SCOREP_Cobi_AdapterOmp.xml
	sed 's!libscorep_mpi.so!$(libdir)/libscorep_mpi.so!g'     $(SCOREP_COBI_CONFIG_IN) > $(SCOREP_SHARE_INSTALL_DIR)/SCOREP_Cobi_AdapterMpi.xml
	sed 's!libscorep_mpi.so!$(libdir)/libscorep_mpi_omp.so!g' $(SCOREP_COBI_CONFIG_IN) > $(SCOREP_SHARE_INSTALL_DIR)/SCOREP_Cobi_AdapterMpiOmp.xml

clean-local-scorep-config-tool:
	rm -f $(builddir)/scorep_config.dat
	@$(am__cd) ../vendor/otf2/build-backend && $(MAKE) $(AM_MAKEFLAGS) clean-local-otf2-config-tool

scorep-config-tool-local: $(builddir)/scorep_config.dat
	@$(am__cd) ../vendor/otf2/build-backend && $(MAKE) $(AM_MAKEFLAGS) otf2-config-tool-local

create-cobi-config-local:
	@$(MKDIR_P) $(SCOREP_SHARE_LOCAL_DIR)
	@sed 's!libscorep_mpi.so!$(PWD)/.libs/libscorep_serial.so!g'  $(SCOREP_COBI_CONFIG_IN) > $(SCOREP_SHARE_LOCAL_DIR)/SCOREP_Cobi_AdapterSerial.xml
	@sed 's!libscorep_mpi.so!$(PWD)/.libs/libscorep_omp.so!g'     $(SCOREP_COBI_CONFIG_IN) > $(SCOREP_SHARE_LOCAL_DIR)/SCOREP_Cobi_AdapterOmp.xml
	@sed 's!libscorep_mpi.so!$(PWD)/.libs/libscorep_mpi.so!g'     $(SCOREP_COBI_CONFIG_IN) > $(SCOREP_SHARE_LOCAL_DIR)/SCOREP_Cobi_AdapterMpi.xml
	@sed 's!libscorep_mpi.so!$(PWD)/.libs/libscorep_mpi_omp.so!g' $(SCOREP_COBI_CONFIG_IN) > $(SCOREP_SHARE_LOCAL_DIR)/SCOREP_Cobi_AdapterMpiOmp.xml
	@if [ ! -e $(SCOREP_SHARE_LOCAL_DIR)/SCOREP_Cobi_Filter.xml ]; then \
	 cp $(SCOREP_SHARE_GLOBAL_DIR)/SCOREP_Cobi_Filter.xml $(SCOREP_SHARE_LOCAL_DIR)/; \
	fi

# determine build dir for config tool
if CROSS_BUILD
config_tool_build_dir = build-frontend
else
config_tool_build_dir = build-backend
endif

$(builddir)/scorep_config.dat: 
	$(AM_V_GEN)( \
	    echo COMPILER_INSTRUMENTATION_CPPFLAGS=\"$(COMPILER_INSTRUMENTATION_CPPFLAGS)\"; \
	    echo OPENMP_CFLAGS=\"$(OPENMP_CFLAGS)\"; \
	    echo LIBDIR=\"$(PWD)/.libs\"; \
	    echo LIBS=\"$(LIBS)\"; \
	    echo LIBDIR=\"$(CUBE_LIB_PATH)\"; \
	    echo LIBDIR=\"$(TIMER_LIBDIR)\"; \
	    echo LIBDIR=\"$(SCOREP_PAPI_LIBDIR)\"; \
	    echo INCDIR=\"$(PWD)/$(PUBLIC_INC_DIR)\"; \
	    echo INCDIR=\"$(PWD)/$(PUBLIC_INC_DIR)/scorep\"; \
	    echo INCDIR=\"$(PWD)/$(INC_DIR_OPARI2)\"; \
	    echo SRC_ROOT=\"$(abs_top_srcdir)/..\"; \
	    echo CC=\"$(CC)\"; \
	    echo CXX=\"$(CXX)\"; \
	    echo LIBS=\"$(LIBBFD)\"; \
	    echo LIBS=\"$(CUBE_LIBS)\"; \
	    echo LIBS=\"$(TIMER_LIB)\"; \
	    echo LIBS=\"$(SCOREP_PAPI_LIBS)\"; \
	    echo OPARI=\"$(PWD)/../vendor/opari2/build-frontend/opari2\"; \
	    echo OPARI_CONFIG=\"$(PWD)/../vendor/opari2/build-frontend/opari2-config --config=$(PWD)/../vendor/opari2/build-frontend/opari2_config.dat\"; \
	    echo PDT=\"$(PDT_PATH)\"; \
	    echo PDT_CONFIG=\"$(SCOREP_SHARE_GLOBAL_DIR)/SCOREP_Pdt_Instrumentation.conf\"; \
	    echo OTF2_CONFIG=\"$(PWD)/../vendor/otf2/$(config_tool_build_dir)/otf2-config --config=$(PWD)/../vendor/otf2/build-backend/otf2_config.dat\"; \
	    echo SCOREP_CONFIG=\"$(PWD)/scorep-config --config=$(PWD)/scorep_config.dat\"; \
	    echo OTF2_PRINT=\"$(PWD)/../vendor/otf2/$(config_tool_build_dir)/otf2_print\"; \
	    echo COBI_CONFIG_DIR=\"$(SCOREP_SHARE_LOCAL_DIR)\"; \
	) >$@

## Provide the scorep-config build with information from this backend Makefile.
$(CONFIG_HEADER_BACKEND):
	$(AM_V_GEN)( \
	    echo "#define SCOREP_CFLAGS \"$(COMPILER_INSTRUMENTATION_CPPFLAGS)\""; \
	    echo "#define SCOREP_OPENMP_CFLAGS \"$(OPENMP_CFLAGS)\""; \
	    echo "#define SCOREP_PREFIX  \"$(prefix)\""; \
	    echo "#define SCOREP_DATADIR \"$(datadir)\""; \
	    echo "#define SCOREP_LIBDIR  \"$(libdir)\""; \
	    echo "#define SCOREP_LIBS    \"$(LIBS) $(LIBBFD) $(CUBE_LIBS) $(TIMER_LIB)\""; \
	    echo "#define CUBE_LIBDIR    \"$(CUBE_LIB_PATH)\""; \
	    echo "#define TIMER_LIBDIR   \"$(TIMER_LIBDIR)\""; \
	    echo "#define PAPI_LIBS      \"$(SCOREP_PAPI_LIBS)\""; \
	    echo "#define PAPI_LDFLAGS   \"$(SCOREP_PAPI_LDFLAGS)\""; \
	    echo "#define PAPI_LIBDIR    \"$(SCOREP_PAPI_LIBDIR)\""; \
	    echo "#define SCOREP_CC      \"$(CC)\""; \
	    echo "#define SCOREP_CXX     \"$(CXX)\""; \
	    echo "#define SCOREP_FC      \"$(FC)\""; \
	    echo "#define OPARI          \"$(prefix)/bin/opari2\""; \
	    echo "#define OPARI_CONFIG   \"$(prefix)/bin/opari2-config\""; \
	    echo "#define PDT            \"$(PDT_PATH)\""; \
	    echo "#define PDT_CONFIG     \"$(SCOREP_SHARE_INSTALL_DIR)/SCOREP_Pdt_Instrumentation.conf\""; \
	    echo "#define RPATH_FLAG     \"$(am_libscorep_serial_la_rpath)\""; \
	    echo "#define COBI_CONFIG_DIR \"$(SCOREP_SHARE_INSTALL_DIR)\""; \
	    echo "#define SCOREP_BACKEND_SION_LIBS     \"$(SCOREP_SION_SERIAL_LIBS)\""; \
	    echo "#define SCOREP_BACKEND_SION_LDFLAGS  \"$(SCOREP_SION_SERIAL_LDFLAGS)\""; \
	    echo "#define SCOREP_BACKEND_SION_CPPFLAGS \"$(SCOREP_SION_SERIAL_CPPFLAGS)\""; \
	) >$@


if OPENMP_SUPPORTED
installcheck-local:
	@$(am__cd) ../test/jacobi/serial/C && $(MAKE) $(AM_MAKEFLAGS)
	@$(am__cd) ../test/jacobi/OpenMP/C && $(MAKE) $(AM_MAKEFLAGS)
else
installcheck-local:
	@$(am__cd) ../test/jacobi/serial/C && $(MAKE) $(AM_MAKEFLAGS)
endif
