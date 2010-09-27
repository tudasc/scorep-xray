# scorep--frontend

include ../build-includes/common.am

AM_CPPFLAGS += -DFRONTEND_BUILD -DCROSS_BUILD






include ../build-includes/frontend-only.am
include ../build-includes/front-and-backend.am



TESTS = $(SCOREP_SERIAL_TESTS) $(SCOREP_OMP_TESTS)

## Convenience variable, used in common.am
BUILD_DIR=build-frontend

## ACLOCAL_AMFLAGS contains options to pass to aclocal when aclocal.m4 is to be
## rebuilt by make. This line is also used by autoreconf to run aclocal with
## suitable options, or by autopoint and gettextize to locate the place where
## Gettext's macros should be installed. So even if you do not really care about
## the rebuild rules, you should define ACLOCAL_AMFLAGS.
## Can't be moved to common.am
ACLOCAL_AMFLAGS = -I ../build-config/m4
