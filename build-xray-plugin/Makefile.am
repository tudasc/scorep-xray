# -*- mode: makefile -*-

# TODO: copied from llvm-plugin/Makefile.am

## Options passed to aclocal to specify local macro directories. Although
## ACLOCAL_AMFLAGS should be replaced by AC_CONFIG_MACRO_DIRS, aclocal-1.13.4
## doesn't honor the paths provided to AC_CONFIG_MACRO_DIRS. See
## https://debbugs.gnu.org/cgi/bugreport.cgi?bug=21352
ACLOCAL_AMFLAGS = -I ../build-config/m4 -I ../build-config/common/m4
include ../build-includes/common.am

AM_CPPFLAGS += -DXRAY_PLUGIN_BUILD

if HAVE_XRAY_PLUGIN_SUPPORT

include ../src/utils/filter/Makefile.inc.am
include ../common/utils/Makefile.inc.am

DISTCLEANFILES += \
    $(builddir)/xray_plugin_supported_c \
    $(builddir)/xray_plugin_supported_cxx \
    $(builddir)/xray_plugin_supported_fc

endif HAVE_XRAY_PLUGIN_SUPPORT

include ../src/adapters/compiler/xray-plugin/Makefile.inc.am
