# -*- mode: makefile -*-

##
## This file is part of the Score-P software (http://www.score-p.org)
##
## Copyright (c) 2017,
## Technische Universitaet Dresden, Germany
##
## This software may be modified and distributed under the terms of
## a BSD-style license.  See the COPYING file in the package base
## directory for details.
##

## file       build-libwrap/Makefile.am

## Options passed to aclocal to specify local macro directories. Although
## ACLOCAL_AMFLAGS should be replaced by AC_CONFIG_MACRO_DIRS, aclocal-1.13.4
## doesn't honor the paths provided to AC_CONFIG_MACRO_DIRS. See
## https://debbugs.gnu.org/cgi/bugreport.cgi?bug=21352
ACLOCAL_AMFLAGS = -I ../build-config/m4 -I ../vendor/common/build-config/m4

include ../build-includes/common.am

AM_CPPFLAGS += -DLIBWRAP_BUILD

DISTCLEANFILES += $(builddir)/library_wrapping_supported

include ../src/tools/lib/Makefile.inc.am
include ../src/tools/library_wrapper_generator/Makefile.inc.am

include ../src/utils/filtering/Makefile.inc.am
include ../vendor/common/src/utils/cstr/Makefile.inc.am
include ../vendor/common/src/utils/io/Makefile.inc.am
include ../vendor/common/src/utils/exception/Makefile.inc.am
include ../vendor/common/src/utils/Makefile.inc.am
