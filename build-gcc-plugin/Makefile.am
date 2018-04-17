# -*- mode: makefile -*-

##
## This file is part of the Score-P software (http://www.score-p.org)
##
## Copyright (c) 2012-2015,
## Technische Universitaet Dresden, Germany
##
## Copyright (c) 2015,
## Forschungszentrum Juelich GmbH, Germany
##
## This software may be modified and distributed under the terms of
## a BSD-style license.  See the COPYING file in the package base
## directory for details.
##

## Options passed to aclocal to specify local macro directories. Although
## ACLOCAL_AMFLAGS should be replaced by AC_CONFIG_MACRO_DIRS, aclocal-1.13.4
## doesn't honor the paths provided to AC_CONFIG_MACRO_DIRS. See
## https://debbugs.gnu.org/cgi/bugreport.cgi?bug=21352
ACLOCAL_AMFLAGS = -I ../build-config/m4 -I ../vendor/common/build-config/m4
include ../build-includes/common.am

AM_CPPFLAGS += -DGCC_PLUGIN_BUILD

if HAVE_GCC_PLUGIN_SUPPORT

include ../src/utils/filter/Makefile.inc.am
include ../vendor/common/src/utils/cstr/Makefile.inc.am
include ../vendor/common/src/utils/io/Makefile.inc.am
include ../vendor/common/src/utils/exception/Makefile.inc.am
include ../vendor/common/src/utils/Makefile.inc.am

DISTCLEANFILES += $(builddir)/gcc_plugin_supported

endif HAVE_GCC_PLUGIN_SUPPORT

include ../src/adapters/compiler/gcc-plugin/Makefile.inc.am
