# silc-utilities--mpi

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
PUBLIC_INC_SRC = $(SRC_ROOT)include/silc/


## Convenience variables, use for referencing include directories, e.g.
## foo_CPPFLAGS = $(AM_CPPFLAGS) $(INC_ROOT)src/bar
## The otf2 and utility variables may be overriden when configuring using
## the --with-otf2/utility options (to be done)
## Note: we need different convenience variable because they are evaluated at
## different times. The SRC_ROOT variable is evaluated at autoreconf time where
## it is not recommended to use $(srcdir). On the other hand the INC_ROOT
## variables are evaluated during configure.
INC_ROOT = $(srcdir)/../../
PUBLIC_INC_DIR = $(INC_ROOT)include/silc/
# temporary: wait until otf2 has a dedicated public header directory
INC_DIR_OTF2 = $(srcdir)/../../vendor/otf2/include/otf2
INC_DIR_UTIL = $(srcdir)/../../vendor/otf2/vendor/utility/include/silc_utility

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
TESTS = 
XFAIL_TESTS = 
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

CC   = $(MPICC)
CXX  = $(MPICXX)
F77  = $(MPIF77)
FC   = $(MPIFC)

# I'm not sure if this is the way to go or if we need per 
# target _LDADD = $(MPILIBS)
LIBS += $(MPILIBS)

AM_CPPFLAGS += -DBACKEND_BUILD
if CROSS_BUILD
AM_CPPFLAGS += -DCROSS_BUILD
else
AM_CPPFLAGS += -DNOCROSS_BUILD
endif

bindir=${exec_prefix}@backend_bin_dir@
libdir=${exec_prefix}@backend_lib_dir@

include ../../build-includes/mpi-backend-only.am

## ACLOCAL_AMFLAGS contains options to pass to aclocal when aclocal.m4 is to be
## rebuilt by make. This line is also used by autoreconf to run aclocal with
## suitable options, or by autopoint and gettextize to locate the place where
## Gettext's macros should be installed. So even if you do not really care about
## the rebuild rules, you should define ACLOCAL_AMFLAGS.
## Can't be moved to common.am
ACLOCAL_AMFLAGS = -I ../../build-config/m4
