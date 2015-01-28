# -*- mode: makefile -*-

##
## This file is part of the Score-P software (http://www.score-p.org)
##
## Copyright (c) 2012-2015,
## Technische Universitaet Dresden, Germany
##
## This software may be modified and distributed under the terms of
## a BSD-style license.  See the COPYING file in the package base
## directory for details.
##

ACLOCAL_AMFLAGS = -I ../vendor/common/build-config/m4 -I ../build-config/m4
include ../build-includes/common.am

AM_CPPFLAGS += -DGCC_PLUGIN_BUILD

if HAVE_GCC_PLUGIN_SUPPORT

include ../src/utils/filtering/Makefile.inc.am
include ../vendor/common/src/utils/cstr/Makefile.inc.am
include ../vendor/common/src/utils/io/Makefile.inc.am
include ../vendor/common/src/utils/exception/Makefile.inc.am
include ../vendor/common/src/utils/Makefile.inc.am

DISTCLEANFILES += $(builddir)/gcc_plugin_supported

endif HAVE_GCC_PLUGIN_SUPPORT

include ../src/adapters/compiler/gcc-plugin/Makefile.inc.am
