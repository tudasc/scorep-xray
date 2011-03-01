# scorep-utilities--mpi

## AM_CPPFLAGS: The contents of this variable are passed to every compilation 
## that invokes the C preprocessor; it is a list of arguments to the 
## preprocessor. For instance, -I and -D options should be listed here.
##
## Automake already provides some -I options automatically, in a separate 
## variable that is also passed to every compilation that invokes the C 
## preprocessor. In particular it generates -I., -I$(srcdir), and a -I pointing 
## to the directory holding config.h (if you've used AC_CONFIG_HEADERS or 
## AM_CONFIG_HEADER). You can disable the default -I options using the 
## nostdinc option.
##
## AM_CPPFLAGS is ignored in preference to a per-executable (or per-library) 
## _CPPFLAGS variable if it is defined. 
##
## -I$(srcdir)/../../src: path to config.h
## -I$(srcdir)/../../vendor/include: external includes are found here, if any 
AM_CPPFLAGS = -I$(srcdir)/../../src 


## Convenience variable, use for referencing sources, e.g.
## foo_SOURCES = $(SRC_ROOT)src/foo/foo.c. For CPPFLAGS use SRC_ROOT_*
## See the note below
SRC_ROOT = ../../
PUBLIC_INC_SRC = $(SRC_ROOT)include/scorep/


## Convenience variables, use for referencing include directories, e.g.
## foo_CPPFLAGS = $(AM_CPPFLAGS) $(INC_ROOT)src/bar
## The otf2 and utility variables may be overriden when configuring using
## the --with-otf2/utility options (to be done)
## Note: we need different convenience variable because they are evaluated at
## different times. The SRC_ROOT variable is evaluated at autoreconf time where
## it is not recommended to use $(srcdir). On the other hand the INC_ROOT
## variables are evaluated during configure.
INC_ROOT = $(srcdir)/../../
PUBLIC_INC_DIR = $(INC_ROOT)include/scorep/
INC_DIR_OTF2 = $(srcdir)/../../vendor/otf2/include
INC_DIR_UTIL = $(srcdir)/../../vendor/otf2/vendor/utility/include

## The otf2 and utility variables may be overriden when configuring using
## the --with-otf2/utility options (to be done)
LIB_ROOT     = ../
LIB_DIR_OTF2 = ../../vendor/otf2/build-backend/
LIB_DIR_UTIL = ../../vendor/otf2/vendor/utility/build-backend/

## Vendor convenience variables.
INC_DIR_VENDOR_HASH = $(srcdir)/../../vendor/hash

## Add entities using += to the following primaries in locale Makefile.inc.ams
bin_PROGRAMS =
noinst_PROGRAMS =
lib_LTLIBRARIES =
noinst_LTLIBRARIES = 
include_HEADERS = 
# TESTS_ENVIRONMENT = 
SCOREP_MPI_TESTS =
SCOREP_MPI_OMP_TESTS =
SCOREP_MPI_XFAIL_TESTS = 
SCOREP_MPI_OMP_XFAIL_TESTS =
check_PROGRAMS = 
# check_LIBRARIES =
# check_LTLIBRARIES =
BUILT_SOURCES = 
CLEANFILES =
EXTRA_DIST = 

# Automatically update the libtool script if it becomes out-of-date.
libtool: $(LIBTOOL_DEPS)
	$(SHELL) ./config.status --recheck

doxygen-dev:
clean-doxygen-dev:

#CC   = $(MPICC)
#CXX  = $(MPICXX)
#F77  = $(MPIF77)
#FC   = $(MPIFC)

# I'm not sure if this is the way to go or if we need per 
# target _LDADD = $(MPILIBS)
LIBS += $(MPILIBS)

AM_CPPFLAGS += -DBACKEND_BUILD_MPI
if CROSS_BUILD
AM_CPPFLAGS += -DCROSS_BUILD
else
AM_CPPFLAGS += -DNOCROSS_BUILD
# no automatic test runs for MPI or hybrid programs
#if BACKEND_TEST_RUNS
#TESTS = $(SCOREP_MPI_TESTS) $(SCOREP_MPI_OMP_TESTS)
#XFAIL_TESTS = $(SCOREP_MPI_XFAIL_TESTS) $(SCOREP_MPI_OMP_XFAIL_TESTS)
#endif
endif


# build always scripts for MPI or hybrid programs
#if !BACKEND_TEST_RUNS
BUILT_SOURCES += scorep_mpi_tests.sh scorep_mpi_omp_tests.sh

if MPI_SUPPORTED
scorep_mpi_tests.sh:
	rm -f scorep_mpi_tests.sh
	echo "#!/bin/bash" > scorep_mpi_tests.sh
	for i in $(SCOREP_MPI_TESTS); do echo "./$$i" >> scorep_mpi_tests.sh; done
	chmod +x scorep_mpi_tests.sh

if OPENMP_SUPPORTED
scorep_mpi_omp_tests.sh:
	rm -f scorep_mpi_omp_tests.sh
	echo "#!/bin/bash" > scorep_mpi_omp_tests.sh
	for i in $(SCOREP_MPI_OMP_TESTS); do echo "./$$i" >> scorep_mpi_omp_tests.sh; done
	chmod +x scorep_mpi_omp_tests.sh
else
scorep_mpi_omp_tests.sh:
endif
else
scorep_mpi_tests.sh:
scorep_mpi_omp_tests.sh:
endif
#endif


bindir=${exec_prefix}@backend_bin_dir@
libdir=${exec_prefix}@backend_lib_dir@

include ../../build-includes/mpi-backend-only.am

## ACLOCAL_AMFLAGS contains options to pass to aclocal when aclocal.m4 is to be
## rebuilt by make. This line is also used by autoreconf to run aclocal with
## suitable options, or by autopoint and gettextize to locate the place where
## Gettext's macros should be installed. So even if you do not really care about
## the rebuild rules, you should define ACLOCAL_AMFLAGS.
## Can't be moved to common.am
ACLOCAL_AMFLAGS = -I ../../vendor/common/build-config/m4

clean-local:
	rm -f scorep_mpi_omp_tests.sh  scorep_mpi_tests.sh

distclean-local:
	rm -f mpi_supported


check-build: $(BUILT_SOURCES) all $(check_PROGRAMS)
	for dir in $(SUBDIRS); do                        \
            (cd $$dir && $(MAKE) $(AM_MAKEFLAGS) $@) || exit 1; \
        done

check-run: check-build check-TESTS
	for dir in $(SUBDIRS); do                        \
            (cd $$dir && $(MAKE) $(AM_MAKEFLAGS) $@) || exit 1; \
        done

beautify:
